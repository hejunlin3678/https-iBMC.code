
#include "redfish_provider.h"
#include "redfish_provider_chassis.h"

#define MAX_CHASSIS_NAME_LEN 20
#define MAX_CHASSIS_LOCATION_LEN 20

gint32 get_fru_elabel_handle(guint32 position, OBJ_HANDLE *elabel_handle);

LOCAL gint32 get_chassis_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_chassis_description(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 smm_get_chassis_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 smm_get_chassis_indicator_led(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_set_chassis_indicator_led(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 smm_get_chassis_part_number(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_hmm_hdd_drawer_link(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_get_chassis_serial_number(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_get_chassis_model(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 smm_get_chassis_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 smm_get_chassis_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 add_managed_state(json_object *huawei_obj);
LOCAL gint32 add_chassis_info(json_object *huawei_obj);
LOCAL gint32 add_backplane_version_info(json_object *huawei_obj);

LOCAL gint32 smm_set_chassis_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_chassis_manage_state(PROVIDER_PARAS_S *i_paras, json_object *o_message_jso, json_object *huawei_obj);
LOCAL gint32 get_chassis_oem_lanswitch_summary(json_object **oem_tem);
LOCAL gint32 add_chassis_oem_lanswitch_collection(json_object *huawei_obj);


LOCAL PROPERTY_PROVIDER_S chassis_enclosure_provider[] = {
    { RFPROP_CHASSIS_STATUS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_chassis_status, NULL, NULL, ETAG_FLAG_DISABLE },
    { RF_PROPERTY_NAME,         MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_name, NULL, NULL, ETAG_FLAG_DISABLE },
    { RF_PROPERTY_DESCRIPTION,         MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_description, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_CHASSIS_INDICATELED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, smm_get_chassis_indicator_led, smm_set_chassis_indicator_led, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_CHASSIS_HEALTH_LED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        smm_get_chassis_indicator_led, smm_set_chassis_indicator_led, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_CHASSIS_PN, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_chassis_part_number, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_CHASSIS_SN, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_chassis_serial_number, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_CHASSIS_MODEL, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_chassis_model, NULL, NULL, ETAG_FLAG_ENABLE },
    { RF_CHASSIS_HDD_DRAWER,    MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_hmm_hdd_drawer_link, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, smm_get_chassis_oem, smm_set_chassis_oem, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_LINKS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_chassis_links, NULL, NULL, ETAG_FLAG_ENABLE}
};

gint32 get_fru_elabel_handle(guint32 position, OBJ_HANDLE *elabel_handle)
{
    gint32 ret;
    OBJ_HANDLE fru_handle = 0;

    
    ret = dal_get_specific_object_uint32(CLASS_FRU, PROPERTY_POSITION, position, &fru_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get class(%s) object by property(name:%s, value:%u) failed, err code %d", __func__,
            CLASS_FRU, PROPERTY_POSITION, position, ret);
        return VOS_ERR;
    }
    
    ret = dfl_get_referenced_object(fru_handle, PROPERTY_FRU_ELABEL_RO, elabel_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get elabel by reference failed, err code %d", __func__, ret);
        return VOS_ERR;
    }
    return VOS_OK;
}


LOCAL gint32 get_chassis_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    return_val_do_info_if_fail(
        (o_result_jso != NULL && o_message_jso != NULL && provider_paras_check(i_paras) == VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    if (dal_match_product_id(PRODUCT_ID_TCE)) {
        *o_result_jso = json_object_new_string("TCE");
        return HTTP_OK;
    }
    
    OBJ_HANDLE obj = 0;
    gchar str_name[MAX_STRBUF_LEN] = {0};
    gint32 ret = dfl_get_object_handle(CLASS_NAME_CHASSIS, &obj);
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = dal_get_property_value_string(obj, PROPERTY_CHASSIS_CUSTOM_DATA1, str_name, sizeof(str_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get custom data failed, ret %d", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (strlen(str_name) != 0) {
        *o_result_jso = json_object_new_string(str_name);
    }
    
    return HTTP_OK;
}


LOCAL gint32 get_chassis_description(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    return_val_do_info_if_fail(
        (o_result_jso != NULL && o_message_jso != NULL && provider_paras_check(i_paras) == VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    if (dal_match_product_id(PRODUCT_ID_TCE)) {
        *o_result_jso = json_object_new_string(
            "The TCE chassis includes the management modules, switch modules, compute nodes, PSUs, and fan modules.");
        return HTTP_OK;
    }

    OBJ_HANDLE obj = 0;
    gchar str_name[MAX_BUF_LEN] = {0};
    gchar tmp[MAX_STRBUF_LEN] = {0};
    gint32 ret = dfl_get_object_handle(CLASS_NAME_CHASSIS, &obj);
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = dal_get_property_value_string(obj, PROPERTY_CHASSIS_CUSTOM_DATA1, tmp, sizeof(tmp));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get custom data failed, ret %d", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (strlen(tmp) != 0) {
        ret = snprintf_s(str_name, sizeof(str_name), sizeof(str_name) - 1,
            "The %s chassis includes the management modules, switch modules,"
            "compute nodes, PSUs, and fan modules.", tmp);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "snprintf_s failed, ret %d", ret);
        }
        *o_result_jso = json_object_new_string(str_name);
    }
    return HTTP_OK;
}


LOCAL gint32 smm_get_chassis_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guint8 chassis_health = 0;
    gchar *chassis_state = NULL;
    OBJ_HANDLE handle = 0;
    gint32 ret;
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "[%s] NULL pointer.", __func__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    chassis_state = ENABLED_STRING;
    
    ret = dfl_get_object_handle(OBJ_NAME_SHELF_MANAGE, &handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get object[%s] failed, err code %d", __func__, OBJ_NAME_SHELF_MANAGE, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    (void)dal_get_property_value_byte(handle, "Health", &chassis_health);
    get_resource_status_property(&chassis_health, NULL, chassis_state, o_result_jso, TRUE);
    return HTTP_OK;
}


LOCAL gint32 smm_get_chassis_indicator_led(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    
    if (dal_get_uid_location() == UID_LOCATION_CHASSIS) {
        return get_indicator_led_status(i_paras, o_message_jso, o_result_jso);
    } else if (dal_get_uid_location() == UID_MAINBOARD_AND_CHASSIS) {
        return get_health_and_uid_status(get_self_provider_service(i_paras, chassis_enclosure_provider,
            G_N_ELEMENTS(chassis_enclosure_provider)), i_paras, o_message_jso, o_result_jso);
    }
    
    return HTTP_OK;
}

LOCAL gint32 smm_set_chassis_indicator_led(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    
    if (dal_get_uid_location() == UID_LOCATION_CHASSIS) {
        return set_indicator_led_status(i_paras, o_message_jso, o_result_jso);
    } else if (dal_get_uid_location() == UID_MAINBOARD_AND_CHASSIS) {
        return set_health_and_uid_status(get_self_provider_service(i_paras, chassis_enclosure_provider,
            G_N_ELEMENTS(chassis_enclosure_provider)), i_paras, o_message_jso, o_result_jso);
    }
    
    return HTTP_BAD_REQUEST;
}


LOCAL gint32 smm_get_chassis_model(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    GVariant *modelVar = NULL;
    const gchar *modelStr = NULL;
    gsize modelLen = 0;
    OBJ_HANDLE objHandle = 0;
    gint32 ret;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] NULL pointer.", __func__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = dfl_get_object_handle(CLASS_NAME_CHASSIS, &objHandle);
    return_val_do_info_if_expr(ret != DFL_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "[%s]: Get Chassis object failed, ret=%d", __func__, ret));
    ret = dfl_get_property_value(objHandle, PROPERTY_CHASSIS_CUSTOM_DATA1, &modelVar);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s]Get chassis model [CustomData1] failed, err code %d", __func__, ret);
        *o_result_jso = NULL;
        return HTTP_OK;
    }
    modelStr = g_variant_get_string(modelVar, &modelLen);
    *o_result_jso = NULL;
    if (modelLen != 0) {
        *o_result_jso = json_object_new_string(modelStr);
    }
    g_variant_unref(modelVar);
    return HTTP_OK;
}


LOCAL gint32 get_hmm_hdd_drawer_link(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    if (dal_is_compute_storage_smm_board() == FALSE) {
        return HTTP_OK;
    }

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    *o_result_jso = json_object_new_object();
    json_object_object_add(*o_result_jso, RFPROP_ODATA_ID,
        json_object_new_string("/redfish/v1/Chassis/Enclosure/HDDdrawer"));
    return HTTP_OK;
}


LOCAL gint32 smm_get_chassis_part_number(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE elabel_handle = 0;
    GVariant *pn_var = NULL;
    const gchar *pn_str = NULL;
    gint32 ret;
    gsize pn_len = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "[%s] NULL pointer.", __func__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = get_fru_elabel_handle(SMM_BACK_PLANE_POSITION, &elabel_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get backplane elabel object handle failed, err code %d", __func__, ret);
        *o_result_jso = NULL;
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    ret = dfl_get_property_value(elabel_handle, PROPERTY_ELABEL_CHASSIS_PN, &pn_var);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get backplane elabel property[ChassisPartNumber] failed, err code %d", __func__,
            ret);
        *o_result_jso = NULL;
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    pn_str = g_variant_get_string(pn_var, &pn_len);
    *o_result_jso = NULL;
    if (pn_len != 0) {
        *o_result_jso = json_object_new_string(pn_str);
    }
    g_variant_unref(pn_var);
    return HTTP_OK;
}

LOCAL gint32 get_sn_from_group_storage(gchar* sn_str, guint32 str_len)
{
    OBJ_HANDLE object_handle = 0;
    gint32 ret = dal_get_specific_object_uint32(CLASS_EEPROM_STORAGE_GROUP_NAME, PROPERTY_EEP_GROUP_ID,
        CHASSIS_SERIAL_NUM, &object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get group storage handle failed, ret[%d]", ret);
        return ret;
    }

    guint16 version = 0;
    ret = dal_get_property_value_uint16(object_handle, PROPERTY_EEP_GROUP_DATA_VER, &version);
    if (ret != RET_OK || version == 0) { 
        debug_log(DLOG_ERROR, "Get %s property %s failed or version is invalid ret[%d].version[%d]",
            dfl_get_object_name(object_handle), PROPERTY_EEP_GROUP_DATA_VER, ret, version);
        return RET_ERR;
    }

    guint8 arry_value[MAX_ARRAY_LEN] = {0};
    gsize lens = 0;
    ret = dal_get_property_value_bytev(object_handle, PROPERTY_EEP_GROUP_VALUE, arry_value,
        sizeof(arry_value), &lens);
    if (ret != RET_OK || lens > MAX_ARRAY_LEN) {
        debug_log(DLOG_ERROR, "Get bayconfig value failed, ret[%d]", ret);
        return ret;
    }

    for (guint32 i = 0; i < lens; i++) {
        if (arry_value[i] < 32 || arry_value[i] > 126) { 
            arry_value[i] = '?'; 
        }
        ret = snprintf_s(sn_str + strlen(sn_str), str_len, str_len - 1, "%c", arry_value[i]);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "sprintf_s fail, ret[%d]", ret);
            return ret;
        }
    }
    return RET_OK;
}

LOCAL gint32 get_elabel_serial_num(gchar* sn_str, guint32 str_len)
{
    
    OBJ_HANDLE elabel_handle = 0;
    gint32 ret = get_fru_elabel_handle(SMM_BACK_PLANE_POSITION, &elabel_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get backplane elabel object handle failed, ret[%d]", ret);
        return ret;
    }
    
    ret = dal_get_property_value_string(elabel_handle, PROPERTY_ELABEL_CHASSIS_SN, sn_str, str_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get backplane elabel property[ChassisSerialNumber] failed, ret[%d]", ret);
    }

    return ret;
}

LOCAL gint32 smm_get_chassis_serial_number(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar sn_str[MAX_STRBUF_LEN] = {0};
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "[%s] NULL pointer.", __func__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    gint32 ret = get_sn_from_group_storage(sn_str, sizeof(sn_str));
    if (ret == RET_OK) {
        *o_result_jso = json_object_new_string(sn_str);
        return HTTP_OK;
    }

    ret = get_elabel_serial_num(sn_str, sizeof(sn_str));
    if (ret != RET_OK) {
        *o_result_jso = NULL;
        return HTTP_INTERNAL_SERVER_ERROR;
    }
   
    *o_result_jso = json_object_new_string(sn_str);

    return HTTP_OK;
}

LOCAL gint32 add_managed_state(json_object *huawei_obj)
{
    OBJ_HANDLE handle = 0;
    gint32 ret;
    guint8 is_managed = 0;
    json_object *obj = NULL;
    
    ret = dfl_get_object_handle(OBJ_NAME_SHELF_MANAGE, &handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get object %s handle failed, err code %d", __func__, OBJ_NAME_SHELF_MANAGE, ret);
        json_object_object_add(huawei_obj, RFPROP_CHASSIS_IS_MANAGED, NULL);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    (void)dal_get_property_value_byte(handle, PROTERY_SHELF_MANAGEMENT_STATE, &is_managed);
    obj = json_object_new_boolean(is_managed);
    json_object_object_add(huawei_obj, RFPROP_CHASSIS_IS_MANAGED, obj);
    return VOS_OK;
}

LOCAL gint32 add_chassis_info(json_object *huawei_obj)
{
    OBJ_HANDLE handle = 0;
    gint32 ret;
    guint32 chassis_id = 0;
    gchar chassis_id_str[RF_CHASSIS_ID_MAX_LEN_VALUE + 1] = { 0 };
    gchar chassis_name_str[RF_CHASSIS_NAME_MAX_LEN + 1] = { 0 };
    gchar chassis_location_str[RF_CHASSIS_LOCATION_MAX_LEN + 1] = { 0 };
    json_object *obj = NULL;
    
    ret = dfl_get_object_handle(OBJECT_NAME_CHASSIS, &handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get object %s handle failed, err code %d", __func__, OBJECT_NAME_CHASSIS, ret);
        json_object_object_add(huawei_obj, RFPROP_CHASSIS_ID, NULL);
        json_object_object_add(huawei_obj, RFPROP_CHASSIS_NAME, NULL);
        json_object_object_add(huawei_obj, RFPROP_CHASSIS_LOCATION, NULL);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    (void)dal_get_property_value_uint32(handle, PROPERTY_CHASSIS_CHASSIS_NUM, &chassis_id);
    (void)snprintf_s(chassis_id_str, sizeof(chassis_id_str), sizeof(chassis_id_str) - 1, "%u", chassis_id);
    
    (void)dal_get_property_value_string(handle, PROPERTY_CHASSIS_CHASSIS_NAME, chassis_name_str,
        sizeof(chassis_name_str));
    
    (void)dal_get_property_value_string(handle, PROPERTY_CHASSIS_LOCATION, chassis_location_str,
        sizeof(chassis_location_str));
    
    
    obj = json_object_new_string(chassis_id_str);
    json_object_object_add(huawei_obj, RFPROP_CHASSIS_ID, obj);
    
    obj = json_object_new_string(chassis_name_str);
    json_object_object_add(huawei_obj, RFPROP_CHASSIS_NAME, obj);
    
    obj = json_object_new_string(chassis_location_str);
    json_object_object_add(huawei_obj, RFPROP_CHASSIS_LOCATION, obj);
    return VOS_OK;
}


LOCAL gint32 add_backplane_version_info(json_object *huawei_obj)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    gchar pcb_ver[RF_BACKPLANE_PCB_VER_MAX_LEN] = {0};
    guint8 type = 0;
    json_object *obj = NULL;
    

    ret = dfl_get_object_list(CLASS_FRU, &obj_list);
    if (VOS_OK != ret || 0 == g_slist_length(obj_list)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_BACKPLANEBOARD_TYPE, &type);

        
        if (COMPONENT_TYPE_BACKPLANE_BOARD == type) {
            (void)dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_BACKPLANEBOARD_PCBVER, pcb_ver,
                sizeof(pcb_ver));
            break;
        }
    }

    g_slist_free(obj_list);

    obj = json_object_new_string(pcb_ver);
    json_object_object_add(huawei_obj, RFPROP_CHASSIS_BACKPLANE_PCB_VERSION, obj);
    return VOS_OK;
}


