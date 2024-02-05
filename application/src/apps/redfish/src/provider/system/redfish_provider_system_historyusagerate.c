
#include "redfish_provider.h"
#include "redfish_provider_system.h"

LOCAL gint32 get_processors_history_usage_rate_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_processors_history_usage_rate_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_processors_history_usage_rate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_system_processors_usage_rate_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, 
        get_processors_history_usage_rate_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, 
        get_processors_history_usage_rate_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_HISTORY_DATA, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, 
        get_processors_history_usage_rate, NULL, NULL, ETAG_FLAG_DISABLE}
};


LOCAL gint32 get_processors_history_usage_rate_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar uri[MAX_URI_LENGTH + 1] = {0};
    gchar slot[MAX_RSC_NAME_LEN + 1] = {0};

    
    ret = redfish_get_board_slot(slot, sizeof(slot) - 1);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 拼接@odata_id
    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_SYS_PROCESSORS_HISTORY_USAGE_RATE, slot);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }

    *o_result_jso = json_object_new_string((const char *)uri);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_processors_history_usage_rate_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar odata_context[MAX_URI_LENGTH + 1] = {0};
    gchar slot[MAX_RSC_NAME_LEN + 1] = {0};

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    iRet = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1,
        SYSTEMS_PROCESSORS_HISTORY_USAGE_RATE_METADATA, slot);
    if (iRet < 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }

    *o_result_jso = json_object_new_string((const char *)odata_context);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_processors_history_usage_rate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar file_path[MAX_FILE_PATH_LENGTH + 1] = {0};
    gchar* labels[] = {RFPROP_PROCESSORS_UTILISE};

    
    iRet = snprintf_s(file_path, MAX_FILE_PATH_LENGTH + 1, MAX_FILE_PATH_LENGTH, "%s%s", OPT_PME_PRAM_FOLDER,
        PROCESSOR_HISTORY_USAGE_RATE_FILE);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }

    
    
    ret = read_history_data_from_file(file_path, sizeof(labels) / sizeof(char *), labels, o_result_jso,
        READ_HISTORY_UINT16);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: read_history_data_from_file fail, ret is %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


gint32 get_process_memory_history_supported(PROVIDER_PARAS_S *i_paras)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guchar me_access_type = 0;
    guint8 cups_support_flag = 0;

    
    ret = get_bma_connect_state(i_paras);
    if (ret == RET_OK) {
        return RET_OK;
    }

    ret = dal_get_object_handle_nth(CLASS_ME_INFO, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get MeInfo object fail.", __FUNCTION__);
        return RET_ERR;
    }
    
    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_MEINFO_UTILISE_SUPPORT, &cups_support_flag);
    if (ret != RET_OK || cups_support_flag == DISABLED) {
        debug_log(DLOG_INFO, "%s: do not support CupS.", __FUNCTION__);
        return RET_ERR;
    }

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_ME_ACCESS_TYPE, &me_access_type);
    if (me_access_type == ME_ACCESS_TYPE_SMBUS_PECI) {
        debug_log(DLOG_INFO, "%s: me type is SMbus-PECI and BMA unconnected.", __FUNCTION__);
        return RET_ERR;
    }

    
    return RET_OK;
}


gint32 system_processors_usage_rate_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gboolean bool_ret;
    guchar board_type = 0;
    gint32 ret;

    
    ret = redfish_get_x86_enable_type(&board_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: redfish_get_x86_enable_type fail, ret is %d.", __FUNCTION__, ret);
        return HTTP_NOT_FOUND;
    }

    if (board_type != ENABLE) {
        debug_log(DLOG_DEBUG, "%s: board_type is disable.", __FUNCTION__);
        return HTTP_NOT_FOUND;
    }

    // 判断当前是否支持历史占用率查询
    ret = get_process_memory_history_supported(i_paras);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: not support hostory query", __FUNCTION__);
        return HTTP_NOT_FOUND;
    }

    
    bool_ret = redfish_check_system_uri_valid(i_paras->uri);
    if (bool_ret != TRUE) {
        debug_log(DLOG_ERROR, "%s: uri is illegals", __FUNCTION__);
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_system_processors_usage_rate_provider;
    *count = sizeof(g_system_processors_usage_rate_provider) / sizeof(PROPERTY_PROVIDER_S);
    return RET_OK;
}
