
#include "redfish_provider.h"
#include "redfish_provider_system.h"

LOCAL gint32 get_system_bios_settings_odatacontext(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bios_settings_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bios_settings_attributes(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_system_bios_settings_attributes(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bios_settings_attributeregistry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_oem_revoke_biossetting_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_revoke_biossetting(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_bios_settings_huawei(PROVIDER_PARAS_S *i_paras, json_object *o_result_jso);
LOCAL gint32 get_system_bios_settings_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_sytem_bios_settings_provider[] = {
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bios_settings_odatacontext, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bios_settings_odataid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RF_BIOS_REG_VERSION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bios_settings_attributeregistry, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ATTRIBUTES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_system_bios_settings_attributes, set_system_bios_settings_attributes, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bios_settings_oem, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_oem_revoke_biossetting_actions, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFACTION_BIOS_REVOKE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, NULL, NULL, act_revoke_biossetting, ETAG_FLAG_ENABLE}
};

LOCAL gint32 get_revoke_biossetting_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL PROPERTY_PROVIDER_S g_revoke_biossetting_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_revoke_biossetting_actioninfo_odataid, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_system_bios_settings_odatacontext(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar string_value[2 * MAX_RSC_ID_LEN] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 获取 @odata.context
    ret = redfish_get_board_slot(slot_str, sizeof(slot_str) - 1);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, RF_BIOS_SETTINGS_ODATA_CONTEXT,
        slot_str);
    return_val_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string(string_value);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_system_bios_settings_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar string_value[MAX_RSC_ID_LEN] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 获取 @odata.id
    ret = redfish_get_board_slot(slot_str, sizeof(slot_str) - 1);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, RF_BIOS_SETTINGS_ODATA_ID, slot_str);
    return_val_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string(string_value);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_system_bios_settings_attributeregistry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar registry_versioon[MAX_RSC_ID_LEN] = {0};

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = rf_bios_get_registryversion(registry_versioon, sizeof(registry_versioon), RF_REGISTRY_VERSION_DOT);
    if (VOS_OK != ret) {
        debug_log(DLOG_DEBUG, "rf_bios_get_registryversion fail");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const char *)registry_versioon);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s, %d, json_object_new_string fail.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_system_bios_settings_attributes(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    const gchar *str = NULL;
    gint32 ret;

    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dfl_get_object_handle(OBJ_NAME_BIOS, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get obj_handle for OBJ_NAME_BIOS fail.", __FUNCTION__));

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(BIOS_JSON_FILE_SETTING_NAME));

    ret = dfl_call_class_method(obj_handle, METHOD_BIOS_GET_JSON_FILE, NULL, input_list, &output_list);

    
    uip_free_gvariant_list(input_list);

    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s: METHOD_BIOS_GET_JSON_FILE fail", __FUNCTION__);
        uip_free_gvariant_list(output_list));

    str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    return_val_do_info_if_fail((NULL != str) && (0 != strlen(str)), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: g_variant_get_string fail", __FUNCTION__);
        uip_free_gvariant_list(output_list));

    *o_result_jso = json_tokener_parse(str);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_tokener_parse fail", __FUNCTION__);
        uip_free_gvariant_list(output_list));

    uip_free_gvariant_list(output_list);

    return HTTP_OK;
}