gint32 add_chassis_fru(json_object *huawei_obj)
{
    OBJ_HANDLE elabel_handle = 0;
    gint32 ret;
    json_object *fru_obj = NULL;
    json_object *obj = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    const gchar *value_str = NULL;
    json_object *value_obj = NULL;
    gsize value_str_len = 0;
    
    ret = get_fru_elabel_handle(SMM_BACK_PLANE_POSITION, &elabel_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get backplane elabel object failed, err code %d", __func__, ret);
        json_object_object_add(huawei_obj, "Fru", NULL);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_CHASSIS_TYPE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_BOARD_PN);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_BOARD_SN);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_BOARD_MFG_TIME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_BOARD_MFG_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_BOARD_PRO_NAME);
    ret = dfl_multiget_property_value(elabel_handle, property_name_list, &property_value_list);
    g_slist_free(property_name_list);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "[%s] Get backplane fru info failed, err code %d", __func__, ret);
        json_object_object_add(huawei_obj, "Fru", NULL);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    NEW_JSON_OBJ(fru_obj, return HTTP_INTERNAL_SERVER_ERROR);
    
    NEW_JSON_OBJ(obj, return HTTP_INTERNAL_SERVER_ERROR);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 0), &value_str_len);
    value_obj = json_object_new_string(value_str);
    json_object_object_add(obj, "Type", value_obj);
    
    json_object_object_add(fru_obj, "Chassis", obj);
    
    NEW_JSON_OBJ(obj, return HTTP_INTERNAL_SERVER_ERROR);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 1), &value_str_len);
    value_obj = json_object_new_string(value_str);
    json_object_object_add(obj, RFPROP_CHASSIS_PN, value_obj);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 2), &value_str_len);
    value_obj = json_object_new_string(value_str);
    json_object_object_add(obj, RFPROP_CHASSIS_SN, value_obj);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 3), &value_str_len);
    value_obj = json_object_new_string(value_str);
    json_object_object_add(obj, RFPROP_CHASSIS_MF_DATE, value_obj);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 4), &value_str_len);
    value_obj = json_object_new_string(value_str);
    json_object_object_add(obj, RFPROP_CHASSIS_MFER, value_obj);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 5), &value_str_len);
    value_obj = json_object_new_string(value_str);
    json_object_object_add(obj, RFRPOP_CHASSIS_PRODUCT_NAME, value_obj);
    
    json_object_object_add(fru_obj, "Board", obj);
    g_slist_free_full(property_value_list, (GDestroyNotify)g_variant_unref);
    
    json_object_object_add(huawei_obj, RFPROP_CHASSIS_FRU, fru_obj);
    return VOS_OK;
}


