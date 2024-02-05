

#include "redfish_provider.h"
#include "redfish_provider_system.h"


LOCAL gint32 get_ld_set_return_val_message(gint32 ret, json_object *msg_array, const gchar *prop,
    const gchar *val, const gchar *extra)
{
    if (prop == NULL) {
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }

    if (ret == RET_OK) {
        return HTTP_OK;
    }

    RET_MSG_INFO_MAP ld_set_ret_val_msg_map[] = {
        { SML_ERR_REBOOT_REQUIRED, HTTP_OK, MSGID_VOLUME_SET_SUCC, NULL, NULL, NULL, NULL},
        { SM_CODE_NOT_SUPPORT_IN_PRESENT_STATE, HTTP_BAD_REQUEST, MSGID_MODIFED_WITH_OS_STATE, prop, prop, NULL, NULL},
        { SML_ERR_CTRL_STATUS_INVALID, HTTP_BAD_REQUEST, MSGID_MODIFED_WITH_OS_STATE, prop, prop, NULL, NULL},
        { SML_ERR_INVALID_PARAMETER, HTTP_BAD_REQUEST, MSGID_MODIFY_FAIL_WITH_CACHECADE, prop, prop, NULL, NULL},
        { SML_ERR_LD_PROPERTY_SET_NOT_ALLOWED, HTTP_BAD_REQUEST, MSGID_MODIFY_FAIL_WITH_CACHECADE, prop, prop,
            NULL, NULL},
        { SML_ERR_LD_OPERATION_NOT_SUPPORT, HTTP_BAD_REQUEST, MSGID_MODIFY_FAIL_WITH_RAID_NOT_SUPPORT, prop, prop,
            NULL, NULL},
        { SM_CODE_PARA_DATA_ILLEGAL, HTTP_BAD_REQUEST, MSGID_PROP_NOT_IN_LIST, prop, val, prop, NULL},
        { SML_ERR_LD_NO_SSCD_OR_INVALID_NUM_OF_SSCD, HTTP_BAD_REQUEST, MSGID_NO_CACHECADE_VOLUME, NULL, NULL,
            NULL, NULL},
        { SML_ERR_LD_PROPERTY_SET_ONLY_DEFAULT_ALLOWED, HTTP_BAD_REQUEST, MSGID_MODIFY_FAIL_WITH_SSD, prop, prop,
            NULL, NULL},
        { SML_ERR_LD_SET_CACHING_ENABLE_FOR_LD_WITH_SSD, HTTP_BAD_REQUEST, MSGID_MODIFY_FAIL_WITH_SSD, prop, prop,
            NULL, NULL},
        { SM_CODE_LD_NAME_INVALID_ASCII, HTTP_BAD_REQUEST, MSGID_INVALID_VOLUME_NAME, NULL, NULL, NULL, NULL},
        { SML_ERR_INVALID_POLICY, HTTP_BAD_REQUEST, MSGID_PROP_INVALID_VALUE, prop, "", prop, NULL},
        { SM_CODE_LD_NAME_EXCEED_MAX_LEN, HTTP_BAD_REQUEST, MSGID_PROP_VAL_EXC_MAXLEN, prop, val, prop, extra},
        { SML_ERR_CONFIG_INVALID_PD_NON_SDD_FOR_CACHECADE, HTTP_BAD_REQUEST, MSGID_PHY_NOT_SSD, prop, val, prop, NULL},
        { SML_ERR_CTRL_BBU_STATUS_ABNORMAL, HTTP_BAD_REQUEST, MSGID_PROPERTY_BBU_STATE_NOT_SUPPORT, prop, NULL,
            NULL, NULL},
        { SML_ERR_LD_SIZE_SHRINK_NOT_ALLOWED, HTTP_BAD_REQUEST, MSGID_VOLUME_SHRINK_NOT_ALLOWED, prop, NULL,
            NULL, NULL},
        { SML_ERR_CONFIG_INVALID_PARAM_CAPACITY_TOO_LARGE, HTTP_BAD_REQUEST, MSGID_VOLUME_CAPACITY_ERR, prop, prop,
            NULL, NULL},
        { SML_ERR_LD_INVALID_POLICY, HTTP_BAD_REQUEST, MSGID_PROP_INVALID_VALUE, prop, "", prop, NULL},
        { SML_ERR_OPERATION_NOT_POSSIBLE, HTTP_BAD_REQUEST, MSGID_PROP_MODIFY_UNSUPPORT, prop, prop, NULL, NULL},
        { SML_ERR_LD_STATE_UNSUPPORTED_TO_SET, HTTP_BAD_REQUEST, MSGID_OPERATION_FAILED, prop, NULL, NULL, NULL},
        { SML_ERR_CONFIG_OPERATION_NOT_SUPPORT, HTTP_BAD_REQUEST, MSGID_OPERATION_FAILED, prop, NULL, NULL, NULL},
        { SM_CODE_OPERATION_IN_PROGRESS, HTTP_BAD_REQUEST, VMM_MESSAGE_OPERATIONINPROCESS, NULL, NULL, NULL, NULL},
    };

    gsize map_size = sizeof(ld_set_ret_val_msg_map) / sizeof(RET_MSG_INFO_MAP);

    return get_ret_message(ret, ld_set_ret_val_msg_map, map_size, msg_array);
}


