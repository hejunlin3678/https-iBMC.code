
#include "redfish_provider.h"
#include "redfish_provider_system.h"

#define RF_PROCESSOR_VALUE_MAXLEN 128
#define RF_PROCESSOR_MAXSPEED_MAXLEN 128
#define ECC_TYPE_SINGLE_BIT     1
#define ECC_TYPE_MULTI_BIT      2

#define ECC_INFO_DATA_MAX_CNT 64

#define CPU_MODEL_KUNPENG   "Kunpeng"
#define CPU_MODEL_INTEL     "Intel"
typedef struct {
    guint32 phy_addr_l[ECC_INFO_DATA_MAX_CNT];
    guint32 phy_addr_h[ECC_INFO_DATA_MAX_CNT];
    guint32 stack_pc_id[ECC_INFO_DATA_MAX_CNT];
    guint32 row_column[ECC_INFO_DATA_MAX_CNT];
    guint32 bank[ECC_INFO_DATA_MAX_CNT];
    guint32 err_cnt[ECC_INFO_DATA_MAX_CNT];
    guint32 time_stamp[ECC_INFO_DATA_MAX_CNT];
    guint8 type;
    guint32 count;
} NPU_ECC_INFO;

typedef struct {
    const GVariantType *type;   // 获取的属性元素数据类型
    gpointer elements;          // 保存属性取值
    gsize element_size;         // 属性元素占用空间
    const char *prop_name;      // 属性名
} SET_ECC_INFO_PROP;

