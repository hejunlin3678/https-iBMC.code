

#include "redfish_provider.h"


LOCAL gint32 get_system_overview_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar url[MAX_URI_LENGTH] = {0};
    gint32 ret;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = strcpy_s(url, sizeof(url), URI_FORMAT_SYSTEM_OVERVIEW_ODATA_ID);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:strncpy_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string(url);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_system_overview_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_context[MAX_URI_LENGTH] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = strcpy_s(odata_context, sizeof(odata_context), URI_FORMAT_SYSTEM_OVERVIEW_ODATA_CONTEXT);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:strncpy_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string(odata_context);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 get_system_overview_odata_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_type[MAX_URI_LENGTH] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s,%d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = strcpy_s(odata_type, sizeof(odata_type), URI_FORMAT_SYSTEM_OVERVIEW_ODATA_TYPE);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:strncpy_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string(odata_type);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


gint32 get_common_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gchar   pslot[PSLOT_MAX_LEN] = {0};
    gint32 ret;

    
    return_val_do_info_if_expr(VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso || NULL == o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(pslot, sizeof(pslot));
    do_if_expr(VOS_OK != ret,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_get_board_slot.", __FUNCTION__, __LINE__));

    
    json_object_object_add(*o_result_jso, RFPROP_COMMON_ID, json_object_new_string(pslot));

    return HTTP_OK;
}


LOCAL PROPERTY_PROVIDER_S collection_system_overview_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_overview_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_overview_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_TYPE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_overview_odata_type, NULL, NULL, ETAG_FLAG_DISABLE},
    {RFPROP_SYSTEM_OVERVIEW_MANAGERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_overview_managers, NULL, NULL, ETAG_FLAG_DISABLE},
    {RFPROP_SYSTEM_OVERVIEW_SYSTEMS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_overview_systems, NULL, NULL, ETAG_FLAG_DISABLE},
    {RFPROP_SYSTEM_OVERVIEW_CHASSIS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_overview_chassis, NULL, NULL, ETAG_FLAG_DISABLE},
};


gint32 system_overview_provider_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    guint8 software_type = 0;

    
    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_EM || software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = collection_system_overview_provider;
    *count = sizeof(collection_system_overview_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
