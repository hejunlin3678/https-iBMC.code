

#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "component/component.h"
#include "component/base.h"
#include "media/e2p_large.h"

typedef struct tag_get_device_max_num {
    guint8 device_type;
    const gchar *property_str;
    GET_DEVICE_MAX_NUM_FUN fun;
} GET_DEVICE_MAX_NUM_S;

typedef gint32 (*__get_device_manu_info)(guint8 device_type, guint8 device_num, gchar *puc_manu_info, gint32 info_len);

typedef struct tag_get_device_manu {
    guint8 device_type;
    __get_device_manu_info exec_func;
} GET_DEVICE_NANU_S;

#define TS2280_PRO_NAME_DEFAULT "TaiShan 200 (Model 2280)"
#define TS2280_PRO_NAME_REPLACE "TaiShan 200K (Model 2280K)"
#define TS5280_PRO_NAME_DEFAULT "TaiShan 200 (Model 5280)"
#define TS5280_PRO_NAME_REPLACE "TaiShan 200K (Model 5280K)"
#define TS2280K_TO_TS2280 0x00
#define TS2280_TO_TS2280K 0x0a
#define TS5280K_TO_TS5280 0x01
#define TS5280_TO_TS5280K 0x0b

LOCAL gint32 __get_cpu_manu_info(guint8 device_type, guint8 device_num, gchar *puc_manu_info, gint32 info_len);
LOCAL gint32 __get_mem_manu_info(guint8 device_type, guint8 device_num, gchar *puc_manu_info, gint32 info_len);
LOCAL gint32 __get_hard_disk_manu(guint8 device_type, guint8 device_num, gchar *puc_manu_info, gint32 info_len);
LOCAL gint32 __get_mezz_manu_info(guint8 device_type, guint8 device_num, gchar *puc_manu_info, gint32 info_len);
LOCAL gint32 __get_component_binded_manu_info(guint8 device_type, guint8 device_num, gchar *puc_manu, gint32 puc_len);
LOCAL gint32 __get_component_type_list(guint8 device_type_code, GSList **component_list);
LOCAL void __remove_hdd_comp_dup_node(GSList **pcomponent_list);
LOCAL gint32 __get_device_max_num_patch_board(guint32 *max_num);
LOCAL gint32 __get_pcie_card_max_num(const gchar *property_str, guint32 *max_num);
LOCAL gint32 __get_sdi_card_max_num(const gchar *property_str, guint32 *max_num);
LOCAL gint32 __get_raid_card_max_num(const gchar *property_str, guint32 *max_num);
LOCAL gint32 __get_component_max_num(const gchar *property_str, guint32 *max_num);
LOCAL gint32 __get_hdd_backplane_max_num(const gchar *property_str, guint32 *max_num);
LOCAL gint32 get_component_lsw_port_num(const gchar *property_str, guint32 *maximum);
LOCAL gint32 get_expansion_module_max_slot_num(const gchar *property_str, guint32 *max_num);
LOCAL gint32 get_pcie_riser_max_num(const gchar *property_str, guint32 *max_num);
LOCAL gint32 get_new_device_type(guint8 device_type, guint8 *device_type_new);

LOCAL GET_DEVICE_MAX_NUM_S g_max_num_get_arr[] = {
    {DEVICE_TYPE_CPU, PROPERTY_COMPOENT_CPU_NUM, __get_component_max_num},
    {DEVICE_TYPE_MEMORY, PROPERTY_COMPOENT_MEMORY_NUM, __get_component_max_num},
    {DEVICE_TYPE_HARDDISK, PROPERTY_COMPOENT_DISK_NUM, __get_component_max_num},
    {DEVICE_TYPE_PS, PROPERTY_COMPOENT_PS_NUM, __get_component_max_num},
    {DEVICE_TYPE_BBU, PROPERTY_COMPONENT_BBU_MODULE_NUM, __get_component_max_num},
    {DEVICE_TYPE_FAN, PROPERTY_COMPOENT_FAN_NUM, __get_component_max_num},
    {DEVICE_TYPE_HDD_BACKPLANE, PROPERTY_COMPOENT_HDDBACK_CARD_NUM, __get_hdd_backplane_max_num},
    {DEVICE_TYPE_RAID_CARD, PROPERTY_COMPOENT_RAID_CARD_NUM, __get_raid_card_max_num},
    {DEVICE_TYPE_PCIE_CARD, PROPERTY_COMPOENT_PCIE_CARD_NUM, __get_pcie_card_max_num},
    {DEVICE_TYPE_MEZZ, PROPERTY_COMPOENT_MEZZ_CARD_NUM, __get_component_max_num},
    {DEVICE_TYPE_PCIE_RISER, PROPERTY_COMPOENT_RISER_CARD_NUM, get_pcie_riser_max_num},
    {DEVICE_TYPE_FAN_BOARD, PROPERTY_COMPOENT_FAN_NUM, __get_component_max_num},
    {DEVICE_TYPE_PATCH_BOARD, PROPERTY_COMPOENT_PCIE_CARD_NUM, __get_pcie_card_max_num},
    {DEVICE_TYPE_SDI_CARD, PROPERTY_COMPOENT_SD_CARD_NUM, __get_sdi_card_max_num},
    {DEVICE_TYPE_GPU_BOARD, PROPERTY_COMPOENT_GPU_BOARD_NUM, __get_component_max_num},
    {DEVICE_TYPE_IO_BOARD, PROPERTY_COMPOENT_IO_CARD_NUM, __get_component_max_num},
    {COMPONENT_TYPE_LSW, PROPERTY_COMPONENT_LSW_PORT_NUM, get_component_lsw_port_num},
    {COMPONENT_TYPE_EPM, PROPERTY_COMPONENT_EXP_MODULE_NUM, get_expansion_module_max_slot_num}
};

LOCAL GET_DEVICE_NANU_S g_get_device_manu[] = {
    {DEVICE_TYPE_CPU, __get_cpu_manu_info},
    {DEVICE_TYPE_MEMORY, __get_mem_manu_info},
    {DEVICE_TYPE_HARDDISK, __get_hard_disk_manu},
    {DEVICE_TYPE_MEZZ, __get_mezz_manu_info}
};

LOCAL gint32 __multi_get_cpu_properties(OBJ_HANDLE obj_handle, GSList **property_value)
{
    GSList *property_name_list = NULL;
    property_name_list = g_slist_append(property_name_list, PROPERTY_CPU_MANUFACTURER);
    property_name_list = g_slist_append(property_name_list, PROPERTY_CPU_VERSION);
    property_name_list = g_slist_append(property_name_list, PROPERTY_CPU_CURRENT_SPEED);
    property_name_list = g_slist_append(property_name_list, PROPERTY_CPU_PROCESSORID);
    property_name_list = g_slist_append(property_name_list, PROPERTY_CPU_CORE_COUNT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_CPU_THREAD_COUNT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_CPU_MEMORY_TEC);
    property_name_list = g_slist_append(property_name_list, PROPERTY_CPU_L1CACHE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_CPU_L2CACHE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_CPU_L3CACHE);

    gint32 ret = dfl_multiget_property_value(obj_handle, property_name_list, property_value);
    g_slist_free(property_name_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_multiget_property_value cpu Failed(%d).", ret);
        return ret;
    }

    return RET_OK;
}

LOCAL void __get_cpu_manu_string(GSList *property_value, gchar *puc_manu_info, gint32 info_len)
{
    const gchar *cpu_manufacturer = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 0), 0);
    const gchar *cpu_type = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 1), 0);
    const gchar *cpu_clockspeed = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 2), 0);
    const gchar *cpu_processor_id = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 3), 0);
    guint16 cpu_core_count = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 4));
    guint16 cpu_thread_count = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 5));
    const gchar *cpu_memory_tec = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 6), 0);
    guint32 cpu_L1_cache = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 7));
    guint32 cpu_L2_cache = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 8));
    guint32 cpu_L3_cache = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 9));

    gint32 ret = snprintf_s(puc_manu_info, info_len, info_len - 1, "%s,%s,%s,%s", cpu_manufacturer, cpu_type,
        cpu_clockspeed, cpu_processor_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    snprintf_with_unknown(puc_manu_info + strlen(puc_manu_info), info_len - strlen(puc_manu_info), cpu_core_count,
        " cores");
    snprintf_with_unknown(puc_manu_info + strlen(puc_manu_info), info_len - strlen(puc_manu_info), cpu_thread_count,
        " threads");

    ret = snprintf_s(puc_manu_info + strlen(puc_manu_info), info_len - strlen(puc_manu_info),
        info_len - strlen(puc_manu_info) - 1, ",%s", cpu_memory_tec);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    snprintf_with_guint32(puc_manu_info + strlen(puc_manu_info), info_len - strlen(puc_manu_info), cpu_L1_cache, " K");
    snprintf_with_guint32(puc_manu_info + strlen(puc_manu_info), info_len - strlen(puc_manu_info), cpu_L2_cache, " K");
    snprintf_with_guint32(puc_manu_info + strlen(puc_manu_info), info_len - strlen(puc_manu_info), cpu_L3_cache, " K");
}


