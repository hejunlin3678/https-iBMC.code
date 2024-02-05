
#include "ctype.h"
#include "redfish_provider.h"
#include "redfish_util.h"
#include "redfish_provider_system.h"
#include "redfish_provider_storage_controller.h"

#define BOOT_DEVICE_NUM    2

LOCAL gint32 get_storage_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_storage_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_storage_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_storage_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_storage_Controllers(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_storage_Controllers(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_storage_driver_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_storage_drivers(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 restore_storage_controller_default_setting(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 import_ctrl_foreign_config(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 clear_ctrl_foreign_config(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_storage_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);


LOCAL gint32 get_volumes_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);


LOCAL gint32 get_system_sdcards_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_sdcards_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_sdcards_controllers(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_sdcards_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_sdcards_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_sdcards_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_sdcards_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_sdcards_volumes(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_sdcards_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL void get_storage_fault_details(gushort health_status_code, json_object *status_jso);
LOCAL void get_storage_capacitance_fault_details(OBJ_HANDLE bbu_handle, json_object **result_obj);


LOCAL gint32 get_storage_controller_memory_size(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso);


gint32 g_drives_count = 0;
LOCAL pthread_mutex_t g_dirves_count_lock = PTHREAD_MUTEX_INITIALIZER;


LOCAL PROPERTY_PROVIDER_S g_storage_raidcontroller_provider[] = {
    {RFPROP_COMMON_ID,                 MAP_PROPERTY_NULL,          MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_storage_id,                   NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID,                  MAP_PROPERTY_NULL,          MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_storage_odata_id,             NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_NAME,               MAP_PROPERTY_NULL,          MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_storage_name,                   NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_STORAGE_CONTROLLERS,       CLASS_RAID_CONTROLLER_NAME, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_storage_Controllers,          set_storage_Controllers, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_STORAGE_DRIVERS,           CLASS_HDD_NAME,             MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_storage_drivers,              NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_STORAGE_DRIVER_COUNT,      CLASS_HDD_NAME,             MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_storage_driver_count,         NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT,             MAP_PROPERTY_NULL,          MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_storage_odata_context,        NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACTIONS,                   MAP_PROPERTY_NULL,          MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_storage_actions,              NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_STORAGE_CONTROLLERS_RESTORE, CLASS_RAID_CONTROLLER_NAME, MAP_PROPERTY_NULL, USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, NULL, NULL, restore_storage_controller_default_setting,        ETAG_FLAG_ENABLE},
    {RFPROP_STORAGE_CONTROLLERS_IMPORT_FOREIGN_CONFIG, CLASS_RAID_CONTROLLER_NAME, MAP_PROPERTY_NULL,
        USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, NULL, NULL, import_ctrl_foreign_config, ETAG_FLAG_ENABLE},
    {RFPROP_STORAGE_CONTROLLERS_CLEAR_FOREIGN_CONFIG,  CLASS_RAID_CONTROLLER_NAME, MAP_PROPERTY_NULL, USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, NULL, NULL, clear_ctrl_foreign_config,            ETAG_FLAG_ENABLE},
    {RFPROP_STORAGE_VOLUMES_ODATA_ID,                  MAP_PROPERTY_NULL,          MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_volumes_odata_id,             NULL,                    NULL, ETAG_FLAG_ENABLE}
};


LOCAL PROPERTY_PROVIDER_S g_storage_sdcardcontroller_provider[] = {
    {RFPROP_COMMON_ID,                 MAP_PROPERTY_NULL,          MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcards_id,                   NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_NAME,               MAP_PROPERTY_NULL,          MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcards_name,                   NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT,             MAP_PROPERTY_NULL,          MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcards_odata_context,        NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID,                  MAP_PROPERTY_NULL,          MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcards_odata_id,             NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_STORAGE_CONTROLLERS,       CLASS_RAID_CHIP_NAME,       MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcards_controllers,          NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_STORAGE_DRIVER_COUNT,      CLASS_MASS_STORAGE_NAME,    MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcards_count,         NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_STORAGE_DRIVERS,           CLASS_COMPONENT,            MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcards_members,              NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACTIONS,                   MAP_PROPERTY_NULL,          MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcards_actions,              NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_STORAGE_VOLUMES_ODATA_ID,  MAP_PROPERTY_NULL,          MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_sdcards_volumes,             NULL,                    NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_system_sdcards_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    *o_result_jso = json_object_new_string(STORAGE_SDCONTROLLER_MEMBERID);
    if (NULL == *o_result_jso) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}

LOCAL gint32 get_system_sdcards_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    *o_result_jso = json_object_new_string(STORAGE_SDCONTROLLER_MEMBERID);
    if (NULL == *o_result_jso) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}

LOCAL gint32 get_system_sdcards_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar odata_context[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    gint32 ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: get board slot fail.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, SYSTEMS_STROGE_METADATA, slot_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s %d  snprintf_s fail.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)odata_context);
    if (*o_result_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_system_sdcards_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar uri[PSLOT_URI_LEN] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    gint32 ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "%s:%d get board slot fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, SYSTEMS_STORAGE_SDCARDS, slot);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s %d  snprintf_s fail.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)uri);
    if (*o_result_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL gint32 get_storage_sdcontroller_string_property(OBJ_HANDLE obj_handle, gchar *property_name,
    json_object **o_result_jso)
{
    gchar property_string[STORAGE_CTRL_INFO_LENGTH] = {0};

    if ((obj_handle == 0) || (property_name == NULL)) {
        debug_log(DLOG_MASS, "%s, %d: Input parameter Invalid ", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    gint32 ret = dal_get_property_value_string(obj_handle, property_name, property_string, sizeof(property_string));
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s, %d: get %s property fail.\n", __FUNCTION__, __LINE__, property_name);
        return RET_ERR;
    }

    
    if (strlen(property_string) == 0 || g_strcmp0(property_string, STRING_VALUE_IS_NA) == 0 ||
        g_strcmp0(property_string, STRING_VALUE_IS_UNKNOWN) == 0) {
        return RET_ERR;
    } else {
        *o_result_jso = json_object_new_string((const gchar *)property_string);
        if (*o_result_jso == NULL) {
            debug_log(DLOG_MASS, "%s, %d: new string fail.\n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }
    }

    return RET_OK;
}


LOCAL gint32 get_storage_sdcontroller_status(OBJ_HANDLE obj_handle, gchar *property_name, json_object **o_result_jso)
{
    guint8 controller_health = 0;

    if (obj_handle == 0 || property_name == NULL) {
        debug_log(DLOG_MASS, "%s, %d: Input parameter Invalid.", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    gint32 ret = dal_get_property_value_byte(obj_handle, property_name, &controller_health);
    
    if (ret != RET_OK) {
        controller_health = DRIVE_INVALID_VALUE;
        debug_log(DLOG_MASS, "%s, %d: get Health value fail.", __FUNCTION__, __LINE__);
    }

    (void)get_resource_status_property(&controller_health, NULL, STRING_CONSET_VALUE_ENABLE, o_result_jso, FALSE);
    return RET_OK;
}


LOCAL gint32 get_system_sdcards_controllers(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *obj_sdcontroller = NULL;
    json_object *obj_string = NULL;
    const gchar *sd_description = "SD Controller";
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gint32 member_id = 0;
    gchar prop_info[STORAGE_CTRL_INFO_LENGTH] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    gint32 ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s, %d: function  get board slot_id.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_array();
    if (NULL == *o_result_jso) {
        debug_log(DLOG_MASS, "%s, %d: get sdcard controllers json object fail.\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    obj_sdcontroller = json_object_new_object();
    if (obj_sdcontroller == NULL) {
        debug_log(DLOG_MASS, "%s, %d: new object fail.\n", __FUNCTION__, __LINE__);
        return HTTP_OK;
    }

    ret = snprintf_s(prop_info, STORAGE_CTRL_INFO_LENGTH, STORAGE_CTRL_INFO_LENGTH - 1,
        STORAGE_SDCARD_CONTROLLERS_ODATA_ID, slot, member_id);
    if (ret <= 0) {
        debug_log(DLOG_MASS, "%s %d : snprintf_s fail.", __FUNCTION__, __LINE__);
    }
    obj_string = json_object_new_string((const gchar *)prop_info);
    if (obj_string == NULL) {
        debug_log(DLOG_MASS, "%s, %d: new string fail.\n", __FUNCTION__, __LINE__);
    }
    (void)json_object_object_add(obj_sdcontroller, RFPROP_ODATA_ID, obj_string);
    obj_string = NULL;

    
    (void)memset_s(prop_info, STORAGE_CTRL_INFO_LENGTH, 0, STORAGE_CTRL_INFO_LENGTH);

    ret = snprintf_s(prop_info, STORAGE_CTRL_INFO_LENGTH, STORAGE_CTRL_INFO_LENGTH - 1, "%d", member_id);
    if (ret <= 0) {
        debug_log(DLOG_MASS, "%s %d : snprintf_s fail.", __FUNCTION__, __LINE__);
    }

    obj_string = json_object_new_string((const gchar *)prop_info);
    if (obj_string == NULL) {
        debug_log(DLOG_MASS, "%s, %d: new string fail.\n", __FUNCTION__, __LINE__);
    }
    (void)json_object_object_add(obj_sdcontroller, RFPROP_STORAGE_CONTROLLER_ID, obj_string);
    obj_string = NULL;

    
    obj_string = json_object_new_string(sd_description);
    if (obj_string == NULL) {
        debug_log(DLOG_MASS, "%s, %d: new string fail.\n", __FUNCTION__, __LINE__);
    }
    (void)json_object_object_add(obj_sdcontroller, RFPROP_COMMON_NAME, obj_string);
    obj_string = NULL;

    
    obj_string = json_object_new_string(sd_description);
    if (obj_string == NULL) {
        debug_log(DLOG_MASS, "%s, %d: new string fail.\n", __FUNCTION__, __LINE__);
    }
    (void)json_object_object_add(obj_sdcontroller, RFPROP_STORAGE_CONTROLLER_DESCRIPTION, obj_string);
    obj_string = NULL;

    
    ret = get_storage_sdcontroller_string_property(i_paras->obj_handle, PROPETRY_NAME_NUM, &obj_string);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s, %d: get sdcontroller Manufacturer fail.\n", __FUNCTION__, __LINE__);
    }
    (void)json_object_object_add(obj_sdcontroller, RFPROP_SDCONTROLLER_MANUFACTURER, obj_string);
    obj_string = NULL;

    
    ret = get_storage_sdcontroller_status(i_paras->obj_handle, PROPETRY_SDRAID_HEALTH, &obj_string);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s, %d: get sdcontroller status fail.\n", __FUNCTION__, __LINE__);
    }
    (void)json_object_object_add(obj_sdcontroller, RFPROP_STORAGE_CONTROLLER_STATUS, obj_string);
    obj_string = NULL;

    
    ret = get_storage_sdcontroller_string_property(i_paras->obj_handle, PROPETRY_FX3S_VERSION, &obj_string);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s, %d: get sdcontroller FirmwareVersion fail.\n", __FUNCTION__, __LINE__);
    }
    (void)json_object_object_add(obj_sdcontroller, RFPROP_STORAGE_CONTROLLER_FW_VERSION, obj_string);

    ret = json_object_array_add(*o_result_jso, obj_sdcontroller);
    if (ret != 0) {
        json_object_put(obj_sdcontroller);
    }
    return HTTP_OK;
}


LOCAL gint32 get_system_sdcards_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 count = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guchar sdcard_presence = 0;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    gint32 ret = dfl_get_object_list(g_storage_sdcardcontroller_provider[i_paras->index].pme_class_name, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_MASS, "%s: get MassStorage list fail", __FUNCTION__);
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPETRY_PRESENT, &sdcard_presence);
        if (ret != RET_OK || sdcard_presence == 0) {
            continue;
        }

        count++;
    }

    
    *o_result_jso = json_object_new_int(count);
    
    g_slist_free(obj_list);
    
    return HTTP_OK;
}

LOCAL gint32 get_system_sdcards_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    gint32 ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s:%d get board slot fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_array();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_MASS, "%s, %d: new object array fail.\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)chassis_get_sdcard_odata_id(*o_result_jso);
    return HTTP_OK;
}


LOCAL gint32 get_system_sdcards_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 get_system_sdcards_volumes(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 get_storage_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guint8 controller_id = 0;
    gchar str_name[MAX_RSC_NAME_LEN] = {0};

    
    if (o_message_jso == NULL || o_result_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }
    gint32 ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_RAID_CONTROLLER_ID, &controller_id);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s, %d: get storage controller id fail.\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(str_name, sizeof(str_name), sizeof(str_name) - 1, STORAGE_CONTROLLERS_MEMBERID, controller_id);
    if (0 >= ret) {
        debug_log(DLOG_ERROR, "%s %d : snprintf_s fail.", __FUNCTION__, __LINE__);
    }

    *o_result_jso = json_object_new_string((const gchar *)str_name);
    if (NULL == *o_result_jso) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


LOCAL gint32 get_storage_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guint8 controller_id = 0;
    gchar str_name[MAX_RSC_NAME_LEN] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    gint32 ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_RAID_CONTROLLER_ID, &controller_id);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s, %d: get storage controller id fail.\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(str_name, sizeof(str_name), sizeof(str_name) - 1, STORAGE_CONTROLLERS_MEMBERID, controller_id);
    if (0 >= ret) {
        debug_log(DLOG_ERROR, "%s %d : snprintf_s fail.", __FUNCTION__, __LINE__);
    }

    *o_result_jso = json_object_new_string((const gchar *)str_name);
    if (NULL == *o_result_jso) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


LOCAL gint32 get_storage_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gchar uri[PSLOT_URI_LEN] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    guint8 controller_id = 0;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    gint32 ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:%d fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_RAID_CONTROLLER_ID, &controller_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:%d fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    int iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, SYSTEMS_STORAGE, slot, controller_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }

    *o_result_jso = json_object_new_string((const gchar *)uri);
    
    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_storage_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar odata_context[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || RET_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    gint32 ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    int iRet =
        snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, SYSTEMS_STROGE_METADATA, slot_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }

    *o_result_jso = json_object_new_string((const gchar *)odata_context);
    if (*o_result_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_storage_controller_status(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gushort health_status_code = 0;
    json_object *status_jso = NULL;

    if (controller_handle == 0) {
        debug_log(DLOG_ERROR, "%s, %d: Invalid controller handle", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    gint32 ret = dal_get_property_value_uint16(controller_handle, PROPERTY_RAID_CONTROLLER_HEALTH_STATUS_CODE,
        &health_status_code);
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s: get health_status_code error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    status_jso = json_object_new_object();

    
    (void)json_object_object_add(status_jso, RF_STATUS_STATE, json_object_new_string(RF_STATE_ENABLED));

    
    (void)get_storage_fault_details(health_status_code, status_jso);

    
    if (health_status_code >= STORAGE_INFO_INVALID_BYTE) {
        // NULL表示未获取到状态
        (void)json_object_object_add(status_jso, RF_STATUS_HEALTH, NULL);
    } else if (health_status_code == LOGENTRY_STATUS_INFORMATIONAL_CODE) {
        // OK表示状态为正常
        (void)json_object_object_add(status_jso, RF_STATUS_HEALTH, json_object_new_string(LOGENTRY_SEVERITY_OK));
    } else {
        // Warning表示状态为异常
        (void)json_object_object_add(status_jso, RF_STATUS_HEALTH, json_object_new_string(LOGENTRY_SEVERITY_WARNING));
    }

    *o_result_jso = status_jso;
    return HTTP_OK;
}


LOCAL gint32 get_storage_controller_cachesummary(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *cachesummary_jso = NULL;
    json_object *memory_size = NULL;

    if (0 >= controller_handle) {
        debug_log(DLOG_ERROR, "%s, %d: Invalid controller handle", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    cachesummary_jso = json_object_new_object();
    if (cachesummary_jso == NULL) {
        debug_log(DLOG_ERROR, "%s,%d:new json object pointer is NULL", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    gint32 ret = get_storage_controller_memory_size(controller_handle, o_message_jso, &memory_size);
    if (RET_OK != ret) {
        debug_log(DLOG_MASS, "%s, %d: get controller memory size error", __FUNCTION__, __LINE__);
    }
    json_object_object_add(cachesummary_jso, RFPROP_STORAGE_CONTROLLERS_TOTAL_CACHE_SIZE, memory_size);

    *o_result_jso = cachesummary_jso;
    return RET_OK;
}


LOCAL gint32 get_storage_controller_speedgbps(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 controller_interface = 0xff;
    double controller_speedgbps;

    if (controller_handle == 0) {
        debug_log(DLOG_ERROR, "%s, %d: Invalid controller handle", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret = dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_DEVICE_INTERFACE,
        &controller_interface);
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get controller_speedgbps = %d.\n", __FUNCTION__, __LINE__, controller_interface);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    switch (controller_interface) {
        case CTRL_DEVICE_INTERFACE_SAS_3G: // SAS_3G
            controller_speedgbps = 3;
            break;

        case CTRL_DEVICE_INTERFACE_SATA_1_5G: // SATA_1_5G
            controller_speedgbps = 1.5;
            break;

        case CTRL_DEVICE_INTERFACE_SATA_3G: // SATA_3G
            controller_speedgbps = 3;
            break;

        case CTRL_DEVICE_INTERFACE_SAS_6G: // SAS_6G
            controller_speedgbps = 6;
            break;

        case CTRL_DEVICE_INTERFACE_SAS_12G: // SAS_12G
            controller_speedgbps = 12;
            break;

        case CTRL_DEVICE_INTERFACE_SAS_32G:
            controller_speedgbps = 32; // SAS_32G
            break;

        case CTRL_DEVICE_INTERFACE_SAS_64G:
            controller_speedgbps = 64; // SAS_64G
            break;

        case CTRL_DEVICE_INTERFACE_SATA_32G:
            controller_speedgbps = 32; // SATA_32G
            break;

        case CTRL_DEVICE_INTERFACE_SATA_64G:
            controller_speedgbps = 64; // SATA_64G
            break;

        default: // 0代表SPI，还有other不能识别的接口,根据SE意见redfish中显示null
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = ex_json_object_new_double(controller_speedgbps, "%.2f");
    return HTTP_OK;
}


LOCAL gint32 get_storage_controller_fwversion(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar fw_version[STORAGE_CTRL_INFO_LENGTH] = {0};

    if (controller_handle == 0) {
        debug_log(DLOG_ERROR, "%s, %d: Invalid controller handle", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret = dal_get_property_value_string(controller_handle, PROPERTY_RAID_CONTROLLER_FW_VERSION, fw_version,
        sizeof(fw_version));
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get FirmwareVersion fail.\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (0 == g_strcmp0(fw_version, "N/A")) {
        return HTTP_INTERNAL_SERVER_ERROR;
    } else {
        *o_result_jso = json_object_new_string((const gchar *)fw_version);
    }

    return HTTP_OK;
}

LOCAL gint32 get_storage_controller_name(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar  prop_info[STORAGE_CTRL_INFO_LENGTH] = {0};
    gchar controller_name[STORAGE_CTRL_INFO_LENGTH] = {0};

    if (controller_handle == 0) {
        debug_log(DLOG_ERROR, "%s, %d: Invalid controller handle", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret = dal_get_property_value_string(controller_handle, PROPERTY_RAID_CONTROLLER_COMPONENT_NAME, prop_info,
        sizeof(prop_info));
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get Raid Controller name fail.\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (RET_OK != check_string_val_effective((const gchar *)prop_info)) {
        return RET_ERR;
    }

    ret = snprintf_s(controller_name, sizeof(controller_name), sizeof(controller_name) - 1, "%s %s", prop_info,
        RFPROP_STORAGE_CONTROLLER_CONTROLLER);
    if (0 >= ret) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    *o_result_jso = json_object_new_string((const gchar *)controller_name);

    return HTTP_OK;
}

LOCAL gint32 get_storage_controller_description(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    *o_result_jso = json_object_new_string(STORAGE_CONTROLLER_DESCRIPTION_VALUE);

    return HTTP_OK;
}

LOCAL gint32 get_storage_controller_device_protocol(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 i = 0;
    gchar  prop_info[STORAGE_CTRL_INFO_LENGTH] = {0};
    json_object *json_prop = NULL;

    *o_result_jso = json_object_new_array();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: json object new array fail.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    gint32 ret = dal_get_property_value_string(controller_handle, PROPERTY_RAID_CONTROLLER_DEVICE_INTERFACE_STR,
        prop_info, sizeof(prop_info));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: get Raid Controller device protocol fail.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }
    if (check_string_val_effective((const gchar *)prop_info) != RET_OK) {
        return RET_ERR;
    }

    while (prop_info[i] != '\0') {
        if (prop_info[i] == ' ') {
            prop_info[i] = '\0';
            break;
        }
        i++;
    }

    json_prop = json_object_new_string((const gchar *)prop_info);
    if (json_prop == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: get Raid Controller device protocol fail.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    ret = json_object_array_add(*o_result_jso, json_prop);
    if (ret != RET_OK) {
        json_object_put(json_prop);
    }

    return RET_OK;
}

LOCAL gint32 get_storage_controller_sasaddress(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar  prop_info[STORAGE_CTRL_INFO_LENGTH] = {0};

    gint32 ret = dal_get_property_value_string(controller_handle, PROPERTY_RAID_CONTROLLER_SAS_ADDR, prop_info,
        sizeof(prop_info));
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get Raid Controller SASAddress fail.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }
    if (RET_OK != check_string_val_effective((const gchar *)prop_info)) {
        return RET_ERR;
    }

    *o_result_jso = json_object_new_string((const gchar *)prop_info);

    return RET_OK;
}


LOCAL gint32 get_storage_controller_no_battery_write_cache(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 no_battery_write_cache;

    ret = dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_NO_BATTERY_WRITE_CACHE,
        &no_battery_write_cache);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get Raid Controller no battery write Cache fail", __FUNCTION__);
        return RET_ERR;
    }
    if (no_battery_write_cache != ENABLE && no_battery_write_cache != DISABLE) {
        debug_log(DLOG_MASS, "%s, get Raid Controller no battery write Cache value invalid", __FUNCTION__);
        return RET_ERR;
    }
    *o_result_jso = json_object_new_boolean(no_battery_write_cache);
    return RET_OK;
}


LOCAL gint32 get_storage_controller_spare_activation_mode(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 spare_activation_mode;

    ret = dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_SPARE_ACTIVATION_MODE,
        &spare_activation_mode);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get Raid Controller spare activation mode fail", __FUNCTION__);
        return RET_ERR;
    }
    if (spare_activation_mode == 0) { // SpareActivationMode为failure
        *o_result_jso = json_object_new_string(SPARE_ACTIVATION_MODE_FAILURE_VALUE);
    } else if (spare_activation_mode == 1) { // SpareActivationMode为Predictive
        *o_result_jso = json_object_new_string(SPARE_ACTIVATION_MODE_PREDICTIVE_VALUE);
    } else {
        debug_log(DLOG_MASS, "%s, get Raid Controller spare activation mode value invalid", __FUNCTION__);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 get_storage_controller_read_cache_percent(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 read_cache_percent;

    ret = dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_READ_CACHE_PERCENT,
        &read_cache_percent);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get Raid Controller read cache percent fail", __FUNCTION__);
        return RET_ERR;
    }
    if (read_cache_percent == STORAGE_INFO_INVALID_BYTE) {
        debug_log(DLOG_MASS, "%s, get Raid Controller read cache percent value invalid", __FUNCTION__);
        return RET_ERR;
    }
    *o_result_jso = json_object_new_int(read_cache_percent);
    return RET_OK;
}


LOCAL gint32 get_storage_controller_pcie_link_width(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 pcie_link_width;

    ret = dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_PCIE_LINK_WIDTH, &pcie_link_width);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get Raid Controller pcie link width fail", __FUNCTION__);
        return RET_ERR;
    }
    if (pcie_link_width == STORAGE_INFO_INVALID_BYTE) {
        debug_log(DLOG_MASS, "%s, get Raid Controller pcie link width value invalid", __FUNCTION__);
        return RET_ERR;
    }
    *o_result_jso = json_object_new_int(pcie_link_width);
    return RET_OK;
}


LOCAL gint32 get_storage_controller_mode(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guchar prop_info = 0;
    const gchar *prop_str = NULL;

    gint32 ret = dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_MODE, &prop_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: get Raid Controller mode fail, ret %d.\n", __FUNCTION__, __LINE__, ret);
        return RET_ERR;
    }

    prop_str = sml_raid_ctrl_mode_num2str(prop_info);
    *o_result_jso = json_object_new_string(prop_str);

    return RET_OK;
}

LOCAL gint32 get_storage_controller_cache_pinned_state(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guchar prop_info = 0;

    gint32 ret = dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_CACHE_PINNED, &prop_info);
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get Raid Controller SASAddress fail.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (TRUE == prop_info || FALSE == prop_info) {
        *o_result_jso = json_object_new_boolean(prop_info);
    }

    return RET_OK;
}

LOCAL gint32 get_storage_controller_configuration_version(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar  prop_info[STORAGE_CTRL_INFO_LENGTH] = {0};

    gint32 ret = dal_get_property_value_string(controller_handle, PROPERTY_RAID_CONTROLLER_NVDT_VERSION, prop_info,
        sizeof(prop_info));
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get Raid Controller configuration version.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }
    if (RET_OK != check_string_val_effective((const gchar *)prop_info)) {
        return RET_ERR;
    }

    *o_result_jso = json_object_new_string(prop_info);
    return RET_OK;
}

LOCAL gint32 get_storage_controller_memory_size(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gushort memory_size = 0;

    gint32 ret = dal_get_property_value_uint16(controller_handle, PROPERTY_RAID_CONTROLLER_MEM_SIZE, &memory_size);
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get Raid Controller memory size\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }
    
    if (DRIVE_SMART_INVALID_U16_VALUE == memory_size) {
        debug_log(DLOG_MASS, "%s, %d: count value is invalid \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }
    
    *o_result_jso = json_object_new_int(memory_size);
    return RET_OK;
}

LOCAL gint32 get_storage_controller_fault_history(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar fault_history = 0;

    ret = dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_MAINT_PD_FAIL, &fault_history);
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get Raid Controller fault history switch fail\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    if (ENABLE != fault_history && DISABLE != fault_history) {
        debug_log(DLOG_MASS, "%s, %d: get Raid Controller fault history invalid\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }
    

    *o_result_jso = json_object_new_boolean(fault_history);
    return RET_OK;
}

LOCAL gint32 get_storage_controller_copyback(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guchar copy_back = 0;

    gint32 ret = dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_COPYBACK_ENABLED, &copy_back);
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get Raid Controller fault copyback switch fail\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    if (ENABLE != copy_back && DISABLE != copy_back) {
        debug_log(DLOG_MASS, "%s, %d: get Raid Controller copyback invalid\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }
    
    *o_result_jso = json_object_new_boolean(copy_back);
    return RET_OK;
}

LOCAL gint32 get_storage_controller_smarter_copyback(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guchar smarter_copyback = 0;

    gint32 ret = dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_SMARTER_COPYBACK_ENABLED,
        &smarter_copyback);
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get Raid Controller smarter copyback switch fail\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    if (ENABLE != smarter_copyback && DISABLE != smarter_copyback) {
        debug_log(DLOG_MASS, "%s, %d: get Raid Controller smarter copyback invalid\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }
    
    *o_result_jso = json_object_new_boolean(smarter_copyback);
    return RET_OK;
}

LOCAL gint32 get_storage_controller_jbod(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guchar jbod = 0;

    gint32 ret = dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_JBOD_ENABLED, &jbod);
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get Raid Controller jbod switch fail\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    if (ENABLE != jbod && DISABLE != jbod) {
        debug_log(DLOG_MASS, "%s, %d: get Raid Controller jbod invalid\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }
    

    *o_result_jso = json_object_new_boolean(jbod);
    return RET_OK;
}


LOCAL void get_storage_controller_consis_check_enabled(OBJ_HANDLE controller_handle, json_object *cc_obj)
{
    gint32 ret;
    guint8 enabled = STORAGE_INFO_INVALID_BYTE;
    json_object *prop_obj = NULL;
    ret = dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_CC_ENABLED, &enabled);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get Raid Controller consistency check enabled fail", __FUNCTION__);
    } else if (enabled != CTRL_CC_ENABLE && enabled != CTRL_CC_DISABLE) {
        debug_log(DLOG_DEBUG, "%s: get Raid Controller consistency check enabled invalid, enabled: %d",
            __FUNCTION__, enabled);
    } else {
        prop_obj = json_object_new_boolean(enabled);
    }
    json_object_object_add(cc_obj, RFPROP_STORAGE_CONTROLLER_CC_ENABLED, prop_obj);
    return;
}


LOCAL void get_storage_controller_consis_check_period(OBJ_HANDLE controller_handle, json_object *cc_obj)
{
    gint32 ret;
    guint16 period = STORAGE_INFO_INVALID_WORD;
    json_object *prop_obj = NULL;
    ret = dal_get_property_value_uint16(controller_handle, PROPERTY_RAID_CONTROLLER_CC_PERIOD, &period);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get Raid Controller consistency check period fail", __FUNCTION__);
    } else if (period < CTRL_CC_PERIOD_MIN || period > CTRL_CC_PERIOD_MAX) {
        debug_log(DLOG_DEBUG, "%s: get Raid Controller consistency check period invalid, period: %d",
            __FUNCTION__, period);
    } else {
        prop_obj = json_object_new_int(period);
    }
    json_object_object_add(cc_obj, RFPROP_STORAGE_CONTROLLER_CC_PERIOD, prop_obj);
    return;
}


LOCAL void get_storage_controller_consis_check_rate(OBJ_HANDLE controller_handle, json_object *cc_obj)
{
    gint32 ret;
    guint8 rate = STORAGE_INFO_INVALID_BYTE;
    const gchar *speed = NULL;
    json_object *prop_obj = NULL;
    ret = dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_CC_RATE, &rate);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get Raid Controller consistency check rate fail", __FUNCTION__);
    } else {
        speed = sml_ctrl_ccheck_rate_num2str(rate);
        prop_obj = json_object_new_string(speed);
    }
    json_object_object_add(cc_obj, RFPROP_STORAGE_CONTROLLER_CC_RATE, prop_obj);
    return;
}


LOCAL void get_storage_controller_consis_check_repair(OBJ_HANDLE controller_handle, json_object *cc_obj)
{
    gint32 ret;
    guint8 repair = STORAGE_INFO_INVALID_BYTE;
    json_object *prop_obj = NULL;
    ret = dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_CC_REPAIR, &repair);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get Raid Controller consistency check repair fail", __FUNCTION__);
    } else if (repair != CTRL_CC_ENABLE && repair != CTRL_CC_DISABLE) {
        debug_log(DLOG_DEBUG, "%s: get Raid Controller consistency check auto repair invalid, repair: %d",
            __FUNCTION__, repair);
    } else {
        prop_obj = json_object_new_boolean(repair);
    }
    json_object_object_add(cc_obj, RFPROP_STORAGE_CONTROLLER_CC_REPAIR, prop_obj);
    return;
}


LOCAL void get_storage_controller_consis_check_status(OBJ_HANDLE controller_handle, json_object *cc_obj)
{
    gint32 ret;
    guint8 status = STORAGE_INFO_INVALID_BYTE;
    const gchar *status_s = NULL;
    json_object *prop_obj = NULL;
    ret = dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_CC_STATUS, &status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get Raid Controller consistency check status fail", __FUNCTION__);
    } else {
        status_s = sml_ctrl_ccheck_status_num2str(status);
        prop_obj = json_object_new_string(status_s);
    }
    json_object_object_add(cc_obj, RFPROP_STORAGE_CONTROLLER_CC_STATUS, prop_obj);
    return;
}


LOCAL void get_storage_controller_consis_check_total(OBJ_HANDLE controller_handle, json_object *cc_obj)
{
    gint32 ret;
    guint16 total = STORAGE_INFO_INVALID_WORD;
    json_object *prop_obj = NULL;
    ret = dal_get_property_value_uint16(controller_handle, PROPERTY_RAID_CONTROLLER_CC_TOTALVD, &total);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get Raid Controller consistency check total volume count fail",
            __FUNCTION__);
    } else if (total < STORAGE_INFO_INVALID_BYTE) { 
        prop_obj = json_object_new_int(total);
    }
    json_object_object_add(cc_obj, RFPROP_STORAGE_CONTROLLER_CC_TOTALVD, prop_obj);
    return;
}


LOCAL void get_storage_controller_consis_check_completed(OBJ_HANDLE controller_handle, json_object *cc_obj)
{
    gint32 ret;
    guint16 completed = STORAGE_INFO_INVALID_WORD;
    json_object *prop_obj = NULL;
    ret = dal_get_property_value_uint16(controller_handle, PROPERTY_RAID_CONTROLLER_CC_COMPLETEDVD, &completed);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get Raid Controller consistency check completed volume count fail",
            __FUNCTION__);
    } else if (completed < STORAGE_INFO_INVALID_BYTE) { 
        prop_obj = json_object_new_int(completed);
    }
    json_object_object_add(cc_obj, RFPROP_STORAGE_CONTROLLER_CC_COMPLETEDVD, prop_obj);
    return;
}


