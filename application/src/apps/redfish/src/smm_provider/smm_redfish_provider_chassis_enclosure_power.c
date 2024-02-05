
#include "redfish_provider.h"


#define PEM_TYPE_AC 0
#define PEM_TYPE_DC 1
#define PEM_TYPE_AC_AND_DC 2

#define METHOD_RETURN_CODE_OK 0
#define METHOD_RETURN_CODE_SLOT_INVALID 1
#define PEM_COMMUNICATION_LOST 2
#define PEM_NOT_PRESENT 3

#define SOFTWARE_VER_MAX_LEN 64
#define HARDWARE_VER_MAX_LEN 8
#define POWERSUPPLY_NAME_MAX_LEN 8
#define POWERSUPPLY_REDUNDANCY_ID_MAX_LEN 8

#define PEM_GET_SOFT_VER_FAIL 5      
#define PEM_GET_HARD_VER_FAIL 6      
#define PEM_GET_SOFT_HARD_VER_FAIL 7 


#define POWER_SET_SLEEPCONFIG_UNKNOW_ERR (-1)   
#define POWER_SET_SLEEPCONFIG_MANUAL_CLOSE (-2) 
#define POEWR_SET_SLEEPNFIG_CONFIG_ERR 1        

LOCAL gint32 get_chassis_enclosure_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_enclosure_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_enclosure_odata_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_enclosure_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_enclosure_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_enclosure_powercontrol(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_chassis_enclosure_powercontrol(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_chassis_enclosure_power_capping_value(PROVIDER_PARAS_S *i_paras, json_object *limit_value_jso,
    json_object **o_message_jso);
LOCAL gint32 set_powercontrol_power_limit_mode(OBJ_HANDLE handle, PROVIDER_PARAS_S *i_paras, json_object *huawei_jso,
    json_object **o_message_jso);
LOCAL gint32 set_powercontrol_power_limit_enable(OBJ_HANDLE handle, PROVIDER_PARAS_S *i_paras, json_object *huawei_jso,
    json_object **o_message_jso);
LOCAL gint32 set_powercontrol_power_limit_threshold(OBJ_HANDLE handle, PROVIDER_PARAS_S *i_paras,
    json_object *huawei_jso, json_object **o_message_jso);
LOCAL gint32 set_powercontrol_power_limit_blade_info(OBJ_HANDLE handle, PROVIDER_PARAS_S *i_paras,
    json_object *huawei_jso, json_object **o_message_jso);
LOCAL gint32 set_chassis_enclosure_powercontrol_oem(PROVIDER_PARAS_S *i_paras, json_object *huawei_jso,
    json_object **o_message_jso);
LOCAL gint32 get_chassis_enclosure_powersupplies(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_enclosure_redundancy(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_redundancy_attribute(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 add_power_limit(json_object *power_control);
LOCAL gint32 add_power_consumed(json_object *power_control);
LOCAL gint32 add_power_capacity(json_object *power_control);
LOCAL gint32 add_power_control_oem(json_object *power_control);
LOCAL gint32 get_minimum_power_capping_value(guint16 *demand_value, guint16 *extra_value,
    guint16 *setted_manually_value);

extern gint32 act_power_reset_historydata(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
extern gint32 act_power_collect_history_data(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_chassis_enclosure_power_provider[] = {
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_enclosure_odata_context, NULL, NULL, ETAG_FLAG_DISABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_enclosure_odata_id, NULL, NULL, ETAG_FLAG_DISABLE},
    {RFPROP_ODATA_TYPE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_enclosure_odata_type, NULL, NULL, ETAG_FLAG_DISABLE},
    {RFPROP_COMMON_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_enclosure_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_NAME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_enclosure_name, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_POWER_CONTROL, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_chassis_enclosure_powercontrol, set_chassis_enclosure_powercontrol, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_ENCLOSURE_POWERSUPPLIES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_enclosure_powersupplies, NULL, NULL, ETAG_FLAG_DISABLE},
    {RFPROP_CHASSIS_ENCLOSURE_REDUNDANCY, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_chassis_enclosure_redundancy, set_redundancy_attribute, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_POWER_OEM_ACTION_RESET_HISTORY_DATA, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, act_power_reset_historydata, ETAG_FLAG_ENABLE},
    {RFPROP_POWER_OEM_ACTION_COLLECT_HISTORY_DATA, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_power_collect_history_data, ETAG_FLAG_ENABLE},
};


LOCAL gint32 get_chassis_enclosure_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_string("/redfish/v1/$metadata#Chassis/Members/Enclosure/Power/$entity");
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "new object failed."));

    return HTTP_OK;
}


LOCAL gint32 get_chassis_enclosure_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    *o_result_jso = json_object_new_string("/redfish/v1/Chassis/Enclosure/Power");
    return HTTP_OK;
}


LOCAL gint32 get_chassis_enclosure_odata_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    *o_result_jso = json_object_new_string("#Power.v1_1_0.Power");
    return HTTP_OK;
}


LOCAL gint32 get_chassis_enclosure_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_string(RF_CHASSIS_POWER);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "new object failed."));

    return HTTP_OK;
}


LOCAL gint32 get_chassis_enclosure_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    *o_result_jso = json_object_new_string(RF_CHASSIS_POWER);
    return HTTP_OK;
}


LOCAL void convert_fault_reason_str_to_json(json_object *fault_reason_jso, gchar *fault_reason,
    guint32 fault_reason_len)
{
    gchar *head_ptr = NULL;
    gchar *cur_ptr = NULL;
    if (!fault_reason_jso || !fault_reason) {
        return;
    }
    if (fault_reason_len == 0 || strlen(fault_reason) == 0) {
        return;
    }
    head_ptr = fault_reason;
    cur_ptr = fault_reason;
    while (*cur_ptr != '\0') {
        
        if (*cur_ptr == '\r') {
            *cur_ptr = '\0'; 
            json_object_array_add(fault_reason_jso, json_object_new_string(head_ptr));
            cur_ptr += 2; 
            head_ptr = cur_ptr;
            continue;
        }
        cur_ptr++;
    }
}


LOCAL guint32 get_one_power_present(gint8 pem_slot)
{
    guint8         present = 0;
    OBJ_HANDLE     obj_handle = 0;
    gint32         ret;

    
    ret = dal_get_specific_object_byte(CLASS_POWER_MGNT, PROPERTY_POWER_MGNT_SLOT, pem_slot, &obj_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "get pem(%d) powermgnt object handle failed. ret = %d", pem_slot, ret);
        return FALSE;
    }

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_POWER_MGNT_PRESENCE, &present);
    debug_log(DLOG_DEBUG, "get pem(%d) powermgnt present status = %d", pem_slot, present);
    return present;
}

LOCAL gint32 add_powersupplies_status(json_object* powersupply_unit_jso, gint8 pem_slot)
{
    OBJ_HANDLE          onepower_handle = 0;
    guint8              health = 0;
    gint32              ret;
    json_object*        status_jso = NULL;
    gchar *state = NULL;

    
    if (get_one_power_present(pem_slot) == TRUE) {
        state = ENABLED_STRING;
        
        ret = dal_get_specific_object_byte(CLASS_ONEPOWER, PROTERY_PS_INDEX, pem_slot, &onepower_handle);
        return_val_do_info_if_fail(VOS_OK == ret,
            HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "get pem(%d) onepower object handle failed. ret=%d", pem_slot - 1, ret));
        (void)dal_get_property_value_byte(onepower_handle, PROTERY_PS_STATUS, (guchar *)&health);
    } else {
        state = ABSENT_STRING;
        health = 0xff;
    }
    get_resource_status_property(&health, NULL, state, &status_jso, TRUE);
    json_object_object_add(powersupply_unit_jso, RFPROP_CHASSIS_STATUS, status_jso);
    return VOS_OK;
}


LOCAL gint32 add_pem_rate_power(json_object* powersupply_unit_jso, gint8 pem_slot)
{
    OBJ_HANDLE          obj_handle = 0;
    guint16             rate_power = 0;
    guint8              comm_status = 0;
    gint32              ret;

    
    ret = dal_get_specific_object_byte(CLASS_NAME_PS, PROTERY_PS_INDEX, pem_slot, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "Get pem(%d) onepower object handle failed. ret=%d", pem_slot, ret));

    
    ret = dal_get_property_value_byte(obj_handle, PROTERY_PS_COMMU_STATE, (guchar*)&comm_status);
    return_val_do_info_if_fail(VOS_OK == ret,  HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "Get pem(%d) communication status failed. ret=%d", pem_slot, ret));

    
    return_val_do_info_if_expr(1 == comm_status, VOS_OK,
        json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_CAPACITY, NULL);
        debug_log(DLOG_ERROR, "Pem(%d) communication lost. ret=%d", pem_slot, ret));

    
    ret = dal_get_property_value_uint16(obj_handle, PROTERY_PS_RATING, &rate_power);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_OK,
        json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_CAPACITY, NULL);
        debug_log(DLOG_ERROR, "Get pem(%d) rate power failed. ret=%d", pem_slot, ret));
    json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_CAPACITY,
        json_object_new_int((gint32)rate_power));

    return VOS_OK;
}


LOCAL gint32 add_pem_real_power(json_object* powersupply_unit_jso, gint8 pem_slot)
{
    OBJ_HANDLE          obj_handle = 0;
    double              real_power = 0;
    guint8              comm_status = 0;
    gint32              ret;

    
    ret = dal_get_specific_object_byte(CLASS_NAME_PS, PROTERY_PS_INDEX, pem_slot, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "Get pem(%d) onepower object handle failed. ret=%d", pem_slot, ret));

    
    ret = dal_get_property_value_byte(obj_handle, PROTERY_PS_COMMU_STATE, (guchar*)&comm_status);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "Get pem(%d) communication status failed. ret=%d", pem_slot, ret));

    
    return_val_do_info_if_expr(1 == comm_status, VOS_OK,
        json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_LASTPOWEROUTPUT, NULL);
        debug_log(DLOG_ERROR, "Pem(%d) communication lost. ret=%d", pem_slot, ret));

    
    ret = dal_get_property_value_double(obj_handle, PROTERY_PS_INPUTPOWER, (double*)&real_power);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_OK,
        json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_LASTPOWEROUTPUT, NULL);
        debug_log(DLOG_ERROR, "%s, %d: get pem(%d) real power failed. ret=%d", __FUNCTION__, __LINE__, pem_slot, ret));
    json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_LASTPOWEROUTPUT,
        json_object_new_int((gint32)real_power));

    return VOS_OK;
}


LOCAL gint32 add_pem_work_mode(json_object* powersupply_unit_jso, gint32 pem_slot)
{
    OBJ_HANDLE          obj_handle = 0;
    guint8              pem_mode = 0;
    guint8              comm_status = 0;
    gint32              ret;
    guchar              protocol;

    
    ret = dal_get_specific_object_byte(CLASS_NAME_PS, PROTERY_PS_INDEX, pem_slot, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "Get pem(%d) onepower object handle failed. ret=%d", pem_slot, ret));

    
    ret = dal_get_property_value_byte(obj_handle, PROTERY_PS_COMMU_STATE, (guchar*)&comm_status);
    return_val_do_info_if_fail(VOS_OK == ret,  HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get pem(%d) communication status failed. ret=%d", pem_slot, ret));

    
    return_val_do_info_if_expr(1 == comm_status, VOS_OK, json_object_object_add(powersupply_unit_jso,
        RFPROP_POWER_SUPPLIES_TYPE, json_object_new_string(STRING_VALUE_IS_UNKNOWN));
        debug_log(DLOG_ERROR, "Pem(%d) communication lost. ret=%d", pem_slot, ret));

    
    ret = dal_get_property_value_byte(obj_handle, PROTERY_PS_INPUTMODE, (guchar*)&pem_mode);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_OK, json_object_object_add(powersupply_unit_jso,
        RFPROP_POWER_SUPPLIES_TYPE, json_object_new_string(STRING_VALUE_IS_UNKNOWN));
        debug_log(DLOG_ERROR, "Get pem(%d) work mode failed. ret=%d", pem_slot, ret));

    ret = dal_get_property_value_byte(obj_handle, PROTERY_PS_PROTOCAL, &protocol);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s]: Get protocol failed: %d.\n", __FUNCTION__, ret);
        return RET_ERR;
    }
    
    if (protocol == PROTOCOL_TCE_PMBUS) {
        pem_mode = (pem_mode == PEM_TYPE_AC) ? PEM_TYPE_DC : ((pem_mode == PEM_TYPE_DC) ? PEM_TYPE_AC : pem_mode);
    }

    switch (pem_mode) {
        case PEM_TYPE_AC:
            json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_TYPE,
                json_object_new_string(RF_POWER_SUPPLIES_TYPE_VALUE_AC));
            break;

        case PEM_TYPE_DC:
            json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_TYPE,
                json_object_new_string(RF_POWER_SUPPLIES_TYPE_VALUE_DC));
            break;

        case PEM_TYPE_AC_AND_DC:
            json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_TYPE,
                json_object_new_string(RF_POWER_SUPPLIES_TYPE_VALUE_AC_OR_DC));
            break;

        default:
            json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_TYPE,
                json_object_new_string(STRING_VALUE_IS_UNKNOWN));
            debug_log(DLOG_ERROR, "%s, %d: unknown pem work mode(%d).", __FUNCTION__, __LINE__, pem_mode);
            break;
    }

    return VOS_OK;
}


