 

#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "retimer_manage.h"


 
gint32 retimer_notify_upging(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    GVariant *property_value = NULL;
    guint8 is_upging;
    gchar accessor_name[MAX_OBJECT_NAME_LEN] = {0};

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Retimer notify upgrade is failed, input list is NULL.");
        return RET_ERR;
    }

    property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    is_upging = g_variant_get_byte(property_value);
    if (is_upging != RETIMER_IDLE && is_upging != RETIMER_UPGRADING) {
        debug_log(DLOG_ERROR, "Retimer upgrade state(%d) is not correct.", is_upging);
        return RET_ERR;
    }

    ret = dfl_get_property_accessor(object_handle, PROPERTY_RETIMER_REQ_ACC, accessor_name, sizeof(accessor_name));
    if (ret == DFL_OK) {
        ret = dal_set_property_value_byte(object_handle, PROPERTY_RETIMER_REQ_ACC, is_upging, DF_HW);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Set channel notify failed(ret: %d).", __FUNCTION__, ret);
            return ret;
        }
    } else {
        ret = dal_set_property_value_byte(object_handle, PROPERTY_RETIMER_REQ_ACC, is_upging, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Set channel notify failed(ret: %d).", __FUNCTION__, ret);
            return ret;
        }
    }

    return RET_OK;
}


gint32 retimer_channel_switch(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    GVariant *property_value = NULL;
    guint8 channel_switch;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Retimer channel switch is failed, input list is NULL.");
        return RET_ERR;
    }

    property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    channel_switch = g_variant_get_byte(property_value);
    if (channel_switch != RETIMER_CHN_OPEN && channel_switch != RETIMER_CHN_CLOSE) {
        debug_log(DLOG_ERROR, "Retimer channel switch(%d) is not correct.", channel_switch);
        return RET_ERR;
    }

    return retimer_channel_switch_by_handle(object_handle, channel_switch);
}


gint32 retimer_set_npu_cdr_temp(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    guint16 temp;
    gint32 ret;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input param null.", __FUNCTION__);
        return RET_ERR;
    }

    temp = g_variant_get_uint16((GVariant *)g_slist_nth_data(input_list, 0));
    ret = dal_set_property_value_uint16(object_handle, PROPERTY_RETIMER_TEMPERATURE, temp, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:set temprature failed ret = %d", __FUNCTION__, ret);
    }

    return ret;
}


gint32 retimer_set_npu_cdr_ver(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    const gchar *ver = NULL;
    gsize ver_len = 0;
    gint32 ret;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input param null.", __FUNCTION__);
        return RET_ERR;
    }

    ver = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), &ver_len);
    if (ver == NULL || ver_len == 0 || ver_len > MAX_PROPERTY_VALUE_LEN) {
        debug_log(DLOG_ERROR, "%s: list_value is NULL", __FUNCTION__);
        return RET_ERR;
    }

    ret = dal_set_property_value_string(object_handle, PROPERTY_RETIMER_SOFT_VER, ver, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_set_property_value_string failed, ret = %d", __FUNCTION__, ret);
    }

    return ret;
}


gint32 retimer_set_upg_result(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    GVariant *property_value = NULL;
    guint8 upg_result;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input list is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    upg_result = g_variant_get_byte(property_value);

    ret = dal_set_property_value_byte(object_handle, PROPERTY_RETIMER_UPG_RESULT, upg_result, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:retimer_set_upg_result failed(ret: %d).", __FUNCTION__, ret);
    }
    return ret;
}


gint32 retimer_method_sync_info(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;

    ret = property_sync_common_handler_ext(object_handle, caller_info, input_list, output_list, DF_NONE);
    debug_log(DLOG_DEBUG, "%s property_sync ret:%d.", dfl_get_object_name(object_handle), ret);
    return ret;
}