LOCAL void get_storage_controller_consis_check_delay(OBJ_HANDLE controller_handle, json_object *cc_obj)
{
    gint32 ret;
    guint32 delay = STORAGE_INFO_INVALID_DWORD;
    json_object *prop_obj = NULL;
    ret = dal_get_property_value_uint32(controller_handle, PROPERTY_RAID_CONTROLLER_CC_DELAY, &delay);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get Raid Controller consistency check delay fail", __FUNCTION__);
    } else if (delay > CTRL_CC_DELAY_MAX) {
        debug_log(DLOG_DEBUG, "%s: get Raid Controller consistency check delay invalid, delay: %u",
            __FUNCTION__, delay);
        delay = 0;
    }
    if (delay != STORAGE_INFO_INVALID_DWORD) {
        prop_obj = json_object_new_int(delay);
    }
    json_object_object_add(cc_obj, RFPROP_STORAGE_CONTROLLER_CC_DELAY, prop_obj);
    return;
}


LOCAL gint32 get_storage_controller_consis_check(OBJ_HANDLE controller_handle, json_object *oem_obj)
{
    json_object *cc_obj = json_object_new_object();
    if (cc_obj == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: new object ConsisCheckInfo fail.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }
    get_storage_controller_consis_check_enabled(controller_handle, cc_obj);
    get_storage_controller_consis_check_period(controller_handle, cc_obj);
    get_storage_controller_consis_check_rate(controller_handle, cc_obj);
    get_storage_controller_consis_check_repair(controller_handle, cc_obj);
    get_storage_controller_consis_check_status(controller_handle, cc_obj);
    get_storage_controller_consis_check_total(controller_handle, cc_obj);
    get_storage_controller_consis_check_completed(controller_handle, cc_obj);
    get_storage_controller_consis_check_delay(controller_handle, cc_obj);
    (void)json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_CC_INFO, cc_obj);
    return RET_OK;
}



LOCAL gint32 get_storage_controller_out_of_band_management(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guchar oobsupport = 0;

    gint32 ret = dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_OOB_SUPPORT, &oobsupport);
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get Raid Controller oobsupport fail\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (ENABLE != oobsupport && DISABLE != oobsupport) {
        debug_log(DLOG_MASS, "%s, %d: get Raid Controller oobsupport invalid\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    *o_result_jso = json_object_new_boolean(oobsupport);

    return RET_OK;
}

LOCAL gint32 get_storage_controller_capacitance_status(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar presence = 0xff;
    guchar health_info = 0xff;
    gchar  state_info[STORAGE_CTRL_INFO_LENGTH] = {0};
    OBJ_HANDLE bbu_handle = 0;

    ret = dfl_get_referenced_object(controller_handle, PROPERTY_RAID_CONTROLLER_REF_OBJECT_BBUSTATUS, &bbu_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_MASS, "%s, %d: get bbu object referenced RefObjectBBUStatus fail.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(bbu_handle, PROPERTY_BBU_PRESENT, &presence);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: bbu presence fail.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    
    if (0xff == presence) {
        *o_result_jso = NULL;
        return RET_OK;
    }
    if (0 == presence) {
        
        (void)strncpy_s(state_info, STORAGE_CTRL_INFO_LENGTH, RFPROP_SENSOR_ABSENT, strlen(RFPROP_SENSOR_ABSENT));
        
    } else {
        (void)strncpy_s(state_info, STORAGE_CTRL_INFO_LENGTH, RFPROP_SENSOR_ENABLED, strlen(RFPROP_SENSOR_ENABLED));

        
        ret = dal_get_property_value_byte(bbu_handle, PROPERTY_BBU_HEALTH, &health_info);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s, %d: get bbu health fail.\n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }
    }

    
    get_resource_status_property(&health_info, NULL, state_info, o_result_jso, TRUE);

    
    get_storage_capacitance_fault_details(bbu_handle, o_result_jso);
    

    return RET_OK;
}



LOCAL gint32 get_storage_controller_capacitance_name(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE bbu_handle = 0;
    gchar bbu_type[MAX_DES_LENGTH] = {0};
    guchar presence = 0xff;

    ret = dfl_get_referenced_object(controller_handle, PROPERTY_RAID_CONTROLLER_REF_OBJECT_BBUSTATUS, &bbu_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_MASS, "%s, %d: get bbu object referenced RefObjectBBUStatus fail.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(bbu_handle, PROPERTY_BBU_PRESENT, &presence);
    (void)dal_get_property_value_string(bbu_handle, PROPERTY_BBU_TYPE, bbu_type, sizeof(bbu_type));

    ret = check_string_val_effective((const gchar *)bbu_type);
    if (ret == RET_OK && presence != 0) {
        *o_result_jso = json_object_new_string(bbu_type);
    } else {
        
        
        *o_result_jso = json_object_new_string("");
        
    }

    return RET_OK;
}


LOCAL gint32 get_storage_controller_driveinfo(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *driveinfo_obj = NULL;
    gchar drive_info[STORAGE_CTRL_INFO_LENGTH] = {0};

    *o_result_jso = json_object_new_object();

    
    gint32 ret = dal_get_property_value_string(controller_handle, PROPERTY_RAID_CONTROLLER_DRIVE_NAME, drive_info,
        sizeof(drive_info));
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get DriveName fail.\n", __FUNCTION__, __LINE__);
        json_object_put(driveinfo_obj);
    }

    
    if (RET_OK != check_string_val_effective((const gchar *)drive_info)) {
        json_object_object_add(*o_result_jso, RFPROP_STORAGE_CONTROLLER_DRIVER_NAME, NULL);
    } else {
        json_object_object_add(*o_result_jso, RFPROP_STORAGE_CONTROLLER_DRIVER_NAME,
            json_object_new_string((const gchar *)drive_info));
    }

    
    (void)memset_s(drive_info, sizeof(drive_info), 0, sizeof(drive_info));
    ret = dal_get_property_value_string(controller_handle, PROPERTY_RAID_CONTROLLER_DRIVE_VERSION, drive_info,
        sizeof(drive_info));
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get DriveVersion fail.\n", __FUNCTION__, __LINE__);
        json_object_put(driveinfo_obj);
    }

    
    if (RET_OK != check_string_val_effective((const gchar *)drive_info)) {
        json_object_object_add(*o_result_jso, RFPROP_STORAGE_CONTROLLER_DRIVER_VERSION, NULL);
    } else {
        json_object_object_add(*o_result_jso, RFPROP_STORAGE_CONTROLLER_DRIVER_VERSION,
            json_object_new_string((const gchar *)drive_info));
    }

    return RET_OK;
}