LOCAL gint32 get_processor_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_processor_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_processor_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_processor_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_processor_architecture(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_processor_instructionset(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_processor_manufacturer(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_processor_model(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_processor_processorid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_processor_maxspeedmhz(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_processor_totalcores(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_processor_totalthreads(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_processor_socket(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_processor_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_processor_firmware_version(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 get_processor_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_processor_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_system_processor_provider[] = {
    {
        RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, PROPERTY_CPU_PHYSIC_ID,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_odata_context,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ODATA_ID, MAP_PROPERTY_NULL, PROPERTY_CPU_PHYSIC_ID,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_odata_id,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_COMMON_NAME, MAP_PROPERTY_NULL, PROPERTY_CPU_NAME,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_name,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_COMMON_ID, MAP_PROPERTY_NULL, PROPERTY_CPU_PHYSIC_ID,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_id,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESSOR_TYPE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_type,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_ARCHITECTURE, MAP_PROPERTY_NULL, PROPERTY_CPU_ARCHITECTURE,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_architecture,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_INSTRUCTIONSET, MAP_PROPERTY_NULL, PROPERTY_CPU_INSTRUCTIONSET,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_instructionset,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_MANUFACTURER, MAP_PROPERTY_NULL, PROPERTY_CPU_MANUFACTURER,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_manufacturer,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_MODEL, MAP_PROPERTY_NULL, PROPERTY_CPU_VERSION,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_model,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_PROCESSORID, MAP_PROPERTY_NULL, PROPERTY_CPU_PROCESSORID,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_processorid,
        NULL, NULL, ETAG_FLAG_ENABLE},
    {
        RFPROP_SYSTEM_PROCESEEOR_MAXSPEED, MAP_PROPERTY_NULL, PROPERTY_CPU_MAX_SPEED,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_maxspeedmhz,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_TOTALCORES, MAP_PROPERTY_NULL, PROPERTY_CPU_CORE_COUNT,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_totalcores,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_TOTALTHTREADS, MAP_PROPERTY_NULL, PROPERTY_CPU_THREAD_COUNT,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_totalthreads,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_SOCKET, MAP_PROPERTY_NULL, PROPERTY_CPU_PHYSIC_ID,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_socket,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SYSTEM_PROCESEEOR_STATUS, MAP_PROPERTY_NULL, PROPERTY_CPU_HEALTH,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_status,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_GPU_CARD_FIRMWARE_VERSION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_processor_firmware_version,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_COMMON_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_processor_oem, set_processor_oem,
        NULL, ETAG_FLAG_ENABLE
    },
};

typedef struct tag_rf_npu_utl {
    gchar *npu_prop_name;
    gchar *npu_utl_rf;
} RF_NPU_UTL;


gint32 get_processor_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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

    iRet = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, SYSTEMS_PROCE_METADATA, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string(odata_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 rfprocessor_string_check(gchar *data, guint32 len)
{
    if ((NULL == data) || (0 == len)) {
        return VOS_ERR;
    }
    data[len - 1] = '\0';

    if (0 == strlen(data) || 0 == g_strcmp0("N/A", data) || 0 == g_strcmp0("Unknown", data)) {
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gchar *rfprocessor_cpu_state_to_string(guchar cpu_state)
{
    if (0 == cpu_state) {
        return "Enabled";
    } else if (1 == cpu_state) {
        return "Disabled";
    } else {
        return "\0";
    }
}

gint32 rfprocessor_maxspeed_to_intmhz(gchar *speed, guint32 len)
{
    gint32 ret_num = 0;

    if (NULL == speed || len < 3) {
        return 0;
    }

    if ((*(speed + len - 3) == 'M') || (*(speed + len - 3) == 'm')) {
        if (redfish_strip_to_int(speed, len) != VOS_OK) {
            return ret_num;
        }

        if (vos_str2int(speed, 10, &ret_num, NUM_TPYE_UINT32)) {
            return 0;
        }
    }

    return ret_num;
}


LOCAL PROPERTY_PROVIDER_S *rfprocessor_get_self_provider(PROVIDER_PARAS_S *i_paras)
{
    return_val_if_fail((NULL != i_paras) && (i_paras->index >= 0) &&
        (i_paras->index < (gint32)(sizeof(g_system_processor_provider) / sizeof(PROPERTY_PROVIDER_S))),
        (PROPERTY_PROVIDER_S *)NULL);
    return &g_system_processor_provider[i_paras->index];
}


LOCAL gint32 rfprocessor_get_property_string(PROPERTY_PROVIDER_S *self, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar value_string[RF_PROCESSOR_VALUE_MAXLEN] = {0};

    
    if (self == NULL || o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!(i_paras->is_satisfy_privi)) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    ret = dal_get_property_value_string(i_paras->obj_handle, self->pme_prop_name, value_string, sizeof(value_string));
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get property_data fail, ret:%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (rfprocessor_string_check(value_string, sizeof(value_string)) != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_string(value_string);
    return HTTP_OK;
}


LOCAL gint32 rfprocessor_get_property_uint16(PROPERTY_PROVIDER_S *self, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gushort value_ushort = 0;

    
    if (self == NULL || o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!(i_paras->is_satisfy_privi)) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    ret = dal_get_property_value_uint16(i_paras->obj_handle, self->pme_prop_name, &value_ushort);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get property_data fail, ret:%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (value_ushort == 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_int(value_ushort);
    return HTTP_OK;
}


gint32 rfprocessor_get_sensor_reading(OBJ_HANDLE obj_handle, gdouble *pchReading)
{
    if ((NULL == pchReading) || (0 == obj_handle)) {
        return VOS_ERR;
    }

    int iRet;
    gdouble reading_value;
    guchar reading_status = 0;
    EVENT_WORK_STATE_S *sensor_reading_status = NULL;

    iRet = dal_get_property_value_double(obj_handle, PROPERTY_THR_SENSOR_READING_CONVERT, &reading_value);
    if (VOS_OK != iRet) {
        return VOS_ERR;
    }

    iRet = dal_get_property_value_byte(obj_handle, PROPERTY_THS_SENSOR_READING_STATUS, &reading_status);
    if (VOS_OK != iRet) {
        return VOS_ERR;
    }

    sensor_reading_status = (EVENT_WORK_STATE_S *)&reading_status;

    if (!sensor_reading_status->disable_scanning || !sensor_reading_status->disable_scanning_local ||
        !sensor_reading_status->disable_all ||
        !sensor_reading_status->initial_update_progress 
        || !sensor_reading_status->disable_access_error) {
        return VOS_ERR;
    } else {
        *pchReading = reading_value;
    }

    return VOS_OK;
}


LOCAL gint32 rfprocessor_get_temperature(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso)
{
    gint32 ret;
    gchar name_string[RF_PROCESSOR_VALUE_MAXLEN] = {0};
    gdouble value = 0.0;
    OBJ_HANDLE obj_handle = 0;
    gchar sensor_name[MAX_NAME_SIZE] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    
    if (i_paras == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = snprintf_s(name_string, RF_PROCESSOR_VALUE_MAXLEN, RF_PROCESSOR_VALUE_MAXLEN - 1, "CPU%s Core Rem",
        i_paras->member_id);
    return_val_do_info_if_expr(ret <= VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: snprintf_s  error.", __FUNCTION__, __LINE__));

    ret = dfl_get_object_list(CLASS_THRESHOLD_SENSOR, &obj_list);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: dfl_get_object_list fail.\n", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        obj_handle = (OBJ_HANDLE)obj_node->data;
        ret = dal_get_property_value_string(obj_handle, PROPERTY_THR_SENSOR_SENSOR_NAME, sensor_name,
            sizeof(sensor_name));
        if (VOS_OK != ret) {
            g_slist_free(obj_list);
            debug_log(DLOG_ERROR, "Get SensorName failed:%d;\n", ret);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        if (0 == g_strcmp0(name_string, sensor_name)) {
            break;
        }
    }
    
    g_slist_free(obj_list);
    

    ret = rfprocessor_get_sensor_reading(obj_handle, &value);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));

    
    *o_result_jso = ex_json_object_new_double(value, "%.2f");

    return HTTP_OK;
}


LOCAL gint32 rfprocessor_get_enablementstatus(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso)
{
    gint32 ret;
    guchar value_byte = 0;
    OBJ_HANDLE obj_handle = 0;
    guchar cpu_id = 0;
    json_bool cpu_state = FALSE;

    
    if (NULL == i_paras || NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = vos_str2int(i_paras->member_id, 10, &cpu_id, NUM_TPYE_UCHAR);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    ret = dal_get_specific_object_byte(CLASS_CPU, PROPERTY_CPU_PHYSIC_ID, cpu_id, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_CPU_DISABLE_CPU_SW, &value_byte);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));

    
    if (0 == value_byte) {
        cpu_state = TRUE;
    } else if (1 == value_byte) {
        cpu_state = FALSE;
    } else {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_boolean(cpu_state);

    return HTTP_OK;
}


LOCAL gint32 redfish_system_processor_uri_check_cpu(const gchar* i_member_id, OBJ_HANDLE* obj_handle)
{
    gint32  ret;
    guchar  cpu_id;
    gchar   cpu_id_string[3] = {0};
    GSList* obj_list = NULL;
    GSList* obj_node = NULL;
    OBJ_HANDLE cpu_handle = 0;

    
    ret = dfl_get_object_list(CLASS_CPU, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_INFO, "%s: get obj_list failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        cpu_handle = (OBJ_HANDLE)(intptr_t)obj_node->data;
        if (!cpu_is_present(cpu_handle)) {
            continue;
        }

        ret = dal_get_property_value_byte(cpu_handle, PROPERTY_CPU_PHYSIC_ID, &cpu_id);
        if (ret != DFL_OK) {
            continue;
        }

        ret = sprintf_s(cpu_id_string, sizeof(cpu_id_string), "%u", cpu_id);
        if (ret <= 0) {
            continue;
        }

        if (g_strcmp0(cpu_id_string, i_member_id) == 0) {
            *obj_handle = cpu_handle;
            g_slist_free(obj_list);
            return RET_OK;
        }
    }
    g_slist_free(obj_list);
    return RET_ERR;
}

gint32 construct_npu_name(OBJ_HANDLE npu_handle, gchar *npu_name, size_t npu_name_len)
{
    gint32 ret;
    OBJ_HANDLE fpga_handle = 0;
    guint8 npu_id = 0;
    guint8 slot_id = 0;
    debug_log(DLOG_DEBUG, "%s: started", __FUNCTION__);
    ret = dal_get_property_value_byte(npu_handle, PROPERTY_NPU_ID, &npu_id);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get npu id for NPU %s failed with %d",
            __FUNCTION__, dfl_get_object_name(npu_handle), ret);
        return RET_ERR;
    }
    ret = dal_get_specific_object_position(npu_handle, CLASS_PCIE_FPGA_CARD, &fpga_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get fpga failed for NPU %s failed with %d",
            __FUNCTION__, dfl_get_object_name(npu_handle), ret);
        return RET_ERR;
    }
    ret = dal_get_property_value_byte(fpga_handle, PROPERTY_FPGA_CARD_SLOT, &slot_id);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get slot for NPU %s failed with %d",
            __FUNCTION__, dfl_get_object_name(npu_handle), ret);
        return RET_ERR;
    }
    ret = sprintf_s(npu_name, npu_name_len, "NPU%u-%u", slot_id, npu_id);
    if (ret <= 0) {
        debug_log(DLOG_DEBUG, "%s: sprintf_s failed with %d",
            __FUNCTION__, ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 __dboard_npu_uri_check(const gchar* i_member_id, OBJ_HANDLE* obj_handle, GSList* obj_list)
{
    gint32  ret;
    GSList* obj_node = NULL;
    OBJ_HANDLE npu_handle = 0;
    gchar npu_name[MAX_RSC_NAME_LEN] = {0};
    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        npu_handle = (OBJ_HANDLE)(intptr_t)obj_node->data;
        if (!npu_is_present(npu_handle)) {
            continue;
        }

        ret = dal_get_property_value_string(npu_handle, PROPERTY_NPU_NAME, npu_name, sizeof(npu_name));
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: dal_get_property_value_string failed, ret = %d", __FUNCTION__, ret);
            continue;
        }

        if (g_ascii_strcasecmp(i_member_id, npu_name) == 0) {
            *obj_handle = npu_handle;
            return RET_OK;
        }
    }
    return RET_ERR;
}

LOCAL gint32 __pcie_npu_uri_check(const gchar* i_member_id, OBJ_HANDLE* obj_handle, GSList* obj_list)
{
    gint32  ret;
    GSList* obj_node = NULL;
    OBJ_HANDLE npu_handle = 0;
    gchar npu_name[MAX_RSC_NAME_LEN] = {0};
    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        npu_handle = (OBJ_HANDLE)(intptr_t)obj_node->data;
        if (!npu_is_present(npu_handle)) {
            continue;
        }

        ret = construct_npu_name(npu_handle, npu_name, sizeof(npu_name));
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: construct_npu_name failed, ret = %d", __FUNCTION__, ret);
            continue;
        }

        if (g_ascii_strcasecmp(i_member_id, npu_name) == 0) {
            *obj_handle = npu_handle;
            return RET_OK;
        }
    }
    return RET_ERR;
}


LOCAL gint32 redfish_system_processor_uri_check_npu(const gchar* i_member_id, OBJ_HANDLE* obj_handle)
{
    gint32  ret;
    GSList* obj_list = NULL;

    
    ret = dfl_get_object_list(CLASS_NPU, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_INFO, "%s: get obj_list failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    if (dal_is_pcie_npu() == TRUE) {
        ret = __pcie_npu_uri_check(i_member_id, obj_handle, obj_list);
    } else {
        ret = __dboard_npu_uri_check(i_member_id, obj_handle, obj_list);
    }
    g_slist_free(obj_list);
    return ret;
}


LOCAL gint32 redfish_system_processor_uri_check_gpu(const gchar* i_member_id, OBJ_HANDLE* obj_handle)
{
    gint32  ret;
    gchar   gpu_card_member_id[MAX_RSC_NAME_LEN] = {0};
    guchar  slot = 0;
    OBJ_HANDLE gpu_handle = 0;
    GSList* obj_list = NULL;
    GSList* obj_node = NULL;

    
    ret = dfl_get_object_list(CLASS_GPU_CARD, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_INFO, "%s: get obj_list failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        gpu_handle = (OBJ_HANDLE)(intptr_t)(obj_node->data);
        if (!gpu_is_present(gpu_handle)) {
            continue;
        }

        ret = dal_get_property_value_byte(gpu_handle, PROPERTY_GPU_CARD_SLOT, &slot);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte(%s) failed, ret = %d",
                __FUNCTION__, PROPERTY_GPU_CARD_SLOT, ret);
            continue;
        }

        ret = sprintf_s(gpu_card_member_id, sizeof(gpu_card_member_id),
            MEMBER_ID_FORMAT_SYSTEM_REPROCESSORS_GPU, slot);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: sprintf_s failed, ret = %d", __FUNCTION__, ret);
            continue;
        }

        if (g_ascii_strcasecmp(i_member_id, gpu_card_member_id) == 0) {
            *obj_handle = gpu_handle;
            g_slist_free(obj_list);
            return RET_OK;
        }
    }
    g_slist_free(obj_list);
    return RET_ERR;
}


LOCAL gint32 redfish_system_processor_uri_check(const gchar *i_member_id, const gchar *i_uri, OBJ_HANDLE *obj_handle)
{
    gint32 ret;

    
    if (i_member_id == NULL || i_uri == NULL || obj_handle == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = redfish_system_processor_uri_check_cpu(i_member_id, obj_handle);
    if (ret == RET_OK) {
        return ret;
    }

    ret = redfish_system_processor_uri_check_npu(i_member_id, obj_handle);
    if (ret == RET_OK) {
        return ret;
    }

    ret = redfish_system_processor_uri_check_gpu(i_member_id, obj_handle);
    if (ret == RET_OK) {
        return ret;
    }

    
    debug_log(DLOG_DEBUG, "%s: match_flag = ERR", __FUNCTION__);
    return RET_ERR;
}


LOCAL gint32 get_processor_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet;

    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    
    gint32 processor_id;
    
    gchar class_name[MAX_CLASS_NAME] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));

    
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));
    

    ret = dfl_get_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:get class name failed, ret:%d", __FUNCTION__, ret));

    if (g_strcmp0((const gchar *)class_name, CLASS_CPU) == 0) {
        
        ret = vos_str2int(i_paras->member_id, 10, (void *)&processor_id, NUM_TPYE_INT32);
        return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "get processor id from %s failed, ret is %d", i_paras->member_id, ret));

        iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_SYSTEM_REPROCESSORS, slot, processor_id);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        
    } else {
        ret = get_npu_rsc_link(i_paras->obj_handle, uri, sizeof(uri), slot);
        if (ret != RET_OK) {
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return HTTP_OK;
}


LOCAL gint32 get_processor_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar class_name[MAX_CLASS_NAME] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dfl_get_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s:get class name failed, ret:%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(g_strcmp0((const gchar *)class_name, CLASS_CPU) == 0 ? "CPU" : "NPU");
    return HTTP_OK;
}


