
#include "redfish_provider.h"

LOCAL PROPERTY_PROVIDER_S  g_manager_sp_raid_provider[] = {
};


gint32 manager_sp_raid_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    return_val_do_info_if_expr(FALSE == rf_support_sp_service(), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s, %d: not support", __FUNCTION__, __LINE__));

    if (redfish_check_manager_uri_valid(i_paras->uri)) {
        *prop_provider = g_manager_sp_raid_provider;
        
        *count = 0;
        ;
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}


gint32 get_sp_raid_file_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gint32 ret;

    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)get_sp_memberid_odata_context_id(o_rsc_jso, MANAGERS_SPSERVICE_RAID_METADATA, MANAGER_SPSERVICE_RAID_URI,
        i_paras->member_id);

    (void)get_sp_pcie_memberid_id(o_rsc_jso, i_paras);

    ret = get_sp_file_data(i_paras, UMS_TRANS_FILE_ID_SP_RAID, i_paras->member_id, o_rsc_jso);
    return_val_do_info_if_expr((VOS_OK != ret), RF_RSC_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s, %d: file_json  NULL.\n", __FUNCTION__, __LINE__));

    return RF_OK;
}


LOCAL gint32 redfish_check_board_memberid_by_type(const gchar *i_memberid, gint32 type)
{
    gint32 ret;
    gboolean match_flag = VOS_ERR;

    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    guint8 component_type = 0;
    guint8 fru_id = 0;
    guint8 presence = 0;
    guint8 arm_enable = 0;

    
    return_val_if_expr(NULL == i_memberid, match_flag);

    
    ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
    do_val_if_expr(VOS_OK != ret, debug_log(DLOG_MASS, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_PRESENCE, &presence);
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_FRUID, &fru_id);
        continue_if_expr(type != component_type);
        continue_if_expr(0 == presence);
        continue_if_expr(0xff == fru_id);

        
        if (arm_enable || redfish_check_board_memberid((OBJ_HANDLE)obj_node->data, component_type, i_memberid)) {
            match_flag = VOS_OK;
            break;
        }
    }

    do_if_expr(NULL != obj_list, g_slist_free(obj_list));

    return match_flag;
}