LOCAL gint32 get_storage_controller_ddr_ecc_count(OBJ_HANDLE controller_handle, json_object **o_result_jso)
{
    gushort error_count = 0;

    gint32 ret = dal_get_property_value_uint16(controller_handle, PROPERTY_RAID_CONTROLLER_ECC_COUNT, &error_count);
    if (RET_OK != ret) {
        debug_log(DLOG_MASS, "%s, %d: get Raid Controller ddr ecc count fail\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (DRIVE_SMART_INVALID_U16_VALUE == error_count) {
        debug_log(DLOG_MASS, "%s, %d: count value is invalid \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    *o_result_jso = json_object_new_int(error_count);

    return RET_OK;
}


LOCAL void get_redfish_oem_property_type_is_uint32(OBJ_HANDLE obj_handle, const gchar *prop_name,
    json_object **o_prop_jso)
{
    guint32 prop_value = 0;
    gint32 ret = dal_get_property_value_uint32(obj_handle, prop_name, &prop_value);
    if (RET_OK != ret) {
        *o_prop_jso = NULL;
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.", __FUNCTION__, __LINE__);
        return;
    }

    if (DRIVE_SMART_INVALID_U32_VALUE == prop_value) {
        *o_prop_jso = NULL;
        debug_log(DLOG_MASS, "%s, %d: property value is invalid.", __FUNCTION__, __LINE__);
        return;
    }

    *o_prop_jso = json_object_new_int((gint32)prop_value);
    if (NULL == *o_prop_jso) {
        debug_log(DLOG_ERROR, "%s, %d: json object new int fail.", __FUNCTION__, __LINE__);
    }
    return;
}


LOCAL void get_storage_controller_phystatus_info(OBJ_HANDLE ref_object, guint8 id, json_object **o_object_jso)
{
    str_map_s phystatus_property_array[] = {
        {PROPERTY_SASPHYSTATUS_INVALID_DWORD_COUNT,       RFPROP_STORAGE_PHYSTATUS_INVALID_DWORD_COUNT},
        {PROPERTY_SASPHYSTATUS_LOSS_DWORD_SYNC_COUNT,     RFPROP_STORAGE_PHYSTATUS_LOSS_DWORD_COUNT},
        {PROPERTY_SASPHYSTATUS_PHY_RESET_PROBLEM_COUNT,   RFPROP_STORAGE_PHYSTATUS_RESET_PROBLEM_COUNT},
        {PROPERTY_SASPHYSTATUS_RUNNING_DISPARITY_ERROR_COUNT, RFPROP_STORAGE_PHYSTATUS_RUNNING_DIS_ERR_COUNT},
    };

    guint32 length = G_N_ELEMENTS(phystatus_property_array);
    json_object *jso_obj = json_object_new_int(id);
    (void)json_object_object_add(*o_object_jso, RFPROP_STORAGE_PHYSTATUS_PHYID, jso_obj);
    jso_obj = NULL;

    for (guint32 i = 0; i < length; i++) {
        (void)get_redfish_oem_property_type_is_uint32(ref_object, phystatus_property_array[i].str1, &jso_obj);
        (void)json_object_object_add(*o_object_jso, phystatus_property_array[i].str2, jso_obj);
        jso_obj = NULL;
    }

    return;
}


LOCAL gint32 get_storage_controller_phystatus(OBJ_HANDLE controller_handle, json_object **o_result_jso)
{
    gint32 ret = 0;
    gint32 retval;
    GVariant *array_obj = NULL;
    GVariant *obj_name_array = NULL;
    json_object *json_obj = NULL;
    const gchar *obj_name = 0;
    GVariantIter iter;
    OBJ_HANDLE ref_object = 0;
    guint8 id = 0;

    *o_result_jso = json_object_new_array();

    if (NULL == *o_result_jso) {
        debug_log(DLOG_MASS, "%s, %d: get json new arrry fail\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    retval = dfl_get_property_value(controller_handle, PROPERTY_RAID_CONTROLLER_REF_OBJECT_SASPHYSTATUS, &array_obj);
    if (retval != DFL_OK) {
        debug_log(DLOG_MASS, "Not found SAS PHY Status object");
        return RET_ERR;
    }
    g_variant_iter_init(&iter, array_obj);

    while (NULL != (obj_name_array = g_variant_iter_next_value(&iter))) {
        obj_name = g_variant_get_string(obj_name_array, NULL);
        if (NULL == obj_name) {
            g_variant_unref(obj_name_array);
            obj_name_array = NULL;
            continue;
        }

        
        json_obj = json_object_new_object();
        if (json_obj == NULL) {
            g_variant_unref(obj_name_array);
            obj_name_array = NULL;
            continue;
        }

        
        retval = dfl_get_object_handle(obj_name, &ref_object);
        if (retval != DFL_OK) {
            g_variant_unref(obj_name_array);
            json_object_put(json_obj);
            continue;
        }

        
        retval = dal_get_property_value_byte(ref_object, PROPERTY_SASPHYSTATUS_PHY_ID, &id);
        if (RET_OK != retval) {
            g_variant_unref(obj_name_array);
            debug_log(DLOG_ERROR, "%s: Get object (%s) handle failed, result=%d\n", __FUNCTION__, obj_name, retval);
            json_object_put(json_obj);
            continue;
        }

        
        if (0xFF != id) {
            (void)get_storage_controller_phystatus_info(ref_object, id, &json_obj);
            ret = json_object_array_add(*o_result_jso, json_obj);
            if (ret != 0) {
                json_object_put(json_obj);
                debug_log(DLOG_MASS, "%s:json_object_array_add fail", __FUNCTION__);
            }
        } else {
            json_object_put(json_obj);
        }

        g_variant_unref(obj_name_array);
    }

    g_variant_unref(array_obj);

    return RET_OK;
}


LOCAL gint32 get_storage_controller_stripsize(OBJ_HANDLE controller_handle, const gchar *property_name,
    json_object **o_result_jso)
{
    guint8 stripe_size = 0;
    gint32 rf_size_bytes = 0;

    gint32 ret = dal_get_property_value_byte(controller_handle, property_name, &stripe_size);
    if (RET_OK != ret) {
        debug_log(DLOG_MASS, "%s, %d: get property byte value fail.", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    if (0 == stripe_size) {
        rf_size_bytes = 512;
    } else if (stripe_size <= 21) { 
        rf_size_bytes = 1024 * (1 << (stripe_size - 1));
    } else {
        debug_log(DLOG_MASS, "%s:invalid strip size is %d", __FUNCTION__, rf_size_bytes);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_int(rf_size_bytes);
    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s: json_object_new_int fail.", __FUNCTION__);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 get_storage_controller_property_string(OBJ_HANDLE controller_handle, const gchar *property_name,
    json_object **o_result_jso)
{
    gchar  prop_info[STORAGE_CTRL_INFO_LENGTH] = {0};

    gint32 ret = dal_get_property_value_string(controller_handle, property_name, prop_info, sizeof(prop_info));
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get Raid Controller driver name.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }
    if ((RET_OK != check_string_val_effective((const gchar *)prop_info))) {
        return RET_ERR;
    }

    *o_result_jso = json_object_new_string((const gchar *)prop_info);
    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s: json_object_new_int fail.", __FUNCTION__);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 get_storage_controller_support_raidlevel(OBJ_HANDLE controller_handle, json_object **o_result_jso)
{
    gint32 ret = 0;
    guint32 raidlevel = 0;
    guint32 i;
    gchar* level_array[] = {
        SML_RAID_LEVEL_0,
        SML_RAID_LEVEL_1,
        SML_RAID_LEVEL_5,
        SML_RAID_LEVEL_6,
        SML_RAID_LEVEL_10,
        SML_RAID_LEVEL_50,
        SML_RAID_LEVEL_60,
        SML_RAID_LEVEL_1ADM,
        SML_RAID_LEVEL_10ADM,
        SML_RAID_LEVEL_1TRIPLE,
        SML_RAID_LEVEL_10TRIPLE,
    };
    json_object *json_str = NULL;
    *o_result_jso = json_object_new_array();
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: json object new array fail.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    ret = dal_get_property_value_uint32(controller_handle, PROPERTY_RAID_CONTROLLER_CTRL_OPTION3, &raidlevel);
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get Raid Controller RAID Level ret = %d.\n", __FUNCTION__, __LINE__, ret);
        return RET_ERR;
    }

    
    for (i = 8; i < 19; i++) {
        if (raidlevel & (1 << i)) {
            json_str = json_object_new_string(level_array[i - 8]);
            if (NULL == json_str) {
                continue;
            }
            ret = json_object_array_add(*o_result_jso, json_str);
            if (ret != 0) {
                json_object_put(json_str);
            }
        }
    }

    return RET_OK;
}


LOCAL void __get_storage_controller_support_mode(OBJ_HANDLE controller_handle, json_object* oem_obj)
{
    SML_CTRL_OPTION2_S ctrl_option2;
    json_object* json_str = NULL;

    gint32 ret = dal_get_property_value_uint32(controller_handle, PROPERTY_RAID_CONTROLLER_CTRL_OPTION2,
        &ctrl_option2.opt2_val);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get CtrlOption2 failed, ret = %d.", __FUNCTION__, ret);
        return;
    }

    json_object *prop_obj = json_object_new_array();
    if (prop_obj == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_array failed.", __FUNCTION__);
        return;
    }

    if (ctrl_option2.opt2_div.ctrl_support_raid == 1) {
        json_str = json_object_new_string(CTRL_MODE_RAID_STR);
        if (json_str != NULL) {
            json_object_array_add(prop_obj, json_str);
        }
    }

    if (ctrl_option2.opt2_div.ctrl_support_hba == 1) {
        json_str = json_object_new_string(CTRL_MODE_HBA_STR);
        if (json_str != NULL) {
            json_object_array_add(prop_obj, json_str);
        }
    }

    if (ctrl_option2.opt2_div.ctrl_support_jbod == 1) {
        json_str = json_object_new_string(CTRL_MODE_JBOD_STR);
        if (json_str != NULL) {
            json_object_array_add(prop_obj, json_str);
        }
    }

    if (ctrl_option2.opt2_div.ctrl_support_mixed == 1) {
        json_str = json_object_new_string(CTRL_MODE_MIXED_STR);
        if (json_str != NULL) {
            json_object_array_add(prop_obj, json_str);
        }
    }

    json_object_object_add(oem_obj, REFROP_STORAGE_CONYTOLLER_SUPPORT_MODE, prop_obj);

    return;
}



LOCAL gint32 get_storage_controller_refcard(OBJ_HANDLE controller_handle, json_object **json_obj)
{
    gchar uri[PSLOT_URI_LEN] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    OBJ_HANDLE component_handle;
    gchar pcie_memberid[MAX_RSC_ID_LEN] = {0};
    guint8 fru_id = 0;

    gint32 ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:%d get board slot fail", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    ret = dal_get_specific_object_position(controller_handle, CLASS_COMPONENT, &component_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:%d dal_get_specific_object_position fail", __FUNCTION__, __LINE__);
        return RET_ERR;
    }
    
    (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_FRUID, &fru_id);
    if (DRIVE_INVALID_VALUE == fru_id) {
        debug_log(DLOG_MASS, "%s:%d fru id is invalid.", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    ret = get_object_obj_location_devicename(component_handle, pcie_memberid, sizeof(pcie_memberid));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:%d get_object_obj_location_devicename fail", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_CHASSIS_BOARD, slot, pcie_memberid);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s %d  snprintf_s fail.", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    *json_obj = json_object_new_object();
    if (*json_obj == NULL) {
        debug_log(DLOG_ERROR, "%s,%d:new json object pointer is NULL", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(*json_obj, RFPROP_ODATA_ID, json_object_new_string((const gchar *)uri));
    return RET_OK;
}

#define MAX_BDF_STR_LEN 16
LOCAL gint32 get_storage_crtl_bdf(OBJ_HANDLE obj_handle, json_object **o_result_jso)
{
    guint8 function;
    guint8 device;
    guint8 bus;
    guint8 segment;
    gchar bdf_str[MAX_BDF_STR_LEN] = {0};
    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_RAID_CONTROLLER_PCI_SEGMENT, &segment);
    if (ret != RET_OK) {
        return ret;
    }
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_RAID_CONTROLLER_PCI_BUS, &bus);
    if (ret != RET_OK) {
        return ret;
    }
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_RAID_CONTROLLER_PCI_DEVICE, &device);
    if (ret != RET_OK) {
        return ret;
    }
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_RAID_CONTROLLER_PCI_FUNCTION, &function);
    if (ret != RET_OK) {
        return ret;
    }

    ret = snprintf_s(bdf_str, sizeof(bdf_str), sizeof(bdf_str) - 1, "%04x:%02x:%02x.%01x",
        segment, bus, device, function);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    debug_log(DLOG_INFO, "rootBDF:%s", bdf_str);

    *o_result_jso = json_object_new_string((const gchar *)bdf_str);
    return RET_OK;
}


LOCAL gint32 get_storage_ctrl_bdf_by_pcieaddrinfo(GSList *pcie_addr_list, guint8 com_device_type,
    guint8 slot_id, guint8 group_id, json_object **o_result_jso)
{
    guint8 addr_device_type = 0;
    guint8 addr_slot_id = 0;
    guint8 addr_group_id = 0;
    guint8 segment = 0;
    guint8 function = 0;
    guint8 device = 0;
    guint8 bus = 0;
    OBJ_HANDLE addr_handle;
    GSList *addr_node = NULL;
    gchar bdf_str[MAX_BDF_STR_LEN] = {0};

    for (addr_node = pcie_addr_list; addr_node; addr_node = addr_node->next) {
        addr_handle = (OBJ_HANDLE)addr_node->data;
        (void)dal_get_property_value_byte(addr_handle, PROPERTY_COMPONENT_TYPE, &addr_device_type);
        (void)dal_get_property_value_byte(addr_handle, PROPERTY_PCIE_SLOT_ID, &addr_slot_id);
        (void)dal_get_property_value_byte(addr_handle, PROPERTY_GROUP_ID, &addr_group_id);

        // 根据devicetype和slot id，group_id来匹配
        if ((com_device_type == addr_device_type) && (slot_id == addr_slot_id) && (group_id == addr_group_id)) {
            (void)dal_get_property_value_byte(addr_handle, PROPERTY_PCIE_BUS, &bus);
            (void)dal_get_property_value_byte(addr_handle, PROPERTY_PCIE_DEVICE, &device);
            (void)dal_get_property_value_byte(addr_handle, PROPERTY_PCIE_FUNCTION, &function);
            debug_log(DLOG_DEBUG, "%s: get bdf fail, device type %d, slot id %d, group id %d", __FUNCTION__,
                com_device_type, slot_id, group_id);

            (void)snprintf_s(bdf_str, MAX_BDF_STR_LEN, MAX_BDF_STR_LEN - 1, "%04x:%02x:%02x.%01x", segment, bus, device,
                function);
            debug_log(DLOG_INFO, "rootBDF:%s", bdf_str);

            *o_result_jso = json_object_new_string((const gchar *)bdf_str);
            return RET_OK;
        }
    }
    return RET_ERR;
}



LOCAL gint32 get_storage_controller_bdf(OBJ_HANDLE obj_handle, json_object **o_result_jso)
{
    gint32 ret;
    GSList *pcie_addr_list = NULL;
    OBJ_HANDLE com_handle;
    guint8 com_device_type = 0;
    guint8 slot_id = 0;
    guint8 group_id = 0;
    gchar ref_object_name[MAX_NAME_SIZE] = {0};
    gchar ref_property_name[MAX_NAME_SIZE] = {0};

    if (o_result_jso == NULL) {
        return ret;
    }
    if (dal_check_board_special_me_type() == RET_OK) {
        return get_storage_crtl_bdf(obj_handle, o_result_jso);
    }
    // 通过引用对象获取PcieAddrInfo类的对象去匹配后获取BDF信息
    ret = dfl_get_referenced_property(obj_handle, PROPERTY_RAID_CONTROLLER_COMPONENT_NAME, ref_object_name,
        ref_property_name, MAX_NAME_SIZE, MAX_NAME_SIZE);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: get ref_object_name fail, ret is %d", __FUNCTION__, ret);
        return ret;
    }

    ret = dfl_get_object_handle((const gchar *)ref_object_name, &com_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: get ref_object_name handle fail, ret is %d", __FUNCTION__, ret);
        return ret;
    }

    // 获取devicetype和slot id(devicenum等于slot id)，group_id
    (void)dal_get_property_value_byte(com_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &com_device_type);
    (void)dal_get_property_value_byte(com_handle, PROPERTY_COMPONENT_DEVICENUM, &slot_id);
    (void)dal_get_property_value_byte(com_handle, PROPERTY_COMPONENT_GROUPID, &group_id);

    ret = dfl_get_object_list(CLASS_PCIE_ADDR_INFO, &pcie_addr_list);
    if (ret != DFL_OK || pcie_addr_list == NULL) {
        debug_log(DLOG_DEBUG, "%s: dfl_get_object_list CLASS_PCIE_ADDR_INFO fail,ret is %d", __FUNCTION__, ret);
        return ret;
    }
    ret = get_storage_ctrl_bdf_by_pcieaddrinfo(pcie_addr_list, com_device_type, slot_id, group_id, o_result_jso);
    g_slist_free(pcie_addr_list);
    return ret;
}



LOCAL gint32 get_storage_controller_vid_did_raid(OBJ_HANDLE com_handle_raid_ctrl, json_object *oem_obj)
{
    guint8 comp_type = 0;
    gint32 ret = 0;
    OBJ_HANDLE raid_card_rf_com_handle;
    OBJ_HANDLE raid_card_handle;
    GSList *raid_card_node = NULL;
    GSList *raid_card_list = NULL;

    if (oem_obj == NULL || com_handle_raid_ctrl == 0) {
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(com_handle_raid_ctrl, PROPERTY_COMPONENT_DEVICE_TYPE, &comp_type);
    // 如果是raid标卡，从所有PcieCard类里面找
    if (comp_type == COMPONENT_TYPE_PCIE_CARD) {
        ret = dfl_get_object_list(CLASS_PCIE_CARD, &raid_card_list);
        if (ret != DFL_OK || raid_card_list == NULL) {
            debug_log(DLOG_DEBUG, "%s: dfl_get_object_list CLASS_PCIE_CARD fail,ret is %d", __FUNCTION__, ret);
            return ret;
        }
        // 如果是raid扣卡，从所有Raid类里面找
    } else if (comp_type == COMPONENT_TYPE_RAID_CARD) {
        ret = dfl_get_object_list(CLASS_RAIDCARD_NAME, &raid_card_list);
        if (ret != DFL_OK || raid_card_list == NULL) {
            debug_log(DLOG_DEBUG, "%s: dfl_get_object_list from CLASS_PCIE_CARD fail, ret is %d", __FUNCTION__, ret);
            return ret;
        }
    } else {
        debug_log(DLOG_DEBUG, "%s: invaild Raid Card Component Type.", __FUNCTION__);
        return RET_ERR;
    }
    // 获取Raid卡(PcieCard类或者Raid类)引用的Component对象和RaidController引用的Component对象相同，视为找到目标对象
    for (raid_card_node = raid_card_list; raid_card_node; raid_card_node = raid_card_node->next) {
        raid_card_handle = (OBJ_HANDLE)raid_card_node->data;
        ret = dfl_get_referenced_object(raid_card_handle, PROPERTY_RAIDCARD_REF_COMPONENT, &raid_card_rf_com_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_DEBUG, "%s: get RefComponent object of PcieCard or Raid class failed, ret is %d",
                __FUNCTION__, ret);
            continue;
        }
        // Raid扣卡的Raid类和Raid标卡的PcieCard类都有四元组信息
        if (com_handle_raid_ctrl == raid_card_rf_com_handle) {
            (void)rf_add_property_jso_uint16_hex(raid_card_handle, PROPERTY_RAIDCARD_VID, PROPERTY_DEVICE_OEM_VID,
                oem_obj);
            (void)rf_add_property_jso_uint16_hex(raid_card_handle, PROPERTY_RAIDCARD_DID, PROPERTY_DEVICE_OEM_DID,
                oem_obj);
            (void)rf_add_property_jso_uint16_hex(raid_card_handle, PROPERTY_RAIDCARD_SVID, PROPERTY_DEVICE_OEM_SUB_VID,
                oem_obj);
            (void)rf_add_property_jso_uint16_hex(raid_card_handle, PROPERTY_RAIDCARD_SDID, PROPERTY_DEVICE_OEM_SUB_DID,
                oem_obj);
            break;
        }
    }
    g_slist_free(raid_card_list);
    return RET_OK;
}


LOCAL gint32 get_storage_controller_vid_did_inner(OBJ_HANDLE obj_handle, json_object *oem_obj)
{
    OBJ_HANDLE com_handle_raid_ctrl;
    gchar ref_object_name[MAX_NAME_SIZE] = {0};
    gchar ref_property_name[MAX_NAME_SIZE] = {0};

    // 通过RaidController获取Component对象
    gint32 ret = dfl_get_referenced_property(obj_handle, PROPERTY_RAID_CONTROLLER_COMPONENT_NAME, ref_object_name,
        ref_property_name, MAX_NAME_SIZE, MAX_NAME_SIZE);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: get ref_object_name fail, ret is %d", __FUNCTION__, ret);
        return ret;
    }

    ret = dfl_get_object_handle((const gchar *)ref_object_name, &com_handle_raid_ctrl);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: get ref_object_name handle fail, ret is %d", __FUNCTION__, ret);
        return ret;
    }

    ret = get_storage_controller_vid_did_raid(com_handle_raid_ctrl, oem_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: PCIe RAID Card get PCIe identity info failed, ret is %d", __FUNCTION__, ret);
        return ret;
    }
    return RET_OK;
}


LOCAL void get_storage_controller_type(OBJ_HANDLE obj_handle, json_object *huawei)
{
    gchar raid_type[PROP_VAL_LENGTH] = {0};
    json_object *type_jso = NULL;

    if (huawei == NULL) {
        return;
    }

    (void)dal_get_property_value_string(obj_handle, PROPERTY_RAID_CONTROLLER_TYPE, raid_type, sizeof(raid_type));

    gint32 ret_val = check_string_val_effective((const gchar *)raid_type);
    if (ret_val != RET_OK) {
        goto exit;
    }

    type_jso = json_object_new_string((const gchar *)raid_type);

exit:

    json_object_object_add(huawei, RFPROP_STORAGE_CONTROLLER_TYPE, type_jso);
}

void get_storage_is_support_volume(OBJ_HANDLE obj_handle, json_object *huawei)
{
    guint8 TypeId = 0;

    if (NULL == huawei) {
        return;
    }
    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_RAID_CONTROLLER_TYPE_ID, &TypeId);
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s:get TypeID error", __FUNCTION__);
        json_object_object_add(huawei, RFPROP_STORAGE_CONTROLLER_SUPPORT_RIAD, NULL);
        return;
    }
    // 当raid卡为IT卡时，不支持组逻辑盘
    if (LSI_3108_WITH_IT == TypeId || LSI_3008_WITH_IT == TypeId || LSI_3408_WITH_IT == TypeId ||
        LSI_3416_WITH_IT == TypeId) {
        json_object_object_add(huawei, RFPROP_STORAGE_CONTROLLER_SUPPORT_RIAD, json_object_new_boolean(FALSE));
    } else {
        json_object_object_add(huawei, RFPROP_STORAGE_CONTROLLER_SUPPORT_RIAD, json_object_new_boolean(TRUE));
    }

    return;
}


void get_oem_storage_controller_mode(OBJ_HANDLE obj_handle, json_object *huawei)
{
    guint8 mode = 0;

    const gchar *prop_str = NULL;

    if (huawei == NULL) {
        return;
    }

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_RAID_CONTROLLER_MODE, &mode);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get controller %s mode error", __FUNCTION__, dfl_get_object_name(obj_handle));
        json_object_object_add(huawei, RFPROP_STORAGE_CONTROLLER_MODE, json_object_new_string(STRING_VALUE_IS_NA));
        return;
    }

    prop_str = sml_raid_ctrl_mode_num2str(mode);
    json_object_object_add(huawei, RFPROP_STORAGE_CONTROLLER_MODE, json_object_new_string(prop_str));

    return;
}


LOCAL void get_storage_fault_details(gushort health_status_code, json_object *status_jso)
{
    errno_t safe_fun_ret = EOK;
    gchar health_status_string[PROP_VAL_MAX_LENGTH] = {0};
    gint32 last_index = 0;
    json_object *oem_jso = NULL;
    json_object *huawei_jso = NULL;

    return_if_expr(status_jso == NULL);

    huawei_jso = json_object_new_object();
    return_do_info_if_expr(huawei_jso == NULL, debug_log(DLOG_ERROR, "%s: new object fail", __func__));

    oem_jso = json_object_new_object();
    return_do_info_if_expr(oem_jso == NULL, json_object_put(huawei_jso);
        debug_log(DLOG_ERROR, "%s: new oem_jso fail", __func__));

    json_object_object_add(oem_jso, RFPROP_COMMON_HUAWEI, huawei_jso);
    json_object_object_add(status_jso, RFPROP_COMMON_OEM, oem_jso);

    if (STORAGE_INFO_INVALID_WORD == health_status_code) {
        json_object_object_add(huawei_jso, RFPROP_STORAGE_CONTROLLER_FAULT_DETAILS, NULL);
        return;
    }

    if (health_status_code & TRUE) {
        safe_fun_ret = strncat_s(health_status_string, sizeof(health_status_string), MEMORY_CORRECTAB_ERRORS,
            strlen(MEMORY_CORRECTAB_ERRORS));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }
    if (health_status_code & (TRUE << BIT_OFFSET_MEMORY_CORRECTABLE_ERROR)) {
        safe_fun_ret = strncat_s(health_status_string, sizeof(health_status_string), MEMORY_UNCORRECTAB_ERRORS,
            strlen(MEMORY_UNCORRECTAB_ERRORS));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }
    if (health_status_code & (TRUE << BIT_OFFSET_MEMORY_UNCORRECTABLE_ERROR)) {
        safe_fun_ret = strncat_s(health_status_string, sizeof(health_status_string), MEMORY_ECC_ERRORS,
            strlen(MEMORY_ECC_ERRORS));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }
    if (health_status_code & (TRUE << BIT_OFFSET_ECC_ERROR)) {
        safe_fun_ret = strncat_s(health_status_string, sizeof(health_status_string), NVRAM_UNCORRECTABLE_ERRORS,
            strlen(NVRAM_UNCORRECTABLE_ERRORS));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }
    if (health_status_code & (TRUE << BIT_OFFSET_COMMUNICATION_LOST)) {
        safe_fun_ret = strncat_s(health_status_string, sizeof(health_status_string), RAID_COMMUNICATION_LOSS,
            strlen(RAID_COMMUNICATION_LOSS));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }

    if (strlen(health_status_string)) {
        
        last_index = strlen(health_status_string) - 1;
        health_status_string[last_index] = 0;
        json_object_object_add(huawei_jso, RFPROP_STORAGE_CONTROLLER_FAULT_DETAILS,
            json_object_new_string((const gchar *)health_status_string));
    } else {
        json_object_object_add(huawei_jso, RFPROP_STORAGE_CONTROLLER_FAULT_DETAILS, NULL);
    }

    return;
}