LOCAL gint32 get_system_bios_settings_huawei(PROVIDER_PARAS_S *i_paras, json_object *o_result_jso)
{
#define BIOS_SETTING_STATUS_INEFFECTIVE 0
#define BIOS_SETTING_STATUS_DESC_INEFFECTIVE "Ineffective"
#define BIOS_SETTING_STATUS_DESC_EFFECTIVE "Effective"

    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle;
    gint32 ret;
    guchar effective_status;

    return_val_do_info_if_expr(provider_paras_check(i_paras) != VOS_OK || o_result_jso == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "get_system_bios_settings : provider_paras_check fail"));

    ret = dfl_get_object_handle(OBJ_NAME_BIOS, &obj_handle);
    goto_label_do_info_if_expr(ret != VOS_OK, ERR_OUT,
        debug_log(DLOG_ERROR, "get_system_bios_settings: get bios obj fail."));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_BIOS, METHOD_BIOS_GET_SETTING_EFFECTIVE_STATUS, AUTH_ENABLE, i_paras->user_role_privilege, NULL,
        &output_list);
    goto_label_do_info_if_expr(ret != VOS_OK, ERR_OUT,
        debug_log(DLOG_ERROR, "get_system_bios_settings: call METHOD_BIOS_GET_SETTING_EFFECTIVE_STATUS fail");
        uip_free_gvariant_list(output_list));

    effective_status = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));

    uip_free_gvariant_list(output_list);

    json_object_object_add(o_result_jso, RFPROP_EFFECTIVE_STATUS,
        json_object_new_string(effective_status == BIOS_SETTING_STATUS_INEFFECTIVE ?
        BIOS_SETTING_STATUS_DESC_INEFFECTIVE :
        BIOS_SETTING_STATUS_DESC_EFFECTIVE));

    return VOS_OK;

ERR_OUT:
    json_object_object_add(o_result_jso, RFPROP_EFFECTIVE_STATUS, NULL);

    return VOS_OK;
}


LOCAL gint32 get_system_bios_settings_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *huawei_result_jso = NULL;

    if (o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 入参检查: 403 错误
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    huawei_result_jso = json_object_new_object();
    return_val_do_info_if_fail(huawei_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : json_object_new_object fail", __FUNCTION__));

    // 获取"Huawei" 的值
    ret = get_system_bios_settings_huawei(i_paras, huawei_result_jso);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR, json_object_put(huawei_result_jso);
        debug_log(DLOG_ERROR, "%s : get_system_bios_settings fail", __FUNCTION__));

    // 将获得的结果添加入o_result_jso
    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR, json_object_put(huawei_result_jso);
        debug_log(DLOG_ERROR, "%s : json_object_new_object fail", __FUNCTION__));

    // 获取"OEM"的值
    json_object_object_add(*o_result_jso, RFPROP_OEM_HUAWEI, huawei_result_jso);

    return HTTP_OK;
}


LOCAL gint32 get_regrist_mapfrom_info(json_object *mapfrom, DEPEND_INFO *depend_info, gint32 *prop_num)
{
    gint32 mapfrom_length;
    gint32 index_id;
    gint32 ret = 0;

    json_object *mapfrom_item_jso = NULL;
    json_object *mapfrom_item_prop_jso = NULL;
    const gchar *map_name = NULL;
    const gchar *map_prop = NULL;

    mapfrom_length = json_object_array_length(mapfrom);

    *prop_num = mapfrom_length;

    return_val_do_info_if_expr(mapfrom_length > MAX_DEPEND_PROP, VOS_ERR,
        debug_log(DLOG_ERROR, "[%s][%d] mapfrom_length is too long \n", __FUNCTION__, __LINE__));

    for (index_id = 0; index_id < mapfrom_length; index_id++) {
        mapfrom_item_jso = NULL;
        mapfrom_item_jso = json_object_array_get_idx(mapfrom, index_id);
        return_val_do_info_if_expr(NULL == mapfrom_item_jso, MODULE_ERR,
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__));

        
        ret = json_object_object_get_ex(mapfrom_item_jso, REGRIST_PROP_MAPFORMATTRIBUTE, &mapfrom_item_prop_jso);
        return_val_do_info_if_expr(FALSE == ret, MODULE_ERR,
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__));

        map_name = dal_json_object_get_str(mapfrom_item_prop_jso);
        return_val_do_info_if_expr(NULL == map_name, MODULE_ERR,
            debug_log(DLOG_ERROR, "[%s][%d] dal_json_object_get_str error \n", __FUNCTION__, __LINE__));
        mapfrom_item_prop_jso = NULL;

        // 成功或者失败都继续执行
        (void)strncpy_s(depend_info[index_id].depend_name, PROPERTY_NAME_LENGTH, map_name, PROPERTY_NAME_LENGTH - 1);

        
        ret = json_object_object_get_ex(mapfrom_item_jso, REGRIST_PROP_MAPFROMPROPERTY, &mapfrom_item_prop_jso);
        return_val_do_info_if_expr(FALSE == ret, MODULE_ERR,
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__));

        map_prop = dal_json_object_get_str(mapfrom_item_prop_jso);
        return_val_do_info_if_expr(NULL == map_prop, MODULE_ERR,
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__));
        mapfrom_item_prop_jso = NULL;

        (void)strncpy_s(depend_info[index_id].depend_prop, PROPERTY_NAME_LENGTH, map_prop, PROPERTY_NAME_LENGTH - 1);

        
        ret = json_object_object_get_ex(mapfrom_item_jso, REGRIST_PROP_MAPFROMVALUE, &mapfrom_item_prop_jso);
        return_val_do_info_if_expr(FALSE == ret, MODULE_ERR,
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__));

        depend_info[index_id].depend_value = json_object_get(mapfrom_item_prop_jso);
        mapfrom_item_prop_jso = NULL;

        ret = json_object_object_get_ex(mapfrom_item_jso, REGRIST_PROP_MAPTERMS, &mapfrom_item_prop_jso);
        return_val_do_info_if_expr(FALSE == ret, MODULE_ERR,
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__));

        depend_info[index_id].prop_relation =
            (0 == g_strcmp0(dal_json_object_get_str(mapfrom_item_prop_jso), REGRIST_STRING_AND) ? 1 : 0);

        mapfrom_item_prop_jso = NULL;
    }

    return VOS_OK;
}