LOCAL gint32 get_pem_fru_object(guint32 pem_slot, OBJ_HANDLE *fru_handle)
{
    gint32              ret;
    GSList*             obj_list = NULL;
    GSList*             obj_note =  NULL;
    guchar              fru_type = 0;
    guint32             position = 0;
    guint8              slot = 0;
    OBJ_HANDLE          connector_handle = 0;

    ret = dfl_get_object_list(CLASS_FRU, &obj_list);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "get fru object list failed. ret=%d", ret);
        return VOS_ERR;
    }

    
    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_FRU_TYPE, &fru_type);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "get fru object list failed. ret=%d", ret);
            break;
        }

        if (fru_type != FRU_TYPE_POWER) {
            continue;
        }
        
        
        ret = dal_get_property_value_uint32((OBJ_HANDLE)obj_note->data, PROPERTY_FRU_POSITION, &position);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "get pem(%d) fru position failed. ret=%d", pem_slot, ret);
            break;
        }

        
        ret = dal_get_specific_object_uint32(CLASS_CONNECTOR_NAME, PROPERTY_CONNECTOR_POSITION,
            position, &connector_handle);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "get position%d ps connector obj failed. ret=%d", position, ret);
            break;
        }

        
        ret = dal_get_property_value_byte(connector_handle, PROPERTY_CONNECTOR_SLOT, &slot);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "get ps connector slot id failed. ret=%d", ret);
            break;
        }

        if (slot == (pem_slot + 1)) {
            *fru_handle = (OBJ_HANDLE)obj_note->data;
            break;
        }
    }

    g_slist_free(obj_list);
    return ret;
}


LOCAL gint32 get_pem_fru_info(json_object *powersupply_unit_jso, OBJ_HANDLE obj_handle)
{
    gint32 ret;
    gsize ref_obj_elabel_len = 0;
    gsize board_manu_len = 0;
    gsize product_part_number_len = 0;
    gsize product_serial_len = 0;
    const gchar *ref_obj_elabel = NULL;
    const gchar *product_manu = NULL;
    const gchar *product_part_number = NULL;
    const gchar *product_serial = NULL;
    OBJ_HANDLE ref_handle = 0;
    GSList *property_name_fru = NULL;
    GSList *property_value_fru = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    property_name_fru = g_slist_append(property_name_fru, PROPERTY_FRU_ELABEL_RO);

    ret = dfl_multiget_property_value(obj_handle, property_name_fru, &property_value_fru);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, g_slist_free(property_name_fru);
        uip_free_gvariant_list(property_value_fru);
        debug_log(DLOG_ERROR, "get multiple fru property value failed. ret=%d", ret));

    ref_obj_elabel = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_fru, 0), &ref_obj_elabel_len);

    g_slist_free(property_name_fru);

    return_val_do_info_if_fail(ref_obj_elabel && 0 != strlen(ref_obj_elabel), VOS_ERR,
        debug_log(DLOG_ERROR, "get fru elabel object failed.");
        uip_free_gvariant_list(property_value_fru));

    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_PRODUCT_MFG_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_PRODUCT_PN);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_PRODUCT_SN);

    ret = dfl_get_referenced_object(obj_handle, PROPERTY_FRU_ELABEL_RIMM_OBJ, &ref_handle);
    if (0 == ret) {
        ret = uip_multiget_object_property(CLASS_PERIPHERAL_ELABEL, dfl_get_object_name(ref_handle), property_name_list,
            &property_value);
    } else {
        ret = uip_multiget_object_property(CLASS_ELABEL, ref_obj_elabel, property_name_list, &property_value);
    }

    uip_free_gvariant_list(property_value_fru);

    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, g_slist_free(property_name_list);
        uip_free_gvariant_list(property_value); debug_log(DLOG_ERROR, "get multiget fru property failed.ret=%d", ret));

    product_manu = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 0), &board_manu_len);
    product_part_number =
        g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 1), &product_part_number_len);
    product_serial = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 2), &product_serial_len);

    
    if (board_manu_len != 0) {
        json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_MANUFACTURER,
            json_object_new_string(product_manu));
    } else {
        json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_MANUFACTURER, NULL);
        debug_log(DLOG_ERROR, "get manufacturer failed");
    }

    
    if (product_part_number_len != 0) {
        json_object_object_add(powersupply_unit_jso, RFPROP_PART_NUMBER, json_object_new_string(product_part_number));
    } else {
        json_object_object_add(powersupply_unit_jso, RFPROP_PART_NUMBER, NULL);
        debug_log(DLOG_ERROR, "get partnumber failed");
    }

    
    if (product_serial_len != 0) {
        json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_SERIAL_NUM,
            json_object_new_string(product_serial));
    } else {
        json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_SERIAL_NUM, NULL);
        debug_log(DLOG_ERROR, "get serialnumber failed");
    }

    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);

    return VOS_OK;
}


LOCAL gint32 add_pem_fru_data(json_object *powersupply_unit_jso, gint32 pem_slot)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guint8 comm_status = 0;

    
    ret = dal_get_specific_object_byte(CLASS_NAME_PS, PROTERY_PS_INDEX, pem_slot, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get pem(%d) onepower object handle failed. ret=%d", __FUNCTION__, __LINE__,
        pem_slot - 1, ret));

    
    ret = dal_get_property_value_byte(obj_handle, PROTERY_PS_COMMU_STATE, (guchar *)&comm_status);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get pem(%d) communication status failed. ret=%d", __FUNCTION__, __LINE__,
        pem_slot - 1, ret));

    
    return_val_do_info_if_expr(1 == comm_status, VOS_OK,
        json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_MANUFACTURER, NULL);
        json_object_object_add(powersupply_unit_jso, RFPROP_PART_NUMBER, NULL);
        json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_SERIAL_NUM, NULL);
        debug_log(DLOG_ERROR, "%s, %d: pem(%d) communication lost. ret=%d", __FUNCTION__, __LINE__, pem_slot - 1, ret));

    
    ret = get_pem_fru_object(pem_slot, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get pem(%d) fru object handle failed. ret=%d", pem_slot - 1, ret));

    
    ret = get_pem_fru_info(powersupply_unit_jso, obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get pem(%d) fru info failed. ret=%d", pem_slot - 1, ret));

    return VOS_OK;
}


LOCAL gint32 add_pem_firmware_version(json_object *powersupply_unit_jso, gint32 pem_slot)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    gchar               software_version_str[SOFTWARE_VER_MAX_LEN] = {0};

    
    ret = dal_get_specific_object_byte(CLASS_ONEPOWER, PROTERY_PS_INDEX, pem_slot, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get pem(%d) onepower handle failed. ret=%d", pem_slot, ret));
    (void)dal_get_property_value_string(obj_handle, PROTERY_PS_FWVERSION, software_version_str,
        sizeof(software_version_str));
    if (strlen(software_version_str) == 0) {
        json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_FIRMWARE_VER, NULL);
    } else {
        json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_FIRMWARE_VER,
            json_object_new_string(software_version_str));
    }
    return VOS_OK;
}


LOCAL gint32 add_pem_hardware_version(json_object *huawei_jso, gint32 pem_slot)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    gchar hardware_version[HARDWARE_VER_MAX_LEN] = { 0 };

    
    ret = dal_get_specific_object_byte(CLASS_ONEPOWER, PROTERY_PS_INDEX, pem_slot, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get pem(%d) powermgnt handle failed. ret=%d", pem_slot, ret));
    (void)dal_get_property_value_string(obj_handle, PROTERY_PS_VERSION, hardware_version, sizeof(hardware_version));
    if (strlen(hardware_version) == 0) {
        json_object_object_add(huawei_jso, RFPROP_POWER_SUPPLIES_HARDWARE_VER, NULL);
    } else {
        json_object_object_add(huawei_jso, RFPROP_POWER_SUPPLIES_HARDWARE_VER,
            json_object_new_string(hardware_version));
    }
    return VOS_OK;
}