LOCAL void get_storage_capacitance_fault_details(OBJ_HANDLE bbu_handle, json_object **result_obj)
{
    errno_t safe_fun_ret = EOK;
    guint8 bbu_parameter = 0;
    gchar bbu_health_status_string[PROP_VAL_MAX_LENGTH + 1] = {0};
    gint32 last_index = 0;
    gint32 ret = 0;
    gchar* bbu_parameter_str[7] = {PROPERTY_BBU_VOLTAGE_LOW,
                                   PROPERTY_BBU_REPLACE_PACK,
                                   PROPERTY_BBU_LEARN_CYCLE_FAILED,
                                   PROPERTY_BBU_LEARN_CYCLE_TIMEOUT,
                                   PROPERTY_BBU_PREDICTIVE_FAILURE,
                                   PROPERTY_BBU_REMAINING_CAPACITY_LOW,
                                   PROPERTY_BBU_NO_SPACE_FOR_CACHE_OFFLOAD};
    gchar* bbu_fault_details_str[7] = {BBU_VOLTAGE_LOW,
                                       BBU_NEEDS_TO_BE_REPLACED,
                                       BBU_LEARN_CYCLE_FAILED,
                                       BBU_LEARN_CYCLE_TIMEOUT,
                                       BBU_PACK_IS_ABOUT_TO_FAIL,
                                       BBU_REMAINING_CAPACITY_LOW,
                                       BBU_NO_SPACE_FOR_CACHE_OFFLOAD};

    return_if_expr(bbu_handle == 0);
    // 若上一层未成功创建对象，此处需要创建对象
    if (NULL == *result_obj) {
        *result_obj = json_object_new_object();
        if (NULL == *result_obj) {
            debug_log(DLOG_ERROR, "%s, %d: new *o_result_jso fail.\n", __FUNCTION__, __LINE__);
            return;
        }
    }
    // 遍历数组，获取每一个健康信息
    for (gint32 i = 0; i < 7; i++) {
        ret = dal_get_property_value_byte(bbu_handle, bbu_parameter_str[i], &bbu_parameter);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s, %d: get parameter %s fail,ret=%d\n", __FUNCTION__, __LINE__,
                bbu_parameter_str[i], ret);
            continue;
        }
        if (bbu_parameter == 1) {
            safe_fun_ret = strncat_s(bbu_health_status_string, sizeof(bbu_health_status_string),
                bbu_fault_details_str[i], strlen(bbu_fault_details_str[i]));
            do_val_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        }
        // 每次用完之后把这个参数置为0，以免影响下一次判断
        bbu_parameter = 0;
    }

    if (strlen(bbu_health_status_string) > 0) {
        
        last_index = strlen(bbu_health_status_string) - 1;
        bbu_health_status_string[last_index] = 0;
        json_object_object_add(*result_obj, RFPROP_STORAGE_CONTROLLER_CAPACITION_FAULT_DETAILS,
            json_object_new_string((const gchar *)bbu_health_status_string));
    } else {
        json_object_object_add(*result_obj, RFPROP_STORAGE_CONTROLLER_CAPACITION_FAULT_DETAILS, NULL);
    }

    return;
}


LOCAL void parse_storage_policy(guint32 ctrl_option, json_object *policy_obj, const gchar *supported_policy_string1,
    const gchar *supported_policy_string2, const gchar *supported_policy_string3, const guint32 offset)
{
    json_object *supported_policy_jso = NULL;
    // 入参supported_policy_string3可以为空
    if (NULL == policy_obj || NULL == supported_policy_string1 || NULL == supported_policy_string2) {
        return;
    }

    supported_policy_jso = json_object_new_array();
    if (NULL == supported_policy_jso) {
        return;
    }
    // 控制器类中的conption1和conption2的每个byte的bit7代表是否支持配置相应策略，bit0,bit1,bit2代表支持的策略
    // 判断bit7
    if (ctrl_option & (TRUE << (offset + BIT_OFFSET_SEVEN))) {
        json_object_object_add(policy_obj, RFPROP_STORAGE_CONTROLLER_READPOLICY_CONFIGURABLE,
            json_object_new_boolean(TRUE));
    } else {
        json_object_object_add(policy_obj, RFPROP_STORAGE_CONTROLLER_READPOLICY_CONFIGURABLE,
            json_object_new_boolean(FALSE));
    }
    // 判断相对的bit0,bit1,bit2
    if (ctrl_option & (TRUE << offset)) {
        json_object_array_add(supported_policy_jso, json_object_new_string(supported_policy_string1));
    }
    if (ctrl_option & (TRUE << (offset + BIT_OFFSET_ONE))) {
        json_object_array_add(supported_policy_jso, json_object_new_string(supported_policy_string2));
    }
    if ((ctrl_option & (TRUE << (offset + BIT_OFFSET_TWO))) && (NULL != supported_policy_string3)) {
        json_object_array_add(supported_policy_jso, json_object_new_string(supported_policy_string3));
    }

    json_object_object_add(policy_obj, RFPROP_STORAGE_CONTROLLER_READPOLICY_SUPPORTED, supported_policy_jso);

    return;
}

LOCAL void get_storage_policy(OBJ_HANDLE obj_handle, json_object *huawei)
{
    guint32 ctrl_option = 0;
    json_object *read_policy_obj = NULL;
    json_object *write_policy_obj = NULL;
    json_object *cache_policy_obj = NULL;
    json_object *access_policy_obj = NULL;
    json_object *driver_cache_policy_obj = NULL;

    return_if_expr(huawei == NULL);

    gint32 ret = dal_get_property_value_uint32(obj_handle, PROPERTY_RAID_CONTROLLER_CTRL_OPTION1, &ctrl_option);
    return_do_info_if_fail(ret == RET_OK, debug_log(DLOG_ERROR, "%s:get CtrlOption1  error", __FUNCTION__));

    read_policy_obj = json_object_new_object();
    if (read_policy_obj == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: new read_policy json object fail.\n", __FUNCTION__, __LINE__);
        return;
    }
    parse_storage_policy(ctrl_option, read_policy_obj, LD_READ_NO, LD_READ_AHEAD, NULL, FIRST_BYTE_START_BIT);
    json_object_object_add(huawei, RFPROP_STORAGE_CONTROLLER_READ_POLICY, read_policy_obj);

    write_policy_obj = json_object_new_object();
    if (write_policy_obj == NULL) {
        debug_log(DLOG_ERROR, "%s: new write_policy json object fail.", __FUNCTION__);
        return;
    }
    parse_storage_policy(ctrl_option, write_policy_obj, LD_WRITE_THROUGH, LD_WRITE_BACKBBU, LD_WRITE_BACK,
        SECOND_BYTE_START_BIT);
    json_object_object_add(huawei, RFPROP_STORAGE_CONTROLLER_WRITE_POLICY, write_policy_obj);

    cache_policy_obj = json_object_new_object();
    if (cache_policy_obj == NULL) {
        debug_log(DLOG_ERROR, "%s: new cache_policy json object fail.", __FUNCTION__);
        return;
    }
    parse_storage_policy(ctrl_option, cache_policy_obj, LD_IO_CACHE, LC_IO_DIRECT, NULL, THIRD_BYTE_START_BIT);
    json_object_object_add(huawei, RFPROP_STORAGE_CONTROLLER_CACHE_POLICY, cache_policy_obj);

    access_policy_obj = json_object_new_object();
    if (access_policy_obj == NULL) {
        debug_log(DLOG_ERROR, "%s: new access_policy json object fail.", __FUNCTION__);
        return;
    }
    parse_storage_policy(ctrl_option, access_policy_obj, LD_ACC_RW, LD_ACC_READ, LD_ACC_BLOCK, FOURTH_BYTE_START_BIT);
    json_object_object_add(huawei, RFPROP_STORAGE_CONTROLLER_ACCESS_POLICY, access_policy_obj);

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_RAID_CONTROLLER_CTRL_OPTION2, &ctrl_option);
    return_do_info_if_fail(ret == RET_OK, debug_log(DLOG_ERROR, "%s:get CtrlOption2  error", __FUNCTION__));

    driver_cache_policy_obj = json_object_new_object();
    if (driver_cache_policy_obj == NULL) {
        debug_log(DLOG_ERROR, "%s: new driver_cache_policy json object fail.", __FUNCTION__);
        return;
    }
    parse_storage_policy(ctrl_option, driver_cache_policy_obj, LD_DCACHE_DEF, LD_DCACHE_ENABLE, LD_DCACHE_DISABLE,
        FIRST_BYTE_START_BIT);
    json_object_object_add(huawei, RFPROP_STORAGE_CONTROLLER_DRIVE_CACHE_POLICY, driver_cache_policy_obj);

    return;
}

LOCAL void get_boot_device_desc(const gchar *boot_device, gchar *buf, gsize buf_size)
{
    if (strstr(boot_device, "Logical") != NULL) {
        errno_t securec_rv = strncpy_s(buf, buf_size, boot_device, strlen(boot_device));
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s failed, ret = %d", __FUNCTION__, securec_rv);
        }
        dal_clear_string_blank(buf, buf_size);
    } else {
        OBJ_HANDLE hdd_obj = 0;
        (void)dal_get_specific_object_string(CLASS_HDD_NAME, PROPERTY_HDD_DEVICENAME, boot_device, &hdd_obj);
        get_drive_id(hdd_obj, buf, buf_size);
    }
}

LOCAL void get_storage_controller_boot_devices(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object *oem_obj)
{
    gchar **boot_devices = NULL;
    gsize len = 0;

    gint32 ret =
        dal_get_property_value_strv(controller_handle, PROPERTY_RAID_CONTROLLER_BOOT_DEVICES, &boot_devices, &len);
    if (ret != RET_OK) {
        return;
    }

    json_object *prop_obj = json_object_new_array();
    if (prop_obj == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_array failed", __FUNCTION__);
        g_strfreev(boot_devices);
        return;
    }

    gchar desc[MAX_STRBUF_LEN + 1] = {0};
    for (gsize i = 0; i < len; i++) {
        if (g_strcmp0(boot_devices[i], "None") == 0) {
            json_object_array_add(prop_obj, json_object_new_string(boot_devices[i]));
        } else {
            (void)memset_s(desc, sizeof(desc), 0, sizeof(desc));
            get_boot_device_desc(boot_devices[i], desc, sizeof(desc));
            json_object_array_add(prop_obj, json_object_new_string(desc));
        }
    }

    json_object_object_add(oem_obj, REPROP_STORAGE_CONTROLLER_BOOT_DEVICES, prop_obj);
    g_strfreev(boot_devices);
    return;
}

LOCAL json_object* wcp_to_json_obj(guint8 wcp)
{
    if (wcp >= CTRL_DRIVE_WCP_INVALID) {
        return NULL;
    }

    return json_object_new_string(sml_ctrl_drive_wcp2str(wcp));
}

LOCAL void get_drive_write_cache_policy(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object *oem_obj)
{
    guint8 type_id = 0;
    (void)dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_TYPE_ID, &type_id);
    if (dal_test_controller_vendor(type_id, VENDER_PMC) == FALSE) {
        json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_WRITE_CACHE_POLICY, NULL);
        return;
    }
    guint32 ctrl_opt = 0;

    gint32 ret = dal_get_property_value_uint32(controller_handle, PROPERTY_RAID_CONTROLLER_CTRL_OPTION3, &ctrl_opt);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get CtrlOption2 failed, ret = %d", __FUNCTION__, ret);
        return;
    }

    SML_CTRL_OPTION3_S ctrl_opt_t = (SML_CTRL_OPTION3_S)ctrl_opt;
    SML_CTRL_OPTION3_S *opt3 = &ctrl_opt_t;

    json_object *prop_obj = json_object_new_object();
    if (prop_obj == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return;
    }

    json_object_object_add(prop_obj, RFPROP_STORAGE_CONTROLLER_CONFIG_DRIVE,
        wcp_to_json_obj(opt3->opt3_div.configured_drive_wcp));
    json_object_object_add(prop_obj, RFPROP_STORAGE_CONTROLLER_UNCONFIG_DRIVE,
        wcp_to_json_obj(opt3->opt3_div.unconfigured_drive_wcp));
    json_object_object_add(prop_obj, RFPROP_STORAGE_CONTROLLER_HBA_DRIVE,
        wcp_to_json_obj(opt3->opt3_div.hba_drive_wcp));

    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_WRITE_CACHE_POLICY, prop_obj);
    return;
}

LOCAL void get_storage_controller_pmc_properties(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object *oem_obj)
{
    gint32 ret;
    json_object *prop_obj = NULL;
    ret = get_storage_controller_pcie_link_width(controller_handle, o_message_jso, &prop_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s, get controller pcie link width fail.", __FUNCTION__);
    }
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_PCIE_LINK_WIDTH, prop_obj);
    prop_obj = NULL;

    ret = get_storage_controller_read_cache_percent(controller_handle, o_message_jso, &prop_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s, get controller read cache percent fail.", __FUNCTION__);
    }
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_READ_CACHE_PERCENT, prop_obj);
    prop_obj = NULL;

    ret = get_storage_controller_spare_activation_mode(controller_handle, o_message_jso, &prop_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s, get controller spare activation mode fail.", __FUNCTION__);
    }
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_SPARE_ACTIVATION_MODE, prop_obj);
    prop_obj = NULL;

    ret = get_storage_controller_no_battery_write_cache(controller_handle, o_message_jso, &prop_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s, get controller no battery write Cache fail.", __FUNCTION__);
    }
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_NO_BATTERY_WRITE_CACHE, prop_obj);
    prop_obj = NULL;

    get_storage_controller_boot_devices(controller_handle, o_message_jso, oem_obj);
    get_drive_write_cache_policy(controller_handle, o_message_jso, oem_obj);
    return;
}

LOCAL void get_storage_controller_vid_did(OBJ_HANDLE controller_handle, json_object *oem_obj)
{
    gint32 ret = get_storage_controller_vid_did_inner(controller_handle, oem_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s: get controller device id, vender id fail.", __FUNCTION__);
        json_object_object_add(oem_obj, PROPERTY_DEVICE_OEM_VID, NULL);
        json_object_object_add(oem_obj, PROPERTY_DEVICE_OEM_DID, NULL);
        json_object_object_add(oem_obj, PROPERTY_DEVICE_OEM_SUB_VID, NULL);
        json_object_object_add(oem_obj, PROPERTY_DEVICE_OEM_SUB_DID, NULL);
    }
}


LOCAL void get_storage_controller_config(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object *oem_obj)
{
    json_object *prop_obj = NULL;
    gint32 ret = get_storage_controller_copyback(controller_handle, o_message_jso, &prop_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get controller copyback fail, return: %d", __FUNCTION__, ret);
    }
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_COPYBACK, prop_obj);

    prop_obj = NULL;
    ret = get_storage_controller_smarter_copyback(controller_handle, o_message_jso, &prop_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get controller smarter copyback fail, return: %d", __FUNCTION__, ret);
    }
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_SMART_COPYBACK, prop_obj);

    prop_obj = NULL;
    ret = get_storage_controller_jbod(controller_handle, o_message_jso, &prop_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get controller jbod switch fail, return: %d", __FUNCTION__, ret);
    }
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_JBOD, prop_obj);

    ret = get_storage_controller_consis_check(controller_handle, oem_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get controller consistency check information fail, return: %d", __FUNCTION__, ret);
    }
    return;
}



LOCAL gint32 get_storage_controller_oem(OBJ_HANDLE controller_handle, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *prop_obj = NULL;

    if (controller_handle == 0) {
        debug_log(DLOG_ERROR, "%s, %d: Invalid controller handle", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object *oem_obj = json_object_new_object();
    return_val_do_info_if_fail(NULL != oem_obj, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: new json object fail.\n", __FUNCTION__, __LINE__));
    
    gint32 ret = get_storage_controller_support_raidlevel(controller_handle, &prop_obj);
    do_val_if_expr(VOS_OK != ret, debug_log(DLOG_MASS, "get controller support raid level fail.\n"));
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_SUPPORT_RAIDLEVEL, prop_obj);
    prop_obj = NULL;
    

    __get_storage_controller_support_mode(controller_handle, oem_obj);

    
    get_storage_is_support_volume(controller_handle, oem_obj);
    get_storage_policy(controller_handle, oem_obj);
    

    
    ret = get_storage_controller_refcard(controller_handle, &prop_obj);
    do_val_if_expr(VOS_OK != ret, debug_log(DLOG_MASS, "get controller raid card fail.\n"));
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_ASSOCIATED_CARD, prop_obj);
    prop_obj = NULL;
    

    
    ret = get_storage_controller_mode(controller_handle, o_message_jso, &prop_obj);
    do_val_if_expr(VOS_OK != ret, debug_log(DLOG_MASS, "get controller sasaddress fail.\n"));
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_MODE, prop_obj);
    prop_obj = NULL;

    ret = get_storage_controller_cache_pinned_state(controller_handle, o_message_jso, &prop_obj);
    do_val_if_expr(VOS_OK != ret, debug_log(DLOG_MASS, "get controller sasaddress fail.\n"));
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_CACHE_PINNED, prop_obj);
    prop_obj = NULL;
    

    
    get_storage_controller_type(controller_handle, oem_obj);
    
    get_storage_controller_jbod_state_supported(controller_handle, oem_obj);
    get_storage_controller_epd_supported(controller_handle, oem_obj);
    ret = get_storage_controller_sasaddress(controller_handle, o_message_jso, &prop_obj);
    do_val_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s, %d: get controller sasaddress fail.\n", __FUNCTION__, __LINE__));
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_SASADDRESS, prop_obj);
    prop_obj = NULL;

    ret = get_storage_controller_configuration_version(controller_handle, o_message_jso, &prop_obj);
    do_val_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s, %d: get controller configuration version fail.\n", __FUNCTION__, __LINE__));
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_CFG_VERSION, prop_obj);
    prop_obj = NULL;

    get_storage_controller_pmc_properties(controller_handle, o_message_jso, oem_obj);

    ret = get_storage_controller_memory_size(controller_handle, o_message_jso, &prop_obj);
    do_val_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s, %d: get controller memory size fail.\n", __FUNCTION__, __LINE__));
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_MEMORY_SIZE, prop_obj);
    prop_obj = NULL;

    ret = get_storage_controller_fault_history(controller_handle, o_message_jso, &prop_obj);
    do_val_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s, %d: get controller fault history fail.\n", __FUNCTION__, __LINE__));
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_FAULT_HISTORY, prop_obj);
    prop_obj = NULL;

    get_storage_controller_config(controller_handle, o_message_jso, oem_obj);

    
    ret = get_storage_controller_out_of_band_management(controller_handle, o_message_jso, &prop_obj);
    do_val_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s, %d: get controller oobsupport switch fail.\n", __FUNCTION__, __LINE__));
    json_object_object_add(oem_obj, RFPROP_STORAGE_OOB_SUPPORT, prop_obj);
    prop_obj = NULL;

    ret = get_storage_controller_capacitance_name(controller_handle, o_message_jso, &prop_obj);
    do_val_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s, %d: get controller capacitance name fail.\n", __FUNCTION__, __LINE__));
    json_object_object_add(oem_obj, RFPROP_STORAGE_BBU_NAME, prop_obj);
    prop_obj = NULL;
    

    ret = get_storage_controller_capacitance_status(controller_handle, o_message_jso, &prop_obj);
    do_val_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s, %d: get controller capacitance status fail.\n", __FUNCTION__, __LINE__));
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_CAPACITANCE_STATUS, prop_obj);
    prop_obj = NULL;

    ret = get_storage_controller_driveinfo(controller_handle, o_message_jso, &prop_obj);
    do_val_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s, %d: get controller driveinfo fail.\n", __FUNCTION__, __LINE__));
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_DRIVER_INFO, prop_obj);
    prop_obj = NULL;

    
    ret = get_storage_controller_ddr_ecc_count(controller_handle, &prop_obj);
    do_val_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s, %d: get controller ddr ecc count fail.\n", __FUNCTION__, __LINE__));
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_DDRECCCOUNT, prop_obj);
    prop_obj = NULL;

    
    
    ret = get_storage_controller_stripsize(controller_handle, PROPERTY_RAID_CONTROLLER_MIN_STRIP_SUPPORT, &prop_obj);
    do_val_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s, %d: get controller min strip size fail.\n", __FUNCTION__, __LINE__));
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_MIN_STRIPE, prop_obj);
    prop_obj = NULL;

    
    ret = get_storage_controller_stripsize(controller_handle, PROPERTY_RAID_CONTROLLER_MAX_STRIP_SUPPORT, &prop_obj);
    do_val_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s, %d: get controller max strip size fail.\n", __FUNCTION__, __LINE__));
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_MAX_STRIPE, prop_obj);
    prop_obj = NULL;

    
    ret = get_storage_controller_phystatus(controller_handle, &prop_obj);
    do_val_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s, %d: get controller ddr ecc count fail.\n", __FUNCTION__, __LINE__));
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_PHYSTATUS, prop_obj);
    
    prop_obj = NULL;
    ret = get_storage_controller_bdf(controller_handle, &prop_obj);
    do_val_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s, %d: get controller ddr ecc count fail.\n", __FUNCTION__, __LINE__));
    if (VOS_OK != ret) {
        json_object_object_add(oem_obj, PROPERTY_DEVICE_OEM_BDF, NULL);
    } else {
        json_object_object_add(oem_obj, PROPERTY_DEVICE_OEM_BDF, prop_obj);
    }

    
    get_storage_controller_vid_did(controller_handle, oem_obj);

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, VOS_ERR, json_object_put(oem_obj);
        debug_log(DLOG_ERROR, "%s, %d: new json object fail.\n", __FUNCTION__, __LINE__));

    (void)json_object_object_add(*o_result_jso, RFPROP_COMMON_MANUFACTURER, oem_obj);

    return HTTP_OK;
}

LOCAL gint32 get_storage_driver_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    
    if (NULL == o_result_jso || NULL == o_message_jso || RET_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }
    (void)pthread_mutex_lock(&g_dirves_count_lock);
    *o_result_jso = json_object_new_int(g_drives_count);
    (void)pthread_mutex_unlock(&g_dirves_count_lock);

    return HTTP_OK;
}