LOCAL gint32 add_chassis_device_max_num(json_object **max_num_jso)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    guint8 ps_num;
    guint8 fan_num;
    guint32 cnt;
    gint32 ret;
    gchar *property_compoent_list[] = {PROPERTY_COMPOENT_PS_NUM, PROPERTY_COMPOENT_FAN_NUM};

    
    if (max_num_jso == NULL) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_object_handle_nth(CLASS_PRODUCT_COMPONENT, 0, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s, get o_obj_handle fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    for (cnt = 0; cnt < sizeof(property_compoent_list) / sizeof(gchar*); cnt++) {
        property_name_list = g_slist_append(property_name_list, property_compoent_list[cnt]);
    }

    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value_list);
    g_slist_free(property_name_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s, get property failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ps_num = g_variant_get_byte((GVariant*)g_slist_nth_data(property_value_list, 0));
    fan_num = g_variant_get_byte((GVariant*)g_slist_nth_data(property_value_list, 1));
    g_slist_free_full(property_value_list, (GDestroyNotify)g_variant_unref);
    *max_num_jso = json_object_new_object();
    if (*max_num_jso == NULL) {
        debug_log(DLOG_ERROR, "alloc new json object failed");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object_object_add(*max_num_jso, RFPROP_POWERSUPPLY_NUM, json_object_new_int(ps_num));
    json_object_object_add(*max_num_jso, RFPROP_FAN_NUM, json_object_new_int(fan_num));
    return HTTP_OK;
}


LOCAL gint32 get_board_manufacturer_value(gchar *manufacturer, gsize name_len)
{
    gint32 ret;
    OBJ_HANDLE elabel_handle = 0;
    OBJ_HANDLE fru_handle = 0;
    gchar manu_str[PROP_VAL_LENGTH];

    
    ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, 0, &fru_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dal_get_specific_object_byte(%s) failed: ret %d.", CLASS_FRU, ret);
        return RET_ERR;
    }
    
    
    ret = dfl_get_referenced_object(fru_handle, PROPERTY_FRU_ELABEL_RO, &elabel_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_referenced_object(%s) handle failed: ret %d.", PROPERTY_FRU_ELABEL_RO, ret);
        return RET_ERR;
    }

    
    ret = dal_get_property_value_string(elabel_handle, PROPERTY_ELABEL_BOARD_MFG_NAME, manu_str, sizeof(manu_str));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_property_value failed: ret %d.", ret);
        return RET_ERR;
    }
    ret = strcpy_s(manufacturer, name_len, manu_str);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "strcpy_s manufacturername failed.");
        return RET_ERR;
    }

    return ret;
}


