
#include "redfish_provider.h"
#include "pme_app/smm/smm_public.h"

#include <dirent.h>
#include <errno.h>

LOCAL gint32 __get_manager_switch_profiles_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_manager_switch_profiles_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_switch_profiles(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);


LOCAL PROPERTY_PROVIDER_S g_managers_switch_profiles_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_manager_switch_profiles_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_manager_switch_profiles_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SWITCH_PROFILES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_switch_profiles, NULL, NULL, ETAG_FLAG_ENABLE},
};

LOCAL gint32 __get_manager_switch_profiles_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)

{
    gint32 ret;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar switch_profiles_uri[MAX_URI_LENGTH] = {0};

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, MAX_RSC_NAME_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get slot fail", __FUNCTION__));

    ret = snprintf_s(switch_profiles_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT_MANAGER_SWITCHPROFILES, slot);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: snprintf_s switch profiles uri fail", __FUNCTION__));

    *o_result_jso = json_object_new_string((const char *)switch_profiles_uri);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new switch profiles uri  json string fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 __get_manager_switch_profiles_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)

{
    gint32 ret;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar switch_profiles_context[MAX_URI_LENGTH] = {0};

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, MAX_RSC_NAME_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get slot fail", __FUNCTION__));

    ret =
        snprintf_s(switch_profiles_context, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, MANAGERS_SWITCHPROFILES_METADATA, slot);
    return_val_do_info_if_expr(ret <= 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: snprintf_s switch profiles context fail", __FUNCTION__));

    *o_result_jso = json_object_new_string((const char *)switch_profiles_context);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new switch profiles context  json string fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_manager_switch_cfg_update(OBJ_HANDLE obj_handle, const gchar *cur_swi_model)
{
    gint32 ret;
    GSList *input_list = NULL;

    input_list = g_slist_append(input_list, g_variant_new_string(cur_swi_model));
    ret = dfl_call_class_method(obj_handle, METHOD_DELETE_INVALID_SWI_CFG_FILE, NULL, input_list, NULL);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "dfl_call_class_method %s.%s. Ret is %d\n", OBJECT_NAME_IPMBETH_BLADE1,
            METHOD_DELETE_INVALID_SWI_CFG_FILE, ret);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        return VOS_ERR;
    }

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    return ret;
}

LOCAL void get_manager_switch_system_name(OBJ_HANDLE obj_handle, guchar slaveaddr, gchar *systemname, gint32 length)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *systemname_temp = NULL;

    input_list = g_slist_append(input_list, g_variant_new_byte(slaveaddr));
    ret = dfl_call_class_method(obj_handle, METHOD_SHELF_GET_BLADE_INFO, NULL, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    return_if_expr(VOS_OK != ret);

    systemname_temp = g_variant_get_bytestring((GVariant *)g_slist_nth_data(output_list, 0));
    return_do_info_if_expr(NULL == systemname_temp,
        debug_log(DLOG_ERROR, "%s: g_variant_get_bytestring failed.", __FUNCTION__);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref));
    ret = strncpy_s(systemname, length, systemname_temp, strlen(systemname_temp));
    do_val_if_expr(ret != EOK, debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, ret));
    (void)g_strstrip(systemname);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
}


LOCAL void get_swi_cfg_info(const gchar *cfg_file_path, const gchar *backup_time, const gchar *cur_swi_model,
    guchar slot, guchar swi_id, json_object **o_result_jso)
{
    const gchar *planetype = NULL;
    guint32 ck_plane_id = 0;
    guint32 ck_swi_id = 0;
    gchar **name_array = NULL;
    DIR *dp = NULL;
    struct dirent *sub_ptr = NULL;
    json_object *profile_json = NULL;

    
    if ((dp = opendir(cfg_file_path)) == NULL) {
        return;
    }

    while ((sub_ptr = readdir(dp)) != NULL) {
        continue_if_expr(0 == g_strcmp0(".", sub_ptr->d_name) || 0 == g_strcmp0("..", sub_ptr->d_name));

        // 13_1_1.cfg
        name_array = NULL;
        name_array = g_strsplit_set(sub_ptr->d_name, "_.", 0);
        if (name_array == NULL || name_array[0] == NULL || name_array[1] == NULL) {
            g_strfreev(name_array);
            continue;
        }

        ck_swi_id = 0;
        ck_plane_id = 0;
        (void)vos_str2int(name_array[0], 10, &ck_swi_id, NUM_TPYE_UINT32);
        (void)vos_str2int(name_array[1], 10, &ck_plane_id, NUM_TPYE_UINT32);
        g_strfreev(name_array);

        if (swi_id != ck_swi_id) {
            continue;
        }

        planetype = ck_plane_id == 0 ? SWI_BASE_STR : SWI_FABRIC_STR;

        profile_json = json_object_new_object();
        json_object_object_add(profile_json, RFPROP_SWITCH_PROFILES_SLOT, json_object_new_int(slot));
        json_object_object_add(profile_json, RFPROP_SWITCH_PROFILES_BOARDTYPE, json_object_new_string(cur_swi_model));
        json_object_object_add(profile_json, RFPROP_SWITCH_PROFILES_PLANETYPE, json_object_new_string(planetype));
        json_object_object_add(profile_json, RFPROP_SWITCH_PROFILES_FLIENAME, json_object_new_string(sub_ptr->d_name));
        json_object_object_add(profile_json, RFPROP_SWITCH_PROFILES_BACKUPTIME, json_object_new_string(backup_time));
        json_object_array_add(*o_result_jso, profile_json);
    }
    closedir(dp);
}


