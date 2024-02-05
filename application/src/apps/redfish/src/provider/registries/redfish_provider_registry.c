

#include <dirent.h>

#include "redfish_provider.h"


LOCAL gchar g_evt_major_ver[MAX_VER_INFO_LEN + 1] = {0};
LOCAL gchar g_evt_minor_ver[MAX_VER_INFO_LEN + 1] = {0};
LOCAL gchar g_evt_aux_ver[MAX_VER_INFO_LEN + 1] = {0};


gint32 get_event_lang_version(gchar** major_ver, gchar** minor_ver, gchar** aux_ver)
{
    gint32 out_ver_num = 0;
    struct dirent *entry = NULL;
    DIR *dirp = NULL;
    gint32 len;

    return_val_do_info_if_fail((NULL != major_ver) && (NULL != minor_ver) && (NULL != aux_ver), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    if (strlen(g_evt_major_ver) != 0 && strlen(g_evt_minor_ver) != 0 && strlen(g_evt_aux_ver) != 0) {
        *major_ver = g_evt_major_ver;
        *minor_ver = g_evt_minor_ver;
        *aux_ver = g_evt_aux_ver;
        return RET_OK;
    }
    
    dirp = opendir(REDFISH_MSG_REGISTRY_DIR);
    if (dirp == NULL) {
        debug_log(DLOG_ERROR, "folder %s not found.", REDFISH_MSG_REGISTRY_DIR);
        return RET_ERR;
    }

    while (1) {
        if ((entry = readdir(dirp)) == NULL) {
            break;
        }

        len = strlen(entry->d_name);
        if ((len >= MAX_FILE_NAME) || (len < MIN_FILE_NAME)) {
            continue;
        }

        if (g_strrstr(entry->d_name, REDFISH_EVT_REGISTRY_LOWER_PRE) != NULL) {
            
            out_ver_num = sscanf_s(entry->d_name, "ibmcevents.v%[0-9]_%[0-9]_%[0-9].json", g_evt_major_ver, 
                sizeof(g_evt_major_ver) - 1, g_evt_minor_ver, sizeof(g_evt_minor_ver) - 1,
                g_evt_aux_ver, sizeof(g_evt_aux_ver) - 1);
            break;
        }
    }

    closedir(dirp);

    if (out_ver_num == 3) {
        *major_ver = g_evt_major_ver;
        *minor_ver = g_evt_minor_ver;
        *aux_ver = g_evt_aux_ver;
        return  RET_OK;
    }
    return RET_ERR;
}

gint32 get_registry_file_collection_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gchar               event_registry_odata_id[MAX_URI_LENGTH] = {0};
    gchar*              major_ver = NULL;
    gchar*              minor_ver = NULL;
    gchar*              aux_ver = NULL;
    json_object *member_array_jso = NULL;
    json_object *member_item_jso = NULL;
    guint32 bmc_ver = 0;
    guchar board_type = 0;
    gchar               string_value[MAX_RSC_ID_LEN] = {0};
    gchar               uri_value[MAX_PROP_VAL_LEN] = {0};
    guchar arm_enable = 0;

    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RF_FAILED;
    }
    gint32 ret = get_event_lang_version(&major_ver, &minor_ver, &aux_ver);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "get event registry version info"));
    // MAX_URI_LENGTH大小为512，大于格式化后字符串长度，无需判断返回值
    (void)snprintf_s(event_registry_odata_id, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s/iBMCEvents.v%s_%s_%s",
        REGISTRY_FILE_COL_ODATAID, major_ver, minor_ver, aux_ver);

    (void)json_object_object_get_ex(o_rsc_jso, RFPROP_MEMBERS, &member_array_jso);
    return_val_do_info_if_fail(NULL != member_array_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "get property %s from registry collection rsc failed", RFPROP_MEMBERS));

    member_item_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != member_item_jso, VOS_ERR, debug_log(DLOG_ERROR, "alloc json object failed"));

    json_object_object_add(member_item_jso, ODATA_ID, json_object_new_string((const gchar *)event_registry_odata_id));

    
    ret = json_object_array_add(member_array_jso, member_item_jso);
    return_val_do_info_if_fail(0 == ret, VOS_ERR, debug_log(DLOG_ERROR, "add array item failed, ret is %d", ret);
        (void)json_object_put(member_item_jso));

    

    (void)redfish_get_x86_enable_type(&board_type);

    (void)dal_get_product_version_num(&bmc_ver);

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    guint8 conf_supp = NOT_SUPPORT;
    (void)dal_get_func_ability(BIOS_CLASS_NAME, BIOS_CONFIG_EXPORT_SUPPORT, &conf_supp);
    // arm产品支持通过redfish配置bios setup菜单
    if ((bmc_ver >= PRODUCT_VERSION_V5 || arm_enable == ENABLE) && (board_type != DISABLE) && (conf_supp == SUPPORT)) {
        member_item_jso = NULL;

        // 获取 AttributeRegistry
        ret = rf_bios_get_registryversion(string_value, sizeof(string_value), RF_REGISTRY_VERSION_V);
        return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR, debug_log(DLOG_ERROR, "rf_bios_get_registryversion fail"));

        ret = snprintf_s(uri_value, sizeof(uri_value), sizeof(uri_value) - 1, RFPROP_REGRIST_ODTAID_URI, string_value);
        return_val_if_expr(ret < 0, VOS_ERR);

        member_item_jso = json_object_new_object();
        json_object_object_add(member_item_jso, ODATA_ID, json_object_new_string((const gchar *)uri_value));
        ret = json_object_array_add(member_array_jso, member_item_jso);
        return_val_do_info_if_fail(0 == ret, VOS_ERR, debug_log(DLOG_ERROR, "add array item failed, ret is %d", ret);
            (void)json_object_put(member_item_jso));
    }

    
    gint32 member_cnt = json_object_array_length(member_array_jso);
    json_object_object_add(o_rsc_jso, RFPROP_MEMBERS_COUNT, json_object_new_int(member_cnt));

    return VOS_OK;
}