LOCAL gint32 get_boardname_sn_value(gchar* boardname, gsize name_len, gchar* sn, gsize sn_len)
{
    gint32 ret;
    OBJ_HANDLE elabel_handle = 0;
    OBJ_HANDLE fru_handle = 0;
    GSList* property_name_list = NULL;
    GSList* property_value_list = NULL;
    gsize value_str_len = 0;
    const gchar* name_value = NULL;
    const gchar* sn_value = NULL;

    
    ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, 0, &fru_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dal_get_specific_object_byte(%s) failed: ret %d.", CLASS_FRU, ret);
        return RET_ERR;
    }

    
    ret = dfl_get_referenced_object(fru_handle, PROPERTY_FRU_ELABEL_RO, &elabel_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_referenced_object(%s) handle failed: ret %d.", PROPERTY_FRU_ELABEL_RO, ret);
        return RET_ERR;
    }

    
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_BOARD_PRO_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_ELABEL_BOARD_SN);
    ret = dfl_multiget_property_value(elabel_handle, property_name_list, &property_value_list);
    g_slist_free(property_name_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s]dfl_multiget_property_value failed: ret %d.", __FUNCTION__, ret);
        return RET_ERR;
    }
    
    name_value = g_variant_get_string((GVariant*) g_slist_nth_data(property_value_list, 0), &value_str_len);
    
    sn_value = g_variant_get_string((GVariant*) g_slist_nth_data(property_value_list, 1), &value_str_len);
    ret = RET_OK;
    if ((name_value == NULL) || (sn_value == NULL)) {
        debug_log(DLOG_ERROR, "%s: name or sn value is NULL.", __FUNCTION__);
        ret = RET_ERR;
    } else {
        if ((strcpy_s(boardname, name_len, name_value) != EOK) ||
            (strcpy_s(sn, sn_len, sn_value) != EOK)) {
            ret = RET_ERR;
            debug_log(DLOG_ERROR, "%s: strcpy_s failed.", __FUNCTION__);
        }
    }

    g_slist_free_full(property_value_list, (GDestroyNotify)g_variant_unref);
    property_value_list = NULL;

    return ret;
}