LOCAL gint32 get_processor_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar class_name[MAX_CLASS_NAME] = {0};
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dfl_get_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get class name failed, ret:%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 如果是NPU,则需将用户请求URL中的memberId格式化成标准格式
    if (g_strcmp0((const gchar *)class_name, CLASS_NPU) == 0) {
        format_xpu_member_id(i_paras->member_id, sizeof(i_paras->member_id));
    }

    
    *o_result_jso = json_object_new_string(i_paras->member_id);
    return HTTP_OK;
}

LOCAL gint32 get_processor_pcie_npu_name(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    gchar processor_name[MAX_PROP_VAL_LEN] = {0};

    
    if (provider_paras_check(i_paras) != RET_OK || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = construct_npu_name(i_paras->obj_handle, processor_name, sizeof(processor_name));
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *o_result_jso = json_object_new_string(processor_name);

    return HTTP_OK;
}


LOCAL gint32 get_processor_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{   
    gchar class_name[MAX_CLASS_NAME] = {0};
    gint32 ret = dfl_get_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get class name failed, ret:%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((g_strcmp0((const gchar *)class_name, CLASS_NPU) == 0) && dal_is_pcie_npu() == TRUE) {
        return get_processor_pcie_npu_name(i_paras, o_message_jso, o_result_jso);
    } else {
        return rfprocessor_get_property_string(rfprocessor_get_self_provider(i_paras), i_paras, o_message_jso,
            o_result_jso);
    }
}


LOCAL gint32 get_processor_architecture(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return rfprocessor_get_property_string(rfprocessor_get_self_provider(i_paras), i_paras, o_message_jso,
        o_result_jso);
}


LOCAL gint32 get_processor_instructionset(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return rfprocessor_get_property_string(rfprocessor_get_self_provider(i_paras), i_paras, o_message_jso,
        o_result_jso);
}


LOCAL gint32 get_processor_manufacturer(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return rfprocessor_get_property_string(rfprocessor_get_self_provider(i_paras), i_paras, o_message_jso,
        o_result_jso);
}

LOCAL gint32 get_customized_kunpeng_cpu_model(gchar *in_model, gchar *out_model, guint32 out_size)
{
    gint32 ret;
    gchar *first_token = NULL;
    gchar *model_token = NULL;
    gchar* saveptr = NULL;
    gboolean str_matched = FALSE; 
    
    
    first_token = strtok_s(in_model, " ", &saveptr);
    if (first_token == NULL) {
        debug_log(DLOG_DEBUG, "unsupport kunpeng cpu model(%s)", in_model);
        goto END_OP;
    }
    model_token = strtok_s(NULL, " ", &saveptr);
    if (model_token == NULL) {
        debug_log(DLOG_DEBUG, "unsupport kunpeng cpu model str(%s)", saveptr);
        goto END_OP;
    }
    
    if (g_strcmp0((const char*)first_token, CPU_MODEL_KUNPENG) == 0) {
        (void)g_strdelimit(model_token, "-", '_');
        ret = sprintf_s(out_model, out_size, "%s%s", first_token, model_token);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "sprintf_s failed");
            goto END_OP;
        }
        str_matched = TRUE; 
    } else if (g_strcmp0((const char*)first_token, "HUAWEI") == 0) { 
        if (g_strcmp0((const char*)model_token, CPU_MODEL_KUNPENG) != 0) {
            goto END_OP;
        }
        if (saveptr == NULL) {
            debug_log(DLOG_DEBUG, "unsupport kunpeng cpu model str(%s)", in_model);
            goto END_OP;
        }
        (void)g_strdelimit(saveptr, " ", '_');
        ret = sprintf_s(out_model, out_size, "%s%s", model_token, saveptr);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "sprintf_s failed");
            goto END_OP;
        }
        str_matched = TRUE; 
    }
END_OP:
    if (!str_matched) { 
        ret = strcpy_s(out_model, out_size, in_model);
        if (ret != EOK) {
            debug_log(DLOG_ERROR, "strcpy_s (%s) failded, ret(%d)", in_model, ret);
            return RET_ERR;
        }
    }
    return RET_OK;
}

LOCAL gint32 get_customized_intel_cpu_model(gchar *in_model, gchar *out_model, guint32 out_size)
{
    gint32 ret;
    gchar *manu_token = NULL;
    gchar *brand_token = NULL;
    gchar *model_token = NULL;
    gchar *gen_token = NULL;
    gchar *saveptr = NULL;
  
    
    manu_token = strtok_s(in_model, " ", &saveptr);
    if (manu_token == NULL) {
        debug_log(DLOG_ERROR, "%s: unsupport kunpeng cpu model(%s)", __FUNCTION__, in_model);
        return RET_ERR;
    }

    brand_token = strtok_s(NULL, " ", &saveptr);
    if (brand_token == NULL) {
        debug_log(DLOG_ERROR, "%s: unsupport kunpeng cpu model(%s)", __FUNCTION__, in_model);
        return RET_ERR;
    }

    model_token = strtok_s(NULL, " ", &saveptr);
    if (model_token == NULL) {
        debug_log(DLOG_ERROR, "%s: unsupport kunpeng cpu model(%s)", __FUNCTION__, in_model);
        return RET_ERR;
    }

    gen_token = strtok_s(NULL, " ", &saveptr);
    if (gen_token == NULL) {
        debug_log(DLOG_ERROR, "%s: unsupport kunpeng cpu model(%s)", __FUNCTION__, in_model);
        return RET_ERR;
    }

    if (strstr(brand_token, "(R)") == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid brand(%s)", __FUNCTION__, brand_token);
        return RET_ERR;
    }
    g_strdelimit(brand_token, "(R)", '\0');
        
    ret = sprintf_s(out_model, out_size, "%s_%s_%s", brand_token, model_token, gen_token);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s failed", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}

gint32 format_cpu_model_by_cmcc(gchar *in_model, gchar *out_model, guint32 out_size)
{
    gint32 ret;

    if (strstr(in_model, CPU_MODEL_KUNPENG) != NULL) {
        ret = get_customized_kunpeng_cpu_model(in_model, out_model, out_size);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get customized kunpeng cpu model failed, model(%s)", __FUNCTION__, in_model);
            return RET_ERR;
        }
        return RET_OK;
    } else if (strstr(in_model, CPU_MODEL_INTEL) != NULL) {
        ret = get_customized_intel_cpu_model(in_model, out_model, out_size);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get customized intel cpu model failed, model(%s)", __FUNCTION__, in_model);
            return RET_ERR;
        }
        return RET_OK;
    } 
    // 如果不是鲲鹏和Intel芯片，直接返回，不对Model做修改
    return RET_ERR;
}