LOCAL gint32 add_pem_sleep_status(json_object *huawei_jso, gint32 pem_slot)
{
    OBJ_HANDLE pssleep_obj_handle = 0;
    gint32 ret;
    guint16 sleep_bitmap = 0;

    
    ret = dal_get_object_handle_nth(CLASS_POWER_SLEEP, 0, &pssleep_obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get PsSleep object handle failed. ret=%d", ret));

    
    ret = dal_get_property_value_uint16(pssleep_obj_handle, ROPERTY_POWER_SLEEP_SLEEP_STATUS_BITMAP, &sleep_bitmap);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get pem SleepStatusBitmap failed. ret=%d", ret));

    
    if ((sleep_bitmap >> ((guint32)pem_slot)) & 0x0001) {
        
        json_object_object_add(huawei_jso, RFPROP_POWER_SUPPLIES_SLEEP_STATUS, json_object_new_boolean(TRUE));
    } else {
        json_object_object_add(huawei_jso, RFPROP_POWER_SUPPLIES_SLEEP_STATUS, json_object_new_boolean(FALSE));
    }

    return VOS_OK;
}


LOCAL gint32 add_pem_fault_reasons(json_object *huawei_jso, gint32 pem_slot)
{
#define FAULT_REASON_BUFF 1024
    OBJ_HANDLE powermgnt_handle = 0;
    OBJ_HANDLE onepower_handle = 0;
    gchar fault_reason[FAULT_REASON_BUFF] = { 0 };
    json_object *fault_jso = NULL;
    guint8 comunication_state = 0;
    gint32 ret;
    guint8 presence = 0;

    
    ret = dal_get_specific_object_byte(CLASS_POWER_MGNT, PROPERTY_POWER_MGNT_SLOT, pem_slot,
        &powermgnt_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get pem(%d) powermgnt object handle failed. ret=%d", pem_slot, ret));
    
    (void)dal_get_property_value_byte(powermgnt_handle, PROPERTY_POWER_MGNT_PRESENCE, (guchar *)&presence);
    fault_jso = json_object_new_array();
    if (presence) {
        
        ret = dal_get_specific_object_byte(CLASS_NAME_PS, PROTERY_PS_INDEX, pem_slot, &onepower_handle);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, json_object_put(fault_jso);
            debug_log(DLOG_ERROR, "get pem(%d) onepower object handle failed. ret=%d", pem_slot, ret));
        
        (void)dal_get_property_value_byte(onepower_handle, PROTERY_PS_COMMU_STATE, &comunication_state);
        
        if (VOS_OK != comunication_state) {
            json_object_array_add(fault_jso, json_object_new_string("Communication lost."));
        } else { 
            (void)dal_get_property_value_string(powermgnt_handle, PROPERTY_POWER_MGNT_FAULT_REASON, fault_reason,
                sizeof(fault_reason));
            convert_fault_reason_str_to_json(fault_jso, fault_reason, sizeof(fault_reason));
        }
    }
    
    json_object_object_add(huawei_jso, RFPROP_POWER_FAULT_REASONS, fault_jso);
    return VOS_OK;
}


LOCAL gint32 add_pem_oem_fru_data(json_object* p_obj, gint8 pem_slot)
{
    gint32 ret;
    OBJ_HANDLE elabel_handle = 0;
    json_object *obj = NULL;
    json_object *value_obj = NULL;
    json_object *fru_obj = NULL;
    const gchar *value_str = NULL;
    gsize value_str_len = 0;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    OBJ_HANDLE obj_handle = 0;

    ret = get_pem_fru_object(pem_slot, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "get pem(%d) fru object handle failed. ret=%d", pem_slot, ret));

    
    ret = dfl_get_referenced_object(obj_handle, PROPERTY_FRU_ELABEL_RO, &elabel_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get pem fru referenced elabel object failed, err code %d", __func__, ret);
        return VOS_ERR;
    }
    
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_BOARD_ID);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_BOARD_MFG_TIME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_BOARD_MFG_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_BOARD_PRO_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_BOARD_SN);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_BOARD_PN);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_PRODUCT_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_PRODUCT_VERSION);

    ret = dfl_multiget_property_value(elabel_handle, property_name_list, &property_value_list);

    g_slist_free(property_name_list);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "[%s] Get pem fru info failed, err code %d", __func__, ret);
        return VOS_ERR;
    }

    fru_obj = json_object_new_object();
    if (fru_obj == NULL) {
        g_slist_free_full(property_value_list, (GDestroyNotify)g_variant_unref);
        return VOS_ERR;
    }

    
    obj = json_object_new_object();
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 0), &value_str_len);
    if (value_str_len != 0) {
        value_obj = json_object_new_string(value_str);
    } else {
        value_obj = NULL;
    }
    json_object_object_add(obj, "FruFileId", value_obj);

    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 1), &value_str_len);
    if (value_str_len != 0) {
        value_obj = json_object_new_string(value_str);
    } else {
        value_obj = NULL;
    }
    json_object_object_add(obj, "ManufacturingDate", value_obj);

    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 2), &value_str_len);
    if (value_str_len != 0) {
        value_obj = json_object_new_string(value_str);
    } else {
        value_obj = NULL;
    }
    json_object_object_add(obj, "Manufacturer", value_obj);

    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 3), &value_str_len);
    if (value_str_len != 0) {
        value_obj = json_object_new_string(value_str);
    } else {
        value_obj = NULL;
    }
    json_object_object_add(obj, "ProductName", value_obj);

    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 4), &value_str_len);
    if (value_str_len != 0) {
        value_obj = json_object_new_string(value_str);
    } else {
        value_obj = NULL;
    }
    json_object_object_add(obj, "SerialNumber", value_obj);

    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 5), &value_str_len);
    if (value_str_len != 0) {
        value_obj = json_object_new_string(value_str);
    } else {
        value_obj = NULL;
    }
    json_object_object_add(obj, "PartNumber", value_obj);

    
    json_object_object_add(fru_obj, "Board", obj);

    
    obj = json_object_new_object();

    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 6), &value_str_len);
    if (value_str_len != 0) {
        value_obj = json_object_new_string(value_str);
    } else {
        value_obj = NULL;
    }
    json_object_object_add(obj, "ProductName", value_obj);

    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 7), &value_str_len);
    if (value_str_len != 0) {
        value_obj = json_object_new_string(value_str);
    } else {
        value_obj = NULL;
    }
    json_object_object_add(obj, "Version", value_obj);

    
    json_object_object_add(fru_obj, "Product", obj);

    g_slist_free_full(property_value_list, (GDestroyNotify)g_variant_unref);
    
    json_object_object_add(p_obj, "Fru", fru_obj);
    return VOS_OK;
}


LOCAL gint32 add_powersupply_oem_huawei(json_object *huawei_jso, gint32 pem_slot)
{
    gint32 ret;
    
    json_object_object_add(huawei_jso, RFPROP_POWER_SUPPLIES_SLOTNUM, json_object_new_int(pem_slot + 1));

    
    ret = add_pem_fault_reasons(huawei_jso, pem_slot);
    return_val_if_fail(VOS_OK == ret, ret);

    ret = add_pem_hardware_version(huawei_jso, pem_slot);
    return_val_if_fail(VOS_OK == ret, ret);

    ret = add_pem_sleep_status(huawei_jso, pem_slot);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = add_pem_oem_fru_data(huawei_jso, pem_slot);
    return_val_if_fail(VOS_OK == ret, ret);

    return VOS_OK;
}


LOCAL gint32 add_power_consumed(json_object *power_control)
{
    guint16 current_power = 0;
    OBJ_HANDLE handle = 0;
    gint32 ret;
    ret = dfl_get_object_handle(OBJ_NAME_SHELF_POWER_CAPPING, &handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get shelf power capping object handle failed, err code %d", __func__, ret);
        json_object_object_add(power_control, RFPROP_POWER_CONTROL_POWER_CONSUME, NULL);
        return ret;
    }
    (void)dal_get_property_value_uint16(handle, PROTERY_SHELF_POWER_CAPPING_CURRENT_POWER, &current_power);
    json_object_object_add(power_control, RFPROP_POWER_CONTROL_POWER_CONSUME, json_object_new_int(current_power));
    return VOS_OK;
}


LOCAL gint32 add_power_capacity(json_object *power_control)
{
    guint16 capacity_watts = 0;
    OBJ_HANDLE handle = 0;
    gint32 ret;
    ret = dal_get_object_handle_nth(CLASS_NAME_AMMETER, 0, &handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get class[%s] object0 handle failed, err code %d", __func__, CLASS_NAME_AMMETER,
            ret);
        json_object_object_add(power_control, RFPROP_POWER_CAPACITY_WATTS, NULL);
        return ret;
    }
    (void)dal_get_property_value_uint16(handle, PROTERY_POWERCAP_MAXCAPVALUE, &capacity_watts);
    json_object_object_add(power_control, RFPROP_POWER_CAPACITY_WATTS, json_object_new_int(capacity_watts));
    return VOS_OK;
}


LOCAL gint32 add_power_limit(json_object *power_control)
{
    json_object *power_limit = NULL;
    guint16 limit_value = 0;
    OBJ_HANDLE handle = 0;
    gint32 ret;
    ret = dfl_get_object_handle(OBJ_NAME_SHELF_POWER_CAPPING, &handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get shelf power capping object handle failed, err code %d", __func__, ret);
        json_object_object_add(power_control, RFPROP_POWER_CONTROL_LIMIT, NULL);
        return ret;
    }
    (void)dal_get_property_value_uint16(handle, PROTERY_SHELF_POWER_CAPPING_VALUE, &limit_value);
    power_limit = json_object_new_object();
    
    json_object_object_add(power_limit, RFPROP_POWER_CONTROL_LIMIT_VALUE, json_object_new_int(limit_value));
    
    json_object_object_add(power_control, RFPROP_POWER_CONTROL_LIMIT, power_limit);
    return VOS_OK;
}

LOCAL gint32 add_power_related_item(json_object *power_control)
{
    json_object *related_item_array = NULL;
    json_object *chassis_item = NULL;
    chassis_item = json_object_new_object();
    json_object_object_add(chassis_item, RFPROP_ODATA_ID, json_object_new_string(URI_CHASSIS_ENCLOSURE));
    related_item_array = json_object_new_array();
    json_object_array_add(related_item_array, chassis_item);
    json_object_object_add(power_control, RFPROP_POWER_RELATED_ITEM, related_item_array);
    return VOS_OK;
}


LOCAL gint32 add_power_drop_stamp(json_object *power_control)
{
    OBJ_HANDLE handle = 0;
    gint32 ret;
    gchar time_stamp[MAX_TIME_LENGTH] = { 0 };
    // 获取syspower对象
    ret = dal_get_object_handle_nth(CLASS_NAME_AMMETER, 0, &handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get class[%s] object[%d] failed, err code %d", __func__, CLASS_NAME_AMMETER, 0,
            ret);
        json_object_object_add(power_control, RFPROP_POWER_DROP_STAMP, NULL);
        return ret;
    }
    // 读取掉电时间
    ret = dal_get_property_value_string(handle, PROPERTY_BOARD_POWER_DROP_STAMP, time_stamp, sizeof(time_stamp));
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get class[%s] object[%d] property[%s] failed, err code %d", __func__,
            CLASS_NAME_AMMETER, 0, PROPERTY_BOARD_POWER_DROP_STAMP, ret);
        json_object_object_add(power_control, RFPROP_POWER_DROP_STAMP, NULL);
        return ret;
    }
    json_object_object_add(power_control, RFPROP_POWER_DROP_STAMP,
        strlen(time_stamp) == 0 ? NULL : json_object_new_string(time_stamp));
    return VOS_OK;
}

LOCAL gint32 add_power_control_oem(json_object *power_control)
{
    int iRet = -1;
    json_object *huawei = NULL;
    json_object *oem = NULL;
    guint8 mode = 0;
    guint8 enable = 0;
    guint8 threshold = 0;
    guint8 active_state = 0;
    OBJ_HANDLE handle = 0;
    gint32 ret;

    guint16 blade_demand_value = 0;
    guint16 blade_set_value = 0;
    guint8 manual_state = 0;
    json_object *blade_info = NULL;
    GSList *blade_handle_list = NULL;
    GSList *blade_node = NULL;
    OBJ_HANDLE blade_handle = 0;

    GSList *output_list = NULL;
    guint16 minimum_value = 0;
    guint16 shelf_demand_value = 0;
    guint16 shelf_extra_value = 0;
    guint16 shelf_setted_value = 0;
    guint16 recommend_minimum_value = 0;
    guint16 maximum_value = 0;
    json_object *value_range_json = NULL;
    guint8 blade_presence = 0;
    gchar odata_id[MAX_URI_LEN] = { 0 };

    json_object *obj = NULL;
    
    ret = dfl_get_object_handle(OBJ_NAME_SHELF_POWER_CAPPING, &handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get shelf power capping object handle failed, err code %d", __func__, ret);
        json_object_object_add(power_control, RFPROP_POWER_CONTROL_LIMIT, NULL);
        return ret;
    }
    
    (void)dal_get_property_value_byte(handle, PROTERY_SHELF_POWER_CAPPING_MODE, &mode);
    (void)dal_get_property_value_byte(handle, PROTERY_SHELF_POWER_CAPPING_ENABLE, &enable);
    (void)dal_get_property_value_byte(handle, PROTERY_SHELF_POWER_CAPPING_THRESHOLD, &threshold);
    (void)dal_get_property_value_byte(handle, PROTERY_SHELF_POWER_CAPPING_ACTIVE_STATE, &active_state);
    
    huawei = json_object_new_object();
    
    json_object_object_add(huawei, RFPROP_POWER_LIMIT_MODE,
        json_object_new_string((mode == 0) ? "Equal" : ((mode == 1) ? "Manual" : "Proportion")));
    
    json_object_object_add(huawei, "PowerLimitEnabled", json_object_new_boolean(enable));
    
    json_object_object_add(huawei, RFPROP_POWER_LIMIT_THRESHOLD, json_object_new_int(threshold));
    
    output_list = NULL;
    ret = dfl_call_class_method(handle, METHOD_GET_SHELF_POWER_CAPPING_VALUE_RANGE, NULL, NULL, &output_list);
    if (ret == VOS_OK) {
        minimum_value = 0;
        recommend_minimum_value = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 0));
        maximum_value = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 1));
        value_range_json = json_object_new_object();
        ret = get_minimum_power_capping_value(&shelf_demand_value, &shelf_extra_value, &shelf_setted_value);
        if (ret != VOS_OK) {
            json_object_object_add(value_range_json, "MinimumValue", NULL);
        } else {
            minimum_value = shelf_extra_value + shelf_setted_value;
            json_object_object_add(value_range_json, "MinimumValue", json_object_new_int(minimum_value));
        }
        json_object_object_add(value_range_json, "RecommendMinimumValue", json_object_new_int(recommend_minimum_value));
        json_object_object_add(value_range_json, "MaximumValue", json_object_new_int(maximum_value));
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    } else {
        value_range_json = NULL;
    }
    json_object_object_add(huawei, RFPROP_POWER_LIMIT_VALUE_RANGE, value_range_json);
    
    json_object_object_add(huawei, RFPROP_POWER_LIMIT_ACTIVATED, json_object_new_boolean(active_state));
    
    ret = dfl_get_object_list(CLASS_NAME_IPMBETH_BLADE, &blade_handle_list);
    if (ret == VOS_OK) {
        blade_info = json_object_new_array();
        blade_node = blade_handle_list;
        while (blade_node != NULL) {
            blade_handle = (OBJ_HANDLE)(blade_node->data);
            blade_node = blade_node->next;
            if (!is_compute_node(blade_handle)) { // 非计算节点无功率封顶
                continue;
            }
            // 在位判断
            (void)dal_get_property_value_byte(blade_handle, PROTERY_IPMBETH_BLADE_PRESENCE, &blade_presence);
            obj = json_object_new_object();
            iRet = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, "/redfish/v1/Chassis/%s",
                dfl_get_object_name(blade_handle));
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string(odata_id));
            if (blade_presence == BLADE_PRESENCE) { // 在位时，查询数据
                (void)dal_get_property_value_uint16(blade_handle, PROTERY_IPMBETH_POWER_CAPPING_BASE_VALUE,
                    &blade_demand_value);
                (void)dal_get_property_value_byte(blade_handle, PROTERY_IPMBETH_POWER_CAPPING_MANUAL_STATE,
                    &manual_state);
                (void)dal_get_property_value_uint16(blade_handle, PROTERY_IPMBETH_POWER_CAPPING_VALUE,
                    &blade_set_value);
                json_object_object_add(obj, "MinimumValueWatts", json_object_new_int(blade_demand_value));
                json_object_object_add(obj, "IsManual", json_object_new_boolean(manual_state));
                json_object_object_add(obj, "LimitInWatts", json_object_new_int(blade_set_value));
            } else {
                json_object_object_add(obj, "MinimumValueWatts", NULL);
                json_object_object_add(obj, "IsManual", NULL);
                json_object_object_add(obj, "LimitInWatts", NULL);
            }
            json_object_array_add(blade_info, obj);
        }
    } else {
        blade_info = NULL;
        debug_log(DLOG_ERROR, "[%s] Get object list of class[%s] failed, err code %d", __func__,
            CLASS_NAME_IPMBETH_BLADE, ret);
    }
    g_slist_free(blade_handle_list);
    json_object_object_add(huawei, RFPROP_POWER_LIMIT_BLADE_INFO, blade_info);
    oem = json_object_new_object();
    json_object_object_add(oem, RFPROP_OEM_HUAWEI, huawei);
    json_object_object_add(power_control, RFPROP_OEM, oem);
    return VOS_OK;
}

