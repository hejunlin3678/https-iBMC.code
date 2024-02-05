
#include "redfish_provider.h"
#include "redfish_util.h"

LOCAL gint32 get_system_storage_drive_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_drive_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_drive_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_drive_modelnumber(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_drive_revision(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_drive_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_drive_capacitybytes(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_drive_failurepredicted(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_drive_protocol(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_drive_mediatype(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_drive_manufacturer(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_drive_serialnumber(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_drive_capablespeedgbs(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_drive_negotiatedspeedgbs(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_drive_lifeleftpercent(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_drive_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_system_storage_drive_location(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_drive_indicatorled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_drive_hotsparetype(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL void get_drive_oem_crypto_erase_enabled(OBJ_HANDLE obj_handle, json_object *o_huawei_jso);
LOCAL gint32 get_system_storage_drive_oem_property(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_drive_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_system_storage_drive_indicatorled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_system_storage_drive_hotsparetype(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_system_storage_drive_oem_property(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);


LOCAL gint32 get_system_storage_drive_statusindicator(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_system_storage_drive_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 action_crypto_erase(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_chassis_drive_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_drive_block_size(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);
LOCAL gint32 get_drive_rotation_speed(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_chassis_drive_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_ID, CLASS_HDD_NAME, PROPERTY_HDD_ID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_NAME, CLASS_HDD_NAME, PROPERTY_HDD_ID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_name, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_STORAGE_DRIVE_MODEL, CLASS_HDD_NAME, PROPERTY_HDD_MODEL_NUMBER, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_modelnumber, NULL, NULL, ETAG_FLAG_ENABLE},
    {REPROP_SYSTEM_STORAGE_DRIVE_STATUS, CLASS_HDD_NAME, PROPERTY_HDD_FIRMWARE_STATUS, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_status, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_STORAGE_DRIVE_REVISION, CLASS_HDD_NAME, PROPERTY_HDD_FIRMWARE_VERSION, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_revision, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_SYSTEM_STORAGE_DRIVE_CAPACITYBYTES, CLASS_HDD_NAME, PROPERTY_HDD_CAPACITY_MB, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_capacitybytes, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_SYSTEM_STORAGE_DRIVE_BLOCK_SIZE, CLASS_HDD_NAME, PROPERTY_HDD_BLOCK_SIZE, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, get_system_storage_drive_block_size, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_STORAGE_DRIVE_FAILUREPREDICTED, CLASS_HDD_NAME, PROPERTY_HDD_PREDICTIVEFAILURE, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_failurepredicted, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_STORAGE_DRIVE_PROTOCOL, CLASS_HDD_NAME, PROPERTY_HDD_INTERFACE_TYPE_STR, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_protocol, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_STORAGE_DRIVE_MEDIATYPE, CLASS_HDD_NAME, PROPERTY_HDD_MEDIA_TYPE, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_mediatype, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_STORAGE_DRIVE_MANUFACTURER, CLASS_HDD_NAME, PROPERTY_HDD_MANUFACTURER, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_manufacturer, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_STORAGE_DRIVE_SERIAL_NUMBER, CLASS_HDD_NAME, PROPERTY_HDD_SERIAL_NUMBER, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_serialnumber, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_STORAGE_DRIVE_CAPABLE_SPEEDGBS, CLASS_HDD_NAME, PROPERTY_HDD_INTERFACE_SPEED, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_capablespeedgbs, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_STORAGE_DRIVE_NEGOTIATED_SPEEDGBS, CLASS_HDD_NAME, PROPERTY_HDD_LINK_SPEED, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_negotiatedspeedgbs, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_STORAGE_DRIVE_PREDICTED_MEDIA_LIFEFT_PERCENT, CLASS_HDD_NAME, PROPERTY_HDD_REMNANT_MEDIA_WEAROUT, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_lifeleftpercent, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_SYSTEM_STORAGE_DRIVE_STATUSINDICATOR, CLASS_HDD_NAME, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_statusindicator, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_DRIVE_INDICATORLED, CLASS_HDD_NAME, PROPERTY_HDD_DISKLED, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, get_system_storage_drive_indicatorled, set_system_storage_drive_indicatorled, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_DRIVE_HOTSPARETYPE, CLASS_HDD_NAME, PROPERTY_HDD_HOT_SPARE, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_system_storage_drive_hotsparetype, set_system_storage_drive_hotsparetype, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_DRIVE_LOCATION, CLASS_HDD_NAME, PROPERTY_HDD_PLANE_ID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_location, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_DRIVE_ROTATION_SPEED_RPM, CLASS_HDD_NAME, PROPERTY_HDD_ROTATION_SPEED, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_drive_rotation_speed, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_OEM, CLASS_HDD_NAME, PROPERTY_HDD_REBUILD_STATE, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_system_storage_drive_oem_property, set_system_storage_drive_oem_property, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_DRIVE_LINKS, CLASS_HDD_NAME, CLASS_HDD_NAME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_links, NULL, NULL, ETAG_FLAG_ENABLE},
    
    
    {RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_drive_actions, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFACTION_DRIVE_CRYPTO_ERASE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, action_crypto_erase, ETAG_FLAG_ENABLE}
    
};

LOCAL gint32 get_system_sdcard_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_sdcard_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_sdcard_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_sdcard_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_sdcard_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_sdcard_manufacturer(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_sdcard_capacitybytes(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_sdcard_serialnumber(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_sdcard_oem_porperty(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_sdcard_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_sdcard_location(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_hot_spare(PROVIDER_PARAS_S *i_paras, json_object *i_hotspare_jso, json_object **o_msg_jso);


LOCAL PROPERTY_PROVIDER_S g_system_storage_sdcard_provider[] = {
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcard_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcard_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_ID, CLASS_MASS_STORAGE_NAME, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcard_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_NAME, CLASS_MASS_STORAGE_NAME, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcard_name, NULL, NULL, ETAG_FLAG_ENABLE},
    {REPROP_SYSTEM_STORAGE_DRIVE_STATUS, CLASS_MASS_STORAGE_NAME, PROPETRY_HEALTH, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcard_status, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_STORAGE_DRIVE_CAPACITYBYTES, CLASS_MASS_STORAGE_NAME, PROPETRY_VOLUME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcard_capacitybytes, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_STORAGE_DRIVE_MANUFACTURER, CLASS_MASS_STORAGE_NAME, PROPETRY_MANUFACTURER, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcard_manufacturer, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_STORAGE_DRIVE_SERIAL_NUMBER, CLASS_MASS_STORAGE_NAME, PROPETRY_SN, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcard_serialnumber, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_DRIVE_LOCATION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcard_location, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_OEM, CLASS_MASS_STORAGE_NAME, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcard_oem_porperty, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_DRIVE_LINKS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcard_links, NULL, NULL, ETAG_FLAG_ENABLE}
};

typedef struct tag_storage_drive_speedgbs_desc {
    guint8 storage_drive_speedgbs;
    gdouble details;
} STORAGE_DRIVE_SPEEDGBS_DESC;

LOCAL STORAGE_DRIVE_SPEEDGBS_DESC g_storage_drive_speedgbs[] = {
    {PD_SPEED_1P5G, (gdouble)1.5},   // 1.5Gb/s
    {PD_SPEED_3G, (gdouble)3},       // 3.0Gb/s
    {PD_SPEED_6G, (gdouble)6},       // 6.0Gb/s
    {PD_SPEED_12G, (gdouble)12},     // 12.0Gb/s
    {PD_SPEED_2P5G, (gdouble)2.5},   // 2.5Gb/s
    {PD_SPEED_5G, (gdouble)5},       // 5.0Gb/s
    {PD_SPEED_8G, (gdouble)8},       // 8.0Gb/s
    {PD_SPEED_10G, (gdouble)10},     // 10.0Gb/s
    {PD_SPEED_16G, (gdouble)16},     // 16.0Gb/s
    {PD_SPEED_20G, (gdouble)20},     // 20.0Gb/s
    {PD_SPEED_30G, (gdouble)30},     // 30.0Gb/s
    {PD_SPEED_32G, (gdouble)32},     // 32.0Gb/s
    {PD_SPEED_40G, (gdouble)40},     // 40.0Gb/s
    {PD_SPEED_64G, (gdouble)64},     // 64.0Gb/s
    {PD_SPEED_80G, (gdouble)80},     // 80.0Gb/s
    {PD_SPEED_96G, (gdouble)96},     // 96.0Gb/s
    {PD_SPEED_128G, (gdouble)128},   // 128.0Gb/s
    {PD_SPEED_160G, (gdouble)160},   // 160.0Gb/s
    {PD_SPEED_256G, (gdouble)256},   // 256.0Gb/s
    {PD_SPEED_22P5G, (gdouble)22.5}, // 22.5Gb/s
};


gint32 redfish_system_storage_drive_string(const gchar *redfish_provider_name, OBJ_HANDLE i_obj_handle,
    json_object **o_result_jso)
{
    gchar str_buf[MAX_STRBUF_LEN + 1] = {0};

    
    gint32 ret = dal_get_property_value_string(i_obj_handle, redfish_provider_name, str_buf, sizeof(str_buf));
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get %s value fail.", __FUNCTION__, __LINE__, redfish_provider_name));

    
    dal_trim_string(str_buf, MAX_STRBUF_LEN + 1);

    
    if (strlen(str_buf) == 0 || g_strcmp0(STRING_VALUE_IS_UNDEFINED, str_buf) == 0 ||
        g_strcmp0(STRING_VALUE_IS_UNKNOWN, str_buf) == 0 || g_strcmp0(STRING_VALUE_IS_NA, str_buf) == 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_string(str_buf);
    return HTTP_OK;
}


gint32 get_object_obj_location_devicename(OBJ_HANDLE obj_handle, gchar *o_loc_dev, gint32 length)
{
    gchar location_buf[MAX_STRBUF_LEN + 1] = {0};
    gchar devicename_buf[MAX_STRBUF_LEN + 1] = {0};
    gchar class_name[MAX_NAME_SIZE] = {0};

    
    gint32 ret = dfl_get_class_name(obj_handle, class_name, MAX_NAME_SIZE);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get class name fail.", __FUNCTION__, __LINE__));

    if (0 == g_strcmp0(class_name, CLASS_PERIPHERAL_CARD_NAME)) {
        (void)strcpy_s(location_buf, sizeof(location_buf), RF_PREFIX_PERIPHERAL);

        ret = dal_get_property_value_string(obj_handle, PROPERTY_CARD_NAME, devicename_buf, sizeof(devicename_buf));
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_MASS, "%s, %d: get property PROPERTY_CARD_NAME value fail.", __FUNCTION__, __LINE__));
    } else {
        
        ret =
            dal_get_property_value_string(obj_handle, PROPERTY_COMPONENT_LOCATION, location_buf, sizeof(location_buf));
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_MASS, "%s, %d: get property PROPERTY_COMPONENT_LOCATION value fail.", __FUNCTION__,
            __LINE__));
        (void)dal_clear_string_blank(location_buf, sizeof(location_buf));

        ret = dal_get_property_value_string(obj_handle, PROPERTY_COMPONENT_DEVICE_NAME, devicename_buf,
            sizeof(devicename_buf));
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_MASS, "%s, %d: get property PROPERTY_COMPONENT_DEVICE_NAME value fail.", __FUNCTION__,
            __LINE__));
    }
    

    (void)dal_clear_string_blank(devicename_buf, sizeof(devicename_buf));
    return_val_do_info_if_expr((0 == strlen(location_buf)) || (0 == strlen(devicename_buf)), VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get property string is null.", __FUNCTION__, __LINE__));

    ret = snprintf_s(o_loc_dev, length, length - 1, "%s%s", location_buf, devicename_buf);
    return_val_do_info_if_fail(0 < ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s %d:snprintf_s fail.", __FUNCTION__, __LINE__));

    return VOS_OK;
}

LOCAL gint32 get_sdcard_obj_handle(OBJ_HANDLE obj_handle, OBJ_HANDLE *o_obj_handle)
{
    guchar device_num = 0;
    
    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICENUM, &device_num);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s : %d get property value fail", __FUNCTION__, __LINE__));

    ret = dal_get_specific_object_byte(CLASS_MASS_STORAGE_NAME, PROPETRY_ID, device_num, o_obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    return VOS_OK;
}


LOCAL gint32 check_sdcard_uri_effective_get_handle(const gchar *i_member_id, OBJ_HANDLE *o_obj_handle)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guchar component_type = 0;
    guchar presence = 0;
    gchar sdcard_id[MAX_STRBUF_LEN + 1] = {0};
    gint32 match_flag = 1;

    
    gint32 ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
        continue_if_expr(ret != VOS_OK);
        continue_if_expr(component_type != COMPONENT_TYPE_SD_CARD);

        
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_PRESENCE, &presence);
        continue_if_expr(ret != VOS_OK || presence == 0);

        
        ret = get_object_obj_location_devicename((OBJ_HANDLE)obj_node->data, sdcard_id, sizeof(sdcard_id));
        continue_if_expr(ret != VOS_OK);

        if (g_ascii_strcasecmp(i_member_id, sdcard_id) == 0) {
            match_flag = RF_MATCH_OK;
            break;
        }
    }

    return_val_do_info_if_expr((obj_node != NULL) && (match_flag == RF_MATCH_OK), VOS_OK,
        *o_obj_handle = (OBJ_HANDLE)obj_node->data;
        g_slist_free(obj_list));
    debug_log(DLOG_MASS, "%s, %d: the uri is invalid.\n", __FUNCTION__, __LINE__);

    g_slist_free(obj_list);
    return VOS_ERR;
}


LOCAL gint32 get_system_sdcard_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar odata_context[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    gint32 ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, SYSTEMS_STRORAGE_SDCARD_METADATA,
        slot_id);
    return_val_do_info_if_fail(0 < ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s %d  snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string(odata_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_system_sdcard_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar sdcardid_buf[MAX_STRBUF_LEN + 1] = {0};
    gchar sdcard_uri[MAX_STRBUF_LEN + 1] = {0};
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    gint32 ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s:%d fail", __FUNCTION__, __LINE__));

    ret = check_sdcard_uri_effective_get_handle(i_paras->member_id, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get obj_handle fail.", __FUNCTION__, __LINE__));

    
    ret = get_object_obj_location_devicename(obj_handle, sdcardid_buf, sizeof(sdcardid_buf));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get property fail.", __FUNCTION__, __LINE__));

    ret = snprintf_s(sdcard_uri, sizeof(sdcard_uri), sizeof(sdcard_uri) - 1, URI_FORMAT_CHASSIS_DRIVE, slot,
        sdcardid_buf);
    return_val_do_info_if_fail(0 < ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s %d : snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string(sdcard_uri);
    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_system_sdcard_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gchar sdcardid_buf[MAX_STRBUF_LEN + 1] = {0};
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    gint32 ret = check_sdcard_uri_effective_get_handle(i_paras->member_id, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get obj_handle fail.", __FUNCTION__, __LINE__));

    
    ret = get_object_obj_location_devicename(obj_handle, sdcardid_buf, sizeof(sdcardid_buf));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get property fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string(sdcardid_buf);
    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_system_sdcard_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gchar sdcardid_buf[MAX_STRBUF_LEN + 1] = {0};
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    gint32 ret = check_sdcard_uri_effective_get_handle(i_paras->member_id, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get obj_handle fail.", __FUNCTION__, __LINE__));

    
    ret = get_object_obj_location_devicename(obj_handle, sdcardid_buf, sizeof(sdcardid_buf));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get obj_handle fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string(sdcardid_buf);
    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_system_sdcard_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 health = 0;
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE sdcard_handle = 0;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    gint32 ret = check_sdcard_uri_effective_get_handle(i_paras->member_id, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get obj_handle fail.", __FUNCTION__, __LINE__));

    ret = get_sdcard_obj_handle(obj_handle, &sdcard_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get sdcard obj_handle fail.", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_byte(sdcard_handle, PROPETRY_HEALTH, &health);
    do_val_if_fail(VOS_OK == ret, health = DRIVE_INVALID_VALUE;
        debug_log(DLOG_MASS, "%s, %d: get Health value fail.", __FUNCTION__, __LINE__));

    (void)get_resource_status_property(&health, NULL, STRING_CONSET_VALUE_ENABLE, o_result_jso, FALSE);
    return HTTP_OK;
}


LOCAL gint32 get_system_sdcard_capacitybytes(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint32 sd_capacity_megabyte = 0;
    guint64 capacity_bytes;
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE sdcard_handle = 0;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    gint32 ret = check_sdcard_uri_effective_get_handle(i_paras->member_id, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get obj_handle fail.", __FUNCTION__, __LINE__));

    ret = get_sdcard_obj_handle(obj_handle, &sdcard_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get sdcard obj_handle fail.", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_uint32(sdcard_handle, PROPETRY_VOLUME, &sd_capacity_megabyte);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get sd capacity fail.", __FUNCTION__, __LINE__));

    capacity_bytes = (guint64)sd_capacity_megabyte * 1024 * 1024;
    *o_result_jso = json_object_new_int64(capacity_bytes);
    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_system_sdcard_manufacturer(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE sdcard_handle = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    gint32 ret = check_sdcard_uri_effective_get_handle(i_paras->member_id, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get obj_handle fail.", __FUNCTION__, __LINE__));

    ret = get_sdcard_obj_handle(obj_handle, &sdcard_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get obj_handle fail.", __FUNCTION__, __LINE__));

    return redfish_system_storage_drive_string(PROPETRY_MANUFACTURER, sdcard_handle, o_result_jso);
}


LOCAL gint32 get_system_sdcard_serialnumber(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE sdcard_handle = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    gint32 ret = check_sdcard_uri_effective_get_handle(i_paras->member_id, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get obj_handle fail.", __FUNCTION__, __LINE__));

    ret = get_sdcard_obj_handle(obj_handle, &sdcard_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get sdcard obj_handle fail.", __FUNCTION__, __LINE__));

    return redfish_system_storage_drive_string(PROPETRY_SN, sdcard_handle, o_result_jso);
}


LOCAL gint32 get_system_sdcard_location(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if ((NULL == o_result_jso) || (NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras))) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: new array fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 get_system_sdcard_oem_porperty(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE sdcard_handle = 0;
    json_object *oem_huawei_jso = NULL;
    json_object *json_prop = NULL;
    const gchar *sdcard_type = "SDCard";
    guchar sdrebuild_state = 0;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    gint32 ret = check_sdcard_uri_effective_get_handle(i_paras->member_id, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get obj_handle fail.", __FUNCTION__, __LINE__));

    ret = get_sdcard_obj_handle(obj_handle, &sdcard_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get obj_handle fail.", __FUNCTION__, __LINE__));

    oem_huawei_jso = json_object_new_object();
    if (NULL != oem_huawei_jso) {
        *o_result_jso = json_object_new_object();
        return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(oem_huawei_jso));

        
        json_prop = json_object_new_string(sdcard_type);
        (void)json_object_object_add(oem_huawei_jso, RFPROP_STORAGE_OEM_TYPE, json_prop);
        json_prop = NULL;

        
        ret = dal_get_property_value_byte(sdcard_handle, PROPETRY_REBUILDSTATE, &sdrebuild_state);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_OK, json_object_put(oem_huawei_jso);
            debug_log(DLOG_MASS, "%s, %d: get property value fail.", __FUNCTION__, __LINE__));

        
        switch (sdrebuild_state) {
                
            case SDCARD_SYNC_DONE:
            case SDCARD_WAITTING_SYNC:
                json_prop = json_object_new_string(RF_REBUILDSTATE_REBUILDORNOREBUILD);
                break;

                
            case SDCARD_SYNC_IN_PROGRESS:
                json_prop = json_object_new_string(RF_REBUILDSTATE_REBUILDING);
                break;

            default:
                debug_log(DLOG_MASS, "%s, %d: the value of rebuildstate is error.", __FUNCTION__, __LINE__);
        }

        (void)json_object_object_add(oem_huawei_jso, RFPROP_DRIVE_OEM_REBUILDSTATE, json_prop);
        (void)json_object_object_add(*o_result_jso, RFPROP_FWINV_HUAWEI, oem_huawei_jso);
    } else {
        
        debug_log(DLOG_MASS, "%s, %d :json object new object fail", __FUNCTION__, __LINE__);
        
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_system_sdcard_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    json_object *volumes_jso_array = NULL;

    
    if ((NULL == o_result_jso) || (NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras))) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_object();
    return_val_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);
    volumes_jso_array = json_object_new_array();
    return_val_do_info_if_fail(NULL != volumes_jso_array, HTTP_INTERNAL_SERVER_ERROR, json_object_put(*o_result_jso);
        *o_result_jso = NULL);

    (void)json_object_object_add(*o_result_jso, RF_LINKS_VOLUMES, volumes_jso_array);
    return HTTP_OK;
}


LOCAL gint32 get_system_storage_drive_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;

    gint32 ret;
    gchar odata_context[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    iRet =
        snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, CHASSIS_DRIVES_METADATA, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string(odata_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 redfish_system_storage_drive_speedgbs_result(guint8 speedgbs, gdouble *result)
{
    for (guint8 i = 0; i < G_N_ELEMENTS(g_storage_drive_speedgbs); i++) {
        if (speedgbs == g_storage_drive_speedgbs[i].storage_drive_speedgbs) {
            *result = g_storage_drive_speedgbs[i].details;
            return RET_OK;
        }
    }
    debug_log(DLOG_MASS, "%s, speedgbs value %d is error.", __FUNCTION__, speedgbs);
    return RET_ERR;
}



LOCAL gint32 redfish_get_drive_uri(OBJ_HANDLE obj_handle, gchar *hdd_uri, guint32 hdd_uri_lengh)
{
    int iRet;
    gint32 ret;
    gchar slot[MAX_RSC_ID_LEN + 1] = {0};
    gchar location_buf[MAX_STRBUF_LEN + 1] = {0};
    gchar devicename_buf[MAX_STRBUF_LEN + 1] = {0};
    
    gchar rsc_id[MAX_MEM_ID_LEN] = {0};
    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: get slot id error.\n", __FUNCTION__, __LINE__));

    (void)dal_get_property_value_string(obj_handle, PROPERTY_HDD_PYSICAL_LOCATION, location_buf, sizeof(location_buf));
    (void)dal_get_property_value_string(obj_handle, PROPERTY_HDD_DEVICENAME, devicename_buf, sizeof(devicename_buf));
    
    dal_clear_string_blank(location_buf, sizeof(location_buf));
    dal_clear_string_blank(devicename_buf, sizeof(devicename_buf));

    
    ret = snprintf_s(rsc_id, MAX_MEM_ID_LEN, MAX_MEM_ID_LEN - 1, "%s%s", location_buf, devicename_buf);
    return_val_do_info_if_fail(ret > 0, VOS_ERR, debug_log(DLOG_ERROR, "format string failed, ret is %d", ret));
    return_val_do_info_if_expr(hdd_uri_lengh == 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error, hdd_uri_lengh is 0.", __FUNCTION__));
    iRet = snprintf_s(hdd_uri, hdd_uri_lengh, hdd_uri_lengh - 1, URI_FORMAT_CHASSIS_DRIVE, slot, rsc_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    

    return VOS_OK;
}



gint32 redfish_check_every_drives_get_handle(const gchar *class_name, const gchar *i_uri, OBJ_HANDLE *o_handle)
{
    errno_t safe_fun_ret = EOK;
    guint8 presence_val = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    gchar hdd_uri[MAX_STRBUF_LEN + 1] = {0};

    gint32 ret = dfl_get_object_list(class_name, &obj_list);
    if (VOS_OK == ret && NULL != obj_list) {
        for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
            
            (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_PRESENCE, &presence_val);
            do_if_fail(RF_HDD_PRESENCE == presence_val, continue);

            (void)memset_s(hdd_uri, sizeof(hdd_uri), 0, sizeof(hdd_uri));
            ret = redfish_get_drive_uri((OBJ_HANDLE)obj_node->data, hdd_uri, sizeof(hdd_uri));
            check_fail_continue(VOS_OK == ret);

            if (0 == g_ascii_strcasecmp(i_uri, hdd_uri)) {
                *o_handle = (OBJ_HANDLE)obj_node->data;
                g_slist_free(obj_list);
                return VOS_OK;
            } else {
                if (0 == g_strcmp0(class_name, CLASS_HDD_NAME)) {
                    
                    safe_fun_ret = strncat_s(hdd_uri, sizeof(hdd_uri), ACTION_SEGMENT, strlen(ACTION_SEGMENT));
                    do_if_expr(safe_fun_ret != EOK,
                        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
                    safe_fun_ret = strncat_s(hdd_uri, sizeof(hdd_uri), RFACTION_DRIVE_CRYPTO_ERASE,
                        strlen(RFACTION_DRIVE_CRYPTO_ERASE));
                    do_if_expr(safe_fun_ret != EOK,
                        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
                    return_val_do_info_if_expr(0 == g_ascii_strcasecmp(i_uri, hdd_uri), VOS_OK,
                        *o_handle = (OBJ_HANDLE)obj_node->data;
                        g_slist_free(obj_list));
                }
            }
        }

        g_slist_free(obj_list);
    }

    return VOS_ERR;
}


LOCAL gint32 check_drive_uri_effective_get_handle(const gchar *i_uri, OBJ_HANDLE *obj_handle)
{
    gint32 ret = redfish_check_every_drives_get_handle(CLASS_HDD_NAME, i_uri, obj_handle);
    return_val_if_expr(ret == VOS_OK, VOS_OK);

    ret = redfish_check_every_drives_get_handle(CLASS_PCIEHDD_NAME, i_uri, obj_handle);
    return_val_if_expr(ret == VOS_OK, VOS_OK);

    debug_log(DLOG_MASS, "%s, %d: check uri fail.\n", __FUNCTION__, __LINE__);
    return VOS_ERR;
}


LOCAL gint32 get_system_storage_driveid_and_controllerid(OBJ_HANDLE obj_handle, guchar *controller_id, guchar *hdd_id)
{
    OBJ_HANDLE controller_obj_handle = 0;
    gchar hdd_of_controller[MAX_STRBUF_LEN + 1] = {0};

    
    gint32 ret = dal_get_property_value_string(obj_handle, PROPERTY_HDD_REF_RAID_CONTROLLER, hdd_of_controller,
        sizeof(hdd_of_controller));
    return_val_if_fail(VOS_OK == ret, ret);

    ret = dfl_get_object_handle(hdd_of_controller, &controller_obj_handle);
    return_val_if_fail(VOS_OK == ret, ret);

    ret = dal_get_property_value_byte(controller_obj_handle, PROPERTY_RAID_CONTROLLER_ID, controller_id);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_HDD_ID, hdd_id);
    return_val_if_fail(VOS_OK == ret, ret);
    return VOS_OK;
}


LOCAL void redfish_storage_drive_logical(OBJ_HANDLE obj_handle, const gchar *property_slot_name,
    const gchar *prop_enclosure_name, json_object *volumes_jso_array, const gchar *ref_raid_ctrl_obj_name,
    guint16 logical_id, guint8 hdd_id)
{
    gsize i;
    
    guint8 drive_id = 0;
    guint8 controller_id = 0;
    OBJ_HANDLE controller_handle = 0;
    
    GVariant *hdd_slot_array = NULL;
    const guint8 *hdd_slot_array_pointer = NULL;
    gsize len_hdd_array_pointer = 0;
    gchar links_buf[MAX_STRBUF_LEN + 1] = {0};
    json_object *json_links_buf = NULL;
    json_object *link_odata_id = NULL;
    gchar slot[MAX_RSC_ID_LEN + 1] = {0};

    GVariant *hdd_enclosure_array = NULL;
    const guint16 *hdd_enclosure_array_pointer = NULL;

    gint32 ret = redfish_get_board_slot(slot, sizeof(slot));
    return_do_info_if_fail(ret == VOS_OK,
        debug_log(DLOG_ERROR, "%s, %d: get slot id error.\n", __FUNCTION__, __LINE__));
    
    ret = dfl_get_object_handle(ref_raid_ctrl_obj_name, &controller_handle);
    return_do_info_if_fail(ret == VOS_OK,
        debug_log(DLOG_ERROR, "%s, %d: get controller handle fail.\n", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_ID, &controller_id);
    return_do_info_if_fail(ret == VOS_OK,
        debug_log(DLOG_ERROR, "%s, %d: get controller id fail.\n", __FUNCTION__, __LINE__));
    
    
    ret = dfl_get_property_value(obj_handle, prop_enclosure_name, &hdd_enclosure_array);
    return_if_fail(ret == VOS_OK);
    return_if_fail(hdd_enclosure_array != NULL);

    hdd_enclosure_array_pointer =
        (const guint16 *)g_variant_get_fixed_array(hdd_enclosure_array, &len_hdd_array_pointer, sizeof(guint16));
    return_do_info_if_fail(hdd_enclosure_array_pointer != NULL, g_variant_unref(hdd_enclosure_array);
        hdd_enclosure_array = NULL; debug_log(DLOG_MASS, "%s, %d: g_variant_get_fixed_array.", __FUNCTION__, __LINE__));

    ret = dfl_get_property_value(obj_handle, property_slot_name, &hdd_slot_array);
    return_do_info_if_fail(ret == VOS_OK, g_variant_unref(hdd_enclosure_array); hdd_enclosure_array = NULL);
    return_do_info_if_fail(hdd_slot_array != NULL, g_variant_unref(hdd_enclosure_array); hdd_enclosure_array = NULL);

    hdd_slot_array_pointer =
        (const guint8 *)g_variant_get_fixed_array(hdd_slot_array, &len_hdd_array_pointer, sizeof(guint8));
    return_do_info_if_fail(hdd_slot_array_pointer != NULL, g_variant_unref(hdd_slot_array); hdd_slot_array = NULL;
        g_variant_unref(hdd_enclosure_array); hdd_enclosure_array = NULL;
        debug_log(DLOG_MASS, "%s, %d: g_variant_get_fixed_array.", __FUNCTION__, __LINE__));

    for (i = 0; i < len_hdd_array_pointer; i++) {
        
        dal_get_pd_id_by_slot_and_enclosure(ref_raid_ctrl_obj_name, hdd_slot_array_pointer[i],
            hdd_enclosure_array_pointer[i], &drive_id);

        if (hdd_id == drive_id) {
            
            
            (void)memset_s(links_buf, sizeof(links_buf), 0, sizeof(links_buf));
            
            ret = snprintf_s(links_buf, sizeof(links_buf), sizeof(links_buf) - 1, RFPROP_SYSTEM_REVOLUME, slot,
                controller_id, logical_id);
            do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));
            
            link_odata_id = json_object_new_object();
            continue_if_expr_true(link_odata_id == NULL);

            json_links_buf = json_object_new_string(links_buf);
            continue_do_info_if_fail(json_links_buf != NULL, json_object_put(link_odata_id));

            json_object_object_add(link_odata_id, RFPROP_ODATA_ID, json_links_buf);

            ret = json_object_array_add(volumes_jso_array, link_odata_id);
            do_val_if_fail(ret == VOS_OK, json_object_put(link_odata_id));
        }
    }

    g_variant_unref(hdd_slot_array);
    g_variant_unref(hdd_enclosure_array);
    
    return;
}


LOCAL gint32 get_system_storage_drive_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    gchar hdd_uri[MAX_STRBUF_LEN + 1] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    gint32 ret = redfish_get_drive_uri(i_paras->obj_handle, hdd_uri, sizeof(hdd_uri));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get drive uri fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string(hdd_uri);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);
    
    return HTTP_OK;
}


gint32 redfish_get_drives_location_devicename(OBJ_HANDLE obj_handle, gchar *o_loc_dev, gint32 length)
{
    gint32 ret;
    gchar location_buf[MAX_STRBUF_LEN + 1] = {0};
    gchar devicename_buf[MAX_STRBUF_LEN + 1] = {0};

    if (NULL == o_loc_dev || 0 > length) {
        debug_log(DLOG_MASS, "%s, %d: input parameter is invalid.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    ret = dal_get_property_value_string(obj_handle, PROPERTY_HDD_PYSICAL_LOCATION, location_buf, sizeof(location_buf));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get drive location fail.", __FUNCTION__, __LINE__));
    dal_clear_string_blank(location_buf, sizeof(location_buf));

    ret = dal_get_property_value_string(obj_handle, PROPERTY_HDD_DEVICENAME, devicename_buf, sizeof(devicename_buf));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get drive location fail.", __FUNCTION__, __LINE__));
    dal_clear_string_blank(devicename_buf, sizeof(devicename_buf));

    
    if (0 == strlen(location_buf) || 0 == strlen(devicename_buf)) {
        debug_log(DLOG_MASS, "%s, %d: get location or devicename length is error fail.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    ret = snprintf_s(o_loc_dev, length, length - 1, "%s%s", location_buf, devicename_buf);
    return_val_do_info_if_expr(0 >= ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: snprintf_s fail.", __FUNCTION__, __LINE__));

    return VOS_OK;
}

void get_drive_id(OBJ_HANDLE dri_obj_handle, gchar *drives_id_str, gsize str_len)
{
    int iRet;
    gchar location_buf[MAX_STRBUF_LEN + 1] = {0};
    gchar devicename_buf[MAX_STRBUF_LEN + 1] = {0};
    return_do_info_if_fail(NULL != drives_id_str,
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    (void)dal_get_property_value_string(dri_obj_handle, PROPERTY_HDD_PYSICAL_LOCATION, location_buf,
        sizeof(location_buf));
    (void)dal_get_property_value_string(dri_obj_handle, PROPERTY_HDD_DEVICENAME, devicename_buf,
        sizeof(devicename_buf));

    
    dal_clear_string_blank(location_buf, sizeof(location_buf));
    dal_clear_string_blank(devicename_buf, sizeof(devicename_buf));
    
    return_do_info_if_expr(str_len == 0,
        debug_log(DLOG_ERROR, "%s failed:input param error, str_len is 0.", __FUNCTION__));
    iRet = snprintf_s(drives_id_str, str_len, str_len - 1, "%s%s", location_buf, devicename_buf);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    
    return;
}


LOCAL gint32 get_system_storage_drive_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar drives_id[MAX_STRBUF_LEN + 1] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    get_drive_id(i_paras->obj_handle, drives_id, MAX_STRBUF_LEN + 1);

    *o_result_jso = json_object_new_string(drives_id);

    return HTTP_OK;
}


LOCAL gint32 get_system_storage_drive_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    return redfish_system_storage_drive_string(PROPERTY_HDD_DEVICENAME, i_paras->obj_handle, o_result_jso);
}


LOCAL gint32 get_system_storage_drive_modelnumber(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return redfish_system_storage_drive_string(PROPERTY_HDD_MODEL_NUMBER, i_paras->obj_handle, o_result_jso);
}


LOCAL gint32 get_system_storage_drive_revision(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return redfish_system_storage_drive_string(PROPERTY_HDD_FIRMWARE_VERSION, i_paras->obj_handle, o_result_jso);
}


LOCAL void get_pd_status_desc(guint8 state, gchar *str_state, gsize buf_len)
{
    if (buf_len <= MAX_STATUS_LEN) {
        return;
    }

    switch (state) {
        case PD_STATE_UNCONFIGURED_GOOD:
        case PD_STATE_UNCONFIGURED_BAD:
        case PD_STATE_FAILED:
        case PD_STATE_PREDICTIVE_FAILURE:
        case PD_STATE_ONLINE:
        case PD_STATE_EPD:
        case PD_STATE_REBUILD:
        case PD_STATE_COPYBACK:
        case PD_STATE_SYSTEM:
        case PD_STATE_ACTIVE:
        case PD_STATE_DST:
        case PD_STATE_SMART:
        case PD_STATE_SCT:
        case PD_STATE_RAW:
        case PD_STATE_READY:
            (void)strcpy_s(str_state, buf_len, "Enabled");
            break;

        case PD_STATE_SHIELD_UNCONFIGURED:
        case PD_STATE_SHIELD_HOT_SPARE:
        case PD_STATE_SHIELD_CONFIGURED:
        case PD_STATE_FOREIGN:
        case PD_STATE_NOT_SUPPORTED:
        case PD_STATE_INCOMPATIBLE:
            (void)strcpy_s(str_state, buf_len, "Disabled");
            break;

        case PD_STATE_HOT_SPARE:
            (void)strcpy_s(str_state, buf_len, "StandbySpare");
            break;

        case PD_STATE_OFFLINE:
        case PD_STATE_STANDBY:
        case PD_STATE_SLEEP:
            (void)strcpy_s(str_state, buf_len, "StandbyOffline");
            break;

        case PD_STATE_DIAGNOSING:
        case PD_STATE_ERASING:
            (void)strcpy_s(str_state, buf_len, "Updating");
            break;

        default:
            debug_log(DLOG_MASS, "%s, state value %d is error.", __FUNCTION__, state);
            break;
    }

    return;
}


LOCAL void get_nvme_state(OBJ_HANDLE obj, gchar *str_state, gsize buf_len)
{
    guint16 health_code = 0;
    (void)dal_get_property_value_uint16(obj, PROPERTY_HDD_HEALTH_CODE, &health_code);
    if (health_code == STORAGE_INFO_INVALID_WORD) {
        return;
    }

    if (!(health_code & BIT_CODE_PORT0_PCIE_LINK_ACTIVE)) {
        (void)strcpy_s(str_state, buf_len, "UnavailableOffline");
        return;
    }

    if (!(health_code & BIT_DRIVE_FUNCTIONAL) || (health_code & BIT_DRIVE_NOT_READY)) {
        (void)strcpy_s(str_state, buf_len, "Disabled");
        return;
    }

    if (!(health_code & BIT_RESET_NOT_REQUIRED)) {
        (void)strcpy_s(str_state, buf_len, "StandbyOffline");
        return;
    }

    (void)strcpy_s(str_state, buf_len, "Enabled");
    return;
}


LOCAL gint32 get_system_storage_drive_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 state = 0;
    guint8 health = 0;
    gchar str_state[MAX_STATUS_LEN + 1] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    gint32 ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_HDD_HEALTH, &health);
    do_val_if_fail(VOS_OK == ret, health = 255;
        debug_log(DLOG_MASS, "%s, %d: get Health value fail.", __FUNCTION__, __LINE__));

    guint8 interface_type = 0;
    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_HDD_INTERFACE_TYPE, &interface_type);
    if (interface_type == PD_INTERFACE_TYPE_PCIE) {
        get_nvme_state(i_paras->obj_handle, str_state, sizeof(str_state));
    } else {
        
        ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_HDD_FIRMWARE_STATUS, &state);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get status value fail, ret = %d.", __FUNCTION__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        get_pd_status_desc(state, str_state, sizeof(str_state));
    }

    (void)get_resource_status_property(&health, NULL, str_state, o_result_jso, FALSE);
    return HTTP_OK;
}


LOCAL gint32 get_system_storage_drive_capacitybytes(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    guint32 capacity = 0;
    guint64 capacity_bytes;

    
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    gint32 ret = dal_get_property_value_uint32(i_paras->obj_handle, PROPERTY_HDD_CAPACITY_MB, &capacity);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get capacitybytes value fail.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_expr(RF_DEVIES_CAPACITY_ERROR == capacity, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get capacitybytes value si error.", __FUNCTION__, __LINE__));

    
    
    capacity_bytes = (guint64)capacity * 1024 * 1024;
    
    
    *o_result_jso = json_object_new_int64(capacity_bytes);
    
    return HTTP_OK;
}


LOCAL gint32 get_system_storage_drive_block_size(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso)
{
    guint16 block_size = 0;

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer or input paras error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    gint32 ret = dal_get_property_value_uint16(i_paras->obj_handle, PROPERTY_HDD_BLOCK_SIZE, &block_size);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: %s get block size failed", __FUNCTION__, dfl_get_object_name(i_paras->obj_handle));
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (block_size == INVALID_DATA_WORD || block_size == 0) {
        debug_log(DLOG_DEBUG, "%s: %s the block size %d value is invalid", __FUNCTION__,
            dfl_get_object_name(i_paras->obj_handle), block_size);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_int(block_size);
    return HTTP_OK;
}


LOCAL gint32 get_system_storage_drive_failurepredicted(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 failurepredicted = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    gint32 ret =
        dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_HDD_DEVICEPREDICTIVEFAILURE, &failurepredicted);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get failurepredicted value fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_boolean((json_bool)failurepredicted);
    return HTTP_OK;
}


LOCAL gint32 get_system_storage_drive_protocol(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar str_buf[MAX_STRBUF_LEN + 1] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_HDD_INTERFACE_TYPE_STR, str_buf, sizeof(str_buf));
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get %s value fail.", __FUNCTION__, __LINE__, PROPERTY_HDD_INTERFACE_TYPE_STR));

    
    return_val_if_expr((g_strcmp0("Undefined", str_buf) == VOS_OK || g_strcmp0("Unknown", str_buf) == VOS_OK ||
        g_strcmp0("N/A", str_buf) == VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR);

    
    if (g_strcmp0("parallel SCSI", str_buf) == VOS_OK) {
        (void)snprintf_s(str_buf, sizeof(str_buf), sizeof(str_buf) - 1, "%s", "iSCSI");
    } else if (g_strcmp0("Fiber Channel", str_buf) == VOS_OK) {
        (void)snprintf_s(str_buf, sizeof(str_buf), sizeof(str_buf) - 1, "%s", "FCoE");
    }

    
    *o_result_jso = json_object_new_string(str_buf);
    return HTTP_OK;
}

gint32 get_drive_mediatype(OBJ_HANDLE dri_obj_handle, gchar *str_buf, gsize buf_len)
{
    guint8 mediatype = 0;

    return_val_do_info_if_fail(NULL != str_buf, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    gint32 ret = dal_get_property_value_byte(dri_obj_handle, PROPERTY_HDD_MEDIA_TYPE, &mediatype);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get mediatype value fail.", __FUNCTION__, __LINE__));
    
    switch (mediatype) {
        case 0:
            return_val_do_info_if_expr(buf_len == 0, VOS_ERR,
                debug_log(DLOG_ERROR, "%s failed:input param error, buf_len is 0.", __FUNCTION__));
            (void)snprintf_s(str_buf, buf_len, buf_len - 1, "%s", "HDD");
            break;

        case 1:
            return_val_do_info_if_expr(buf_len == 0, VOS_ERR,
                debug_log(DLOG_ERROR, "%s failed:input param error, buf_len is 0.", __FUNCTION__));
            (void)snprintf_s(str_buf, buf_len, buf_len - 1, "%s", "SSD");
            break;

        case 2:
            return_val_do_info_if_expr(buf_len == 0, VOS_ERR,
                debug_log(DLOG_ERROR, "%s failed:input param error, buf_len is 0.", __FUNCTION__));
            (void)snprintf_s(str_buf, buf_len, buf_len - 1, "%s", "SSM");
            break;

        default:
            debug_log(DLOG_MASS, "%s, %d: get mediatype unknown.", __FUNCTION__, __LINE__);
            return VOS_ERR;
    }

    return VOS_OK;
}

LOCAL gint32 get_system_storage_drive_mediatype(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar str_buf[MAX_STRBUF_LEN + 1] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    gint32 ret = get_drive_mediatype(i_paras->obj_handle, str_buf, MAX_STRBUF_LEN + 1);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get mediatype fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string(str_buf);
    return HTTP_OK;
}


LOCAL gint32 get_system_storage_drive_manufacturer(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar str_buf[MAX_STRBUF_LEN + 1] = {0};
    gboolean is_customized;
    guint32 i;
    gchar* manus[][2] = {
        {"SSSTC", "Ssstc"},
        {"HYNIX", "Hynix"},
        {"LITEON", "Liteon"},
        {"INTEL", "Intel"},
        {"SAMSUNG", "Samsung"},
        {"MICRON", "Micron"},
        {"SANDISK", "Sandisk"},
        {"FUJITSU", "Fujitsu"},
        {"SEAGATE", "Seagate"},
        {"WDC", "WD"},
        {"HGST", "HGST"},
        {"TOSHIBA", "Toshiba"},
        {"SMI", "Smi"},
        {"PHISON", "Phison"}
    };

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_HDD_MANUFACTURER, str_buf, sizeof(str_buf));
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s: get %s value fail.", __FUNCTION__, PROPERTY_HDD_MANUFACTURER);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    dal_trim_string(str_buf, MAX_STRBUF_LEN + 1);

    
    if ((strlen(str_buf) == 0) || (g_strcmp0(STRING_VALUE_IS_UNDEFINED, str_buf) == 0
        || g_strcmp0(STRING_VALUE_IS_UNKNOWN, str_buf) == 0
        || g_strcmp0(STRING_VALUE_IS_NA, str_buf) == 0)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    is_customized = dal_is_customized_by_cmcc();
    if (is_customized) {
        for (i = 0; i < sizeof(manus) / sizeof(manus[0]); i++) {
            if (strcasecmp(str_buf, manus[i][0]) == 0) {
                *o_result_jso = json_object_new_string(manus[i][1]);
                return HTTP_OK;
            }
        }
    }

    *o_result_jso = json_object_new_string(str_buf);
    return HTTP_OK;
}


LOCAL gint32 get_system_storage_drive_serialnumber(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return redfish_system_storage_drive_string(PROPERTY_HDD_SERIAL_NUMBER, i_paras->obj_handle, o_result_jso);
}


LOCAL gint32 get_system_storage_drive_capablespeedgbs(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gdouble result = 0.0;
    guint8 capablespeedgbs = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    gint32 ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_HDD_INTERFACE_SPEED, &capablespeedgbs);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get capablespeedgbs value fail.", __FUNCTION__, __LINE__));

    
    ret = redfish_system_storage_drive_speedgbs_result(capablespeedgbs, &result);
    return_val_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR);

    
    *o_result_jso = ex_json_object_new_double(result, "%.2f");
    return HTTP_OK;
}


LOCAL gint32 get_system_storage_drive_negotiatedspeedgbs(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gdouble result = 0.0;
    guint8 negotiatedspeedgbs = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    gint32 ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_HDD_LINK_SPEED, &negotiatedspeedgbs);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get negotiatedspeedgbs value fail.", __FUNCTION__, __LINE__));

    
    ret = redfish_system_storage_drive_speedgbs_result(negotiatedspeedgbs, &result);
    return_val_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = ex_json_object_new_double(result, "%.2f");
    return HTTP_OK;
}


LOCAL gint32 get_system_storage_drive_lifeleftpercent(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 lifeleftpercent = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    
    gint32 ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_HDD_REMNANT_MEDIA_WEAROUT, &lifeleftpercent);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get lifeleftpercent value fail.", __FUNCTION__, __LINE__));

    
    if (0xFF == lifeleftpercent) {
        debug_log(DLOG_MASS, "%s, %d: get lifeleftpercent value is invalid.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    

    *o_result_jso = json_object_new_int((int32_t)lifeleftpercent);
    return HTTP_OK;
}



LOCAL gint32 get_system_storage_drive_indicatorled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 fault = 0;
    guint8 indicatorled = 0;
    gchar str_buf[MAX_STATUS_LEN + 1] = {0};

    
    if ((NULL == o_result_jso) || (NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras))) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_HDD_LOCATION, &indicatorled);
    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_HDD_FAULT, &fault);

    
    
    if ((DRIVE_INDICATORLED_OFF == indicatorled) ||
        ((DRIVE_INDICATORLED_BLINKING == indicatorled) && (DRIVE_FAULT_YES == fault))) {
        // sizeof(str_buf)大小为32+1，大于RF_INDICATORLED_OFF的大小，无需判断返回值
        (void)snprintf_s(str_buf, sizeof(str_buf), sizeof(str_buf) - 1, "%s", RF_INDICATORLED_OFF);
    } else if ((DRIVE_INDICATORLED_BLINKING == indicatorled) && (DRIVE_FAULT_OK == fault)) {
        // sizeof(str_buf)大小为32+1，大于RF_INDICATORLED_BLINKING的大小，无需判断返回值
        (void)snprintf_s(str_buf, sizeof(str_buf), sizeof(str_buf) - 1, "%s", RF_INDICATORLED_BLINKING);
    } else {
        debug_log(DLOG_MASS, "%s, %d: indicatorled value error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_string(str_buf);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}


LOCAL gint32 get_system_storage_drive_hotsparetype(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 hotsparetype = 0;
    gchar str_buf[MAX_STRBUF_LEN + 1] = {0};

    
    if ((NULL == o_result_jso) || (NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras))) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    gint32 ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_HDD_HOT_SPARE, &hotsparetype);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get hotsparetype value fail.", __FUNCTION__, __LINE__));

    switch (hotsparetype) {
        case PD_HOT_SPARE_NONE:
            // sizeof(str_buf)大小为128+1，大于RF_HOTSPARETYPE_NONE的大小，无需判断返回值
            (void)snprintf_s(str_buf, sizeof(str_buf), sizeof(str_buf) - 1, "%s", RF_HOTSPARETYPE_NONE);
            break;

        case PD_HOT_SPARE_GLOBAL:
            (void)snprintf_s(str_buf, sizeof(str_buf), sizeof(str_buf) - 1, "%s", RF_HOTSPARETYPE_GLOBAL);
            break;

        case PD_HOT_SPARE_DEDICATED:
            (void)snprintf_s(str_buf, sizeof(str_buf), sizeof(str_buf) - 1, "%s", RF_HOTSPARETYPE_DEDICATED);
            break;

        case PD_HOT_SPARE_AUTO_REPLACE:
            (void)snprintf_s(str_buf, sizeof(str_buf), sizeof(str_buf) - 1, "%s", RF_HOTSPARETYPE_AUTO_REPLACE);
            break;

        default:
            debug_log(DLOG_MASS, "%s, %d: hotsparetype value is error.", __FUNCTION__, __LINE__);
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(str_buf);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}


LOCAL gint32 get_system_storage_drive_location(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *jso_location = NULL;
    json_object *jso_info = NULL;
    json_object *jso_infoformat = NULL;
    gchar devicename_buf[MAX_STRBUF_LEN + 1] = {0};

    
    if ((NULL == o_result_jso) || (NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras))) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    (void)dal_get_property_value_string(i_paras->obj_handle, PROPERTY_HDD_DEVICENAME, devicename_buf,
        sizeof(devicename_buf));
    (void)dal_clear_string_blank(devicename_buf, sizeof(devicename_buf));
    
    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: new array fail.", __FUNCTION__, __LINE__));

    jso_location = json_object_new_object(); 
    return_val_do_info_if_fail(NULL != jso_location, HTTP_OK,
        debug_log(DLOG_ERROR, "%s, %d: new array fail.", __FUNCTION__, __LINE__));

    
    jso_info = json_object_new_string(devicename_buf);
    return_val_do_info_if_fail(NULL != jso_info, HTTP_OK, json_object_put(jso_location));
    (void)json_object_object_add(jso_location, RF_LOCATION_INFO, jso_info);

    
    jso_infoformat = json_object_new_string(RFPROP_DRIVE_DEVICENAME);
    return_val_do_info_if_fail(NULL != jso_infoformat, HTTP_OK, json_object_put(jso_location));
    (void)json_object_object_add(jso_location, RF_LOCATION_INFOFORMAT, jso_infoformat);

    
    gint32 ret = json_object_array_add(*o_result_jso, jso_location);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, json_object_put(jso_location));

    return HTTP_OK;
}


LOCAL gint32 get_drive_rotation_speed(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso)
{
    guint16 rotation_speed = G_MAXUINT16;
    const gchar *pme_prop_name = NULL;

    if (i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    pme_prop_name = g_chassis_drive_provider[i_paras->index].pme_prop_name;
    gint32 ret_val = dal_get_property_value_uint16(i_paras->obj_handle, pme_prop_name, &rotation_speed);
    if (ret_val != RET_OK || rotation_speed == G_MAXUINT16) {
        debug_log(DLOG_DEBUG, "%s: get %s.%s failed, ret: %d", __FUNCTION__, dfl_get_object_name(i_paras->obj_handle),
            pme_prop_name, ret_val);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_int(rotation_speed);

    return HTTP_OK;
}


LOCAL gint32 get_system_storage_drive_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 hdd_id = 0;
    guint16 logical_id = 0;
    guint8 hdd_sparetype = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *volumes_jso_array = NULL;
    gchar hdd_controller_name[MAX_STRBUF_LEN + 1] = {0};
    gchar logical_controller_name[MAX_STRBUF_LEN + 1] = {0};
    guint8 ld_raid_level = 0;

    
    if ((o_result_jso == NULL) || (o_message_jso == NULL) || (provider_paras_check(i_paras)) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_object();
    return_val_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);
    volumes_jso_array = json_object_new_array();
    return_val_do_info_if_fail(volumes_jso_array != NULL, HTTP_INTERNAL_SERVER_ERROR, json_object_put(*o_result_jso);
        *o_result_jso = NULL);

    
    ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_HDD_REF_RAID_CONTROLLER, hdd_controller_name,
        sizeof(hdd_controller_name));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_OK,
        json_object_object_add(*o_result_jso, RF_LINKS_VOLUMES, volumes_jso_array);
        debug_log(DLOG_MASS, "%s: get hdd controller name fail.", __FUNCTION__));

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_HDD_ID, &hdd_id);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_OK,
        json_object_object_add(*o_result_jso, RF_LINKS_VOLUMES, volumes_jso_array);
        debug_log(DLOG_MASS, "%s: get hdd_id fail.", __FUNCTION__));

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_HDD_HOT_SPARE, &hdd_sparetype);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_OK,
        json_object_object_add(*o_result_jso, RF_LINKS_VOLUMES, volumes_jso_array);
        debug_log(DLOG_MASS, "%s: get hdd spare type fail.", __FUNCTION__));

    
    ret = dfl_get_object_list(CLASS_LOGICAL_DRIVE_NAME, &obj_list);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_OK,
        json_object_object_add(*o_result_jso, RF_LINKS_VOLUMES, volumes_jso_array);
        debug_log(DLOG_MASS, "%s: get obj_list fail.", __FUNCTION__));

    
    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_RAID_LEVEL,
            &ld_raid_level);
        continue_if_expr(RAID_LEVEL_DELETED == ld_raid_level);
        
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER,
            logical_controller_name, sizeof(logical_controller_name));
        (void)dal_get_property_value_uint16((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_TARGET_ID, &logical_id);

        
        if (g_strcmp0(hdd_controller_name, logical_controller_name) == 0) {
            
            
            // 局部热备链接局部热备Links
            if (hdd_sparetype == PD_HOT_SPARE_DEDICATED || hdd_sparetype == PD_HOT_SPARE_AUTO_REPLACE) {
                (void)redfish_storage_drive_logical((OBJ_HANDLE)obj_node->data,
                    PROPERTY_LOGICAL_DRIVE_DEDICATED_SPARED_PD_SLOT,
                    PROPERTY_LOGICAL_DRIVE_DEDICATED_SPARED_PD_ENCLOSURE, volumes_jso_array, hdd_controller_name,
                    logical_id, hdd_id);
            } else {
                (void)redfish_storage_drive_logical((OBJ_HANDLE)obj_node->data,
                    PROPERTY_LOGICAL_DRIVE_PARTICIPATED_PD_SLOT, PROPERTY_LOGICAL_DRIVE_PARTICIPATED_PD_ENCLOSURE,
                    volumes_jso_array, hdd_controller_name, logical_id, hdd_id);
            }
            
            
        }
    }

    g_slist_free(obj_list);
    json_object_object_add(*o_result_jso, RF_LINKS_VOLUMES, volumes_jso_array);
    return HTTP_OK;
}


gint32 get_drive_oem_driveid(OBJ_HANDLE obj_handle, json_object *o_huawei_jso)
{
    guint8 driveid = 0;
    json_object *json_driveid = NULL;

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_HDD_ID, &driveid);
    return_val_if_expr(VOS_OK != ret, VOS_ERR);
    return_val_if_expr(DRIVE_INVALID_VALUE == driveid, VOS_ERR);

    json_driveid = json_object_new_int((int32_t)driveid);
    return_val_if_expr(NULL == json_driveid, VOS_ERR);

    (void)json_object_object_add(o_huawei_jso, RFPROP_DRIVE_DRIVEID, json_driveid);
    return VOS_OK;
}


LOCAL gint32 get_drive_oem_rebuildstate(OBJ_HANDLE obj_handle, json_object *o_huawei_jso)
{
    guint8 hdd_rebuildstate = 0;
    json_object *new_rebuildstate = NULL;

    
    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_HDD_REBUILD_STATE, &hdd_rebuildstate);
    return_val_if_expr(VOS_OK != ret, VOS_ERR);

    
    switch (hdd_rebuildstate) {
        case 0:
            new_rebuildstate = json_object_new_string(RF_REBUILDSTATE_REBUILDORNOREBUILD);
            break;

        case 1:
            new_rebuildstate = json_object_new_string(RF_REBUILDSTATE_REBUILDING);
            break;

        default:
            
            debug_log(DLOG_DEBUG, "%s, %d: the value of rebuildstate is error.", __FUNCTION__, __LINE__);
            
    }

    do_if_expr(new_rebuildstate == NULL, return VOS_ERR);
    json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_REBUILDSTATE, new_rebuildstate);
    return VOS_OK;
}


LOCAL gint32 get_drive_oem_rebuildprogress(OBJ_HANDLE obj_handle, json_object *o_huawei_jso)
{
    errno_t securec_rv;
    guint8 rebuildprogress = 0;
    json_object *new_rebuildprogress = NULL;
    gchar str_buf[MAX_STRBUF_LEN + 1] = {0};

    
    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_HDD_REBUILD_STATE, &rebuildprogress);
    return_val_if_fail(VOS_OK == ret, VOS_ERR);
    return_val_if_fail(DRIVE_REBUILTING_VALUE == rebuildprogress, VOS_ERR);

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_HDD_REBUILDPROGRESS, &rebuildprogress);
    return_val_if_fail(VOS_OK == ret, VOS_ERR);

    
    if (DRIVE_INVALID_VALUE == rebuildprogress) {
        if (dal_test_pd_under_which_vendor(obj_handle, VENDER_PMC)) {
            securec_rv = strcpy_s(str_buf, sizeof(str_buf), STRING_VALUE_IS_NA);
        } else {
            securec_rv = strcpy_s(str_buf, sizeof(str_buf), RF_EBUILDPROGRESS_UNKNOWNREBUILDPROGRESS);
        }

        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, securec_rv);
        }
    } else {
        // sizeof(str_buf)大小为128+1，远大于格式化后的字符串长度，无需判断返回值
        (void)snprintf_s(str_buf, sizeof(str_buf), sizeof(str_buf) - 1, "%u%%", rebuildprogress);
    }

    new_rebuildprogress = json_object_new_string(str_buf);
    return_val_if_fail(NULL != new_rebuildprogress, VOS_ERR);

    json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_EBUILDPROGRESS, new_rebuildprogress);
    return VOS_OK;
}

LOCAL gint32 get_drive_oem_patrolstate(OBJ_HANDLE obj_handle, json_object *o_huawei_jso)
{
    guint8 hdd_patrolstate = 0;
    json_object *jso_pratolstate = NULL;

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_HDD_PATROL_STATE, &hdd_patrolstate);
    return_val_if_fail(VOS_OK == ret, ret);

    return_val_if_fail(DRIVE_INVALID_VALUE != hdd_patrolstate, VOS_ERR);

    // 0 没有巡检或巡检结束 1 正在巡检
    switch (hdd_patrolstate) {
        case 0:
            jso_pratolstate = json_object_new_string(RF_PATROLSTATE_PATROLEDORNOPATROLED);
            break;

        case 1:
            jso_pratolstate = json_object_new_string(RF_PATROLSTATE_PATROLLING);
            break;

        default:
            debug_log(DLOG_MASS, "%s, %d, The value of hdd patrolstate is error.", __FUNCTION__, __LINE__);
    }

    do_if_expr(jso_pratolstate == NULL, return VOS_ERR);
    json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_PATROLSTATE, jso_pratolstate);
    return VOS_OK;
}


LOCAL gint32 get_drive_oem_temperature(OBJ_HANDLE obj_handle, json_object *o_huawei_jso)
{
    guint8 hdd_temperature = 0;
    json_object *jso_temperature = NULL;

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_HDD_TEMPERATURE, &hdd_temperature);
    return_val_if_fail(VOS_OK == ret, ret);

    return_val_if_fail(DRIVE_INVALID_VALUE != hdd_temperature, VOS_ERR);

    jso_temperature = json_object_new_int((int32_t)(gint8)hdd_temperature);
    return_val_if_fail(NULL != jso_temperature, VOS_ERR);

    (void)json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_TEMPERATURE, jso_temperature);
    return VOS_OK;
}