LOCAL void redfish_storage_get_related_drive_info(OBJ_HANDLE dri_obj_handle, json_object *result_jso,
    json_object *array_info_jso)
{
    guint32 capacity_value = 0;
    guint64 capacity_bytes = 0;
    gchar drives_id[MAX_STRBUF_LEN + 1] = {0};
    gchar str_buf[MAX_STRBUF_LEN + 1] = {0};
    json_object *json_dri_name = NULL;
    json_object *json_oem = NULL;
    json_object *json_huawei = NULL;

    return_if_expr(result_jso == NULL || dri_obj_handle == 0);

    
    get_drive_id(dri_obj_handle, drives_id, MAX_STRBUF_LEN + 1);
    json_object_object_add(result_jso, RFPROP_COMMON_ID, json_object_new_string((const gchar *)drives_id));

    
    (void)redfish_system_storage_drive_string(PROPERTY_HDD_DEVICENAME, dri_obj_handle, &json_dri_name);
    json_object_object_add(result_jso, RFPROP_COMMON_NAME, json_dri_name);

    
    gint32 ret = get_drive_mediatype(dri_obj_handle, str_buf, MAX_STRBUF_LEN + 1);
    if (ret == RET_OK) {
        json_object_object_add(result_jso, PROPERTY_HDD_MEDIA_TYPE, json_object_new_string((const gchar *)str_buf));
    } else {
        json_object_object_add(result_jso, PROPERTY_HDD_MEDIA_TYPE, NULL);
    }

    
    if (dal_get_property_value_uint32(dri_obj_handle, PROPERTY_HDD_CAPACITY_MB, &capacity_value) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: get device capacity bytes fail.\n", __FUNCTION__, __LINE__);
        json_object_object_add(result_jso, RFPROP_SYSTEM_STORAGE_DRIVE_CAPACITYBYTES, NULL);
    } else {
        capacity_bytes = (guint64)capacity_value * MULTIPLE_MB_TO_BYTES;
        json_object_object_add(result_jso, RFPROP_SYSTEM_STORAGE_DRIVE_CAPACITYBYTES,
            json_object_new_int64(capacity_bytes));
    }

    json_oem = json_object_new_object();
    if (json_oem == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: new object oem fail.\n", __FUNCTION__, __LINE__);
        return;
    }
    json_huawei = json_object_new_object();
    if (json_huawei == NULL) {
        json_object_put(json_oem);
        debug_log(DLOG_ERROR, "%s, %d: new object fail.\n", __FUNCTION__, __LINE__);
        return;
    }

    
    ret = get_drive_oem_driveid(dri_obj_handle, json_huawei);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: get drive id fail.\n", __FUNCTION__, __LINE__);
        json_object_object_add(json_huawei, RFPROP_DRIVE_DRIVEID, NULL);
    }

    
    get_drive_oem_physicaldiskstate(dri_obj_handle, json_huawei);
    
    redfish_storage_get_related_array_info(dri_obj_handle, json_huawei, array_info_jso);

    json_object_object_add(json_oem, RFPROP_OEM_HUAWEI, json_huawei);
    json_object_object_add(result_jso, RFPROP_OEM, json_oem);

    return;
}


LOCAL void redfish_storage_parase_related_array_member(const gchar *ctrl_obj_name, const guint8 *slot_array_pointer,
    const guint16 *enclosure_array_pointer, gsize len_hdd_array_pointer, json_object *array_jso)
{
    json_object *member_jso = NULL;
    guint8 drive_id = 0;
    OBJ_HANDLE drive_obj_handle = 0;
    gint32 ret = 0;
    gchar device_name[STORAGE_CTRL_NAME_LENGTH] = {0};

    if (ctrl_obj_name == NULL || slot_array_pointer == NULL || enclosure_array_pointer == NULL || array_jso == NULL) {
        return;
    }
    member_jso = json_object_new_array();
    if (NULL == member_jso) {
        debug_log(DLOG_ERROR, "%s:new array fail", __FUNCTION__);
        return;
    }

    for (gsize i = 0; i < len_hdd_array_pointer; i++) {
        dal_get_pd_id_by_slot_and_enclosure(ctrl_obj_name, slot_array_pointer[i], enclosure_array_pointer[i],
            &drive_id);
        (void)dal_get_specific_object_byte(CLASS_HDD, PROPERTY_HDD_ID, drive_id, &drive_obj_handle);
        ret = dal_get_property_value_string(drive_obj_handle, PROPERTY_HDD_DEVICENAME, device_name,
            STORAGE_CTRL_NAME_LENGTH);
        if (RET_OK == ret) {
            json_object_array_add(member_jso, json_object_new_string((const gchar *)device_name));
        }
    }

    json_object_object_add(array_jso, RFPROP_MEMBERS, member_jso);
    return;
}

LOCAL gint32 redfish_storage_get_related_array_object(OBJ_HANDLE dri_obj_handle, json_object *array_jso,
    OBJ_HANDLE *ld_obj_handle, json_object *array_info_jso)
{
    gint32 logical_jso_len;
    gint32 member_jso_len = 0;
    gint32 i;
    gint32 idx = 0;
    gchar drive_device_name[STORAGE_CTRL_NAME_LENGTH] = {0};
    const gchar *member_device_name = NULL;
    json_object *prop_jso = NULL;
    json_object *member_prop_jso = NULL;
    json_object *drive_array_jso = NULL;
    json_object *logical_handle_json = NULL;
    json_bool ret_json = FALSE;

    return_val_if_expr(array_jso == NULL || dri_obj_handle == 0 || array_info_jso == NULL, RET_ERR);

    gint32 ret = dal_get_property_value_string(dri_obj_handle, PROPERTY_HDD_DEVICENAME, drive_device_name,
        STORAGE_CTRL_NAME_LENGTH);
    return_val_do_info_if_fail(ret == RET_OK, RET_ERR,
        debug_log(DLOG_ERROR, "%s: ger device name for drive fail ", __FUNCTION__));

    logical_jso_len = json_object_array_length(array_info_jso);
    return_val_do_info_if_expr(logical_jso_len <= 0, RET_ERR,
        debug_log(DLOG_DEBUG, "%s:lenth of logical_jso is zero", __FUNCTION__));

    for (i = 0; i < logical_jso_len; i++) {
        prop_jso = json_object_array_get_idx(array_info_jso, i);
        ret_json = json_object_object_get_ex(prop_jso, RFPROP_MEMBERS, &drive_array_jso);
        continue_do_info_if_fail(TRUE == ret_json, debug_log(DLOG_ERROR, "%s: get members object fail", __FUNCTION__));

        member_jso_len = json_object_array_length(drive_array_jso);
        continue_do_info_if_expr(member_jso_len <= 0,
            debug_log(DLOG_DEBUG, "%s:lenth of members object is zero, i = %d", __FUNCTION__, i));

        for (idx = 0; idx < member_jso_len; idx++) {
            member_prop_jso = json_object_array_get_idx(drive_array_jso, idx);
            member_device_name = dal_json_object_get_str(member_prop_jso);
            if (strcmp(drive_device_name, member_device_name) == 0) {
                json_object_object_add(array_jso, RFPROP_MEMBERS, json_object_get(drive_array_jso));
                ret_json = json_object_object_get_ex(prop_jso, LOGICAL_DRIVE_OBJ_HANDLE, &logical_handle_json);
                continue_do_info_if_fail(TRUE == ret_json,
                    debug_log(DLOG_ERROR, "%s: get logical object fail", __FUNCTION__));
                *ld_obj_handle = (OBJ_HANDLE)json_object_get_int(logical_handle_json);

                return RET_OK;
            }
        }
    }
    return RET_ERR;
}


void get_free_blocks_space(OBJ_HANDLE array_obj_handle, guint8 span_depth, json_object *array_jso)
{
    GVariant *free_block_gv = NULL;
    const guint32 *free_block_au = NULL;
    gsize free_count = 0;
    gsize idx = 0;
    json_object *free_block_jso = 0;

    free_block_jso = json_object_new_array();
    if (free_block_jso == NULL) {
        json_object_object_add(array_jso, RFPROP_STORAGE_CONTROLLER_FREE_BLOCKS_SPACE, NULL);
        return;
    }

    gint32 ret_val = dfl_get_property_value(array_obj_handle, PROPERTY_DISK_ARRAY_FREE_BLOCKS_SPACE, &free_block_gv);
    if (ret_val == DFL_OK) {
        free_block_au = (const guint32 *)g_variant_get_fixed_array(free_block_gv, &free_count, sizeof(guint32));
        if (free_block_au != NULL) {
            for (idx = 0; idx < free_count; idx++) {
                if (free_block_au[idx] != STORAGE_INFO_INVALID_DWORD) {
                    json_object_array_add(free_block_jso,
                        json_object_new_int64((int64_t)span_depth * free_block_au[idx]));
                }
            }
        }
        g_variant_unref(free_block_gv);
    }
    json_object_object_add(array_jso, RFPROP_STORAGE_CONTROLLER_FREE_BLOCKS_SPACE, free_block_jso);

    return;
}


LOCAL gint32 recalculate_pd_count(guint8 array_pd_count, guint8 raid_level, guint8 *o_pd_count)
{
    guint8 inner_pd_count = 0;

    if (o_pd_count == NULL) {
        return RET_ERR;
    }

    switch (raid_level) {
        case RAID_LEVEL_1:
        case RAID_LEVEL_10:
            // 必须是2的倍数
            if (array_pd_count % 2 != 0) {
                return RET_ERR;
            }
            inner_pd_count = array_pd_count / 2;
            break;

        case RAID_LEVEL_5:
        case RAID_LEVEL_50:
            // 每个SPAN是RAID5，至少3块硬盘
            if (array_pd_count < 3) {
                return RET_ERR;
            }
            inner_pd_count = array_pd_count - 1;
            break;

        case RAID_LEVEL_6:
        case RAID_LEVEL_60:
            // 每个SPAN是RAID6，至少3块硬盘
            if (array_pd_count < 3) {
                return RET_ERR;
            }
            inner_pd_count = array_pd_count - 2;
            break;

        case RAID_LEVEL_1ADM:
        case RAID_LEVEL_1TRIPLE:
        case RAID_LEVEL_10ADM:
        case RAID_LEVEL_10TRIPLE:
            // 必须是3的倍数
            if (array_pd_count % 3 != 0) {
                return RET_ERR;
            }
            // 必须是3的倍数
            inner_pd_count = array_pd_count / 3;
            break;

        default:
            inner_pd_count = array_pd_count;
            break;
    }

    *o_pd_count = inner_pd_count;

    return RET_OK;
}


LOCAL gint32 get_ld_ref_first_array(OBJ_HANDLE ld_obj_handle, guint16 first_array_id, OBJ_HANDLE *array_obj_handle)
{
    guint16 array_id = 0;
    gchar       array_ref_raid_name[MAX_NAME_SIZE] = {0};
    gchar       ld_ref_raid_name[MAX_NAME_SIZE] = {0};
    gint32 ret_code = RET_ERR;
    OBJ_HANDLE obj_handle = 0;
    GSList *obj_list = NULL;
    GSList *node = NULL;

    if (array_obj_handle == NULL) {
        return ret_code;
    }

    (void)dal_get_property_value_string(ld_obj_handle, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER, ld_ref_raid_name,
        sizeof(ld_ref_raid_name));

    gint32 ret_val = dfl_get_object_list(CLASS_DISK_ARRAY_NAME, &obj_list);
    if (ret_val != DFL_OK || obj_list == NULL) {
        return ret_code;
    }

    for (node = obj_list; node; node = g_slist_next(node)) {
        obj_handle = POINTER_TO_INT32(node->data);
        ret_val = dal_get_property_value_uint16(obj_handle, PROPERTY_DISK_ARRAY_ID, &array_id);
        ret_val += dal_get_property_value_string(obj_handle, PROPERTY_DISK_ARRAY_REF_RAID_CONTROLLER,
            array_ref_raid_name, sizeof(array_ref_raid_name));
        if (ret_val != RET_OK) {
            continue;
        }

        if (first_array_id == array_id && g_strcmp0(ld_ref_raid_name, array_ref_raid_name) == 0) {
            *array_obj_handle = obj_handle;
            ret_code = RET_OK;
            break;
        }
    }
    g_slist_free(obj_list);

    return ret_code;
}


LOCAL void get_array_free_space(OBJ_HANDLE array_obj_handle, OBJ_HANDLE ld_obj_handle, json_object *array_jso,
    guint8 span_depth)
{
    guint32 free_space = 0;
    guint8 array_pd_count = 0;

    gint32 ret = dal_get_property_value_uint32(array_obj_handle, PROPERTY_DISK_ARRAY_TOTAL_FREE_SPACE, &free_space);
    (void)dal_get_property_value_byte(array_obj_handle, PROPERTY_DISK_ARRAY_PD_COUNT, &array_pd_count);
    if (ret != RET_OK || array_pd_count == 0) {
        debug_log(DLOG_DEBUG, "%s: get free space of array fail or pd count is 0.", __FUNCTION__);
        json_object_object_add(array_jso, RFPROP_STORAGE_CONTROLLER_ARRAY_DRIVE_FREE_SPACE, NULL);
        json_object_object_add(array_jso, RFPROP_STORAGE_CONTROLLER_TOTAL_FREE_SPACE, NULL);
    } else {
        json_object *free_space_drive = NULL;
        json_object *total_free_space = NULL;
        const gchar *array_obj_name = dfl_get_object_name(array_obj_handle);
        if (strstr(array_obj_name, DYNAMIC_OBJ_DISK_ARRAY)) {
            // 该分支是BMA创建的Array对象， BMA 目前没有总剩余容量的接口
            free_space_drive = json_object_new_int(free_space / array_pd_count);
        } else {
            guint8 raid_level = 0;
            (void)dal_get_property_value_byte(ld_obj_handle, PROPERTY_LOGICAL_DRIVE_RAID_LEVEL, &raid_level);
            ret = recalculate_pd_count(array_pd_count, raid_level, &array_pd_count);
            if (ret == RET_OK && array_pd_count != 0) {
                free_space_drive = json_object_new_int(free_space / array_pd_count);
            } else {
                debug_log(DLOG_DEBUG, "%s call recalculate_pd_count failed(ret:%d) or array_pd_count=0",
                    __FUNCTION__, ret);
            }
            total_free_space = json_object_new_int64((int64_t)free_space * span_depth);
        }
        json_object_object_add(array_jso, RFPROP_STORAGE_CONTROLLER_ARRAY_DRIVE_FREE_SPACE, free_space_drive);
        json_object_object_add(array_jso, RFPROP_STORAGE_CONTROLLER_TOTAL_FREE_SPACE, total_free_space);
    }
    return;
}


void redfish_storage_get_related_array_info(OBJ_HANDLE dri_obj_handle, json_object *result_jso,
    json_object *array_info_jso)
{
    OBJ_HANDLE ld_obj_handle = 0;
    json_object *array_jso = NULL;
    GVariant *ld_ref_array_list = NULL;
    const guint16 *ld_array_list = NULL;
    gsize array_num = 0;
    OBJ_HANDLE array_obj_handle = 0;
    guint8 drive_num_per_span = 0;
    guint8 span_depth = 0;

    array_jso = json_object_new_object();
    if (NULL == array_jso) {
        debug_log(DLOG_ERROR, "%s, %d: new array object fail.", __FUNCTION__, __LINE__);
        return;
    }
    // 返回关联的第一个逻辑盘的句柄
    gint32 ret = redfish_storage_get_related_array_object(dri_obj_handle, array_jso, &ld_obj_handle, array_info_jso);
    // 如果没有找到关联的逻辑盘，则填充NULL
    if (RET_OK != ret || 0 == ld_obj_handle) {
        json_object_put(array_jso);
        json_object_object_add(result_jso, RFPROP_STORAGE_CONTROLLER_ARRAY, NULL);
        return;
    }

    gchar ref_controller[STORAGE_CTRL_INFO_LENGTH + 1] = {0};
    (void)dal_get_property_value_string(dri_obj_handle, PROPERTY_HDD_REF_RAID_CONTROLLER, ref_controller,
        sizeof(ref_controller));
    if (dal_check_ctrl_using_subarray(ref_controller)) {
        // 自研卡或PMC卡的RAID group，相对于DiskArray对象就只有一个子组
        span_depth = 1;
    } else {
        // 博通卡的RAID group，相对于DiskArray对象可能有1个或多个子组
        (void)dal_get_property_value_byte(ld_obj_handle, PROPERTY_LOGICAL_DRIVE_SPAN_DEPTH, &span_depth);
    }

    // 获取array 的每个物理盘剩余容量
    ret = dfl_get_property_value(ld_obj_handle, PROPERTY_LOGICAL_DRIVE_REF_ARRAY, &ld_ref_array_list);
    ld_array_list = (const guint16 *)g_variant_get_fixed_array(ld_ref_array_list, &array_num, sizeof(guint16));
    if (NULL != ld_array_list) {
        
        ret = get_ld_ref_first_array(ld_obj_handle, ld_array_list[0], &array_obj_handle);
        if (ret != RET_OK) {
            g_variant_unref(ld_ref_array_list);
            json_object_put(array_jso);
            json_object_object_add(result_jso, RFPROP_STORAGE_CONTROLLER_ARRAY, NULL);
            return;
        }
        get_array_free_space(array_obj_handle, ld_obj_handle, array_jso, span_depth);
        g_variant_unref(ld_ref_array_list);
    } else {
        if (ret == DFL_OK) {
            g_variant_unref(ld_ref_array_list);
        }
        json_object_object_add(array_jso, RFPROP_STORAGE_CONTROLLER_ARRAY_DRIVE_FREE_SPACE, NULL);
        json_object_object_add(array_jso, RFPROP_STORAGE_CONTROLLER_TOTAL_FREE_SPACE, NULL);
    }
    get_free_blocks_space(array_obj_handle, span_depth, array_jso);

    // 获取span中的硬盘个数
    ret = dal_get_property_value_byte(ld_obj_handle, PROPERTY_LOGICAL_DRIVE_NUMDRIVE_PER_SPAN, &drive_num_per_span);
    if (0xff != drive_num_per_span && RET_OK == ret) {
        json_object_object_add(array_jso, PROPERTY_LOGICAL_DRIVE_NUMDRIVE_PER_SPAN,
            json_object_new_int(drive_num_per_span));
    } else {
        debug_log(DLOG_ERROR, "%s, %d: get number of per span fail, ret=%d.", __FUNCTION__, __LINE__, ret);
        json_object_object_add(array_jso, PROPERTY_LOGICAL_DRIVE_NUMDRIVE_PER_SPAN, NULL);
    }
    // 获取raid级别
    get_oem_raid_level(ld_obj_handle, array_jso);
    json_object_object_add(result_jso, RFPROP_STORAGE_CONTROLLER_ARRAY, array_jso);

    return;
}


LOCAL void get_controller_drives_odata_id(OBJ_HANDLE obj_handle, json_object **o_result_jso)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *obj_json = NULL;
    json_object *prop_json = NULL;
    guchar hdd_presence = 0;
    gchar  driver_id[STORAGE_CTRL_INFO_LENGTH] = {0};
    gchar  driver_info[STORAGE_CTRL_NAME_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar controller_name[MAX_RSC_NAME_LEN] = {0};
    const gchar *object_name = NULL;
    json_object *array_info_jso = NULL;
    gint32 ctr_drives_count = 0;

    gint32 ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s:%d fail", __FUNCTION__, __LINE__);
        return;
    }

    
    ret = dfl_get_object_list(CLASS_HDD_NAME, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_MASS, "%s, %d: ret = %d fail.\n", __FUNCTION__, __LINE__, ret);
    }

    object_name = dfl_get_object_name(obj_handle);
    if (NULL == object_name) {
        debug_log(DLOG_MASS, "%s, %d: get controller name fail.\n", __FUNCTION__, __LINE__);
        g_slist_free(obj_list);
        return;
    }

    array_info_jso = json_object_new_array();
    if (NULL == array_info_jso) {
        debug_log(DLOG_ERROR, "%s, %d: new logical object fail.\n", __FUNCTION__, __LINE__);
        g_slist_free(obj_list);
        return;
    }

    ret = get_storage_logical_rsc(obj_handle, array_info_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s, %d: get logiccal drive error.\n", __FUNCTION__, __LINE__);
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next, prop_json = NULL) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_PRESENCE, &hdd_presence);
        if (RET_OK != ret || 0 == hdd_presence) {
            continue;
        }

        
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_REF_RAID_CONTROLLER,
            controller_name, sizeof(controller_name));

        if (0 == g_strcmp0(controller_name, object_name)) {
            ctr_drives_count++;
            
            (void)memset_s(driver_id, sizeof(driver_id), 0, sizeof(driver_id));

            ret = redfish_get_drives_location_devicename((OBJ_HANDLE)obj_node->data, driver_id, sizeof(driver_id));
            if (ret != RET_OK) {
                continue;
            }

            obj_json = NULL;
            
            obj_json = json_object_new_object();
            if (NULL == obj_json) {
                continue;
            }

            (void)memset_s(driver_info, sizeof(driver_info), 0, sizeof(driver_info));
            
            ret = snprintf_s(driver_info, sizeof(driver_info), sizeof(driver_info) - 1,
                URI_FORMAT_CHASSIS_STORAGE_DRIVES, slot, driver_id);
            if (0 >= ret) {
                debug_log(DLOG_ERROR, "%s %d: snprintf_s fail.", __FUNCTION__, __LINE__);
                json_object_put(obj_json);
                continue;
            }
            
            prop_json = json_object_new_string((const gchar*)driver_info);
            if (NULL == prop_json) {
                json_object_put(obj_json);
                continue;
            }
            (void)json_object_object_add(obj_json, RFPROP_ODATA_ID, prop_json);
            
            redfish_storage_get_related_drive_info((OBJ_HANDLE)obj_node->data, obj_json, array_info_jso);
            
            
            ret = json_object_array_add(*o_result_jso, obj_json);
            if (ret != 0) {
                json_object_put(obj_json);
                debug_log(DLOG_ERROR, "%s, %d: json object array add fail.", __FUNCTION__, __LINE__);
            }
        }
    }
    (void)pthread_mutex_lock(&g_dirves_count_lock);
    g_drives_count = ctr_drives_count;
    (void)pthread_mutex_unlock(&g_dirves_count_lock);
    if (NULL != obj_list) {
        g_slist_free(obj_list);
    }
    json_object_put(array_info_jso);
    return;
}