LOCAL gint32 get_chassis_enclosure_powercontrol(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *power_control = NULL;
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    *o_result_jso = json_object_new_array();
    power_control = json_object_new_object();
    
    json_object_object_add(power_control, RFPROP_ODATA_ID,
        json_object_new_string("/redfish/v1/Chassis/Enclosure/Power#/PowerControl/0"));
    
    json_object_object_add(power_control, MEMBER_ID, json_object_new_string("0"));
    
    json_object_object_add(power_control, RF_PROPERTY_NAME, json_object_new_string("Chassis Power Control"));
    
    add_power_consumed(power_control);
    
    add_power_capacity(power_control);
    
    add_power_drop_stamp(power_control);
    
    add_power_limit(power_control);
    
    add_power_related_item(power_control);
    
    add_power_control_oem(power_control);
    
    json_object_array_add(*o_result_jso, power_control);
    return HTTP_OK;
}


LOCAL gint32 get_minimum_power_capping_value(guint16 *demand_value, guint16 *extra_value,
    guint16 *setted_manually_value)
{
    gint32 result;
    OBJ_HANDLE object_handle = 0;
    GSList *handle_list = NULL;
    GSList *list_item = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    guint8 presence = 0;
    guint8 state = 0;
    guint16 blade_value = 0;
    guint16 base_value = 0;
    OBJ_HANDLE syspower_object_handle = 0;
    guint32 ps_total_power = 0;
    guint16 current_power = 0;
    guint32 total_current_power = 0;

    return_val_if_fail(NULL != demand_value, VOS_ERR);

    
    result = dfl_get_object_list(CLASS_NAME_IPMBETH_BLADE, &handle_list);
    return_val_if_fail(VOS_OK == result, result);

    for (list_item = handle_list; list_item; list_item = g_slist_next(list_item)) {
        object_handle = (OBJ_HANDLE)list_item->data;

        
        continue_if_expr(FALSE == is_compute_node(object_handle));
        

        property_name_list = g_slist_append(property_name_list, PROTERY_IPMBETH_POWER_CAPPING_MANUAL_STATE);
        property_name_list = g_slist_append(property_name_list, PROTERY_IPMBETH_POWER_CAPPING_VALUE);
        property_name_list = g_slist_append(property_name_list, PROTERY_IPMBETH_POWER_CAPPING_BASE_VALUE);
        result = dfl_multiget_property_value(object_handle, property_name_list, &property_value_list);
        if (VOS_OK != result) {
            g_slist_free(property_name_list);
            property_name_list = NULL;
            continue;
        }

        state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 0));
        blade_value = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, 1));
        base_value = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, 2));
        g_slist_free(property_name_list);
        g_slist_free_full(property_value_list, (GDestroyNotify)g_variant_unref);
        property_name_list = NULL;
        property_value_list = NULL;

        
        (void)dal_get_property_value_byte(object_handle, PROTERY_IPMBETH_BLADE_PRESENCE, &presence);
        

        
        if (BLADE_PRESENCE == presence) {
            if (1 == state) {
                if (blade_value < USHRT_MAX - *demand_value) {
                    *demand_value += blade_value;
                    *setted_manually_value += blade_value;
                } else {
                    g_slist_free(handle_list);
                    return VOS_ERR;
                }
            } else {
                if (base_value < USHRT_MAX - *demand_value) {
                    *demand_value += base_value;
                } else {
                    g_slist_free(handle_list);
                    return VOS_ERR;
                }
            }

            
            (void)dal_get_property_value_uint16(object_handle, PROTERY_IPMBETH_POWER_CAPPING_CURRENT_POWER,
                &current_power);

            total_current_power += current_power;
            
        }
    }

    g_slist_free(handle_list);

    
    
    (void)dfl_get_object_handle(OBJ_NAME_AMMETER, &syspower_object_handle);
    (void)dal_get_property_value_uint32(syspower_object_handle, PROTERY_PSTOTAL_POWER, &ps_total_power);

    if (ps_total_power > total_current_power) {
        *demand_value += (ps_total_power - total_current_power);
        *extra_value = ps_total_power - total_current_power;
    } else {
        return VOS_ERR;
    }
    

    return VOS_OK;
}


LOCAL gint32 set_chassis_enclosure_power_capping_value(PROVIDER_PARAS_S *i_paras, json_object *limit_value_jso,
    json_object **o_message_jso)
{
    gint32 ret;
    OBJ_HANDLE pc_handle = 0;
    OBJ_HANDLE shelf_pc_handle = 0;
    guint16 max_value = 0;
    guint16 demand_value =
        0; // 推荐最小值，单板需求最小值之和 加上 extra_value，如果手动设置了值，则改为单板设置值计入求和
    guint16 extra_value = 0;           // 除业务单板外的其他功率消耗
    guint16 setted_manually_value = 0; // 手动设置的单板值之和
    GSList *input_list = 0;
    guint16 limit_value;
    json_object *msg_jso = NULL;
    const gchar *property_name = "PowerControl/0/PowerLimit/LimitInWatts";
    gchar param1[STRING_LEN_MAX] = { 0 };
    gchar param2[STRING_LEN_MAX] = { 0 };

    if (json_object_get_type(limit_value_jso) != json_type_int) { // 数据格式不为整数
        (void)create_message_info(MSGID_PROP_TYPE_ERR, property_name, &msg_jso,
            dal_json_object_get_str(limit_value_jso), property_name);
        json_object_array_add(*o_message_jso, msg_jso);
        return HTTP_BAD_REQUEST;
    }
    limit_value = json_object_get_int(limit_value_jso);
    ret = dal_get_object_handle_nth(CLASS_NAME_POWERCAPPING, 0, &pc_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get class[%s] object[%d] failed, err code %d", __func__, CLASS_NAME_POWERCAPPING, 0,
            ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = dal_get_object_handle_nth(CLASS_NAME_SHELF_POWER_CAPPING, 0, &shelf_pc_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get class[%s] object[%d] failed, err code %d", __func__,
            CLASS_NAME_SHELF_POWER_CAPPING, 0, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    (void)dal_get_property_value_uint16(pc_handle, PROTERY_POWERCAP_TOPVALUE, &max_value);
    
    ret = get_minimum_power_capping_value(&demand_value, &extra_value, &setted_manually_value);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Call get_minimum_power_capping_value failed, err code %d", __func__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if (demand_value > max_value) { // 整框总功率不足
        (void)snprintf_s(param1, sizeof(param1), sizeof(param1) - 1, "%u", max_value);
        (void)snprintf_s(param2, sizeof(param2), sizeof(param2) - 1, "%u", demand_value);
        (void)create_message_info(MSGID_CAPACITY_VALUE_LESS_THAN_DEMAND, property_name, &msg_jso, param1, param2);
        json_object_array_add(*o_message_jso, msg_jso);
        return HTTP_BAD_REQUEST;
    }
    if (limit_value < extra_value + setted_manually_value) { // 设定值小于必须的值
        (void)snprintf_s(param1, sizeof(param1), sizeof(param1) - 1, "%u", limit_value);
        (void)snprintf_s(param2, sizeof(param2), sizeof(param2) - 1, "%u", extra_value + setted_manually_value);
        (void)create_message_info(MSGID_POWER_CAPPING_VALUE_LESS_THAN_ALLOWED, property_name, &msg_jso, param1, param2);
        json_object_array_add(*o_message_jso, msg_jso);
        return HTTP_BAD_REQUEST;
    }
    if (limit_value > max_value) { // 超过功率总额
        (void)snprintf_s(param1, sizeof(param1), sizeof(param1) - 1, "%u", limit_value);
        (void)snprintf_s(param2, sizeof(param2), sizeof(param2) - 1, "%u", max_value);
        (void)create_message_info(MSGID_POWER_CAPPING_VALUE_GREAT_THAN_CAPACITY, property_name, &msg_jso, param1,
            param2);
        json_object_array_add(*o_message_jso, msg_jso);
        return HTTP_BAD_REQUEST;
    }
    // (limit_value < demand_value) is allowed, but may failed
    // 设置值
    input_list = g_slist_append(input_list, g_variant_new_uint16(limit_value));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, shelf_pc_handle,
        CLASS_NAME_SHELF_POWER_CAPPING, METHOD_SET_SHELF_POWER_CAPPING_VALUE, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Call class[%s] method[%s] failed, err code %d", __func__,
            CLASS_NAME_SHELF_POWER_CAPPING, METHOD_SET_SHELF_POWER_CAPPING_VALUE, ret);
        (void)create_message_info(MSGID_POWER_CAPPING_VALUE_SET_FAILED, property_name, &msg_jso);
        json_object_array_add(*o_message_jso, msg_jso);
        return HTTP_BAD_REQUEST;
    }
    return HTTP_OK;
}