LOCAL gint32 redfish_drive_physicaldiskstate_result(guint8 hdd_state, const gchar **state_result)
{
    guint32 i;

    INT2STR_MAP_S    state_result_map[] = {
        {PD_STATE_UNCONFIGURED_GOOD,          RF_PHYSICALDISKSTATE_UNCONFIGUREDGOOD},
        {PD_STATE_UNCONFIGURED_BAD,           RF_PHYSICALDISKSTATE_UNCONFIGUREDBAD},
        {PD_STATE_HOT_SPARE,                  RF_PHYSICALDISKSTATE_HOTSPARE},
        {PD_STATE_OFFLINE,                    RF_PHYSICALDISKSTATE_CONFIGUREDGOOD},
        {PD_STATE_FAILED,                     RF_PHYSICALDISKSTATE_CONFIGUREDBAD},
        {PD_STATE_REBUILD,                    RF_PHYSICALDISKSTATE_REBUILDING},
        {PD_STATE_ONLINE,                     RF_PHYSICALDISKSTATE_ONLINE},
        {PD_STATE_COPYBACK,                   RF_PHYSICALDISKSTATE_GETTINGCOPIED},
        {PD_STATE_SYSTEM,                     RF_PHYSICALDISKSTATE_CONTROLLEDBYHOST},
        {PD_STATE_SHIELD_UNCONFIGURED,        RF_PHYSICALDISKSTATE_UNCONFIGUREDSHIELDED},
        {PD_STATE_SHIELD_HOT_SPARE,           RF_PHYSICALDISKSTATE_HOTSPARESHIELDED},
        {PD_STATE_SHIELD_CONFIGURED,          RF_PHYSICALDISKSTATE_CONFIGUREDSHIELDED},
        {PD_STATE_FOREIGN,                    RF_PHYSICALDISKSTATE_FOREIGN},
        
        {PD_STATE_ACTIVE,                     RF_PHYSICALDISKSTATE_ACTIVE},
        {PD_STATE_STANDBY,                    RF_PHYSICALDISKSTATE_STANDBY},
        {PD_STATE_SLEEP,                      RF_PHYSICALDISKSTATE_SLEEP},
        {PD_STATE_DST,                        RF_PHYSICALDISKSTATE_DST},
        {PD_STATE_SMART,                      RF_PHYSICALDISKSTATE_SMART},
        {PD_STATE_SCT,                        RF_PHYSICALDISKSTATE_SCT},
        {PD_STATE_EPD,                        RF_PHYSICALDISKSTATE_ONLINE},
        {PD_STATE_RAW,                        RF_PHYSICALDISKSTATE_RAW},
        {PD_STATE_READY,                      RF_PHYSICALDISKSTATE_READY},
        {PD_STATE_NOT_SUPPORTED,              RF_PHYSICALDISKSTATE_NOTSUPPORTED},
        {PD_STATE_PREDICTIVE_FAILURE,         RF_PHYSICALDISKSTATE_PREDICTIVE_FAILURE},
        {PD_STATE_DIAGNOSING,                 RF_PHYSICALDISKSTATE_DIAGNOSING},
        {PD_STATE_INCOMPATIBLE,               RF_PHYSICALDISKSTATE_INCOMPATIBLE},
        {PD_STATE_ERASING,                    RF_PHYSICALDISKSTATE_ERASING},
        
    };

    *state_result = NULL;
    guint32 size = sizeof(state_result_map) / sizeof(state_result_map[0]);

    for (i = 0; i < size; i++) {
        if (hdd_state == state_result_map[i].int_val) {
            *state_result = state_result_map[i].str_val;
            debug_log(DLOG_MASS, "%s, %d, get state value ok.", __FUNCTION__, __LINE__);
            return VOS_OK;
        }
    }

    return VOS_ERR;
}