LOCAL gint32 set_oem_volume_name(json_object *huawei, PROVIDER_PARAS_S *i_paras, json_object *o_message_jso,
    OBJ_HANDLE ld_obj_handle)
{
#define MAX_NAME_LEN_NUM    8
    gint32 ret;
    gint32 response_code;
    const gchar *name = NULL;
    GSList *ld_input_list = NULL;
    json_object *ld_message = NULL;
    const char *volume_name_err = "Oem/Huawei/VolumeName";
    json_object *oem_property = NULL;

    if (json_object_object_get_ex(huawei, RFPROP_VOLUME_NAME, &oem_property) == FALSE) {
        return HTTP_NOT_FOUND;
    }

    

    if (oem_property == NULL) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, volume_name_err, &ld_message, RF_VALUE_NULL, volume_name_err);
        
        ret = json_object_array_add(o_message_jso, ld_message);
        if (ret != 0) {
            (void)json_object_put(ld_message);
            debug_log(DLOG_ERROR, "%s:json_object_array_add fail", __FUNCTION__);
        }
        return HTTP_BAD_REQUEST;
    }

    name = dal_json_object_get_str(oem_property);
    if (name == NULL) {
        return RET_ERR;
    }

    ld_input_list = g_slist_append(ld_input_list, g_variant_new_string(name));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ld_obj_handle,
        CLASS_LOGICAL_DRIVE_NAME, METHOD_LOGICAL_DRIVE_SET_NAME, AUTH_DISABLE, 0, ld_input_list, NULL);
    uip_free_gvariant_list(ld_input_list);

    debug_log(DLOG_DEBUG, "%s:set volume name return 0x%x", __FUNCTION__, ret);

    guint8 ctrl_id;
    const gchar *extra = NULL;
    gchar max_len_str[MAX_NAME_LEN_NUM] = {0};
    if (ret == SM_CODE_LD_NAME_EXCEED_MAX_LEN && dal_get_ctrl_id_by_ld_handle(ld_obj_handle, &ctrl_id) == RET_OK) {
        (void)snprintf_s(max_len_str, MAX_NAME_LEN_NUM, MAX_NAME_LEN_NUM - 1, "%zu", dal_get_ld_name_max_len(ctrl_id));
        extra = max_len_str;
    }

    response_code = get_ld_set_return_val_message(ret, o_message_jso, volume_name_err, name, extra);

    return response_code;
}