LOCAL gint32 get_storage_drivers(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || RET_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    *o_result_jso = json_object_new_array();
    if (NULL == *o_result_jso) {
        debug_log(DLOG_MASS, "%s, %d: new object array fail.\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)get_controller_drives_odata_id(i_paras->obj_handle, o_result_jso);
    return HTTP_OK;
}

LOCAL gint32 get_storage_controller_model(OBJ_HANDLE controller_handle, json_object** o_result_jso)
{
    gint32  ret;
    gboolean is_customized;
    gboolean is_soft_raid = FALSE;
    gchar  prop_info[STORAGE_CTRL_INFO_LENGTH] = {0};
    gchar  mode[STORAGE_CTRL_INFO_LENGTH] = {0};

    is_customized = dal_is_customized_by_cmcc();
    guint8 type_id = 0;
    (void)dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_TYPE_ID, &type_id);
    if (type_id == SM_SOFT_RAID_CONTROLLER_TYPE_ID) { 
        is_soft_raid = TRUE;
    }
    if (is_customized) {
        ret = dal_get_property_value_string(controller_handle, PROPERTY_RAID_CONTROLLER_TYPE,
            prop_info, sizeof(prop_info));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get property failed", __FUNCTION__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        ret = sprintf_s(mode, sizeof(mode), "RAID_%s", prop_info);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: sprintf_s failed, ret(%d)", __FUNCTION__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        *o_result_jso = json_object_new_string(mode);
    } else if (is_soft_raid) {
        ret = get_storage_controller_property_string(controller_handle, PROPERTY_RAID_CONTROLLER_TYPE, o_result_jso);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get raid controller type failed", __FUNCTION__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    } else {
        ret = get_storage_controller_property_string(controller_handle, PROPERTY_RAID_CONTROLLER_NAME, o_result_jso);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get raid controller name failed", __FUNCTION__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }
    return HTTP_OK;
}


LOCAL void process_storage_cmcc(OBJ_HANDLE raid, json_object *controller_obj)
{
    json_object *prop_obj = NULL;
    struct {
        gchar *refpop;
        gchar *pcie_prop;
        gchar *raid_prop;
    } map[] = {
        {RFPROP_STORAGE_CONTROLLER_CARD_MANUFACTURER,
            PROPETRY_PCIECARD_MANUFACTURER, PROPERTY_RAID_CONTROLLER_MANUFACTURER},
        {RFPROP_STORAGE_CONTROLLER_CARD_MODEL, PROPERTY_PCIE_CARD_SLAVE_CARD_NAME, PROPERTY_RAID_CONTROLLER_NAME}
    };
    OBJ_HANDLE pcie_card = 0;
    gint32 ret = dal_get_specific_object_position(raid, CLASS_PCIE_CARD, &pcie_card);
    gboolean option = (ret == RET_OK);
    for (guint8 i = 0; i < ARRAY_SIZE(map); i++) {
        ret = RET_ERR;
        if (option) {
            ret = get_storage_controller_property_string(pcie_card, map[i].pcie_prop, &prop_obj);
        }
        if (ret != RET_OK) {
            ret = get_storage_controller_property_string(raid, map[i].raid_prop, &prop_obj);
        }
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "get controller %s fail", map[i].refpop);
        }
        json_object_object_add(controller_obj, map[i].refpop, prop_obj);
        prop_obj = NULL;
    }
}


LOCAL gint32 check_parameter(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
     
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }
    return HTTP_OK;
}


LOCAL void get_controller_other_property(OBJ_HANDLE raid_controller,
    json_object *controller_obj, json_object **o_message_jso)
{
    json_object *prop_obj = NULL;
    gint32 ret = get_storage_controller_property_string(raid_controller,
        PROPERTY_RAID_CONTROLLER_MANUFACTURER, &prop_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s: get controller manufacturer fail", __FUNCTION__);
    }
    json_object_object_add(controller_obj, RFPROP_STORAGE_CONTROLLER_MANUFACTURER, prop_obj);
    prop_obj = NULL;

    
    ret = get_storage_controller_model(raid_controller, &prop_obj);
    if (ret != HTTP_OK) {
        debug_log(DLOG_MASS, "%s: get controller model fail", __FUNCTION__);
    }
    json_object_object_add(controller_obj, RFPROP_STORAGE_CONTROLLER_MODEL, prop_obj);
    prop_obj = NULL;

     
    if (dal_is_customized_by_cmcc()) {
        process_storage_cmcc(raid_controller, controller_obj);
    }

    
    ret = get_storage_controller_cachesummary(raid_controller, o_message_jso, &prop_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s: get controller cache summary fail", __FUNCTION__);
    }
    json_object_object_add(controller_obj, RFPROP_STORAGE_CONTROLLERS_CACHE_SUMMARY, prop_obj);
    prop_obj = NULL;

    // 获取支持的RAID等级
    ret = get_storage_controller_support_raidlevel(raid_controller, &prop_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s: get supported raid types fail", __FUNCTION__);
    }
    json_object_object_add(controller_obj, RFPROP_STORAGE_CONTROLLERS_SUPPORT_RAIDTYPES, prop_obj);
    prop_obj = NULL;

    
    ret = get_storage_controller_oem(raid_controller, o_message_jso, &prop_obj);
    if (ret != HTTP_OK) {
        debug_log(DLOG_MASS, "%s: get controller oem fail", __FUNCTION__);
    }
    json_object_object_add(controller_obj, RFPROP_COMMON_OEM, prop_obj);
}



LOCAL void get_controller_dynamic_property(OBJ_HANDLE raid_controller,
    json_object *controller_obj, json_object **o_message_jso)
{
    json_object *prop_obj = json_object_new_string(STORAGE_CONTROLLERS_ODATA_TYPE_VALUE);
    json_object_object_add(controller_obj, RFPROP_ODATA_TYPE, prop_obj);
    prop_obj = NULL;
    
    gint32 ret = get_storage_controller_name(raid_controller, o_message_jso, &prop_obj);
    if (ret != HTTP_OK) {
        debug_log(DLOG_MASS, "%s: get controller name fail", __FUNCTION__);
    }
    json_object_object_add(controller_obj, RFPROP_COMMON_NAME, prop_obj);
    prop_obj = NULL;

    
    ret = get_storage_controller_description(raid_controller, o_message_jso, &prop_obj);
    if (ret != HTTP_OK) {
        debug_log(DLOG_MASS, "%s: get controller description fail", __FUNCTION__);
    }
    json_object_object_add(controller_obj, RFPROP_STORAGE_CONTROLLER_DESCRIPTION, prop_obj);
    prop_obj = NULL;

    
    ret = get_storage_controller_status(raid_controller, o_message_jso, &prop_obj);
    if (ret != HTTP_OK) {
        debug_log(DLOG_MASS, "%s: get controller status fail", __FUNCTION__);
    }
    json_object_object_add(controller_obj, RFPROP_STORAGE_CONTROLLER_STATUS, prop_obj);
    prop_obj = NULL;

    
    ret = get_storage_controller_speedgbps(raid_controller, o_message_jso, &prop_obj);
    if (ret != HTTP_OK) {
        debug_log(DLOG_MASS, "%s: get controller speedgbps fail", __FUNCTION__);
    }
    json_object_object_add(controller_obj, RFPROP_STORAGE_CONTROLLER_SPEED_GBPS, prop_obj);
    prop_obj = NULL;

    
    ret = get_storage_controller_fwversion(raid_controller, o_message_jso, &prop_obj);
    if (ret != HTTP_OK) {
        debug_log(DLOG_MASS, "%s: get controller FirmwareVersion fail", __FUNCTION__);
    }
    json_object_object_add(controller_obj, RFPROP_STORAGE_CONTROLLER_FW_VERSION, prop_obj);
    prop_obj = NULL;

    
    ret = get_storage_controller_device_protocol(raid_controller, o_message_jso, &prop_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s: get controller device protocol fail", __FUNCTION__);
    }
    json_object_object_add(controller_obj, RFPROP_STORAGE_CONTROLLER_DEVICE_PROTOCOL, prop_obj);
    prop_obj = NULL;
    

    ret = get_storage_controller_property_string(raid_controller, PROPERTY_RAID_CONTROLLER_SERIALNUMBER, &prop_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s: get controller sn failed", __FUNCTION__);
    }
    json_object_object_add(controller_obj, RFPROP_STORAGE_CONTROLLER_SERIALNUMBER, prop_obj);

    get_controller_other_property(raid_controller, controller_obj, o_message_jso);
}


LOCAL gint32 get_storage_Controllers(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    guchar storage_controller_id = 0;
    gchar  prop_info[STORAGE_CTRL_INFO_LENGTH] = {0};
    json_object *controller_obj = NULL;
    json_object *prop_obj = NULL;
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    ret = check_parameter(i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_OK) {
        return ret;
    }

    
    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get storage controllers json object fail", __FUNCTION__));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_OK,
        debug_log(DLOG_MASS, "%s: get board slot fail", __FUNCTION__));

    controller_obj = json_object_new_object();
    return_val_do_info_if_fail(NULL != controller_obj, HTTP_OK,
        debug_log(DLOG_ERROR, "%s: json new obejct fail", __FUNCTION__));

    
    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_RAID_CONTROLLER_ID, &storage_controller_id);
    (void)snprintf_s(prop_info, STORAGE_CTRL_INFO_LENGTH, STORAGE_CTRL_INFO_LENGTH - 1, "%u", storage_controller_id);
    prop_obj = json_object_new_string((const gchar *)prop_info);
    json_object_object_add(controller_obj, RFPROP_STORAGE_CONTROLLER_ID, prop_obj);
    prop_obj = NULL;

    
    (void)memset_s(prop_info, STORAGE_CTRL_INFO_LENGTH, 0, STORAGE_CTRL_INFO_LENGTH);
    
    iRet = snprintf_s(prop_info, STORAGE_CTRL_INFO_LENGTH, STORAGE_CTRL_INFO_LENGTH - 1,
        STORAGE_CONTROLLERS_ODATA_ID_VALUE, slot, storage_controller_id, storage_controller_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    

    prop_obj = json_object_new_string((const gchar *)prop_info);
    do_val_if_expr(NULL == prop_obj, debug_log(DLOG_MASS, "%s: get controller odata_id fail", __FUNCTION__));
    json_object_object_add(controller_obj, RFPROP_ODATA_ID, prop_obj);

    
    get_controller_dynamic_property(i_paras->obj_handle, controller_obj, o_message_jso);

    ret = json_object_array_add(*o_result_jso, controller_obj);
    do_if_expr((0 != ret), json_object_put(controller_obj));

    return HTTP_OK;
}

LOCAL void check_set_storage_controller_method_ret(gint32 *ret, const gchar *prop, json_object *jso, const gchar *val)
{
    if (prop == NULL) {
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__);
        *ret = HTTP_BAD_REQUEST;
        return;
    }

    gint32 input_ret = *ret;
    if (input_ret == RET_OK) {
        *ret = HTTP_OK;
        return;
    }

    if (input_ret == SML_ERR_REBOOT_REQUIRED) {
        debug_log(DLOG_INFO, "%s: set raid controller succ, please restart OS.", __FUNCTION__);
        *ret = HTTP_OK;
        return;
    }

    RET_MSG_INFO_MAP set_ret_val_msg_map[] = {
        { RFERR_INSUFFICIENT_PRIVILEGE, HTTP_FORBIDDEN, MSGID_PROP_MODIFY_NEED_PRIV, prop, prop, NULL, NULL},
        { SML_ERR_CTRL_STATUS_INVALID, HTTP_BAD_REQUEST, MSGID_CURRENT_STATUS_NOT_SUPPORT, prop, NULL, NULL, NULL},
        { SML_ERR_CTRL_INIT_NOT_COMPLETED, HTTP_BAD_REQUEST, MSGID_CURRENT_STATUS_NOT_SUPPORT, prop, NULL, NULL, NULL},
        { SML_ERR_I2C_READ_WRITE_FAILED, HTTP_BAD_REQUEST, MSGID_CURRENT_STATUS_NOT_SUPPORT, prop, NULL, NULL, NULL},
        { SM_CODE_NOT_SUPPORT_IN_PRESENT_STATE, HTTP_BAD_REQUEST, MSGID_CURRENT_STATUS_NOT_SUPPORT, prop, NULL,
            NULL, NULL},
        { SML_ERR_CTRL_SET_PROP_UPLEVEL_FUNC_DISABLED, HTTP_BAD_REQUEST, MSGID_CURRENT_STATUS_NOT_SUPPORT, prop, NULL,
            NULL, NULL},
        { SM_CODE_GET_PD_LIST_FAILED, HTTP_BAD_REQUEST, MSGID_CURRENT_STATUS_NOT_SUPPORT, prop, NULL, NULL, NULL},
        { SML_ERR_CTRL_NO_EDITABLE_LD, HTTP_BAD_REQUEST, MSGID_CURRENT_STATUS_NOT_SUPPORT, prop, NULL, NULL, NULL},
        { SML_ERR_CTRL_OPERATION_NOT_SUPPORT, HTTP_BAD_REQUEST, MSGID_MODIFY_FAIL_CONTROLLER_NOT_SUPPORT, prop, prop,
            NULL, NULL},
        { SML_ERR_INVALID_PARAMETER, HTTP_BAD_REQUEST, MSGID_MODIFY_FAIL_CONTROLLER_NOT_SUPPORT, prop, prop,
            NULL, NULL},
        { SML_ERR_OPERATION_NOT_POSSIBLE, HTTP_BAD_REQUEST, MSGID_MODIFY_FAIL_CONTROLLER_NOT_SUPPORT, prop, prop,
            NULL, NULL},
        { SM_CODE_PD_NOT_IDENTIFIED, HTTP_BAD_REQUEST, MSGID_PROP_INVALID_VALUE, prop, val, prop, NULL},
        { SML_ERR_DATA_INVALID, HTTP_BAD_REQUEST, MSGID_PROP_INVALID_VALUE, prop, val, prop, NULL},
        { SML_ERR_CONFIG_PRESENT_ERROR, HTTP_BAD_REQUEST, MSGID_CONFIGURATION_EXIST, prop, NULL, NULL, NULL},
        { SM_CODE_PARA_DATA_ILLEGAL, HTTP_BAD_REQUEST, MSGID_PROP_NOT_IN_LIST, prop, val, prop, NULL},
        { SML_ERR_NULL_INFTERFACE, HTTP_BAD_REQUEST, MSGID_ACT_PARA_NOT_SUPPORTED, prop, val, prop, NULL},
        { SML_ERR_CTRL_BBU_STATUS_ABNORMAL, HTTP_BAD_REQUEST, MSGID_PROPERTY_BBU_STATE_NOT_SUPPORT, prop, NULL,
            NULL, NULL},
        { SML_ERR_CTRL_RCP_NOT_IN_RANGE, HTTP_BAD_REQUEST, MSGID_RCP_NOT_IN_RANGE, prop, prop, NULL, NULL},
        { SM_CODE_OPERATION_IN_PROGRESS, HTTP_BAD_REQUEST, VMM_MESSAGE_OPERATIONINPROCESS, NULL, NULL, NULL, NULL},
    };

    gsize map_size = sizeof(set_ret_val_msg_map) / sizeof(RET_MSG_INFO_MAP);

    *ret = get_ret_message(input_ret, set_ret_val_msg_map, map_size, jso);
    return;
}


LOCAL gint32 set_storage_controller_wcp_prop(guint8 wcp_type, json_object *obj, const gchar *json_pointer,
    PROVIDER_PARAS_S *i_paras, json_object *o_message_jso)
{
    GSList *input_list = NULL;
    const gchar *str = dal_json_object_get_str(obj);
    guint8 wcp_policy = sml_raid_ctrl_wcp_str2num(str);

    input_list = g_slist_append(input_list, g_variant_new_byte(wcp_type));
    input_list = g_slist_append(input_list, g_variant_new_byte(wcp_policy));
    gint32 ret_val = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_RAID_CONTROLLER_NAME, METHOD_RAID_CONTROLLER_SET_WCP,
        AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    check_set_storage_controller_method_ret(&ret_val, json_pointer, o_message_jso, str);

    return ret_val;
}


LOCAL gint32 set_storage_controller_rcp(PROVIDER_PARAS_S *i_paras, json_object *prop_jso,
    const gchar *json_pointer, json_object *o_message_jso)
{
    guint8 uint8_val;
    GSList *input_list = NULL;
    gint32 ret_val;

    if (prop_jso == NULL) {
        json_object *message_jso = NULL;
        (void)create_message_info(MSGID_PROP_TYPE_ERR, json_pointer, &message_jso, RF_NULL_STR, json_pointer);
        if (json_object_array_add(o_message_jso, message_jso) != 0) {
            json_object_put(message_jso);
        }
        return HTTP_BAD_REQUEST;
    }

    uint8_val = json_object_get_int(prop_jso);

    input_list = g_slist_append(input_list, g_variant_new_byte(uint8_val));
    ret_val = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_RAID_CONTROLLER_NAME, METHOD_RAID_CONTROLLER_SET_READ_CAHCE_PERCENT,
        AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    check_set_storage_controller_method_ret(&ret_val, json_pointer, o_message_jso, dal_json_object_get_str(prop_jso));

    return ret_val;
}


LOCAL gint32 set_storage_controller_prop(PROVIDER_PARAS_S *i_paras, json_object *prop_jso, const gchar *method_name,
    const gchar *json_pointer, json_object **o_message_jso)
{
    json_bool bool_val;
    guint8 uint8_val;
    GSList *input_list = NULL;
    gint32 ret_val;
    json_object *message_jso = NULL;

    if (json_object_get_type(prop_jso) != json_type_boolean) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, json_pointer, &message_jso, RF_NULL_STR, json_pointer);
        ret_val = json_object_array_add(*o_message_jso, message_jso);
        if (ret_val != 0) {
            json_object_put(message_jso);
        }
        return HTTP_BAD_REQUEST;
    }

    bool_val = json_object_get_boolean(prop_jso);
    uint8_val = (bool_val) ? ENABLE : DISABLE;

    input_list = g_slist_append(input_list, g_variant_new_byte(uint8_val));
    ret_val =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->obj_handle,
        CLASS_RAID_CONTROLLER_NAME, method_name, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    check_set_storage_controller_method_ret(&ret_val, json_pointer, *o_message_jso, NULL);

    return ret_val;
}


LOCAL void set_consis_check_enabled(json_object *cc_proj, guint8 *enabled)
{
    json_bool prop_exist;
    json_object *obj_prop = NULL;
    json_bool o_enabled;
    prop_exist = json_object_object_get_ex(cc_proj, RFPROP_STORAGE_CONTROLLER_CC_ENABLED, &obj_prop);
    if (prop_exist) {
        o_enabled = json_object_get_boolean(obj_prop);
        *enabled = (o_enabled) ? CTRL_CC_ENABLE : CTRL_CC_DISABLE;
    } else {
        *enabled = CTRL_CC_SET;
    }
    return;
}


LOCAL gint32 set_consis_check_period(json_object *cc_proj, guint16 *period, json_object **err_jso, guint8 *mask)
{
    json_bool prop_exist;
    json_object *obj_prop = NULL;
    guint16 o_peirod;
    json_object *message = NULL;
    const gchar *property_str =
        RFPROP_OEM "/" RFPROP_OEM_HUAWEI "/" RFPROP_STORAGE_CONTROLLER_CC_INFO "/" RFPROP_STORAGE_CONTROLLER_CC_PERIOD;
    prop_exist = json_object_object_get_ex(cc_proj, RFPROP_STORAGE_CONTROLLER_CC_PERIOD, &obj_prop);
    if (prop_exist) {
        o_peirod = (guint16)json_object_get_int(obj_prop);
        // 对于1880卡，period需要是24-1440范围内的24的整数倍
        if (o_peirod >= CTRL_CC_PERIOD_MIN && o_peirod <= CTRL_CC_PERIOD_MAX && (o_peirod % CTRL_CC_PERIOD_MIN == 0)) {
            *period = o_peirod;
            *mask |= CTRL_CC_MASK_PERIOD;
        } else {
            const char *period_s = dal_json_object_get_str(obj_prop);
            debug_log(DLOG_ERROR, "%s: period is invalid, period:%s", __FUNCTION__, period_s);
            (void)create_message_info(MSGID_PROP_INVALID_VALUE, property_str, &message, period_s, property_str);
            (void)json_object_array_add(*err_jso, message);
            return HTTP_BAD_REQUEST;
        }
    } else {
        *period = CTRL_CC_PERIOD_MIN;
    }
    return HTTP_OK;
}


LOCAL guint8 consis_check_rate_to_num(json_object *prop_jso)
{
    guint8 ret;
    const gchar *rate_s = dal_json_object_get_str(prop_jso);
    ret = sml_ctrl_ccheck_rate_str2num(rate_s);
    if (ret == STORAGE_INFO_INVALID_BYTE) {
        debug_log(DLOG_ERROR, "Rate is invalid, rate: %s", rate_s);
    }
    return ret;
}


LOCAL gint32 set_consis_check_rate(json_object *cc_proj, guint8 *rate, json_object **err_jso, guint8 *mask)
{
    json_bool prop_exist;
    json_object *obj_prop = NULL;
    guint8 o_rate;
    json_object *message = NULL;
    const gchar *property_str =
        RFPROP_OEM "/" RFPROP_OEM_HUAWEI "/" RFPROP_STORAGE_CONTROLLER_CC_INFO "/" RFPROP_STORAGE_CONTROLLER_CC_RATE;
    prop_exist = json_object_object_get_ex(cc_proj, RFPROP_STORAGE_CONTROLLER_CC_RATE, &obj_prop);
    if (prop_exist) {
        o_rate = consis_check_rate_to_num(obj_prop);
        if (o_rate != STORAGE_INFO_INVALID_BYTE) {
            *rate = o_rate;
            *mask |= CTRL_CC_MASK_RATE;
        } else {
            const gchar *rate_s = dal_json_object_get_str(obj_prop);
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, property_str, &message, rate_s, property_str);
            (void)json_object_array_add(*err_jso, message);
            return HTTP_BAD_REQUEST;
        }
    } else {
        *rate = CTRL_CCHECK_RATE_LOW;
    }
    return HTTP_OK;
}


LOCAL void set_consis_check_repair(json_object *cc_proj, guint8 *repair, guint8 *mask)
{
    json_bool prop_exist;
    json_bool o_rapair;
    json_object *obj_prop = NULL;
    prop_exist = json_object_object_get_ex(cc_proj, RFPROP_STORAGE_CONTROLLER_CC_REPAIR, &obj_prop);
    if (prop_exist) {
        o_rapair = json_object_get_boolean(obj_prop);
        *repair = (o_rapair) ? CTRL_CC_ENABLE : CTRL_CC_DISABLE;
        *mask |= CTRL_CC_MASK_REPAIR;
    } else {
        *repair = CTRL_CC_DISABLE;
    }
    return;
}


LOCAL guint32 set_consis_check_delay(json_object *cc_proj, guint32 *delay, guint8 enable, json_object **err_jso,
    OBJ_HANDLE handle)
{
    json_bool prop_exist;
    json_object *obj_prop = NULL;
    guint32 o_delay = 0;
    json_object *message = NULL;
    const gchar *property_str =
        RFPROP_OEM "/" RFPROP_OEM_HUAWEI "/" RFPROP_STORAGE_CONTROLLER_CC_INFO "/" RFPROP_STORAGE_CONTROLLER_CC_DELAY;
    prop_exist = json_object_object_get_ex(cc_proj, RFPROP_STORAGE_CONTROLLER_CC_DELAY, &obj_prop);
    if (prop_exist) {
        o_delay = (guint32)json_object_get_int(obj_prop);
        if (enable != CTRL_CC_ENABLE) {
            debug_log(DLOG_ERROR, "%s: delay can not be set in this situation", __FUNCTION__);
            (void)create_message_info(MSGID_PROP_NOT_REQUIRED, property_str, &message, property_str);
            (void)json_object_array_add(*err_jso, message);
            return HTTP_BAD_REQUEST;
        }
        if (o_delay > CTRL_CC_DELAY_MAX) {
            const char *delay_s = dal_json_object_get_str(obj_prop);
            debug_log(DLOG_ERROR, "%s: delay is out of bound", __FUNCTION__);
            (void)create_message_info(MSGID_PROP_INVALID_VALUE, property_str, &message, delay_s, property_str);
            (void)json_object_array_add(*err_jso, message);
            return HTTP_BAD_REQUEST;
        }
        *delay = o_delay;
    } else {
        // 如果并没有传入delay的数值，并且原来的值小于等于最大值，delay的数值不变；否则置0
        (void)dal_get_property_value_uint32(handle, PROPERTY_RAID_CONTROLLER_CC_DELAY, &o_delay);
        *delay = (o_delay <= CTRL_CC_DELAY_MAX) ? o_delay : 0;
    }
    return HTTP_OK;
}