gint32 get_drive_oem_physicaldiskstate(OBJ_HANDLE obj_handle, json_object *o_huawei_jso)
{
    guint8 hdd_state = 0;
    json_object *jso_state = NULL;
    const gchar *state_result = NULL;

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_HDD_FIRMWARE_STATUS, &hdd_state);
    return_val_if_fail(VOS_OK == ret, VOS_ERR);

    ret = redfish_drive_physicaldiskstate_result(hdd_state, &state_result);
    return_val_if_fail(VOS_OK == ret, VOS_ERR);

    jso_state = json_object_new_string(state_result);
    return_val_if_fail(NULL != jso_state, VOS_ERR);

    json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_PHYSICALDISKSTATE, jso_state);

    json_object* is_epd = json_object_new_boolean(hdd_state == PD_STATE_EPD);
    if (is_epd == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_boolean failed.", __FUNCTION__);
        return RET_ERR;
    }
    json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_IS_EPD, is_epd);
    return VOS_OK;
}


LOCAL gint32 get_drive_oem_hotspareoforlogical(OBJ_HANDLE obj_handle, json_object *o_hauwei_jso)
{
    guint8 hdd_id = 0;
    guint16 logical_id = 0;
    guint8 logical_controller_id = 0;
    guint8 hdd_sparestype = 0;
    guint8 controller_id = 0;
    GSList *obj_list = NULL;
    json_object *odata_id = NULL;
    json_object *json_buf = NULL;
    const guint8 *hdd_array_pointer = NULL;
    gsize len_hdd_array_pointer = 0;
    gchar str_buf[MAX_STRBUF_LEN + 1] = {0};
    OBJ_HANDLE controller_obj_handle = 0;
    gchar slot[MAX_RSC_ID_LEN + 1] = {0};
    gchar logical_of_controller[MAX_STRBUF_LEN + 1] = {0};
    GVariant *hdd_enclosuer_array = NULL;
    const guint16 *hdd_encloser_array_pointer = NULL;
    gsize num_index;
    gchar raid_object_name[MAX_NAME_SIZE] = {0};
    guint8 ld_raid_level = 0;

    gint32 ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: get slot id error.\n", __FUNCTION__, __LINE__));

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_HOT_SPARE, &hdd_sparestype);
    if (hdd_sparestype != PD_HOT_SPARE_DEDICATED && hdd_sparestype != PD_HOT_SPARE_AUTO_REPLACE) {
        debug_log(DLOG_MASS, "%s, The value of SpareType is not Deicated or Auto Replace.", __FUNCTION__);
        return RET_ERR;
    }

    
    ret = get_system_storage_driveid_and_controllerid(obj_handle, &controller_id, &hdd_id);
    return_val_if_fail(ret == VOS_OK, VOS_ERR);

    
    ret = dfl_get_object_list(CLASS_LOGICAL_DRIVE_NAME, &obj_list);
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    for (GSList *obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_RAID_LEVEL,
            &ld_raid_level);
        continue_if_expr(ld_raid_level == RAID_LEVEL_DELETED);
        
        guint8 *hdd_id_array = NULL;
        
        GVariant *hdd_slot_array = NULL;
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_REF_RAID_CONTROLLER,
            logical_of_controller, sizeof(logical_of_controller));
        
        (void)dfl_get_object_handle(logical_of_controller, &controller_obj_handle);
        
        (void)dal_get_property_value_byte(controller_obj_handle, PROPERTY_RAID_CONTROLLER_ID, &logical_controller_id);

        
        (void)dal_get_property_value_uint16((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_TARGET_ID, &logical_id);

        check_fail_continue(controller_id == logical_controller_id);
        ret = dfl_get_property_value((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_DEDICATED_SPARED_PD_SLOT,
            &hdd_slot_array);
        check_fail_continue(ret == VOS_OK);
        check_fail_continue(hdd_slot_array != NULL);

        hdd_array_pointer =
            (const guint8 *)g_variant_get_fixed_array(hdd_slot_array, &len_hdd_array_pointer, sizeof(guint8));

        continue_do_info_if_fail(hdd_array_pointer != NULL, g_variant_unref(hdd_slot_array); hdd_slot_array = NULL);

        
        hdd_enclosuer_array = NULL;
        ret = dfl_get_property_value((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_DEDICATED_SPARED_PD_ENCLOSURE,
            &hdd_enclosuer_array);
        continue_do_info_if_fail(ret == VOS_OK, g_variant_unref(hdd_slot_array); hdd_slot_array = NULL);
        continue_do_info_if_fail(hdd_enclosuer_array != NULL, g_variant_unref(hdd_slot_array); hdd_slot_array = NULL);
        hdd_encloser_array_pointer = NULL;
        hdd_encloser_array_pointer =
            (const guint16 *)g_variant_get_fixed_array(hdd_enclosuer_array, &len_hdd_array_pointer, sizeof(guint16));
        continue_do_info_if_fail(hdd_encloser_array_pointer != NULL, g_variant_unref(hdd_slot_array);
            hdd_slot_array = NULL; g_variant_unref(hdd_enclosuer_array); hdd_enclosuer_array = NULL;
            debug_log(DLOG_MASS, "%s, %d: g_variant_get_fixed_array.", __FUNCTION__, __LINE__));

        hdd_id_array = (guint8 *)g_malloc0(len_hdd_array_pointer);
        break_do_info_if_fail(hdd_id_array != NULL, g_variant_unref(hdd_slot_array); hdd_slot_array = NULL;
            g_variant_unref(hdd_enclosuer_array); hdd_enclosuer_array = NULL;
            debug_log(DLOG_MASS, "%s %d: g_malloc0 fail.", __FUNCTION__, __LINE__));

        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER,
            raid_object_name, sizeof(raid_object_name));

        for (num_index = 0; num_index < len_hdd_array_pointer; num_index++) {
            dal_get_pd_id_by_slot_and_enclosure(raid_object_name, hdd_array_pointer[num_index],
                hdd_encloser_array_pointer[num_index], hdd_id_array + num_index);
        }

        for (num_index = 0; num_index < len_hdd_array_pointer; num_index++) {
            if (hdd_id == hdd_id_array[num_index]) {
                
                (void)memset_s(str_buf, sizeof(str_buf), 0, sizeof(str_buf));
                
                ret = snprintf_s(str_buf, sizeof(str_buf), sizeof(str_buf) - 1, RFPROP_SYSTEM_REVOLUME, slot,
                    controller_id, logical_id);
                do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret));
                
                odata_id = json_object_new_object();
                continue_if_expr_true(odata_id == NULL);

                json_buf = json_object_new_string(str_buf);
                continue_do_info_if_fail(json_buf != NULL, json_object_put(odata_id));

                json_object_object_add(odata_id, RFPROP_ODATA_ID, json_buf);

                ret = json_object_array_add(o_hauwei_jso, odata_id);
                do_val_if_fail(ret == VOS_OK, json_object_put(odata_id));
            }
        }

        g_free(hdd_id_array);
        hdd_id_array = NULL;
        g_variant_unref(hdd_slot_array);
        hdd_slot_array = NULL;
        g_variant_unref(hdd_enclosuer_array);
        hdd_enclosuer_array = NULL;
        
    }

    g_slist_free(obj_list);
    return VOS_OK;
}