LOCAL gint32 read_policy_str_to_num(const gchar *read_policy_str, guint8 *read_policy_num)
{
    if (read_policy_str == NULL || read_policy_num == NULL) {
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__);
        return RET_ERR;
    }

    if (g_strcmp0(VOLUME_READ_POLICY_NOREADAHEAD, read_policy_str) == 0) {
        *read_policy_num = LD_CACHE_NO_READ_AHEAD;
    } else if (g_strcmp0(VOLUME_READ_POLICY_READAHEAD, read_policy_str) == 0) {
        *read_policy_num = LD_CACHE_READ_AHEAD;
    } else {
        debug_log(DLOG_DEBUG, "%s:invalid read policy is %s", __FUNCTION__, read_policy_str);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 set_oem_default_read_policy(json_object *huawei, PROVIDER_PARAS_S *i_paras, json_object *o_message_jso,
    OBJ_HANDLE ld_obj_handle)
{
    gint32 ret;
    gint32 response_code = HTTP_BAD_REQUEST;
    const gchar *read_policy = NULL;
    guint8 read_policy_num = 0;
    GSList *ld_input_list = NULL;
    const gchar *read_policy_err = "Oem/Huawei/DefaultReadPolicy";
    json_object *oem_property = NULL;

    if (json_object_object_get_ex(huawei, RFPROP_VOLUME_DEFAULT_READ_POLICY, &oem_property) == FALSE) {
        return HTTP_NOT_FOUND;
    }

    read_policy = dal_json_object_get_str(oem_property);

    ret = read_policy_str_to_num(read_policy, &read_policy_num);
    if (ret != RET_OK) {
        return response_code;
    }

    ld_input_list = g_slist_append(ld_input_list, g_variant_new_byte(read_policy_num));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ld_obj_handle,
        CLASS_LOGICAL_DRIVE_NAME, METHOD_LOGICAL_DRIVE_SET_READ_POLICY, AUTH_DISABLE, 0, ld_input_list, NULL);
    uip_free_gvariant_list(ld_input_list);
    debug_log(DLOG_DEBUG, "%s:set volume read policy return 0x%x", __FUNCTION__, ret);

    response_code = get_ld_set_return_val_message(ret, o_message_jso, read_policy_err, NULL, NULL);

    return response_code;
}


LOCAL gint32 set_oem_default_write_policy(json_object *huawei, PROVIDER_PARAS_S *i_paras, json_object *o_message_jso,
    OBJ_HANDLE ld_obj_handle)
{
    gint32 ret;
    gint32 response_code;
    GSList *input_list = NULL;
    const gchar *write_policy_str = NULL;
    const gchar *write_policy_err = "Oem/Huawei/DefaultWritePolicy";
    json_object *write_object = NULL;

    if (json_object_object_get_ex(huawei, RFPROP_VOLUME_DEFAULT_WRITE_POLICY, &write_object) == FALSE) {
        return HTTP_NOT_FOUND;
    }

    
    write_policy_str = dal_json_object_get_str(write_object);
    if (write_policy_str == NULL) {
        return RET_ERR;
    }

    if (g_strcmp0(VOLUME_WRITE_POLICY_WRITETHROUGH, write_policy_str) == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(LD_CACHE_WRITE_THROUGH));
    } else if (g_strcmp0(VOLUME_WRITE_POLICY_WRITEBACKWITHBBU, write_policy_str) == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(LD_CACHE_WRITE_BACK));
    } else if (g_strcmp0(VOLUME_WRITE_POLICY_WRITEBACK, write_policy_str) == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(LD_CACHE_WRITE_CACHE_IF_BAD_BBU));
    } else {
        return RET_ERR;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ld_obj_handle,
        CLASS_LOGICAL_DRIVE_NAME, METHOD_LOGICAL_DRIVE_SET_WRITE_POLICY, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);
    debug_log(DLOG_DEBUG, "%s:set volume write policy return 0x%x", __FUNCTION__, ret);

    response_code = get_ld_set_return_val_message(ret, o_message_jso, write_policy_err, NULL, NULL);

    return response_code;
}