LOCAL gint32 add_chassis_oem_mainboard(json_object* huawei_obj)
{
    gint32 ret;
    json_object* oem_mianboard = NULL;
    gchar name_value[FRU_DATA_STRING_LEN] = {0};
    gchar sn_value[FRU_DATA_STRING_LEN] = {0};
    gchar facturer_name[FRU_DATA_STRING_LEN] = {0};

    
    oem_mianboard = json_object_new_object();
    if (oem_mianboard == NULL) {
        debug_log(DLOG_ERROR, "%s: new object fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = get_boardname_sn_value(name_value, FRU_DATA_STRING_LEN, sn_value, FRU_DATA_STRING_LEN);
    if (ret != RET_OK) {
        json_object_put(oem_mianboard);
        debug_log(DLOG_ERROR, "%s: get board or sn value failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (check_string_val_effective((const gchar*)name_value) != VOS_OK) {
        json_object_object_add(oem_mianboard, RFPROP_BOARD_BOARDNAME, NULL);
    } else {
        json_object_object_add(oem_mianboard, RFPROP_BOARD_BOARDNAME, json_object_new_string(name_value));
    }

    if (check_string_val_effective((const gchar*)sn_value) != VOS_OK) {
        json_object_object_add(oem_mianboard, RFPROP_SERIAL_NUMBER, NULL);
    } else {
        json_object_object_add(oem_mianboard, RFPROP_SERIAL_NUMBER, json_object_new_string(sn_value));
    }

    ret = get_board_manufacturer_value(facturer_name, FRU_DATA_STRING_LEN);
    if (ret != RET_OK || check_string_val_effective((const gchar*)facturer_name) != RET_OK) {
        json_object_object_add(oem_mianboard, RFPROP_SYSTEM_MANUFACTURER, NULL);
    } else {
        json_object_object_add(oem_mianboard, RFPROP_SYSTEM_MANUFACTURER, json_object_new_string(facturer_name));
    }

    json_object_object_add(huawei_obj, RFPROP_CHASSIS_MAINBOARD, oem_mianboard);

    return HTTP_OK;
}


LOCAL gint32 smm_get_chassis_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    json_object *huawei_obj = NULL;
    json_object *value_obj = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "[%s] NULL pointer.", __func__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    *o_result_jso = json_object_new_object();
    huawei_obj = json_object_new_object();

    
    (void)add_managed_state(huawei_obj);
    
    (void)add_chassis_info(huawei_obj);
    
    (void)add_backplane_version_info(huawei_obj);
    
    (void)get_chassis_oem_powersupply_summary(&value_obj);
    json_object_object_add(huawei_obj, RFPROP_CHASSIS_POWERSUPPLY_SUMMARY, value_obj);
    value_obj = NULL;
    
    (void)add_chassis_fru(huawei_obj);

    
    (void)add_chassis_oem_lanswitch_collection(huawei_obj);

    
    (void)add_chassis_device_max_num(&value_obj);
    if (value_obj != NULL) {
        json_object_object_add(huawei_obj, RFPROP_CHASSIS_MAX_NUM, value_obj);
    }
    value_obj = NULL;
    (void)add_chassis_oem_mainboard(huawei_obj);

    
    json_object_object_add(*o_result_jso, RFPROP_OEM_HUAWEI, huawei_obj);
    return HTTP_OK;
}


LOCAL gint32 add_chassis_oem_lanswitch_collection(json_object *huawei_obj)
{
    gint32 ret;
    json_object *value_obj = NULL;
    OBJ_HANDLE handle = 0;

    
    ret = dfl_get_object_handle(OBJ_LSW_PUBLIC_CENTER_CP, &handle);
    return_val_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR);

    (void)get_chassis_oem_lanswitch_summary(&value_obj);
    json_object_object_add(huawei_obj, RFPROP_CHASSIS_LANSWITCH_SUMMARY, value_obj);

    return VOS_OK;
}


LOCAL gint32 get_chassis_oem_lanswitch_summary(json_object **oem_tem)
{
    return_val_do_info_if_expr(oem_tem == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, NULL pointer.", __FUNCTION__));

    
    *oem_tem = json_object_new_object();
    json_object_object_add(*oem_tem, RFPROP_ODATA_ID, json_object_new_string(URI_FORMAT_LANSWITCH_COLLECTION));
    return HTTP_OK;
}


LOCAL gint32 smm_get_chassis_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    json_object *managed_by = NULL;
    json_object *obj = NULL;
    gchar odata_id[MAX_URI_LEN] = { 0 };
    guint8 smm_index = 0;
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "[%s] NULL pointer.", __func__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    *o_result_jso = json_object_new_object();
    managed_by = json_object_new_array();
    obj = json_object_new_object();
    get_smm_index(&smm_index);
    (void)snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, "/redfish/v1/Managers/HMM%u", smm_index);
    json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string(odata_id));
    json_object_array_add(managed_by, obj);
    json_object_object_add(*o_result_jso, REDFISH_CHASSIS_MANAGEDBY, managed_by);
    return HTTP_OK;
}