LOCAL gint32 get_drive_oem_sasaddress_property(OBJ_HANDLE obj_handle, gchar *i_property_name,
    json_object *o_sasaddress_jso)
{
    json_object *jso_sasaddress = NULL;
    gchar str_sasaddress[MAX_STRBUF_LEN + 1] = {0};

    gint32 ret = dal_get_property_value_string(obj_handle, i_property_name, str_sasaddress, sizeof(str_sasaddress));
    return_val_if_fail(VOS_OK == ret, VOS_ERR);
    return_val_if_fail(0 != g_strcmp0(INVALID_DATA_STRING, str_sasaddress), VOS_ERR);

    jso_sasaddress = json_object_new_string(str_sasaddress);
    if (NULL == jso_sasaddress) {
        (void)json_object_array_add(o_sasaddress_jso, NULL);
    } else {
        (void)json_object_array_add(o_sasaddress_jso, jso_sasaddress);
        debug_log(DLOG_MASS, "%s, %d: new string ok.", __FUNCTION__, __LINE__);
    }

    return VOS_OK;
}


LOCAL void get_drive_oem_sasaddress(OBJ_HANDLE obj_handle, json_object *o_sasaddress_jso)
{
    gint32 ret = get_drive_oem_sasaddress_property(obj_handle, PROPERTY_HDD_SAS_ADDR1, o_sasaddress_jso);
    do_val_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s, %d: get sasaddr1 property fail.", __FUNCTION__, __LINE__));

    ret = get_drive_oem_sasaddress_property(obj_handle, PROPERTY_HDD_SAS_ADDR2, o_sasaddress_jso);
    do_val_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s, %d: get sasaddr2 property fail.", __FUNCTION__, __LINE__));
    return;
}


LOCAL gboolean redfish_check_everyonet_drive_is_failedarray(const guint8 *hdd_array_pointer,
    gsize len_hdd_array_pointer)
{
    gsize i;
    guint8 failedarray = 0;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    
    if (hdd_array_pointer == NULL) {
        debug_log(DLOG_MASS, "%s, %d: input parameter invalid.", __FUNCTION__, __LINE__);
        return FALSE;
    }

    
    for (i = 0; i < len_hdd_array_pointer; i++) {
        ret = dal_get_specific_object_byte(CLASS_HDD_NAME, PROPERTY_HDD_ID, hdd_array_pointer[i], &obj_handle);
        do_if_fail(ret == VOS_OK, debug_log(DLOG_MASS, "%s, %d: get specific handle fail.", __FUNCTION__, __LINE__);
            continue);

        (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_FAILEDARRAY, &failedarray);

        if (failedarray == 1) {
            return TRUE;
        }
    }

    return FALSE;
}


LOCAL gboolean redfish_check_physicaldrive_in_logicaldrive(const guint8 *hdd_array_pointer, gsize len_hdd_array_pointer,
    guint8 hdd_id)
{
    gsize i;

    
    if (hdd_array_pointer == NULL) {
        debug_log(DLOG_MASS, "%s, %d: input parameter invalid.", __FUNCTION__, __LINE__);
        return FALSE;
    }

    for (i = 0; i < len_hdd_array_pointer; i++) {
        if (hdd_id == hdd_array_pointer[i]) {
            return TRUE;
        }
    }

    return FALSE;
}

LOCAL gboolean redfish_check_pd_inafailedarray(OBJ_HANDLE obj_handle)
{
    guint8 hdd_id = 0;
    gboolean result = FALSE;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    GVariant *hdd_slot_array = NULL;
    const guint8 *hdd_array_pointer = NULL;
    gsize len_hdd_array_pointer = 0;
    GVariant *hdd_encloser_array = NULL;
    const guint16 *hdd_encloser_array_pointer = NULL;
    guint8 *hdd_id_array = NULL;
    gsize num_index = 0;
    gchar raid_object_name[MAX_NAME_SIZE] = {0};

    if (obj_handle == 0) {
        debug_log(DLOG_MASS, "%s, %d: input param invalid.", __FUNCTION__, __LINE__);
        return result;
    }

    
    gint32 ret = dfl_get_object_list(CLASS_LOGICAL_DRIVE_NAME, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, result,
        debug_log(DLOG_MASS, "%s %d: get logical obj list fail.", __FUNCTION__, __LINE__));
    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_ID, &hdd_id);

    
    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        hdd_id_array = NULL;
        
        hdd_slot_array = NULL;
        ret = dfl_get_property_value((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_PARTICIPATED_PD_SLOT,
            &hdd_slot_array);
        do_if_fail(VOS_OK == ret, continue);
        do_if_fail(NULL != hdd_slot_array, continue);
        hdd_array_pointer = NULL;
        hdd_array_pointer =
            (const guint8 *)g_variant_get_fixed_array(hdd_slot_array, &len_hdd_array_pointer, sizeof(guint8));
        do_if_fail(NULL != hdd_array_pointer, g_variant_unref(hdd_slot_array);
            debug_log(DLOG_MASS, "%s, %d: g_variant_get_fixed_array.", __FUNCTION__, __LINE__); continue);

        hdd_encloser_array = NULL;
        ret = dfl_get_property_value((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_PARTICIPATED_PD_ENCLOSURE,
            &hdd_encloser_array);
        continue_do_info_if_fail(VOS_OK == ret, g_variant_unref(hdd_slot_array));
        continue_do_info_if_fail(NULL != hdd_encloser_array, g_variant_unref(hdd_slot_array));
        hdd_encloser_array_pointer = NULL;
        hdd_encloser_array_pointer =
            (const guint16 *)g_variant_get_fixed_array(hdd_encloser_array, &len_hdd_array_pointer, sizeof(guint16));
        continue_do_info_if_fail(NULL != hdd_encloser_array_pointer, g_variant_unref(hdd_slot_array);
            g_variant_unref(hdd_encloser_array);
            debug_log(DLOG_MASS, "%s, %d: g_variant_get_fixed_array.", __FUNCTION__, __LINE__););

        hdd_id_array = (guint8 *)g_malloc0(len_hdd_array_pointer);
        break_do_info_if_expr(NULL == hdd_id_array, g_variant_unref(hdd_slot_array); hdd_slot_array = NULL;
            g_variant_unref(hdd_encloser_array);
            debug_log(DLOG_MASS, "%s %d: g_malloc0 fail.", __FUNCTION__, __LINE__));

        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER,
            raid_object_name, sizeof(raid_object_name));

        for (num_index = 0; num_index < len_hdd_array_pointer; num_index++) {
            dal_get_pd_id_by_slot_and_enclosure(raid_object_name, hdd_array_pointer[num_index],
                hdd_encloser_array_pointer[num_index], hdd_id_array + num_index);
        }

        
        do_if_fail(redfish_check_physicaldrive_in_logicaldrive(hdd_id_array, len_hdd_array_pointer, hdd_id),
            g_variant_unref(hdd_slot_array);
            g_variant_unref(hdd_encloser_array); g_free(hdd_id_array); continue);

        
        if (redfish_check_everyonet_drive_is_failedarray(hdd_id_array, len_hdd_array_pointer)) {
            result = TRUE;
            do_val_if_expr(NULL != hdd_slot_array, g_variant_unref(hdd_slot_array));
            do_val_if_expr(NULL != hdd_encloser_array, g_variant_unref(hdd_encloser_array));
            g_free(hdd_id_array);
            break;
        }

        g_free(hdd_id_array);
        do_val_if_expr(NULL != hdd_slot_array, g_variant_unref(hdd_slot_array));
        do_val_if_expr(NULL != hdd_encloser_array, g_variant_unref(hdd_encloser_array));
    }

    
    do_if_expr(NULL != obj_list, g_slist_free(obj_list));

    return result;
}


LOCAL json_object* get_indicator_by_pd_state(guint8 firmware_state)
{
    switch (firmware_state) {
        case PD_STATE_UNCONFIGURED_GOOD:
        case PD_STATE_UNCONFIGURED_BAD:
        case PD_STATE_FAILED:
        case PD_STATE_PREDICTIVE_FAILURE:
        case PD_STATE_ONLINE:
        case PD_STATE_EPD:
        case PD_STATE_COPYBACK:
        case PD_STATE_SYSTEM:
        case PD_STATE_ACTIVE:
        case PD_STATE_STANDBY:
        case PD_STATE_SLEEP:
        case PD_STATE_DST:
        case PD_STATE_SMART:
        case PD_STATE_SCT:
        case PD_STATE_RAW:
        case PD_STATE_READY:
        case PD_STATE_DIAGNOSING:
        case PD_STATE_ERASING:
            return json_object_new_string(RFPROP_DRIVE_OK);

        case PD_STATE_SHIELD_UNCONFIGURED:
        case PD_STATE_SHIELD_HOT_SPARE:
        case PD_STATE_SHIELD_CONFIGURED:
        case PD_STATE_FOREIGN:
        case PD_STATE_NOT_SUPPORTED:
        case PD_STATE_INCOMPATIBLE:
            return json_object_new_string(RFPROP_DRIVE_FAIL);

        case PD_STATE_HOT_SPARE:
            return json_object_new_string(RFPROP_DRIVE_HOTSPARE);

        case PD_STATE_REBUILD:
            return json_object_new_string(RFPROP_DRIVE_REBUILD);

        default:
            return NULL;
    }
}


LOCAL gint32 get_system_storage_drive_statusindicator(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 firmware_state = 0;
    guint8 pre_fail = 0;

    
    if ((o_result_jso == NULL) || (o_message_jso == NULL) || (provider_paras_check(i_paras) != VOS_OK)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_HDD_FIRMWARE_STATUS, &firmware_state);

    
    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_HDD_PREDICTIVEFAILURE, &pre_fail);

    
    if (pre_fail == 1) {
        *o_result_jso = json_object_new_string(RFPROP_DRIVE_PRE_FAIL_ANALYSIS);
    } else if (redfish_check_pd_inafailedarray(i_paras->obj_handle)) {
        
        *o_result_jso = json_object_new_string(RFPROP_DRIVE_INAFAILEDARRAY);
    } else {
        
        *o_result_jso = get_indicator_by_pd_state(firmware_state);
    }

    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}