LOCAL void get_customized_model_by_cmcc(struct json_object **model)
{
    const gchar *tmp = NULL;
    gchar in_model[RF_PROCESSOR_VALUE_MAXLEN] = {0};
    gchar out_model[RF_PROCESSOR_VALUE_MAXLEN] = {0};
    gint32 ret;
    
    tmp = dal_json_object_get_str(*model);
    if (tmp == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid model", __FUNCTION__);
        return;
    }

    ret = strcpy_s(in_model, sizeof(in_model), tmp);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strcpy_s failded, ret(%d)", __FUNCTION__, ret);
        return;
    }
    
    ret = format_cpu_model_by_cmcc(in_model, out_model, sizeof(out_model));
    if (ret != RET_OK) {
        return;
    }
    
    json_object_put(*model);
    *model = json_object_new_string(out_model);
    return;
}


LOCAL gint32 get_processor_model(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32  ret;
    gboolean is_customized;
    
    ret = rfprocessor_get_property_string(rfprocessor_get_self_provider(i_paras),
        i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_OK) {
        return ret;
    }
    
    is_customized = dal_is_customized_by_cmcc();
    if (is_customized) {
        
        get_customized_model_by_cmcc(o_result_jso);
    }
    return HTTP_OK;
}


LOCAL gint32 get_processor_step_family_model(const gchar *id_str, guint8 *step, guint8 *model, guint16 *family)
{
#define PROCESSORID_STR_LEN 23
#define PROCESSORID_BUFF_LEN 8
    if (strlen(id_str) != PROCESSORID_STR_LEN) {
        debug_log(DLOG_DEBUG, "%s: id_str err = %s", __FUNCTION__, id_str);
        return RET_ERR;
    }

    guint32 id_buff[PROCESSORID_BUFF_LEN];
    gint32 ret;

    // 解析id字符串，id字符串为固定格式及长度，包含8个16进制数
    ret = sscanf_s(id_str, "%x-%x-%x-%x-%x-%x-%x-%x", &id_buff[0], &id_buff[1], &id_buff[2], // 将数据存放在buff[0][1][2]
        &id_buff[3], &id_buff[4], &id_buff[5], &id_buff[6], &id_buff[7]);  // 将数据存放在buff[3][4][5][6][7]
    if (ret != PROCESSORID_BUFF_LEN) {
        debug_log(DLOG_ERROR, "%s: sscanf_s err ret = %d, id_str:%s", __FUNCTION__, ret, id_str);
        return RET_ERR;
    }



    *step = (guint8)(id_buff[0] & 0x0F);
    // 按CPU id字段定义，将buff[2]的低4位，buff[0]的高4位拼接成model
    *model = (guint8)(((id_buff[2] & 0x0F) << 4) | ((id_buff[0] >> 4) & 0x0F));
    // 按CPU id字段定义，将buff[3]的低4位左移8位，buff[2]的高4位，buff[1]的低4位拼接成family
    *family = (guint16)(((id_buff[3] & 0x0F) << 8) | (id_buff[2] & 0xF0) | (id_buff[1] & 0x0F));
    return RET_OK;
}


LOCAL gint32 get_CPU_processorid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    json_object *o_jso = NULL;
    json_object* jso_step = NULL;
    json_object* jso_family = NULL;
    json_object* jso_model = NULL;
    const gchar* id_str = NULL;
    guint16 family = 0;
    guint8 model = 0;
    guint8 step = 0;
    gchar arch[RF_PROCESSOR_VALUE_MAXLEN] = {0};

    ret = rfprocessor_get_property_string(rfprocessor_get_self_provider(i_paras), i_paras, o_message_jso, &o_jso);
    if (ret != HTTP_OK) {
        debug_log(DLOG_MASS, "%s: get cpu processor id error, ret:%d", __FUNCTION__, ret);
        goto EXIT;
    }

    ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_CPU_ARCHITECTURE, arch, sizeof(arch));
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get property_data fail, ret:%d", __FUNCTION__, ret);
        json_object_put(o_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (g_strcmp0((const gchar *)arch, "ARM") == 0) {
        goto EXIT;
    }

    id_str = dal_json_object_get_str(o_jso);
    if (id_str == NULL) {
        debug_log(DLOG_ERROR, "%s: id_str is null", __FUNCTION__);
        goto EXIT;
    }

    ret = get_processor_step_family_model(id_str, &step, &model, &family);
    if (ret != RET_OK) {
        goto EXIT;
    }

    jso_step = json_object_new_int(step);
    jso_family = json_object_new_int(family);
    jso_model = json_object_new_int(model);

EXIT:
    json_object_object_add(*o_result_jso, RFPROP_SYSTEM_PROCESEEOR_IDENTIFICATIONREGISTERS, o_jso);
    json_object_object_add(*o_result_jso, RFPROP_SYSTEM_PROCESEEOR_EFFECTIVEFAMILY, jso_family);
    json_object_object_add(*o_result_jso, RFPROP_SYSTEM_PROCESEEOR_EFFECTIVEMODEL, jso_model);
    json_object_object_add(*o_result_jso, RFPROP_SYSTEM_PROCESEEOR_STEP, jso_step);
    return HTTP_OK;
}


LOCAL gint32 get_processor_processorid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar class_name[MAX_CLASS_NAME] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dfl_get_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s:get class name failed, ret:%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (g_strcmp0((const gchar *)class_name, CLASS_CPU) == 0) {
        return get_CPU_processorid(i_paras, o_message_jso, o_result_jso);
    } else {
        // NPU资源的ProcessorId以VendorId来表达
        
        return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
        rf_add_property_jso_uint16_hex(i_paras->obj_handle, PROPERTY_NPU_VENDER_ID, RFPROP_SYSTEM_PROCESSOR_VENDOR_ID,
            *o_result_jso);
    }

    return HTTP_OK;
}