LOCAL gint32 __get_cpu_manu_info(guint8 device_type, guint8 device_num, gchar *puc_manu_info, gint32 info_len)
{
    if (puc_manu_info == NULL) {
        debug_log(DLOG_ERROR, "%s %d: para is null.", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (device_num <= 0) {
        debug_log(DLOG_ERROR, "%s %d: invalid index(%d) .", __FUNCTION__, __LINE__, device_num);
        return RET_ERR;
    }

    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dal_get_specific_object_byte(CLASS_CPU, PROPERTY_CPU_PHYSIC_ID, device_num, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dal_get_specific_object_byte failed. ret(%d)", __FUNCTION__, ret);
        return RET_ERR;
    }

    GSList *property_value = NULL;
    ret = __multi_get_cpu_properties(obj_handle, &property_value);
    if (ret != RET_OK) {
        return ret;
    }

    __get_cpu_manu_string(property_value, puc_manu_info, info_len);

    g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);

    return RET_OK;
}

LOCAL gint32 __multi_get_mem_properties(OBJ_HANDLE obj_handle, GSList **property_value)
{
    GSList *property_name_list = NULL;
    property_name_list = g_slist_append(property_name_list, PROPERTY_MEM_MANUFACTURER);
    property_name_list = g_slist_append(property_name_list, PROPERTY_MEM_CLOCK_SPEED);
    property_name_list = g_slist_append(property_name_list, PROPERTY_MEM_CAPACITY);
    property_name_list = g_slist_append(property_name_list, PROPERTY_MEM_TYPE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_MEM_SN);
    property_name_list = g_slist_append(property_name_list, PROPERTY_MEM_MINIMUM_VOLTAGE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_MEM_RANK);
    property_name_list = g_slist_append(property_name_list, PROPERTY_MEM_BIT_WIDTH);
    property_name_list = g_slist_append(property_name_list, PROPERTY_MEM_TEC);

    gint32 ret = dfl_multiget_property_value(obj_handle, property_name_list, property_value);
    g_slist_free(property_name_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_multiget_property_value mem Failed(%d).", ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL void __get_mem_manu_string(GSList *property_value, gchar *puc_manu_info, gint32 info_len)
{
    const gchar *mem_manufacturer = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 0), 0);
    const gchar *mem_clockspeed = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 1), 0);
    const gchar *mem_capacity = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 2), 0);
    const gchar *mem_type = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 3), 0);
    const gchar *mem_sn = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 4), 0);
    guint16 minimum_voltage = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 5));
    guint8 minimum_rank = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 6));
    guint16 mem_bit_width = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 7));
    const gchar *mem_tec = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 8), 0);

    gint32 ret = snprintf_s(puc_manu_info, info_len, info_len - 1, "%s,%s,%s,%s,%s", mem_manufacturer, mem_clockspeed,
        mem_capacity, mem_type, mem_sn);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    snprintf_with_unknown(puc_manu_info + strlen(puc_manu_info), info_len - strlen(puc_manu_info), minimum_voltage,
        " mV");
    snprintf_with_unknown(puc_manu_info + strlen(puc_manu_info), info_len - strlen(puc_manu_info), minimum_rank,
        " rank");
    snprintf_with_unknown(puc_manu_info + strlen(puc_manu_info), info_len - strlen(puc_manu_info), mem_bit_width,
        " bit");

    ret = snprintf_s(puc_manu_info + strlen(puc_manu_info), info_len - strlen(puc_manu_info),
        info_len - strlen(puc_manu_info) - 1, ",%s", mem_tec);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
}


LOCAL gint32 __get_mem_manu_info(guint8 device_type, guint8 device_num, gchar *puc_manu_info, gint32 info_len)
{
    if (puc_manu_info == NULL) {
        debug_log(DLOG_ERROR, "%s %d: para is null.", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (device_num <= 0) {
        debug_log(DLOG_ERROR, "%s %d: invalid index(%d).", __FUNCTION__, __LINE__, device_num);
        return RET_ERR;
    }

    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dal_get_specific_object_byte(CLASS_MEMORY, PROPERTY_MEM_DIMM_NUM, device_num, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dal_get_specific_object_byte failed. ret(%d)", __FUNCTION__, ret);
        return RET_ERR;
    }

    GSList *property_value = NULL;
    ret = __multi_get_mem_properties(obj_handle, &property_value);
    if (ret != RET_OK) {
        return ret;
    }

    __get_mem_manu_string(property_value, puc_manu_info, info_len);

    g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);

    return RET_OK;
}


LOCAL gint32 __get_hard_disk_manu(guint8 device_type, guint8 device_num, gchar *puc_manu_info, gint32 info_len)
{
    if (puc_manu_info == NULL) {
        debug_log(DLOG_ERROR, "%s para is null.", __FUNCTION__);
        return RET_ERR;
    }

    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dal_get_specific_object_byte(CLASS_HDD_NAME, PROPERTY_HDD_ID, device_num, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s dal_get_specific_object_byte failed. ret(%d)", __FUNCTION__, ret);
        return RET_ERR;
    }

    gchar type_info[DEVICE_INFO_LEN + 1] = {0};
    ret = dal_get_property_value_string(obj_handle, PROPERTY_HDD_INTERFACE_TYPE_STR, type_info, sizeof(type_info));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dal_get_property_value_string mezz Failed(%d).", ret);
        return RET_ERR;
    }

    gchar serial_info[DEVICE_INFO_LEN + 1] = {0};
    ret = dal_get_property_value_string(obj_handle, PROPERTY_HDD_SERIAL_NUMBER, serial_info, sizeof(serial_info));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dal_get_property_value_string mezz Failed(%d).", ret);
        return RET_ERR;
    }

    gchar model_info[DEVICE_INFO_LEN + 1] = {0};
    ret = dal_get_property_value_string(obj_handle, PROPERTY_HDD_MODEL_NUMBER, model_info, sizeof(model_info));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dal_get_property_value_string mezz Failed(%d).", ret);
        return RET_ERR;
    }

    gchar manu_info[DEVICE_INFO_LEN + 1] = {0};
    ret = dal_get_property_value_string(obj_handle, PROPERTY_HDD_MANUFACTURER, manu_info, sizeof(manu_info));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dal_get_property_value_string mezz Failed(%d).", ret);
        return RET_ERR;
    }

    ret =
        snprintf_s(puc_manu_info, info_len, info_len - 1, "%s,%s,%s,%s", type_info, serial_info, model_info, manu_info);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 __get_mezz_manu_info(guint8 device_type, guint8 device_num, gchar *puc_manu_info, gint32 info_len)
{
    if (puc_manu_info == NULL) {
        debug_log(DLOG_ERROR, "%s para is null.", __FUNCTION__);
        return RET_ERR;
    }

    if (device_num <= 0) {
        debug_log(DLOG_ERROR, "%s invalid index(%d).", __FUNCTION__, device_num);
        return RET_ERR;
    }

    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dal_get_specific_object_byte(CLASS_MEZZCARD_NAME, PROPERTY_MEZZCARD_SLOT, device_num, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s dal_get_specific_object_byte failed. ret(%d)", __FUNCTION__, ret);
        return RET_ERR;
    }

    gchar mezz_name[DEVICE_INFO_LEN + 1] = {0};
    ret = dal_get_property_value_string(obj_handle, PROPERTY_MEZZCARD_PRODUCTNAME, mezz_name, sizeof(mezz_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dal_get_property_value_string mezz Failed(%d).", ret);
        return RET_ERR;
    }

    errno_t safe_fun_ret = strncpy_s(puc_manu_info, info_len, mezz_name, info_len - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    return RET_OK;
}

LOCAL gint32 __get_component_binded_manu_info(guint8 device_type, guint8 device_num, gchar *puc_manu, gint32 puc_len)
{
    const guint8 default_group_id = 1;
    component_union component_index = { 0 };
    component_index.device_info[0] = device_type;
    component_index.device_info[1] = device_num;
    component_index.device_info[2] = default_group_id;
    component_index.device_info[3] = 0;

    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_binded_object(CLASS_COMPONENT, (guint32)component_index.component_alias_id, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_binded_object failed. ret(%d)", __FUNCTION__, ret);
        return RET_ERR;
    }

    GVariant *property_value = NULL;
    ret = dfl_get_property_value(obj_handle, PROPERTY_COMPONENT_MANUFACTURER, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_property_value failed. ret(%d)", __FUNCTION__, ret);
        return RET_ERR;
    }
    const gchar *device_manu = g_variant_get_string(property_value, 0);
    if (device_manu == NULL) {
        debug_log(DLOG_ERROR, "%s g_variant_get_string failed", __FUNCTION__);
        g_variant_unref(property_value);
        return RET_ERR;
    }

    errno_t safe_fun_ret = strncpy_s(puc_manu, puc_len, device_manu, puc_len - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    g_variant_unref(property_value);

    return RET_OK;
}


LOCAL gint32 __get_component_type_list(guint8 device_type_code, GSList **component_list)
{
    if (component_list == NULL) {
        return RET_ERR;
    }

    GSList *component_obj_list = NULL;
    gint32 ret = dfl_get_object_list(CLASS_COMPONENT, &component_obj_list);
    if (ret != DFL_OK) {
        return ret;
    }

    for (GSList *obj_node = component_obj_list; obj_node != NULL; obj_node = obj_node->next) {
        OBJ_HANDLE obj_handle = (OBJ_HANDLE)obj_node->data;
        guint8 component_type = COMPONENT_TYPE_INVALID;
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
        if (device_type_code != component_type) {
            continue;
        }

        if (device_type_code != DEVICE_TYPE_HDD_BACKPLANE) {
            *component_list = g_slist_insert_sorted(*component_list, obj_node->data, component_device_num_compare_fun);
            continue;
        }

        guint8 virtual_flag = COMPONENT_VIRTUAL;
        (void)dal_get_property_value_byte(obj_handle, METHOD_COMPONENT_VIRTUALFLAG, &virtual_flag);
        if (virtual_flag == COMPONENT_VIRTUAL) {
            *component_list = g_slist_insert_sorted(*component_list, obj_node->data, component_device_num_compare_fun);
        }
    }

    g_slist_free(component_obj_list);
    return RET_OK;
}


LOCAL void __remove_hdd_comp_dup_node(GSList **pcomponent_list)
{
#define MAX_HDD_ID 0xff
    GSList *component_list = *pcomponent_list;
    if (component_list == NULL) {
        return;
    }

    GSList *obj_list = NULL;
    gint32 ret = dfl_get_object_list(CLASS_HDD, &obj_list);
    if (ret != DFL_OK) {
        return;
    }

    for (GSList *obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        OBJ_HANDLE hdd_obj_handle = (OBJ_HANDLE)obj_node->data;
        OBJ_HANDLE hdd_comp_handle = 0;
        ret = dfl_get_referenced_object(hdd_obj_handle, PROPERTY_HDD_REF_COMPONENT, &hdd_comp_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_DEBUG, "%s: Get Hdd (%s) RefComponent failed, ret: %d.", __FUNCTION__,
                dfl_get_object_name(hdd_obj_handle), ret);
            g_slist_free(obj_list);
            return;
        }

        guchar hdd_id = MAX_HDD_ID;
        ret = dal_get_property_value_byte(hdd_obj_handle, PROPERTY_HDD_ID, &hdd_id);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: Get Hdd (%s) ID failed, ret: %d.", __FUNCTION__,
                dfl_get_object_name(hdd_obj_handle), ret);
            g_slist_free(obj_list);
            return;
        }

        
        GSList *comp_obj_node = component_list;
        while (comp_obj_node != NULL) {
            OBJ_HANDLE comp_obj_handle = (OBJ_HANDLE)comp_obj_node->data;
            guchar device_Num = 0;
            ret = dal_get_property_value_byte(comp_obj_handle, PROPERTY_COMPONENT_DEVICENUM, &device_Num);
            if (ret != RET_OK) {
                debug_log(DLOG_DEBUG, "%s: Get Component (%s) device_num failed, ret: %d.", __FUNCTION__,
                    dfl_get_object_name(comp_obj_handle), ret);
                g_slist_free(obj_list);
                return;
            }

            
            if ((device_Num == hdd_id) && (comp_obj_handle != hdd_comp_handle)) {
                
                GSList *node_to_be_del = comp_obj_node;
                comp_obj_node = comp_obj_node->next;
                component_list = g_slist_delete_link(component_list, node_to_be_del);
                continue;
            }

            comp_obj_node = comp_obj_node->next;
        }
    }

    *pcomponent_list = component_list;

    g_slist_free(obj_list);
}