LOCAL gint32 set_powercontrol_power_limit_mode(OBJ_HANDLE handle, PROVIDER_PARAS_S *i_paras, json_object *huawei_jso,
    json_object **o_message_jso)
{
    json_object *limit_mode_jso = NULL;
    const gchar *limit_mode_str = 0;
    guint8 limit_mode_value = 0;
    gint32 ret;
    json_object *msg_jso = NULL;
    GSList *input_list = NULL;
    gint32 rf_ret = HTTP_BAD_REQUEST;

    ret = json_object_object_get_ex(huawei_jso, RFPROP_POWER_LIMIT_MODE, &limit_mode_jso);
    if (!ret) {
        return HTTP_BAD_REQUEST;
    }
    limit_mode_str = dal_json_object_get_str(limit_mode_jso);
    if (g_ascii_strcasecmp(limit_mode_str, RF_POWER_LIMIT_MODE_EQUAL) == 0) {
        limit_mode_value = 0;
    } else if (g_ascii_strcasecmp(limit_mode_str, RF_POWER_LIMIT_MODE_MANUAL) == 0) {
        limit_mode_value = 1;
    } else if (g_ascii_strcasecmp(limit_mode_str, RF_POWER_LIMIT_MODE_PROPORTION) == 0) {
        limit_mode_value = 2;
    } else {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, "PowerControl/0/Oem/Huawei/PowerLimitMode", &msg_jso,
            limit_mode_str, "PowerControl/0/Oem/Huawei/PowerLimitMode");
        json_object_array_add(*o_message_jso, msg_jso);
        return HTTP_BAD_REQUEST;
    }
    input_list = g_slist_append(input_list, g_variant_new_byte(limit_mode_value));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, handle,
        CLASS_NAME_SHELF_POWER_CAPPING, METHOD_SET_SHELF_POWER_CAPPING_MODE, AUTH_DISABLE, i_paras->user_role_privilege,
        input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Set power capping mode failed, err code %d", __func__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    } else {
        rf_ret = HTTP_OK;
    }
    return rf_ret;
}


LOCAL gint32 set_powercontrol_power_limit_enable(OBJ_HANDLE handle, PROVIDER_PARAS_S *i_paras, json_object *huawei_jso,
    json_object **o_message_jso)
{
    gint32 rf_ret = HTTP_BAD_REQUEST;
    gint32 ret;
    json_object *enable_state_jso = NULL;
    guint8 enable_state_value;
    GSList *input_list = NULL;
    ret = json_object_object_get_ex(huawei_jso, "PowerLimitEnabled", &enable_state_jso);
    if (!ret) {
        return HTTP_BAD_REQUEST;
    }
    enable_state_value = json_object_get_boolean(enable_state_jso) ? TRUE : FALSE; // Codex
    input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_byte(enable_state_value));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, handle,
        CLASS_NAME_SHELF_POWER_CAPPING, METHOD_SET_SHELF_POWER_CAPPING_ENABLE, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Set power capping enable failed, err code %d", __func__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    } else {
        rf_ret = HTTP_OK;
    }
    return rf_ret;
}


LOCAL gint32 set_powercontrol_power_limit_threshold(OBJ_HANDLE handle, PROVIDER_PARAS_S *i_paras,
    json_object *huawei_jso, json_object **o_message_jso)
{
    json_object *threshold_jso = NULL;
    const gchar *threshold_str = NULL;
    guint8 max_threshold_value = 0;
    gchar max_threshold_value_str[STRING_LEN_MAX] = { 0 };
    guint8 threshold_value = 0;
    gint32 int_value;
    gint32 ret;
    gint32 rf_ret = HTTP_BAD_REQUEST;
    json_object *msg_jso = NULL;
    GSList *input_list = NULL;
    const gchar *property_name = "PowerControl/0/Oem/Huawei/PowerLimitThreshold";

    ret = json_object_object_get_ex(huawei_jso, RFPROP_POWER_LIMIT_THRESHOLD, &threshold_jso);
    if (!ret) {
        return HTTP_BAD_REQUEST;
    }
    if (json_object_get_type(threshold_jso) != json_type_int) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, property_name, &msg_jso, dal_json_object_get_str(threshold_jso),
            property_name);
        json_object_array_add(*o_message_jso, msg_jso);
        msg_jso = NULL;
        return HTTP_BAD_REQUEST;
    }
    int_value = json_object_get_int(threshold_jso);
    if (int_value <= G_MAXUINT8) {
        threshold_value = (guint8)int_value;
    }
    (void)dal_get_property_value_byte(handle, PROTERY_SHELF_POWER_CAPPING_MAX_THRESHOLD, &max_threshold_value);
    if (threshold_value > max_threshold_value || int_value > G_MAXUINT8) {
        threshold_str = dal_json_object_get_str(threshold_jso);
        (void)snprintf_s(max_threshold_value_str, sizeof(max_threshold_value_str), sizeof(max_threshold_value_str) - 1,
            "%u", max_threshold_value);
        (void)create_message_info(MSGID_PROP_VALUE_OUTOFRANGE, "PowerControl/0/Oem/Huawei/PowerLimitThreshold",
            &msg_jso, threshold_str, "PowerControl/0/Oem/Huawei/PowerLimitThreshold", "0", max_threshold_value_str);
        json_object_array_add(*o_message_jso, msg_jso);
        return HTTP_BAD_REQUEST;
    }
    input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_byte(threshold_value));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, handle,
        CLASS_NAME_SHELF_POWER_CAPPING, METHOD_SET_SHELF_POWER_CAPPING_THRESHOLD, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Set power capping threshold failed, err code %d", __func__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    } else {
        rf_ret = HTTP_OK;
    }
    return rf_ret;
}


LOCAL gint32 set_powercontrol_power_limit_blade_info(OBJ_HANDLE handle, PROVIDER_PARAS_S *i_paras,
    json_object *huawei_jso, json_object **o_message_jso)
{
    gint32 ret;
    gint32 rf_ret = HTTP_BAD_REQUEST;
    json_object *blade_info_array_jso = NULL;
    json_object *blade_info = NULL;
    gint32 array_len;
    gint32 blade_count = 0;
    json_object *msg_jso = NULL;
    gchar info_str[MAX_STRBUF_LEN] = { 0 };
    json_object *obj = NULL;
    gint32 blade_number = 0;
    gboolean b_set_blade_value = FALSE;
    guint16 *set_value_array = NULL;
    OBJ_HANDLE shelf_handle = 0;
    guint8 mode = 0;
    guint8 ipmb_addr = 0;
    OBJ_HANDLE blade_handle = 0;
    guint8 presence = 0;
    guint16 max_value = 0;
    guint16 min_value = 0;
    GSList *input_list = NULL;
    gchar property_name[MAX_URI_LEN] = { 0 };
    gchar param1[STRING_LEN_MAX] = { 0 };
    gchar param2[STRING_LEN_MAX] = { 0 };
    ret = json_object_object_get_ex(huawei_jso, "PowerLimitBladeInfo", &blade_info_array_jso);
    if (!ret) {
        return HTTP_BAD_REQUEST;
    }
    // 数组长度检查
    ret = get_compute_node_count(&blade_count);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get compute node count failed, err code %d", __func__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    array_len = json_object_array_length(blade_info_array_jso);
    if (array_len == 0 || array_len > blade_count) {
        (void)snprintf_s(info_str, sizeof(info_str), sizeof(info_str) - 1, "1 ~ %d", blade_count);
        (void)create_message_info(MSGID_PROP_WRONG_ARRAY_LENGTH, "PowerControl/0/Oem/Huawei/PowerLimitBladeInfo",
            &msg_jso, info_str);
        json_object_array_add(*o_message_jso, msg_jso);
        return HTTP_BAD_REQUEST;
    }
    // 数组对象不可以全为空
    // 遍历输入
    set_value_array = (guint16 *)g_malloc0(sizeof(guint16) * array_len);
    if (set_value_array == NULL) {
        debug_log(DLOG_ERROR, "[%s] Malloc failed", __func__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    (void)memset_s(set_value_array, sizeof(guint16) * array_len, 0xff, sizeof(guint16) * array_len);
    json_object_array_foreach(blade_info_array_jso, blade_info)
    {
        ret = json_object_object_get_ex(blade_info, "LimitInWatts", &obj);
        if (ret) { // 设置单板功率封顶值
            // 检查值类型
            if (json_type_int != json_object_get_type(obj)) {
                (void)snprintf_s(info_str, sizeof(info_str), sizeof(info_str) - 1,
                    "PowerControl/0/Oem/Huawei/PowerLimitBladeInfo/%d/LimitInWatts", blade_number);
                (void)create_message_info(MSGID_PROP_TYPE_ERR, info_str, &msg_jso, dal_json_object_get_str(obj),
                    info_str);
                json_object_array_add(*o_message_jso, msg_jso);
                set_value_array[blade_number] = 0xffff;
            } else {
                b_set_blade_value = TRUE;
                set_value_array[blade_number] = json_object_get_int(obj);
            }
        }
        blade_number++;
    }
    // 设置单板封顶值
    if (b_set_blade_value) {
        ret = dal_get_object_handle_nth(CLASS_NAME_SHELF_POWER_CAPPING, 0, &shelf_handle);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "[%s] Get object0 of class[%s] failed, err code %d", __func__,
                CLASS_NAME_SHELF_POWER_CAPPING, ret);
            g_free(set_value_array);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        (void)dal_get_property_value_byte(shelf_handle, PROTERY_SHELF_POWER_CAPPING_MODE, &mode);
        if (1 != mode) { // 如果不是手动设置的模式
            (void)create_message_info(MSGID_POWER_CAPPING_NOT_IN_MANUAL_MODE, NULL, &msg_jso);
            json_object_array_add(*o_message_jso, msg_jso);
            g_free(set_value_array);
            return HTTP_BAD_REQUEST;
        }
        // 设置单板封顶值

        for (blade_number = 0; blade_number < array_len; blade_number++) {
            if (set_value_array[blade_number] == 0xffff) { // 值无效
                continue;
            }
            // 获取句柄
            ipmb_addr = get_blade_ipmbaddr(blade_number + 1);
            ret = dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_SLAVEADDR, ipmb_addr,
                &blade_handle);
            if (ret != VOS_OK) {
                debug_log(DLOG_ERROR, "[%s] Get object by[%s:%u] of class[%s] failed, err code %d", __func__,
                    PROTERY_IPMBETH_BLADE_SLAVEADDR, ipmb_addr, CLASS_NAME_IPMBETH_BLADE, ret);
                g_free(set_value_array);
                return HTTP_INTERNAL_SERVER_ERROR;
            }
            // 在位判断
            ret = dal_get_property_value_byte(blade_handle, PROTERY_IPMBETH_BLADE_PRESENCE, &presence);
            if (ret != VOS_OK) {
                debug_log(DLOG_ERROR, "[%s] Get blade property[%s] failed, err code %d", __func__,
                    PROTERY_IPMBETH_BLADE_PRESENCE, ret);
                g_free(set_value_array);
                return HTTP_INTERNAL_SERVER_ERROR;
            }
            if (presence != BLADE_PRESENCE) { // 不在位，不可设置数据
                (void)snprintf_s(property_name, sizeof(property_name), sizeof(property_name) - 1,
                    "PowerControl/0/Oem/Huawei/PowerLimitBladeInfo/%d/LimitInWatts", blade_number);
                (void)create_message_info(MSGID_BLADE_NOT_PRESENT, property_name, &msg_jso);
                json_object_array_add(*o_message_jso, msg_jso);
                continue;
            }
            // 获取可设置的值范围
            ret = smm_get_maximum_blade_power_capping_value(blade_handle, &max_value);
            if (VOS_OK != ret) {
                if (max_value & 0x8000) { // 剩余功率不足
                    (void)snprintf_s(property_name, sizeof(property_name), sizeof(property_name) - 1,
                        "PowerControl/0/Oem/Huawei/PowerLimitBladeInfo/%d/LimitInWatts", blade_number);
                    (void)create_message_info(MSGID_NO_POWER_CAPPING_VALUE_FOR_THE_BLADE, property_name, &msg_jso);
                    json_object_array_add(*o_message_jso, msg_jso);
                    continue;
                } else { // 内部错误
                    debug_log(DLOG_ERROR, "[%s] Call method[get_maximum_blade_power_capping_value] failed, err code %d",
                        __func__, ret);
                    g_free(set_value_array);
                    return HTTP_INTERNAL_SERVER_ERROR;
                }
            }
            
            if (max_value > 9999) {
                max_value = 9999; 
            }
            (void)dal_get_property_value_uint16(blade_handle, PROTERY_IPMBETH_POWER_CAPPING_BASE_VALUE, &min_value);
            // 判断值合法性
            if (set_value_array[blade_number] > max_value) {
                (void)snprintf_s(param1, sizeof(param1), sizeof(param1) - 1, "%u", set_value_array[blade_number]);
                (void)snprintf_s(param2, sizeof(param2), sizeof(param2) - 1, "%u", max_value);
                (void)snprintf_s(property_name, sizeof(property_name), sizeof(property_name) - 1,
                    "PowerControl/0/Oem/Huawei/PowerLimitBladeInfo/%d/LimitInWatts", blade_number);
                (void)create_message_info(MSGID_POWER_CAPPING_VALUE_GREAT_THAN_CAPACITY, property_name, &msg_jso,
                    param1, param2);
                json_object_array_add(*o_message_jso, msg_jso);
                continue;
            }
            
            input_list = NULL;
            input_list = g_slist_append(input_list, g_variant_new_uint16(set_value_array[blade_number]));
            ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                blade_handle, CLASS_NAME_IPMBETH_BLADE, METHOD_SET_BLADE_POWER_CAPPING_VALUE, AUTH_DISABLE,
                i_paras->user_role_privilege, input_list, NULL);
            uip_free_gvariant_list(input_list);
            if (ret == VOS_OK) {
                rf_ret = HTTP_OK;
            } else {
                debug_log(DLOG_ERROR, "[%s] Call method[%s] of class[%s] failed, err code %d", __func__,
                    METHOD_SET_BLADE_POWER_CAPPING_VALUE, CLASS_NAME_IPMBETH_BLADE, ret);
                (void)snprintf_s(property_name, sizeof(property_name), sizeof(property_name) - 1,
                    "PowerControl/0/Oem/Huawei/PowerLimitBladeInfo/%d/LimitInWatts", blade_number);
                (void)create_message_info(MSGID_POWER_CAPPING_BLADE_VALUE_SET_FAILED, property_name, &msg_jso);
                json_object_array_add(*o_message_jso, msg_jso);
                rf_ret = HTTP_BAD_REQUEST;
            }
        }
    }
    g_free(set_value_array);
    return rf_ret;
}


