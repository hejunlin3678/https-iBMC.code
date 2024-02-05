

#include "redfish_provider.h"
#define FPC_SERVICE_NUM 1

LOCAL gint32 get_manager_fpcservice_context(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    return get_universal_provider_odata_info(i_paras,
        o_message_jso, o_result_jso, RFPROP_MANAGER_FPCSERVICE_CONTEXT);
}

LOCAL gint32 get_manager_fpcservice_odataid(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    return get_universal_provider_odata_info(i_paras,
        o_message_jso, o_result_jso, RFPROP_MANAGER_FPCSERVICE_ODATA_ID);
}

LOCAL gint32 get_manager_fpcservice_count(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    guint8 fpc_service_enable = DISABLE;

    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG,
        PROPERTY_PME_SERVICECONFIG_FPC_SUPPORT, &fpc_service_enable);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get PMEServiceConfig obj's FPCSupport failed, ret = %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (fpc_service_enable != ENABLE) {
        *o_result_jso = json_object_new_int(0);
    } else {
        *o_result_jso = json_object_new_int(FPC_SERVICE_NUM);
    }
    if (*o_result_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}
LOCAL gint32 get_manager_fpcservice_members(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    gchar odata_id[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_NAME_LEN] = {0};
    json_object* odata_id_json = NULL;
    json_object* mem_id = NULL;
    guint8 fpc_service_enable = DISABLE;

    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG,
        PROPERTY_PME_SERVICECONFIG_FPC_SUPPORT, &fpc_service_enable);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get PMEServiceConfig obj's FPCSupport failed, ret = %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (fpc_service_enable != ENABLE) {
        debug_log(DLOG_DEBUG, "%s:This resource is not enabled by license, fpc_service_enable = %d.",
            __FUNCTION__, fpc_service_enable);
        return HTTP_OK;
    }

    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_array();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_array fail.",  __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, RFPROP_MANAGER_FPCSERVICE_MEMORY, slot_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    odata_id_json = json_object_new_object();
    if (odata_id_json == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    mem_id = json_object_new_string(odata_id);
    if (mem_id == NULL) {
        json_object_put(odata_id_json);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)json_object_object_add(odata_id_json, RFPROP_ODATA_ID, mem_id);
    (void)json_object_array_add(*o_result_jso, odata_id_json);

    return HTTP_OK;
}
LOCAL PROPERTY_PROVIDER_S g_manager_fpcservice_provider[] = {
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_fpcservice_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_fpcservice_odataid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_fpcservice_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_fpcservice_members, NULL, NULL, ETAG_FLAG_ENABLE}
};

gint32 manager_provider_fpcservice_entry(PROVIDER_PARAS_S* i_paras,
    PROPERTY_PROVIDER_S** prop_provider, guint32* count)
{
    gboolean ret;
    ret = redfish_check_manager_uri_valid(i_paras->uri);
    if (ret != TRUE) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_manager_fpcservice_provider;
    *count = sizeof(g_manager_fpcservice_provider) / sizeof(PROPERTY_PROVIDER_S);

    return RET_OK;
}