LOCAL json_bool check_input_and_depend(const gchar *inputname, DEPEND_INFO *depend_info, DEPEND_INFO *depend_info_tmp,
    gint32 prop_temp_num, json_object *user_input)
{
    gint32 prop_id;
    json_bool true_flag = FALSE;

    for (prop_id = 0; prop_id < prop_temp_num; prop_id++) {
        // 判断依赖属性是否在用户输入中
        if (0 != strlen(depend_info_tmp[prop_id].depend_name)) {
            true_flag = FALSE;
            // 此处用于消除foreach编译告警"assignment discards 'const' qualifier from pointer target type"
#pragma GCC diagnostic push // require GCC 4.6
#pragma GCC diagnostic ignored "-Wcast-qual"

            
            json_object_object_foreach0(user_input, name, val)
            {
                if (0 == g_strcmp0(name, depend_info_tmp[prop_id].depend_name)) {
                    // BIOS 多对一关系，并未全部采用AND 和OR 表示，这边需自己处理下
                    do_val_if_expr(1 == prop_temp_num, json_object_put(depend_info_tmp[0].depend_value);
                        depend_info_tmp[0].depend_value = json_object_get(val));

                    true_flag = TRUE;
                }

                if (TRUE == true_flag && 0 == g_strcmp0(name, depend_info[prop_id].depend_name)) {
                    true_flag = FALSE;
                }

                if (0 == g_strcmp0(name, inputname) || json_type_null == json_object_get_type(val)) {
                    break;
                }
            }

#pragma GCC diagnostic pop // require GCC 4.6

            // OR
            if (0 == depend_info_tmp[0].prop_relation) {
                if (TRUE == true_flag) {
                    break;
                }
            }

            // AND
            if (1 == depend_info_tmp[0].prop_relation) {
                if (TRUE == true_flag && prop_id == prop_temp_num - 1) {
                    break;
                }

                if (FALSE == true_flag) {
                    break;
                }
            }
        }
    }

    return true_flag;
}