LOCAL gint32 set_chassis_enclosure_powercontrol_oem(PROVIDER_PARAS_S *i_paras, json_object *huawei_jso,
    json_object **o_message_jso)
{
    OBJ_HANDLE handle = 0;
    gint32 ret;
    gint32 rf_ret = 0;
    // 获取句柄
    ret = dal_get_object_handle_nth(CLASS_NAME_SHELF_POWER_CAPPING, 0, &handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get class[%s] object[%d] failed, err code %d", __func__,
            CLASS_NAME_SHELF_POWER_CAPPING, 0, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    // 功率封顶模式
    ret = set_powercontrol_power_limit_mode(handle, i_paras, huawei_jso, o_message_jso);
    if (HTTP_INTERNAL_SERVER_ERROR == ret) {
        return ret;
    } else if (HTTP_OK == ret) {
        rf_ret = HTTP_OK;
    }
    // 功率封顶使能
    ret = set_powercontrol_power_limit_enable(handle, i_paras, huawei_jso, o_message_jso);
    if (HTTP_INTERNAL_SERVER_ERROR == ret) {
        return ret;
    } else if (HTTP_OK == ret) {
        rf_ret = HTTP_OK;
    }
    // 功率封顶门限
    ret = set_powercontrol_power_limit_threshold(handle, i_paras, huawei_jso, o_message_jso);
    if (HTTP_INTERNAL_SERVER_ERROR == ret) {
        return ret;
    } else if (HTTP_OK == ret) {
        rf_ret = HTTP_OK;
    }
    // 功率封顶单板设置值
    ret = set_powercontrol_power_limit_blade_info(handle, i_paras, huawei_jso, o_message_jso);
    if (HTTP_INTERNAL_SERVER_ERROR == ret) {
        return ret;
    } else if (HTTP_OK == ret) {
        rf_ret = HTTP_OK;
    }
    return rf_ret;
}


LOCAL gint32 set_chassis_enclosure_powercontrol(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 rf_ret = HTTP_BAD_REQUEST;
    json_object *power_limit_obj = NULL;
    json_object *limit_value_obj = NULL;
    json_object *power_control = NULL;
    json_object *oem = NULL;
    json_object *huawei = NULL;
    gint32 ret;
    guchar power_capping_support = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_POWERMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_POWER_CONTROL, o_message_jso,
        RFPROP_POWER_CONTROL));
    
    ret = rf_check_array_length_valid(i_paras->val_jso, 1, o_message_jso, RFPROP_POWER_CONTROL);
    return_val_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST);
    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_POWER_CAPPLING, 
                               &power_capping_support);
    if (ret == VOS_OK && power_capping_support == 0) {
        debug_log(DLOG_ERROR, "shelf smm no support powercapping.");
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, RFPROP_POWER_CONTROL, o_message_jso, RFPROP_POWER_CONTROL);
        return HTTP_METHOD_NOT_ALLOWED;
    }
    
    power_control = json_object_array_get_idx(i_paras->val_jso, 0);
    *o_message_jso = json_object_new_array();
    if (*o_message_jso == NULL) {
        debug_log(DLOG_ERROR, "%s json_object_new_array failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    // 封顶值设置
    (void)json_object_object_get_ex(power_control, RFPROP_POWER_CONTROL_LIMIT, &power_limit_obj);
    ret = json_object_object_get_ex(power_limit_obj, RFPROP_POWER_CONTROL_LIMIT_VALUE, &limit_value_obj);
    if (ret) { // 设置封顶值
        ret = set_chassis_enclosure_power_capping_value(i_paras, limit_value_obj, o_message_jso);
        if (ret == HTTP_OK) {
            rf_ret = HTTP_OK;
        } else if (ret == HTTP_INTERNAL_SERVER_ERROR) {
            if (*o_message_jso != NULL) {
                json_object_put(*o_message_jso);
            }
            create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }
    // OEM属性设置
    (void)json_object_object_get_ex(power_control, RFPROP_OEM, &oem);
    ret = json_object_object_get_ex(oem, RFPROP_OEM_HUAWEI, &huawei);
    if (ret) { // 设置OEM
        ret = set_chassis_enclosure_powercontrol_oem(i_paras, huawei, o_message_jso);
        if (ret == HTTP_OK) {
            rf_ret = HTTP_OK;
        } else if (ret == HTTP_INTERNAL_SERVER_ERROR) {
            if (*o_message_jso != NULL) {
                json_object_put(*o_message_jso);
            }
            create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }
    return rf_ret;
}


LOCAL gint32 fill_powersupply_basic_info(json_object *powersupply_unit_jso, gint32 pem_slot)
{
    gint32 ret;
    gchar               odata_id_str[MAX_URI_LEN] = {0};
    gchar               powersupply_name_str[POWERSUPPLY_NAME_MAX_LEN] = {0};
    gchar               member_id_str[STRING_LEN_MAX] = { 0 };
    json_object *redundancy_id_jso = NULL;
    json_object *redundancy_id_array_jso = NULL;
    
    ret = sprintf_s(odata_id_str, MAX_URI_LEN, "/redfish/v1/Chassis/Enclosure/Power#/PowerSupplies/%d", pem_slot);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s psu(%d)sprintf_s failed,ret(%d).", __FUNCTION__, pem_slot, ret));
    json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_ODATA_ID, json_object_new_string(odata_id_str));

    
    ret = sprintf_s(member_id_str, sizeof(member_id_str), "%d", pem_slot);
    do_val_if_expr(ret <= 0,
        debug_log(DLOG_ERROR, "%s psu(%d) sprintf_s memberid failed,ret(%d).", __FUNCTION__, pem_slot, ret));
    json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_MEMBER_ID,
        json_object_new_string(member_id_str));

    
    ret = sprintf_s(powersupply_name_str, POWERSUPPLY_NAME_MAX_LEN, POWER_SUPPLY_UNIT_NAME"%d", pem_slot + 1);
    do_val_if_expr(ret <= 0,
        debug_log(DLOG_ERROR, "%s psu(%d)sprintf_s powersupply_name failed,ret(%d).", __FUNCTION__, pem_slot, ret));
    json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_NAME,
        json_object_new_string(powersupply_name_str));

    
    redundancy_id_jso = json_object_new_object();
    return_val_do_info_if_fail(redundancy_id_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s new redundancy object failed.", __FUNCTION__));

    redundancy_id_array_jso = json_object_new_array();
    return_val_do_info_if_fail(redundancy_id_array_jso, HTTP_INTERNAL_SERVER_ERROR,
        (void)json_object_put(redundancy_id_jso);
        debug_log(DLOG_ERROR, "%s new redundancy id array object failed.", __FUNCTION__));

    json_object_object_add(redundancy_id_jso, RFPROP_POWER_REDUNDANCY_ODATA_ID,
        json_object_new_string("/redfish/v1/Chassis/1/Power#/Redundancy/0"));
    json_object_array_add(redundancy_id_array_jso, redundancy_id_jso);
    json_object_object_add(powersupply_unit_jso, RFPROP_CHASSIS_ENCLOSURE_REDUNDANCY, redundancy_id_array_jso);
    return VOS_OK;
}


LOCAL gint32 add_pem_input_voltage(json_object* powersupply_unit_jso, gint32 pem_slot)
{
    OBJ_HANDLE obj_handle = 0;    
    gdouble double_value = 0.0;
    gint32  ret;

    
    ret = dal_get_specific_object_byte(CLASS_ONEPOWER, PROTERY_PS_INDEX, (guchar)pem_slot, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get pem(%d) onepower handle failed. ret=%d", __FUNCTION__, pem_slot, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    
    ret = dal_get_property_value_double(obj_handle, PROTERY_PS_VIN, &double_value);
    if (ret != DFL_OK) {
        json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_INPUT_VOLTAGE, NULL);
    } else {
        json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_INPUT_VOLTAGE, 
            json_object_new_int((gint32)double_value));
    }

    return HTTP_OK;
}


LOCAL gint32 add_pem_model(json_object* powersupply_unit_jso, gint32 pem_slot)
{
    OBJ_HANDLE obj_handle = 0;    
    gchar string_value[POWER_STR_MAX_LEN] = {0};
    gint32  ret;

    
    ret = dal_get_specific_object_byte(CLASS_ONEPOWER, PROTERY_PS_INDEX, (guchar)pem_slot, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get pem(%d) onepower handle failed. ret=%d", __FUNCTION__, pem_slot, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_property_value_string(obj_handle, PROTERY_PS_MODEL, string_value, sizeof(string_value));
    if (strlen(string_value) == 0) {
        json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_MODEL, NULL);
    } else {
        json_object_object_add(powersupply_unit_jso, RFPROP_POWER_SUPPLIES_MODEL, json_object_new_string(string_value));
    }

    return HTTP_OK;
}