LOCAL gint32 __get_device_max_num_patch_board(guint32 *max_num)
{
    GSList *connector_obj_list = NULL;
    gint32 ret = dfl_get_object_list(CLASS_CONNECTOR_NAME, &connector_obj_list);
    if (ret != DFL_OK || g_slist_length(connector_obj_list) == 0) {
        debug_log(DLOG_ERROR, "%s dfl_get_object_list failed. ret(%d)", __FUNCTION__, ret);
        return COMP_CODE_STATUS_INVALID;
    }

    guint8 max_connector_slot = 0;
    gchar type[DEVICE_INFO_LEN] = {0};
    for (GSList *obj_node = connector_obj_list; obj_node; obj_node = obj_node->next) {
        (void)memset_s(type, sizeof(type), 0x0, sizeof(type));
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_CONNECTOR_TYPE, type, sizeof(type));

        if ((strcmp(type, CONNECTOR_TYPE_PCIE) != 0) && (strcmp(type, CONNECTOR_TYPE_PCIEPLUS) != 0) &&
            (strcmp(type, CONNECTOR_TYPE_PCIECONNECTOR) != 0)) {
            continue;
        }

        guint8 connector_slot = 0;
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_CONNECTOR_SLOT, &connector_slot);
        if (connector_slot > max_connector_slot) {
            max_connector_slot = connector_slot;
        }
    }

    *max_num = max_connector_slot;
    g_slist_free(connector_obj_list);
    return ret;
}

LOCAL gint32 __get_pcie_card_max_num(const gchar *property_str, guint32 *max_num)
{
    if (max_num == NULL) {
        debug_log(DLOG_ERROR, "%s The para is wrong, max_num is null.", __FUNCTION__);
        return COMP_CODE_OUTOF_RANGE;
    }

    gint32 ret = __get_device_max_num_patch_board(max_num);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_object_list failed. ret(%d)", __FUNCTION__, ret);
        return COMP_CODE_STATUS_INVALID;
    }
    return RET_OK;
}

LOCAL gint32 __get_sdi_card_max_num(const gchar *property_str, guint32 *max_num)
{
    if (max_num == NULL) {
        debug_log(DLOG_ERROR, "%s The para is wrong, max_num is null.", __FUNCTION__);
        return COMP_CODE_OUTOF_RANGE;
    }

    gint32 ret = dfl_get_object_count(CLASS_PCIE_SDI_CARD, max_num);
    if (ret != DFL_OK) {
        
        *max_num = 0;
    }
    return RET_OK;
}


LOCAL gint32 get_component_lsw_port_num(const gchar *property_str, guint32 *maximum)
{
    if ((maximum == NULL) || (property_str == NULL)) {
        debug_log(DLOG_ERROR, "The para is wrong, maximum is null OR property_str is null");
    }

    return dfl_get_object_count(CLASS_LSW_PORT_ATTR_LOCAL_CP, maximum);
}

LOCAL gint32 __get_raid_card_max_num(const gchar *property_str, guint32 *max_num)
{
    if (max_num == NULL) {
        debug_log(DLOG_ERROR, "%s The para is wrong, max_num is null.", __FUNCTION__);
        return COMP_CODE_OUTOF_RANGE;
    }

    GSList *connector_obj_list = NULL;
    gint32 ret = dfl_get_object_list(CLASS_RAID_CONTROLLER_NAME, &connector_obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_object_list failed. ret(%d)", __FUNCTION__, ret);
        return COMP_CODE_STATUS_INVALID;
    }

    guint32 raid_controller_count = 0;
    for (GSList *obj_node = connector_obj_list; obj_node; obj_node = obj_node->next) {
        guint8 type_id = SM_SOFT_RAID_CONTROLLER_TYPE_ID;
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_RAID_CONTROLLER_TYPE_ID, &type_id);
        if (type_id == SM_SOFT_RAID_CONTROLLER_TYPE_ID) { 
            continue;
        }
        raid_controller_count++;
    }

    // 1: The max_num number of RAID cards at least 1.
    *max_num = (raid_controller_count == 0) ? 1 : raid_controller_count;
    if (connector_obj_list != NULL) {
        g_slist_free(connector_obj_list);
    }
    return RET_OK;
}

LOCAL gint32 __get_component_max_num(const gchar *property_str, guint32 *max_num)
{
    if (max_num == NULL || property_str == NULL) {
        debug_log(DLOG_ERROR, "%s The para is wrong, max_num is null OR property_str is null.", __FUNCTION__);
        return COMP_CODE_OUTOF_RANGE;
    }

    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_object_handle(OBJ_PRODUCT_COMPONENT, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_object_handle failed, ret(%d).", __FUNCTION__, ret);
        return COMP_CODE_STATUS_INVALID;
    }

    guint8 property_value = 0;
    ret = dal_get_property_value_byte(obj_handle, property_str, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dal_get_property_value_byte failed. property=(%s) ret=(%d).", __FUNCTION__,
            property_str, ret);
        return COMP_CODE_STATUS_INVALID;
    }

    *max_num = property_value;

    return RET_OK;
}


LOCAL gint32 get_expansion_module_max_slot_num(const gchar *property_str, guint32 *max_num)
{
    gint32 ret = __get_component_max_num(property_str, max_num);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s __get_component_max_num failed, ret(%d).", __FUNCTION__, ret);
        return ret;
    }

    
    *max_num -= 1;
    return RET_OK;
}


LOCAL gint32 fix_max_num(const gchar *property_name, guint32 *max_num, guint8 device_type)
{
    if (max_num == NULL || property_name == NULL) {
        debug_log(DLOG_ERROR, "%s: The para is wrong, max_num is null OR property_name is null.", __FUNCTION__);
        return COMP_CODE_OUTOF_RANGE;
    }

    OBJ_HANDLE product_component_handle = 0;
    gint32 ret = dfl_get_object_handle(OBJ_PRODUCT_COMPONENT, &product_component_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_object_handle(%s) failed. ret(%d)", __FUNCTION__, OBJ_PRODUCT_COMPONENT, ret);
        return COMP_CODE_STATUS_INVALID;
    }

    guint8 product_component_num = 0;
    (void)dal_get_property_value_byte(product_component_handle, property_name, &product_component_num);

    GSList *obj_list = NULL;
    ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get object list(%s) failed. ret(%d)", __FUNCTION__, CLASS_COMPONENT, ret);
        return COMP_CODE_STATUS_INVALID;
    }

    guint8 device_num_max = 0;
    for (GSList *obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        guint8 type_code = COMPONENT_TYPE_INVALID;
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICE_TYPE, &type_code);
        if (type_code != device_type) {
            continue;
        }

        guint8 device_num = 0;
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICENUM, &device_num);
        if (device_num > device_num_max) {
            device_num_max = device_num;
        }
    }
    g_slist_free(obj_list);

    
    *max_num = (device_num_max > product_component_num) ? device_num_max : product_component_num;

    return RET_OK;
}