LOCAL gint32 set_oem_default_cache_policy(json_object *huawei, PROVIDER_PARAS_S *i_paras, json_object *o_message_jso,
    OBJ_HANDLE ld_obj_handle)
{
    gint32 ret;
    gint32 response_code;
    GSList *input_list = NULL;
    const gchar *default_cache_policy = NULL;
    const gchar *cache_policy_err = "Oem/Huawei/DefaultCachePolicy";
    json_object *cache_object = NULL;

    if (json_object_object_get_ex(huawei, RFPROP_VOLUME_DEFAULT_CACHE_POLICY, &cache_object) == FALSE) {
        return HTTP_NOT_FOUND;
    }

    default_cache_policy = dal_json_object_get_str(cache_object);
    if (default_cache_policy == NULL) {
        return RET_ERR;
    }

    if (g_strcmp0(VOLUME_CACHE_POLICY_CACHEIO, default_cache_policy) == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(LD_CACHE_CACHED_IO));
    } else if (g_strcmp0(VOLUME_CACHE_POLICY_DRECTIO, default_cache_policy) == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(LD_CACHE_DIRECT_IO));
    } else {
        return RET_ERR;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ld_obj_handle,
        CLASS_LOGICAL_DRIVE_NAME, METHOD_LOGICAL_DRIVE_SET_IO_POLICY, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);
    debug_log(DLOG_DEBUG, "%s:set volume cache policy return 0x%x", __FUNCTION__, ret);

    response_code = get_ld_set_return_val_message(ret, o_message_jso, cache_policy_err, NULL, NULL);

    return response_code;
}


LOCAL gint32 set_oem_access_policy(json_object *huawei, PROVIDER_PARAS_S *i_paras, json_object *o_message_jso,
    OBJ_HANDLE ld_obj_handle)
{
    gint32 ret;
    gint32 response_code;
    GSList *input_list = NULL;
    const gchar *set_access_str = NULL;
    const gchar *error_access_policy = "Oem/Huawei/AccessPolicy";
    json_object *current_access_object = NULL;

    if (json_object_object_get_ex(huawei, RFPROP_VOLUME_ACCESS_POLICY, &current_access_object) == FALSE) {
        return HTTP_NOT_FOUND;
    }

    
    set_access_str = dal_json_object_get_str(current_access_object);
    if (set_access_str == NULL) {
        return RET_ERR;
    }

    if (g_strcmp0(VOLUME_ACCESS_POLICY_READWRITE, set_access_str) == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(LD_ACCESS_RW));
    } else if (g_strcmp0(VOLUME_ACCESS_POLICY_READONLY, set_access_str) == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(LD_ACCESS_READ_ONLY));
    } else if (g_strcmp0(VOLUME_ACCESS_POLICY_BLOCKED, set_access_str) == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(LD_ACCESS_BLOCKED));
    } else {
        json_object *ld_message = NULL;
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, error_access_policy, &ld_message, set_access_str,
            error_access_policy);
        (void)json_object_array_add(o_message_jso, ld_message);
        return RET_ERR;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ld_obj_handle,
        CLASS_LOGICAL_DRIVE_NAME, METHOD_LOGICAL_DRIVE_SET_ACCESS_POLICY, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);
    debug_log(DLOG_DEBUG, "%s:set volume access policy return 0x%x", __FUNCTION__, ret);

    response_code = get_ld_set_return_val_message(ret, o_message_jso, error_access_policy, VOLUME_ACCESS_POLICY_HIDDEN,
        NULL);

    return response_code;
}