LOCAL gint32 add_single_pem_info(json_object* powersupply_unit_jso, gint32 pem_slot)
{
    gint32  ret;
    if (powersupply_unit_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    ret = add_pem_rate_power(powersupply_unit_jso, pem_slot);
    if (ret != RET_OK) {
        (void)json_object_put(powersupply_unit_jso);
        return ret;
    }
    
    ret = add_pem_real_power(powersupply_unit_jso, pem_slot);
    if (ret != RET_OK) {
        (void)json_object_put(powersupply_unit_jso);
        return ret;
    }

    
    ret = add_pem_work_mode(powersupply_unit_jso, pem_slot);
    if (ret != RET_OK) {
        (void)json_object_put(powersupply_unit_jso);
        return ret;
    }

    
    ret = add_pem_fru_data(powersupply_unit_jso, pem_slot);
    if (ret != RET_OK) {
        (void)json_object_put(powersupply_unit_jso);
        return ret;
    }

    
    ret = add_pem_firmware_version(powersupply_unit_jso, pem_slot);
    if (ret != RET_OK) {
        (void)json_object_put(powersupply_unit_jso);
        return ret;
    }
    
    
    ret = add_pem_input_voltage(powersupply_unit_jso, pem_slot);
    if (ret != HTTP_OK) {
        (void)json_object_put(powersupply_unit_jso);
        return ret;
    }
    
    
    ret = add_pem_model(powersupply_unit_jso, pem_slot);
    if (ret != HTTP_OK) {
        (void)json_object_put(powersupply_unit_jso);
        return ret;
    }
    return RET_OK;
}

LOCAL gint32 add_powersupply_info(json_object* powersupply_unit_jso, gint32 pem_slot)
{
    gint32              ret;
    json_object*        oem_jso = NULL;
    json_object*        huawei_jso = NULL;
    
    
    ret = add_single_pem_info(powersupply_unit_jso, pem_slot);
    if (ret != RET_OK) {
        return ret;
    }
    
    oem_jso = json_object_new_object();
    if (oem_jso == NULL) { 
        (void)json_object_put(powersupply_unit_jso);
        debug_log(DLOG_ERROR, "%s new oem object failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    huawei_jso = json_object_new_object();
    if (huawei_jso == NULL) {
        (void)json_object_put(oem_jso);
        (void)json_object_put(powersupply_unit_jso);
        debug_log(DLOG_ERROR, "%s new object failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object_object_add(oem_jso, RFPROP_POWER_REDUNDANCY_HUAWEI, huawei_jso);
    json_object_object_add(powersupply_unit_jso, RFPROP_POWER_REDUNDANCY_OEM, oem_jso);

    ret = add_powersupply_oem_huawei(huawei_jso, pem_slot);
    if (ret != VOS_OK) {
        (void)json_object_put(powersupply_unit_jso);
        return ret;
    }
    return RET_OK;
}

LOCAL gint32 foreach_get_one_powersupply(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32              ret;
    json_object*        powersupply_unit_jso = NULL;
    json_object**       o_result_jso = (json_object **)user_data;
    guint8              pem_slot = 0;

    
    powersupply_unit_jso = json_object_new_object();
    if (powersupply_unit_jso == NULL) {
        debug_log(DLOG_ERROR, "%s new object failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SLOT_ID, &pem_slot);
    if (ret != VOS_OK) {
        (void)json_object_put(powersupply_unit_jso);
        debug_log(DLOG_ERROR, "%s get psuslot id failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = fill_powersupply_basic_info(powersupply_unit_jso, pem_slot);
    if (ret != RET_OK) {
        (void)json_object_put(powersupply_unit_jso);
        debug_log(DLOG_ERROR, "%s fill_powersupply_basic_info failed.", __FUNCTION__);
        return ret;
    }

    
    ret = add_powersupplies_status(powersupply_unit_jso, pem_slot);
    if (ret != VOS_OK) {
        (void)json_object_put(powersupply_unit_jso);
        return ret;
    }

    if (get_one_power_present(pem_slot) == TRUE) {
        ret = add_powersupply_info(powersupply_unit_jso, pem_slot);
        if (ret != RET_OK) {
            return ret;
        }
    }
    json_object_array_add(*o_result_jso, powersupply_unit_jso);
    return VOS_OK;
}


LOCAL gint32 get_chassis_enclosure_powersupplies(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
                                                 json_object** o_result_jso)
{
    gint32 ret;

    
    if (!o_result_jso || !o_message_jso || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(!*o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s new object failed.", __FUNCTION__));

    ret = dfl_foreach_object(CLASS_POWER_SLOT, foreach_get_one_powersupply, o_result_jso, NULL);
    if (ret != VOS_OK) {
        (void)json_object_put(*o_result_jso);
        debug_log(DLOG_ERROR, "%s foreach PSU info failed.", __FUNCTION__);
        return ret;
    }

    return HTTP_OK;
}


LOCAL gint32 add_redundancy_health_prop(json_object *status_jso)
{
    OBJ_HANDLE onepowerinfo_obj_handle = 0;
    OBJ_HANDLE pssleep_obj_handle = 0;
    guint8 mode = 0;
    guint32 pem_slot;
    gint32 ret;
    guint8 presence = 0;
    guint8 health = 0;
    guint32 unhealth_count = 0; 
    guint32 presence_count = 0; 
    guint32 sleep_count = 0;    
    guint16 sleep_bitmap = 0;
    guint32 pem_count = 0;

    (void)dfl_get_object_count(CLASS_POWER_SLOT, &pem_count);
    
    for (pem_slot = 1; pem_slot <= pem_count; pem_slot++) {
        
        ret = dal_get_specific_object_byte(CLASS_POWER_MGNT, PROPERTY_POWER_MGNT_SLOT, (guchar)pem_slot - 1,
            &onepowerinfo_obj_handle);
        return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "get pem(%d) powermgnt object handle failed. ret=%d", pem_slot - 1, ret));

        
        ret = dal_get_property_value_byte(onepowerinfo_obj_handle, PROPERTY_POWER_MGNT_PRESENCE, (guchar *)&presence);
        return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "get pem(%d) presence failed. ret=%d", pem_slot, ret));

        if (presence) {
            
            ret =
                dal_get_property_value_byte(onepowerinfo_obj_handle, PROPERTY_POWER_MGNT_PS_HEALTH, (guchar *)&health);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "get pem(%d) health failed. ret=%d", pem_slot, ret));

            check_fail_do(0 == health, unhealth_count++);
            presence_count++;
        }
    }

    
    ret = dal_get_object_handle_nth(CLASS_POWER_SLEEP, 0, &pssleep_obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get PsSleep object handle failed. ret=%d", ret));

    
    ret = dal_get_property_value_byte(pssleep_obj_handle, ROPERTY_POWER_SLEEP_MODE, (guchar *)&mode);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get pem Mode failed. ret=%d", ret));

    
    ret = dal_get_property_value_uint16(pssleep_obj_handle, ROPERTY_POWER_SLEEP_SLEEP_STATUS_BITMAP, &sleep_bitmap);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get pem SleepStatusBitmap failed. ret=%d", ret));

    
    while (sleep_bitmap) {
        
        sleep_bitmap &= sleep_bitmap - 1;
        sleep_count++;
    }

    if (0 == mode) {
        
        check_success_do(0 == unhealth_count,
            json_object_object_add(status_jso, RFPROP_CHASSIS_STATUS_HEALTH, json_object_new_string(OK_STRING)));
        check_success_do(1 == unhealth_count,
            json_object_object_add(status_jso, RFPROP_CHASSIS_STATUS_HEALTH, json_object_new_string(WARNING_STRING)));
        check_success_do(1 < unhealth_count,
            json_object_object_add(status_jso, RFPROP_CHASSIS_STATUS_HEALTH, json_object_new_string(CRITICAL_STRING)));
    } else {
        
        check_success_do(0 == unhealth_count,
            json_object_object_add(status_jso, RFPROP_CHASSIS_STATUS_HEALTH, json_object_new_string(OK_STRING)));
        check_success_do(0 < unhealth_count && (unhealth_count * 2 + sleep_count <= presence_count),
            json_object_object_add(status_jso, RFPROP_CHASSIS_STATUS_HEALTH, json_object_new_string(WARNING_STRING)));
        check_success_do(presence_count < unhealth_count * 2 + sleep_count,
            json_object_object_add(status_jso, RFPROP_CHASSIS_STATUS_HEALTH, json_object_new_string(CRITICAL_STRING)));
    }

    return VOS_OK;
}


LOCAL gint32 add_redundancy_status(json_object *redundancy_unit_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guint8 sleep_enable = 0;
    json_object *status_jso = NULL;

    
    ret = dal_get_object_handle_nth(CLASS_POWER_SLEEP, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get PsSleep object handle failed. ret=%d", ret));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_POWER_SLEEP_ENABLE, &sleep_enable);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get pem sleep status failed. ret=%d", ret));

    status_jso = json_object_new_object();

    
    if (0 == sleep_enable) {
        json_object_object_add(status_jso, RFPROP_CHASSIS_STATUS_STATE, json_object_new_string(DISABLED_STRING));
        json_object_object_add(status_jso, RFPROP_CHASSIS_STATUS_HEALTH, json_object_new_string(OK_STRING));
    } else {
        json_object_object_add(status_jso, RFPROP_CHASSIS_STATUS_STATE, json_object_new_string(ENABLED_STRING));

        ret = add_redundancy_health_prop(status_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(status_jso);
            status_jso = NULL);
    }

    json_object_object_add(redundancy_unit_jso, RFPROP_CHASSIS_STATUS, status_jso);

    return VOS_OK;
}


LOCAL gint32 add_redundancy_mode(json_object *huawei_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guint8 mode = 0;
    guint8 sleep_enable = 0;

    
    ret = dal_get_object_handle_nth(CLASS_POWER_SLEEP, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get PsSleep object handle failed. ret=%d", ret));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_POWER_SLEEP_ENABLE, &sleep_enable);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get pem sleep config failed. ret=%d", ret));

    
    if (sleep_enable) {
        
        ret = dal_get_property_value_byte(obj_handle, ROPERTY_POWER_SLEEP_MODE, &mode);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "get pem Mode failed. ret=%d", ret));

        if (0 == mode) {
            json_object_object_add(huawei_jso, "SleepMode", json_object_new_string(N1_MODE_STRING));
        } else {
            json_object_object_add(huawei_jso, "SleepMode", json_object_new_string(NN_MODE_STRING));
        }
    }

    return VOS_OK;
}


LOCAL gint32 parse_set_pem_sleep_error_message(gint32 ret, json_object **o_message_jso)
{
    gint32 rf_ret = 0;

    switch (ret) {
        case VOS_OK:
            rf_ret = HTTP_OK;
            break;

        case POWER_SET_SLEEPCONFIG_MANUAL_CLOSE:
            rf_ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_POWER_SET_SLEEPCONFIG_MANUAL_CLOSE, NULL, o_message_jso);
            debug_log(DLOG_ERROR,
                "failed to set the PSU hibernation mode because the PSU has been manually turned off. ret=%d", ret);
            break;

        case POEWR_SET_SLEEPNFIG_CONFIG_ERR:
            rf_ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_POEWR_SET_SLEEPNFIG_CONFIG_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "the current PSU configuration does not support hibernation. ret=%d", ret);
            break;

        case POWER_SET_SLEEPCONFIG_UNKNOW_ERR:

        default:
            rf_ret = HTTP_INTERNAL_SERVER_ERROR;
            debug_log(DLOG_ERROR, "failed to set the PSU hibernation mode because of unknown error. ret=%d", ret);
    }

    return rf_ret;
}


LOCAL gint32 check_redundancy_mode(json_object** o_message_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 sleep_enable = 0;
    
    
    ret = dal_get_object_handle_nth(CLASS_POWER_SLEEP, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get PsSleep object handle failed. ret=%d", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_POWER_SLEEP_ENABLE, &sleep_enable);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get pem sleep config failed. ret=%d", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    
    if (sleep_enable == 0) {
        (void)create_message_info(MSGID_POWER_MODE_NOT_SUPPORTED_MODIFY, "SleepMode", o_message_jso);
        debug_log(DLOG_ERROR, 
            "cannot set the power redundancy mode. power redundancy switch(SleepConfig) is not turned on");
        return HTTP_BAD_REQUEST;
    }
    return HTTP_OK;
}
 

LOCAL gint32 add_redundancy_sleepconfig(json_object *huawei_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guint8 sleep_enable = 0;

    
    ret = dal_get_object_handle_nth(CLASS_POWER_SLEEP, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get PsSleep object handle failed. ret=%d", ret));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_POWER_SLEEP_ENABLE, &sleep_enable);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get pem sleep config failed. ret=%d", ret));
    json_object_object_add(huawei_jso, RFPROP_POWER_REDUNDANCY_SLEEPCONFIG, json_object_new_boolean(sleep_enable));

    return VOS_OK;
}
   

