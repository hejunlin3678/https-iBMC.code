

#include "redfish_provider.h"
#include "redfish_provider_system.h"

#define ACCESS_FAIL_MASK                0x8000
#define ACCESS_IN_UPDATE_PROGRESS       0x4000
#define GPU_CARD_UUID_STR_LEN           32

#define RF_GPU_NVLINK_PROPERTY_SIZE     5   // GPU的Nvlink的属性个数
LOCAL const gchar *g_nvlink_prop[RF_GPU_NVLINK_PROPERTY_SIZE] = {       // nvlink各个属性名
    PROPERTY_GPU_CARD_NVLINK_STATUS,
    PROPERTY_GPU_CARD_REPLAY_ERROR_COUNT,   PROPERTY_GPU_CARD_RECOVERY_ERROR_COUNT,
    PROPERTY_GPU_CARD_FLIT_CRC_ERROR_COUNT, PROPERTY_GPU_CARD_DATA_CRC_ERROR_COUNT
};

LOCAL gint32 get_processor_gpu_null_property(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 get_processor_gpu_odata_id(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 get_processor_gpu_name(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 get_processor_gpu_id(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 get_processor_gpu_type(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 get_processor_gpu_manufacturer(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 get_processor_gpu_model(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 get_processor_gpu_status(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 get_processor_gpu_firmware_version(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 get_processor_gpu_oem(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_system_processor_gpu_provider[] = {
    {
        RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_odata_context,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_gpu_odata_id,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_COMMON_NAME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_gpu_name,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_COMMON_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_gpu_id,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_TYPE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_gpu_type,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_ARCHITECTURE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_gpu_null_property,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_INSTRUCTIONSET, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_gpu_null_property, 
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_MANUFACTURER, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_gpu_manufacturer,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_MODEL, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_gpu_model,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_PROCESSORID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_gpu_null_property,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_MAXSPEED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_gpu_null_property,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_TOTALCORES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_gpu_null_property, 
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_TOTALTHTREADS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_gpu_null_property,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_SOCKET, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_gpu_null_property,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_STATUS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_gpu_status,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_GPU_CARD_FIRMWARE_VERSION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_gpu_firmware_version,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_COMMON_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_processor_gpu_oem,
        NULL, NULL, ETAG_FLAG_ENABLE
    }
};

void get_system_processor_gpu_provider(PROPERTY_PROVIDER_S **gpu_provider, guint32 *arr_len)
{
    *gpu_provider = g_system_processor_gpu_provider;
    *arr_len = sizeof(g_system_processor_gpu_provider) / sizeof(PROPERTY_PROVIDER_S);
}


LOCAL gint32 get_processor_gpu_null_property(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 get_processor_gpu_odata_id(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    gint32  ret;
    gchar   uri[MAX_URI_LENGTH] = {0};
    gchar   slot[MAX_RSC_NAME_LEN] = {0};

    
    if (provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot failed, ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = get_gpu_rsc_link(i_paras->obj_handle, uri, sizeof(uri), slot);
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(uri);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_string failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_processor_gpu_name(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    guchar slot = 0;
    gchar processor_name[MAX_PROP_VAL_LEN] = {0};

    
    if (provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_GPU_CARD_SLOT, &slot);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte failed, ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = sprintf_s(processor_name, sizeof(processor_name), "GPU%u", slot);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s failed, ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *o_result_jso = json_object_new_string(processor_name);

    return HTTP_OK;
}


LOCAL gint32 get_processor_gpu_id(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    
    if (provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    format_xpu_member_id(i_paras->member_id, strlen(i_paras->member_id));
    *o_result_jso = json_object_new_string(i_paras->member_id);
    return HTTP_OK;
}


LOCAL gint32 get_processor_gpu_type(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    
    if (provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    *o_result_jso = json_object_new_string("GPU");
    return HTTP_OK;
}


LOCAL gint32 get_processor_gpu_manufacturer(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    gchar manu_string[MAX_PROP_VAL_LEN] = {0};

    
    if (provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_property_value_string(i_paras->obj_handle,
        PROPERTY_GPU_CARD_MANU, manu_string, sizeof(manu_string));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_string failed, ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (rf_string_check(manu_string) != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(manu_string);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_string failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


LOCAL gint32 get_processor_gpu_model(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    gchar model_string[MAX_PROP_VAL_LEN] = {0};

    
    if (provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_property_value_string(i_paras->obj_handle,
        PROPERTY_GPU_CARD_MODEL, model_string, sizeof(model_string));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_string failed, ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (rf_string_check(model_string) != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(model_string);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_string failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


LOCAL gint32 get_processor_gpu_status(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    gint32  ret;
    guchar gpu_health = 0;

    
    if (provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_GPU_CARD_HEALTH, &gpu_health);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte failed, ret = %d", __FUNCTION__, ret);
        json_object_object_add(*o_result_jso, RFPROP_GPU_CARD_HEALTH, NULL);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)get_resource_status_property(&gpu_health, NULL, "Enabled", o_result_jso, FALSE);
    return HTTP_OK;
}


LOCAL gint32 get_processor_gpu_firmware_version(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    gint32  ret;
    gchar   value_string[MAX_PROP_VAL_LEN] = {0};

    
    if (provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_GPU_CARD_FIRM_VER,
        value_string, sizeof(value_string));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_string failed, ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (rf_string_check(value_string) != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(value_string);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_string failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


LOCAL void add_property_jso_guint32(OBJ_HANDLE obj_handle,
    const gchar* property, const gchar* rf_property, json_object* o_result_jso)
{
    gint32 ret;
    guint32 prop_val = 0;

    if (property == NULL || rf_property == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return;
    }

    if (obj_handle == 0) {
        json_object_object_add(o_result_jso, rf_property, NULL);
        return;
    }

    
    ret = dal_get_property_value_uint32(obj_handle, property, &prop_val);
    if (ret != DFL_OK) {
        json_object_object_add(o_result_jso, rf_property, NULL);
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_uint32(%s.%s) failed, ret = %d",
            __FUNCTION__, dfl_get_object_name(obj_handle), property, ret);
        return;
    }

    
    if (prop_val == ACCESS_FAIL_MASK || prop_val == ACCESS_IN_UPDATE_PROGRESS) {
        json_object_object_add(o_result_jso, rf_property, NULL);
        return;
    }

    json_object_object_add(o_result_jso, rf_property, json_object_new_int(prop_val));
}


LOCAL void add_property_json_bool_from_guint32(OBJ_HANDLE obj_handle,
    const gchar* property, const gchar* rf_property, json_object* o_result_jso)
{
    guint32 prop_val = 0;
    gint32 ret;

    if (property == NULL || rf_property == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return;
    }

    if (obj_handle == 0) {
        json_object_object_add(o_result_jso, rf_property, NULL);
        return;
    }

    
    ret = dal_get_property_value_uint32(obj_handle, property, &prop_val);
    if (ret != DFL_OK) {
        json_object_object_add(o_result_jso, rf_property, NULL);
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_uint32(%s.%s) failed, ret=%d.",
            __FUNCTION__, dfl_get_object_name(obj_handle), property, ret);
        return;
    }

    if (prop_val == 1) {
        json_object_object_add(o_result_jso, rf_property, json_object_new_boolean(TRUE));
    } else if (prop_val == 0) {
        json_object_object_add(o_result_jso, rf_property, json_object_new_boolean(FALSE));
    } else {
        json_object_object_add(o_result_jso, rf_property, NULL);
    }
}


LOCAL void get_gpu_memory_vendor(OBJ_HANDLE obj_handle, json_object* inventory_jso)
{
    gint32 ret;
    gchar memory_vendor[MAX_PROP_VAL_LEN] = {0};

    ret = dal_get_property_value_string(obj_handle,
        PROPERTY_GPU_CARD_MEMORY_VENDOR, memory_vendor, sizeof(memory_vendor));
    if (ret == DFL_OK) {
        
        if (g_strcmp0(memory_vendor, "H") == 0) {
            json_object_object_add(inventory_jso, RFPROP_GPU_CARD_MEMORY_VENDOR, json_object_new_string("Hynix"));
        } else if (g_strcmp0(memory_vendor, "S") == 0) {
            json_object_object_add(inventory_jso, RFPROP_GPU_CARD_MEMORY_VENDOR, json_object_new_string("Samsung"));
        } else {
            json_object_object_add(inventory_jso, RFPROP_GPU_CARD_MEMORY_VENDOR, NULL);
        }
    } else {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_string failed, ret = %d", __FUNCTION__, ret);
        json_object_object_add(inventory_jso, RFPROP_GPU_CARD_MEMORY_VENDOR, NULL);
    }
}


LOCAL void get_gpu_vid_did(OBJ_HANDLE obj_handle, json_object* inventory_jso)
{
    gint32 ret;
    OBJ_HANDLE pcie_card_handle = 0;

    
    ret = dal_get_specific_object_position(obj_handle, CLASS_PCIE_CARD, &pcie_card_handle);
    if (ret == DFL_OK) {
        rf_add_property_jso_uint16_hex(pcie_card_handle, PROPERTY_PCIE_CARD_VID,
                                       RFPROP_GPU_CARD_PCIE_VID, inventory_jso);
        rf_add_property_jso_uint16_hex(pcie_card_handle, PROPERTY_PCIE_CARD_DID,
                                       RFPROP_GPU_CARD_PCIE_DID, inventory_jso);
        rf_add_property_jso_uint16_hex(pcie_card_handle, PROPERTY_PCIE_CARD_SVID,
                                       RFPROP_GPU_CARD_PCIE_SVID, inventory_jso);
        rf_add_property_jso_uint16_hex(pcie_card_handle, PROPERTY_PCIE_CARD_SDID,
                                       RFPROP_GPU_CARD_PCIE_SDID, inventory_jso);
    } else {
        debug_log(DLOG_ERROR, "%s: get %s same position pcie card failed, ret = %d",
                  __FUNCTION__, dfl_get_object_name(obj_handle), ret);
        json_object_object_add(inventory_jso, RFPROP_GPU_CARD_PCIE_VID, NULL);
        json_object_object_add(inventory_jso, RFPROP_GPU_CARD_PCIE_DID, NULL);
        json_object_object_add(inventory_jso, RFPROP_GPU_CARD_PCIE_SVID, NULL);
        json_object_object_add(inventory_jso, RFPROP_GPU_CARD_PCIE_SDID, NULL);
    }
}


LOCAL gint32 get_gpu_uuid_format(OBJ_HANDLE obj_handle, gchar *gpu_uuid, gint32 length)
{
    gint32 ret;
    gchar uuid[MAX_PROP_VAL_LEN] = {0};  // 未格式化的UUID

    ret = dal_get_property_value_string(obj_handle, PROPERTY_GPU_CARD_UUID, uuid, sizeof(uuid));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_string failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    if (strlen(uuid) != GPU_CARD_UUID_STR_LEN) {
        debug_log(DLOG_DEBUG, "%s: The length of UUID(%zu) is not 32", __FUNCTION__, strlen(uuid));
        return RET_ERR;
    }

    ret = sprintf_s(gpu_uuid, length,
        "GPU-%c%c%c%c%c%c%c%c-%c%c%c%c-%c%c%c%c-%c%c%c%c-%c%c%c%c%c%c%c%c%c%c%c%c",
        uuid[0],  uuid[1],  uuid[2],  uuid[3],  uuid[4],  uuid[5],  uuid[6],  uuid[7],
        uuid[8],  uuid[9],  uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15],
        uuid[16], uuid[17], uuid[18], uuid[19], uuid[20], uuid[21], uuid[22], uuid[23],
        uuid[24], uuid[25], uuid[26], uuid[27], uuid[28], uuid[29], uuid[30], uuid[31]);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL void get_gpu_inventory_property(OBJ_HANDLE obj_handle, const gchar* rf_property, json_object* huawei_jso)
{
    gint32 ret;
    json_object *inventory_jso = NULL;
    gchar gpu_uuid[MAX_PROP_VAL_LEN] = {0};

    inventory_jso = json_object_new_object();
    if (inventory_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        json_object_object_add(huawei_jso, rf_property, NULL);
        return;
    }

    json_object_object_add(huawei_jso, rf_property, inventory_jso);

    
    rf_add_property_jso_string(obj_handle, PROPERTY_GPU_CARD_BOARD_PART_NUMBER,
        RFPROP_GPU_CARD_BOARD_PART_NUMBER, inventory_jso);

    
    rf_add_property_jso_string(obj_handle, PROPERTY_GPU_CARD_GPU_PART_NUMBER,
        RFPROP_GPU_CARD_PART_NUMBER, inventory_jso);

    
    get_gpu_memory_vendor(obj_handle, inventory_jso);

    
    rf_add_property_jso_string(obj_handle, PROPERTY_GPU_CARD_MEMORY_PART_NUMBER,
        RFPROP_GPU_CARD_MEMORY_PART_NUMBER, inventory_jso);

    
    rf_add_property_jso_string(obj_handle, PROPERTY_GPU_CARD_BUILD_DATE,
        RFPROP_GPU_CARD_BUILD_DATE, inventory_jso);

    
    get_gpu_vid_did(obj_handle, inventory_jso);

    
    ret = get_gpu_uuid_format(obj_handle, gpu_uuid, sizeof(gpu_uuid));
    json_object_object_add(inventory_jso, RFPROP_GPU_CARD_UUID,
        (ret == RET_OK) ? json_object_new_string(gpu_uuid) : NULL);
}


LOCAL void get_gpu_power_property(OBJ_HANDLE obj_handle, const gchar* rf_property, json_object* huawei_jso)
{
    json_object *power_jso = NULL;

    power_jso = json_object_new_object();
    if (power_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        json_object_object_add(huawei_jso, rf_property, NULL);
        return;
    }

    json_object_object_add(huawei_jso, rf_property, power_jso);

    
    add_property_jso_guint32(obj_handle, PROPERTY_GPU_CARD_POWER, RFPROP_GPU_CARD_POWER_DRAW_WATTS, power_jso);

    
    add_property_json_bool_from_guint32(obj_handle, PROPERTY_GPU_CARD_POWER_BRAKE_IS_SET, 
        RFPROP_GPU_CARD_POWER_BRAKE_IS_SET, power_jso);

    
    add_property_json_bool_from_guint32(obj_handle, PROPERTY_GPU_CARD_SUFFI_EX_POWER, 
        RFPROP_GPU_CARD_SUFFICIENT_EXTERNAL_POWER, power_jso);
}


LOCAL gint32 get_gpu_nvlink_num_val(OBJ_HANDLE obj_handle, guint32 *nvlink_num)
{
#define MAX_NVLINKNUM 256
    gint32 ret;

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_GPU_CARD_NVLINK_NUM, nvlink_num);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_uint32 failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    if (*nvlink_num == 0 || *nvlink_num == ACCESS_FAIL_MASK || *nvlink_num == ACCESS_IN_UPDATE_PROGRESS ||
        *nvlink_num > MAX_NVLINKNUM) {
        debug_log(DLOG_DEBUG, "%s: the nvlink length(%d) is incorrect", __FUNCTION__, *nvlink_num);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL void get_gpu_arr_guint32_property(OBJ_HANDLE obj_handle, const gchar* property,
    guint32 nvlink_num, guint32 **nvlink_prop, gsize *nvlink_prop_len)
{
    gint32 ret;
    GVariant *gv_nvlink_prop = NULL;
    const guint32 *arr = NULL;
    gsize arr_len = 0;

    ret = dfl_get_property_value(obj_handle, property, &gv_nvlink_prop);
    if (ret != DFL_OK || gv_nvlink_prop == NULL) {
        debug_log(DLOG_ERROR, "%s: dfl_get_property_value failed, ret = %d", __FUNCTION__, ret);
        goto EXIT;
    }

    arr = (const guint32 *)g_variant_get_fixed_array(gv_nvlink_prop, &arr_len, sizeof(guint32));
    if (arr == NULL) {
        debug_log(DLOG_ERROR, "%s: g_variant_get_fixed_array failed", __FUNCTION__);
        goto EXIT;
    }

    
    if (arr_len != nvlink_num) {
        debug_log(DLOG_DEBUG, "%s: %s length(%zu) is not equal to nvlink_num(%d)",
            __FUNCTION__, property, arr_len, nvlink_num);
        goto EXIT;
    }

    *nvlink_prop = (guint32 *)g_malloc(arr_len * sizeof(guint32));
    if (*nvlink_prop == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc failed", __FUNCTION__);
        goto EXIT;
    }

    ret = memcpy_s(*nvlink_prop, arr_len * sizeof(guint32), arr, arr_len * sizeof(guint32));
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s failed, ret = %d", __FUNCTION__, ret);
        goto EXIT;
    }

    g_variant_unref(gv_nvlink_prop);
    gv_nvlink_prop = NULL;
    *nvlink_prop_len = arr_len;
    return;

EXIT:
    if (gv_nvlink_prop != NULL) {
        g_variant_unref(gv_nvlink_prop);
        gv_nvlink_prop = NULL;
    }
    *nvlink_prop_len = 0;
}


LOCAL void get_gpu_nvlink_property(json_object *nvlink_jso, const gchar *nvlink_prop, guint32 prop_val)
{
    // LinkStatus 需要转义为Up(1)或Down(0)
    if (g_strcmp0(nvlink_prop, PROPERTY_GPU_CARD_NVLINK_STATUS) == 0) {
        if (prop_val == 0) {
            json_object_object_add(nvlink_jso, nvlink_prop, json_object_new_string("Down"));
        } else if (prop_val == 1) {
            json_object_object_add(nvlink_jso, nvlink_prop, json_object_new_string("Up"));
        } else {
            json_object_object_add(nvlink_jso, nvlink_prop, NULL);
        }
        return;
    }

    json_object_object_add(nvlink_jso, nvlink_prop, json_object_new_int((int32_t)prop_val));
}


LOCAL void get_gpu_nvlink_arr_property(OBJ_HANDLE obj_handle, const gchar* rf_property, json_object* status_jso)
{
    gint32 ret;
    gsize idx, jdx;
    json_object *nvlink_arr_jso = NULL;
    json_object *nvlink_jso = NULL;
    guint32 nvlink_num = 0; // NvLinkNum属性的大小

    guint32 *nvlink_prop_val[RF_GPU_NVLINK_PROPERTY_SIZE];    // NvLink各个属性的数据
    gsize nvlink_prop_len[RF_GPU_NVLINK_PROPERTY_SIZE];       // NvLink各个属性的数组长度

    for (jdx = 0; jdx < RF_GPU_NVLINK_PROPERTY_SIZE; ++jdx) { // 初始化
        nvlink_prop_val[jdx] = NULL;
        nvlink_prop_len[jdx] = 0;
    }

    ret = get_gpu_nvlink_num_val(obj_handle, &nvlink_num);
    if (ret != RET_OK) {
        json_object_object_add(status_jso, rf_property, NULL);
        return;
    }

    
    nvlink_arr_jso = json_object_new_array();
    json_object_object_add(status_jso, rf_property, nvlink_arr_jso);

    
    for (jdx = 0; jdx < RF_GPU_NVLINK_PROPERTY_SIZE; ++jdx) {
        get_gpu_arr_guint32_property(obj_handle, g_nvlink_prop[jdx], nvlink_num,
            &nvlink_prop_val[jdx], &nvlink_prop_len[jdx]);
    }

    for (idx = 0; idx < nvlink_num; ++idx) {
        nvlink_jso = NULL;
        nvlink_jso = json_object_new_object();
        if (nvlink_jso == NULL) {
            debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
            continue;
        }
        json_object_array_add(nvlink_arr_jso, nvlink_jso);

        for (jdx = 0; jdx < RF_GPU_NVLINK_PROPERTY_SIZE; ++jdx) {
            
            if (nvlink_num != nvlink_prop_len[jdx]) {
                json_object_object_add(nvlink_jso, g_nvlink_prop[jdx], NULL);
                continue;
            }

            
            get_gpu_nvlink_property(nvlink_jso, g_nvlink_prop[jdx], nvlink_prop_val[jdx][idx]);
        }
    }

    for (jdx = 0; jdx < RF_GPU_NVLINK_PROPERTY_SIZE; ++jdx) {
        if (nvlink_prop_val[jdx] != NULL) {
            g_free(nvlink_prop_val[jdx]);
            nvlink_prop_val[jdx] = NULL;
        }
    }
}


LOCAL void get_gpu_error_status_property(OBJ_HANDLE obj_handle, const gchar* rf_property, json_object* status_jso)
{
    json_object *error_status_jso = NULL;
    json_object *retired_page_jso = NULL;
    json_object *single_error_jso = NULL;
    json_object *double_error_jso = NULL;

    error_status_jso = json_object_new_object();
    if (error_status_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        json_object_object_add(status_jso, rf_property, NULL);
        return;
    }

    json_object_object_add(status_jso, rf_property, error_status_jso);

    
    add_property_json_bool_from_guint32(obj_handle, PROPERTY_GPU_CARD_ECC_MODE_ENABLED,
        RFPROP_GPU_CARD_ECC_MODE_ENABLED, error_status_jso);

    
    add_property_json_bool_from_guint32(obj_handle, PROPERTY_GPU_CARD_ECC_MODE_PEND_ENSBLED,
        RFPROP_GPU_CARD_ECC_MODE_PENDING_ENABLED, error_status_jso);

    
    add_property_json_bool_from_guint32(obj_handle, PROPERTY_GPU_CARD_GPU_RESET_REQUIRED,
        RFPROP_GPU_CARD_RESET_REQUIRED, error_status_jso);

    
    retired_page_jso = json_object_new_object();
    if (retired_page_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        json_object_object_add(error_status_jso, RFPROP_GPU_CARD_RETIRED_PAGES, NULL);
        return;
    }
    json_object_object_add(error_status_jso, RFPROP_GPU_CARD_RETIRED_PAGES, retired_page_jso);

    
    single_error_jso = json_object_new_object();
    if (single_error_jso != NULL) {
        json_object_object_add(retired_page_jso, RFPROP_GPU_CARD_DUE_TO_SINGLE_BIT_ERRORS, single_error_jso);
        add_property_jso_guint32(obj_handle, PROPERTY_GPU_CARD_SBE_COUNT,
            RFPROP_GPU_CARD_PAGE_COUNT, single_error_jso);
    } else {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        json_object_object_add(retired_page_jso, RFPROP_GPU_CARD_DUE_TO_SINGLE_BIT_ERRORS, NULL);
    }

    double_error_jso = json_object_new_object();
    if (double_error_jso != NULL) {
        json_object_object_add(retired_page_jso, RFPROP_GPU_CARD_DUE_TO_DOUBLE_BIT_ERRORS, double_error_jso);
        add_property_jso_guint32(obj_handle, PROPERTY_GPU_CARD_DBE_COUNT,
            RFPROP_GPU_CARD_PAGE_COUNT, double_error_jso);
    } else {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        json_object_object_add(retired_page_jso, RFPROP_GPU_CARD_DUE_TO_DOUBLE_BIT_ERRORS, NULL);
    }
}


LOCAL void get_gpu_utilization_property(OBJ_HANDLE obj_handle, const gchar* rf_property, json_object* status_jso)
{
    json_object *summary_jso = NULL;

    summary_jso = json_object_new_object();
    if (summary_jso == NULL) {
        debug_log(DLOG_INFO, "%s: json_object_new_object failed", __FUNCTION__);
        json_object_object_add(status_jso, rf_property, NULL);
        return;
    }
    json_object_object_add(status_jso, rf_property, summary_jso);
    get_oem_prop_threshold_uint16(obj_handle, PROPERTY_GPU_CARD_GPUUTILIZATION,
        RFPROP_GPU_CARD_UTILIZATION_GPU, summary_jso);
    get_oem_prop_threshold_uint16(obj_handle, PROPERTY_GPU_CARD_MEMORYUTILIZATION,
        RFPROP_GPU_CARD_UTILIZATION_MEM, summary_jso);
}


LOCAL void get_gpu_statistics_property(OBJ_HANDLE obj_handle, const gchar* rf_property, json_object* huawei_jso)
{
    json_object *status_jso = NULL;

    status_jso = json_object_new_object();
    if (status_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        json_object_object_add(huawei_jso, rf_property, NULL);
        return;
    }

    json_object_object_add(huawei_jso, rf_property, status_jso);

    
    get_gpu_error_status_property(obj_handle, RFPROP_GPU_CARD_ERROR_STATUS, status_jso);

    
    get_gpu_nvlink_arr_property(obj_handle, RFPROP_GPU_CARD_NVLINK, status_jso);
    
    get_gpu_utilization_property(obj_handle, RFPROP_GPU_CARD_UTILIZATION, status_jso);
}


LOCAL void get_gpu_thermals_property(OBJ_HANDLE obj_handle, const gchar* rf_property, json_object* huawei_jso)
{
    gint32 ret;
    json_object *thermals_jso = NULL;
    guint32 thermals_altert_status = 0;

    thermals_jso = json_object_new_object();
    if (thermals_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        json_object_object_add(huawei_jso, rf_property, NULL);
        return;
    }

    json_object_object_add(huawei_jso, rf_property, thermals_jso);

    
    ret = dal_get_property_value_uint32(obj_handle,
        PROPERTY_GPU_CARD_THERMALS_ALTERT_STATE, &thermals_altert_status);
    if (ret == DFL_OK) {
        if (thermals_altert_status == 0) {
            json_object_object_add(thermals_jso, RFPROP_GPU_CARD_THERMALS_ALERT_STATE,
                json_object_new_string("NoPending"));
        } else if (thermals_altert_status == 1) {
            json_object_object_add(thermals_jso, RFPROP_GPU_CARD_THERMALS_ALERT_STATE,
                json_object_new_string("Pending"));
        } else {
            json_object_object_add(thermals_jso, RFPROP_GPU_CARD_THERMALS_ALERT_STATE, NULL);
        }
    } else {
        json_object_object_add(thermals_jso, RFPROP_GPU_CARD_THERMALS_ALERT_STATE, NULL);
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_uint32 failed, ret = %d", __FUNCTION__, ret);
    }

    
    add_property_jso_guint32(obj_handle, PROPERTY_GPU_CARD_PRIMARY_GPU_TEMP_CELSIUS,
        RFPROP_GPU_CARD_PRIMARY_GPU_TEMP_CELSIUS, thermals_jso);

    
    add_property_jso_guint32(obj_handle, PROPERTY_GPU_CARD_MEMORY_TMP_CELSIUS,
        RFPROP_GPU_CARD_MEMORY_TEMP_CELISIUS, thermals_jso);
}


void get_xpu_devicelocator_position(OBJ_HANDLE obj_handle, json_object* huawei_jso)
{
    gint32 ret;
    OBJ_HANDLE pcie_card_handle = 0;
    OBJ_HANDLE comp_handle = 0;

    ret = dal_get_specific_object_position(obj_handle, CLASS_PCIE_CARD, &pcie_card_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: dal_get_specific_object_position failed, ret = %d", __FUNCTION__, ret);
        goto EXIT;
    }

    ret = dfl_get_referenced_object(pcie_card_handle, PROPERTY_PCIE_CARD_REF_COMPONENT, &comp_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: dfl_get_referenced_object failed, ret = %d", __FUNCTION__, ret);
        goto EXIT;
    }

    rf_add_property_jso_string(comp_handle, PROPERTY_COMPONENT_DEVICE_NAME, RFPROP_GPU_CARD_DEVICE_LOCATOR, huawei_jso);
    rf_add_property_jso_string(comp_handle, PROPERTY_COMPONENT_LOCATION, RFPROP_GPU_CARD_POSITION, huawei_jso);
    return;

EXIT:
    json_object_object_add(huawei_jso, RFPROP_GPU_CARD_DEVICE_LOCATOR, NULL);
    json_object_object_add(huawei_jso, RFPROP_GPU_CARD_POSITION, NULL);
}


void get_gpu_oem_huawei(OBJ_HANDLE obj_handle, json_object* huawei_jso)
{
    
    rf_add_property_jso_byte(obj_handle, PROPERTY_GPU_CARD_SLOT, RFPROP_GPU_CARD_SLOT_NUMBER, huawei_jso);

    
    get_xpu_devicelocator_position(obj_handle, huawei_jso);

    
    rf_add_property_jso_string(obj_handle, PROPERTY_GPU_CARD_SER_NUM,
        RFPROP_GPU_CARD_SERIAL_NUMBER, huawei_jso);

    
    rf_add_property_jso_string(obj_handle, PROPERTY_GPU_CARD_INFO_ROM_VERSION,
        RFPROP_GPU_CARD_INFO_ROM_VERSION, huawei_jso);

    
    get_gpu_inventory_property(obj_handle, RFPROP_GPU_CARD_INVENTORY, huawei_jso);

    
    get_gpu_power_property(obj_handle, RFPROP_GPU_CARD_POWER, huawei_jso);

    
    get_gpu_statistics_property(obj_handle, RFPROP_GPU_CARD_STATISTICS, huawei_jso);

    
    get_gpu_thermals_property(obj_handle, RFPROP_GPU_CARD_THERMALS, huawei_jso);
}


LOCAL gint32 get_processor_gpu_oem(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    json_object* huawei_jso = NULL;

    
    if (provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    huawei_jso = json_object_new_object();
    if (huawei_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        json_object_put(*o_result_jso);
        *o_result_jso = NULL;
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei_jso);
    get_gpu_oem_huawei(i_paras->obj_handle, huawei_jso);
    return HTTP_OK;
}