LOCAL gint32 set_oem_logical_physical_cache_policy(json_object *huawei, PROVIDER_PARAS_S *i_paras,
    json_object *o_message_jso, OBJ_HANDLE ld_obj_handle)
{
    gint32 ret;
    gint32 response_code;
    GSList *input_list = NULL;
    const gchar *set_physical_cache_str = NULL;
    const gchar *error_disk_cache_policy = "Oem/Huawei/DriveCachePolicy";
    json_object *physical_cache_object = NULL;

    if (json_object_object_get_ex(huawei, RFPROP_VOLUME_DRIVE_CACHE_POLICY, &physical_cache_object) == FALSE) {
        return HTTP_NOT_FOUND;
    }

    set_physical_cache_str = dal_json_object_get_str(physical_cache_object);
    if (set_physical_cache_str == NULL) {
        return RET_ERR;
    }

    if (g_strcmp0(VOLUME_DISK_CACHE_POLICY_ENABLE, set_physical_cache_str) == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(PD_CACHE_ENABLE));
    } else if (g_strcmp0(VOLUME_DISK_CACHE_POLICY_DISABLE, set_physical_cache_str) == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(PD_CACHE_DISABLE));
    } else if (g_strcmp0(VOLUME_DISK_CACHE_POLICY_UNCHANGED, set_physical_cache_str) == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(PD_CACHE_UNCHANGED));
    } else {
        return RET_ERR;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ld_obj_handle,
        CLASS_LOGICAL_DRIVE_NAME, METHOD_LOGICAL_DRIVE_SET_DISK_CACHE_POLICY, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);
    debug_log(DLOG_DEBUG, "%s:set pd cache policy return 0x%x", __FUNCTION__, ret);

    response_code = get_ld_set_return_val_message(ret, o_message_jso, error_disk_cache_policy, NULL, NULL);

    return response_code;
}


LOCAL gint32 set_oem_logical_bgi(json_object *huawei, PROVIDER_PARAS_S *i_paras, json_object *o_message_jso,
    OBJ_HANDLE ld_obj_handle)
{
    gint32 ret;
    gint32 response_code;
    json_object *ld_message = NULL;
    GSList *input_list = NULL;
    const gchar *error_bgi = "Oem/Huawei/BGIEnabled";
    json_object *logical_bgi_object = NULL;

    if (json_object_object_get_ex(huawei, RFPROP_VOLUME_BGI_ENABLE, &logical_bgi_object) == FALSE) {
        return HTTP_NOT_FOUND;
    }

    if (logical_bgi_object == NULL) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, error_bgi, &ld_message, RF_VALUE_NULL, error_bgi);
        
        ret = json_object_array_add(o_message_jso, ld_message);
        if (ret != 0) {
            (void)json_object_put(ld_message);
            debug_log(DLOG_ERROR, "%s:json_object_array_add fail", __FUNCTION__);
        }
        return HTTP_BAD_REQUEST;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(json_object_get_boolean(logical_bgi_object)));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ld_obj_handle,
        CLASS_LOGICAL_DRIVE_NAME, METHOD_LOGICAL_DRIVE_SET_BGI, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);
    debug_log(DLOG_DEBUG, "%s:set volume bgi enable return 0x%x", __FUNCTION__, ret);

    response_code = get_ld_set_return_val_message(ret, o_message_jso, error_bgi, NULL, NULL);

    return response_code;
}


LOCAL gint32 set_oem_logical_cachecade(json_object *huawei, PROVIDER_PARAS_S *i_paras, json_object *o_message_jso,
    OBJ_HANDLE ld_obj_handle)
{
    gint32 ret;
    gint32 response_code;
    json_object *ld_message = NULL;
    GSList *input_list = NULL;
    const gchar *error_cachecade = "Oem/Huawei/SSDCachingEnable";
    json_object *logical_cachecade_object = NULL;

    if (json_object_object_get_ex(huawei, RFPROP_VOLUME_SSD_CACHING_ENABLE, &logical_cachecade_object) == FALSE) {
        return HTTP_NOT_FOUND;
    }

    if (logical_cachecade_object == NULL) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, error_cachecade, &ld_message, RF_VALUE_NULL, error_cachecade);
        
        ret = json_object_array_add(o_message_jso, ld_message);
        if (ret != 0) {
            (void)json_object_put(ld_message);
            debug_log(DLOG_ERROR, "%s:json_object_array_add fail", __FUNCTION__);
        }
        return HTTP_BAD_REQUEST;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(json_object_get_boolean(logical_cachecade_object)));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ld_obj_handle,
        CLASS_LOGICAL_DRIVE_NAME, METHOD_LOGICAL_DRIVE_SET_CACHECADE, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);
    debug_log(DLOG_DEBUG, "%s:set volume SSD caching enable return 0x%x", __FUNCTION__, ret);

    response_code = get_ld_set_return_val_message(ret, o_message_jso, error_cachecade, NULL, NULL);

    return response_code;
}