LOCAL gint32 set_redundancy_sleepconfig(PROVIDER_PARAS_S* i_paras, gint32 config_exist, gint32 mode_exist, 
    json_object* sleep_config_jso, json_object* mode_jso, json_object** o_message_jso)
{
    gint32 ret;
    guint8 is_enable = 1;  // 1表示使能，
    guint8 mode = 0; // 0表示N+1模式，1表示N+N模式。休眠开关打开时默认初始化为N+1模式
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    ret = dal_get_object_handle_nth(CLASS_POWER_SLEEP, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get PsSleep object failed.ret is %d", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (config_exist) {
        is_enable = json_object_get_boolean(sleep_config_jso); 
    } 
    
    
    if (mode_exist) {
        if (!config_exist) {
            
            ret = check_redundancy_mode(o_message_jso);
            if (ret != HTTP_OK) { 
                return ret;
            } 
        }
     
        if (!g_strcmp0(dal_json_object_get_str(mode_jso), N1_MODE_STRING)) {
            mode = 0;
        } else {
            mode = 1;
        }
    } 

    input_list = g_slist_append(input_list, g_variant_new_byte(is_enable));
    input_list = g_slist_append(input_list, g_variant_new_byte(mode));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle, 
        CLASS_POWER_SLEEP, METHOD_SET_PS_SLEEP_CONFIG, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set psu sleep config failed.ret is %d", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    

    ret = g_variant_get_int32((GVariant*)g_slist_nth_data(output_list, 0));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    
    ret = parse_set_pem_sleep_error_message(ret, o_message_jso);

    return ret;
}


LOCAL gint32 add_redundancy_oem_huawei(json_object *huawei_jso)
{
    gint32 ret;

    
    ret = add_redundancy_mode(huawei_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = add_redundancy_sleepconfig(huawei_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    return VOS_OK;
}


LOCAL gint32 get_chassis_enclosure_redundancy(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint32 pem_slot;
    json_object *redundancy_unit_jso = NULL;
    json_object *redundancy_set_jso = NULL;
    json_object *redundancyset_odata_id_jso = NULL;
    json_object *oem_jso = NULL;
    json_object *huawei_jso = NULL;
    gchar               redundancyset_odata_id_str[MAX_URI_LEN] = {0};
    guint32 pem_count = 0;

    (void)dfl_get_object_count(CLASS_POWER_SLOT, &pem_count);
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    redundancy_unit_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == redundancy_unit_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "new object failed."));

    
    json_object_object_add(redundancy_unit_jso, RFPROP_POWER_REDUNDANCY_ODATA_ID,
        json_object_new_string("/redfish/v1/Chassis/Enclosure/Power#/Redundancy/0"));

    
    json_object_object_add(redundancy_unit_jso, RFPROP_POWER_REDUNDANCY_MEMBER_ID, json_object_new_string("0"));

    
    json_object_object_add(redundancy_unit_jso, RFPROP_POWER_REDUNDANCY_NAME, json_object_new_string("System Powers"));

    
    json_object_object_add(redundancy_unit_jso, RFPROP_POWER_REDUNDANCY_MIN_NUM_NEEDED,
        json_object_new_int(RF_POWER_REDUNDANCY_MIN_NUM_NEEDED_VALUE));

    
    json_object_object_add(redundancy_unit_jso, RFPROP_POWER_REDUNDANCY_MAX_NUM_SUPPORTED,
        json_object_new_int(pem_count));

    
    ret = add_redundancy_status(redundancy_unit_jso);
    return_val_do_info_if_fail(ret == VOS_OK, ret, (void)json_object_put(redundancy_unit_jso));

    redundancy_set_jso = json_object_new_array();
    return_val_do_info_if_fail(redundancy_set_jso, HTTP_INTERNAL_SERVER_ERROR,
        (void)json_object_put(redundancy_unit_jso);
        debug_log(DLOG_ERROR, "new object failed."));
    
    for (pem_slot = 0; pem_slot < pem_count; pem_slot++) {
        redundancyset_odata_id_jso = json_object_new_object();
        return_val_do_info_if_fail(redundancyset_odata_id_jso, HTTP_INTERNAL_SERVER_ERROR,
            (void)json_object_put(redundancy_unit_jso);
            (void)json_object_put(redundancy_set_jso); debug_log(DLOG_ERROR, "new object failed."));

        (void)snprintf_s(redundancyset_odata_id_str, MAX_URI_LEN, MAX_URI_LEN - 1,
            "/redfish/v1/Chassis/Enclosure/Power#/PowerSupplies/%u", pem_slot);

        json_object_object_add(redundancyset_odata_id_jso, RFPROP_POWER_REDUNDANCY_ODATA_ID,
            json_object_new_string(redundancyset_odata_id_str));
        json_object_array_add(redundancy_set_jso, redundancyset_odata_id_jso);
    }
    json_object_object_add(redundancy_unit_jso, RFPROP_POWER_REDUNDANCY_SET, redundancy_set_jso);

    
    oem_jso = json_object_new_object();
    return_val_do_info_if_fail(oem_jso, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(redundancy_unit_jso);
        debug_log(DLOG_ERROR, "new object failed."));

    huawei_jso = json_object_new_object();
    return_val_do_info_if_fail(huawei_jso, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(redundancy_unit_jso);
        (void)json_object_put(oem_jso); debug_log(DLOG_ERROR, "new object failed."));

    json_object_object_add(oem_jso, RFPROP_POWER_REDUNDANCY_HUAWEI, huawei_jso);
    json_object_object_add(redundancy_unit_jso, RFPROP_POWER_REDUNDANCY_OEM, oem_jso);

    ret = add_redundancy_oem_huawei(huawei_jso);
    return_val_do_info_if_fail(ret == VOS_OK, ret, (void)json_object_put(redundancy_unit_jso));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        (void)json_object_put(redundancy_unit_jso);
        debug_log(DLOG_ERROR, "new object failed."));

    json_object_array_add(*o_result_jso, redundancy_unit_jso);

    return HTTP_OK;
}

LOCAL gint32 set_redundancy_attribute(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 config_exist;
    gint32 mode_exist;
    gint32 rf_ret = HTTP_BAD_REQUEST;
    gint32 arr_len;
    json_object *arr_elem_jso = NULL;
    json_object *mode_jso = NULL;
    json_object *sleep_config_jso = NULL;
    json_object *oem_jso = NULL;
    json_object *huawei_jso = NULL;
    json_object *msg_jso = NULL;
    guint32 pem_count = 0;

    (void)dfl_get_object_count(CLASS_POWER_SLOT, &pem_count);
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_POWERMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_CHASSIS_ENCLOSURE_REDUNDANCY, o_message_jso,
        RFPROP_CHASSIS_ENCLOSURE_REDUNDANCY));

    arr_len = json_object_array_length(i_paras->val_jso);
    return_val_do_info_if_fail(arr_len >= 0 && (guint32)arr_len <= pem_count, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_MISSING, RFPROP_CHASSIS_ENCLOSURE_REDUNDANCY, o_message_jso,
        RFPROP_CHASSIS_ENCLOSURE_REDUNDANCY));

    
    arr_elem_jso = json_object_array_get_idx(i_paras->val_jso, 0);
    return_val_do_info_if_fail(arr_elem_jso, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_MISSING, RFPROP_CHASSIS_ENCLOSURE_POWERSUPPLIES, o_message_jso,
        RFPROP_CHASSIS_ENCLOSURE_POWERSUPPLIES));

    
    (void)json_object_object_get_ex(arr_elem_jso, RFPROP_POWER_REDUNDANCY_OEM, &oem_jso);
    (void)json_object_object_get_ex(oem_jso, RFPROP_POWER_REDUNDANCY_HUAWEI, &huawei_jso);
    config_exist = json_object_object_get_ex(huawei_jso, RFPROP_POWER_REDUNDANCY_SLEEPCONFIG, &sleep_config_jso);
    mode_exist = json_object_object_get_ex(huawei_jso, "SleepMode", &mode_jso);
    if (config_exist == TRUE || mode_exist == TRUE) {
        ret = set_redundancy_sleepconfig(i_paras, config_exist, mode_exist, sleep_config_jso, mode_jso, &msg_jso);
        if (ret != HTTP_OK) {
            if (*o_message_jso == NULL) {
                *o_message_jso = json_object_new_array();
            }
            json_object_array_add(*o_message_jso, msg_jso);
        }
        if (HTTP_INTERNAL_SERVER_ERROR == ret) {
            return ret;
        }
        
        if (ret == HTTP_OK) {
            rf_ret = HTTP_OK;
        }
    }

    return rf_ret;
}


gint32 chassis_enclosure_power_etag_del_property(json_object *object)
{
    SPECIAL_PROP_S value[] = {
        {2, {RFPROP_CHASSIS_ENCLOSURE_POWERSUPPLIES, RFPROP_POWER_SUPPLIES_CAPACITY, NULL, NULL, NULL}},
        {2, {RFPROP_CHASSIS_ENCLOSURE_POWERSUPPLIES, RFPROP_POWER_SUPPLIES_LASTPOWEROUTPUT, NULL, NULL, NULL}},
        {2, {RFPROP_CHASSIS_ENCLOSURE_POWERSUPPLIES, RFPROP_POWER_SUPPLIES_INPUT_VOLTAGE, NULL, NULL, NULL}},
        {2, {RFPROP_CHASSIS_ENCLOSURE_POWERSUPPLIES, RFPROP_POWER_SUPPLIES_INPUT_POWER_OEM_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_CHASSIS_ENCLOSURE_POWERSUPPLIES, RFPROP_POWER_SUPPLIES_INPUT_CURRENT_OEM_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_CHASSIS_ENCLOSURE_POWERSUPPLIES, RFPROP_POWER_SUPPLIES_OUTPUT_POWER_OEM_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_CHASSIS_ENCLOSURE_POWERSUPPLIES, RFPROP_POWER_SUPPLIES_OUTPUT_CURRENT_OEM_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_CHASSIS_ENCLOSURE_POWERSUPPLIES, RFPROP_POWER_SUPPLIES_OUTPUT_VOLTAGE_OEM_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_VOLTAGES, RFPROP_SENSOR_READINGVOLTS, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_CONTROL, RFPROP_POWER_CONTROL_POWER_CONSUME, NULL, NULL, NULL }},
        {2, {RFPROP_POWER_CONTROL, RFPROP_POWER_CONTROL_STATIS_COLL_PATH, NULL, NULL, NULL}},
        {2, {RFPROP_POWER_CONTROL, RFPROP_POWER_CONTROL_MAX_CONSUME_OCC_PATH, NULL, NULL, NULL}},
        { 2, { RFPROP_POWER_CONTROL, RFPROP_POWER_DROP_STAMP, NULL, NULL, NULL } },
        { 2, { RFPROP_POWER_CONTROL, RFPROP_POWER_CAPACITY_WATTS, NULL, NULL, NULL } },
        { 2, { RFPROP_POWER_CONTROL, RFPROP_POWER_AVAILABLE_WATTS, NULL, NULL, NULL } },
        { 4, { RFPROP_POWER_CONTROL, RFPROP_OEM, RFPROP_OEM_HUAWEI, RFPROP_POWER_LIMIT_VALUE_RANGE, NULL } },
        { 4, { RFPROP_POWER_CONTROL, RFPROP_OEM, RFPROP_OEM_HUAWEI, RFPROP_POWER_LIMIT_ACTIVATED, NULL } },
        { 4, { RFPROP_POWER_CONTROL, RFPROP_OEM, RFPROP_OEM_HUAWEI, RFPROP_POWER_LIMIT_BLADE_INFO, NULL } },
        {0, {NULL, NULL, NULL, NULL, NULL}}
    };

    return rsc_del_none_etag_affected_property(object, value, G_N_ELEMENTS(value));
}


gint32 chassis_enclosure_power_provider_entry(PROVIDER_PARAS_S* i_paras,
    PROPERTY_PROVIDER_S** prop_provider, guint32* count)
{
    if (!redfish_check_smm_chassis_uri_valid(i_paras->uri)) {
        return HTTP_NOT_FOUND;
    }
    
    if (dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_MGNT)) {
        return chassis_enc_power_provider_entry(i_paras, prop_provider, count);
    }

    *prop_provider = g_chassis_enclosure_power_provider;
    *count = sizeof(g_chassis_enclosure_power_provider) / sizeof(PROPERTY_PROVIDER_S);
    
    return VOS_OK;
}
