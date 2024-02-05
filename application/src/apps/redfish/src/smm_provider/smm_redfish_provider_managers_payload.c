
#include "redfish_provider.h"
#include "dirent.h"
#include "redfish_provider_managers_payload.h"


gint32 get_shelf_power_button_mode(json_object **oem_property)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 power_button_mode = 0;
    gchar buf[MAX_STATUS_LEN + 1] = {0};

    if (oem_property == NULL) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret = dal_get_object_handle_nth(CLASS_SHELF_PAYLOAD, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get obj_handle fail, ret = %d.", ret);
        return ret;
    }

    
    ret = dal_get_property_value_byte(obj_handle, PROTERY_SHELF_POWER_BUTTON_MODE, &power_button_mode);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get shelf power state fail, ret = %d.", ret);
        return ret;
    }

    ret = sprintf_s(buf, MAX_STATUS_LEN, "%s", power_button_mode == 1 ? "host" : "bmc");
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "snprintf_s fail, ret = %d", ret);
        return RET_ERR;
    }

    
    *oem_property = json_object_new_string((const gchar *)buf);
    if (*oem_property == NULL) {
        debug_log(DLOG_ERROR, "new json obj fail.");
        return RET_ERR;
    }
    return RET_OK;
}


gint32 set_shelf_power_button_mode(PROVIDER_PARAS_S *i_paras, json_object **o_msg_jso)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 button_mode = 0;
    json_object *huawei_obj = NULL;
    json_object *obj = NULL;
    json_object *msg_jso = NULL;
    GSList *input_list = NULL;
    const char *value_str = NULL;

    if (o_msg_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    gint32 ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_OEM_HUAWEI, &huawei_obj);
    if (ret == 0) {
        debug_log(DLOG_ERROR, "Request body is not right");
        return HTTP_BAD_REQUEST;
    }

    ret = json_object_object_get_ex(huawei_obj, RFPROP_MANAGER_SHELF_POWER_BUTTON_MODE, &obj);
    if (ret == 0) {
        debug_log(DLOG_ERROR, "Request body is not right");
        return HTTP_BAD_REQUEST;
    }

    value_str = dal_json_object_get_str(obj);
    if (g_strcmp0(value_str, "host") == 0) {
        button_mode = 1;
    }

    ret = dal_get_object_handle_nth(CLASS_SHELF_PAYLOAD, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get shelfpaylod object handle failed, err code %d", ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_jso);
        (void)json_object_array_add(*o_msg_jso, msg_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(button_mode));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        obj_handle, CLASS_SHELF_PAYLOAD, METHOD_SHELF_PAYLOAD_SET_BUTTON_MODE, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Call method[%s] of class[%s] failed, err code %d",
            METHOD_SHELF_PAYLOAD_SET_BUTTON_MODE, CLASS_SHELF_PAYLOAD, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_jso);
        (void)json_object_array_add(*o_msg_jso, msg_jso);
        return HTTP_BAD_REQUEST;
    }

    return HTTP_OK;
}