LOCAL void get_regrist_depend_attrbributes(const gchar *type, const gchar *inputname, json_object *inputvalue,
    DEPEND_INFO *depend_info, gint32 *prop_num, json_object *user_input)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    const gchar *regrist_file_data = NULL;
    json_object *file_data_obj = NULL;
    json_object *registryentries = NULL;
    json_object *dependencies = NULL;
    json_object *item_obj_jso = NULL;
    json_object *dependency = NULL;
    json_object *maptoattribute = NULL;
    json_object *maptoroperty = NULL;
    json_object *maptovalue = NULL;
    json_object *mapfrom = NULL;
    const gchar *name_str = NULL;
    const gchar *prop_str = NULL;

    DEPEND_INFO depend_info_tmp[MAX_DEPEND_PROP];

    gint32 dependencies_length;
    gint32 index_id;
    gint32 prop_temp_num = 0;

    (void)memset_s(depend_info_tmp, sizeof(depend_info_tmp), 0, sizeof(depend_info_tmp));

    // 递归深度限制
    static gint32 depth_value = 0;
    depth_value++;

    if (depth_value > 10 || depth_value < 0) {
        depth_value--;
        debug_log(DLOG_ERROR, "%s: depth_valueerror.\n", __FUNCTION__);
        return;
    }

    GSList *output_list = NULL;
    GSList *input_list = NULL;

    
    return_do_info_if_expr(inputname == NULL || inputvalue == NULL || depend_info == NULL || user_input == NULL,
        depth_value--);

    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: get o_obj_handle fail.\n", __FUNCTION__));

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(BIOS_JSON_FILE_REGISTRY_NAME));

    ret = dfl_call_class_method(obj_handle, METHOD_BIOS_GET_JSON_FILE, NULL, input_list, &output_list);

    return_do_info_if_expr(VOS_OK != ret, debug_log(DLOG_DEBUG, "%s:ret %d.", __FUNCTION__, ret));

    regrist_file_data = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    if (NULL != regrist_file_data) {
        file_data_obj = json_tokener_parse(regrist_file_data);
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    goto_label_do_info_if_expr(file_data_obj == NULL, ERR_EXIT,
        debug_log(DLOG_ERROR, "%s:  file_data_obj == NULL.\n", __FUNCTION__));

    (void)json_object_object_get_ex(file_data_obj, REGRIST_PROP_REGISTRYENTRIES, &registryentries);

    (void)json_object_object_get_ex(registryentries, REGRIST_PROP_DEPENDENCIES, &dependencies);

    
    dependencies_length = json_object_array_length(dependencies);

    for (index_id = 0; index_id < dependencies_length; index_id++) {
        item_obj_jso = NULL;
        item_obj_jso = json_object_array_get_idx(dependencies, index_id);
        do_if_true(NULL == item_obj_jso, debug_log(DLOG_ERROR, "[%s] json_object_object_get_ex error \n", __FUNCTION__);
            goto ERR_EXIT);

        ret = json_object_object_get_ex(item_obj_jso, REGRIST_PROP_DEPENDENCY, &dependency);
        do_if_true(FALSE == ret, debug_log(DLOG_ERROR, "[%s] name error \n", __FUNCTION__); goto ERR_EXIT);

        ret = json_object_object_get_ex(dependency, REGRIST_PROP_MAPTOATTRIBUTE, &maptoattribute);
        do_if_true(FALSE == ret, debug_log(DLOG_ERROR, "[%s] name error \n", __FUNCTION__); goto ERR_EXIT);

        name_str = dal_json_object_get_str(maptoattribute);
        do_if_true(0 != g_strcmp0(name_str, inputname), continue);

        ret = json_object_object_get_ex(dependency, REGRIST_PROP_MAPTOPROPERTY, &maptoroperty);
        do_if_true(FALSE == ret, debug_log(DLOG_ERROR, "[%s] name error \n", __FUNCTION__); goto ERR_EXIT);

        prop_str = dal_json_object_get_str(maptoroperty);
        do_if_true(0 != g_strcmp0(prop_str, type), continue);

        ret = json_object_object_get_ex(dependency, REGRIST_PROP_MAPTOVALUE, &maptovalue);
        do_if_true(FALSE == ret, debug_log(DLOG_ERROR, "[%s] name error \n", __FUNCTION__); goto ERR_EXIT);

        // 当为hidden 或者readonly 或者值
        if (json_object_get_type(maptovalue) == json_object_get_type(inputvalue)) {
            do_if_true((json_type_boolean == json_object_get_type(maptovalue) &&
                json_object_get_boolean(inputvalue) != json_object_get_boolean(maptovalue)),
                continue);
            do_if_true((json_type_string == json_object_get_type(maptovalue) &&
                0 != g_strcmp0(dal_json_object_get_str(inputvalue), dal_json_object_get_str(maptovalue))),
                continue);
        } else {
            continue;
        }

        ret = json_object_object_get_ex(dependency, REGRIST_PROP_MAPFORM, &mapfrom);
        do_if_true(FALSE == ret, debug_log(DLOG_ERROR, "[%s] name error \n", __FUNCTION__); goto ERR_EXIT);

        // MapFromProperty   获取该属性依赖的属性列表信息
        (void)get_regrist_mapfrom_info(mapfrom, depend_info, prop_num);
        json_object_put(file_data_obj);

        // 如果一个属性依赖多个属性，直接返回
        if (*prop_num > 1) {
            depth_value--;
            return;
        }

        // 判断依赖的属性所依赖的属性是否还有父级依赖
        if (0 != strlen(depend_info[0].depend_name)) {
            get_regrist_depend_attrbributes(depend_info[0].depend_prop, depend_info[0].depend_name,
                depend_info[0].depend_value, depend_info_tmp, &prop_temp_num, user_input);

            // 判断是否父及依赖导致属性不能设置
            ret = check_input_and_depend(inputname, depend_info, depend_info_tmp, prop_temp_num, user_input);
            debug_log(DLOG_DEBUG, "[%s] depend_name:%s  depend_name_temp:%s\n", __FUNCTION__,
                depend_info[0].depend_name, depend_info_tmp[0].depend_name);

            if (TRUE == ret) {
                // 先释放depend_info中的json对象
                json_object_put(depend_info[0].depend_value);
                (void)memcpy_s(depend_info, MAX_DEPEND_PROP * sizeof(DEPEND_INFO), depend_info_tmp,
                    MAX_DEPEND_PROP * sizeof(DEPEND_INFO));
            }
        }

        depth_value--;
        return;
    }

ERR_EXIT:

    depth_value--;
    json_object_put(file_data_obj);

    return;
}