LOCAL gint32 get_processor_maxspeedmhz(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 value;
    gchar value_string[RF_PROCESSOR_VALUE_MAXLEN] = {0};
    gchar class_name[MAX_CLASS_NAME] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dfl_get_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    // 当前NPU无法获取最大频率,直接返回null
    if (ret != VOS_OK || g_strcmp0((const gchar *)class_name, CLASS_NPU) == 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    ret = dal_get_property_value_string(i_paras->obj_handle, g_system_processor_provider[i_paras->index].pme_prop_name,
        value_string, RF_PROCESSOR_VALUE_MAXLEN);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get property_data fail, ret:%d", __FUNCTION__, ret));

    
    if (rfprocessor_string_check(value_string, RF_PROCESSOR_VALUE_MAXLEN) != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    value = rfprocessor_maxspeed_to_intmhz(value_string, strlen(value_string));
    if (0 == value) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_int(value);

    return HTTP_OK;
}


LOCAL gint32 get_processor_totalcores(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar class_name[MAX_CLASS_NAME] = {0};

    ret = dfl_get_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    // NPU不支持查询核心数
    if (ret != VOS_OK || g_strcmp0((const gchar *)class_name, CLASS_NPU) == 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return rfprocessor_get_property_uint16(rfprocessor_get_self_provider(i_paras), i_paras, o_message_jso,
        o_result_jso);
}


LOCAL gint32 get_processor_totalthreads(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar class_name[MAX_CLASS_NAME] = {0};

    ret = dfl_get_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    // NPU不支持查询线程数
    if (ret != VOS_OK || g_strcmp0((const gchar *)class_name, CLASS_NPU) == 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return rfprocessor_get_property_uint16(rfprocessor_get_self_provider(i_paras), i_paras, o_message_jso,
        o_result_jso);
}


LOCAL gint32 get_processor_socket(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar cpu_id = 0;
    gchar class_name[MAX_CLASS_NAME] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dfl_get_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    // NPU不支持查询线程数
    if (ret != VOS_OK || g_strcmp0((const gchar *)class_name, CLASS_NPU) == 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = vos_str2int(i_paras->member_id, 10, &cpu_id, NUM_TPYE_UCHAR);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_FOUND,
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri));

    
    *o_result_jso = json_object_new_int(cpu_id - 1);
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  fail.\n", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 get_processor_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar health = 0;
    guchar state = 0;
    guchar enablementstatus = 0;
    gchar *state_p = NULL;
    guchar power_state = 0;
    gchar class_name[MAX_CLASS_NAME] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dfl_get_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:get class name failed, ret:%d", __FUNCTION__, ret));

    
    ret = rf_get_system_powerstate(&power_state);
    do_info_if_true(ret != VOS_OK, debug_log(DLOG_ERROR, "%s: get system power state fail", __FUNCTION__));
    // power_state取值1表示上电状态
    if (power_state == 1) {
        if (g_strcmp0((const gchar *)class_name, CLASS_CPU) == 0) {
            ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_CPU_HEALTH, &health);
        } else {
            ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_NPU_HEALTH, &health);
        }
        do_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s: error.", __FUNCTION__); health = INVALID_DATA_BYTE);
    } else {
        health = INVALID_DATA_BYTE;
    }

    if (g_strcmp0((const gchar *)class_name, CLASS_CPU) == 0) {
        
        ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_CPU_FAILURE_ISO, &state);
        do_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s: error.", __FUNCTION__); state = INVALID_DATA_BYTE);

        ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_CPU_DISABLE_CPU_HW, &enablementstatus);
        do_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s: error.", __FUNCTION__); state = INVALID_DATA_BYTE);

        state = (state | enablementstatus);
    } else {
        // NPU目前固定返回Enabled
        state = 0;
    }

    
    state_p = rfprocessor_cpu_state_to_string(state);

    
    get_resource_status_property(&health, NULL, state_p, o_result_jso, FALSE);

    return HTTP_OK;
}


LOCAL gint32 rfprocessor_get_partnumber(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso)
{
    gint32 ret;
    guchar cpu_id = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar value_string[RF_PROCESSOR_VALUE_MAXLEN] = {0};

    
    if (o_result_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = vos_str2int(i_paras->member_id, 10, &cpu_id, NUM_TPYE_UCHAR);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s, %d:vos_str2int fail.\n", __FUNCTION__, __LINE__));

    ret = dal_get_specific_object_byte(CLASS_CPU, PROPERTY_CPU_PHYSIC_ID, cpu_id, &obj_handle);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:dal_get_specific_object_byte fail.\n", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_string(obj_handle, PROPERTY_CPU_PART_NUM, value_string, RF_PROCESSOR_VALUE_MAXLEN);
    
    return_val_do_info_if_fail(ret == VOS_OK && check_string_val_effective(value_string) == VOS_OK,
        HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get dal_get_property_value_string fail.\n", __FUNCTION__, __LINE__));
    

    *o_result_jso = json_object_new_string(value_string);
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:json_object_new_string fail.\n", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 rfprocessor_get_frequency(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso)
{
    gint32 ret;
    gint32 value;
    OBJ_HANDLE obj_handle = 0;
    guchar cpu_id = 0;
    gchar value_string[RF_PROCESSOR_VALUE_MAXLEN] = {0};

    
    if (NULL == o_result_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = vos_str2int(i_paras->member_id, 10, &cpu_id, NUM_TPYE_UCHAR);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:vos_str2int fail.\n", __FUNCTION__, __LINE__));

    ret = dal_get_specific_object_byte(CLASS_CPU, PROPERTY_CPU_PHYSIC_ID, cpu_id, &obj_handle);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

    
    ret =
        dal_get_property_value_string(obj_handle, PROPERTY_CPU_CURRENT_SPEED, value_string, RF_PROCESSOR_VALUE_MAXLEN);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));

    
    if (rfprocessor_string_check(value_string, RF_PROCESSOR_VALUE_MAXLEN) != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    value = rfprocessor_maxspeed_to_intmhz(value_string, strlen(value_string));
    if (0 == value) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_int(value);

    return HTTP_OK;
}


LOCAL gint32 get_cpu_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    json_object *huawei = NULL;
    json_object *temperature = NULL;
    json_object *enablementstatus = NULL;
    json_object *part_number = NULL;
    json_object *frequency = NULL;

    *o_result_jso = json_object_new_object();
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    huawei = json_object_new_object();
    return_val_if_expr(huawei == NULL, HTTP_INTERNAL_SERVER_ERROR); // 不用释放*o_result_jso
    json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei);

    // 获取三级缓存
    get_processor_cache_size(i_paras->obj_handle, PROPERTY_CPU_L1CACHE, RFPROP_SYSTEM_PROCESEEOR_ONECACHE, huawei);
    get_processor_cache_size(i_paras->obj_handle, PROPERTY_CPU_L2CACHE, RFPROP_SYSTEM_PROCESEEOR_TWOCACHE, huawei);
    get_processor_cache_size(i_paras->obj_handle, PROPERTY_CPU_L3CACHE, RFPROP_SYSTEM_PROCESEEOR_THREECACHE, huawei);

    // 获取丝印
    rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_CPU_DEVICENAME, RFPROP_DEVICELOCATOR, huawei);
    // 获取容器
    rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_CPU_LOCATION, RFPROP_POSITION, huawei);

    // 获取温度
    ret = rfprocessor_get_temperature(i_paras, &temperature);
    json_object_object_add(huawei, RFPROP_SYSTEM_PROCESEEOR_TEMPERATURE, ret == HTTP_OK ? temperature : NULL);

    // 获取启用状态
    ret = rfprocessor_get_enablementstatus(i_paras, &enablementstatus);
    json_object_object_add(huawei, RFPROP_SYSTEM_PROCESEEOR_ENABLEDSETTING, ret == HTTP_OK ? enablementstatus : NULL);

    
    ret = rfprocessor_get_partnumber(i_paras, &part_number);
    json_object_object_add(huawei, RFPROP_PART_NUMBER, ret == HTTP_OK ? part_number : NULL);
    
    
    // 获取cpu主频
    ret = rfprocessor_get_frequency(i_paras, &frequency);
    json_object_object_add(huawei, RFPROP_PROCESSOR_FREQUENCY, ret == HTTP_OK ? frequency : NULL);
    

    
    rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_CPU_MEMORY_TEC, RFPROP_SYSTEM_PROCESEEOR_OTHERPARAMETERS,
        huawei);
    

    
    rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_CPU_SN, RFPROP_SERIAL_NUMBER, huawei);
    
    return HTTP_OK;
}