LOCAL void get_drive_oem_property_type_is_byte(OBJ_HANDLE obj_handle, const gchar *prop_name, json_object **o_prop_jso)
{
    guint8 prop_value = 0;

    gint32 ret = dal_get_property_value_byte(obj_handle, prop_name, &prop_value);
    return_do_info_if_fail(VOS_OK == ret, *o_prop_jso = NULL;
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.", __FUNCTION__, __LINE__));

    return_do_info_if_fail(DRIVE_INVALID_VALUE != prop_value, *o_prop_jso = NULL;
        debug_log(DLOG_MASS, "%s, %d: property value is invalid.", __FUNCTION__, __LINE__));

    *o_prop_jso = json_object_new_int((int32_t)prop_value);
    do_val_if_expr(NULL == *o_prop_jso,
        debug_log(DLOG_ERROR, "%s, %d: json object new int fail.", __FUNCTION__, __LINE__));
    return;
}


LOCAL void get_drive_oem_property_type_is_string(OBJ_HANDLE obj_handle, const gchar *pem_property_name,
    json_object **o_prop_jso)
{
    gchar str_buf[MAX_STRBUF_LEN + 1] = {0};

    gint32 ret = dal_get_property_value_string(obj_handle, pem_property_name, str_buf, sizeof(str_buf));
    return_do_info_if_fail(VOS_OK == ret, *o_prop_jso = NULL;
        debug_log(DLOG_ERROR, "%s, %d: get %s property value fail.", __FUNCTION__, __LINE__, pem_property_name));
    return_do_info_if_expr((g_strcmp0("Undefined", str_buf) == 0) || (g_strcmp0("Unknown", str_buf) == 0) ||
        (g_strcmp0("N/A", str_buf) == 0) || (strlen(str_buf) == 0),
        *o_prop_jso = NULL;
        debug_log(DLOG_MASS, "%s, %d: property value invalid.", __FUNCTION__, __LINE__));
    dal_clear_string_blank(str_buf, sizeof(str_buf));
    *o_prop_jso = json_object_new_string(str_buf);
    do_val_if_expr(*o_prop_jso == NULL,
        debug_log(DLOG_ERROR, "%s, %d: json object new string fail.", __FUNCTION__, __LINE__));
    return;
}


LOCAL void get_drive_oem_property_type_is_uint32(OBJ_HANDLE obj_handle, const gchar *prop_name,
    json_object **o_prop_jso)
{
    guint32 prop_value = 0;
    gint32 ret = dal_get_property_value_uint32(obj_handle, prop_name, &prop_value);

    return_do_info_if_fail(VOS_OK == ret, *o_prop_jso = NULL;
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.", __FUNCTION__, __LINE__));

    
    return_do_info_if_fail(DRIVE_SMART_INVALID_U32_VALUE != prop_value, *o_prop_jso = NULL;
        debug_log(DLOG_MASS, "%s, %d: property value is invalid.", __FUNCTION__, __LINE__));

    return_do_info_if_fail(DRIVE_SMART_INVALID_U16_VALUE != (guint16)prop_value, *o_prop_jso = NULL;
        debug_log(DLOG_MASS, "%s, %d: property value is invalid.", __FUNCTION__, __LINE__));

    *o_prop_jso = json_object_new_int((int32_t)prop_value);
    do_val_if_expr(NULL == *o_prop_jso,
        debug_log(DLOG_ERROR, "%s, %d: json object new int fail.", __FUNCTION__, __LINE__));
    return;
}


LOCAL void get_drive_oem_sassmartinfo(OBJ_HANDLE obj_handle, json_object *o_huawei_jso)
{
    gint32 i;
    json_object *jso_obj = NULL;
    json_object *json_sas = NULL;
    
    str_map_s sas_property_array[] = {
        {PROPERTY_HDD_MEDIA_ERR_COUNT,       RFPROP_SASSMART_MEDIA_ERROR_COUNT},
        {PROPERTY_HDD_PREFAIL_ERR_COUNT,     RFPROP_SASSMART_PREFAIL_ERROR_COUNT},
        {PROPERTY_HDD_OTHER_ERR_COUNT,       RFPROP_SASSMART_OTHER_ERROR_COUNT},
        {PROPERTY_HDD_DRIVE_TEMP,            RFPROP_SASSMART_DISK_TEMPERATURE},
        {PROPERTY_HDD_TRIP_TEMP,             RFPROP_SASSMART_DISK_STRIPE_TEMPERATURE},
        {PROPERTY_HDD_ELE_GROWN_LIST,        RFPROP_SASSMART_ELEMENTS_IN_GROWN_DEFECT_LIST},
        {PROPERTY_HDD_ELE_DEFECT_LIST,       RFPROP_SASSMART_ELEMENTS_IN_PRIMARY_DEFECT_LIST},
        {PROPERTY_HDD_BLOCK_SEND,            RFPROP_SASSMART_BLOCKS_SEND_TO_INITIATOR},
        {PROPERTY_HDD_BLOCK_FROM,            RFPROP_SASSMART_BLOCKS_RECEIVED_FROM_INITIATOR},
        {PROPERTY_HDD_MINU_SMART_TEST,       RFPROP_SASSMART_NUM_OF_MINUTES_UNTIL_NEXT_TEST},
        {PROPERTY_HDD_OS_LAST_EVENT_NUM,     RFPROP_SASSMART_SEQ_NUM_OF_LAST_PREDFAIL_EVENT}
    };
    gint32 length = G_N_ELEMENTS(sas_property_array);
    json_sas = json_object_new_object();
    return_do_info_if_fail(NULL != json_sas, json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_SASSMARTINFO, NULL);
        debug_log(DLOG_ERROR, "%s, %d: new object fail.", __FUNCTION__, __LINE__));

    for (i = 0; i < length; i++) {
        (void)get_drive_oem_property_type_is_uint32(obj_handle, sas_property_array[i].str1, &jso_obj);
        (void)json_object_object_add(json_sas, sas_property_array[i].str2, jso_obj);
    }

    
    (void)get_drive_oem_property_type_is_string(obj_handle, PROPERTY_HDD_HEAL_STATUS, &jso_obj);
    (void)json_object_object_add(json_sas, RFPROP_SASSMART_HEALTH_STATUS, jso_obj);

    
    (void)get_drive_oem_property_type_is_string(obj_handle, PROPERTY_HDD_MNU_WEEK_YEAR, &jso_obj);
    (void)json_object_object_add(json_sas, RFPROP_SASSMART_WEEK_AND_YEAR_OF_MANUFACTURE, jso_obj);
    (void)json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_SASSMARTINFO, json_sas);
    return;
}


LOCAL gboolean redfish_drive_get_attr_json_value(json_object *i_attr_json, json_object **o_jso_obj)
{
#define SMART_SATA_ATTRIBUTE_RAW_VALUE "RAW_VALUE"
#define SMART_SATA_ATTRIBUTE_UPDATED "UPDATED"
#define SMART_SATA_ATTRIBUTE_NAME "ATTRIBUTE_NAME"
#define SMART_SATA_ATTRIBUTE_VALUE "VALUE"
#define SMART_SATA_ATTRIBUTE_FALAG "FLAG"
#define SMART_SATA_ATTRIBUTE_WORST "WORST"
#define SMART_SATA_ATTRIBUTE_WHEN_FAILED "WHEN_FAILED"
#define SMART_SATA_ATTRIBUTE_THRESHOLD "THRESHOLD"
#define SMART_SATA_ATTRIBUTE_TYPE "TYPE"
#define SMART_SATA_ATTRIBUTE_ID "ID#"

    gint32 i;
    json_object *attr_value = NULL;
    str_map_s bmc_to_attr[] = {
        {SMART_SATA_ATTRIBUTE_RAW_VALUE,    RFPROP_SATASMART_ATTR_RAWVALUE},
        {SMART_SATA_ATTRIBUTE_UPDATED,      RFPROP_SATASMART_ATTR_UPDATEMODE},
        {SMART_SATA_ATTRIBUTE_NAME,         RFPROP_SATASMART_ATTR_NAME},
        {SMART_SATA_ATTRIBUTE_VALUE,        RFPROP_SATASMART_ATTR_NORMALVALUE},
        {SMART_SATA_ATTRIBUTE_FALAG,        RFPROP_SATASMART_ATTR_FLAG},
        {SMART_SATA_ATTRIBUTE_WORST,        RFPROP_SATASMART_ATTR_WORSTVALUE},
        {SMART_SATA_ATTRIBUTE_WHEN_FAILED,  RFPROP_SATASMART_ATTR_STATUS},
        {SMART_SATA_ATTRIBUTE_THRESHOLD,    RFPROP_SATASMART_ATTR_THRESHOLD},
        {SMART_SATA_ATTRIBUTE_TYPE,         RFPROP_SATASMART_ATTR_MODE},
        {SMART_SATA_ATTRIBUTE_ID,           RFPROP_SATASMART_ATTR_ID}
    };

    if (NULL == i_attr_json || NULL == *o_jso_obj) {
        return FALSE;
    }

    
    gint32 length = G_N_ELEMENTS(bmc_to_attr);
    gint32 count = json_object_object_length(i_attr_json);
    return_val_if_fail(count == length, FALSE);

    for (i = 0; i < count; i++) {
        
        if (json_object_object_get_ex(i_attr_json, bmc_to_attr[i].str1, &attr_value)) {
            if (NULL != attr_value) {
                json_object_object_add(*o_jso_obj, bmc_to_attr[i].str2, attr_value);
                json_object_get(attr_value);
            }
        }
    }

    return TRUE;
}


LOCAL gint32 get_drive_oem_satasmart_attrlist(OBJ_HANDLE obj_handle, json_object **o_json_attr)
{
    guint32 i;
    guint32 len;
    gsize parm_temp = 0;
    GVariant *attr_array = NULL;
    const gchar **str_value = NULL;
    json_object *attr_jso = NULL;
    json_object *jso_obj = NULL;

    
    gint32 ret = dfl_get_property_value(obj_handle, PROPERTY_HDD_ATTR_ITEM_LIST, &attr_array);
    return_val_do_info_if_fail((VOS_OK == ret) || (NULL != attr_array), VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: get %s property fail.", __FUNCTION__, __LINE__, PROPERTY_HDD_ATTR_ITEM_LIST));

    str_value = g_variant_get_strv(attr_array, &parm_temp);
    len = (guint32)parm_temp;

    if (NULL == str_value) {
        debug_log(DLOG_MASS, "get value array failed");
        g_variant_unref(attr_array);
        return VOS_ERR;
    }

    
    for (i = 0; i < len; i++) {
        attr_jso = NULL;
        
        attr_jso = json_tokener_parse(str_value[i]);
        do_if_expr(NULL == attr_jso, continue);

        
        jso_obj = json_object_new_object();
        do_if_expr(NULL == jso_obj, json_object_put(attr_jso); continue);
        
        do_if_fail(redfish_drive_get_attr_json_value(attr_jso, &jso_obj), json_object_put(attr_jso);
            json_object_put(jso_obj); continue);

        json_object_put(attr_jso);
        ret = json_object_array_add(*o_json_attr, jso_obj);
        do_info_if_true(ret, json_object_put(jso_obj);
            debug_log(DLOG_ERROR, "%s, %d: json object array add fail.", __FUNCTION__, __LINE__));
    }

    g_free(str_value);
    g_variant_unref(attr_array);

    return VOS_OK;
}


LOCAL void get_drive_oem_satasmartinfo(OBJ_HANDLE obj_handle, json_object *o_huawei_jso)
{
    json_object *jso_prop = NULL;
    json_object *json_sata = NULL;
    json_object *json_array = NULL;

    
    json_sata = json_object_new_object();
    return_do_info_if_fail(NULL != json_sata,
        json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_SATASMARTINFO, NULL);
        debug_log(DLOG_ERROR, "%s, %d: new json object fail.", __FUNCTION__, __LINE__));
    
    (void)get_drive_oem_property_type_is_uint32(obj_handle, PROPERTY_HDD_ATTR_REV_NUM, &jso_prop);
    (void)json_object_object_add(json_sata, RFPROP_SATASMART_ATTRREVISION, jso_prop);

    
    (void)get_drive_oem_property_type_is_byte(obj_handle, PROPERTY_HDD_ATTR_ITEM_NUM, &jso_prop);
    (void)json_object_object_add(json_sata, RFPROP_SATASMART_ATTRREVISIONNUM, jso_prop);

    
    json_array = json_object_new_array();
    return_do_info_if_fail(NULL != json_array,
        json_object_object_add(json_sata, RFPROP_SATASMART_ATTRREVISIONLIST, NULL);
        (void)json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_SATASMARTINFO, json_sata));

    (void)get_drive_oem_satasmart_attrlist(obj_handle, &json_array);
    (void)json_object_object_add(json_sata, RFPROP_SATASMART_ATTRREVISIONLIST, json_array);
    (void)json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_SATASMARTINFO, json_sata);

    return;
}


LOCAL gint32 get_check_drive_type(OBJ_HANDLE obj_handle, json_object *o_huawei_jso)
{
    gchar class_name[MAX_NAME_SIZE] = {0};
    json_object *jso_prop = NULL;

    gint32 ret = dfl_get_class_name(obj_handle, class_name, MAX_NAME_SIZE);
    return_val_do_info_if_fail(DFL_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "dfl_get_class_name fail, ret is %d\n", ret));

    if (0 == g_strcmp0(class_name, CLASS_HDD_NAME)) {
        jso_prop = json_object_new_string(DRIVE_TYPE_DISK);
    } else if (0 == g_strcmp0(class_name, CLASS_PCIEHDD_NAME)) {
        jso_prop = json_object_new_string(DRIVE_TYPE_PCIESSDCARD);
    } else {
        jso_prop = NULL;
    }

    
    (void)json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_TYPE, jso_prop);
    return VOS_OK;
}


LOCAL void get_drive_oem_nvmesmartinfo(OBJ_HANDLE obj_handle, json_object *o_huawei_jso)
{
    guint32 media_err = 0;
    json_object *smart_obj = NULL;
    json_object *media_err_jso = NULL;
    return_if_expr(0 == obj_handle || NULL == o_huawei_jso);

    
    smart_obj = json_object_new_object();
    return_do_info_if_expr(NULL == smart_obj,
        json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_NVMESMARTINFO, smart_obj));
    

    // 媒体错误计数
    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_HDD_MEDIA_ERR_COUNT, &media_err);
    if (DRIVE_SMART_INVALID_U32_VALUE != media_err) {
        media_err_jso = json_object_new_int((int32_t)media_err);
    }
    json_object_object_add(smart_obj, RFPROP_SASSMART_MEDIA_ERROR_COUNT, media_err_jso);

    json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_NVMESMARTINFO, smart_obj);

    return;
}


LOCAL void get_drive_oem_property_powerstate(OBJ_HANDLE obj_handle, json_object *o_huawei_jso)
{
    guchar prop_info = 0;
    const gchar *prop_str = NULL;

    return_if_expr(NULL == o_huawei_jso);

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_HDD_POWER_STATE, &prop_info);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s, %d: get Raid Controller SASAddress fail.\n", __FUNCTION__, __LINE__));

    if (RFPROP_DRIVE_POWER_STATE_V_SPUNUP_NUM == prop_info) {
        prop_str = RFPROP_DRIVE_POWER_STATE_V_SPUNUP;
    } else if (RFPROP_DRIVE_POWER_STATE_V_SPUNDOWN_NUM == prop_info) {
        prop_str = RFPROP_DRIVE_POWER_STATE_V_SPUNDOWN;
    } else if (RFPROP_DRIVE_POWER_STATE_V_TRANSITION_NUM == prop_info) {
        prop_str = RFPROP_DRIVE_POWER_STATE_V_TRANSITION;
    } else {
        json_object_object_add(o_huawei_jso, RFPROP_DRIVE_POWER_STATE, NULL);
        return;
    }
    json_object_object_add(o_huawei_jso, RFPROP_DRIVE_POWER_STATE, json_object_new_string(prop_str));

    return;
}


LOCAL void get_drive_oem_property(OBJ_HANDLE obj_handle, json_object *o_huawei_jso)
{
    json_object *jso_prop = NULL;
    gchar prop_str[MAX_STRBUF_LEN] = {0};
    gdouble prop_value = 0.0;
    
    gint32 ret = get_check_drive_type(obj_handle, o_huawei_jso);
    do_val_if_fail(VOS_OK == ret, (void)json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_TYPE, NULL);
        debug_log(DLOG_ERROR, "%s, %d: get drive type is fail.", __FUNCTION__, __LINE__));
    
    jso_prop = NULL;
    
    (void)get_drive_oem_property_type_is_string(obj_handle, PROPERTY_HDD_PYSICAL_LOCATION, &jso_prop);
    (void)json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_POSITION, jso_prop);
    jso_prop = NULL;

    
    (void)dal_get_property_value_double(obj_handle, PROPERTY_HDD_HOUR_POWER_UP, &prop_value);

    if ((DRIVE_SMART_INVALID_U16_VALUE <= (guint16)prop_value)) {
        json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_POWERUP, NULL);
    } else {
        jso_prop = ex_json_object_new_double(prop_value, "%.2f");
        do_val_if_fail(NULL != jso_prop, json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_POWERUP, NULL));
        (void)json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_POWERUP, jso_prop);
    }

    (void)memset_s(prop_str, sizeof(prop_str), 0, sizeof(prop_str));

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_HDD_INTERFACE_TYPE_STR, prop_str, sizeof(prop_str));
    return_do_info_if_fail(VOS_OK == ret, json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_SATASMARTINFO, NULL);
        (void)json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_SASSMARTINFO, NULL));

    
    if (0 == g_strcmp0(prop_str, RFPROP_DRIVE_INTERFACE_SATA)) {
        (void)get_drive_oem_satasmartinfo(obj_handle, o_huawei_jso);
        (void)json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_SASSMARTINFO, NULL);
    } 
    else if (0 == g_strcmp0(prop_str, RFPROP_DRIVE_INTERFACE_SAS)) {
        (void)get_drive_oem_sassmartinfo(obj_handle, o_huawei_jso);
        (void)json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_SATASMARTINFO, NULL);
    }
    
    else if (0 == g_strcmp0(prop_str, RFPROP_DRIVE_INTERFACE_PCIE)) { // nvme的smart信息
        get_drive_oem_nvmesmartinfo(obj_handle, o_huawei_jso);
    }
    
    else {
        (void)json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_SATASMARTINFO, NULL);
        (void)json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_SASSMARTINFO, NULL);
    }

    return;
}


LOCAL void get_drive_oem_bdf(OBJ_HANDLE obj_handle, json_object *o_huawei_jso)
{
#define MAX_BDF_STR_LEN 16
    guint8 ret;
    guint8 InterfaceType = 0;
    guint8 device_num = 0;
    guint8 group_id = 0;
    guint8 handle_device_type = 0;
    GSList *obj_list = NULL;
    GSList *node = NULL;
    guint8 segment = 0;
    guint8 function = 0;
    guint8 device = 0;
    guint8 bus = 0;
    guint8 tmp_id = 0;
    guint8 tmp_type = 0;
    guint8 tmp_groupid = 0;
    OBJ_HANDLE tmp_handle = 0;
    OBJ_HANDLE component_handle = 0;
    gchar bdf_str[MAX_BDF_STR_LEN] = {0};

    return_if_expr(NULL == o_huawei_jso);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_INTERFACE_TYPE, &InterfaceType);
    // 判断是不是Pcie盘
    if (InterfaceType != PD_INTERFACE_TYPE_PCIE) {
        (void)json_object_object_add(o_huawei_jso, PROPERTY_DEVICE_OEM_BDF, NULL);
        return;
    }

    // 通过引用对象获取PcieAddrInfo类的对象去匹配后获取BDF信息
    ret = dfl_get_referenced_object(obj_handle, PROPERTY_PCIE_CARD_REF_COMPONENT, &component_handle);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "%s: get object fail, ret is %d", __FUNCTION__, ret));
    
    (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICENUM, &device_num);
    (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_GROUPID, &group_id);
    (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &handle_device_type);
    ret = dfl_get_object_list(CLASS_PCIE_ADDR_INFO, &obj_list);
    return_do_info_if_fail(VOS_OK == ret, do_if_expr(obj_list != NULL, g_slist_free(obj_list));
        debug_log(DLOG_DEBUG, "%s: dfl_get_object_list CLASS_PCIE_ADDR_INFO fail", __FUNCTION__));

    
    for (node = obj_list; node; node = node->next) {
        tmp_handle = (OBJ_HANDLE)node->data;
        (void)dal_get_property_value_byte(tmp_handle, PROPERTY_PCIE_SLOT_ID, &tmp_id);
        (void)dal_get_property_value_byte(tmp_handle, PROPERTY_COMPONENT_TYPE, &tmp_type);
        (void)dal_get_property_value_byte(tmp_handle, PROPERTY_GROUP_ID, &tmp_groupid);
        if ((tmp_id == device_num) && (handle_device_type == tmp_type) && (tmp_groupid == group_id)) {
            (void)dal_get_property_value_byte(tmp_handle, PROPERTY_PCIE_BUS, &bus);
            (void)dal_get_property_value_byte(tmp_handle, PROPERTY_PCIE_DEVICE, &device);
            (void)dal_get_property_value_byte(tmp_handle, PROPERTY_PCIE_FUNCTION, &function);
            // 格式化Bus.Drives.Function值为字符串
            (void)snprintf_s(bdf_str, MAX_BDF_STR_LEN, MAX_BDF_STR_LEN - 1, "%04x:%02x:%02x.%01x", segment, bus, device,
                function);
            debug_log(DLOG_INFO, "rootBDF:%s \r\n", bdf_str);
            (void)json_object_object_add(o_huawei_jso, PROPERTY_DEVICE_OEM_BDF,
                json_object_new_string((const gchar*)bdf_str));
            g_slist_free(obj_list);
            return;
        }
    }
    g_slist_free(obj_list);
    (void)json_object_object_add(o_huawei_jso, PROPERTY_DEVICE_OEM_BDF, NULL);
    return;
}


