

#include "pme_app/pme_app.h"

#include "get_version.h"

#include "security_module.h"






gint32 security_module_set_self_test_result(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = RET_OK;
    guint8 value = 0;
    GVariant *property_data = NULL;

    if (input_list == NULL) {
        return RET_ERR;
    }

    property_data = (GVariant *)g_slist_nth_data(input_list, 0);
    value = g_variant_get_byte(property_data);
    if ((value != 0) && (value != 1)) {
        debug_log(DLOG_ERROR, "%s: Invalid parameter.", __FUNCTION__);
        return RET_ERR;
    }

    ret =
        dal_set_property_value_byte(object_handle, PROPERTY_SECURITY_MODULE_SELF_TEST_RESULT, value, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Set security module self test result failed: %d.", __FUNCTION__, ret);
        maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Set security module self test result %d", ret);
    } else {
        maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Set security module self test result to (%s) successfully",
            (value == 1) ? "pass" : "fail");
    }

    return ret;
}


gint32 security_module_set_specification_type(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = RET_OK;
    const gchar *value = NULL;
    GVariant *property_data = NULL;

    if (input_list == NULL) {
        return RET_ERR;
    }

    property_data = (GVariant *)g_slist_nth_data(input_list, 0);
    value = g_variant_get_string(property_data, NULL);
    if (value == NULL || strlen(value) >= SECURITY_MODULE_MAX_STRING_LENGTH) {
        debug_log(DLOG_ERROR, "%s: Invalid parameter.", __FUNCTION__);
        return RET_ERR;
    }

    ret = dal_set_property_value_string(object_handle, PROPERTY_SECURITY_MODULE_SPECIFICATION_TYPE, value,
        DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Set security module specification type failed: %d.", __FUNCTION__, ret);
        strategy_log(SLOG_ERROR, "Set security module specification type failed");
    } else {
        strategy_log(SLOG_INFO, "Set security module specification type to (%s) successfully", value);
    }

    return ret;
}


gint32 security_module_set_manufacturer_name(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = RET_OK;
    const gchar *value = NULL;
    GVariant *property_data = NULL;

    if (input_list == NULL) {
        return RET_ERR;
    }

    property_data = (GVariant *)g_slist_nth_data(input_list, 0);
    value = g_variant_get_string(property_data, NULL);
    if (value == NULL || strlen(value) >= SECURITY_MODULE_MAX_STRING_LENGTH) {
        debug_log(DLOG_ERROR, "%s: Invalid parameter.", __FUNCTION__);
        return RET_ERR;
    }

    ret = dal_set_property_value_string(object_handle, PROPERTY_SECURITY_MODULE_MANUFACTURER_NAME, value,
        DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Set security module manufacturer name failed: %d.", __FUNCTION__, ret);
        strategy_log(SLOG_ERROR, "Set security module manufacturer name failed");
    } else {
        strategy_log(SLOG_INFO, "Set security module manufacturer name to (%s) successfully", value);
    }

    return ret;
}


gint32 security_module_set_specification_version(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = RET_OK;
    const gchar *value = NULL;
    GVariant *property_data = NULL;

    if (input_list == NULL) {
        return RET_ERR;
    }

    property_data = (GVariant *)g_slist_nth_data(input_list, 0);
    value = g_variant_get_string(property_data, NULL);
    if (value == NULL || strlen(value) >= SECURITY_MODULE_MAX_STRING_LENGTH) {
        debug_log(DLOG_ERROR, "%s: Invalid parameter.", __FUNCTION__);
        return RET_ERR;
    }

    ret = dal_set_property_value_string(object_handle, PROPERTY_SECURITY_MODULE_SPECIFICATION_VERSION, value,
        DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Set security module specification version failed: %d.", __FUNCTION__, ret);
        strategy_log(SLOG_ERROR, "Set security module specification version failed");
    } else {
        strategy_log(SLOG_INFO, "Set security module specification version to (%s) successfully", value);
    }

    return ret;
}


gint32 security_module_set_manufacturer_version(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = RET_OK;
    const gchar *value = NULL;
    GVariant *property_data = NULL;

    if (input_list == NULL) {
        return RET_ERR;
    }

    property_data = (GVariant *)g_slist_nth_data(input_list, 0);
    value = g_variant_get_string(property_data, NULL);
    if (value == NULL || strlen(value) >= SECURITY_MODULE_MAX_STRING_LENGTH) {
        debug_log(DLOG_ERROR, "%s: Invalid parameter.", __FUNCTION__);
        return RET_ERR;
    }

    ret = dal_set_property_value_string(object_handle, PROPERTY_SECURITY_MODULE_MANUFACTURER_VERSION, value,
        DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Set security module manufacturer version failed: %d.", __FUNCTION__, ret);
        strategy_log(SLOG_ERROR, "Set security module manufacturer version failed");
    } else {
        strategy_log(SLOG_INFO, "Set security module manufacturer version to (%s) successfully", value);
    }

    return ret;
}
 
LOCAL void securitymodule_operation_log(OBJ_HANDLE obj_handle, guint8 option)
{
    GSList *caller_info_list = NULL;
    gchar sec_moudle_type[MAX_PROPERTY_VALUE_LEN] = {};
 
    (void)dal_get_property_value_string(obj_handle, PROPERTY_SECURITY_MODULE_SPECIFICATION_TYPE,
        sec_moudle_type, sizeof(sec_moudle_type));
 
    caller_info_list = g_slist_append(caller_info_list, g_variant_new_string("N/A"));
    caller_info_list = g_slist_append(caller_info_list, g_variant_new_string("N/A"));
    caller_info_list = g_slist_append(caller_info_list, g_variant_new_string("127.0.0.1"));
 
    switch (option) {
        case CARD_PLUG_IN:
        case CARD_PLUG_OUT:
            method_operation_log(caller_info_list, NULL, "%s %s successfully", sec_moudle_type,
                (option == CARD_PLUG_IN) ? "plug in" : "plug out");
            break;
 
        default:
            debug_log(DLOG_ERROR, "securitymodule_operation_log: option(%d) is out of range!", option);
            break;
    }
    g_slist_free_full(caller_info_list, (GDestroyNotify)g_variant_unref);
    return;
}
 

gint32 securitymodule_add_object_callback(OBJ_HANDLE obj_handle)
{
    securitymodule_operation_log(obj_handle, CARD_PLUG_IN);
    return RET_OK;
}
 

gint32 securitymodule_del_object_callback(OBJ_HANDLE obj_handle)
{
    securitymodule_operation_log(obj_handle, CARD_PLUG_OUT);
    return RET_OK;
}