void get_npu_temp_or_power(OBJ_HANDLE obj_handle, const gchar *prop_name, const gchar *rf_prop,
    json_object *o_result_jso)
{
    gint32 ret;
    guint16 val = SENSOR_INVALID_READING;

    ret = dal_get_property_value_uint16(obj_handle, prop_name, &val);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get %s of NPU failed, ret:%d", __FUNCTION__, prop_name, ret);
    }
    // 如果为无效值, 则返回null
    if (ret != VOS_OK || val == SENSOR_INVALID_READING) {
        json_object_object_add(o_result_jso, rf_prop, NULL);
        return;
    }

    json_object_object_add(o_result_jso, rf_prop, json_object_new_int(val));
}
LOCAL gint32 get_ecc_info_handle_and_count(OBJ_HANDLE npu_handle, NPU_ECC_INFO *ecc_info, OBJ_HANDLE *ecc_info_handle)
{
    gint32 ret;
    const gchar *rf_object_name = (ecc_info->type == ECC_TYPE_SINGLE_BIT) ?
        PROPERTY_NPU_HISTORY_SINGLE_BIT_ECC : PROPERTY_NPU_HISTORY_MULTI_BIT_ECC;

    ret = dfl_get_referenced_object(npu_handle, rf_object_name, ecc_info_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_INFO, "%s: dfl_get_referenced_object failed(%s.%s),ret = %d",
            __FUNCTION__, dfl_get_object_name(npu_handle), rf_object_name, ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_uint32(*ecc_info_handle, PROPERTY_HISTORY_ECC_COUNT, &(ecc_info->count));
    if (ret != RET_OK || ecc_info->count > ECC_INFO_DATA_MAX_CNT) {
        debug_log(DLOG_INFO, "%s: dal_get_property_value_uint32 failed, ret = %d, count = %d",
            __FUNCTION__, ret, ecc_info->count);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 get_ecc_info_from_prop(OBJ_HANDLE obj_handle, NPU_ECC_INFO *ecc_info)
{
    gint32 ret;
    gconstpointer data_out = NULL;
    gsize element_count = 0;
    GVariant *property_gv_arr = NULL;
    OBJ_HANDLE ecc_info_handle = 0;

    SET_ECC_INFO_PROP ecc_info_prop[] = {
        { G_VARIANT_TYPE_UINT32, ecc_info->phy_addr_l, sizeof(guint32), PROPERTY_HISTORY_ECC_PHY_ADDR_LOW },
        { G_VARIANT_TYPE_UINT32, ecc_info->phy_addr_h, sizeof(guint32), PROPERTY_HISTORY_ECC_PHY_ADDR_HIGH },
        { G_VARIANT_TYPE_UINT32, ecc_info->stack_pc_id, sizeof(guint32), PROPERTY_HISTORY_ECC_STACK_PC_ID },
        { G_VARIANT_TYPE_UINT32, ecc_info->row_column, sizeof(guint32), PROPERTY_HISTORY_ECC_ROW_COLUMN },
        { G_VARIANT_TYPE_UINT32, ecc_info->bank, sizeof(guint32), PROPERTY_HISTORY_ECC_BANK },
        { G_VARIANT_TYPE_UINT32, ecc_info->err_cnt, sizeof(guint32), PROPERTY_HISTORY_ECC_ERR_CNT},
        { G_VARIANT_TYPE_UINT32, ecc_info->time_stamp, sizeof(guint32), PROPERTY_HISTORY_ECC_TIME },
    };

    ret = get_ecc_info_handle_and_count(obj_handle, ecc_info, &ecc_info_handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    if (ecc_info->count == 0) {
        
        return RET_OK;
    }

    for (gsize idx = 0; idx < ARRAY_SIZE(ecc_info_prop); idx++) {
        ret = dfl_get_property_value(ecc_info_handle, ecc_info_prop[idx].prop_name, &property_gv_arr);
        if (ret != DFL_OK) {
            debug_log(DLOG_INFO, "%s: get %s.%s failed, ret = %d", __FUNCTION__, dfl_get_object_name(ecc_info_handle),
                ecc_info_prop[idx].prop_name, ret);
            return RET_ERR;
        }

        data_out = g_variant_get_fixed_array(property_gv_arr, &element_count, ecc_info_prop[idx].element_size);
        if (data_out == NULL) {
            g_variant_unref(property_gv_arr);
            debug_log(DLOG_INFO, "%s: g_variant_get_fixed_array failed", __FUNCTION__);
            return RET_ERR;
        }

        if (element_count != ecc_info->count) {
            g_variant_unref(property_gv_arr);
            return RET_ERR;
        }

        ret = memcpy_s(ecc_info_prop[idx].elements, ecc_info_prop[idx].element_size * ECC_INFO_DATA_MAX_CNT,
            data_out, ecc_info_prop[idx].element_size * ecc_info->count);
        g_variant_unref(property_gv_arr);
        if (ret != EOK) {
            debug_log(DLOG_INFO, "%s: memcpy_s failed, ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    }
    return RET_OK;
}


LOCAL void add_ecc_info_phy_addr(json_object* result_jso, guint32 addr_l, guint32 addr_h)
{
    gint32 ret;
    gchar str_buf[32] = {0};

    ret = sprintf_s(str_buf, sizeof(str_buf), "0x%08x%08x", addr_h, addr_l);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "sprintf_s failed, ret = %d", ret);
        json_object_object_add(result_jso, RFPROP_NPU_PHY_ADDR, NULL);
        return;
    }

    json_object_object_add(result_jso, RFPROP_NPU_PHY_ADDR, json_object_new_string(str_buf));
}


LOCAL void add_ecc_info_from_guint32(json_object* result_jso,
    const gchar* json_property, guint32 prop_val)
{
    gint32 ret;
    gchar str_buf[STRING_LEN_MAX] = {0};

    ret = sprintf_s(str_buf, sizeof(str_buf), "0x%08x", prop_val);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "sprintf_s failed, ret = %d", ret);
        json_object_object_add(result_jso, json_property, NULL);
        return;
    }

    json_object_object_add(result_jso, json_property, json_object_new_string(str_buf));
}


LOCAL void add_ecc_info_error_time(json_object* result_jso, guint32 time)
{
#define BASE_YEAR 1900
    gint32 ret;
    gchar time_str[MAX_TIME_STRING_LEN] = {0};
    struct tm local_time = { 0 };
    time_t tmp_value = time;

    
    ret = dal_localtime_r(&tmp_value, &local_time);
    if (ret != RET_OK) {
        json_object_object_add(result_jso, RFPROP_NPU_ECC_TIME, NULL);
        return;
    }

    ret = sprintf_s(time_str, sizeof(time_str), "%04d-%02d-%02d %02d:%02d:%02d", (BASE_YEAR + local_time.tm_year),
        (1 + local_time.tm_mon), local_time.tm_mday, local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "sprintf_s failed, ret = %d", ret);
        json_object_object_add(result_jso, RFPROP_NPU_ECC_TIME, NULL);
        return;
    }

    json_object_object_add(result_jso, RFPROP_NPU_ECC_TIME, json_object_new_string(time_str));
}

LOCAL void collect_one_npu_ecc_info(OBJ_HANDLE obj_handle,
    json_object *one_npu_ecc, guint8 ecc_type)
{
    gint32 ret;
    NPU_ECC_INFO ecc_info = {0};
    ecc_info.type = ecc_type;
    json_object *bit_ecc_json = NULL;
    json_object *info_arr = NULL;
    json_object *addr_info = NULL;

    const gchar *bit_type = (ecc_type == ECC_TYPE_SINGLE_BIT) ?
        RFPROP_NPU_SINGLE_BIT_ECC : RFPROP_NPU_MULTI_BIT_ECC;

    ret = get_ecc_info_from_prop(obj_handle, &ecc_info);
    if (ret != RET_OK) {
        json_object_object_add(one_npu_ecc, bit_type, NULL);
        return;
    }

    bit_ecc_json = json_object_new_object();
    if (bit_ecc_json == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        json_object_object_add(one_npu_ecc, bit_type, NULL);
        return;
    }

    json_object_object_add(one_npu_ecc, bit_type, bit_ecc_json);
    json_object_object_add(bit_ecc_json, RFPROP_NPU_ECC_COUNT,  json_object_new_int(ecc_info.count));
    if (ecc_info.count == 0) {
        json_object_object_add(bit_ecc_json, RFPROP_NPU_INFO, NULL);
        return;
    }
    
    info_arr = json_object_new_array();
    if (info_arr == NULL) {
        json_object_object_add(bit_ecc_json, RFPROP_NPU_INFO, NULL);
        return;
    }

    json_object_object_add(bit_ecc_json, RFPROP_NPU_INFO, info_arr);
    for (guint32 idx = 0; idx < ecc_info.count; idx++) {
        addr_info = json_object_new_object();
        if (addr_info == NULL) {
            debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
            continue;
        }

        json_object_array_add(info_arr, addr_info);
        add_ecc_info_phy_addr(addr_info, ecc_info.phy_addr_l[idx], ecc_info.phy_addr_h[idx]);
        add_ecc_info_from_guint32(addr_info, RFPROP_NPU_ECC_STACK_PC_ID, ecc_info.stack_pc_id[idx]);
        add_ecc_info_from_guint32(addr_info, RFPROP_NPU_ECC_ROW_COLUMN, ecc_info.row_column[idx]);
        add_ecc_info_from_guint32(addr_info, RFPROP_NPU_ECC_BANK, ecc_info.bank[idx]);
        json_object_object_add(addr_info, RFPROP_NPU_ECC_ADDR_ERROR_COUNT,
            json_object_new_int(ecc_info.err_cnt[idx]));
        add_ecc_info_error_time(addr_info, ecc_info.time_stamp[idx]);
        addr_info = NULL;
    }
}
        

void get_npu_ecc_info(OBJ_HANDLE obj_handle, json_object* o_result_jso)
{
    json_object *ecc_info = NULL;

    ecc_info = json_object_new_object();
    if (ecc_info == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        json_object_object_add(o_result_jso, RFPROP_NPU_ECC_INFO, NULL);
        return;
    }

    json_object_object_add(o_result_jso, RFPROP_NPU_ECC_INFO, ecc_info);
    
    collect_one_npu_ecc_info(obj_handle, ecc_info, ECC_TYPE_SINGLE_BIT);
    collect_one_npu_ecc_info(obj_handle, ecc_info, ECC_TYPE_MULTI_BIT);
}


void get_npu_memory_info(OBJ_HANDLE obj_handle, json_object* o_result_jso)
{
    gint32 ret;
    guint32 val;
    json_object* memory_jso = NULL;

    const gchar* bmc_prop[] = {
        PROPERTY_NPU_TOTAL_MEMORY,
        PROPERTY_NPU_MEMORY_USAGE,
        PROPERTY_NPU_MEMORY_FREE
    };
    const gchar* rf_prop[] = {
        RFPROP_SYSTEM_PROCESSOR_NPU_TOTAL_MEMORY,
        RFPROP_SYSTEM_PROCESSOR_NPU_MEMORY_USAGE,
        RFPROP_SYSTEM_PROCESSOR_NPU_MEMORY_FREE
    };

    memory_jso = json_object_new_object();
    if (memory_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: new object fail", __FUNCTION__);
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_PROCESSOR_NPU_MEMORY_ON_CHIP, NULL);
        return;
    }

    json_object_object_add(o_result_jso, RFPROP_SYSTEM_PROCESSOR_NPU_MEMORY_ON_CHIP, memory_jso);

    for (guint32 i = 0; i < G_N_ELEMENTS(bmc_prop); i++) {
        val = 0;
        ret = dal_get_property_value_uint32(obj_handle, bmc_prop[i], &val);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get %s of NPU failed, ret:%d", __FUNCTION__, bmc_prop[i], ret);
        }
        if (ret != RET_OK || val == RF_INFO_INVALID_DWORD) {
            json_object_object_add(memory_jso, rf_prop[i], NULL);
            continue;
        }
        json_object_object_add(memory_jso, rf_prop[i], json_object_new_int(val));
    }
}