LOCAL gint32 set_oem_volume_boot_enable(json_object *huawei, PROVIDER_PARAS_S *i_paras, json_object *o_message_jso,
    OBJ_HANDLE ld_obj_handle)
{
    gint32 ret;
    gint32 response_code = HTTP_BAD_REQUEST;
    json_object *ld_message = NULL;
    const gchar *err_boot_enable = "Oem/Huawei/BootEnable";
    GSList *input_list = NULL;
    json_object *oem_property = NULL;

    if (json_object_object_get_ex(huawei, RFPROP_VOLUME_BOOT_ABLE, &oem_property) == FALSE) {
        return HTTP_NOT_FOUND;
    }

    if ((oem_property == NULL) || (json_object_get_type(oem_property) != json_type_boolean)) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, err_boot_enable, &ld_message, RF_VALUE_NULL, err_boot_enable);
        
        ret = json_object_array_add(o_message_jso, ld_message);
        if (ret != 0) {
            (void)json_object_put(ld_message);
            debug_log(DLOG_ERROR, "%s:json_object_array_add fail", __FUNCTION__);
        }
        return response_code;
    }

    gboolean boot_enable = json_object_get_boolean(oem_property);
    const gchar* boot_priority_str = dal_json_object_get_str(oem_property);

    input_list = g_slist_append(input_list, g_variant_new_byte(boot_enable));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ld_obj_handle,
        CLASS_LOGICAL_DRIVE_NAME, METHOD_LOGICAL_DRIVE_SET_BOOTABLE, AUTH_DISABLE, 0, input_list, NULL);
    debug_log(DLOG_DEBUG, "%s:set volume boot enable return 0x%x", __FUNCTION__, ret);
    
    uip_free_gvariant_list(input_list);

    response_code = get_ld_set_return_val_message(ret, o_message_jso, err_boot_enable, boot_priority_str, NULL);

    return response_code;
}


LOCAL gint32 set_oem_volume_boot_priority(json_object *huawei, PROVIDER_PARAS_S *i_paras, json_object *o_message_jso,
    OBJ_HANDLE ld_obj_handle)
{
    gint32 ret;
    json_object *ld_message = NULL;
    const gchar *err_boot_priority = "Oem/Huawei/BootPriority";
    GSList *input_list = NULL;
    json_object *ld_boot_priority_obj = NULL;

    if (json_object_object_get_ex(huawei, RFPROP_VOLUME_BOOT_PRIORITY, &ld_boot_priority_obj) == FALSE) {
        return HTTP_NOT_FOUND;
    }

    const gchar* boot_priority_str = dal_json_object_get_str(ld_boot_priority_obj);
    guint8 boot_priority = sml_ctrl_boot_priority_str2num(boot_priority_str);
    if (boot_priority == BOOT_PRIORITY_INVALID) {
        (void)create_message_info(MSGID_PROP_INVALID_VALUE, err_boot_priority, &ld_message,
            boot_priority_str, err_boot_priority);
        (void)json_object_array_add(o_message_jso, ld_message);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(boot_priority));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ld_obj_handle,
        CLASS_LOGICAL_DRIVE_NAME, METHOD_LOGICAL_DRIVE_SET_BOOTABLE, AUTH_DISABLE, 0, input_list, NULL);
    debug_log(DLOG_DEBUG, "%s:set volume boot priority return 0x%x", __FUNCTION__, ret);
    
    uip_free_gvariant_list(input_list);

    return get_ld_set_return_val_message(ret, o_message_jso, err_boot_priority, NULL, NULL);
}