LOCAL void get_drive_oem_associated_resource(OBJ_HANDLE obj_handle, json_object *o_huawei_jso)
{
    guint8 interface_type = 0;
    gint32 ret = 0;
    json_object *associated_resource_jso = NULL;
    return_if_expr(NULL == o_huawei_jso);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_INTERFACE_TYPE, &interface_type);
    if (PD_INTERFACE_TYPE_PCIE == interface_type) {
        
        ret = get_chassis_pciefunction_oem_associatedresource(obj_handle, &associated_resource_jso);
        return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "%s: get  associated resource fail", __FUNCTION__));
        json_object_object_add(o_huawei_jso, RFPROP_PCIEFUNCTION_CONNECTEDCPU, associated_resource_jso);
    } else {
        json_object_object_add(o_huawei_jso, RFPROP_PCIEFUNCTION_CONNECTEDCPU, NULL);
    }
    return;
}


void is_disk_support_crypto_erase(OBJ_HANDLE obj_handle, guint8 *crypto_erase_enabled)
{
    OBJ_HANDLE raid_obj_handle = 0;
    guint32 property_uint = G_MAXUINT32;
    guint8 fde_capable = G_MAXUINT8;
    gchar       property_temp[PROP_VAL_LENGTH] = {0};

#define CTRL_OPTION2_SUPPORT_CRYPTO_ERASE 17

    return_if_expr(NULL == crypto_erase_enabled);

    // 获取硬盘是否是FDE
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_FDE_CAPABLE, &fde_capable);
    // 获取失败或者不支持带外管理，无法确定是否支持该操作
    if (G_MAXUINT8 == fde_capable) {
        return;
    } else if (DISABLE == fde_capable) {
        *crypto_erase_enabled = DISABLE;
        return;
    }

    // 获取硬盘关联的RAID卡的FW是否支持加密盘擦除操作
    (void)dal_get_property_value_string(obj_handle, PROPERTY_HDD_REF_RAID_CONTROLLER, property_temp,
        sizeof(property_temp));
    gint32 ret_val = dfl_get_object_handle(property_temp, &raid_obj_handle);
    // 获取关联RAID卡失败，无法确定是否支持该操作
    if (DFL_OK != ret_val) {
        return;
    }
    (void)dal_get_property_value_uint32(raid_obj_handle, PROPERTY_RAID_CONTROLLER_CTRL_OPTION2, &property_uint);
    // 获取属性失败，无法确定是否支持该操作
    if (G_MAXUINT32 == property_uint) {
        return;
    } else if (0 == (property_uint & BIT(CTRL_OPTION2_SUPPORT_CRYPTO_ERASE))) {
        *crypto_erase_enabled = DISABLE;
        return;
    }
    *crypto_erase_enabled = ENABLE;

    return;
}


LOCAL void get_drive_oem_crypto_erase_enabled(OBJ_HANDLE obj_handle, json_object *o_huawei_jso)
{
    json_object *crypto_erase_enabled_jso = NULL;
    guint8 crypto_erase_enabled = G_MAXUINT8;
    return_if_expr(NULL == o_huawei_jso);

    is_disk_support_crypto_erase(obj_handle, &crypto_erase_enabled);

    if (G_MAXUINT8 != crypto_erase_enabled) {
        crypto_erase_enabled_jso = json_object_new_boolean(crypto_erase_enabled);
    }
    json_object_object_add(o_huawei_jso, PROPERTY_DEVICE_OEM_CRYPTO_ERASE_SUPPORTED, crypto_erase_enabled_jso);
    return;
}


LOCAL gint32 redfish_drive_form_factor_size(guint8 form_factor, const gchar** str_from_factor)
{
    guint32 i;
    guint32 size;
    INT2STR_MAP_S    form_factor_map[] = {
        {PD_FORM_FACTOR_UNKNOWN,                RFPROP_DRIVE_OEM_FORM_FACTOR_UNKNOWN},
        {PD_FORM_FACTOR_5_25_INCH,              RFPROP_DRIVE_OEM_FORM_FACTOR_5_25_INCH},
        {PD_FORM_FACTOR_3_5_INCH,               RFPROP_DRIVE_OEM_FORM_FACTOR_3_5_INCH},
        {PD_FORM_FACTOR_2_5_INCH,               RFPROP_DRIVE_OEM_FORM_FACTOR_2_5_INCH},
        {PD_FORM_FACTOR_1_8_INCH,               RFPROP_DRIVE_OEM_FORM_FACTOR_1_8_INCH},
        {PD_FORM_FACTOR_LESS_THAN_1_8_INCH,     RFPROP_DRIVE_OEM_FORM_FACTOR_LESS_THAN_1_8_INCH},
        {PD_FORM_FACTOR_MSATA,                  RFPROP_DRIVE_OEM_FORM_FACTOR_MSATA},
        {PD_FORM_FACTOR_M2,                     RFPROP_DRIVE_OEM_FORM_FACTOR_M2},
        {PD_FORM_FACTOR_MICRO_SSD,              RFPROP_DRIVE_OEM_FORM_FACTOR_MICRO_SSD},
        {PD_FORM_FACTOR_CFAST,                  RFPROP_DRIVE_OEM_FORM_FACTOR_CFAST},
    };

    *str_from_factor = NULL;
    size = sizeof(form_factor_map) / sizeof(form_factor_map[0]);

    for (i = 0; i < size; i++) {
        if (form_factor == form_factor_map[i].int_val) {
            *str_from_factor = form_factor_map[i].str_val;
            return RET_OK;
        }
    }

    return RET_ERR;
}


LOCAL void get_drive_oem_form_factor(OBJ_HANDLE obj_handle, json_object *o_huawei_jso)
{
    guint8 form_factor = G_MAXUINT8;
    json_object *form_factor_jso = NULL;
    const gchar* str_form_factor = NULL;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_FORM_FACTOR, &form_factor);

    if (redfish_drive_form_factor_size(form_factor, &str_form_factor) == RET_OK) {
        form_factor_jso = json_object_new_string(str_form_factor);
    }

    json_object_object_add(o_huawei_jso, RFPROP_DRIVE_OEM_FORM_FACTOR, form_factor_jso);
}

LOCAL void get_drive_oem_boot_priority(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 boot_priority = INVALID_DATA_BYTE;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_BOOT_PRIORITY, &boot_priority);

    const gchar* boot_priority_name = sml_ctrl_boot_priority_num2str(boot_priority);
    if (g_strcmp0(boot_priority_name, STORAGE_INFO_INVALID_STRING) == 0) {
        json_object_object_add(result, RFPROP_DRIVE_OEM_BOOT_PRIORITY, NULL);
    } else {
        json_object_object_add(result, RFPROP_DRIVE_OEM_BOOT_PRIORITY, json_object_new_string(boot_priority_name));
    }
    return;
}

LOCAL void add_drive_oem_property(OBJ_HANDLE obj_handle, json_object *oem_huawei_jso)
{
    rf_add_property_jso_byte(obj_handle, PROPERTY_HDD_SLOT_NUMBER, RFPROP_DRIVE_OEM_SLOTNUMBER, oem_huawei_jso);
    rf_add_property_jso_uint16(obj_handle, PROPERTY_HDD_ENCLOSURE_DEVICE_ID, RFPROP_DRIVE_OEM_ENCLOSUREDEVICEID,
        oem_huawei_jso);
    get_drive_oem_boot_priority(obj_handle, oem_huawei_jso);
    rf_add_property_jso_bool(obj_handle, PROPERTY_HDD_BOOT_PRIORITY, RFPROP_DRIVE_OEM_BOOT_ENABLE, oem_huawei_jso);
    return;
}

LOCAL void get_oem_property(OBJ_HANDLE obj_handle, json_object *oem_huawei_jso)
{
    json_object *jso_sasaddress = NULL;
    json_object *jso_hotspare = NULL;
    gchar controller_name[MAX_RSC_NAME_LEN] = {0};
    OBJ_HANDLE ctr_obj_handle = 0;
    
    gint32 ret = get_drive_oem_driveid(obj_handle, oem_huawei_jso);
    do_val_if_fail(VOS_OK == ret, (void)json_object_object_add(oem_huawei_jso, RFPROP_DRIVE_DRIVEID, NULL);
        debug_log(DLOG_ERROR, "%s, %d: get value of driveid is fail.", __FUNCTION__, __LINE__));
    
    
    ret = get_drive_oem_rebuildstate(obj_handle, oem_huawei_jso);
    do_val_if_fail(VOS_OK == ret, (void)json_object_object_add(oem_huawei_jso, RFPROP_DRIVE_OEM_REBUILDSTATE, NULL);
        debug_log(DLOG_MASS, "%s, %d: get value of rebuildstate is fail.", __FUNCTION__, __LINE__));
    
    get_drive_oem_associated_resource(obj_handle, oem_huawei_jso);
    
    ret = get_drive_oem_rebuildprogress(obj_handle, oem_huawei_jso);
    do_val_if_fail(VOS_OK == ret, (void)json_object_object_add(oem_huawei_jso, RFPROP_DRIVE_OEM_EBUILDPROGRESS, NULL);
        debug_log(DLOG_MASS, "%s, %d: get value of rebuildstate is fail.", __FUNCTION__, __LINE__));

    
    ret = get_drive_oem_patrolstate(obj_handle, oem_huawei_jso);
    do_val_if_fail(VOS_OK == ret, (void)json_object_object_add(oem_huawei_jso, RFPROP_DRIVE_OEM_PATROLSTATE, NULL);
        debug_log(DLOG_MASS, "%s, %d: get value of patrolstate is fail.", __FUNCTION__, __LINE__));

    
    ret = get_drive_oem_temperature(obj_handle, oem_huawei_jso);
    do_val_if_fail(VOS_OK == ret, (void)json_object_object_add(oem_huawei_jso, RFPROP_DRIVE_OEM_TEMPERATURE, NULL);
        debug_log(DLOG_MASS, "%s, %d: get value of Temperature is fail.", __FUNCTION__, __LINE__));

    
    ret = get_drive_oem_physicaldiskstate(obj_handle, oem_huawei_jso);
    do_val_if_fail(VOS_OK == ret,
        (void)json_object_object_add(oem_huawei_jso, RFPROP_DRIVE_OEM_PHYSICALDISKSTATE, NULL);
        debug_log(DLOG_MASS, "%s, %d: get value of PhysicalDiskState is fail.", __FUNCTION__, __LINE__));

    
    jso_hotspare = json_object_new_array();
    if (NULL != jso_hotspare) {
        
        (void)get_drive_oem_hotspareoforlogical(obj_handle, jso_hotspare);
        json_object_object_add(oem_huawei_jso, RFPROP_DRIVE_OEM_HOTFORLOGICAL, jso_hotspare);
    } else {
        json_object_object_add(oem_huawei_jso, RFPROP_DRIVE_OEM_HOTFORLOGICAL, NULL);
    }

    
    jso_sasaddress = json_object_new_array();
    if (NULL != jso_sasaddress) {
        (void)get_drive_oem_sasaddress(obj_handle, jso_sasaddress);
        json_object_object_add(oem_huawei_jso, RFPROP_DRIVE_OEM_SASADDRESS, jso_sasaddress);
    } else {
        json_object_object_add(oem_huawei_jso, RFPROP_DRIVE_OEM_SASADDRESS, NULL);
    }

    
    (void)get_drive_oem_property_powerstate(obj_handle, oem_huawei_jso);
    

    
    (void)get_drive_oem_property(obj_handle, oem_huawei_jso);
    
    (void)get_drive_oem_bdf(obj_handle, oem_huawei_jso);
    get_drive_oem_crypto_erase_enabled(obj_handle, oem_huawei_jso);

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_HDD_REF_RAID_CONTROLLER, controller_name,
        sizeof(controller_name));
    if (VOS_OK == ret && strlen(controller_name) > 0) {
        ret = dfl_get_object_handle(controller_name, &ctr_obj_handle);
        if (VOS_OK == ret) {
            json_object *array_jso = json_object_new_array();
            return_do_info_if_fail(NULL != array_jso,
                debug_log(DLOG_ERROR, "%s, %d: new logical object fail.\n", __FUNCTION__, __LINE__));
            ret = get_storage_logical_rsc(ctr_obj_handle, array_jso);
            do_if_fail(ret, debug_log(DLOG_DEBUG, "%s, %d: get logiccal drive error.\n", __FUNCTION__, __LINE__));
            redfish_storage_get_related_array_info(obj_handle, oem_huawei_jso, array_jso);
            json_object_put(array_jso);
        }
    } else {
        json_object_object_add(oem_huawei_jso, RFPROP_STORAGE_CONTROLLER_ARRAY, NULL);
    }
    
    add_drive_oem_property(obj_handle, oem_huawei_jso);

    get_drive_oem_form_factor(obj_handle, oem_huawei_jso);

    return;
}


LOCAL gint32 get_system_storage_drive_oem_property(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *oem_huawei_jso = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    oem_huawei_jso = json_object_new_object();
    if (NULL != oem_huawei_jso) {
        (void)get_oem_property(i_paras->obj_handle, oem_huawei_jso);
        *o_result_jso = json_object_new_object();
        return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(oem_huawei_jso));
        json_object_object_add(*o_result_jso, RFPROP_FWINV_HUAWEI, oem_huawei_jso);
    } else {
        
        debug_log(DLOG_ERROR, "%s, %d:json_object_new_array fail.", __FUNCTION__, __LINE__);
        
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_system_storage_drive_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *oem = NULL;
    json_object *huawei = NULL;
    gchar hdd_uri[MAX_URL_LEN] = {0};
    gchar class_name[MAX_NAME_SIZE] = {0};

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    gint32 ret = dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);
    return_val_if_fail(DFL_OK == ret, HTTP_INTERNAL_SERVER_ERROR);

    // 目前只认为Hdd类可能会使用该action PcieDisk和SD卡类该属性显示null
    return_val_if_fail(0 == g_strcmp0(class_name, CLASS_HDD_NAME), HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_object();
    return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    oem = json_object_new_object();
    
    return_val_if_expr(NULL == oem, HTTP_OK);
    json_object_object_add(*o_result_jso, RFPROP_COMMON_OEM, oem);

    huawei = json_object_new_object();
    
    return_val_if_expr(NULL == huawei, HTTP_OK);
    json_object_object_add(oem, RFPROP_COMMON_HUAWEI, huawei);

    ret = redfish_get_drive_uri(i_paras->obj_handle, hdd_uri, sizeof(hdd_uri));
    return_val_if_fail(VOS_OK == ret, HTTP_OK);

    rf_add_action_prop(huawei, hdd_uri, RFACTION_DRIVE_CRYPTO_ERASE);
    return HTTP_OK;
}


LOCAL gint32 redfish_drive_set_hotspare_result(gint32 i_ret, json_object **o_message_jso)
{
    if (i_ret == RET_OK) {
        return HTTP_OK;
    }

    RET_MSG_INFO_MAP set_hotspare_result_map[] = {
        // 物理盘是坏盘，或者物理盘不属于raid卡管理
        {SM_CODE_PD_NOT_IDENTIFIED, HTTP_BAD_REQUEST, MSGID_PROPERTY_STATUS_UNSUPPORT_MODIF,
         RFPROP_DRIVE_HOTSPARETYPE, RFPROP_DRIVE_HOTSPARETYPE, NULL, NULL},
        {SM_CODE_NOT_SUPPORT_IN_PRESENT_STATE, HTTP_BAD_REQUEST, MSGID_PROPERTY_STATUS_UNSUPPORT_MODIF,
         RFPROP_DRIVE_HOTSPARETYPE, RFPROP_DRIVE_HOTSPARETYPE, NULL, NULL},
        {SM_CODE_GET_PD_LIST_FAILED, HTTP_BAD_REQUEST, MSGID_PROPERTY_STATUS_UNSUPPORT_MODIF,
         RFPROP_DRIVE_HOTSPARETYPE, RFPROP_DRIVE_HOTSPARETYPE, NULL, NULL},
        {SML_ERR_I2C_READ_WRITE_FAILED, HTTP_BAD_REQUEST, MSGID_PROPERTY_STATUS_UNSUPPORT_MODIF,
         RFPROP_DRIVE_HOTSPARETYPE, RFPROP_DRIVE_HOTSPARETYPE, NULL, NULL},
        // 物理盘状态不支持设置
        {SML_ERR_PD_MAKESPARE_NOT_ALLOWED, HTTP_BAD_REQUEST, MSGID_PROPERTY_PD_STATE_NOT_SUPPORT,
         RFPROP_DRIVE_HOTSPARETYPE, RFPROP_DRIVE_HOTSPARETYPE, NULL, NULL},
        {MFI_STAT_WRONG_STATE, HTTP_BAD_REQUEST, MSGID_PROPERTY_PD_STATE_NOT_SUPPORT,
         RFPROP_DRIVE_HOTSPARETYPE, RFPROP_DRIVE_HOTSPARETYPE, NULL, NULL},
        {SML_ERR_INVALID_PARAMETER, HTTP_BAD_REQUEST, MSGID_PROPERTY_PD_STATE_NOT_SUPPORT,
         RFPROP_DRIVE_HOTSPARETYPE, RFPROP_DRIVE_HOTSPARETYPE, NULL, NULL},
        {SML_ERR_PD_STATE_UNSUPPORTED_TO_SET, HTTP_BAD_REQUEST, MSGID_PROPERTY_PD_STATE_NOT_SUPPORT,
         RFPROP_DRIVE_HOTSPARETYPE, RFPROP_DRIVE_HOTSPARETYPE, NULL, NULL},
        {SML_ERR_CONFIG_RESOURCE_CONFLICT, HTTP_BAD_REQUEST, MSGID_PROPERTY_PD_STATE_NOT_SUPPORT,
         RFPROP_DRIVE_HOTSPARETYPE, RFPROP_DRIVE_HOTSPARETYPE, NULL, NULL},
        // RAID等级不支持
        {SML_ERR_PD_SPARE_FOR_RAID0_LD, HTTP_BAD_REQUEST, MSGID_PROPERTY_RAIDLEVEL_INVAID, RFPROP_DRIVE_HOTSPARETYPE,
         RFPROP_DRIVE_HOTSPARETYPE, NULL, NULL},
        // 作为热备的物理盘与逻辑盘成员盘同为SSD,但是接口不同
        {SML_ERR_PD_SPARE_SDD_SAS_SATA_MIXED, HTTP_BAD_REQUEST, MSGID_PROPERTY_PD_INTERFACE_MIXED,
         RFPROP_DRIVE_HOTSPARETYPE, RFPROP_DRIVE_HOTSPARETYPE, NULL, NULL},
        // 作为热备的物理盘与逻辑盘成员盘为不同介质的硬盘(SSD/HDD)
        {SML_ERR_PD_SPARE_SDD_HDD_MIXED, HTTP_BAD_REQUEST, MSGID_PROPERTY_PD_MEDIAMIXED, RFPROP_DRIVE_HOTSPARETYPE,
         RFPROP_DRIVE_HOTSPARETYPE, NULL, NULL},
        {SML_ERR_CTRL_STATUS_INVALID, HTTP_BAD_REQUEST, MSGID_OPERATION_FAILED, RFPROP_DRIVE_HOTSPARETYPE,
         NULL, NULL, NULL},
        {SML_ERR_PD_OPERATION_NOT_SUPPORT, HTTP_BAD_REQUEST, MSGID_MODIFY_FAIL_CONTROLLER_NOT_SUPPORT,
         RFPROP_DRIVE_HOTSPARETYPE, RFPROP_DRIVE_HOTSPARETYPE, NULL, NULL},
        {SML_ERR_CONFIG_ASSOCIATED_LD_SIZE_OUT_OF_RANGE, HTTP_BAD_REQUEST, MSGID_ASSOCIATED_VOLUME_CAPACITY_ERR,
         RFPROP_DRIVE_HOTSPARETYPE, NULL, NULL, NULL},
        {SML_ERR_CONFIG_BLOCK_SIZE_NOT_SAME, HTTP_BAD_REQUEST, MSGID_DIFF_DRIVES_SECTOR_SIZE,
         RFPROP_DRIVE_HOTSPARETYPE, NULL, NULL, NULL},
        {SM_CODE_OPERATION_IN_PROGRESS, HTTP_BAD_REQUEST, VMM_MESSAGE_OPERATIONINPROCESS, NULL, NULL, NULL, NULL},
    };

    for (gsize i = 0; i < G_N_ELEMENTS(set_hotspare_result_map); i++) {
        if (i_ret == set_hotspare_result_map[i].ret_val) {
            (void)create_message_info(set_hotspare_result_map[i].msg_id, set_hotspare_result_map[i].prop_name,
                o_message_jso, set_hotspare_result_map[i].param1, set_hotspare_result_map[i].param2,
                set_hotspare_result_map[i].param3);
            return set_hotspare_result_map[i].response_code;
        }
    }

    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL void del_firmware_status_if_exist(json_object *user_data)
{
    json_object *json_oem = NULL;
    json_object *json_huawei = NULL;
    json_object *json_logical = NULL;

    if (json_object_object_get_ex(user_data, RFPROP_COMMON_OEM, &json_oem) != TRUE ||
        json_object_object_get_ex(json_oem, RFPROP_FWINV_HUAWEI, &json_huawei) != TRUE) {
        return;
    }

    if (json_object_object_get_ex(json_huawei, RFPROP_DRIVE_OEM_PHYSICALDISKSTATE, &json_logical) == TRUE) {
        (void)json_object_object_del(json_huawei, RFPROP_DRIVE_OEM_PHYSICALDISKSTATE);
    }

    return;
}


LOCAL void del_hotspare_drives_if_exist(json_object *user_data)
{
    json_object *json_oem = NULL;
    json_object *json_huawei = NULL;
    json_object *json_logical = NULL;

    if (json_object_object_get_ex(user_data, RFPROP_COMMON_OEM, &json_oem) != TRUE ||
        json_object_object_get_ex(json_oem, RFPROP_FWINV_HUAWEI, &json_huawei) != TRUE) {
        return;
    }

    if (json_object_object_get_ex(json_huawei, RFPROP_DRIVE_OEM_HOTFORLOGICAL, &json_logical) == TRUE) {
        (void)json_object_object_del(json_huawei, RFPROP_DRIVE_OEM_HOTFORLOGICAL);
    }

    return;
}


LOCAL gint32 redfish_set_oem_compare_property(GSList **input_list, const gchar *json_string)
{
    return_val_if_expr(NULL == json_string, VOS_ERR);

    if ((0 == g_strcmp0(RF_PHYSICALDISKSTATE_ONLINE, json_string))) {
        *input_list = g_slist_append(*input_list, g_variant_new_byte(PD_STATE_ONLINE));
    } else if ((0 == g_strcmp0(RF_PHYSICALDISKSTATE_CONFIGUREDGOOD, json_string))) {
        *input_list = g_slist_append(*input_list, g_variant_new_byte(PD_STATE_OFFLINE));
    } else if ((0 == g_strcmp0(RF_PHYSICALDISKSTATE_CONTROLLEDBYHOST, json_string))) {
        *input_list = g_slist_append(*input_list, g_variant_new_byte(PD_STATE_SYSTEM));
    } else if ((0 == g_strcmp0(RF_PHYSICALDISKSTATE_UNCONFIGUREDGOOD, json_string))) {
        *input_list = g_slist_append(*input_list, g_variant_new_byte(PD_STATE_UNCONFIGURED_GOOD));
    } else {
        return VOS_ERR;
    }

    return RET_OK;
}


LOCAL gint32 pick_logical_id(OBJ_HANDLE i_hdd_handle, const gchar *i_json_str,
    gchar *i_path, guint16 *o_id, json_object **o_msg_jso)
{
    guint8 hdd_id = 0;
    guint8 ctrl_id = 0;

    
    gint32 ret = get_system_storage_driveid_and_controllerid(i_hdd_handle, &ctrl_id, &hdd_id);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_msg_jso);
        return RET_ERR;
    }

    if (get_ld_id_from_uri(ctrl_id, i_json_str, o_id) != RET_OK) {
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, i_path, o_msg_jso, i_json_str, i_path);
        return RET_ERR;
    }

    return VOS_OK;
}