LOCAL gint32 set_chassis_manage_state(PROVIDER_PARAS_S *i_paras, json_object *o_message_jso, json_object *huawei_obj)
{
    json_bool bret;
    gint32 ret;
    json_object *obj = NULL;
    OBJ_HANDLE handle = 0;
    GSList *input_list = NULL;
    json_bool state;

    bret = json_object_object_get_ex(huawei_obj, RFPROP_CHASSIS_IS_MANAGED, &obj);
    if (!bret) { // 如果不包含管理状态设置
        return HTTP_BAD_REQUEST;
    }
    ret = dfl_get_object_handle(OBJ_NAME_SHELF_MANAGE, &handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get object %s handle failed, err code %d", __func__, OBJ_NAME_SHELF_MANAGE, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    state = json_object_get_boolean(obj) ? TRUE : FALSE; // Codex
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)state));
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, handle,
        CLASS_NAME_SHELF_MANAGE, METHOD_SHELF_SET_SHELF_MANAGEMENT_STATE, AUTH_DISABLE, i_paras->user_role_privilege,
        input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Call class(%s) method(%s) failed, err code %d", __func__, CLASS_NAME_SHELF_MANAGE,
            METHOD_SHELF_SET_SHELF_MANAGEMENT_STATE, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return VOS_OK;
}