LOCAL gint32 __get_hdd_backplane_max_num(const gchar *property_name, guint32 *max_num)
{
    return fix_max_num(property_name, max_num, COMPONENT_TYPE_HDD_BACKPLANE);
}


LOCAL gint32 get_pcie_riser_max_num(const gchar *property_name, guint32 *max_num)
{
    return fix_max_num(property_name, max_num, COMPONENT_TYPE_PCIE_RISER);
}

gboolean get_fru_present_by_position(guint32 position)
{
#define CONNECTOR_NOT_IN_PRESENT 0
    
    GVariant *property_value = g_variant_new_uint32(position);
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_specific_object(CONNECTOR_CLASS_NAME, CONNECTOR_POSITION, property_value, &obj_handle);
    g_variant_unref(property_value);
    property_value = NULL;

    if (ret != DFL_OK) {
        return FALSE;
    }

    guint8 present = FALSE;
    (void)dal_get_property_value_byte(obj_handle, CONNECTOR_PRESENT, &present);

    return (present == CONNECTOR_NOT_IN_PRESENT) ? FALSE : TRUE;
}

gint32 get_position_info_by_fruid(DFT_COMPONENT_POSN_INFO_S *position_info)
{
    if (position_info == NULL) {
        debug_log(DLOG_ERROR, "The position_info is null.");
        return RET_ERR;
    }

    OBJ_HANDLE fru_obj_handle = 0;
    gint32 ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, position_info->fru_id, &fru_obj_handle);
    if (ret != RET_OK) {
        
        lock_connector();
        DFT_COMPONENT_POSN_INFO_S *pos_list_data =
            (DFT_COMPONENT_POSN_INFO_S *)g_slist_nth_data(get_connectors(), position_info->fru_id);
        if (pos_list_data == NULL) {
            unlock_connector();
            return RET_ERR;
        }

        (void)strncpy_s(position_info->silk_text, sizeof(position_info->silk_text), pos_list_data->silk_text,
            sizeof(position_info->silk_text) - 1);
        position_info->position_id = pos_list_data->position_id;

        if (get_fru_present_by_position(pos_list_data->position_id) == FALSE) {
            position_info->fru_id = MAX_FRU_ID;
        }
        unlock_connector();
        return RET_OK;
    }

    position_info->position_id = dfl_get_position(fru_obj_handle);
    OBJ_HANDLE connector_handle = 0;
    ret = dal_get_specific_object_uint32(CLASS_CONNECTOR_NAME, PROPERTY_CONNECTOR_POSITION, position_info->position_id,
        &connector_handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    gchar silktext[STRING_LEN_MAX] = {0};
    ret = dal_get_property_value_string(connector_handle, CONNECTOR_SILKTEXT, silktext, sizeof(silktext));
    if (ret != RET_OK) {
        return RET_ERR;
    }

    (void)strncpy_s(position_info->silk_text, sizeof(position_info->silk_text), silktext,
        sizeof(position_info->silk_text) - 1);

    return RET_OK;
}


gint32 get_position_info_by_position_id(DFT_COMPONENT_POSN_INFO_S *position_info)
{
    GSList *tmp_list = get_connectors();
    guint8 pos_offset = 0;
    DFT_COMPONENT_POSN_INFO_S *pos_list_data = NULL;

    
    lock_connector();

    while (tmp_list != NULL) {
        GSList *next = tmp_list->next;
        pos_list_data = (DFT_COMPONENT_POSN_INFO_S *)tmp_list->data;

        if (position_info->position_id == pos_list_data->position_id) {
            break;
        }

        pos_list_data = NULL;
        tmp_list = next;
        pos_offset++;
    }

    if (pos_list_data == NULL) {
        unlock_connector();
        return RET_ERR;
    }

    (void)strncpy_s(position_info->silk_text, sizeof(position_info->silk_text), pos_list_data->silk_text,
        sizeof(position_info->silk_text) - 1);

    if (get_fru_present_by_position(pos_list_data->position_id)) {
        position_info->fru_id = pos_offset;
    } else {
        position_info->fru_id = MAX_FRU_ID;
    }

    unlock_connector();

    return RET_OK;
}


gint32 get_position_info_by_silk(DFT_COMPONENT_POSN_INFO_S *position_info)
{
    GSList *tmp_list = get_connectors();
    guint8 pos_offset = 0;
    DFT_COMPONENT_POSN_INFO_S *pos_list_data = NULL;

    
    lock_connector();

    while (tmp_list != NULL) {
        GSList *next = tmp_list->next;
        pos_list_data = (DFT_COMPONENT_POSN_INFO_S *)tmp_list->data;

        if (!strncmp(position_info->silk_text, pos_list_data->silk_text, MAX_CONNECTOR_SILK_TEXT_LEN)) {
            break;
        }

        pos_list_data = NULL;
        tmp_list = next;
        pos_offset++;
    }

    if (pos_list_data == NULL) {
        unlock_connector();
        return RET_ERR;
    }

    position_info->position_id = pos_list_data->position_id;

    if (get_fru_present_by_position(pos_list_data->position_id)) {
        position_info->fru_id = pos_offset;
    } else {
        position_info->fru_id = MAX_FRU_ID;
    }

    unlock_connector();

    return RET_OK;
}

LOCAL gint32 __get_manu_by_device_type(guint8 device_type, guint8 device_num, gchar *puc_manu, gint32 puc_size,
    gint32 read_offset, gint32 read_len, guint8 *end_flag)
{
    gint32 ret;
    gchar uc_manu_info[DEVICE_INFO_LEN + 1] = {0};

    size_t i = 0;
    size_t func_size = sizeof(g_get_device_manu) / sizeof(GET_DEVICE_NANU_S);
    for (; i < func_size; i++) {
        if (g_get_device_manu[i].device_type == device_type) {
            ret = g_get_device_manu[i].exec_func(device_type, device_num, (gchar *)uc_manu_info, sizeof(uc_manu_info));
            break;
        }
    }

    if (i >= func_size) {
        ret = __get_component_binded_manu_info(device_type, device_num, (gchar *)uc_manu_info, sizeof(uc_manu_info));
    }

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, " get device manu info failed, device_type = %d, device_num = %d", device_type,
            device_num);
        return RET_ERR;
    }

    gint32 manu_size = read_len;
    *end_flag = 1;
    if ((strlen(uc_manu_info) + 1 - read_offset) <= read_len) {
        manu_size = strlen(uc_manu_info) - read_offset;
        *end_flag = 0;
    }
    if (manu_size < 0) {
        return RET_ERR;
    }
    errno_t safe_fun_ret = strncpy_s(puc_manu, puc_size, &uc_manu_info[read_offset], manu_size);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    return RET_OK;
}

gint32 get_device_manu_info(guint8 device_type, guint8 device_num, gchar *puc_manu, gint32 puc_size, gint32 read_offset,
    gint32 read_len, guint8 *end_flag)
{
    if (puc_manu == NULL) {
        debug_log(DLOG_ERROR, "%s puc_manu is null.", __FUNCTION__);
        return RET_ERR;
    }

    if (read_offset < 0 || read_offset > DEVICE_INFO_LEN) {
        debug_log(DLOG_ERROR, "%s read_offset is incorrect, read_offset is %d", __FUNCTION__, read_offset);
        return RET_ERR;
    }

    if (read_len < 0 || read_len >= DEVICE_INFO_LEN + 1) {
        debug_log(DLOG_ERROR, "%s read_len is incorrect, read_len is %d", __FUNCTION__, read_len);
        return RET_ERR;
    }

    if (!is_device_type_valid(device_type)) {
        debug_log(DLOG_ERROR, "%s The para is wrong, device_type=%d", __FUNCTION__, device_type);
        return RET_ERR;
    }

    return __get_manu_by_device_type(device_type, device_num, puc_manu, puc_size, read_offset, read_len, end_flag);
}


void get_device_num_by_index(guint8 *device_num, guint8 device_index, guint8 dev_type)
{
    if (device_num == NULL) {
        return;
    }

    GSList *component_list = NULL;
    gint32 ret = __get_component_type_list(dev_type, &component_list);
    if (ret != RET_OK) {
        return;
    }

    
    if (dev_type == DEVICE_TYPE_HARDDISK) {
        __remove_hdd_comp_dup_node(&component_list);
    }

    GSList *obj_node = g_slist_nth(component_list, device_index - 1);
    if (obj_node == NULL) {
        g_slist_free(component_list);
        return;
    }

    OBJ_HANDLE obj_handle = (OBJ_HANDLE)obj_node->data;
    g_slist_free(component_list);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICENUM, (guchar *)device_num);

    
    if (dev_type != DEVICE_TYPE_HARDDISK) {
        return;
    }

    guchar comp_groupid = 0;
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_GROUPID, (guchar *)&comp_groupid);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: Get Component (%s) device_num failed, ret: %d.", __FUNCTION__,
            dfl_get_object_name(obj_handle), ret);
        return;
    }

    // 26,16,8 Modify parameters
    guint32 component_alias_id =
        ((((guint32)dev_type) << 24) | ((guint32)(*device_num) << 16) | ((guint32)(comp_groupid) << 8));

    
    ret = dfl_bind_object_alias(obj_handle, component_alias_id);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: Component (%s) dfl_bind_object_alias (0x%08x) failed, ret: %d.", __FUNCTION__,
            dfl_get_object_name(obj_handle), component_alias_id, ret);
    }
}