LOCAL gint32 bios_setting_create_message_info(gchar *name_message, DEPEND_INFO *depend_info, gint32 prop_num,
    json_object **o_message_jso)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret = 0;
    gint32 prop_index_id = 0;
    gchar depend_name_message[PROPERTY_LEN] = {0};

    if (prop_num > 1) {
        debug_log(DLOG_DEBUG, ">>%s %s %s %s %s", depend_info[0].depend_name, depend_info[1].depend_name,
            depend_info[2].depend_name, depend_info[3].depend_name, depend_info[4].depend_name);

        for (prop_index_id = 0; prop_index_id < prop_num; prop_index_id++) {
            safe_fun_ret = strncat_s(depend_name_message, PROPERTY_LEN, THE_VALUE_FOR_THE_PROPERTY_BLANK,
                strlen(THE_VALUE_FOR_THE_PROPERTY_BLANK));
            do_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            safe_fun_ret = strncat_s(depend_name_message, PROPERTY_LEN, depend_info[prop_index_id].depend_name,
                strlen(depend_info[prop_index_id].depend_name));
            do_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            safe_fun_ret = strncat_s(depend_name_message, PROPERTY_LEN, BLANK_IS_BLANK, strlen(BLANK_IS_BLANK));
            do_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            safe_fun_ret = strncat_s(depend_name_message, PROPERTY_LEN,
                dal_json_object_get_str(depend_info[prop_index_id].depend_value),
                strlen(dal_json_object_get_str(depend_info[prop_index_id].depend_value)));
            do_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            json_object_put(depend_info[prop_index_id].depend_value);

            if (prop_index_id == (prop_num - 2)) {
                // 0表示or  ，1表示and
                if (0 == depend_info[prop_index_id].prop_relation) {
                    safe_fun_ret = strncat_s(depend_name_message, PROPERTY_LEN, BLANK_OR_BLANK, strlen(BLANK_OR_BLANK));
                    do_if_expr(safe_fun_ret != EOK,
                        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
                } else {
                    safe_fun_ret =
                        strncat_s(depend_name_message, PROPERTY_LEN, BLANK_AND_BLANK, strlen(BLANK_AND_BLANK));
                    do_if_expr(safe_fun_ret != EOK,
                        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
                }
            }

            if (prop_index_id < (prop_num - 2)) {
                safe_fun_ret = strncat_s(depend_name_message, PROPERTY_LEN, COMMA_BLANK, strlen(COMMA_BLANK));
                do_if_expr(safe_fun_ret != EOK,
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            }
        }

        (void)create_message_info(MSGID_PROP_SETTINGPROPFAILDEXT, name_message, o_message_jso, name_message,
            depend_name_message);
    } else {
        ret = snprintf_s(depend_name_message, PROPERTY_LEN, PROPERTY_LEN - 1, "%s/%s", REGRIST_PROP_ATTRIBUTES,
            depend_info[0].depend_name);
        do_val_if_expr(0 >= ret, debug_log(DLOG_ERROR, "[%s][%d] snprintf_s \n", __FUNCTION__, __LINE__));
        (void)create_message_info(MSGID_PROP_SETTINGPROPFAILD, name_message, o_message_jso, name_message,
            depend_name_message, dal_json_object_get_str(depend_info[0].depend_value));
        json_object_put(depend_info[0].depend_value);
    }

    return VOS_OK;
}


LOCAL gint32 process_bios_settings_err(GSList *output_list, json_object *value_json, json_object **o_message_jso)
{
    gint32 ret_type;
    gint32 ret = 0;
    gint32 prop_num = 0;
    const gchar *prop_name = NULL;
    const gchar *value = NULL;
    DEPEND_INFO depend_info[MAX_DEPEND_PROP];

    json_object *json_obj = NULL;
    gchar name_message[PROPERTY_NAME_LENGTH] = {0};
    gint length;

    if (NULL == output_list || NULL == o_message_jso || NULL == value_json) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)memset_s(depend_info, sizeof(depend_info), 0, sizeof(depend_info));

    ret_type = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    if (ret_type == VOS_OK) {
        return HTTP_OK;
    }

    length = g_slist_length(output_list);
    if (length >= 2) {
        prop_name = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), NULL);
        ret = snprintf_s(name_message, PROPERTY_NAME_LENGTH, PROPERTY_NAME_LENGTH - 1, "%s/%s", REGRIST_PROP_ATTRIBUTES,
            prop_name);
        do_val_if_expr(0 >= ret, debug_log(DLOG_ERROR, "[%s][%d] snprintf_s \n", __FUNCTION__, __LINE__));
    }

    if (length >= 3) {
        value = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 2), NULL);
    }

    switch (ret_type) {
        case REGRIST_TYPE_PROP_UNKNOWN:
            (void)create_message_info(MSGID_PROP_UNKNOWN, name_message, o_message_jso, name_message);
            break;

        case REGRIST_TYPE_PROP_IMMUTABLE:
            (void)create_message_info(MSGID_PROP_IMMUTABLE, name_message, o_message_jso);
            break;

        case REGRIST_TYPE_PROP_READONLY:
            
            json_obj = json_object_new_boolean(TRUE);
            get_regrist_depend_attrbributes(REGRIST_PROP_READONLY, prop_name, json_obj, depend_info, &prop_num,
                value_json);
            json_object_put(json_obj);

            
            if (0 == strlen(depend_info[0].depend_name)) {
                (void)create_message_info(MSGID_PROP_NOT_WRITABLE, name_message, o_message_jso, name_message);
            } else {
                
                bios_setting_create_message_info(name_message, depend_info, prop_num, o_message_jso);
                
            }

            break;

        case REGRIST_TYPE_PROP_NOT_LIST:
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, name_message, o_message_jso, value, name_message);
            break;

        case REGRIST_TYPE_PROP_LENGTH_ERR:
        case REGRIST_TYPE_PROP_FORMAT_ERR:

            (void)create_message_info(MSGID_PROP_FORMAT_ERR, name_message, o_message_jso, value, name_message);
            break;

        case REGRIST_TYPE_PROP_NOT_RANGE:
            (void)create_message_info(MSGID_VALUE_OUT_OF_RANGE, name_message, o_message_jso, name_message);
            break;

        case REGRIST_TYPE_PROP_TYPE_ERROR:
            (void)create_message_info(MSGID_PROP_TYPE_ERR, name_message, o_message_jso, value, name_message);
            break;

        case REGRIST_TYPE_PROP_SCALAR_INCREMENT:
            (void)create_message_info(MSGID_PROP_SCALARINCREMENT, name_message, o_message_jso);
            break;

        case REGRIST_TYPE_PROP_HIDE:
            json_obj = json_object_new_boolean(TRUE);
            get_regrist_depend_attrbributes(REGRIST_PROP_HIDDEN, prop_name, json_obj, depend_info, &prop_num,
                value_json);
            json_object_put(json_obj);

            
            bios_setting_create_message_info(name_message, depend_info, prop_num, o_message_jso);
            

            break;

        case REGRIST_TYPE_PROP_BOOTTYPEORDER_ERROR:
            (void)create_message_info(MSGID_PROP_SETTINGBOOTORDERFAILED, NULL, o_message_jso);
            break;

            
        case ERR_NOT_SUPPORT_BIOS_MGNT:
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, REGRIST_PROP_ATTRIBUTES, o_message_jso,
                REGRIST_PROP_ATTRIBUTES);
            
            return HTTP_NOT_IMPLEMENTED;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_BAD_REQUEST;
}