LOCAL guint32 set_chassis_num(PROVIDER_PARAS_S* i_paras, json_object* o_message_jso, json_object *huawei_obj)
{
    guint bret;
    gint32 ret;
    OBJ_HANDLE handle = 0;
    guint32 chassis_id;
    json_object *id_obj = NULL;
    json_object *msg_obj = NULL;
    GSList *input_list = NULL;

    bret = (guint)json_object_object_get_ex(huawei_obj, RFPROP_CHASSIS_ID, &id_obj);
    if (!bret || id_obj == NULL) { // 不设置机框信息
        return HTTP_BAD_REQUEST;
    }

    
    ret = dfl_get_object_handle(OBJECT_NAME_CHASSIS, &handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] Get object %s handle failed, err code %d", __func__, OBJECT_NAME_CHASSIS, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    chassis_id = arith_str_to_int(dal_json_object_get_str(id_obj));
    if (chassis_id > RF_MAX_CHASSIS_ID) {
        create_message_info(MSGID_PROP_VALUE_OUTOFRANGE, "Oem/Huawei/ChassisID", &msg_obj,
            dal_json_object_get_str(id_obj), RFPROP_CHASSIS_ID, "0", "999999");
        json_object_array_add(o_message_jso, msg_obj);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32(chassis_id));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(0));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                                        handle, CLASS_NAME_CHASSIS, METHOD_CHASSIS_SET_CHASSIS_NUMBER,
                                        AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Call class(%s) method(%s) failed, err code %d",
            __func__, CLASS_NAME_CHASSIS, METHOD_CHASSIS_SET_CHASSIS_NUMBER, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL guint32 set_chassis_name(PROVIDER_PARAS_S* i_paras, json_object* o_message_jso, json_object *huawei_obj)
{
    guint bret;
    gint32 ret;
    OBJ_HANDLE handle = 0;
    json_object *name_obj = NULL;
    const gchar *chassis_name = NULL;
    json_object *msg_obj = NULL;
    GSList *input_list = NULL;

    bret = (guint)json_object_object_get_ex(huawei_obj, RFPROP_CHASSIS_NAME, &name_obj);
    if (!bret || name_obj == NULL) { // 不设置机框信息
        return HTTP_BAD_REQUEST;
    }

    
    ret = dfl_get_object_handle(OBJECT_NAME_CHASSIS, &handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] Get object %s handle failed, err code %d", __func__, OBJECT_NAME_CHASSIS, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    chassis_name = dal_json_object_get_str(name_obj);
    if (strlen(chassis_name) > MAX_CHASSIS_NAME_LEN) {
        create_message_info(MSGID_VALUE_OUT_OF_RANGE, "Oem/Huawei/ChassisName", &msg_obj, "ChassisName");
        json_object_array_add(o_message_jso, msg_obj);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(chassis_name));
    input_list = g_slist_append(input_list, g_variant_new_byte(0));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                                        handle, CLASS_NAME_CHASSIS, METHOD_CHASSIS_SET_CHASSIS_NAME,
                                        AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Call class(%s) method(%s) failed, err code %d",
            __func__, CLASS_NAME_CHASSIS, METHOD_CHASSIS_SET_CHASSIS_NAME, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL guint32 set_chassis_location(PROVIDER_PARAS_S* i_paras, json_object* o_message_jso, json_object *huawei_obj)
{
    guint bret;
    gint32 ret;
    OBJ_HANDLE handle = 0;
    json_object *location_obj = NULL;
    const gchar *chassis_location = NULL;
    gchar location_str[MAX_STRBUF_LEN] = { 0 };
    json_object *msg_obj = NULL;
    GSList *input_list = NULL;
    errno_t safe_fun_ret;

    bret = (guint)json_object_object_get_ex(huawei_obj, RFPROP_CHASSIS_LOCATION, &location_obj);
    if (!bret || location_obj == NULL) { // 不设置机框信息
        return HTTP_BAD_REQUEST;
    }

    
    ret = dfl_get_object_handle(OBJECT_NAME_CHASSIS, &handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] Get object %s handle failed, err code %d", __func__, OBJECT_NAME_CHASSIS, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    chassis_location = dal_json_object_get_str(location_obj);
    // 检查是否有非ascii码时，只能传入非const型
    safe_fun_ret = strncpy_s(location_str, sizeof(location_str), chassis_location, sizeof(location_str) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    if (strlen(chassis_location) > MAX_CHASSIS_LOCATION_LEN) {
        create_message_info(MSGID_VALUE_OUT_OF_RANGE, "Oem/Huawei/ChassisLocation", &msg_obj,
            "Oem/Huawei/ChassisLocation");
        json_object_array_add(o_message_jso, msg_obj);
        return HTTP_BAD_REQUEST;
    }
    if (dal_check_string_is_valid_ascii(location_str) != VOS_OK) { // 机框位置不支持非ascii码，上层网管会出错
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, "Oem/Huawei/ChassisLocation", &msg_obj,
            chassis_location, "Oem/Huawei/ChassisLocation");
        json_object_array_add(o_message_jso, msg_obj);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(chassis_location));
    input_list = g_slist_append(input_list, g_variant_new_byte(1)); 
    input_list = g_slist_append(input_list, g_variant_new_byte(1)); 
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                                        handle, CLASS_NAME_CHASSIS, METHOD_CHASSIS_SET_CHASSIS_LOCATION,
                                        AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Call class(%s) method(%s) failed, err code %d",
            __func__, CLASS_NAME_CHASSIS, METHOD_CHASSIS_SET_CHASSIS_LOCATION, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

/*****************************************************************************
* 函 数 名  : set_chassis_info
* 功能描述  : 设置机框信息
* 1.日    期   : 2018年1月9日
*   作    者   : l00422028
*   修改内容   : 新生成函数
* 2.日    期   : 2020年9月10日
*   作    者   : l00574725
*   修改内容   : 函数拆分，添加适配大西洋直通板接口
*****************************************************************************/
LOCAL gint32 set_chassis_info(PROVIDER_PARAS_S* i_paras, json_object* o_message_jso, json_object* huawei_obj)
{
    guint32 ret;

    if (dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_MGNT) == TRUE) {
        ret = set_huawei_chassis_id(i_paras, o_message_jso, huawei_obj);
    } else {
        ret = set_chassis_num(i_paras, o_message_jso, huawei_obj);
    }
    ret |= set_chassis_name(i_paras, o_message_jso, huawei_obj);
    ret |= set_chassis_location(i_paras, o_message_jso, huawei_obj);
    if ((ret & HTTP_OK) == HTTP_OK) { // 任何一个值返回成功则成功
        return HTTP_OK;
    }
    if ((ret & HTTP_INTERNAL_SERVER_ERROR) == HTTP_INTERNAL_SERVER_ERROR) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_BAD_REQUEST;
}


LOCAL gint32 smm_set_chassis_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 rf_ret = HTTP_BAD_REQUEST; // 任意一项设置成功，均返回成功
    json_object *huawei_obj = NULL;
    json_object *tmp_message = NULL;

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_OEM, o_message_jso, RFPROP_OEM);
        dal_proxy_operation_log(MODULE_NAME_REDFISH, i_paras->user_name, i_paras->client, CLASS_NAME_CHASSIS,
            "Set the chassis oem failed"));

    ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_OEM_HUAWEI, &huawei_obj);
    if (!ret) {
        debug_log(DLOG_ERROR, "[%s] Request body is not right", __func__);
        return HTTP_BAD_REQUEST;
    }
    NEW_JSON_ARR(*o_message_jso, return HTTP_INTERNAL_SERVER_ERROR);
    // 设置管理标志
    ret = set_chassis_manage_state(i_paras, *o_message_jso, huawei_obj);
    if (ret == VOS_OK) {
        rf_ret = HTTP_OK;
    } else if (ret == HTTP_INTERNAL_SERVER_ERROR) { // 内部错误直接返回
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &tmp_message);
        (void)json_object_array_add(*o_message_jso, tmp_message);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    // 设置机框信息
    ret = set_chassis_info(i_paras, *o_message_jso, huawei_obj);
    if (ret == HTTP_OK) {
        rf_ret = HTTP_OK;
    } else if (ret == HTTP_INTERNAL_SERVER_ERROR) { // 内部错误直接返回
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &tmp_message);
        (void)json_object_array_add(*o_message_jso, tmp_message);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return rf_ret;
}

gint32 smm_chassis_enclosure_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    if (!redfish_check_smm_chassis_uri_valid(i_paras->uri)) {
        return HTTP_NOT_FOUND;
    }
    *prop_provider = chassis_enclosure_provider;
    *count = sizeof(chassis_enclosure_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