LOCAL gint32 get_spare_drives_obj(json_object *i_val_jso, json_object **o_prop_jso, json_object **o_msg_jso)
{
    json_object *jso_oem_json = NULL;
    json_object *jso_huawei_json = NULL;
    json_object *jso_prop_json = NULL;
    gchar *prop_path = NULL;

    if (json_object_object_get_ex(i_val_jso, RFPROP_COMMON_OEM, &jso_oem_json) != TRUE) {
        prop_path = RFPROP_COMMON_OEM;
        (void)create_message_info(MSGID_PROP_MISSING, prop_path, o_msg_jso, prop_path);
        return VOS_ERR;
    }

    if (json_object_object_get_ex(jso_oem_json, RFPROP_FWINV_HUAWEI, &jso_huawei_json) != TRUE) {
        prop_path = RFPROP_COMMON_OEM "/" RFPROP_FWINV_HUAWEI;
        (void)create_message_info(MSGID_PROP_MISSING, prop_path, o_msg_jso, prop_path);
        return VOS_ERR;
    }

    if (json_object_object_get_ex(jso_huawei_json, RFPROP_DRIVE_OEM_HOTFORLOGICAL, &jso_prop_json) != TRUE) {
        prop_path = RFPROP_COMMON_OEM "/" RFPROP_FWINV_HUAWEI "/" RFPROP_DRIVE_OEM_HOTFORLOGICAL;
        (void)create_message_info(MSGID_PROP_MISSING, prop_path, o_msg_jso, prop_path);
        return VOS_ERR;
    }

    *o_prop_jso = jso_prop_json;

    return VOS_OK;
}


LOCAL gint32 get_spare_logical_id(OBJ_HANDLE i_hdd_handle, json_object *i_prop_jso, guint16 *o_id,
    gint32 *o_id_cnt, json_object **o_msg_jso)
{
    const char *id_str = NULL;
    json_object *jso_value = NULL;
    json_object *jso_odata = NULL;
    gint32 array_len;
    gchar *prop_path = RFPROP_COMMON_OEM "/" RFPROP_FWINV_HUAWEI "/" RFPROP_DRIVE_OEM_HOTFORLOGICAL;

    if (i_prop_jso == NULL) {
        (void)create_message_info(MSGID_PROP_MISSING, prop_path, o_msg_jso, prop_path);
        return VOS_ERR;
    }

    array_len = json_object_array_length(i_prop_jso);
    if (array_len == 0 || array_len > *o_id_cnt) {
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, prop_path, o_msg_jso,
            dal_json_object_to_json_string(i_prop_jso), prop_path);
        debug_log(DLOG_MASS, "%s %d property format error!.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    // 从数组中获取对象
    for (gint32 i = 0; i < array_len; i++) {
        jso_value = json_object_array_get_idx(i_prop_jso, i);
        if (jso_value == NULL) {
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, prop_path, o_msg_jso,
                dal_json_object_to_json_string(i_prop_jso), prop_path);
            return VOS_ERR;
        }

        // 获取属性值
        if (json_object_object_get_ex(jso_value, RFPROP_FWINV_ODATA_ID, &jso_odata) != TRUE) {
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, prop_path, o_msg_jso,
                dal_json_object_to_json_string(i_prop_jso), prop_path);
            return VOS_ERR;
        }

        id_str = dal_json_object_get_str(jso_odata);
        if (pick_logical_id(i_hdd_handle, id_str, prop_path, &o_id[i], o_msg_jso) != VOS_OK) {
            return VOS_ERR;
        }
    }
    *o_id_cnt = array_len;

    return VOS_OK;
}


LOCAL gint32 redfish_check_drive_set_pdstate_ret(gint32 ret, json_object **o_message_jso)
{
    gchar *error_message_state = "Oem/Huawei/FirmwareStatus";

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

        // 物理盘是坏盘，或者物理盘不属于raid卡管理
        case SM_CODE_PD_NOT_IDENTIFIED:
        case SM_CODE_NOT_SUPPORT_IN_PRESENT_STATE:
        case SML_ERR_PD_INVALID_DEVICE_ID:
        case SM_CODE_GET_PD_LIST_FAILED:
            (void)create_message_info(MSGID_PROPERTY_STATUS_UNSUPPORT_MODIF, error_message_state, o_message_jso,
                error_message_state);
            return HTTP_BAD_REQUEST;

        // 物理盘状态不支持设置
        case MFI_STAT_WRONG_STATE:
        case SML_ERR_INVALID_CMD:
        case SML_ERR_PD_STATE_UNSUPPORTED_TO_SET:
            (void)create_message_info(MSGID_PROPERTY_PD_STATE_NOT_SUPPORT, error_message_state, o_message_jso,
                error_message_state);
            return HTTP_BAD_REQUEST;
        case SML_ERR_PD_OPERATION_NOT_SUPPORT:
        case SML_ERR_CTRL_STATUS_INVALID:
            (void)create_message_info(MSGID_MODIFY_FAIL_CONTROLLER_NOT_SUPPORT, error_message_state, o_message_jso,
                error_message_state);
            return HTTP_BAD_REQUEST;

        case SML_ERR_PD_MAX_UNCONFIGURED:
        case SML_ERR_I2C_READ_WRITE_FAILED:
            (void)create_message_info(MSGID_CURRENT_STATUS_NOT_SUPPORT, error_message_state, o_message_jso);
            return HTTP_BAD_REQUEST;
        case SM_CODE_OPERATION_IN_PROGRESS:
            (void)create_message_info(VMM_MESSAGE_OPERATIONINPROCESS, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso); // 内部错误
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 get_pd_set_return_val_message(gint32 ret, json_object *message_array, const gchar *prop_name,
    const gchar *prop_value)
{
    gint32 response = HTTP_BAD_REQUEST;
    json_object *pd_message = NULL;

    if (ret == RET_OK) {
        return HTTP_OK;
    }

    switch (ret) {
        
        case SM_CODE_PARA_DATA_ILLEGAL:
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, prop_name, &pd_message, prop_value, prop_name);
            break;

        
        case SM_CODE_PD_NOT_IDENTIFIED:
        case SM_CODE_NOT_SUPPORT_IN_PRESENT_STATE:
        case SM_CODE_GET_PD_LIST_FAILED:
            (void)create_message_info(MSGID_PROPERTY_STATUS_UNSUPPORT_MODIF, prop_name, &pd_message, prop_name);
            break;

        
        case SML_ERR_PD_INVALID_DEVICE_ID:
            (void)create_message_info(MSGID_PHY_DISK_ID_INVALID, prop_name, &pd_message, prop_value, prop_name);
            break;

        
        case SML_ERR_CTRL_INDEX_INVALID:
        
        case SML_ERR_CTRL_NOT_REGISTERED:
        
        case SML_ERR_CTRL_INIT_NOT_COMPLETED:
            (void)create_message_info(MSGID_CURRENT_STATUS_NOT_SUPPORT, prop_name, &pd_message);
            break;
        case SML_ERR_PD_STATE_UNSUPPORTED_TO_SET:
            (void)create_message_info(MSGID_PROPERTY_PD_STATE_NOT_SUPPORT, prop_name, &pd_message, prop_name);
            break;
        case SML_ERR_PD_OPERATION_NOT_SUPPORT:
            (void)create_message_info(MSGID_MODIFY_FAIL_CONTROLLER_NOT_SUPPORT, prop_name, &pd_message, prop_name);
            break;
        case SM_CODE_OPERATION_IN_PROGRESS:
            (void)create_message_info(VMM_MESSAGE_OPERATIONINPROCESS, NULL, &pd_message);
            break;
        default:
            debug_log(DLOG_ERROR, "%s:default error. response %d, ret %d", __FUNCTION__, response, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, prop_name, &pd_message);\
    }

    if ((json_object_array_add(message_array, pd_message)) != 0) {
        (void)json_object_put(pd_message);
        debug_log(DLOG_ERROR, "%s:json_object_array_add fail", __FUNCTION__);
    }

    return response;
}

LOCAL gint32 set_drive_oem_boot_enable(PROVIDER_PARAS_S *i_paras, json_object *o_message_jso,
    json_object *oem_property)
{
    gint32 response_code = HTTP_BAD_REQUEST;
    json_object *pd_message = NULL;
    const gchar *err_boot_enable = "Oem/Huawei/BootEnable";
    GSList *input_list = NULL;

    if ((oem_property == NULL) || (json_object_get_type(oem_property) != json_type_boolean)) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, err_boot_enable, &pd_message, RF_VALUE_NULL, err_boot_enable);
        
        if (json_object_array_add(o_message_jso, pd_message) != 0) {
            debug_log(DLOG_ERROR, "%s:json_object_array_add fail", __FUNCTION__);
            (void)json_object_put(pd_message);
        }
        return response_code;
    }

    gboolean boot_enable = json_object_get_boolean(oem_property);

    input_list = g_slist_append(input_list, g_variant_new_byte(boot_enable));

    gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_HDD_NAME, METHOD_HDD_SET_BOOTABLE, AUTH_DISABLE, 0, input_list, NULL);

    uip_free_gvariant_list(input_list);

    response_code = get_pd_set_return_val_message(ret, o_message_jso, err_boot_enable,
        dal_json_object_to_json_string(oem_property));

    return response_code;
}


LOCAL gint32 set_drive_oem_boot_priority(PROVIDER_PARAS_S *i_paras, json_object *o_message_jso,
    json_object *oem_property)
{
    json_object *pd_message = NULL;
    const gchar *err_boot_priority = "Oem/Huawei/BootPriority";
    GSList *input_list = NULL;

    const gchar* boot_priority_str = dal_json_object_get_str(oem_property);
    guint8 boot_priority = sml_ctrl_boot_priority_str2num(boot_priority_str);
    if (boot_priority == BOOT_PRIORITY_INVALID) {
        (void)create_message_info(MSGID_PROP_INVALID_VALUE, err_boot_priority, &pd_message,
            boot_priority_str, err_boot_priority);
        (void)json_object_array_add(o_message_jso, pd_message);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(boot_priority));

    gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_HDD_NAME, METHOD_HDD_SET_BOOTABLE, AUTH_DISABLE, 0, input_list, NULL);

    uip_free_gvariant_list(input_list);

    return get_pd_set_return_val_message(ret, o_message_jso, err_boot_priority, boot_priority_str);
}


LOCAL gint32 set_drive_oem_patrol_state(PROVIDER_PARAS_S *i_paras, json_object *o_message_jso,
    json_object *oem_property)
{
    json_object *pd_message = NULL;
    const gchar *err_patrol_state = "Oem/Huawei/PatrolState";
    GSList *input_list = NULL;
    guint8 patrol_state = STORAGE_INFO_INVALID_BYTE;

    const gchar* patrol_state_str = dal_json_object_get_str(oem_property);
    if (g_strcmp0(patrol_state_str, RF_PATROLSTATE_PATROLEDORNOPATROLED) == 0) {
        patrol_state = RFPROP_MANAGER_STATE_FALSE;
    } else if (g_strcmp0(patrol_state_str, RF_PATROLSTATE_PATROLLING) == 0) {
        patrol_state = RFPROP_MANAGER_STATE_TRUE;
    } else {
        (void)create_message_info(MSGID_PROP_INVALID_VALUE, err_patrol_state, &pd_message,
            patrol_state_str, err_patrol_state);
        (void)json_object_array_add(o_message_jso, pd_message);
        debug_log(DLOG_ERROR, "%s Invalid polling setting parameter %d.", __FUNCTION__, patrol_state);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(patrol_state));

    gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_HDD_NAME, METHOD_HDD_SET_PATROL_STATE, AUTH_DISABLE, 0, input_list, NULL);

    uip_free_gvariant_list(input_list);

    return get_pd_set_return_val_message(ret, o_message_jso, err_patrol_state, patrol_state_str);
}


LOCAL gint32 set_physical_state(PROVIDER_PARAS_S *i_paras, json_object *obj_state_json,
    gchar *prop_path, json_object **o_message_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    const char *obj_json_string = NULL;
    json_object *json_sparetype = NULL;

    return_val_if_expr(NULL == obj_state_json, HTTP_BAD_REQUEST);
    obj_json_string = dal_json_object_get_str(obj_state_json);
    // 判断是否有请求体中是否有"HotspareType"属性。
    return_val_do_info_if_expr(
        json_object_object_get_ex(i_paras->user_data, RFPROP_DRIVE_HOTSPARETYPE, &json_sparetype), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROPERTY_PD_NOT_SUPPORTSETSAMETIME, prop_path, o_message_jso,
        RFPROP_DRIVE_HOTSPARETYPE, prop_path);
        (void)json_object_object_add(i_paras->user_data, RFPROP_DRIVE_HOTSPARETYPE, NULL);
        debug_log(DLOG_MASS, "%s %d delete hotsparetype.", __FUNCTION__, __LINE__));
    // 创建消息体属性值不在列表
    if (redfish_set_oem_compare_property(&input_list, obj_json_string) != RET_OK) {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, prop_path, o_message_jso, obj_json_string, prop_path);
        debug_log(DLOG_MASS, "compare property error");
        return HTTP_BAD_REQUEST;
    }
	
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_HDD_NAME, METHOD_HDD_SET_FIRMWARE_STATE, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    return redfish_check_drive_set_pdstate_ret(ret, o_message_jso);
}


LOCAL gint32 set_spare_for_logical(PROVIDER_PARAS_S *i_paras, json_object **o_msg_jso)
{
    GSList *userdata = NULL;
    guint userdata_length = 0;
    json_bool ret_hotspare_type;
    json_object *json_sparetype = NULL;

    
    (void)json_object_get_user_data(i_paras->user_data, (void **)&userdata);
    userdata_length = g_slist_length((GSList *)userdata);

    ret_hotspare_type = json_object_object_get_ex(i_paras->user_data, RFPROP_DRIVE_HOTSPARETYPE, &json_sparetype);
    // 读取失败。用户未传入属性，创建属性缺失消息体
    if ((ret_hotspare_type == FALSE) && (userdata_length == 0)) {
        (void)create_message_info(MSGID_PROP_MISSING, RFPROP_DRIVE_HOTSPARETYPE, o_msg_jso,
            RFPROP_DRIVE_HOTSPARETYPE);
        debug_log(DLOG_MASS, "%s %d property missing.", __FUNCTION__, __LINE__);
        return HTTP_BAD_REQUEST;
    }

    // 读取属性成功
    if (ret_hotspare_type == TRUE) {
        return set_hot_spare(i_paras, json_sparetype, o_msg_jso);
    }

    return HTTP_BAD_REQUEST;
}


LOCAL void del_hotspare_type_if_exist(json_object *user_data)
{
    json_object *json_sparetype = NULL;

    if (json_object_object_get_ex(user_data, RFPROP_DRIVE_HOTSPARETYPE, &json_sparetype) == TRUE) {
        (void)json_object_object_add(user_data, RFPROP_DRIVE_HOTSPARETYPE, NULL);
        debug_log(DLOG_MASS, "%s %d delete hotsparetype.", __FUNCTION__, __LINE__);
    }
}