LOCAL gint32 set_system_bios_settings_attributes(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    const gchar *str = NULL;
    gint32 ret;

    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    void *user_data = NULL;

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_ATTRIBUTES, o_message_jso,
        RFPROP_ATTRIBUTES));
    

    
    if (TRUE == json_object_get_user_data(i_paras->val_jso, (void **)&user_data) && NULL != user_data) {
        return HTTP_BAD_REQUEST;
    }

    ret = dfl_get_object_handle(OBJ_NAME_BIOS, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get obj_handle for OBJ_NAME_BIOS fail.", __FUNCTION__));

    
    str = dal_json_object_to_json_string(i_paras->val_jso);
    return_val_do_info_if_fail(NULL != str, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: dal_json_object_to_json_string failed", __FUNCTION__));

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(BIOS_JSON_FILE_SETTING_NAME));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(str));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_BIOS, METHOD_BIOS_SET_JSON_FILE, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        &output_list);
    if (VOS_OK == ret) { // 返回ok也需要检查返回的err_type
        ret = process_bios_settings_err(output_list, i_paras->val_jso, o_message_jso);
    } else {
        switch (ret) {
                
            case RFERR_INSUFFICIENT_PRIVILEGE:
                (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
                ret = HTTP_BAD_REQUEST;
                break;

                
            case RFERR_NO_RESOURCE:
                (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
                ret = HTTP_BAD_REQUEST;
                break;

            default:
                debug_log(DLOG_ERROR, "%s: ret = %d", __FUNCTION__, ret);
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
                ret = HTTP_INTERNAL_SERVER_ERROR;
                break;
        }
    }

    

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    return ret;
}


LOCAL gint32 get_oem_revoke_biossetting_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar string_value[MAX_URI_LENGTH] = {0};
    gchar actioninfo[MAX_URI_LENGTH] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gint32 ret;

    json_object *jso = NULL;
    json_object *huawei_json = NULL;
    json_object *oem_json = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot_str, sizeof(slot_str) - 1);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot fail.", __FUNCTION__));
    
    ret =
        snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, RFACTION_BIOS_REVOKE_TARGET, slot_str);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: snprintf_s fail.", __FUNCTION__));
    
    ret = snprintf_s(actioninfo, sizeof(actioninfo), sizeof(actioninfo) - 1, RF_BIOS_REVOKE_SETTING_ACTION_INFO,
        slot_str);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: snprintf_s fail.", __FUNCTION__));
    jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail.", __FUNCTION__));
    json_object_object_add(jso, RFPROP_TARGET, json_object_new_string(string_value));
    json_object_object_add(jso, RFPROP_ACTION_INFO, json_object_new_string(actioninfo));

    huawei_json = json_object_new_object();
    return_val_do_info_if_fail(NULL != huawei_json, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail.", __FUNCTION__);
        json_object_put(jso));
    json_object_object_add(huawei_json, RFACTION_PARAM_BIOS_REVOKE, jso);

    oem_json = json_object_new_object();
    return_val_do_info_if_fail(NULL != oem_json, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail.", __FUNCTION__);
        json_object_put(huawei_json));
    json_object_object_add(oem_json, RFPROP_MANAGER_HUAWEI, huawei_json);

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail.", __FUNCTION__);
        json_object_put(oem_json));
    json_object_object_add(*o_result_jso, RFPROP_MANAGER_OEM, oem_json);

    return HTTP_OK;
}