gint32 create_sp_raid_id(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *o_id, guint32 id_len,
    json_object *o_message_array_jso)
{
    gint32 ret;
    guchar value_byte = 0;
    const gchar *memberid = NULL;
    gchar memberid_string[MAX_CHARACTER_NUM] = {0};
    json_object *message_info_jso = NULL;
    json_object *val_jso = NULL;
    json_bool is_ret;
    OBJ_HANDLE o_obj_handle = 0;

    return_val_do_info_if_fail(
        ((NULL != o_id) && (NULL != o_message_array_jso) && (NULL != i_param) && (NULL != body_jso_checked)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s invalid input param", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso));

    
    return_val_do_info_if_expr(FALSE == rf_support_sp_service(), HTTP_NOT_FOUND,
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, i_param->uri);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));
    

    
    ret = redfish_check_manager_uri_valid(i_param->uri);
    return_val_do_info_if_fail(TRUE == ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s: user input uri is bad.\n", __FUNCTION__);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, i_param->uri);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));
    

    
    // 判断用户是否具有基础设置权限,没有直接返回禁止
    return_val_do_info_if_fail(0 != (i_param->user_role_privilege & USERROLE_BASICSETTING), HTTP_FORBIDDEN,
        debug_log(DLOG_ERROR, "%s %d: user has no privilege, user is %s", __FUNCTION__, __LINE__, i_param->user_name);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &message_info_jso);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));
    

    // 从请求体中解析出ID
    is_ret = json_object_object_get_ex(body_jso_checked, RFPROP_COMMON_ID, &val_jso);
    return_val_do_info_if_expr(TRUE != is_ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s, %d:  json_object_object_get_ex  fail", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso));

    memberid = dal_json_object_get_str(val_jso);

    do_if_expr(NULL != memberid, (void)strncpy_s(memberid_string, MAX_CHARACTER_NUM, memberid, MAX_CHARACTER_NUM - 1));

    
    return_val_do_info_if_expr(0 == strlen(memberid_string), HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s, %d:  check pcie device fail", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_COMMON_ID, &message_info_jso, memberid_string,
        RFPROP_COMMON_ID);
        json_object_array_add(o_message_array_jso, message_info_jso));
    

    if (VOS_OK == check_power_state_on()) {
        
        if (VOS_OK != redfish_check_board_memberid_by_type(memberid_string, COMPONENT_TYPE_RAID_CARD)) {
            
            // 校验ID
            
            ret = redfish_check_pciedevice_location_devicename(memberid_string, &o_obj_handle);
            
            return_val_do_info_if_expr(TRUE != ret, HTTP_BAD_REQUEST,
                debug_log(DLOG_ERROR, "%s, %d:  check pcie device fail", __FUNCTION__, __LINE__);
                (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_COMMON_ID, &message_info_jso, memberid_string,
                RFPROP_COMMON_ID);
                json_object_array_add(o_message_array_jso, message_info_jso));

            ret = dal_get_property_value_byte(o_obj_handle, PROPERTY_PCIECARD_FUNCTIONCLASS, &value_byte);
            return_val_do_info_if_expr(VOS_OK != ret || PCIECARD_FUNCTION_RAID != value_byte, HTTP_BAD_REQUEST,
                debug_log(DLOG_ERROR, "%s, %d:  check pcie device fail", __FUNCTION__, __LINE__);
                (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_COMMON_ID, &message_info_jso, memberid_string,
                RFPROP_COMMON_ID);
                json_object_array_add(o_message_array_jso, message_info_jso));

            
        }

        
    }

    ret = create_sp_item_template(i_param, body_jso_checked, o_id, id_len, memberid_string, UMS_TRANS_FILE_ID_SP_RAID);
    if (VOS_OK != ret) {
        switch (ret) {
            case UMS_TRANS_ERR_FILE_TOO_LARGE:
            case UMS_TRANS_ERR_NO_SPACE:
                (void)create_message_info(MSGID_SP_CFG_OVERSIZED, NULL, &message_info_jso);
                break;

            case UMS_WR_ERR_BUSY:
                (void)create_message_info(MSGID_UMS_IS_EXCLUSIVELY_USED, NULL, &message_info_jso);
                break;

            case UMS_TRANS_ERR_FILE_CNT_LIMITED:
                (void)create_message_info(MSGID_SP_CFG_LIMIT, NULL, &message_info_jso);
                break;

            case UMS_WR_ERR_IN_UPGRADING:
                (void)create_message_info(MSGID_SP_IS_BEING_UPGRADED, NULL, &message_info_jso);
                break;

            default:
                debug_log(DLOG_ERROR, "%s, %d:  failed with ret %d.", __FUNCTION__, __LINE__, ret);
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
                json_object_array_add(o_message_array_jso, message_info_jso);
                return HTTP_INTERNAL_SERVER_ERROR;
        }

        debug_log(DLOG_ERROR, "%s %d: create_sp_item_template fail, ret is 0x%x", __FUNCTION__, __LINE__, ret);
        json_object_array_add(o_message_array_jso, message_info_jso);
        ret = HTTP_BAD_REQUEST;
    }

    return ret;
}


gint32 get_sp_memberid_odata_context_id(json_object *o_rsc_jso, gchar *metadata, gchar *odataid, gchar *memberid)
{
    gint32 ret;
    gchar uri_id[MAX_URI_LENGTH] = {0};
    gchar uri_context[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    json_object *o_jso = NULL;

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(uri_context, sizeof(uri_context), sizeof(uri_context) - 1, metadata, slot);
#pragma GCC diagnostic pop

    return_val_do_info_if_expr(0 >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));
    

    o_jso = json_object_new_string((const gchar *)uri_context);
    return_val_do_info_if_expr(NULL == o_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__));
    json_object_object_add(o_rsc_jso, RFPROP_ODATA_CONTEXT, o_jso);
    o_jso = NULL;

    if (NULL != memberid) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
        ret = snprintf_s(uri_id, sizeof(uri_id), sizeof(uri_id) - 1, odataid, slot, memberid);
#pragma GCC diagnostic pop

        return_val_do_info_if_expr(0 >= ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));
    } else {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
        ret = snprintf_s(uri_id, sizeof(uri_id), sizeof(uri_id) - 1, odataid, slot);
#pragma GCC diagnostic pop

        return_val_do_info_if_expr(0 >= ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));
    }

    o_jso = json_object_new_string((const gchar *)uri_id);
    return_val_do_info_if_expr(NULL == o_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__));
    json_object_object_add(o_rsc_jso, RFPROP_ODATA_ID, o_jso);

    return VOS_OK;
}