LOCAL PROPERTY_PROVIDER_S g_registry_collection_provider[] = {
};


gint32 registry_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    *prop_provider = g_registry_collection_provider;
    
    *count = 0;
    ;

    return VOS_OK;
}


LOCAL gint32 get_event_location(const gchar* registry_location_buf, json_object *o_rsc_jso)
{
    json_object *location_array_jso = NULL;
    json_object *location_item_jso = NULL;
    gint32 ret;

    location_array_jso = json_object_new_array(); 
    if (location_array_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: alloc new json array failed", __FUNCTION__);
        return RET_ERR;
    }

    location_item_jso = json_object_new_object();
    if (location_item_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: alloc new json object failed", __FUNCTION__);
        (void)json_object_put(location_array_jso);
        return RET_ERR;
    }

    ret = json_object_array_add(location_array_jso, location_item_jso);    
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: add item to json array failed %d", __FUNCTION__, ret);
        (void)json_object_put(location_array_jso);
        (void)json_object_put(location_item_jso);
        return RET_ERR;
    }
    
    json_object_object_add(o_rsc_jso, RFPROP_LOCATION, location_array_jso);

    json_object_object_add(location_item_jso, RFRPOP_LANGUAGE, json_object_new_string(EVT_REGISTRY_LANG_EN));
    json_object_object_add(location_item_jso, RFPROP_LOCATION_URI,
        json_object_new_string((const gchar *)registry_location_buf));

    return RET_OK;
}