LOCAL gint32 set_oem_accelerator(json_object *huawei, PROVIDER_PARAS_S *i_paras, json_object *o_message_jso,
    OBJ_HANDLE ld_obj_handle)
{
    gint32 ret;
    GSList *input_list = NULL;
    const gchar *set_accelerator_str = NULL;
    const gchar *error_accelerator = "Oem/Huawei/"REPROP_VOLUME_ACCELERATION_METHOD;
    json_object *accelerator_object = NULL;
    json_object *message = NULL;

    if (json_object_object_get_ex(huawei, REPROP_VOLUME_ACCELERATION_METHOD, &accelerator_object) == FALSE) {
        return HTTP_NOT_FOUND;
    }

    
    set_accelerator_str = dal_json_object_get_str(accelerator_object);
    if (set_accelerator_str == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
        (void)json_object_array_add(o_message_jso, message);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // RAID卡不支持直接将加速方法设置为maxCache
    guint8 accelerator = INVALID_DATA_BYTE;
    (void)dal_get_property_value_byte(ld_obj_handle, PROPERTY_LOGICAL_DRIVE_ACCELERATION_METHOD, &accelerator);
    if (accelerator == LD_ACCELERATOR_MAXCACHE) {
        (void)create_message_info(MSGID_MODIFY_FAIL_WITH_RAID_NOT_SUPPORT, error_accelerator, &message,
            error_accelerator);
        (void)json_object_array_add(o_message_jso, message);
        return HTTP_BAD_REQUEST;
    }

    if (g_strcmp0(set_accelerator_str, "None") == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(LD_ACCELERATOR_NONE));
    } else if (g_strcmp0(set_accelerator_str, "ControllerCache") == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(LD_ACCELERATOR_CACHE));
    } else if (g_strcmp0(set_accelerator_str, "IOBypass") == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(LD_ACCELERATOR_IOBYPASS));
    } else {
        (void)create_message_info(MSGID_PROP_INVALID_VALUE, REPROP_VOLUME_ACCELERATION_METHOD, &message,
            set_accelerator_str, REPROP_VOLUME_ACCELERATION_METHOD);
        (void)json_object_array_add(o_message_jso, message);
        return HTTP_BAD_REQUEST;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ld_obj_handle,
        CLASS_LOGICAL_DRIVE_NAME, METHOD_LOGICAL_DRIVE_SET_ACCELERATION_METHOD, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);

    return get_ld_set_return_val_message(ret, o_message_jso, error_accelerator, set_accelerator_str, NULL);
}