gint32 get_device_max_num(guint8 device_type, guint8 *max_mum)
{
    if (max_mum == NULL) {
        debug_log(DLOG_ERROR, "%s The para is wrong, max_mum is null.", __FUNCTION__);
        return COMP_CODE_OUTOF_RANGE;
    }

    if (!is_device_type_valid(device_type)) {
        debug_log(DLOG_INFO, "%s The para is wrong, device_type = %d", __FUNCTION__, device_type);
        return COMP_CODE_INVALID_CMD;
    }

    gint32 arr_index = 0;
    gint32 arr_len = ARRAY_SIZE(g_max_num_get_arr);
    for (; arr_index < arr_len; arr_index++) {
        if (g_max_num_get_arr[arr_index].device_type != device_type) {
            continue;
        }

        GET_DEVICE_MAX_NUM_FUN p_max_num_get_fun = g_max_num_get_arr[arr_index].fun;
        if (p_max_num_get_fun == NULL) {
            debug_log(DLOG_INFO, "%s device_type = %d,  p_max_num_get_fun=null", __FUNCTION__, device_type);
            *max_mum = 1;
            return RET_OK;
        }

        guint32 num = 0;
        gint32 ret = p_max_num_get_fun((g_max_num_get_arr[arr_index].property_str), &num);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s p_max_num_get_fun fail, device_type = %d, max_mum=%d, ret=%d", __FUNCTION__,
                device_type, (*max_mum), ret);
            return ret;
        }
        *max_mum = (guint8)num;
        break;
    }

    if (arr_index == arr_len) {
        *max_mum = 1;
        debug_log(DLOG_ERROR, "%s no fun for device_type , device_type = %d, max_mum=%d", __FUNCTION__, device_type,
            *max_mum);
    }

    return RET_OK;
}

gint32 get_device_presence(guint8 device_type, guint8 device_num, guint8 *device_presence, guint8 *end_flag)
{
    if ((device_presence == NULL) || (end_flag == NULL)) {
        debug_log(DLOG_ERROR, "%s The para is null.", __FUNCTION__);
        return COMP_CODE_STATUS_INVALID;
    }

    if (!is_device_type_valid(device_type)) {
        debug_log(DLOG_ERROR, "%s The para is wrong,device_type=%d", __FUNCTION__, device_type);
        return COMP_CODE_INVALID_CMD;
    }

    *end_flag = 0;
    component_union component_index;
    component_index.device_info[0] = device_type;
    component_index.device_info[1] = device_num;
    component_index.device_info[2] = 1; 
    component_index.device_info[3] = 0;

    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_binded_object(CLASS_COMPONENT, (guint32)component_index.component_alias_id, &obj_handle);
    if (ret != DFL_OK) {
        *device_presence = 0;
        debug_log(DLOG_INFO, "%s dfl_get_binded_object failed. ret(%d), component_index[0](%d)[1](%d)", __FUNCTION__,
            ret, component_index.device_info[0], component_index.device_info[1]);
        return RET_OK;
    }

    guint8 presence = COMPONENT_NOT_IN_PRESENT;
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_PRESENCE, &presence);
    if (ret != DFL_OK) {
        *device_presence = 0;
        debug_log(DLOG_ERROR, "%s dal_get_property_value_byte failed. ret(%d), component_index[0](%d)[1](%d)",
            __FUNCTION__, ret, component_index.device_info[0], component_index.device_info[1]);
        return COMP_CODE_STATUS_INVALID;
    }

    if (presence == COMPONENT_NOT_IN_PRESENT) {
        debug_log(DLOG_DEBUG, "%s device_type=%d, device_num=%d is absent.", __FUNCTION__, device_type, device_num);
        *device_presence = presence;
        return RET_OK;
    }

    ret = dfl_get_object_handle(OBJECT_CHASSISPAYLOAD, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_object_handle failed. ret(%d)", __FUNCTION__, ret);
        return COMP_CODE_STATUS_INVALID;
    }

    guint8 power_state = 0;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_PAYLOAD_CHASSPWR_STATE, &power_state);

    if (power_state == 0) {
        if ((device_type == DEVICE_TYPE_CPU) || (device_type == DEVICE_TYPE_MEMORY)) {
            debug_log(DLOG_DEBUG, "%s device_type=%d, device_num=%d is unknown.", __FUNCTION__, device_type,
                device_num);
            *device_presence = COMPONENT_FORBIDDEN_SCAN;
            return RET_OK;
        }
    }

    *device_presence = presence;
    debug_log(DLOG_DEBUG, "#getDevicePresence:the presence is %d", *device_presence);
    return RET_OK;
}

gint32 get_device_health_status(guint8 device_type, guint8 device_num, gint32 *status, guint8 *end_flag)
{
    if ((status == NULL) || (end_flag == NULL)) {
        debug_log(DLOG_ERROR, "%s The para is null.", __FUNCTION__);
        return COMP_CODE_STATUS_INVALID;
    }

    if (!is_device_type_valid(device_type)) {
        debug_log(DLOG_ERROR, "%s The para is wrong,device_type=%d", __FUNCTION__, device_type);
        return COMP_CODE_INVALID_CMD;
    }

    guint8 device_presence = COMPONENT_NOT_IN_PRESENT;
    if (get_device_presence(device_type, device_num, &device_presence, end_flag) != RET_OK) {
        debug_log(DLOG_ERROR, "%s getDevicePresence failed, device_type=%d, device_num=%d", __FUNCTION__, device_type,
            device_num);
        return COMP_CODE_STATUS_INVALID;
    }

    if (device_presence == COMPONENT_NOT_IN_PRESENT) {
        debug_log(DLOG_DEBUG, "%s device_type=%d, device_num=%d is absent.", __FUNCTION__, device_type, device_num);
        *status = 4; // According to the protocol, the value 4 is returned if it is not in presence.
        *end_flag = 0;
        return RET_OK;
    }

    if (device_presence == COMPONENT_FORBIDDEN_SCAN) {
        debug_log(DLOG_DEBUG, "%s %d: device_type=%d, device_num=%d is unknown.", __FUNCTION__, __LINE__, device_type,
            device_num);
        *status = 5; // According to the protocol, the value 5 is returned if it is forbidden scan.
        *end_flag = 0;
        return RET_OK;
    }

    component_union component_index;
    component_index.device_info[0] = device_type;
    component_index.device_info[1] = device_num;
    component_index.device_info[2] = 1; 
    component_index.device_info[3] = 0;

    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_binded_object(CLASS_COMPONENT, (guint32)component_index.component_alias_id, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_binded_object failed. ret(%d)", __FUNCTION__, ret);
        return COMP_CODE_STATUS_INVALID;
    }

    guint8 device_status = 0;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_HEALTH, &device_status);

    *status = device_status;

    debug_log(DLOG_DEBUG, "#getDevicePresence:the presence is %d", device_status);
    *end_flag = 0;
    return RET_OK;
}

gint32 get_device_boardid(guint8 device_type, guint8 device_num, guint8 *id)
{
    if (id == NULL) {
        return RET_ERR;
    }

    component_union component_index;
    component_index.device_info[0] = device_type;
    component_index.device_info[1] = device_num;
    component_index.device_info[2] = 1; 
    component_index.device_info[3] = 0;

    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_binded_object(CLASS_COMPONENT, (guint32)component_index.component_alias_id, &obj_handle);
    if (ret != DFL_OK) {
        id[1] = 0; 
        id[0] = 0; 
        debug_log(DLOG_ERROR, "%s %d: dfl_get_binded_object failed. ret(%d)", __FUNCTION__, __LINE__, ret);
        return RET_OK;
    }

    guint16 board_id = 0;
    (void)dal_get_property_value_uint16(obj_handle, PROPERTY_COMPONENT_BOARDID, &board_id);

    id[1] = (guint8)(board_id >> 8);   
    id[0] = (guint8)(board_id & 0xff); 

    return RET_OK;
}

gint32 get_device_physical_num(guint8 device_type, guint8 device_num, guint8 *id)
{
    if (id == NULL) {
        return RET_ERR;
    }
    if (device_type != DEVICE_TYPE_MEZZ) {
        debug_log(DLOG_ERROR, "The device type is not mezz, device_type is %d", device_type);
        return RET_ERR;
    }

    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_object_handle(BMC_BOARD_NAME, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get bmc_board handle fail, result is %d", ret);
        return ret;
    }

    guint8 slot_id = 0;
    (void)dal_get_property_value_byte(obj_handle, BMC_SLOT_ID_NAME, &slot_id);

    
    const guint8 max_slot_id = 16;
    if (slot_id > max_slot_id) {
        slot_id = slot_id - max_slot_id;
    }

    
    ret = dfl_get_object_handle(CLASS_PRODUCT_COMPONENT, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get ProductComponet handle fail, result is %d", ret);
        return ret;
    }

    guint8 mezz_location = 0;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPOENT_MEZZ_LOCATION, &mezz_location);

    guint8 physical_num = device_num; 

    return fill_device_physical_num(mezz_location, device_num, slot_id, physical_num, id);
}