LOCAL gint32 is_consischeck_param_valid(guint8 enable, guint8 mask, json_object **err_jso)
{
    json_object *message = NULL;
    const gchar *property_str =
        RFPROP_OEM "/" RFPROP_OEM_HUAWEI "/" RFPROP_STORAGE_CONTROLLER_CC_INFO;
    if (enable == CTRL_CC_SET && mask == 0) {
        debug_log(DLOG_ERROR, "%s: no valid param", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    } else if (enable == CTRL_CC_DISABLE && mask != 0) {
        debug_log(DLOG_ERROR, "%s: enable is invalid", __FUNCTION__);
        if ((mask & CTRL_CC_MASK_PERIOD) != 0) {
            (void)create_message_info(MSGID_FAN_ATTRIBUTE_CONFLICT, property_str, &message,
                RFPROP_STORAGE_CONTROLLER_CC_ENABLED, "false", RFPROP_STORAGE_CONTROLLER_CC_PERIOD);
            (void)json_object_array_add(*err_jso, message);
        }
        if ((mask & CTRL_CC_MASK_RATE) != 0) {
            (void)create_message_info(MSGID_FAN_ATTRIBUTE_CONFLICT, property_str, &message,
                RFPROP_STORAGE_CONTROLLER_CC_ENABLED, "false", RFPROP_STORAGE_CONTROLLER_CC_RATE);
            (void)json_object_array_add(*err_jso, message);
        }
        if ((mask & CTRL_CC_MASK_REPAIR) != 0) {
            (void)create_message_info(MSGID_FAN_ATTRIBUTE_CONFLICT, property_str, &message,
                RFPROP_STORAGE_CONTROLLER_CC_ENABLED, "false", RFPROP_STORAGE_CONTROLLER_CC_REPAIR);
            (void)json_object_array_add(*err_jso, message);
        }
        return HTTP_BAD_REQUEST;
    }
    return HTTP_OK;
}


LOCAL gint32 set_consischeck_param(json_object *prop_jso, GSList **input_list, json_object **err_jso, OBJ_HANDLE handle)
{
    guint8 enable, rate, repair;
    guint8 mask = 0;
    guint16 period;
    guint32 delay;
    gint32 ret_val;
    guint8 current_enabled = CTRL_CC_DISABLE;

    set_consis_check_enabled(prop_jso, &enable);

    ret_val = set_consis_check_period(prop_jso, &period, err_jso, &mask);
    if (ret_val == HTTP_BAD_REQUEST) {
        debug_log(DLOG_ERROR, "set_consis_check_period failed, return: %d", ret_val);
        return ret_val;
    }

    ret_val = set_consis_check_rate(prop_jso, &rate, err_jso, &mask);
    if (ret_val == HTTP_BAD_REQUEST) {
        debug_log(DLOG_ERROR, "set_consis_check_rate failed, return: %d", ret_val);
        return ret_val;
    }

    set_consis_check_repair(prop_jso, &repair, &mask);

    // 判断delay信息是否存在
    json_object *obj_prop = NULL;
    json_bool prop_exist = json_object_object_get_ex(prop_jso, RFPROP_STORAGE_CONTROLLER_CC_DELAY, &obj_prop);
    if (prop_exist) {
        mask |= CTRL_CC_MASK_DELAY;
    }

    // 如果重复开启，并且有参数，那就当作set处理
    (void)dal_get_property_value_byte(handle, PROPERTY_RAID_CONTROLLER_CC_ENABLED, &current_enabled);
    if (enable == current_enabled && enable == CTRL_CC_ENABLE && mask != 0) {
        debug_log(DLOG_ERROR, "Consistency check is already enabled");
        enable = CTRL_CC_SET;
    }
    ret_val = set_consis_check_delay(prop_jso, &delay, enable, err_jso, handle);
    if (ret_val == HTTP_BAD_REQUEST) {
        debug_log(DLOG_ERROR, "set_consis_check_delay failed, return: %d", ret_val);
        return ret_val;
    }

    *input_list = g_slist_append(*input_list, g_variant_new_byte(enable));
    *input_list = g_slist_append(*input_list, g_variant_new_uint16(period));
    *input_list = g_slist_append(*input_list, g_variant_new_byte(rate));
    *input_list = g_slist_append(*input_list, g_variant_new_byte(repair));
    *input_list = g_slist_append(*input_list, g_variant_new_uint32(delay));
    *input_list = g_slist_append(*input_list, g_variant_new_byte(mask));

    ret_val = is_consischeck_param_valid(enable, mask, err_jso);
    return ret_val;
}


LOCAL gint32 set_storage_controller_consischeck(PROVIDER_PARAS_S *i_paras, json_object *prop_jso,
    const gchar *method_name, const gchar *json_pointer, json_object *o_message_jso)
{
    GSList *input_list = NULL;
    gint32 ret_val;
    ret_val = set_consischeck_param(prop_jso, &input_list, &o_message_jso, i_paras->obj_handle);
    if (ret_val == HTTP_BAD_REQUEST) {
        debug_log(DLOG_ERROR, "%s: failed, ret_val: %d", __FUNCTION__, ret_val);
        uip_free_gvariant_list(input_list);
        return ret_val;
    }
    ret_val =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->obj_handle,
            CLASS_RAID_CONTROLLER_NAME, method_name, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    check_set_storage_controller_method_ret(&ret_val, json_pointer, o_message_jso, NULL);

    return ret_val;
}


LOCAL gint32 get_boot_devices_string(json_object *prop_jso, gchar *strv[], gint32 str_size, gint32 arr_cnt)
{
#define LOGICAL_DRIVE_REDFISH_FORMAT    "LogicalDrive%hu"
#define LOGICAL_DRIVE_STORAGE_FORMAT    "Logical Drive %hu"
    GSList *obj_list = NULL;
    const gchar *boot_device = NULL;
    json_object *item = NULL;
    guint16 logical_id = 0;
    gint32 ret = RET_OK;

    
    if (dfl_get_object_list(CLASS_HDD_NAME, &obj_list) != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_object_list fail(Ret:%d)!", ret);
        return RET_ERR;
    }

    for (gint32 i = 0; i < arr_cnt; i++) {
        item = json_object_array_get_idx(prop_jso, i);
        if (item == NULL) {
            continue;
        }

        boot_device = dal_json_object_get_str(item);
        if (g_strcmp0(boot_device, "None") == 0) {
            if (strncpy_s(strv[i], str_size, boot_device, str_size - 1) != 0) {
                debug_log(DLOG_ERROR, "%s: strncpy_s failed", __FUNCTION__);
                ret = RET_ERR;
            }
            continue;
        }

        
        if (sscanf_s(boot_device, LOGICAL_DRIVE_REDFISH_FORMAT, &logical_id) == 1) {
            gint32 tmp_ret = snprintf_s(strv[i], str_size, str_size - 1, LOGICAL_DRIVE_STORAGE_FORMAT, logical_id);
            if (tmp_ret <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret = %d", __FUNCTION__, tmp_ret);
                ret = RET_ERR;
                break;
            }
            continue;
        }

        
        for (GSList *node = obj_list; node; node = node->next) {
            OBJ_HANDLE obj_handle = (OBJ_HANDLE)node->data;

            get_drive_id(obj_handle, strv[i], str_size);

            if (g_strcmp0(boot_device, strv[i]) == 0) {
                dal_get_property_value_string(obj_handle, PROPERTY_HDD_DEVICENAME, strv[i], str_size);
                break;
            }
        }
    }

    g_slist_free(obj_list);
    return ret;
}


LOCAL gint32 set_storage_controller_bootdevices(PROVIDER_PARAS_S *i_paras, json_object *prop_jso,
    const gchar *prop_path, json_object *o_message_jso)
{
    json_object *error_jso = NULL;
    gchar *strv[BOOT_DEVICE_NUM] = { NULL };
    GSList *input_list = NULL;
    gint32 response_code = HTTP_OK;

    do {
        for (int i = 0; i < BOOT_DEVICE_NUM; i++) {
            strv[i] = g_malloc0(MAX_STRBUF_LEN + 1);
            if (strv[i] == NULL) {
                (void)create_message_info(MSGID_INTERNAL_ERR, prop_path, &error_jso);
                response_code = HTTP_INTERNAL_SERVER_ERROR;
                break;
            }
        }

        if (response_code != HTTP_OK) {
            break;
        }

        if ((json_object_get_type(prop_jso) != json_type_array) ||
            (json_object_array_length(prop_jso)) != BOOT_DEVICE_NUM) {
            (void)create_message_info(MSGID_PROP_TYPE_ERR, prop_path, &error_jso, RF_NULL_STR, prop_path);
            response_code = HTTP_BAD_REQUEST;
            break;
        }

        if (get_boot_devices_string(prop_jso, strv, MAX_STRBUF_LEN + 1, BOOT_DEVICE_NUM) != RET_OK) {
            (void)create_message_info(MSGID_INTERNAL_ERR, prop_path, &error_jso);
            response_code = HTTP_INTERNAL_SERVER_ERROR;
            break;
        }

        input_list = g_slist_append(input_list, g_variant_new_strv((const gchar * const *)strv, BOOT_DEVICE_NUM));

        gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
            i_paras->obj_handle, CLASS_RAID_CONTROLLER_NAME, METHOD_RAID_CONTROLLER_SET_BOOT_DEVICES,
            AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

        
        check_set_storage_controller_method_ret(&ret, prop_path, o_message_jso,
            dal_json_object_to_json_string(prop_jso));
        return ret;
    } while (0);

    for (int i = 0; i < BOOT_DEVICE_NUM; i++) {
        if (strv[i] != NULL) {
            free(strv[i]);
        }
    }

    if (error_jso != NULL && json_object_array_add(o_message_jso, error_jso) != 0) {
        json_object_put(error_jso);
    }

    return response_code;
}


LOCAL gint32 set_storage_controller_mode(json_object *obj_ctrl_mode, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso)
{
    gint32 ret;
    const gchar *property_str =
        RFPROP_STORAGE_CONTROLLERS "/0/" RFPROP_OEM "/" RFPROP_OEM_HUAWEI "/" RFPROP_STORAGE_CONTROLLER_MODE;
    const gchar *mode_str = dal_json_object_get_str(obj_ctrl_mode);
    if (mode_str == NULL) {
        ret = RET_ERR;
        debug_log(DLOG_ERROR, "%s:get mode str return null.", __FUNCTION__);
        goto MODE_ERR;
    }

    guint8 ctrl_mode = sml_raid_ctrl_mode_str2num(mode_str);
    if (ctrl_mode == CTRL_MODE_UNKNOWN) {
        json_object *message = NULL;
        debug_log(DLOG_ERROR, "%s:get mode str return null.", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_INVALID_VALUE, property_str, &message,
            mode_str, property_str);
        (void)json_object_array_add(*o_message_jso, message);
        return HTTP_BAD_REQUEST;
    }

    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_byte(ctrl_mode));
    input_list = g_slist_append(input_list, g_variant_new_byte(0xFF)); // profile id目前用0xFF

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_RAID_CONTROLLER_NAME, METHOD_RAID_CONTROLLER_SET_MODE, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    if (ret == SML_ERR_REBOOT_REQUIRED) {
        debug_log(DLOG_INFO, "%s: set controller mode success, please reboot OS to take effect.", __FUNCTION__);
        (void)create_message_info(MSGID_RAID_CONTROLLER_SET_SUCC, NULL, o_message_jso);
    }
    uip_free_gvariant_list(input_list);
    input_list = NULL;
MODE_ERR:
    check_set_storage_controller_method_ret(&ret, property_str, *o_message_jso, mode_str);

    if (ret == HTTP_OK) {
        return HTTP_OK;
    }

    return HTTP_BAD_REQUEST;
}


LOCAL gint32 set_storage_controller_wcp(PROVIDER_PARAS_S *i_paras, json_object *obj_wcp, json_object *o_message_jso)
{
    gchar *str = NULL;
    json_bool config_exist;
    json_bool unconfig_exist;
    json_bool hba_exist;
    json_object *obj_config = NULL;
    json_object *obj_unconfig = NULL;
    json_object *obj_hba = NULL;
    gint32 ret = RET_OK;

    config_exist = json_object_object_get_ex(obj_wcp, RFPROP_STORAGE_CONTROLLER_CONFIG_DRIVE, &obj_config);
    unconfig_exist = json_object_object_get_ex(obj_wcp, RFPROP_STORAGE_CONTROLLER_UNCONFIG_DRIVE,
        &obj_unconfig);
    hba_exist = json_object_object_get_ex(obj_wcp, RFPROP_STORAGE_CONTROLLER_HBA_DRIVE, &obj_hba);

    if (config_exist) {
        str = RFPROP_STORAGE_CONTROLLERS "/0/" RFPROP_OEM "/" RFPROP_OEM_HUAWEI "/"
            RFPROP_STORAGE_CONTROLLER_WRITE_CACHE_POLICY "/" RFPROP_STORAGE_CONTROLLER_CONFIG_DRIVE;
        ret = set_storage_controller_wcp_prop(CTRL_DRIVE_WCP_CONFIG, obj_config, str, i_paras, o_message_jso);
    }
    if (unconfig_exist) {
        str = RFPROP_STORAGE_CONTROLLERS "/0/" RFPROP_OEM "/" RFPROP_OEM_HUAWEI "/"
            RFPROP_STORAGE_CONTROLLER_WRITE_CACHE_POLICY "/" RFPROP_STORAGE_CONTROLLER_UNCONFIG_DRIVE;
        ret = set_storage_controller_wcp_prop(CTRL_DRIVE_WCP_UNCONFIG, obj_unconfig, str, i_paras, o_message_jso);
    }
    if (hba_exist) {
        str = RFPROP_STORAGE_CONTROLLERS "/0/" RFPROP_OEM "/" RFPROP_OEM_HUAWEI "/"
            RFPROP_STORAGE_CONTROLLER_WRITE_CACHE_POLICY "/" RFPROP_STORAGE_CONTROLLER_HBA_DRIVE;
        ret = set_storage_controller_wcp_prop(CTRL_DRIVE_WCP_HBA, obj_hba, str, i_paras, o_message_jso);
    }

    return ret;
}


LOCAL void set_storage_controllers_ccheck(PROVIDER_PARAS_S *i_paras, json_object *i_jso,
    json_object *o_message_jso, gint32 *o_code)
{
    gint32 ret;
    json_object *obj_consis_check = NULL;
    json_bool consis_check_exist = json_object_object_get_ex(i_jso, RFPROP_STORAGE_CONTROLLER_CC_INFO,
        &obj_consis_check);
    
    if (!consis_check_exist) {
        return;
    }
    ret = set_storage_controller_consischeck(i_paras, obj_consis_check, METHOD_RAID_CONTROLLER_SET_CONSIS_CHECK,
        RFPROP_OEM "/" RFPROP_OEM_HUAWEI "/" RFPROP_STORAGE_CONTROLLER_CC_INFO,
        o_message_jso);
    if (ret == HTTP_OK) {
        *o_code = ret;
    } else {
        debug_log(DLOG_ERROR, "%s :failed, return: %d", __FUNCTION__, ret);
    }
    return;
}


LOCAL void set_storage_controllers_extend(PROVIDER_PARAS_S *i_paras, json_object *i_jso,
                                          json_object *o_message_jso, gint32 *o_code)
{
    gint32 ret;
    json_object *obj_nbwc = NULL;
    json_object *obj_rcp = NULL;
    json_object *obj_wcp = NULL;
    json_bool nbwc_exist;
    json_bool rcp_exist;
    json_bool wcp_exist;
    json_object *obj_bootdevices = NULL;

    const gchar *str =
        RFPROP_STORAGE_CONTROLLERS "/0/" RFPROP_OEM "/" RFPROP_OEM_HUAWEI "/" REPROP_STORAGE_CONTROLLER_BOOT_DEVICES;

    if (json_object_object_get_ex(i_jso, REPROP_STORAGE_CONTROLLER_BOOT_DEVICES, &obj_bootdevices) == TRUE) {
        
        if (set_storage_controller_bootdevices(i_paras, obj_bootdevices, str, o_message_jso) == HTTP_OK) {
            *o_code = HTTP_OK;
        }
    }

    nbwc_exist = json_object_object_get_ex(i_jso, RFPROP_STORAGE_CONTROLLER_NO_BATTERY_WRITE_CACHE, &obj_nbwc);
    rcp_exist = json_object_object_get_ex(i_jso, RFPROP_STORAGE_CONTROLLER_READ_CACHE_PERCENT, &obj_rcp);
    wcp_exist = json_object_object_get_ex(i_jso, RFPROP_STORAGE_CONTROLLER_WRITE_CACHE_POLICY, &obj_wcp);
    
    if (nbwc_exist) {
        ret = set_storage_controller_prop(i_paras, obj_nbwc, METHOD_RAID_CONTROLLER_SET_NBWC,
            RFPROP_STORAGE_CONTROLLERS "/0/" RFPROP_OEM "/" RFPROP_OEM_HUAWEI "/"
            RFPROP_STORAGE_CONTROLLER_NO_BATTERY_WRITE_CACHE, &o_message_jso);
        if (ret == HTTP_OK) {
            *o_code = ret;
        }
    }
    
    if (rcp_exist) {
        str = RFPROP_STORAGE_CONTROLLERS "/0/" RFPROP_OEM "/" RFPROP_OEM_HUAWEI "/"
            RFPROP_STORAGE_CONTROLLER_READ_CACHE_PERCENT;
        ret = set_storage_controller_rcp(i_paras, obj_rcp, str, o_message_jso);
        if (ret == HTTP_OK) {
            *o_code = ret;
        }
    }
    
    if (wcp_exist) {
        ret = set_storage_controller_wcp(i_paras, obj_wcp, o_message_jso);
        if (ret == HTTP_OK) {
            *o_code = ret;
        }
    }
    set_storage_controllers_ccheck(i_paras, i_jso, o_message_jso, o_code);
    return;
}

LOCAL gint32 check_count_controller(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso)
{
    gint32 count_controller = json_object_array_length(i_paras->val_jso);
    if (count_controller == 0) {
        (void)create_message_info(MSGID_ARRAY_REQUIRED_ITEM, RFPROP_STORAGE_CONTROLLERS, o_message_jso,
                                  RFPROP_STORAGE_CONTROLLERS);
        return HTTP_BAD_REQUEST;
    }

    if (count_controller != 1) {
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, RFPROP_STORAGE_CONTROLLERS, o_message_jso,
                                  RFPROP_STORAGE_CONTROLLERS);
        return HTTP_BAD_REQUEST;
    }

    return RET_OK;
}


