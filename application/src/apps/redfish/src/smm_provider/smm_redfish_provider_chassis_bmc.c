
#include "redfish_provider.h"
#include "redfish_http.h"
#include "redfish_forward.h"

LOCAL gint32 smm_chassis_json_object_add(json_object *o_rsc_jso, const gchar *blade_name, guint8 ipmb_addr,
    guint8 presence)
{
    int iRet;
    gchar odata_id[MAX_URI_LEN] = { 0 };
    json_object *obj = NULL;
    guint8 as_status = STANDBY_STATE;

    iRet = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, "/redfish/v1/Chassis/%s", blade_name);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    json_object_object_add(o_rsc_jso, RFPROP_ODATA_CONTEXT,
        json_object_new_string("/redfish/v1/$metadata#Chassis/Members/$entity"));
    json_object_object_add(o_rsc_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)odata_id));
    json_object_object_add(o_rsc_jso, RFPROP_ID, json_object_new_string((const gchar *)blade_name));
    json_object_object_add(o_rsc_jso, RFPROP_CHASSIS_TYPE, json_object_new_string("Blade"));
    obj = json_object_new_object();
    json_object_object_add(obj, RFPROP_CHASSIS_STATUS_STATE,
        json_object_new_string(presence == BOARD_PRESENT ? ENABLED_STRING : ABSENT_STRING));
    json_object_object_add(obj, RFPROP_CHASSIS_STATUS_HEALTH, NULL);
    json_object_object_add(o_rsc_jso, RFPROP_STATUS, obj);
    obj = json_object_new_object();
    json_object_object_add(obj, RFPROP_OEM_HUAWEI, json_object_new_object());
    json_object_object_add(o_rsc_jso, RFPROP_OEM, obj);
    (void)get_board_active_state(&as_status);
    
    if ((presence == BOARD_PRESENT) && (as_status == ACTIVE_STATE)) {
        add_blade_system_name(ipmb_addr, o_rsc_jso);
        add_blade_size(blade_name, o_rsc_jso);
    }
    json_object_object_add(o_rsc_jso, RFPROP_LINKS, json_object_new_object());
    return RET_OK;
}


gint32 smm_get_chassis_bmc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gint32 ret;
    guint8 ipmb_addr = 0;
    gchar blade_name[MAX_NAME_LEN] = { 0 };
    json_object *tmp_message_obj = NULL;
    guint8 presence = 0;

    guint8 software_type = 0xff;
    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &tmp_message_obj);
        json_object_array_add(o_err_array_jso, tmp_message_obj);
        return HTTP_NOT_FOUND;
    }

    
    if (NULL == o_rsc_jso || NULL == o_err_array_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "[%s] NULL pointer.", __func__);
        create_message_info(MSGID_INTERNAL_ERR, NULL, &tmp_message_obj);
        json_object_array_add(o_err_array_jso, tmp_message_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_READONLY, HTTP_FORBIDDEN, {
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &tmp_message_obj);
        json_object_array_add(o_err_array_jso, tmp_message_obj);
    });
    // 获取单板名
    ret = get_blade_name_from_uri(i_paras->uri, blade_name, sizeof(blade_name), o_err_array_jso);
    if (ret != RET_OK) {
        return ret;
    }
    format_string_as_upper_head(blade_name, strlen(blade_name));
    // 获取单板ipmb地址
    ret = get_board_slave_addr(blade_name, &ipmb_addr);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get blade(%s) ipmb addr failed, err code %d", __func__, blade_name, ret);
        create_message_info(MSGID_INTERNAL_ERR, NULL, &tmp_message_obj);
        json_object_array_add(o_err_array_jso, tmp_message_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    // 获取在位信息
    ret = get_board_presence(ipmb_addr, &presence);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get blade(%s) presence failed, err code %d", __func__, blade_name, ret);
        create_message_info(MSGID_INTERNAL_ERR, NULL, &tmp_message_obj);
        json_object_array_add(o_err_array_jso, tmp_message_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = smm_chassis_json_object_add(o_rsc_jso, blade_name, ipmb_addr, presence);
    return ret; 
}