gint32 get_event_registry_file_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gchar               registry_id_buf[MAX_STRBUF_LEN] = {0};
    gchar               registry_location_buf[MAX_URI_LENGTH] = {0};
    gchar               odata_id_str[MAX_URI_LENGTH] = {0};
    gint32 ret;
    gchar               major_ver[MAX_VER_INFO_LEN + 1] = {0};
    gchar               minor_ver[MAX_VER_INFO_LEN + 1] = {0};
    gchar               aux_ver[MAX_VER_INFO_LEN + 1] = {0};
    gint32 out_ver_num;

    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RF_FAILED;
    }

    
    out_ver_num = sscanf_s(i_paras->member_id, "ibmcevents.v%[0-9]_%[0-9]_%[0-9].json", major_ver, 
        sizeof(major_ver) - 1, minor_ver, sizeof(minor_ver) - 1, aux_ver, sizeof(aux_ver) - 1);
    return_val_do_info_if_fail(3 == out_ver_num, VOS_ERR,
        debug_log(DLOG_ERROR, "parse registry version info from %s failed, ret is %d", i_paras->member_id,
        out_ver_num));
    // MAX_STRBUF_LEN大小为128，大于格式化后字符串长度，无需判断返回值
    ret = snprintf_s(registry_id_buf, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "iBMCEvents.v%s_%s_%s", major_ver, minor_ver,
        aux_ver);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail %d", __FUNCTION__, ret);
        return RET_ERR;
    }    

    
    ret = snprintf_s(odata_id_str, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s/%s", REGISTRY_FILE_COL_ODATAID,
        registry_id_buf);
    return_val_do_info_if_fail(ret > 0, VOS_ERR,
        debug_log(DLOG_ERROR, "format event registry odata.id failed, ret is %d", ret));

    
    ret = snprintf_s(registry_location_buf, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s/%s.json", REGISTRY_LOCATION_ODATAID,
        registry_id_buf);
    return_val_do_info_if_fail(ret > 0, VOS_ERR,
        debug_log(DLOG_ERROR, "format event registry location failed, ret is %d", ret));

    json_object_object_add(o_rsc_jso, ODATA_ID, json_object_new_string((const gchar *)odata_id_str));
    json_object_object_add(o_rsc_jso, RFPROP_ID, json_object_new_string((const gchar *)registry_id_buf));

    ret = get_event_location((const gchar *)registry_location_buf, o_rsc_jso);
    if (ret != RET_OK) {
        return ret;
    }
    // MAX_STRBUF_LEN大小为128，大于格式化后字符串长度，无需判断返回值
    ret = snprintf_s(registry_id_buf, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "iBMCEvents.%s.%s", major_ver, minor_ver);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail", __FUNCTION__);
        return RET_ERR;
    }
    
    json_object_object_add(o_rsc_jso, RFPROP_REGISTRY, json_object_new_string((const gchar *)registry_id_buf));

    return VOS_OK;
}


LOCAL gint32 _check_event_registry_id_valid(const gchar *event_registry_id)
{
    gchar               registry_file_path[MAX_URI_LENGTH] = {0};
    gint32 ret;
    gchar *file_path_lower = NULL;

    return_val_do_info_if_fail(NULL != event_registry_id, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    ret = snprintf_s(registry_file_path, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s%s/%s.json",
        JSONCHEMAS_CONFG_FILE_PREFIX, REGISTRY_LOCATION_ODATAID, event_registry_id);
    return_val_do_info_if_fail(ret > 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: format string failed, ret is %d", __FUNCTION__, ret));

    file_path_lower = g_ascii_strdown(registry_file_path, -1);
    return_val_do_info_if_fail(NULL != file_path_lower, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get lower string failed", __FUNCTION__));

    
    ret = vos_get_file_accessible(file_path_lower);
    g_free(file_path_lower);

    return_val_do_info_if_fail(1 == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "cann't get registry file with id [%s]", event_registry_id));

    return VOS_OK;
}

LOCAL PROPERTY_PROVIDER_S g_event_registry_provider[] = {};


gint32 registry_rsc_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    ret = _check_event_registry_id_valid(i_paras->member_id);
    if (VOS_OK != ret) {
        debug_log(DLOG_MASS, "event registry id %s check failed, ", i_paras->member_id);

        return HTTP_NOT_FOUND;
    }
    

    *prop_provider = g_event_registry_provider;
    
    *count = 0;
    ;

    return VOS_OK;
}