LOCAL gint32 __get_device_property_paracheck(guint8 device_type, gchar *ret_buff, gint32 read_offset, gint32 read_len)
{
    if (ret_buff == NULL) {
        debug_log(DLOG_ERROR, "%s The ret_buff is null.", __FUNCTION__);
        return RET_ERR;
    }

    if ((read_offset < 0) || (read_offset > DEVICE_INFO_LEN)) {
        debug_log(DLOG_ERROR, "%s Incorrect read_offset value, read_offset = %d, device_type is %d", __FUNCTION__,
            read_offset, device_type);
        return RET_ERR;
    }

    if (!is_device_type_valid(device_type)) {
        debug_log(DLOG_ERROR, "%s The para is wrong,device_type= %d", __FUNCTION__, device_type);
        return RET_ERR;
    }
    if (read_len <= 0) {
        debug_log(DLOG_ERROR, "%s read_len=%d is invalid.", __FUNCTION__, read_len);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 get_device_info_cpy_len(const gchar *tmpbuf, gint32 read_offset, gint32 read_len, guint8 *end_flag)
{
    gint32 copy_len = read_len;
    if ((strlen(tmpbuf) + 1 - read_offset) <= read_len) {
        *end_flag = 0;
        copy_len = strlen(tmpbuf) - read_offset;
    }
    return copy_len;
}
gint32 get_device_property(guint8 property_type, guint8 device_type, guint8 device_num, gchar *ret_buff,
    gint32 ret_buff_size, gint32 read_offset, gint32 read_len, guint8 *end_flag)
{
    if (__get_device_property_paracheck(device_type, ret_buff, read_offset, read_len) != RET_OK) {
        return RET_ERR;
    }

    component_union component_index;
    component_index.device_info[0] = device_type;
    component_index.device_info[1] = device_num;
    component_index.device_info[2] = 1; 
    component_index.device_info[3] = 0;

    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_binded_object(CLASS_COMPONENT, (guint32)component_index.component_alias_id, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_binded_object failed. ret(%d)", __FUNCTION__, ret);
        return RET_ERR;
    }

    const char *property_key = PROPERTY_COMPONENT_DEVICE_NAME;
    if (property_type == DEVICE_PROPERTY_TYPE_LOCATION) {
        property_key = PROPERTY_COMPONENT_LOCATION;
    } else if (property_type == DEVICE_PROPERTY_TYPE_FUNCTION) {
        property_key = PROPERTY_COMPONENT_FUNCTION;
    }

    GVariant *property_value = NULL;
    ret = dfl_get_property_value(obj_handle, property_key, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_property_value failed, property_key is %s ret(%d)", __FUNCTION__,
            property_key, ret);
        return RET_ERR;
    }

    const gchar *device_value = g_variant_get_string(property_value, 0);
    gchar tmpbuf[DEVICE_INFO_LEN + 1] = {0};
    errno_t safe_fun_ret = strncpy_s(tmpbuf, sizeof(tmpbuf), device_value, sizeof(tmpbuf) - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    g_variant_unref(property_value);

    *end_flag = 1;
    gint32 copy_len = get_device_info_cpy_len(tmpbuf, read_offset, read_len, end_flag);
    if (copy_len < 0) {
        return RET_ERR;
    }
    safe_fun_ret = strncpy_s(ret_buff, ret_buff_size, &tmpbuf[read_offset], copy_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    return RET_OK;
}

gint32 get_device_groupid(guint8 device_type, guint8 device_num, gchar *groupid, guint8 *end_flag)
{
    if (groupid == NULL) {
        return RET_ERR;
    }

    if (!is_device_type_valid(device_type)) {
        debug_log(DLOG_ERROR, "%s The para is wrong,device_type=%d", __FUNCTION__, device_type);
        return RET_ERR;
    }

    component_union component_index;
    component_index.device_info[0] = device_type;
    component_index.device_info[1] = device_num;
    component_index.device_info[2] = 1; 
    component_index.device_info[3] = 0;

    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_binded_object(CLASS_COMPONENT, (guint32)component_index.component_alias_id, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_binded_object failed. ret(%d)", __FUNCTION__, ret);
        return RET_ERR;
    }

    guint8 device_group_id = 0;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_GROUPID, &device_group_id);

    *groupid = device_group_id;
    
    *end_flag = 0;
    return RET_OK;
}

gint32 get_device_fruid(guint8 device_type, guint8 device_num, guint8 *fru_id)
{
    if (fru_id == NULL) {
        return RET_ERR;
    }

    if (!is_device_type_valid(device_type)) {
        debug_log(DLOG_ERROR, "%s The para is wrong,device_type=%d", __FUNCTION__, device_type);
        return RET_ERR;
    }

    component_union component_index;
    component_index.device_info[0] = device_type;
    component_index.device_info[1] = device_num;
    component_index.device_info[2] = 1; 
    component_index.device_info[3] = 0;

    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_binded_object(CLASS_COMPONENT, (guint32)component_index.component_alias_id, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_binded_object failed. ret(%d)", __FUNCTION__, ret);
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_FRUID, fru_id);

    return 0;
}


gint32 correct_device_attri(guint8 *device_type, guint8 *device_ori_type, guint8 *device_num)
{
    if (device_type == NULL || device_ori_type == NULL || device_num == NULL) {
        return RET_ERR;
    }

    if (*device_type == DEVICE_TYPE_SDI_CARD) {
        *device_ori_type = DEVICE_TYPE_SDI_CARD;
        *device_type = DEVICE_TYPE_PCIE_CARD;
        
        if (*device_num == INVALID_UINT8) {
            return RET_OK;
        }
        
        OBJ_HANDLE sdi_hnd = 0;
        gint32 ret = get_sdi_card_handle_by_logic_num(*device_num, &sdi_hnd);
        if (ret != RET_OK) {
            return ret;
        }

        ret = dal_get_extern_value_byte(sdi_hnd, PROPERTY_CARD_SLOT, device_num, DF_AUTO);
        if (ret != RET_OK) {
            return COMP_CODE_UNKNOWN;
        }
        return RET_OK;
    }

    if ((*device_type == DEVICE_TYPE_RAID_CARD) || (*device_type == DEVICE_TYPE_HARDDISK) ||
        (*device_type == DEVICE_TYPE_HDD_BACKPLANE)) {
        get_device_num_by_index(device_num, *device_num, *device_type);
    }

    return RET_OK;
}

LOCAL gint32 __get_product_name_handle(OBJ_HANDLE obj_handle, gint is_customize, gchar *product_name, guint32 name_size)
{
    guchar arm_enable = DISABLE;
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);

    guint32 product_version = 0;
    (void)dal_get_product_version_num(&product_version);

    if ((is_customize != NEW_PRODUCT_NAME) || (product_version <= PRODUCT_VERSION_V5) || (arm_enable != ENABLE)) {
        return RET_OK;
    }

    
    gchar object_name[MAX_SYSNAME_LEN] = {0};
    gint32 ret = dal_get_property_value_string(obj_handle, BMC_NEW_PRODUCT_PRO_NAME, object_name, sizeof(object_name));
    if (ret != RET_OK) {
        return ret;
    }

    errno_t securec_rv = strncpy_s(product_name, name_size, object_name, strlen(object_name));
    if (securec_rv != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, securec_rv);
    }

    
    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_string(product_name));
    ret = dfl_call_class_method(obj_handle, METHOD_PRODUCT_SET_PRODUCT_NAME, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        return ret;
    }
    input_list = NULL;

    input_list = g_slist_append(input_list, g_variant_new_string("\0"));
    ret = dfl_call_class_method(obj_handle, METHOD_PRODUCT_SET_PRODUCT_VERSION, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        return ret;
    }

    return RET_OK;
}

LOCAL gint32 __get_product_ver_name_handle(OBJ_HANDLE obj_handle, gchar *product_name, guint32 name_size)
{
    GVariant *property_value = NULL;
    gint32 ret = dfl_get_property_value(obj_handle, BMC_PRODUCT_PRO_NAME, &property_value);
    if (ret != DFL_OK) {
        return ret;
    }

    gsize pro_name_len = 0;
    const gchar *tmp_string = g_variant_get_string(property_value, &pro_name_len);
    errno_t securec_rv = strncpy_s(product_name, name_size, tmp_string, name_size - 1);
    if (securec_rv != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, securec_rv);
    }
    g_variant_unref(property_value);
    property_value = NULL;

    if (name_size <= pro_name_len + 1) {
        return RET_OK;
    }
    
    ret = dfl_get_property_value(obj_handle, BMC_PRODUCT_VER_NAME, &property_value);
    if (ret != DFL_OK) {
        return ret;
    }

    gsize pro_ver_len = 0;
    tmp_string = g_variant_get_string(property_value, &pro_ver_len);
    debug_log(DLOG_DEBUG, "__get_product_name, ProductVersion len = %" G_GSIZE_FORMAT "!", pro_ver_len);
    if (pro_ver_len == 0) {
        g_variant_unref(property_value);
        return RET_OK;
    }

    ret = snprintf_truncated_s(product_name + pro_name_len, name_size - pro_name_len, " %s", tmp_string);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_truncated_s fail ret=%d", __FUNCTION__, ret);
    }
    g_variant_unref(property_value);
    return RET_OK;
}