LOCAL gint32 set_storage_Controllers(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 http_over_code = HTTP_BAD_REQUEST;
    json_bool jbod_exist = FALSE;
    json_bool copyback_exist = FALSE;
    json_bool smarter_copyback_exist = FALSE;
    json_bool mode_exist = FALSE;
    json_bool maintpdfailhistory_exist = FALSE;
    json_object *obj_jbod = NULL;
    json_object *object_smarter_copyback = NULL;
    json_object *obj_copyback = NULL;
    json_object *obj_huawei = NULL;
    json_object *obj_oem = NULL;
    json_object *obj_ctrl_mode = NULL;
    json_object *obj_maintpdfailhistory = NULL;

    return_val_do_info_if_fail(
        ((NULL != o_message_jso) && (VOS_OK == provider_paras_check(i_paras)) && (NULL != i_paras->val_jso)),
        HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: input parameter is error.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    return_val_do_info_if_fail(json_type_array == json_object_get_type(i_paras->val_jso), HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s failed:is not json array", __FUNCTION__));

    gint32 ret = check_count_controller(i_paras, o_message_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s check_count_controller fail, ret = %d", __FUNCTION__, ret);
        return ret;
    }

    json_object *obj_controller = json_object_array_get_idx(i_paras->val_jso, 0);
    *o_message_jso = json_object_new_array();
    return_val_do_info_if_fail((NULL != *o_message_jso), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: alloc json object fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    if (json_object_object_get_ex(obj_controller, RFPROP_COMMON_OEM, &obj_oem)) {
        if (json_object_object_get_ex(obj_oem, RFPROP_COMMON_MANUFACTURER, &obj_huawei)) {
            copyback_exist = json_object_object_get_ex(obj_huawei, RFPROP_STORAGE_CONTROLLER_COPYBACK, &obj_copyback);
            smarter_copyback_exist = json_object_object_get_ex(obj_huawei, RFPROP_STORAGE_CONTROLLER_SMART_COPYBACK,
                &object_smarter_copyback);
            jbod_exist = json_object_object_get_ex(obj_huawei, RFPROP_STORAGE_CONTROLLER_JBOD, &obj_jbod);
            mode_exist = json_object_object_get_ex(obj_huawei, RFPROP_STORAGE_CONTROLLER_MODE, &obj_ctrl_mode);
            maintpdfailhistory_exist =
                json_object_object_get_ex(obj_huawei, RFPROP_STORAGE_CONTROLLER_FAULT_HISTORY, &obj_maintpdfailhistory);
        }
    }

    if (copyback_exist) {
        ret = set_storage_controller_prop(i_paras, obj_copyback, METHOD_RAID_CONTROLLER_SET_COPYBACK,
            RFPROP_STORAGE_CONTROLLERS "/0/" RFPROP_OEM "/" RFPROP_OEM_HUAWEI "/" RFPROP_STORAGE_CONTROLLER_COPYBACK,
            o_message_jso);
        
        do_val_if_expr(HTTP_OK == ret, (http_over_code = ret));
    }

    if (smarter_copyback_exist) {
        ret = set_storage_controller_prop(i_paras, object_smarter_copyback,
            METHOD_RAID_CONTROLLER_SET_SMARTER_COPYBACK,
            RFPROP_STORAGE_CONTROLLERS "/0/" RFPROP_OEM "/" RFPROP_OEM_HUAWEI
            "/" RFPROP_STORAGE_CONTROLLER_SMART_COPYBACK, o_message_jso);
        
        do_val_if_expr(HTTP_OK == ret, (http_over_code = ret));
    }

    if (jbod_exist) {
        ret = set_storage_controller_prop(i_paras, obj_jbod, METHOD_RAID_CONTROLLER_SET_JBOD,
            RFPROP_STORAGE_CONTROLLERS "/0/" RFPROP_OEM "/" RFPROP_OEM_HUAWEI "/" RFPROP_STORAGE_CONTROLLER_JBOD,
            o_message_jso);
        
        do_val_if_expr(HTTP_OK == ret, (http_over_code = ret));
    }

    if (mode_exist) {
        ret = set_storage_controller_mode(obj_ctrl_mode, i_paras, o_message_jso);
        if (ret == HTTP_OK) {
            http_over_code = ret;
        }
    }

    if (maintpdfailhistory_exist) {
        ret = set_storage_controller_prop(i_paras, obj_maintpdfailhistory,
        METHOD_RAID_CONTROLLER_SET_MAINTPDFAILHISTORY, RFPROP_STORAGE_CONTROLLERS "/0/" RFPROP_OEM
        "/" RFPROP_OEM_HUAWEI "/" RFPROP_STORAGE_CONTROLLER_FAULT_HISTORY, o_message_jso);
        if (ret == HTTP_OK) {
            http_over_code = ret;
        }
    }

    set_storage_controllers_extend(i_paras, obj_huawei, *o_message_jso, &http_over_code);
    return http_over_code;
    
}

LOCAL gint32 restore_storage_controller_default_setting(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    const gchar *action_name = "Storage.RestoreStorageControllerDefaultSettings";

    
    if (NULL == o_message_jso || NULL == i_paras) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_RAID_CONTROLLER_NAME, METHOD_RAID_CONTROLLER_RESTORE_SETTINGS, AUTH_ENABLE,
        i_paras->user_role_privilege, NULL, NULL);
    
    if (RFERR_INSUFFICIENT_PRIVILEGE == ret) {
        debug_log(DLOG_ERROR, "[%s][%d]: ERROR: %s", __FUNCTION__, __LINE__, MSGID_ACT_PARA_NOT_SUPPORTED);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    if (ret == SM_CODE_NOT_SUPPORT_IN_PRESENT_STATE || ret == SML_ERR_CTRL_STATUS_INVALID ||
        ret == SML_ERR_CTRL_INIT_NOT_COMPLETED || ret == SML_ERR_I2C_READ_WRITE_FAILED) {
        debug_log(DLOG_ERROR, "[%s][%d]: ERROR: %s", __FUNCTION__, __LINE__, MSGID_ACT_NOT_SUPPORTED);
        (void)create_message_info(MSGID_CURRENT_STATUS_NOT_SUPPORT, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }
    if (ret == SML_ERR_REBOOT_REQUIRED) {
        debug_log(DLOG_INFO, "%s: restore raid controller set success, please reboot OS to take effect.", __FUNCTION__);
        (void)create_message_info(MSGID_RAID_CONTROLLER_SET_SUCC, NULL, o_message_jso);
        return HTTP_OK;
    }

    if (ret == SML_ERR_CTRL_OPERATION_NOT_SUPPORT) {
        debug_log(DLOG_ERROR, "%s: raid controller does not support to set", __FUNCTION__);
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, o_message_jso, action_name);
        return HTTP_BAD_REQUEST;
    }

    if (ret == SM_CODE_OPERATION_IN_PROGRESS) {
        (void)create_message_info(VMM_MESSAGE_OPERATIONINPROCESS, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s][%d]: ERROR: %s", __FUNCTION__, __LINE__, MSGID_ACT_PARA_NOT_SUPPORTED);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL gint32 redfish_check_ctrl_foreign_config_ret(gint32 ret, json_object **o_message_jso)
{
    switch (ret) {
        case RET_OK:
            return HTTP_OK;
        case RFERR_INSUFFICIENT_PRIVILEGE:
            debug_log(DLOG_ERROR, "[%s]: ERROR: %s", __FUNCTION__, MSGID_ACT_PARA_NOT_SUPPORTED);
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return HTTP_FORBIDDEN;
        case SM_CODE_NOT_SUPPORT_IN_PRESENT_STATE:
        case SML_ERR_CTRL_STATUS_INVALID:
        case SML_ERR_CTRL_INIT_NOT_COMPLETED:
        case SML_ERR_I2C_READ_WRITE_FAILED:
        case SML_ERR_CTRL_OPERATION_NOT_SUPPORT:
            debug_log(DLOG_ERROR, "[%s]: ERROR: %s", __FUNCTION__, MSGID_ACT_NOT_SUPPORTED);
            (void)create_message_info(MSGID_CURRENT_STATUS_NOT_SUPPORT, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        case SML_ERR_CONFIG_INCOMPLETE_FOREIGN_CONFIG:
            debug_log(DLOG_ERROR, "[%s]: ERROR: %s", __FUNCTION__, MSGID_INCOMPLETE_FOREIGN_CONFIG);
            (void)create_message_info(MSGID_INCOMPLETE_FOREIGN_CONFIG, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        case SML_ERR_CONFIG_NO_FOREIGN_CONFIG:
            debug_log(DLOG_ERROR, "[%s]: ERROR: %s", __FUNCTION__, MSGID_NO_FOREIGN_CONFIG);
            (void)create_message_info(MSGID_NO_FOREIGN_CONFIG, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        case SM_CODE_OPERATION_IN_PROGRESS:
            (void)create_message_info(VMM_MESSAGE_OPERATIONINPROCESS, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        default:
            debug_log(DLOG_ERROR, "[%s]: ERROR: %s", __FUNCTION__, MSGID_ACT_PARA_NOT_SUPPORTED);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
    }
}


LOCAL gint32 import_ctrl_foreign_config(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    if ((o_message_jso == NULL) || (i_paras == NULL)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_RAID_CONTROLLER_NAME, METHOD_RAID_CONTROLLER_IMPORT_FOREIGN_CONFIG, AUTH_ENABLE,
        i_paras->user_role_privilege, NULL, NULL);

    return redfish_check_ctrl_foreign_config_ret(ret, o_message_jso);
}


LOCAL gint32 clear_ctrl_foreign_config(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    if ((o_message_jso == NULL) || (i_paras == NULL)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_RAID_CONTROLLER_NAME, METHOD_RAID_CONTROLLER_CLEAR_FOREIGN_CONFIG, AUTH_ENABLE,
        i_paras->user_role_privilege, NULL, NULL);

    return redfish_check_ctrl_foreign_config_ret(ret, o_message_jso);
}


LOCAL gint32 get_storage_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet;
    
    gint32 ret;
    json_object *oem_obj = NULL;
    json_object *huawei_obj = NULL;
    gchar board_slot[MAX_RSC_ID_LEN] = {0};
    gchar storage_uri[MAX_URI_LENGTH] = {0};
    guint8 controller_id = 0;

    ret = redfish_get_board_slot(board_slot, MAX_RSC_ID_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: get board lost fail.\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_RAID_CONTROLLER_ID, &controller_id);
    
    
    iRet = snprintf_s(storage_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, SYSTEMS_STORAGE, board_slot, controller_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    

    huawei_obj = json_object_new_object();
    if (NULL == huawei_obj) {
        debug_log(DLOG_ERROR, "%s, %d: new oem json object fail.\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    rf_add_action_prop(huawei_obj, (const gchar *)storage_uri, RFPROP_STORAGE_CONTROLLERS_RESTORE);
    rf_add_action_prop(huawei_obj, (const gchar *)storage_uri, RFPROP_STORAGE_CONTROLLERS_IMPORT_FOREIGN_CONFIG);
    rf_add_action_prop(huawei_obj, (const gchar *)storage_uri, RFPROP_STORAGE_CONTROLLERS_CLEAR_FOREIGN_CONFIG);

    oem_obj = json_object_new_object();
    if (NULL == oem_obj) {
        json_object_put(huawei_obj);
        debug_log(DLOG_ERROR, "%s, %d: new oem json object fail.\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(oem_obj, RFPROP_COMMON_HUAWEI, huawei_obj);

    *o_result_jso = json_object_new_object();
    if (NULL == *o_result_jso) {
        json_object_put(oem_obj);
        debug_log(DLOG_ERROR, "%s, %d: new oem json object fail.\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(*o_result_jso, RFPROP_COMMON_OEM, oem_obj);

    
    return HTTP_OK;
}


LOCAL gint32 get_volumes_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar slot[MAX_RSC_ID_LEN] = {0};
    gchar ld_collection_uri[MAX_URI_LENGTH] = {0};

    json_object *odata = NULL;
    guint8 controller_id = 0;

    if (NULL == o_result_jso || NULL == o_message_jso || RET_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    *o_result_jso = json_object_new_object();
    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get slot id fail", __FUNCTION__);
        return HTTP_OK;
    }

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_RAID_CONTROLLER_ID, &controller_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get property fail ", __FUNCTION__);
        return HTTP_OK;
    }

    iRet =
        snprintf_s(ld_collection_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, RFPROP_SYSTEM_REVOLUMES, slot, controller_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    odata = json_object_new_string((const gchar *)ld_collection_uri);
    if (NULL == odata) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_OK;
    }

    json_object_object_add(*o_result_jso, RFPROP_ODATA_ID, odata);

    return HTTP_OK;
}


LOCAL gboolean redfish_check_storage_raidcontroller_uri_effective(const gchar *i_memberid, OBJ_HANDLE *obj_handle)
{
    gboolean result = FALSE;
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 controller_id = 0;
    gchar controller_memberid[STORAGE_CTRL_INFO_LENGTH] = {0};

    ret = dfl_get_object_list(CLASS_RAID_CONTROLLER_NAME, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_MASS, "%s, %d: get storage controller list fail, ret = %d.\n", __FUNCTION__, __LINE__, ret);
        return result;
    }

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_RAID_CONTROLLER_ID, &controller_id);
        if (ret != RET_OK || controller_id == 0xFF) {
            continue;
        }

        (void)snprintf_s(controller_memberid, sizeof(controller_memberid), sizeof(controller_memberid) - 1,
            "RAIDStorage%u", controller_id);

        if (0 == g_ascii_strcasecmp(i_memberid, controller_memberid)) {
            result = TRUE;
            break;
        }
    }

    if (NULL != obj_node && result == TRUE) {
        *obj_handle = (OBJ_HANDLE)obj_node->data;
        g_slist_free(obj_list);
        return result;
    }
    if (NULL != obj_list) {
        g_slist_free(obj_list);
    }

    debug_log(DLOG_MASS, "%s, %d: check raid controlelr uri fail.\n", __FUNCTION__, __LINE__);
    return result;
}


LOCAL gboolean redfish_check_storage_sdcardcontroller_uri_effective(gchar *i_memberid, OBJ_HANDLE *o_obj_handle)
{
    gint32 ret = 0;
    GSList *obj_list = NULL;
    OBJ_HANDLE sd_handle = 0;
    gchar *sdcontroller_memberid = "SDStorage";
    guint8 presence = 0;

    if (0 == g_ascii_strcasecmp(i_memberid, sdcontroller_memberid)) {
        ret = dfl_get_object_list(CLASS_RAID_CHIP_NAME, &obj_list);
        if (ret != DFL_OK) {
            debug_log(DLOG_MASS, "%s: get storage controller list fail", __FUNCTION__);
            return FALSE;
        }

        sd_handle = (OBJ_HANDLE)g_slist_nth_data(obj_list, 0);
        g_slist_free(obj_list);

        
        ret = dal_get_property_value_byte(sd_handle, PROPETRY_PRESENT, &presence);
        if (RET_OK != ret || (0 == presence)) {
            debug_log(DLOG_MASS, "%s, %d:sd controller is not presence.\n", __FUNCTION__, __LINE__);
            return FALSE;
        }

        *o_obj_handle = sd_handle;
        return TRUE;
    }

    debug_log(DLOG_MASS, "%s, %d: check sdcard controlelr uri fail.\n", __FUNCTION__, __LINE__);
    return FALSE;
}


gint32 storage_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    guchar board_type = 0;
    gboolean bool_ret;

    
    ret = redfish_get_x86_enable_type(&board_type);
    if (ret != RET_OK) {
        return HTTP_NOT_FOUND;
    }
    if (DISABLE == board_type) {
        return HTTP_NOT_FOUND;
    }
    

    bool_ret = redfish_check_system_uri_valid(i_paras->uri);
    if (bool_ret == FALSE) {
        return HTTP_NOT_FOUND;
    }

    
    if (redfish_check_storage_raidcontroller_uri_effective(i_paras->member_id, &i_paras->obj_handle)) {
        *prop_provider = g_storage_raidcontroller_provider;
        *count = sizeof(g_storage_raidcontroller_provider) / sizeof(PROPERTY_PROVIDER_S);
        return RET_OK;
    } else if (redfish_check_storage_sdcardcontroller_uri_effective(i_paras->member_id, &i_paras->obj_handle)) {
        *prop_provider = g_storage_sdcardcontroller_provider;
        *count = sizeof(g_storage_sdcardcontroller_provider) / sizeof(PROPERTY_PROVIDER_S);
        return RET_OK;
    }

    
    return HTTP_NOT_FOUND;
}


LOCAL gint32 get_storage_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gchar               actioninfo_uri[MAX_URI_LENGTH] = {0};
    gint32 ret;
    gchar               storage_uri[MAX_URI_LENGTH] = {0};
    gchar               slot[MAX_RSC_NAME_LEN] = {0};
    guint8 controller_id = 0;
    const gchar*        storage_action_info_array[] = {
        RFPROP_STORAGE_CONTROLLERS_RESTORE,
        RFPROP_STORAGE_CONTROLLERS_IMPORT_FOREIGN_CONFIG,
        RFPROP_STORAGE_CONTROLLERS_CLEAR_FOREIGN_CONFIG
    };

    
    if (NULL == o_result_jso || NULL == o_message_jso || RET_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:%d fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_RAID_CONTROLLER_ID, &controller_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:%d fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    iRet = snprintf_s(storage_uri, sizeof(storage_uri), sizeof(storage_uri) - 1, SYSTEMS_STORAGE, slot, controller_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }

    ret = rf_update_rsc_actioninfo_uri(storage_action_info_array, G_N_ELEMENTS(storage_action_info_array), i_paras->uri,
        storage_uri, actioninfo_uri, MAX_URI_LENGTH);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get %s actioninfo uri failed", i_paras->uri);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)actioninfo_uri);
    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_storage_action_info_provider[] = {
    {
        RFPROP_ODATA_ID,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_storage_actioninfo_odataid,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    }
};

LOCAL gboolean __check_raid_storage_actioninfo_uri(const gchar *uri, const gchar *board_slot, guint8 controller_id)
{
    gint32 ret;
    gchar uri_restore[MAX_URI_LENGTH] = {0};
    gchar uri_import[MAX_URI_LENGTH] = {0};
    gchar uri_clear[MAX_URI_LENGTH] = {0};

    (void)memset_s(uri_restore, sizeof(uri_restore), 0, sizeof(uri_restore));
    ret = snprintf_s(uri_restore, sizeof(uri_restore), sizeof(uri_restore) - 1,
        URI_FORMAT_RAIDSTORAGE_RESTORE_DFLT_SET_ACTIONINFO, board_slot, controller_id);
    if (ret <= 0) {
        debug_log(DLOG_DEBUG, "%s: snprintf_s fail.", __FUNCTION__);
        return FALSE;
    }
    
    (void)memset_s(uri_import, sizeof(uri_import), 0, sizeof(uri_import));
    ret = snprintf_s(uri_import, sizeof(uri_import), sizeof(uri_import) - 1,
        URI_FORMAT_RAIDSTORAGE_IMPORT_FOREIGN_CONFIG_ACTIONINFO, board_slot, controller_id);
    if (ret <= 0) {
        debug_log(DLOG_DEBUG, "%s: snprintf_s fail.", __FUNCTION__);
        return FALSE;
    }
    
    (void)memset_s(uri_clear, sizeof(uri_clear), 0, sizeof(uri_clear));
    ret = snprintf_s(uri_clear, sizeof(uri_clear), sizeof(uri_clear) - 1,
        URI_FORMAT_RAIDSTORAGE_CLEAR_FOREIGN_CONFIG_ACTIONINFO, board_slot, controller_id);
    if (ret <= 0) {
        debug_log(DLOG_DEBUG, "%s: snprintf_s fail.", __FUNCTION__);
        return FALSE;
    }

    if (g_ascii_strcasecmp(uri, uri_restore) != 0 || g_ascii_strcasecmp(uri, uri_import) != 0
        || g_ascii_strcasecmp(uri, uri_clear) != 0) {
        return FALSE;
    }

    return TRUE;
}


LOCAL gboolean __redfish_check_raid_storage_actioninfo_uri_effective(const gchar *raid_storage_actioninfo_uri)
{
    gchar       board_slot[MAX_RSC_NAME_LEN] = {0};
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 controller_id = 0;

    if (raid_storage_actioninfo_uri == NULL) {
        debug_log(DLOG_DEBUG, "%s: Input is NULL. ", __FUNCTION__);
        return FALSE;
    }

    ret = redfish_get_board_slot(board_slot, MAX_RSC_NAME_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get board slot id fail. ", __FUNCTION__);
        return FALSE;
    }

    ret = dfl_get_object_list(CLASS_RAID_CONTROLLER_NAME, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: get storage controller list fail, ret = %d.s", __FUNCTION__, ret);
        return FALSE;
    }

    for (obj_node = obj_list; NULL != obj_node; obj_node = g_slist_next(obj_node)) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_RAID_CONTROLLER_ID, &controller_id);
        if (RET_OK != ret || 0xFF == controller_id) {
            continue;
        }

        ret = __check_raid_storage_actioninfo_uri(raid_storage_actioninfo_uri, board_slot, controller_id);
        if (ret == TRUE) {
            break;
        }
    }
    g_slist_free(obj_list);

    if (obj_node == NULL) {
        return FALSE;
    }

    return TRUE;
}


gint32 storage_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean b_ret;

    if (NULL == i_paras || NULL == prop_provider || NULL == count) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    b_ret = redfish_check_system_uri_valid(i_paras->uri);
    if (FALSE == b_ret) {
        return HTTP_NOT_FOUND;
    }

    if (TRUE == __redfish_check_raid_storage_actioninfo_uri_effective(i_paras->uri)) {
        *prop_provider = g_storage_action_info_provider;
        *count = sizeof(g_storage_action_info_provider) / sizeof(PROPERTY_PROVIDER_S);
        return RET_OK;
    }

    debug_log(DLOG_ERROR, "rsc at %s doesn't exist", i_paras->uri);

    return HTTP_NOT_FOUND;
}

gint32 get_storage_logical_rsc(OBJ_HANDLE obj_handle, json_object *result_data)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    GVariant *hdd_slot_array = NULL;
    GVariant *hdd_enclosure_array = NULL;
    const gchar *hdd_controller_name = NULL;
    gchar logical_controller_name[MAX_STRBUF_LEN + 1] = {0};
    const guint16 *hdd_enclosure_array_pointer = NULL;
    const guint8 *hdd_slot_array_pointer = NULL;
    gsize len_hdd_array_pointer = 0;
    json_object *array_jso = NULL;
    guint8 ld_raid_level = 0;

    hdd_controller_name = dfl_get_object_name(obj_handle);
    if (NULL == hdd_controller_name) {
        debug_log(DLOG_MASS, "%s, %d: get controller name fail.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    ret = dfl_get_object_list(CLASS_LOGICAL_DRIVE_NAME, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s:get logical drive list error", __FUNCTION__);
        return RET_ERR;
    }

    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_RAID_LEVEL,
            &ld_raid_level);
        if (RAID_LEVEL_DELETED == ld_raid_level) {
            continue;
        }
        
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER,
            logical_controller_name, sizeof(logical_controller_name));
        if (0 == g_strcmp0(hdd_controller_name, logical_controller_name)) {
            
            
            ret = dfl_get_property_value((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_PARTICIPATED_PD_ENCLOSURE,
                &hdd_enclosure_array);
            if (ret != DFL_OK || hdd_enclosure_array == NULL) {
                continue;
            }
            hdd_enclosure_array_pointer = (const guint16 *)g_variant_get_fixed_array(hdd_enclosure_array,
                &len_hdd_array_pointer, sizeof(guint16));
            if (hdd_enclosure_array_pointer == NULL) {
                g_variant_unref(hdd_enclosure_array);
                hdd_enclosure_array = NULL;
                debug_log(DLOG_ERROR, "%s, %d: get property ParticipatedPDEnclosure array fail.", __FUNCTION__,
                    __LINE__);
                continue;
            }

            ret = dfl_get_property_value((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_PARTICIPATED_PD_SLOT,
                &hdd_slot_array);
            if (ret != DFL_OK || NULL == hdd_slot_array) {
                g_variant_unref(hdd_enclosure_array);
                hdd_enclosure_array = NULL;
                continue;
            }
            hdd_slot_array_pointer =
                (const guint8 *)g_variant_get_fixed_array(hdd_slot_array, &len_hdd_array_pointer, sizeof(guint8));
            if (NULL == hdd_slot_array_pointer) {
                g_variant_unref(hdd_enclosure_array);
                hdd_enclosure_array = NULL;
                g_variant_unref(hdd_slot_array);
                hdd_slot_array = NULL;
                debug_log(DLOG_ERROR, "%s, %d: get property ParticipatedPDSlot array fail.", __FUNCTION__, __LINE__);
                continue;
            }

            array_jso = json_object_new_object();
            if (NULL == array_jso) {
                g_slist_free(obj_list);
                g_variant_unref(hdd_enclosure_array);
                g_variant_unref(hdd_slot_array);
                debug_log(DLOG_ERROR, "%s:new object fail", __FUNCTION__);
                return RET_ERR;
            }
            // 查找逻辑盘下的每个物理盘，填充members数组
            redfish_storage_parase_related_array_member(hdd_controller_name, (const guint8 *)hdd_slot_array_pointer,
                (const guint16 *)hdd_enclosure_array_pointer, len_hdd_array_pointer, array_jso);
            g_variant_unref(hdd_slot_array);
            hdd_slot_array = NULL;
            g_variant_unref(hdd_enclosure_array);
            hdd_enclosure_array = NULL;

            json_object_object_add(array_jso, LOGICAL_DRIVE_OBJ_HANDLE,
                json_object_new_int((OBJ_HANDLE)obj_node->data));
            json_object_array_add(result_data, array_jso);
        }
    }

    g_slist_free(obj_list);
    return RET_OK;
}