gint32 get_sp_pcie_memberid_id(json_object *o_rsc_jso, PROVIDER_PARAS_S *i_paras)
{
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE component_handle = 0;
    gchar pcie_memberid[MAX_STRBUF_LEN] = {0};
    gint32 ret;

    return_val_do_info_if_expr(NULL == o_rsc_jso || VOS_OK != provider_paras_check(i_paras), VOS_ERR,
        debug_log(DLOG_ERROR, "%s:  NULL pointer", __FUNCTION__));

    // 获取句柄
    ret = redfish_check_pciedevice_location_devicename(i_paras->member_id, &obj_handle);
    if (TRUE == ret) {
        ret = redfish_get_pcie_component_obj_handle(i_paras->obj_handle, &component_handle);
        return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get objhandle fail.", __FUNCTION__, __LINE__));

        
        ret = get_object_obj_location_devicename(component_handle, pcie_memberid, sizeof(pcie_memberid));
        return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: location devicename fail.", __FUNCTION__, __LINE__));

        json_object_object_add(o_rsc_jso, RFPROP_COMMON_ID, json_object_new_string((const gchar *)pcie_memberid));

        json_object_object_add(o_rsc_jso, RFPROP_COMMON_NAME, json_object_new_string((const gchar *)pcie_memberid));
    } else {
        json_object_object_add(o_rsc_jso, RFPROP_COMMON_ID, json_object_new_string(i_paras->member_id));

        json_object_object_add(o_rsc_jso, RFPROP_COMMON_NAME, json_object_new_string(i_paras->member_id));
    }

    return VOS_OK;
}


gint32 check_power_state_on(void)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    guchar powerstatel_byte = 0;

    ret = dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PAYLOAD_CHASSPWR_STATE, &powerstatel_byte);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));

    if (1 == powerstatel_byte) {
        return VOS_OK;
    }

    return VOS_ERR;
}


gint32 create_sp_item_template(PROVIDER_PARAS_S *i_paras, json_object *body_jso_checked, gchar *o_id, guint32 id_len,
    const gchar *filename, gint32 pathid)
{
    int iRet;
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle;
    gchar jsonfilename[MAX_CHARACTER_NUM] = {0};
    const gchar *data = NULL;
    gchar *filename_lower = NULL;

    return_val_do_info_if_fail(
        ((NULL != o_id) && (NULL != filename) && (NULL != i_paras) && (NULL != body_jso_checked)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s input param error", __FUNCTION__));

    ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle  fail", __FUNCTION__, __LINE__));

    data = dal_json_object_get_str(body_jso_checked);
    return_val_do_info_if_expr(NULL == data, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle  fail", __FUNCTION__, __LINE__));

    filename_lower = g_ascii_strdown(filename, strlen(filename));
    return_val_do_info_if_expr(NULL == filename_lower, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:  filename_lower  fail", __FUNCTION__, __LINE__));

    iRet = snprintf_s(jsonfilename, sizeof(jsonfilename), sizeof(jsonfilename) - 1, "%s.json", filename_lower);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    g_free(filename_lower);

    input_list = g_slist_append(input_list, g_variant_new_byte(pathid));
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)jsonfilename));
    input_list =
        g_slist_append(input_list, g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, data, strlen(data), sizeof(guchar)));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        SP_UMS_CLASS_NAME, UMS_METHOD_WRITE_FILE, AUTH_DISABLE, i_paras->user_role_privilege, input_list, &output_list);

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    
    if (ret == RET_OK) {
        write_sp_task_desc(i_paras, HAVE_SP_TASK);
    }

    return_val_do_info_if_expr(VOS_OK != ret, ret,
        debug_log(DLOG_ERROR, "%s:%s failed, pathid = %d, filename = %s, ret = %d", __FUNCTION__, UMS_METHOD_WRITE_FILE,
        pathid, jsonfilename, ret));
    return_val_do_info_if_expr(0 == id_len, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: param error, id_len is 0.", __FUNCTION__));

    if (UMS_TRANS_FILE_ID_SP_RAID == pathid) {
        iRet = snprintf_s(o_id, id_len, id_len - 1, "%s", filename);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    } else {
        (void)snprintf_s(o_id, id_len, id_len - 1, "%s", "1");
    }

    return ret;
}