LOCAL void get_npu_ipv4_info(OBJ_HANDLE obj_handle, json_object* o_result_jso)
{
    gint32 ret;
    json_object* ipv4_jso = NULL;

    gchar ipv4_info[MAX_IPV4_LEN] = {0};
    const gchar* bmc_prop[] = {
        PROPERTY_NPU_IPV4_ADDR,
        PROPERTY_NPU_IPV4_SUBNET_MASK,
        PROPERTY_NPU_IPV4_GATEWAY
    };
    const gchar* rf_prop[] = {
        RF_PROPERTY_ADDR,
        RF_PROPERTY_SUB_MASK,
        RF_PROPERTY_GATEWAY
    };

    ipv4_jso = json_object_new_object();
    if (ipv4_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: new object fail", __FUNCTION__);
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_PROCESSOR_NPU_IPV4_ADDRESS, NULL);
        return;
    }
    json_object_object_add(o_result_jso, RFPROP_SYSTEM_PROCESSOR_NPU_IPV4_ADDRESS, ipv4_jso);

    for (guint32 i = 0; i < G_N_ELEMENTS(bmc_prop); i++) {
        ret = dal_get_property_value_string(obj_handle, bmc_prop[i], ipv4_info, sizeof(ipv4_info));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get %s of NPU failed, ret:%d", __FUNCTION__, bmc_prop[i], ret);
        }
        if (ret != RET_OK || g_strcmp0(PROPERTY_VALUE_DEFAULT_STRING, ipv4_info) == 0) {
            json_object_object_add(ipv4_jso, rf_prop[i], NULL);
            continue;
        }
        json_object_object_add(ipv4_jso, rf_prop[i], json_object_new_string(ipv4_info));
    }
}


LOCAL void get_npu_eth_statistics(OBJ_HANDLE obj_handle, json_object* o_result_jso)
{
    gint32 ret;
    guint32 val;

    const gchar* bmc_prop[] = {
        PROPERTY_NPU_PACKETS_RX, 
        PROPERTY_NPU_PACKETS_TX,
        PROPERTY_NPU_DROPPED_PACKETS
    };
    const gchar* rf_prop[] = {
        RFPROP_SYSTEM_PROCESSOR_NPU_RX_PACKETS, 
        RFPROP_SYSTEM_PROCESSOR_NPU_TX_PACKETS,
        RFPROP_SYSTEM_PROCESSOR_NPU_DROPPED_PACKETS
    };

    for (guint32 i = 0; i < G_N_ELEMENTS(bmc_prop); i++) {
        val = 0;
        ret = dal_get_property_value_uint32(obj_handle, bmc_prop[i], &val);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get %s of NPU failed, ret:%d", __FUNCTION__, bmc_prop[i], ret);
        }
        if (ret != RET_OK || val == RF_INFO_INVALID_DWORD) {
            json_object_object_add(o_result_jso, rf_prop[i], NULL);
            continue;
        }
        json_object_object_add(o_result_jso, rf_prop[i], json_object_new_uint64(val));
    }
}


void get_npu_eth_info(OBJ_HANDLE obj_handle, json_object* o_result_jso)
{
    gint32 ret;
    json_object* eth_jso = NULL;
    gchar mac_addr[MACADDRESS_LEN + 1] = {0};

    eth_jso = json_object_new_object();
    if (eth_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: new object fail", __FUNCTION__);
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_PROCESSOR_NPU_ETH, NULL);
        return;
    }
    json_object_object_add(o_result_jso, RFPROP_SYSTEM_PROCESSOR_NPU_ETH, eth_jso);

    // 1. MAC Address
    ret = dal_get_property_value_string(obj_handle, PROPERTY_NPU_MAC_ADDR, mac_addr, sizeof(mac_addr));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get MAC address of NPU failed", __FUNCTION__);
    }
    if (ret != RET_OK ||
        g_strcmp0((const gchar*)mac_addr, RF_INIT_VAL_MACADDR) == 0 ||
        g_strcmp0((const gchar*)mac_addr, PROPERTY_VALUE_DEFAULT_STRING) == 0) {
        json_object_object_add(eth_jso, RF_PROPERTY_MAC_ADDR, NULL);
    } else {
        json_object_object_add(eth_jso, RF_PROPERTY_MAC_ADDR, json_object_new_string(mac_addr));
    }

    // 2. IPv4Addresses
    get_npu_ipv4_info(obj_handle, eth_jso);

    // 3. 收发包数和丢包数
    get_npu_eth_statistics(obj_handle, eth_jso);
}

void get_npu_utl_oem_huawei(OBJ_HANDLE obj_handle, json_object* huawei_jso)
{
    const RF_NPU_UTL rf_npu_utl_arry[] = {
        {PROPERTY_NPU_CTRL_CPU_USAGE_PERCENT, RFPROP_SYSTEM_PROCESSOR_NPU_CTRL_CPU_USAGE_PERCENT},
        {PROPERTY_NPU_AICORE_USAGE_PERCENT, RFPROP_SYSTEM_PROCESSOR_NPU_AICORE_USAGE_PERCENT},
        {PROPERTY_NPU_MEM_UTL, RFPROP_NPU_MEM_UTL},
        {PROPERTY_NPU_HPM_UTL, RFPROP_NPU_HPM_UTL},
        {PROPERTY_NPU_MEM_BW, RFPROP_NPU_MEM_BW},
        {PROPERTY_NPU_HBM_BW, RFPROP_NPU_HBM_BW},
        {PROPERTY_NPU_AICPU_UTL, RFPROP_NPU_AICPU_UTL}
    };
    
    get_xpu_devicelocator_position(obj_handle, huawei_jso);

    
    for (size_t i = 0; i < G_N_ELEMENTS(rf_npu_utl_arry); i++) {
        get_oem_prop_threshold_uint16(obj_handle, rf_npu_utl_arry[i].npu_prop_name,
            rf_npu_utl_arry[i].npu_utl_rf, huawei_jso);
    }
}