gint32 get_product_name(gint is_customize, gchar *product_name, guint32 name_size)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_object_handle(BMC_PRODUCT_NAME_APP, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s call dfl_get_object_handle failed, error = %d", __FUNCTION__, ret);
        return ret;
    }

    if (__get_product_name_handle(obj_handle, is_customize, product_name, name_size) != RET_OK) {
        return RET_ERR;
    }
    if (__get_product_ver_name_handle(obj_handle, product_name, name_size) != RET_OK) {
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 get_obj_eeprom_fru(OBJ_HANDLE *obj_eeprom_fru)
{
    gint32 ret = 0;
    OBJ_HANDLE obj_mainboard_component = OBJ_HANDLE_COMMON;
    guint8 eeprom_fru_id = 0;

    ret = dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE,
        MAINBOARD_COMPONENT_DEVICE_TYPE, &obj_mainboard_component);
    if (obj_mainboard_component == OBJ_HANDLE_COMMON) {
        debug_log(DLOG_ERROR, "%s: failed to find mainboard component", __FUNCTION__);
        return ret;
    }
    (void)dal_get_property_value_byte(obj_mainboard_component, PROPERTY_COMPONENT_FRUID, &eeprom_fru_id);
    ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, eeprom_fru_id, obj_eeprom_fru);
    if (*obj_eeprom_fru == OBJ_HANDLE_COMMON) {
        debug_log(DLOG_ERROR, "%s: failed to find eeprom fru obj", __FUNCTION__);
        return ret;
    }

    return ret;
}