LOCAL gint32 act_revoke_biossetting(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    ret = dal_get_object_handle_nth(CLASS_NAME_BIOS, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get obj_handle  fail.", __FUNCTION__));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_BIOS, METHOD_REVOKE_BIOSSETTING, AUTH_ENABLE, i_paras->user_role_privilege, NULL, NULL);

    switch (ret) {
        case VOS_OK:
            ret = HTTP_OK;
            break;

            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            
            ret = HTTP_FORBIDDEN;
            
            break;

        default: 
            debug_log(DLOG_ERROR, "%s: ret = %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            ret = HTTP_INTERNAL_SERVER_ERROR;
            break;
    }

    return ret;
}


gint32 system_bios_settings_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gboolean ret;
    guint32 product_ver = 0;
    guchar arm_enable = 0;

    ret = redfish_check_system_uri_valid(i_paras->uri);
    if (!ret) {
        return HTTP_NOT_FOUND;
    }

    guchar board_type = 0;
    gint32 ret_int;
    
    ret_int = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret_int, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get board_type fail", __FUNCTION__));
    return_val_if_fail(DISABLE != board_type, HTTP_NOT_FOUND);

    ret_int = dal_get_product_version_num(&product_ver);
    return_val_do_info_if_fail(VOS_OK == ret_int, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: dal_get_product_version_num fail", __FUNCTION__));
    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    // arm产品支持通过redfish配置bios setup菜单
    return_val_if_expr(((DISABLE == arm_enable) && (PRODUCT_VERSION_V5 > product_ver)), HTTP_NOT_FOUND);
    

    *prop_provider = g_sytem_bios_settings_provider;
    *count = sizeof(g_sytem_bios_settings_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


LOCAL gint32 get_revoke_biossetting_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    gchar string_value[MAX_RSC_URI_LEN] = {0};
    
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 获取 @odata.id
    ret = redfish_get_board_slot(slot_str, sizeof(slot_str) - 1);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot fail.", __FUNCTION__));

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, RF_BIOS_REVOKE_SETTING_ACTION_INFO,
        slot_str);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: snprintf_s fail.", __FUNCTION__));

    *o_result_jso = json_object_new_string(string_value);
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}


gint32 revoke_biossetting_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gboolean ret;
    guint32 product_ver = 0;
    guchar arm_enable = 0;

    ret = redfish_check_system_uri_valid(i_paras->uri);
    if (!ret) {
        return HTTP_NOT_FOUND;
    }

    
    gint32 ret_int;
    guchar board_type = 0;
    ret_int = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret_int, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get board_type fail", __FUNCTION__));
    return_val_if_fail(DISABLE != board_type, HTTP_NOT_FOUND);

    ret_int = dal_get_product_version_num(&product_ver);
    return_val_do_info_if_fail(VOS_OK == ret_int, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: dal_get_product_version_num fail", __FUNCTION__));
    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    // arm产品支持通过redfish配置bios setup菜单
    return_val_if_expr(((DISABLE == arm_enable) && (PRODUCT_VERSION_V5 > product_ver)), HTTP_NOT_FOUND);
    

    *prop_provider = g_revoke_biossetting_actioninfo_provider;
    *count = sizeof(g_revoke_biossetting_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
