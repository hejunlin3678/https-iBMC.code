
#include "redfish_provider.h"
#include "redfish_provider_with_cucc_customized.h"

gint32 get_card_type_from_uri(const gchar *rsc_uri, guint8 *card_type, gulong *slot_id)
{
#define BASE_TEN 10
    if (rsc_uri == NULL || card_type == NULL || slot_id == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }
    char *npu = strcasestr(rsc_uri, CLASS_NPU);
    char *ptr = NULL;
    *slot_id = strtoul(npu + strlen(CLASS_NPU), &ptr, BASE_TEN);
    if (ptr == NULL || *ptr != '\0') {
        debug_log(DLOG_ERROR, "%s: parse npu slot with %s fail!", __FUNCTION__, npu);
        return RET_ERR;
    }
    *card_type = COMPONENT_TYPE_NPU;
    return RET_OK;
}

gint32 cucc_customized_get_npu_handle(gulong card_slot, OBJ_HANDLE *obj_handle)
{
    if (obj_handle == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }
    OBJ_HANDLE obj = 0;
    gint32 ret = dal_get_specific_object_byte(CLASS_NPU, PROPERTY_NPU_ID, card_slot, &obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: fail to obtain npu handle, slot is %ld, ret = %d", __FUNCTION__, card_slot, ret);
        return RET_ERR;
    }
    *obj_handle = obj;
    return RET_OK;
}

gboolean check_npu_handle_with_cucc_customized(OBJ_HANDLE obj_handle)
{
    if (!dal_is_customized_by_cucc()) {
        return FALSE;
    }
    gchar name[MAX_NAME_SIZE + 1] = {0};
    (void)dfl_get_class_name(obj_handle, name, (MAX_NAME_SIZE + 1));
    if (g_ascii_strcasecmp(name, CLASS_NPU) != 0) {
        return FALSE;
    }
    return TRUE;
}

LOCAL void cucc_customized_add_video_memory(OBJ_HANDLE npu_handle, json_object *jso)
{
#define MB_SCALE 1024
    guint32 memory_capacity = 0;
    gint32 ret = dal_get_property_value_uint32(npu_handle, PROPERTY_NPU_MEMORY_CAPACITY_KB, &memory_capacity);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: get memory capacity failed, ret %d", __FUNCTION__, ret);
        return;
    }
    memory_capacity /= MB_SCALE;
    (void)json_object_object_add(jso, RFPROP_CUCC_NPU_MEMORY_SIZE_MIB, json_object_new_int(memory_capacity));
}

gint32 get_cucc_pciedevices_oem_property(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso)
{
    if (i_paras == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid parameter!", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object *huawei_jso = json_object_new_object();
    if (huawei_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    OBJ_HANDLE npu_handle = i_paras->obj_handle;

    json_object *result = NULL;

    (void)rf_get_property_value_string_as_jso(npu_handle, PROPERTY_NPU_NAME, &result);
    (void)json_object_object_add(huawei_jso, RFPROP_DEVICELOCATOR, result);

    (void)rf_get_property_value_uint8_as_jso(npu_handle, PROPERTY_NPU_ID, &result);
    (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_SLOT_NUM, result);

    (void)json_object_object_add(huawei_jso, RFPROP_POSITION, json_object_new_string(NPU_POSITION));
    
    (void)json_object_object_add(huawei_jso, RFPROP_FUNCTIONTYPE, json_object_new_string(RFPROP_PCIECARD_TYPE_NPU));

    (void)json_object_object_add(huawei_jso, RFPROP_PCIECARDTYPE, json_object_new_string(RFPROP_PCIECARD_TYPE_NPU));

    cucc_customized_add_video_memory(npu_handle, huawei_jso);

    // 暂时写死
#define BANDWIDTH 204.8
    (void)json_object_object_add(huawei_jso, RFPROP_CUCC_NPU_MEMORY_BANDWIDTH,
        ex_json_object_new_double(BANDWIDTH, "%.1f"));

    // 暂时写死，风冷320W
#define CAPACITY_WATTS 320
    (void)json_object_object_add(huawei_jso, RFPROP_POWER_CAPACITY_WATTS, json_object_new_int(CAPACITY_WATTS));

    (void)rf_get_property_value_uint16_as_jso(npu_handle, PROPERTY_NPU_POWER, &result);
    (void)json_object_object_add(huawei_jso, RFPROP_DEVICE_CURRENT_POWER_VALUE, result);

    (void)rf_get_property_value_uint16_as_jso(npu_handle, PROPERTY_NPU_AICORE_TEMP, &result);
    (void)json_object_object_add(huawei_jso, RFPROP_SENSOR_READING_CELSIUS, result);

    // 屏蔽index.json的模板内容
    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        (void)json_object_put(huawei_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    (void)json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei_jso);
    return HTTP_OK;
}