LOCAL void get_manager_switch_one_profile_info(guchar slot, json_object **o_result_jso)
{
    gint32 ret;
    guchar slaveaddr;
    OBJ_HANDLE obj_handle = 0;
    const gchar *sw_name = NULL;
    gchar  cur_swi_model[STRING_LEN_MAX] = {0};
    gchar cfg_file_path[MAX_FILE_NAME_LEN] = {0};
    gchar cfg_file_time_path[MAX_FILE_NAME_LEN] = {0};
    DIR *dirp = NULL;
    struct dirent *ptr = NULL;
    guint8 presence = 0;
    guchar swi_id;

    slaveaddr = SWI_SLAVEADDR_BASE + slot * 2;
    ret =
        dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_SLAVEADDR, slaveaddr, &obj_handle);
    return_if_expr(VOS_OK != ret);
    (void)dal_get_property_value_byte(obj_handle, PROTERY_IPMBETH_BLADE_PRESENCE, &presence);
    return_if_expr(0 == presence);

    get_manager_switch_system_name(obj_handle, slaveaddr, cur_swi_model, STRING_LEN_MAX);

    // 刷新配置文件
    get_manager_switch_cfg_update(obj_handle, cur_swi_model);

    sw_name = dfl_get_object_name(obj_handle);

    return_if_expr(NULL == sw_name);

    swi_id = get_swi_id_from_model_str(cur_swi_model);

    ret = snprintf_s(cfg_file_path, sizeof(cfg_file_path), sizeof(cfg_file_path) - 1, "%s/%s/%s", CFG_FILE_PATH,
        sw_name, SWITCH_CFG_FILE_BAK_PATH);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));

    // cfg_file_path = /data/opt/pme/conf/bladecfg/swi2/bakcfg
    dirp = opendir(cfg_file_path);

    do_val_if_expr(NULL == dirp, goto CURRENT);

    while ((ptr = readdir(dirp)) != NULL) {
        continue_if_expr(0 == g_strcmp0(".", ptr->d_name) || 0 == g_strcmp0("..", ptr->d_name));
        ret = sprintf_s(cfg_file_time_path, sizeof(cfg_file_time_path), "%s/%s", cfg_file_path, ptr->d_name);
        do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: sprintf_s fail, ret = %d", __FUNCTION__, ret));

        
        get_swi_cfg_info(cfg_file_time_path, (const gchar *)ptr->d_name, cur_swi_model, slot, swi_id, o_result_jso);
    }

    closedir(dirp);
    dirp = NULL;

CURRENT:

    (void)memset_s(cfg_file_path, sizeof(cfg_file_path), 0, sizeof(cfg_file_path));
    ret = snprintf_s(cfg_file_path, sizeof(cfg_file_path), sizeof(cfg_file_path) - 1, "%s/%s", CFG_FILE_PATH, sw_name);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));

    // cfg_file_path = /data/opt/pme/conf/bladecfg/swi2/
    (void)get_swi_cfg_info(cfg_file_path, CFG_CURRENT_FLAG, cur_swi_model, slot, swi_id, o_result_jso);
}


LOCAL gint32 get_manager_switch_profiles(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guchar slot;
    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    // 权限不足返回500，因为需要填充null
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s: Insufficient permissions", __FUNCTION__));

    *o_result_jso = json_object_new_array();
    return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    for (slot = 1; slot <= 4; slot++) {
        get_manager_switch_one_profile_info(slot, o_result_jso);
    }

    return HTTP_OK;
}


gint32 managers_provider_switch_profiles_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)

{
    if (!redfish_check_manager_uri_valid(i_paras->uri)) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_managers_switch_profiles_provider;
    *count = sizeof(g_managers_switch_profiles_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