gint32 get_device_type_eeprom_block(guint32 offset, guint32 size, guint8 *eep_block)
{
    gint32 ret;
    OBJ_HANDLE obj_eeprom_fru = OBJ_HANDLE_COMMON;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const guint8 *eep_block_temp = NULL;
    gsize eep_block_temp_size;
    guint16 crc_calulate;
    guint16 crc;

    ret = get_obj_eeprom_fru(&obj_eeprom_fru);
    if (obj_eeprom_fru == OBJ_HANDLE_COMMON) {
        debug_log(DLOG_ERROR, "%s: failed to find eeprom fru obj", __FUNCTION__);
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32(offset));
    input_list = g_slist_append(input_list, g_variant_new_uint32(size));
    ret = dfl_call_class_method(obj_eeprom_fru, METHOD_FRU_GET_EEPROM_DATA, NULL, input_list, &output_list);
    if (ret != DFL_OK) {
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        debug_log(DLOG_ERROR, "%s: failed to call eeprom fru obj method, ret=%d, obj=%s", __FUNCTION__, ret,
            dfl_get_object_name(obj_eeprom_fru));
        return ret;
    }

    eep_block_temp = (const guint8 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(output_list, 0),
        &eep_block_temp_size, sizeof(guint8));
    crc_calulate =
        (guint16)arith_make_crc_checksum(0, eep_block_temp + DEVICE_INFO_AREA_CRC_LEN,
            eep_block_temp_size - DEVICE_INFO_AREA_CRC_LEN);
    crc = MAKE_WORD(eep_block_temp[1], eep_block_temp[0]);
    if (crc_calulate != crc) {
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        debug_log(DLOG_INFO, "%s: crc16 not equal, storage crash or has not write, offset=%u, eep_block_temp_size=%zu",
            __FUNCTION__, offset, eep_block_temp_size);
        return RET_ERR;
    }
    ret = memcpy_s(eep_block, size, eep_block_temp, eep_block_temp_size);
    if (ret != RET_OK) {
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        debug_log(DLOG_ERROR, "%s: failed to memcpy_s, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    return RET_OK;
}


LOCAL gint32 set_device_type_eeprom_block(guint32 offset, guint32 size, guint8 *eep_block)
{
    gint32 ret;
    OBJ_HANDLE obj_eeprom_fru = OBJ_HANDLE_COMMON;
    GSList *input_list = NULL;

    ret = get_obj_eeprom_fru(&obj_eeprom_fru);
    if (obj_eeprom_fru == OBJ_HANDLE_COMMON) {
        debug_log(DLOG_ERROR, "%s: failed to find eeprom fru obj", __FUNCTION__);
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32(offset));
    input_list = g_slist_append(input_list,
        (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, eep_block, size, sizeof(guint8)));
    ret = dfl_call_class_method(obj_eeprom_fru, METHOD_FRU_SET_EEPROM_DATA, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: save to eeprom failed, ret=%d, offset=%u, size=%u", __FUNCTION__, ret, offset,
            size);
        return ret;
    }

    return RET_OK;
}


gint32 save_product_uid(guint8 device_type)
{
    OBJ_HANDLE product_object = 0;
    guint32 product_unique_id = 0;

    gint32 ret = dfl_get_object_handle(CLASS_NAME_PRODUCT, &product_object);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:Get object %s handle failed, ret=%d",
            __FUNCTION__, CLASS_NAME_PRODUCT, ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_uint32(product_object, PROPERTY_PRODUCT_UNIQUE_ID, &product_unique_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get product uid is failed! ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    // 覆写ProductUniqueID的低八位
    guint32 unique_id = ((product_unique_id & 0xffffff00) | device_type);
    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_uint32(unique_id));
    ret = dfl_call_class_method(product_object, METHOD_PRODUCT_SET_PRODUCT_UID, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: call method product uniqueid fail, ret=[%d]", __FUNCTION__, ret);
        return ret;
    }

    return RET_OK;
}


gint32 save_product_name(guint8 device_type)
{
    gboolean save_flag = FALSE;
    OBJ_HANDLE product_object = 0;
    gchar pro_name[NAME_LEN] = { 0 };
    gchar *product_name_upd = NULL;

    gint32 ret = dfl_get_object_handle(CLASS_NAME_PRODUCT, &product_object);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get object %s handle failed", __FUNCTION__, CLASS_NAME_PRODUCT);
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    ret = dal_get_property_value_string(product_object, BMC_PRODUCT_PRO_NAME, pro_name, sizeof(pro_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get product name is failed! ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    // 根据k机型标志更新productname
    if (device_type == TS2280_TO_TS2280K) {
        if (strncmp(TS2280_PRO_NAME_DEFAULT, pro_name, strlen(TS2280_PRO_NAME_DEFAULT)) == 0) {
            product_name_upd = TS2280_PRO_NAME_REPLACE;
            save_flag = TRUE;
        }
    }
    if (device_type == TS5280_TO_TS5280K) {
        if (strncmp(TS5280_PRO_NAME_DEFAULT, pro_name, strlen(TS5280_PRO_NAME_DEFAULT)) == 0) {
            product_name_upd = TS5280_PRO_NAME_REPLACE;
            save_flag = TRUE;
        }
    }

    if (save_flag == TRUE) {
        GSList *input_list = NULL;
        input_list = g_slist_append(input_list, g_variant_new_string(product_name_upd));
        ret = dfl_call_class_method(product_object, METHOD_PRODUCT_SET_PRODUCT_NAME, NULL, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s, call method product name fail, ret=[%d]", __FUNCTION__, ret);
            return ret;
        }
    }

    return RET_OK;
}


gint32 save_product_name_recover(guint8 device_type)
{
    gboolean save_flag = FALSE;
    OBJ_HANDLE product_object = 0;
    gchar pro_name[NAME_LEN] = { 0 };
    gchar *product_name_upd = NULL;

    gint32 ret = dfl_get_object_handle(CLASS_NAME_PRODUCT, &product_object);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get object %s handle failed", __FUNCTION__, CLASS_NAME_PRODUCT);
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    ret = dal_get_property_value_string(product_object, BMC_PRODUCT_PRO_NAME, pro_name, sizeof(pro_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get product name is failed! ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    // 根据非k标志更新productname
    if (device_type == TS2280K_TO_TS2280) {
        if (strncmp(TS2280_PRO_NAME_REPLACE, pro_name, strlen(TS2280_PRO_NAME_REPLACE)) == 0) {
            product_name_upd = TS2280_PRO_NAME_DEFAULT;
            save_flag = TRUE;
        }
    }
    if (device_type == TS5280K_TO_TS5280) {
        if (strncmp(TS5280_PRO_NAME_REPLACE, pro_name, strlen(TS5280_PRO_NAME_REPLACE)) == 0) {
            product_name_upd = TS5280_PRO_NAME_DEFAULT;
            save_flag = TRUE;
        }
    }

    if (save_flag == TRUE) {
        GSList *input_list = NULL;
        input_list = g_slist_append(input_list, g_variant_new_string(product_name_upd));
        ret = dfl_call_class_method(product_object, METHOD_PRODUCT_SET_PRODUCT_NAME, NULL, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s, call method product name fail, ret=[%d]", __FUNCTION__, ret);
            return ret;
        }
    }

    return RET_OK;
}



gint32 save_product_newname(guint8 device_type)
{
    gboolean save_flag = FALSE;
    OBJ_HANDLE product_object = 0;
    gchar pro_name[NAME_LEN] = { 0 };
    gchar *product_name_upd = NULL;

    gint32 ret = dfl_get_object_handle(CLASS_NAME_PRODUCT, &product_object);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get object %s handle failed", __FUNCTION__, CLASS_NAME_PRODUCT);
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    ret = dal_get_property_value_string(product_object, BMC_NEW_PRODUCT_PRO_NAME, pro_name, sizeof(pro_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get productname is failed! ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    // 根据k机型标志更新newproductname
    if (device_type == TS2280_TO_TS2280K) {
        if (strncmp(TS2280_PRO_NAME_DEFAULT, pro_name, strlen(TS2280_PRO_NAME_DEFAULT)) == 0) {
            product_name_upd = TS2280_PRO_NAME_REPLACE;
            save_flag = TRUE;
        }
    }
    if (device_type == TS5280_TO_TS5280K) {
        if (strncmp(TS5280_PRO_NAME_DEFAULT, pro_name, strlen(TS5280_PRO_NAME_DEFAULT)) == 0) {
            product_name_upd = TS5280_PRO_NAME_REPLACE;
            save_flag = TRUE;
        }
    }

    if (save_flag == TRUE) {
        GSList *input_list = NULL;
        input_list = g_slist_append(input_list, g_variant_new_string(product_name_upd));
        ret = dfl_call_class_method(product_object, METHOD_PRODUCT_SET_NEW_PRODUCT_NAME, NULL, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s, call method newproductname fail, ret=[%d]", __FUNCTION__, ret);
            return ret;
        }
    }

    return RET_OK;
}


gint32 save_product_newname_recover(guint8 device_type)
{
    gboolean save_flag = FALSE;
    OBJ_HANDLE product_object = 0;
    gchar pro_name[NAME_LEN] = { 0 };
    gchar *product_name_upd = NULL;

    gint32 ret = dfl_get_object_handle(CLASS_NAME_PRODUCT, &product_object);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get object %s handle failed", __FUNCTION__, CLASS_NAME_PRODUCT);
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    ret = dal_get_property_value_string(product_object, BMC_NEW_PRODUCT_PRO_NAME, pro_name, sizeof(pro_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get productname is failed! ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    // 根据非k标志更新newproductname
    if (device_type == TS2280K_TO_TS2280) {
        if (strncmp(TS2280_PRO_NAME_REPLACE, pro_name, strlen(TS2280_PRO_NAME_REPLACE)) == 0) {
            product_name_upd = TS2280_PRO_NAME_DEFAULT;
            save_flag = TRUE;
        }
    }
    if (device_type == TS5280K_TO_TS5280) {
        if (strncmp(TS5280_PRO_NAME_REPLACE, pro_name, strlen(TS5280_PRO_NAME_REPLACE)) == 0) {
            product_name_upd = TS5280_PRO_NAME_DEFAULT;
            save_flag = TRUE;
        }
    }

    if (save_flag == TRUE) {
        GSList *input_list = NULL;
        input_list = g_slist_append(input_list, g_variant_new_string(product_name_upd));
        ret = dfl_call_class_method(product_object, METHOD_PRODUCT_SET_NEW_PRODUCT_NAME, NULL, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s, call method newproductname fail, ret=[%d]", __FUNCTION__, ret);
            return ret;
        }
    }

    return RET_OK;
}


LOCAL gint32 get_new_device_type(guint8 device_type, guint8 *device_type_new)
{
    gint32 ret = 0;
    guint32 product_unique_id = 0;
    OBJ_HANDLE product_object = 0;

    if (device_type != TS2280_TO_TS2280K && device_type != TS2280K_TO_TS2280 && device_type != 0xff) {
        debug_log(DLOG_ERROR, "%s: device_type=[%hhu] is invalid", __FUNCTION__, device_type);
        return COMP_CODE_INVALID_FIELD;
    }

    if (device_type == TS2280_TO_TS2280K || device_type == TS2280K_TO_TS2280) {
        ret = dfl_get_object_handle(CLASS_NAME_PRODUCT, &product_object);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s:Get object %s handle failed, ret=%d",
                __FUNCTION__, CLASS_NAME_PRODUCT, ret);
            return RET_ERR;
        }

        ret = dal_get_property_value_uint32(product_object, PROPERTY_PRODUCT_UNIQUE_ID, &product_unique_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get product uid is failed! ret=%d", __FUNCTION__, ret);
            return RET_ERR;
        }
        guint8 value = product_unique_id & 0xff;

        if (value == TS5280_TO_TS5280K || value == TS5280K_TO_TS5280) {
            device_type = (device_type + 1);
            *device_type_new = device_type;
        } else {
            *device_type_new = device_type;
        }

        // 覆写ProductUniqueID的低八位
        guint32 unique_id = ((product_unique_id & 0xffffff00) | device_type);
        GSList *input_list = NULL;
        input_list = g_slist_append(input_list, g_variant_new_uint32(unique_id));
        ret = dfl_call_class_method(product_object, METHOD_PRODUCT_SET_PRODUCT_UID, NULL, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: call method product uniqueid fail, ret=[%d]", __FUNCTION__, ret);
            return ret;
        }
    }
    if (device_type == 0xff) {
        *device_type_new = device_type;
        debug_log(DLOG_INFO, "%s: No need to update information", __FUNCTION__);
    }

    return RET_OK;
}


LOCAL gint32 save_device_info_type(const gchar *device_data, guint32 receive_device_data_len)
{
    DEVICE_TYPE_STR_AREA str_area = { 0 };
    guint8 device_type = 0;
    guint8 device_type_new = 0;

    gint32 ret = memcpy_s(str_area.device_type_str, sizeof(str_area.device_type_str),
        device_data, receive_device_data_len);
    if (ret != 0) {
        debug_log(DLOG_ERROR, "memcpy data to device info data failed, ret %d.", ret);
        return RET_ERR;
    }

    device_type = str_area.device_type_str[0];

    // 判断k机型标志是否正确
    ret = get_new_device_type(device_type, &device_type_new);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: device type validity check failed, ret=%d", __FUNCTION__, ret);
        return COMP_CODE_UNKNOWN;
    }

    str_area.device_type_str[0] = device_type_new;
    if (str_area.device_type_str[0] == TS2280_TO_TS2280K || str_area.device_type_str[0] == TS5280_TO_TS5280K) {
        ret = save_product_name(device_type_new);
        ret += save_product_newname(device_type_new);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: save info failed, ret=%d", __FUNCTION__, ret);
            return COMP_CODE_UNKNOWN;
        }
    }
    if (str_area.device_type_str[0] == TS2280K_TO_TS2280 || str_area.device_type_str[0] == TS5280K_TO_TS5280) {
        ret = save_product_name_recover(device_type_new);
        ret += save_product_newname_recover(device_type_new);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: save info failed, ret=%d", __FUNCTION__, ret);
            return COMP_CODE_UNKNOWN;
        }
    }

    str_area.length = DEVICE_EEP_TYPE_EEP_DATA_LEN;
    str_area.crc16 = (guint16)arith_make_crc_checksum(0, (guint8 *)(&str_area) + DEVICE_INFO_AREA_CRC_LEN,
        DEVICE_EEP_TYPE_EEP_SIZE - DEVICE_INFO_AREA_CRC_LEN);

    ret = set_device_type_eeprom_block(DEVICE_EEP_TYPE_OFFSET, DEVICE_EEP_TYPE_EEP_SIZE, (guint8 *)&str_area);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set device type failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }
    ret = get_device_type_eeprom_block(DEVICE_EEP_TYPE_OFFSET, DEVICE_EEP_TYPE_EEP_SIZE, (guint8 *)&str_area);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get eep device type failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    return RET_OK;
}


LOCAL gint32 is_valid_input_for_set_device_info_str(const SET_DEVICE_TYPE_REQ_S *msg_req, guint8 len_limit)
{
    if (len_limit >= MAX_PROPERTY_VALUE_LEN) {
        debug_log(DLOG_ERROR, "%s: len_limit=%hhu too large", __FUNCTION__, len_limit);
        return COMP_CODE_INVALID_FIELD;
    }
    if ((msg_req == NULL) || (msg_req->length == 0)) {
        debug_log(DLOG_ERROR, "%s: req_msg is invalid", __FUNCTION__);
        return COMP_CODE_UNKNOWN;
    }
    if (msg_req->lastdata != IPMI_DEVICE_TYPE_LAST_DATA) {
        debug_log(DLOG_ERROR, "%s: lastdata is invalid, lastdata=%d", __FUNCTION__, msg_req->lastdata);
        return COMP_CODE_INVALID_FIELD;
    }
    if (msg_req->length > len_limit) {
        debug_log(DLOG_ERROR, "%s: req_msg data too long, msg_len=%d, len_limit=%hhu", __FUNCTION__, msg_req->lastdata,
            len_limit);
        return COMP_CODE_INVALID_FIELD;
    }

    return RET_OK;
}


gint32 ipmi_cmd_set_device_type_str(gconstpointer req_msg, const SET_DEVICE_TYPE_REQ_S *msg_req,
    guint8 len_limit)
{
    gchar device_data[MAX_PROPERTY_VALUE_LEN] = {0};
    guint32 receive_device_data_len = 0;
    gint32 ret;

    if ((ret = is_valid_input_for_set_device_info_str(msg_req, len_limit)) != RET_OK) {
        return ret;
    }

    errno_t safe_fun_ret = memcpy_s(device_data, sizeof(device_data), msg_req->data, msg_req->length);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret=%d", __FUNCTION__, safe_fun_ret);
        return COMP_CODE_UNKNOWN;
    }

    receive_device_data_len = msg_req->length;
    if ((receive_device_data_len + 1) > (sizeof(device_data) - 1)) {
        debug_log(DLOG_ERROR, "%s: receive_device_data_len %d invalid", __FUNCTION__, receive_device_data_len);
        return COMP_CODE_INVALID_FIELD;
    }

    device_data[receive_device_data_len + 1] = 0;

    // 保存机器类型信息到EEPROM
    ret = save_device_info_type(device_data, receive_device_data_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: save to eeprom failed, ret=%d", __FUNCTION__, ret);
        return COMP_CODE_UNKNOWN;
    }

    ipmi_operation_log(req_msg, "Set Device Type to (%hhu) successfully", device_data[receive_device_data_len-1]);

    return RET_OK;
}