LOCAL gint32 deal_hotspare_firmstatus_conflict(json_object *user_data, json_object **o_msg_jso)
{
    json_object *json_oem = NULL;
    json_object *json_huawei = NULL;
    json_object *json_state = NULL;
    json_object *json_logical = NULL;
    json_object *json_hotsparetype = NULL;
    gchar *prop_path = NULL;

    if (json_object_object_get_ex(user_data, RFPROP_COMMON_OEM, &json_oem) != TRUE ||
        json_object_object_get_ex(json_oem, RFPROP_FWINV_HUAWEI, &json_huawei) != TRUE) {
        return VOS_OK;
    }
    
    if (json_object_object_get_ex(json_huawei, RFPROP_DRIVE_OEM_PHYSICALDISKSTATE, &json_state) != TRUE) {
        return VOS_OK;
    }

    prop_path = RFPROP_COMMON_OEM "/" RFPROP_FWINV_HUAWEI "/" RFPROP_DRIVE_OEM_PHYSICALDISKSTATE;
    if (json_object_object_get_ex(json_huawei, RFPROP_DRIVE_OEM_HOTFORLOGICAL, &json_logical) == TRUE) {
        gchar *hotspare_path =
            RFPROP_COMMON_OEM "/" RFPROP_FWINV_HUAWEI "/" RFPROP_DRIVE_OEM_HOTFORLOGICAL;
        (void)create_message_info(MSGID_PROPERTY_PD_NOT_SUPPORTSETSAMETIME, prop_path,
            o_msg_jso, hotspare_path, prop_path);
        del_firmware_status_if_exist(user_data);
        del_hotspare_drives_if_exist(user_data);
        del_hotspare_type_if_exist(user_data);
        return VOS_ERR;
    }
    if (json_object_object_get_ex(user_data, RFPROP_DRIVE_HOTSPARETYPE, &json_hotsparetype) == TRUE) {
        (void)create_message_info(MSGID_PROPERTY_PD_NOT_SUPPORTSETSAMETIME, prop_path,
            o_msg_jso, RFPROP_DRIVE_HOTSPARETYPE, prop_path);
        del_firmware_status_if_exist(user_data);
        del_hotspare_drives_if_exist(user_data);
        del_hotspare_type_if_exist(user_data);
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 set_system_storage_drive_oem_process(json_object *obj_huawei_json, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso)
{
    gint32 response_code = HTTP_BAD_REQUEST;
    json_object *obj_json = NULL;

    
    if (json_object_object_get_ex(obj_huawei_json, RFPROP_DRIVE_OEM_BOOT_ENABLE, &obj_json) == TRUE) {
        if (set_drive_oem_boot_enable(i_paras, *o_message_jso, obj_json) == HTTP_OK) {
            response_code = HTTP_OK;
        }
    }

    
    if (json_object_object_get_ex(obj_huawei_json, RFPROP_DRIVE_OEM_BOOT_PRIORITY, &obj_json) == TRUE) {
        if (set_drive_oem_boot_priority(i_paras, *o_message_jso, obj_json) == HTTP_OK) {
            response_code = HTTP_OK;
        }
    }

    
    if (json_object_object_get_ex(obj_huawei_json, RFPROP_DRIVE_OEM_PATROLSTATE, &obj_json) == TRUE) {
        if (set_drive_oem_patrol_state(i_paras, *o_message_jso, obj_json) == HTTP_OK) {
            response_code = HTTP_OK;
        }
    }
    return response_code;
}

LOCAL gint32 set_system_storage_drive_oem_property(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *obj_state_json = NULL;
    json_object *obj_logical_json = NULL;
    json_object *obj_huawei_json = NULL;
    
    gchar *prop_path = "Oem/Huawei/FirmwareStatus";
    
    json_object *obj_error_json = NULL;
    gint32 response_code = HTTP_BAD_REQUEST;

    
    if ((provider_paras_check(i_paras) != VOS_OK) || (o_message_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s %d NULL pointer!.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if ((i_paras->user_role_privilege & USERROLE_BASICSETTING) == 0) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, "Oem/Huawei", o_message_jso, "Oem/Huawei");
        return HTTP_FORBIDDEN;
    }

    
    if (i_paras->val_jso == NULL) {
        return HTTP_BAD_REQUEST;
    }

    if (json_object_object_get_ex(i_paras->val_jso, RFPROP_FWINV_HUAWEI, &obj_huawei_json) != TRUE) {
        del_hotspare_type_if_exist(i_paras->user_data);
        return HTTP_BAD_REQUEST;
    }

    
    *o_message_jso = json_object_new_array();

    
    if (deal_hotspare_firmstatus_conflict(i_paras->user_data, &obj_error_json) == VOS_OK) {
        if (json_object_object_get_ex(obj_huawei_json, RFPROP_DRIVE_OEM_PHYSICALDISKSTATE,
            &obj_state_json) == TRUE) {
            if (set_physical_state(i_paras, obj_state_json, prop_path, &obj_error_json) == HTTP_OK) {
                response_code = HTTP_OK;
            }
        } else if (json_object_object_get_ex(obj_huawei_json, RFPROP_DRIVE_OEM_HOTFORLOGICAL,
            &obj_logical_json) == TRUE) {
            if (set_spare_for_logical(i_paras, &obj_error_json) == HTTP_OK) {
                response_code = HTTP_OK;
            }
        }
    }

    
    if ((obj_error_json != NULL) && (json_object_array_add(*o_message_jso, obj_error_json) != 0)) {
        debug_log(DLOG_ERROR, "%s:json_object_array_add fail", __FUNCTION__);
        (void)json_object_put(obj_error_json);
    }

    
    if (set_system_storage_drive_oem_process(obj_huawei_json, i_paras, o_message_jso) == HTTP_OK) {
        response_code = HTTP_OK;
    }

    return response_code;
}


LOCAL gint32 check_spare_logical_drives(json_object *user_data, guint8 hot_spare_type, json_object **o_msg_jso)
{
    json_object *json_oem = NULL;
    json_object *json_huawei = NULL;
    json_object *json_logical = NULL;
    gchar *prop_path = NULL;

    if (json_object_object_get_ex(user_data, RFPROP_COMMON_OEM, &json_oem) != TRUE ||
        json_object_object_get_ex(json_oem, RFPROP_FWINV_HUAWEI, &json_huawei) != TRUE) {
        return VOS_OK;
    }

    
    if (hot_spare_type == PD_HOT_SPARE_NONE || hot_spare_type == PD_HOT_SPARE_GLOBAL) {
        if (json_object_object_get_ex(json_huawei, RFPROP_DRIVE_OEM_HOTFORLOGICAL, &json_logical) == TRUE) {
            prop_path = RFPROP_COMMON_OEM "/" RFPROP_FWINV_HUAWEI "/" RFPROP_DRIVE_OEM_HOTFORLOGICAL;
            (void)create_message_info(MSGID_PROPERTY_PD_NOT_SUPPORTSETSAMETIME, prop_path,
                o_msg_jso, RFPROP_DRIVE_HOTSPARETYPE, prop_path);
            return VOS_ERR;
        }
    } else {
        if (json_object_object_get_ex(json_huawei, RFPROP_DRIVE_OEM_HOTFORLOGICAL, &json_logical) != TRUE) {
            prop_path = RFPROP_COMMON_OEM "/" RFPROP_FWINV_HUAWEI "/" RFPROP_DRIVE_OEM_HOTFORLOGICAL;
            (void)create_message_info(MSGID_PROP_MISSING, prop_path, o_msg_jso, prop_path);
            return VOS_ERR;
        }
    }

    return VOS_OK;
}


LOCAL gint32 set_system_storage_drive_hotsparetype(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if ((provider_paras_check(i_paras) != VOS_OK) || (o_message_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s %d NULL pointer!.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if ((i_paras->user_role_privilege & USERROLE_BASICSETTING) == 0) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_DRIVE_HOTSPARETYPE, o_message_jso,
            RFPROP_DRIVE_HOTSPARETYPE);
        return HTTP_FORBIDDEN;
    }

    
    if (deal_hotspare_firmstatus_conflict(i_paras->user_data, o_message_jso) != VOS_OK) {
        return HTTP_BAD_REQUEST;
    }
    json_object *json_sparetype = i_paras->val_jso;

    
    if (json_sparetype == NULL) {
        return HTTP_BAD_REQUEST;
    }

    return set_hot_spare(i_paras, json_sparetype, o_message_jso);
}


LOCAL gint32 get_hot_spare_type(json_object *i_hotspare_jso, guint8 *o_hot_spare_type, json_object **o_msg_jso)
{
    const char *hotspare = NULL;

    if (i_hotspare_jso == NULL) {
        (void)create_message_info(MSGID_PROP_MISSING, RFPROP_DRIVE_HOTSPARETYPE, o_msg_jso, RFPROP_DRIVE_HOTSPARETYPE);
        return VOS_ERR;
    }

    hotspare = dal_json_object_get_str(i_hotspare_jso);
    if (hotspare == NULL) {
        (void)create_message_info(MSGID_PROP_MISSING, RFPROP_DRIVE_HOTSPARETYPE, o_msg_jso, RFPROP_DRIVE_HOTSPARETYPE);
        return VOS_ERR;
    }

    if (g_strcmp0(RF_HOTSPARETYPE_NONE, hotspare) == 0) {
        *o_hot_spare_type = PD_HOT_SPARE_NONE;
    } else if (g_strcmp0(RF_HOTSPARETYPE_GLOBAL, hotspare) == 0) {
        *o_hot_spare_type = PD_HOT_SPARE_GLOBAL;
    } else if (g_strcmp0(RF_HOTSPARETYPE_DEDICATED, hotspare) == 0) {
        *o_hot_spare_type = PD_HOT_SPARE_DEDICATED;
    } else if (g_strcmp0(RF_HOTSPARETYPE_AUTO_REPLACE, hotspare) == 0) {
        *o_hot_spare_type = PD_HOT_SPARE_AUTO_REPLACE;
    } else {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_DRIVE_HOTSPARETYPE, o_msg_jso, hotspare,
            RFPROP_DRIVE_HOTSPARETYPE);

        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 set_hot_spare(PROVIDER_PARAS_S *i_paras, json_object *i_hotspare_jso, json_object **o_msg_jso)
{
    gint32 ret;
    guint16 id[SML_MAX_LOGIC_DRIVES_BY_HOT_SPARE] = { 0 };
    gint32 id_cnt = 0;
    guint8 hot_spare_type = PD_HOT_SPARE_UNKNOWN;

    if (get_hot_spare_type(i_hotspare_jso, &hot_spare_type, o_msg_jso) != VOS_OK) {
        del_hotspare_drives_if_exist(i_paras->user_data);
        del_hotspare_type_if_exist(i_paras->user_data);
        return HTTP_BAD_REQUEST;
    }

    if (hot_spare_type == PD_HOT_SPARE_DEDICATED) {
        id_cnt = SML_MAX_LOGIC_DRIVES_BY_HOT_SPARE;
    } else if (hot_spare_type == PD_HOT_SPARE_AUTO_REPLACE) {
        
        id_cnt = 1;
    }

    
    ret = check_spare_logical_drives(i_paras->user_data, hot_spare_type, o_msg_jso);
    if (ret != VOS_OK) {
        del_hotspare_drives_if_exist(i_paras->user_data);
        del_hotspare_type_if_exist(i_paras->user_data);
        return HTTP_BAD_REQUEST;
    }

    
    if (id_cnt != 0) {
        json_object *prop_jso = NULL;
        if (get_spare_drives_obj(i_paras->user_data, &prop_jso, o_msg_jso) != VOS_OK) {
            del_hotspare_drives_if_exist(i_paras->user_data);
            del_hotspare_type_if_exist(i_paras->user_data);
            return HTTP_BAD_REQUEST;
        }

        if (get_spare_logical_id(i_paras->obj_handle, prop_jso, id, &id_cnt, o_msg_jso) != VOS_OK) {
            del_hotspare_drives_if_exist(i_paras->user_data);
            del_hotspare_type_if_exist(i_paras->user_data);
            return HTTP_BAD_REQUEST;
        }

        
        del_hotspare_drives_if_exist(i_paras->user_data);
        del_hotspare_type_if_exist(i_paras->user_data);
    } else {
        id_cnt = 1;
        id[0] = INVALID_DATA_WORD;
    }

    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_byte(hot_spare_type));
    input_list = g_slist_append(input_list,
        (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_UINT16, id, id_cnt, sizeof(guint16)));

    ret =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->obj_handle,
        CLASS_HDD_NAME, METHOD_HDD_SET_HOT_SPARE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);
    return redfish_drive_set_hotspare_result(ret, o_msg_jso);
}

LOCAL gint32 redfish_drive_indicatorled_result(gint32 ret, json_object **o_message_jso)
{
    switch (ret) {
        case RET_OK:
            return HTTP_OK;

            // OS 下电,非RAID管理的物理盘
        case SM_CODE_NOT_SUPPORT_IN_PRESENT_STATE:
            // NVME硬盘
        case ERRCODE_METHOD_NOT_FOUND:
            (void)create_message_info(MSGID_PROPERTY_STATUS_UNSUPPORT_MODIF, RFPROP_DRIVE_INDICATORLED,
                o_message_jso, RFPROP_DRIVE_INDICATORLED);
            return HTTP_BAD_REQUEST;

            // 物理盘正在定位或正在重构
        case SM_CODE_BUSY:
            (void)create_message_info(MSGID_PROPERTY_PD_LOCATE_OR_REBUILDING, RFPROP_DRIVE_INDICATORLED,
                o_message_jso, RFPROP_DRIVE_INDICATORLED);
            return HTTP_BAD_REQUEST;

        case SML_ERR_CTRL_STATUS_INVALID:
        case SML_ERR_I2C_READ_WRITE_FAILED:
            (void)create_message_info(MSGID_OPERATION_FAILED, RFPROP_DRIVE_INDICATORLED, o_message_jso,
                RFPROP_DRIVE_INDICATORLED);
            return HTTP_BAD_REQUEST;
        case SM_CODE_OPERATION_IN_PROGRESS:
            (void)create_message_info(VMM_MESSAGE_OPERATIONINPROCESS, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 set_system_storage_drive_indicatorled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    
    if ((VOS_OK != provider_paras_check(i_paras)) || (NULL == o_message_jso) || (NULL == i_paras->val_jso)) {
        debug_log(DLOG_ERROR, "%s %d NULL Pointer!.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail((i_paras->user_role_privilege) & (USERROLE_BASICSETTING), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_DRIVE_INDICATORLED, o_message_jso,
        RFPROP_DRIVE_INDICATORLED));

    const gchar *set_indicatorled = dal_json_object_get_str(i_paras->val_jso);
    if (0 == g_strcmp0(RF_INDICATORLED_OFF, set_indicatorled)) {
        input_list = g_slist_append(input_list, g_variant_new_byte(PD_OPERATION_STOP_LOCATE));
    } else if (0 == g_strcmp0(RF_INDICATORLED_BLINKING, set_indicatorled)) {
        input_list = g_slist_append(input_list, g_variant_new_byte(PD_OPERATION_LOCATE));
    } else {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_DRIVE_INDICATORLED, o_message_jso, set_indicatorled,
            RFPROP_DRIVE_INDICATORLED);
        return HTTP_BAD_REQUEST;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(DRIVE_SET_INDICATOR_TIME));
    gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_HDD_NAME, METHOD_HDD_SET_LOCATE_STATE, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, &output_list);

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    return redfish_drive_indicatorled_result(ret, o_message_jso);
}



LOCAL gint32 action_crypto_erase(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
#define MSGARG_THE_RAID_CONTROLLER "The RAID controller"
#define MSGARG_THE_DIVE "The drive"
    gchar class_name[MAX_NAME_SIZE] = {0};
    const gchar *message_id = NULL;
    // URL示例:/redfish/v1/Chassis/1/Drives/HDDPlaneDisk0/Actions/Oem/Huawei/Drive.CryptoErase
    if (NULL == i_paras || NULL == o_message_jso) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    gint32 ret = dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);
    return_val_do_info_if_fail(DFL_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s: dfl_get_class_name fail, objct_name = %s", __FUNCTION__,
        dfl_get_object_name(i_paras->obj_handle));
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, o_message_jso, RFACTION_DRIVE_CRYPTO_ERASE));

    // 目前只认为Hdd类可能会使用该action PcieDisk和SD卡类返回action不支持消息
    return_val_do_info_if_fail(0 == g_strcmp0(class_name, CLASS_HDD_NAME), HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s: class name is not hdd", __FUNCTION__);
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, o_message_jso, RFACTION_DRIVE_CRYPTO_ERASE));

    ret =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->obj_handle,
        CLASS_HDD_NAME, METHOD_HDD_CRYPTO_ERASE, AUTH_ENABLE, i_paras->user_role_privilege, NULL, NULL);

    do_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: call %s.%s return 0x%0x", __FUNCTION__,
        dfl_get_object_name(i_paras->obj_handle), METHOD_HDD_CRYPTO_ERASE, ret));

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return HTTP_FORBIDDEN;

        // 物理盘没有在控制器下
        case SM_CODE_PD_NOT_IDENTIFIED:
        // 控制器没初始化好
        case SM_CODE_NOT_SUPPORT_IN_PRESENT_STATE:
        // 控制器状态无效
        case SML_ERR_CTRL_STATUS_INVALID:
        // 控制器初始化未完成
        case SML_ERR_CTRL_INIT_NOT_COMPLETED:
            (void)create_message_info(MSGID_OPERATION_FAILED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        case SM_CODE_OPERATION_IN_PROGRESS:
            (void)create_message_info(VMM_MESSAGE_OPERATIONINPROCESS, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        // 物理盘不支持
        case SML_ERR_PD_OPERATION_NOT_SUPPORT:
            message_id = MSGARG_THE_DIVE;
            break;
        // 控制器不支持操作
        case SML_ERR_CTRL_OPERATION_NOT_SUPPORT:
            message_id = MSGARG_THE_RAID_CONTROLLER;
            break;

        // 其他异常错误
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)create_message_info(MSGID_CRYTO_ERASE_NOT_SUPPORTED, NULL, o_message_jso, message_id);
    return HTTP_BAD_REQUEST;
}


gint32 chassis_drive_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    guchar board_type = 0;
    gboolean ret_result;
    OBJ_HANDLE chassis_handle;
    
    ret_result = redfish_check_chassis_uri_valid(i_paras->uri, &chassis_handle);
    if (!ret_result) {
        return HTTP_NOT_FOUND;
    }

    
    gint32 ret = check_enclosure_component_type(chassis_handle, FALSE);
    
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);

    
    
    ret = redfish_get_x86_enable_type(&board_type);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    return_val_if_fail(DISABLE != board_type, HTTP_NOT_FOUND);
    

    if (ret == check_drive_uri_effective_get_handle(i_paras->uri, &(i_paras->obj_handle))) {
        *prop_provider = g_chassis_drive_provider;
        *count = sizeof(g_chassis_drive_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    } else if (ret == check_sdcard_uri_effective_get_handle(i_paras->member_id, &(i_paras->obj_handle))) {
        *prop_provider = g_system_storage_sdcard_provider;
        *count = sizeof(g_system_storage_sdcard_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}


LOCAL const gchar* g_chassis_drive_action_array[] = {
    RFACTION_DRIVE_CRYPTO_ERASE
};

LOCAL PROPERTY_PROVIDER_S g_chassis_drive_info_provider[] = {
    {
        RFPROP_ODATA_ID,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_chassis_drive_actioninfo_odataid,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    }
};


gint32 chassis_drive_etag_del_property(json_object *object)
{
    SPECIAL_PROP_S value[] = {
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_DRIVE_OEM_TEMPERATURE, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_DRIVE_OEM_POWERUP, NULL, NULL}},
        {0, {NULL, NULL, NULL, NULL, NULL}}
    };
    return rsc_del_none_etag_affected_property(object, value, G_N_ELEMENTS(value));
}


LOCAL gint32 get_chassis_drive_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar hdd_url[MAX_URL_LEN] = {0};
    gchar actioninfo_url[MAX_URL_LEN] = {0};

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    gint32 ret = redfish_get_drive_uri(i_paras->obj_handle, hdd_url, sizeof(hdd_url));
    return_val_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR);

    ret = rf_update_rsc_actioninfo_uri(g_chassis_drive_action_array, G_N_ELEMENTS(g_chassis_drive_action_array),
        i_paras->uri, hdd_url, actioninfo_url, sizeof(actioninfo_url));
    return_val_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string(actioninfo_url);

    return HTTP_OK;
}


LOCAL gint32 rf_check_drive_actioninfo_url(gchar *url, OBJ_HANDLE *o_obj_handle)
{
    int iRet = -1;
    GSList *hdd_list = NULL;
    GSList *node = NULL;
    gchar hdd_url[MAX_URL_LEN] = {0};
    gchar actioninfo_url[MAX_URL_LEN] = {0};
    guint32 i = 0;
    guint32 len;
    gint32 result = VOS_ERR;
    gchar *action_name = NULL;
    gchar full_action_name[PROP_VAL_MAX_LENGTH] = {0};

    return_val_if_expr(NULL == url, result);

    gint32 ret = dfl_get_object_list(CLASS_HDD_NAME, &hdd_list);
    return_val_if_fail(DFL_OK == ret, VOS_ERR);

    len = G_N_ELEMENTS(g_chassis_drive_action_array);
    for (node = hdd_list; node; node = g_slist_next(node)) {
        (void)memset_s(hdd_url, sizeof(hdd_url), 0, sizeof(hdd_url));
        (void)memset_s(actioninfo_url, sizeof(actioninfo_url), 0, sizeof(actioninfo_url));
        ret = redfish_get_drive_uri((OBJ_HANDLE)node->data, hdd_url, sizeof(hdd_url));
        continue_if_fail(VOS_OK == ret);

        for (i = 0; i < len; i++) {
            action_name = NULL;
            (void)memset_s(full_action_name, sizeof(full_action_name), 0, sizeof(full_action_name));

            // strstr第一个参数不是const指针
            (void)strncpy_s(full_action_name, sizeof(full_action_name), g_chassis_drive_action_array[i],
                sizeof(full_action_name) - 1);
            action_name = strstr(full_action_name, DOT_STR);
            continue_if_fail(action_name);

            iRet = snprintf_s(actioninfo_url, sizeof(actioninfo_url), sizeof(actioninfo_url) - 1, "%s/%sActionInfo",
                hdd_url, action_name + 1);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            break_do_info_if_expr(0 == g_ascii_strcasecmp(actioninfo_url, url), (result = VOS_OK));
        }
        if (VOS_OK == result) {
            do_if_fail(NULL == o_obj_handle, (*o_obj_handle = (OBJ_HANDLE)node->data));
            break;
        }
    }
    g_slist_free(hdd_list);

    return result;
}


gint32 chassis_drive_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    
    OBJ_HANDLE chassis_comp_handle = 0;
    

    return_val_if_expr(NULL == i_paras || NULL == prop_provider || NULL == count, HTTP_INTERNAL_SERVER_ERROR);

    
    if (FALSE == redfish_check_chassis_uri_valid(i_paras->uri, &chassis_comp_handle) || (0 == chassis_comp_handle)) {
        return HTTP_NOT_FOUND;
    }
    return_val_if_fail(VOS_OK == check_enclosure_component_type(chassis_comp_handle, FALSE), HTTP_NOT_FOUND);
    

    gint32 ret = rf_check_drive_actioninfo_url(i_paras->uri, &i_paras->obj_handle);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);

    *prop_provider = g_chassis_drive_info_provider;
    *count = sizeof(g_chassis_drive_info_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