gint32 set_systems_storages_ld_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    typedef gint32 (*SET_VOLUME_FN)(json_object * huawei, PROVIDER_PARAS_S * i_paras, json_object * o_message_jso,
        OBJ_HANDLE ld_obj_handle);
    const SET_VOLUME_FN ctrl_fn_arr[] = {
        
        set_oem_volume_name,
        
        set_oem_default_read_policy,
        
        set_oem_default_write_policy,
        
        set_oem_default_cache_policy,
        
        set_oem_access_policy,
        
        set_oem_logical_physical_cache_policy,
        
        set_oem_logical_bgi,
        
        set_oem_logical_cachecade,
        
        set_oem_volume_boot_enable,
        
        set_oem_volume_boot_priority,
        
        set_oem_accelerator,
    };

    gint32 response_code = HTTP_BAD_REQUEST;
    json_object *huawei = NULL;

    if (o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s NULL pointer", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!(i_paras->user_role_privilege & USERROLE_BASICSETTING)) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    OBJ_HANDLE ld_obj_handle = i_paras->obj_handle;

    
    *o_message_jso = json_object_new_array();
    if (*o_message_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_array fail", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    (void)json_object_object_get_ex(i_paras->val_jso, RFPROP_COMMON_MANUFACTURER, &huawei);

    for (size_t i = 0; i < G_N_ELEMENTS(ctrl_fn_arr); i++) {
        if (ctrl_fn_arr[i](huawei, i_paras, *o_message_jso, ld_obj_handle) == HTTP_OK) {
            response_code = HTTP_OK;
        }
    }

    return response_code;
}

gint32 set_systems_storages_ld_capacity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
#define B_TO_MB_RATIO    (1024 * 1024)

    *o_message_jso = json_object_new_array();
    if (*o_message_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_array fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object *ld_message = NULL;
    if (!(i_paras->user_role_privilege & USERROLE_BASICSETTING)) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &ld_message);
        (void)json_object_array_add(*o_message_jso, ld_message);
        return HTTP_FORBIDDEN;
    }

    if (json_object_get_type(i_paras->val_jso) != json_type_int) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_VOLUME_CAPACITY, &ld_message, RF_NULL_STR,
            RFPROP_VOLUME_CAPACITY);
        (void)json_object_array_add(*o_message_jso, ld_message);
        return HTTP_BAD_REQUEST;
    }

    guint64 capacity = json_object_get_uint64(i_paras->val_jso);
    guint32 capacity_mb = (guint32)(capacity / B_TO_MB_RATIO);
    if (capacity % B_TO_MB_RATIO != 0) {
        capacity_mb += 1; // 如果字节折算成MB后有裕量，则向上按MB取整
    }
    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_uint32(capacity_mb));
    input_list = g_slist_append(input_list, g_variant_new_byte(CAPACITY_UNIT_MB));
    gint32 ret =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->obj_handle,
            CLASS_LOGICAL_DRIVE_NAME, METHOD_LOGICAL_DRIVE_SET_CAPACITY, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);

    return get_ld_set_return_val_message(ret, *o_message_jso, RFPROP_VOLUME_CAPACITY, NULL, NULL);
}

LOCAL gint32 convert_strip_size(guint32 strip_size, guint8 *strip_size_as_para)
{
    if (strip_size % STRIPE_SIZE_16K_SCHE != 0) {
        return RET_ERR;
    }

    strip_size /= STRIPE_SIZE_16K_SCHE;

    *strip_size_as_para = 5;  // 5=16K
    while (strip_size > 1) {
        if (strip_size % 2 != 0) {  // 只能为2的幂
            return RET_ERR;
        }
        strip_size >>= 1;
        (*strip_size_as_para)++;
    }
    return RET_OK;
}

gint32 set_systems_storages_ld_optimum_io_size(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    *o_message_jso = json_object_new_array();
    if (*o_message_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_array fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object *ld_message = NULL;
    if (!(i_paras->user_role_privilege & USERROLE_BASICSETTING)) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &ld_message);
        (void)json_object_array_add(*o_message_jso, ld_message);
        return HTTP_FORBIDDEN;
    }

    if (json_object_get_type(i_paras->val_jso) != json_type_int) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_VOLUME_OPTIMUMIOSIZE, &ld_message, RF_NULL_STR,
            RFPROP_VOLUME_OPTIMUMIOSIZE);
        (void)json_object_array_add(*o_message_jso, ld_message);
        return HTTP_BAD_REQUEST;
    }

    guint32 strip_size = json_object_get_int(i_paras->val_jso);
    guint8 strip_size_as_para = 0;
    if (convert_strip_size(strip_size, &strip_size_as_para) != RET_OK) {
        (void)create_message_info(MSGID_PROP_INVALID_VALUE, RFPROP_VOLUME_OPTIMUMIOSIZE, &ld_message,
            dal_json_object_get_str(i_paras->val_jso), RFPROP_VOLUME_OPTIMUMIOSIZE);
        (void)json_object_array_add(*o_message_jso, ld_message);
        return HTTP_BAD_REQUEST;
    }

    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_byte(strip_size_as_para));
    gint32 ret =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->obj_handle,
            CLASS_LOGICAL_DRIVE_NAME, METHOD_LOGICAL_DRIVE_SET_STRIP_SIZE, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);

    return get_ld_set_return_val_message(ret, *o_message_jso, RFPROP_VOLUME_OPTIMUMIOSIZE, NULL, NULL);
}