
#include "redfish_provider.h"

LOCAL gint32 smm_get_chassis_collection_members_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_get_chassis_collection_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);


LOCAL PROPERTY_PROVIDER_S smm_collection_chassis_provider[] = {
    { RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, smm_get_chassis_collection_members_count, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, smm_get_chassis_collection_members, NULL, NULL, ETAG_FLAG_ENABLE },
};

LOCAL gint32 add_blade_as_member(OBJ_HANDLE handle, gpointer data)
{
    int iRet;
    errno_t safe_fun_ret;
    json_object **o_result_jso = (json_object **)data;
    json_object *obj = NULL;
    const gchar *blade_name = NULL;
    gchar blade_name_f[MAX_NAME_SIZE] = { 0 };
    gchar chassis_uri[CHASSIS_URI_LEN] = { 0 };
    guint8 blade_type = 0;
    gint32 ret;

    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_BLADETYPE, &blade_type);
    if (blade_type == 0x03) { // 管理板
        return VOS_OK;
    }
    blade_name = dfl_get_object_name(handle);
    safe_fun_ret = strncpy_s(blade_name_f, sizeof(blade_name_f), blade_name, sizeof(blade_name_f) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    format_string_as_upper_head(blade_name_f, strlen(blade_name_f));
    obj = json_object_new_object();
    return_val_do_info_if_fail(NULL != obj, HTTP_INTERNAL_SERVER_ERROR, json_object_put(*o_result_jso);
        debug_log(DLOG_ERROR, "%s,%d:   memory alloc fail. ", __FUNCTION__, __LINE__));
    iRet = snprintf_s(chassis_uri, sizeof(chassis_uri), sizeof(chassis_uri) - 1, URI_FORMAT_CHASSIS, blade_name_f);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string(chassis_uri));
    ret = json_object_array_add(*o_result_jso, obj);
    do_val_if_expr(VOS_OK != ret, json_object_put(obj);
        debug_log(DLOG_ERROR, "%s %d: Add json object to json array failed, err code %d", __FUNCTION__, __LINE__, ret));
    return VOS_OK;
}


LOCAL gint32 smm_get_chassis_collection_members_count(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    guint32 count = 0;
    gint32 ret;
    guint8 software_type = 0xff;

    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        count += 1; 
    } else {
        
        ret = dfl_get_object_count(CLASS_NAME_IPMBETH_BLADE, &count);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "Get class[%s] object count failed, err code %d", CLASS_NAME_IPMBETH_BLADE, count);
        }
    }
    
    count += 1;

    *o_result_jso = json_object_new_int(count);
    return HTTP_OK;
}


LOCAL gint32 smm_get_odata_id(json_object** o_result_jso)
{
    gchar chassis_uri[CHASSIS_URI_LEN] = { 0 };
    json_object *obj = NULL;
    gint32 ret;
    gchar pslot[PSLOT_MAX_LEN] = {0};
    guint8 software_type = 0xff;

    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        ret = redfish_get_board_slot(pslot, sizeof(pslot));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__);
        }
        ret = snprintf_s(chassis_uri, sizeof(chassis_uri), sizeof(chassis_uri) - 1, URI_FORMAT_CHASSIS,  pslot);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        obj = json_object_new_object();
        json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string(chassis_uri));
        json_object_array_add(*o_result_jso, obj);
        return RET_OK;
    }
    
    obj = json_object_new_object();
    ret = snprintf_s(chassis_uri, sizeof(chassis_uri), sizeof(chassis_uri) - 1, URI_FORMAT_CHASSIS, RFOBJ_HMM1);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string(chassis_uri));
    json_object_array_add(*o_result_jso, obj);

    obj = json_object_new_object();
    ret = snprintf_s(chassis_uri, sizeof(chassis_uri), sizeof(chassis_uri) - 1, URI_FORMAT_CHASSIS, RFOBJ_HMM2);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string(chassis_uri));
    json_object_array_add(*o_result_jso, obj);
    
    ret = dfl_foreach_object(CLASS_NAME_IPMBETH_BLADE, add_blade_as_member, o_result_jso, NULL);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Get blade info failed, err code %d", __func__, ret);
        return ret;
    }
    return RET_OK;
}


LOCAL gint32 smm_get_chassis_collection_members(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    gchar chassis_uri[CHASSIS_URI_LEN] = { 0 };
    json_object *obj = NULL;
    gint32 ret;

    
    return_val_do_info_if_expr(VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso || NULL == o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s %d param error ", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:   memory alloc fail. ", __FUNCTION__, __LINE__));

    
    obj = json_object_new_object();
    return_val_do_info_if_fail(NULL != obj, HTTP_INTERNAL_SERVER_ERROR, json_object_put(*o_result_jso);
        debug_log(DLOG_ERROR, "%s,%d:   memory alloc fail. ", __FUNCTION__, __LINE__));
    ret = snprintf_s(chassis_uri, sizeof(chassis_uri), sizeof(chassis_uri) - 1,
        URI_FORMAT_CHASSIS, SMM_CHASSIS_ENCLOSURE);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string(chassis_uri));
    ret = json_object_array_add(*o_result_jso, obj);
    if (ret != RET_OK) {
        json_object_put(obj);
        debug_log(DLOG_ERROR, "%s %d: Add json object to json array failed, err code %d", __FUNCTION__, __LINE__, ret);
    }

    ret = smm_get_odata_id(o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get odata_id error ret = %d", __FUNCTION__, ret);
        return ret;
    }

    return HTTP_OK;
}


gint32 smm_chassis_provider_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    *prop_provider = smm_collection_chassis_provider;
    *count = sizeof(smm_collection_chassis_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