LOCAL gint32 get_npu_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    json_object *huawei_jso = NULL;
    guchar power_on = ENABLED;
    json_object *obj_jso = NULL;

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:new json object failed", __FUNCTION__));

    huawei_jso = json_object_new_object();
    return_val_do_info_if_expr(huawei_jso == NULL, HTTP_INTERNAL_SERVER_ERROR, json_object_put(*o_result_jso);
        *o_result_jso = NULL; debug_log(DLOG_ERROR, "%s:new json object failed", __FUNCTION__));

    json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei_jso);

    
    rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_NPU_FIRMWARE_VERSION,
        RFPROP_SYSTEM_PROCESSOR_NPU_FWVERSION, huawei_jso);

    
    get_npu_temp_or_power(i_paras->obj_handle, PROPERTY_NPU_POWER, RFPROP_SYSTEM_PROCESSOR_NPU_POWER, huawei_jso);

    
    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_NPU_POWER_ON, &power_on);
    obj_jso = json_object_new_boolean((power_on == ENABLED) ? TRUE : FALSE);
    json_object_object_add(huawei_jso, RFPROP_SYSTEM_PROCESSOR_NPU_POWERON, obj_jso);

    
    rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_NPU_SN, RFPROP_SYSTEM_PROCESSOR_NPU_SERIALNUMBER,
        huawei_jso);

    
    get_npu_temp_or_power(i_paras->obj_handle, PROPERTY_NPU_AICORE_TEMP, RFPROP_SYSTEM_PROCESSOR_NPU_AICORE_TEMP,
        huawei_jso);

    
    get_npu_temp_or_power(i_paras->obj_handle, PROPERTY_NPU_HBM_TEMP, RFPROP_SYSTEM_PROCESSOR_NPU_HBM_TEMP, huawei_jso);

    
    get_npu_temp_or_power(i_paras->obj_handle, PROPERTY_NPU_NIMBUS_TEMP, RFPROP_SYSTEM_PROCESSOR_NPU_NIMBUS_TEMP,
        huawei_jso);

    
    rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_NPU_COMPUTE_CAPABILITY,
        RFPROP_SYSTEM_PROCESSOR_NPU_COMPUTE_CAPABILITY, huawei_jso);

    
    get_npu_temp_or_power(i_paras->obj_handle, PROPERTY_NPU_AICORE_MAX_SPEED, 
        RFPROP_SYSTEM_PROCESSOR_NPU_AICORE_MAX_SPEED, huawei_jso);

    
    get_npu_utl_oem_huawei(i_paras->obj_handle, huawei_jso);

    
    get_npu_ecc_info(i_paras->obj_handle, huawei_jso);

    
    get_npu_memory_info(i_paras->obj_handle, huawei_jso);

    
    get_npu_eth_info(i_paras->obj_handle, huawei_jso);

    return HTTP_OK;
}


LOCAL gint32 get_processor_firmware_version(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 get_processor_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gchar class_name[MAX_CLASS_NAME] = {0};
    gint32 ret;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dfl_get_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s:get class name failed, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (g_strcmp0((const gchar *)class_name, CLASS_CPU) == 0) {
        return get_cpu_oem(i_paras, o_message_jso, o_result_jso);
    } else {
        return get_npu_oem(i_paras, o_message_jso, o_result_jso);
    }
}


LOCAL gint32 set_processor_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    json_object *huawei = NULL;
    json_object *enablementstatus = NULL;
    guchar enablementstatus_byte = 0;
    const gchar *respond_value = "Oem/Huawei/EnabledSetting";

    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gchar class_name[MAX_CLASS_NAME] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dfl_get_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s:get class name failed, ret:%d", __FUNCTION__, ret));
    // NPU不支持设置OEM属性
    if (g_strcmp0((const gchar *)class_name, CLASS_NPU) == 0) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, NULL, o_message_jso, RFPROP_COMMON_OEM);
        return HTTP_BAD_REQUEST;
    }

    if (json_object_object_get_ex(i_paras->val_jso, RFPROP_COMMON_HUAWEI, &huawei) == TRUE &&
        json_object_object_get_ex(huawei, RFPROP_SYSTEM_PROCESEEOR_ENABLEDSETTING, &enablementstatus) == TRUE) {
        enablementstatus_byte = !json_object_get_boolean(enablementstatus);

        input_list = g_slist_append(input_list, g_variant_new_byte(enablementstatus_byte));

        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
            i_paras->obj_handle, CLASS_CPU, METHOD_CPU_SET_DISABLE_CPU, AUTH_ENABLE, i_paras->user_role_privilege,
            input_list, &output_list);

        uip_free_gvariant_list(input_list);
        uip_free_gvariant_list(output_list);

        switch (ret) {
            case VOS_OK:
            case RFERR_SUCCESS:
                return HTTP_OK;

            case RFERR_INSUFFICIENT_PRIVILEGE: 
                (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, respond_value, o_message_jso,
                    respond_value);
                return HTTP_FORBIDDEN;

            case VOS_ERR:
            case RF_ERROR:
                (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, respond_value, o_message_jso,
                    respond_value);
                return HTTP_NOT_IMPLEMENTED;

            default: 
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
                return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    return HTTP_BAD_REQUEST;
}


gint32 system_processor_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    guchar board_type = 0;
    gchar class_name[MAX_CLASS_NAME] = {0};

    ret = redfish_get_x86_enable_type(&board_type);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    return_val_if_fail(DISABLE != board_type, HTTP_NOT_FOUND);

    if (!redfish_check_system_uri_valid(i_paras->uri)) {
        debug_log(DLOG_ERROR, "%s: Invalid system resource URI", __FUNCTION__);
        return HTTP_NOT_FOUND;
    }

    
    ret = redfish_system_processor_uri_check(i_paras->member_id, i_paras->uri, &i_paras->obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Invalid processor resource URI", __FUNCTION__);
        return HTTP_NOT_FOUND;
    }

    
    ret = dfl_get_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_get_class_name failed, ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (strcmp(class_name, CLASS_GPU_CARD) == 0) {
        get_system_processor_gpu_provider(prop_provider, count);
    } else {
        *prop_provider = g_system_processor_provider;
        *count = sizeof(g_system_processor_provider) / sizeof(PROPERTY_PROVIDER_S);
    }
    return VOS_OK;
}


gint32 processor_etag_del_property(json_object *object)
{
    SPECIAL_PROP_S value[] = {
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_SYSTEM_PROCESEEOR_TEMPERATURE, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_SYSTEM_PROCESSOR_NPU_AICORE_TEMP, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_SYSTEM_PROCESSOR_NPU_HBM_TEMP, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_SYSTEM_PROCESSOR_NPU_NIMBUS_TEMP, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_SYSTEM_PROCESSOR_NPU_POWER, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_SYSTEM_PROCESSOR_NPU_CTRL_CPU_USAGE_PERCENT, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_SYSTEM_PROCESSOR_NPU_AICORE_USAGE_PERCENT, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_SYSTEM_PROCESSOR_NPU_MEMORY_ON_CHIP, NULL, NULL}},
        {4, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_SYSTEM_PROCESSOR_NPU_ETH,
            RFPROP_SYSTEM_PROCESSOR_NPU_RX_PACKETS, NULL}},
        {4, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_SYSTEM_PROCESSOR_NPU_ETH,
            RFPROP_SYSTEM_PROCESSOR_NPU_TX_PACKETS, NULL}},
        {4, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_SYSTEM_PROCESSOR_NPU_ETH,
            RFPROP_SYSTEM_PROCESSOR_NPU_DROPPED_PACKETS, NULL}},
        {4, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_GPU_CARD_POWER, RFPROP_GPU_CARD_POWER_DRAW_WATTS, NULL}},
        {4, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_GPU_CARD_THERMALS, RFPROP_GPU_CARD_PRIMARY_GPU_TEMP_CELSIUS, NULL}},
        {4, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_GPU_CARD_THERMALS, RFPROP_GPU_CARD_MEMORY_TEMP_CELISIUS, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_GPU_CARD_UTILIZATION, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_NPU_MEM_UTL, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_NPU_HPM_UTL, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_NPU_MEM_BW, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_NPU_HBM_BW, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_NPU_AICPU_UTL, NULL, NULL}},
        {0, {NULL, NULL, NULL, NULL, NULL}}
    };

    return rsc_del_none_etag_affected_property(object, value, G_N_ELEMENTS(value));
}
