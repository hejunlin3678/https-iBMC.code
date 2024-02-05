


#include "pme_app/pme_app.h"
#include "pme_app/smlib/sml_base.h"


#include "raid_card.h"
#include "get_version.h"
#include "pcie_card.h"
#include "card_manage_module.h"


typedef struct {
    guint8 bus;
    guint8 device;
    guint8 function;
    guint8 error_code;
} TEMP_BIOS_ERRCODE;

typedef struct associated_resource {
    guint8 res_id;
    const gchar res_str[ACCESSOR_NAME_LEN];
} ASSOCIATED_RESOURCE;

typedef struct info_from_component {
    gchar location[ACCESSOR_NAME_LEN];
    gchar serial_num[ACCESSOR_NAME_LEN];
} INFO_FROM_COMPONENT;

LOCAL gint32 raid_recover_device_status(OBJ_HANDLE obj_handle, gpointer user_data);
LOCAL guint8 check_raid_ctrl_hardware_fault_exist(OBJ_HANDLE obj_handle);
LOCAL void task_set_raid_fault_by_bios(void *data);

LOCAL ASSOCIATED_RESOURCE g_associated_resource_map[] = {
    {0xFF, "PCIeSwitch"},
    {0xFE, "PCH"},
    {0xFD, "CPU1,CPU2"},
    {0xFC, "CPU1,CPU2,CPU3,CPU4"},
};


LOCAL void get_raid_card_info_from_firmware(OBJ_HANDLE obj_handle, gchar *logic_ver, size_t buff_size)
{
    OBJ_HANDLE refer_handle = 0;
    
    gint32 ret = dfl_get_referenced_object(obj_handle, PROPERTY_PCIE_CARD_FIRMWARE_OBJ, &refer_handle);
    if (ret == DFL_OK) {
        
        ret = dal_get_property_value_string(refer_handle, PROPERTY_SOFTWARE_VERSIONSTRING, logic_ver, buff_size);
    }

    if ((ret != RET_OK) || (strlen(logic_ver) == 0)) {
        (void)strcpy_s(logic_ver, buff_size, "N/A");
    }
}


LOCAL void get_raid_card_info_from_component(OBJ_HANDLE obj_handle, INFO_FROM_COMPONENT *comp_info)
{
    OBJ_HANDLE refer_handle = 0;
    
    gint32 ret = dfl_get_referenced_object(obj_handle, PROPERTY_PCIE_CARD_REF_COMPONENT, &refer_handle);
    if (ret != RET_OK) {
        (void)strcpy_s(comp_info->location, sizeof(comp_info->location), "N/A");
        (void)strcpy_s(comp_info->serial_num, sizeof(comp_info->serial_num), "N/A");
        return;
    }
    
    ret = dal_get_property_value_string(refer_handle, PROPERTY_COMPONENT_LOCATION, comp_info->location,
        sizeof(comp_info->location));
    if ((ret != RET_OK) || (strlen(comp_info->location) == 0)) {
        (void)strcpy_s(comp_info->location, sizeof(comp_info->location), "N/A");
    }
    
    ret = dal_get_card_sn(refer_handle, comp_info->serial_num, sizeof(comp_info->serial_num));
    if ((ret != RET_OK) || (strlen(comp_info->serial_num) == 0)) {
        (void)strcpy_s(comp_info->serial_num, sizeof(comp_info->serial_num), "N/A");
    }
    return;
}
 

LOCAL void get_raid_card_associated_resource(guint8 res_id, gchar* res_str, size_t buff_size)
{
    if (res_id == 0) {
        (void)strcpy_s(res_str, buff_size, "N/A");
        return;
    }
 
    gint32 i;
    for (i = 0; i < G_N_ELEMENTS(g_associated_resource_map); i++) {
        if (res_id == g_associated_resource_map[i].res_id) {
            (void)strncpy_s(res_str, buff_size, g_associated_resource_map[i].res_str,
                strlen(g_associated_resource_map[i].res_str));
            return;
        }
    }
 
    gint32 ret = snprintf_s(res_str, buff_size, buff_size - 1, "CPU%u", res_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret:%d", __FUNCTION__, ret);
    }
    return;
}
 

LOCAL gint32 get_each_raid_card_dump_info(OBJ_HANDLE obj_handle, gchar *raid_card_info, size_t buff_len)
{
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    gchar logic_ver[FIRMWARE_VERSION_STRING_LEN] = {0};
    gchar res_str[ACCESSOR_NAME_LEN] = {0};
    INFO_FROM_COMPONENT comp_info = {0};
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAIDCARD_SLOT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAIDCARD_BOARDID);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAIDCARD_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAIDCARD_MANUFACTURER);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAIDCARD_PCBVER);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAIDCARD_TYPE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAIDCARD_MODEL);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAIDCARD_PART_NUMBER);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAIDCARD_CPUID);
 
    gint32 ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value);
    g_slist_free(property_name_list);
    property_name_list = NULL;
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get raid card information failed!");
        g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
        property_value = NULL;
        return RET_ERR;
    }
    guint8 slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    guint16 board_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 1));
    const gchar *name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 2), NULL);
    const gchar *manufacturer = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 3), NULL);
    const gchar *pcb_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 4), NULL);
    const gchar *type = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 5), NULL);
    const gchar *model = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 6), NULL);
    const gchar *partnum = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 7), NULL);
    guint8 res_id = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 8));
    get_raid_card_associated_resource(res_id, res_str, sizeof(res_str));
    get_raid_card_info_from_firmware(obj_handle, logic_ver, sizeof(logic_ver));
    get_raid_card_info_from_component(obj_handle, &comp_info);
 
    ret = snprintf_s(raid_card_info, buff_len, buff_len - 1,
        "%-4u | 0x%-8x | %-16s | %-16s | %-32s | %-10s | %-10s | %-25s | %-20s | %-10s | %-20s | %s\n",
        slot_id, board_id, comp_info.location, name, manufacturer, pcb_ver, logic_ver, type, model, partnum,
        comp_info.serial_num, res_str);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
    property_value = NULL;
    return RET_OK;
}

gint32 raid_card_dump_info(const gchar *path)
{
    gint32 iRet = -1;
    gint32 ret = 0;
    gchar file_name[RAID_CARD_DUMPINFO_MAX_LEN + 1] = {0};
    gchar raid_card_info[RAID_CARD_DUMPINFO_MAX_LEN + 1] = {0};
    FILE *fp = NULL;

    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    size_t fwrite_back = 0;

    if (path == NULL) {
        return RET_ERR;
    }

    ret = dfl_get_object_list(CLASS_RAIDCARD_NAME, &obj_list);
    if (ret != DFL_OK) {
        if (ERRCODE_OBJECT_NOT_EXIST == ret) {
            return RET_OK;
        }
        return RET_ERR;
    }
    
    // 在dump_dir目录下创建文件
    iRet = snprintf_s(file_name, RAID_CARD_DUMPINFO_MAX_LEN + 1, RAID_CARD_DUMPINFO_MAX_LEN, "%s/card_info", path);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        g_slist_free(obj_list);
        return RET_ERR;
    }

    
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);
    
    (void)snprintf_s(raid_card_info, RAID_CARD_DUMPINFO_MAX_LEN + 1, RAID_CARD_DUMPINFO_MAX_LEN, "%s",
        "RAID Card Info\n");
    fwrite_back = fwrite(raid_card_info, strlen(raid_card_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    (void)memset_s(raid_card_info, RAID_CARD_DUMPINFO_MAX_LEN + 1, 0, RAID_CARD_DUMPINFO_MAX_LEN + 1);
    (void)snprintf_s(raid_card_info, RAID_CARD_DUMPINFO_MAX_LEN + 1, RAID_CARD_DUMPINFO_MAX_LEN,
        "%-4s | %-10s | %-16s | %-16s | %-32s | %-10s | %-10s | %-25s | %-20s | %-10s | %-20s | %s\n",
        "Slot", "BoardId", "Location", "ProductName", "Manufacturer", "PCB Ver", "Logic Ver", "Type", "Mode",
        "Part Num", "Serial Num", "Connected To");
    fwrite_back = fwrite(raid_card_info, strlen(raid_card_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)memset_s(raid_card_info, RAID_CARD_DUMPINFO_MAX_LEN + 1, 0, RAID_CARD_DUMPINFO_MAX_LEN + 1);
        ret = get_each_raid_card_dump_info((OBJ_HANDLE)obj_node->data, raid_card_info, RAID_CARD_DUMPINFO_MAX_LEN + 1);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "get_each_raid_card_dump_info failed!");
            continue;
        }

        fwrite_back = fwrite(raid_card_info, strlen(raid_card_info), 1, fp);
        if (fwrite_back != 1) {
            debug_log(DLOG_ERROR, "fwrite failed!");
            continue;
        }
    }
    g_slist_free(obj_list);
    (void)memset_s(raid_card_info, RAID_CARD_DUMPINFO_MAX_LEN + 1, 0, RAID_CARD_DUMPINFO_MAX_LEN + 1);
    (void)snprintf_s(raid_card_info, RAID_CARD_DUMPINFO_MAX_LEN + 1, RAID_CARD_DUMPINFO_MAX_LEN, "%s", "\n\n");
    
    fwrite_back = fwrite(raid_card_info, strlen(raid_card_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }
    (void)memset_s(raid_card_info, RAID_CARD_DUMPINFO_MAX_LEN + 1, 0, RAID_CARD_DUMPINFO_MAX_LEN + 1);
    // 关闭文件
    (void)fclose(fp);

    return RET_OK;
}

LOCAL void raid_card_update_resId_info(OBJ_HANDLE obj_handle)
{
    
    // 更新当前RAIDCard属于哪个CPU
    guint8 cpu_id = 0;
    gint32 ret = 0;
    
    ret = get_card_cpu_id(obj_handle, &cpu_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Get %s ResId failed! ret:%d.", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
        return;
    }
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_RAIDCARD_CPUID, cpu_id, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Update %s ResId(%d) failed! ret:%d.", __FUNCTION__, dfl_get_object_name(obj_handle),
            cpu_id, ret);
        return;
    }
    
    
}

LOCAL gint32 process_each_raidcard(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret = 0;
    
    raid_card_update_resId_info(obj_handle);
    

    
    ret = get_pcb_version(obj_handle, PROPERTY_RAIDCARD_PCBID, PROPERTY_RAIDCARD_PCBVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 raid_card_get_status(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret = 0;
    guint8 fault = 0;
    guint8 status = 0;
    GVariant *property_value = NULL;

    ret = dfl_get_extern_value(obj_handle, PROPERTY_RAIDCARD_FAULT, &property_value, DF_COPY);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get %s fault failed!", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    fault = g_variant_get_byte(property_value);
    g_variant_unref(property_value);
    property_value = NULL;

    if (!(fault & 0x1) || !(fault & 0x2)) {
        status = RAID_FAULT; // raid卡故障
    } else {
        status = RAID_OK; // raid卡工作正常
    }

    property_value = g_variant_new_byte(status);
    ret = dfl_set_property_value(obj_handle, PROPERTY_RAIDCARD_STATUS, property_value, DF_NONE);
    g_variant_unref(property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set %s status failed!", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    return RET_OK;
}


static void raid_card_process_task(void)
{
    (void)prctl(PR_SET_NAME, (gulong) "RaidCardTask");

    for (;;) {
        vos_task_delay(RAID_CARD_STATE_UPDATE_TIME);

        
        (void)dfl_foreach_object(CLASS_RAIDCARD_NAME, raid_card_get_status, NULL, NULL);
    }
}


gint32 raid_card_add_object_callback(OBJ_HANDLE object_handle)
{
    return process_each_raidcard(object_handle, NULL);
}


gint32 raid_card_del_object_callback(OBJ_HANDLE object_handle)
{
    
    guint8 default_value = 0;

    (void)raid_recover_device_status(object_handle, &default_value);
    
    return RET_OK;
}


gint32 raid_card_init(void)
{
    (void)dfl_foreach_object(CLASS_RAIDCARD_NAME, process_each_raidcard, NULL, NULL);

    return RET_OK;
}


gint32 raid_card_start(void)
{
    gulong ret = 0;
    gulong task_update_raid_state = 0;

    
    ret = vos_task_create(&task_update_raid_state, "task_update_raid_state", (TASK_ENTRY)raid_card_process_task, 0,
        DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Creat update raid card state task failed!");
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 raid_recover_device_status(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret = 0;
    guint8 default_value = *(guint8 *)user_data;
    GVariant *property_data = NULL;

    property_data = g_variant_new_byte(default_value);

    ret = dfl_set_property_value(obj_handle, PROPERTY_FDM_FAULT_STATE, property_data, DF_SAVE_TEMPORARY);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set RAID FDM fault state default value failed: %d.", ret);
    }

    g_variant_unref(property_data);

    (void)dal_set_property_value_byte(obj_handle, PROPERTY_RAIDCARD_FAULT_BY_BIOS, 0x00, DF_SAVE_TEMPORARY);

    
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_PFA_EVENT, default_value, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set RAID PFAEvent default value failed: %d.", ret);
    }
    

    return RET_OK;
}


void raid_set_device_status_default_value(void)
{
    gint32 ret = 0;
    guint8 default_state = 0;

    ret = dfl_foreach_object(CLASS_RAIDCARD_NAME, raid_recover_device_status, &default_state, NULL);
    if (ret != DFL_OK) {
        if (ERRCODE_OBJECT_NOT_EXIST != ret) {
            debug_log(DLOG_ERROR, "Recover raid default value failed!");
        }
    }

    return;
}


gint32 set_raid_fault_status_by_bios(guint8 bus, guint8 device, guint8 function, guint8 device_status)
{
    gint32 ret = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_node_hanlde = 0;
    GSList *obj_list_1 = NULL;
    GSList *obj_node_1 = NULL;
    OBJ_HANDLE obj_node_hanlde_1 = 0;
    OBJ_HANDLE component_hanlde = 0;
    guchar bus_tmp = 0;
    guchar device_tmp = 0;
    guchar function_tmp = 0;
    guchar group_id_tmp = 0;
    guchar component_type_tmp = 0;
    guchar slot_id_tmp = 0;
    guchar component_group_id_tmp = 0;
    guchar component_device_type_tmp = 0;
    guchar component_device_num_tmp = 0;
    gchar *property_ref_name = NULL;
    gchar ref_object_name[MAX_NAME_SIZE] = {0};
    gchar ref_property_name[MAX_NAME_SIZE] = {0};
    OBJ_HANDLE obj_handle = 0;
    gchar devicename_tmp[MAX_NAME_SIZE] = {0};

    ret = dfl_get_object_list(CLASS_PCIE_ADDR_INFO, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        debug_log(DLOG_ERROR, "Get PcieAddrInfo object list failed!");
        return RET_ERR;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        obj_node_hanlde = (OBJ_HANDLE)obj_node->data;
        (void)dal_get_property_value_byte(obj_node_hanlde, PROPERTY_PCIE_BUS, &bus_tmp);
        (void)dal_get_property_value_byte(obj_node_hanlde, PROPERTY_PCIE_DEVICE, &device_tmp);
        (void)dal_get_property_value_byte(obj_node_hanlde, PROPERTY_PCIE_FUNCTION, &function_tmp);

        if ((bus == bus_tmp) && (device == device_tmp) && (function == function_tmp)) {
            (void)dal_get_property_value_byte(obj_node_hanlde, PROPERTY_GROUP_ID, &group_id_tmp);
            (void)dal_get_property_value_byte(obj_node_hanlde, PROPERTY_COMPONENT_TYPE, &component_type_tmp);
            (void)dal_get_property_value_byte(obj_node_hanlde, PROPERTY_PCIE_SLOT_ID, &slot_id_tmp);

            ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list_1);
            if (ret != DFL_OK || g_slist_length(obj_list_1) == 0) {
                g_slist_free(obj_list);
                debug_log(DLOG_ERROR, "Get Component object list failed!");
                return RET_ERR;
            }

            for (obj_node_1 = obj_list_1; obj_node_1; obj_node_1 = obj_node_1->next) {
                obj_node_hanlde_1 = (OBJ_HANDLE)obj_node_1->data;

                (void)dal_get_property_value_byte(obj_node_hanlde_1, PROPERTY_COMPONENT_GROUPID,
                    &component_group_id_tmp);
                (void)dal_get_property_value_byte(obj_node_hanlde_1, PROPERTY_COMPONENT_DEVICE_TYPE,
                    &component_device_type_tmp);
                (void)dal_get_property_value_byte(obj_node_hanlde_1, PROPERTY_COMPONENT_DEVICENUM,
                    &component_device_num_tmp);

                if ((group_id_tmp == component_group_id_tmp) && (component_type_tmp == component_device_type_tmp) &&
                    (slot_id_tmp == component_device_num_tmp)) {
                    component_hanlde = obj_node_hanlde_1;
                    property_ref_name = PROPERTY_COMPONENT_DEVICENUM;

                    
                    ret = dfl_get_referenced_property(component_hanlde, property_ref_name, ref_object_name,
                        ref_property_name, MAX_NAME_SIZE, MAX_NAME_SIZE);
                    if (ret != DFL_OK) {
                        debug_log(DLOG_ERROR, "Get object name failed, component object=%s, result is %d",
                            dfl_get_object_name(component_hanlde), ret);
                        continue;
                    }

                    debug_log(DLOG_DEBUG, "Get object name: %s.", ref_object_name);

                    
                    ret = dfl_get_object_handle(ref_object_name, &obj_handle);
                    if (ret != DFL_OK) {
                        debug_log(DLOG_ERROR, "Get object failed, result is %d", ret);
                        g_slist_free(obj_list);
                        g_slist_free(obj_list_1);
                        return ret;
                    }

                    if (DEVICE_TYPE_RAID_CARD == component_type_tmp) {
                        
                        if (!check_raid_ctrl_hardware_fault_exist(obj_handle)) {
                            ret = dal_set_property_value_byte(obj_handle, PROPERTY_RAIDCARD_FAULT_BY_BIOS,
                                device_status, DF_SAVE_TEMPORARY);
                            if (ret != RET_OK) {
                                debug_log(DLOG_ERROR, "Set %s object property %s to %d failed, result is %d",
                                    ref_object_name, PROPERTY_RAIDCARD_FAULT_BY_BIOS, device_status, ret);
                                g_slist_free(obj_list);
                                g_slist_free(obj_list_1);
                                return ret;
                            }
                        }
                        

                        
                        if (device_status) {
                            (void)memset_s(devicename_tmp, sizeof(devicename_tmp), 0, sizeof(devicename_tmp));
                            (void)dal_get_property_value_string(component_hanlde, PROPERTY_COMPONENT_DEVICE_NAME,
                                devicename_tmp, sizeof(devicename_tmp));
                            maintenance_log_v2(MLOG_ERROR, FC_STORAGE_RAID_INIT_ERROR,
                                "%s has initialization fault. Error status : 0x%02x.", devicename_tmp, device_status);
                        }
                        

                        g_slist_free(obj_list);
                        g_slist_free(obj_list_1);
                        return RET_OK;
                    } else if (DEVICE_TYPE_PCIE_CARD == component_type_tmp) {
                        
                        if (!check_raid_ctrl_hardware_fault_exist(obj_handle)) {
                            ret = pcie_card_set_obj_property(obj_handle, PROPERTY_PCIECARD_FALT_BY_BIOS, &device_status,
                                PCIE_CARD_P_TYPE_BYTE, DF_SAVE_TEMPORARY);
                            if (ret != RET_OK) {
                                debug_log(DLOG_ERROR, "Set %s object property %s to %d failed, result is %d",
                                    ref_object_name, PROPERTY_PCIECARD_FALT_BY_BIOS, device_status, ret);
                                g_slist_free(obj_list);
                                g_slist_free(obj_list_1);
                                return ret;
                            }
                        }

                        if (device_status) {
                            (void)memset_s(devicename_tmp, sizeof(devicename_tmp), 0, sizeof(devicename_tmp));
                            (void)dal_get_property_value_string(component_hanlde, PROPERTY_COMPONENT_DEVICE_NAME,
                                devicename_tmp, sizeof(devicename_tmp));
                            maintenance_log_v2(MLOG_ERROR, FC_STORAGE_RAID_INIT_ERROR,
                                "%s has initialization fault. Error status : 0x%02x.", devicename_tmp, device_status);
                        }
                        

                        g_slist_free(obj_list);
                        g_slist_free(obj_list_1);
                        return RET_OK;
                    } else {
                        debug_log(DLOG_ERROR, "Invalid card type");
                        g_slist_free(obj_list);
                        g_slist_free(obj_list_1);
                        return RET_ERR;
                    }
                }
            }
            g_slist_free(obj_list_1);
            obj_list_1 = NULL;
        }
    }
    debug_log(DLOG_ERROR, "%s: Not found card type, Bus Num(%d), Device Num(%d), Function Num(%d) ", __FUNCTION__,
        bus, device, function);
    g_slist_free(obj_list);

    return RET_ERR;
}


LOCAL guint8 check_raid_ctrl_hardware_fault_exist(OBJ_HANDLE obj_handle)
{
    guint8 hardware_fault = 0;
    guint16 health_code = 0;
    OBJ_HANDLE ctrl_obj = 0;

    
    (void)dal_get_specific_object_position(obj_handle, CLASS_RAID_CONTROLLER_NAME, &ctrl_obj);
    if (ctrl_obj != 0) {
        (void)dal_get_property_value_byte(ctrl_obj, PROPERTY_RAID_CONTROLLER_HARDWARE_FAULT, &hardware_fault);
        (void)dal_get_property_value_uint16(ctrl_obj, PROPERTY_RAID_CONTROLLER_HEALTH_STATUS_CODE, &health_code);
    }

    
    if ((hardware_fault != 0) || ((health_code & 0x000E) != 0 && health_code != STORAGE_INFO_INVALID_WORD)) {
        return TRUE;
    }

    return FALSE;
}


gint32 ipmi_set_raid_fault_status(const void *msg_data, gpointer user_data)
{
    gint32 ret = 0;
    const guint8 *req_data = NULL;
    guint8 resp_data[255] = {0};
    guint32 manufactureid = 0;
    guint32 imana = 0;
    guint8 event_dir = 0;
    guint8 device_status1 = 0;
    OBJ_HANDLE obj_handle = 0;
    guint8 bus = 0;
    guint8 device = 0;
    guint8 function = 0;
    guint8 error_code = 0;
    OBJ_HANDLE partition_obj = 0;
    guint32 product_id = 0;
    TASKID ulTaskID = 0;
    TEMP_BIOS_ERRCODE *temp_raid_fault = NULL;

    
    if (msg_data == NULL) {
        debug_log(DLOG_ERROR, "%s: msg_data is NULL", __FUNCTION__);
        return RET_ERR;
    }

    
    req_data = get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "%s: get ipmi src data fail", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    
    ret = dfl_get_object_handle(BMC_OBJECT_NAME, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "ret = %d", ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_CANNOT_RESPONSE);
    }

    (void)dal_get_property_value_uint32(obj_handle, BMC_MANU_ID_NAME, &manufactureid);

    
    imana = MAKE_DWORD(0, req_data[2], req_data[1], req_data[0]);
    if (manufactureid != imana) {
        debug_log(DLOG_ERROR, "%s: invalid manufactureid(%d)", __FUNCTION__, imana);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    event_dir = !req_data[5]; // 兼容以1作为触发告警的方案
    device_status1 = req_data[6];
    bus = req_data[7];         
    device = req_data[8];      
    function = req_data[9];    
    error_code = req_data[10]; 

    debug_log(DLOG_ERROR, "RAID fault(%d) %s, Bus Num(%d), Device Num(%d), Function Num(%d), Error Code(%d)",
        device_status1, (event_dir == 1) ? "Asserted" : "Deasserted", bus, device, function, error_code);

    
    
    if (dal_get_object_handle_nth(CLASS_COMPUTER_PARTITION, 0, &partition_obj) == RET_OK) {
        ret = dal_get_property_value_uint32(partition_obj, PROPERTY_COMPUTER_PARTITION_RHPRODUCTID, &product_id);
        if ((ret == RET_OK) && (COMPUTER_ID_9032_V1 == product_id)) {
            dal_ipmi_log_with_bios_set_check(msg_data, MLOG_INFO, FC__PUBLIC_OK, "Set raid status failed");
            return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_CMD);
        }
    }
    
    switch (device_status1) {
        case 0:
            ret = set_raid_fault_status_by_bios(bus, device, function, error_code);
            
            if (ret != RET_OK) {
                
                temp_raid_fault = (TEMP_BIOS_ERRCODE *)g_malloc0(sizeof(TEMP_BIOS_ERRCODE));
                if (temp_raid_fault == NULL) {
                    debug_log(DLOG_ERROR, "%s : g_malloc0 temp_raid_fault fail !", __FUNCTION__);
                    dal_ipmi_log_with_bios_set_check(msg_data, MLOG_INFO, FC__PUBLIC_OK, "Set raid status failed");
                    return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
                }

                temp_raid_fault->bus = bus;
                temp_raid_fault->device = device;
                temp_raid_fault->function = function;
                temp_raid_fault->error_code = error_code;

                ret = vos_task_create(&ulTaskID, "task_set_raid_fault_by_bios", (TASK_ENTRY)task_set_raid_fault_by_bios,
                    (void *)temp_raid_fault, DEFAULT_PRIORITY);
                if (ret != RET_OK) {
                    debug_log(DLOG_ERROR, "%s : create task_set_raid_fault_by_bios fail !", __FUNCTION__);
                    g_free(temp_raid_fault);
                    dal_ipmi_log_with_bios_set_check(msg_data, MLOG_INFO, FC__PUBLIC_OK, "Set raid status failed");
                    return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
                }
            }
            

            break;

        default:
            debug_log(DLOG_ERROR, "%s: invalid device_status(%d)", __FUNCTION__, device_status1);
            return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }
    resp_data[0] = COMP_CODE_SUCCESS;
    resp_data[1] = LONGB0(manufactureid);
    resp_data[2] = LONGB1(manufactureid);
    resp_data[3] = LONGB2(manufactureid);
    dal_ipmi_log_with_bios_set_check(msg_data, MLOG_INFO, FC__PUBLIC_OK, "Set raid status successfully");
    return ipmi_send_response(msg_data, 4, resp_data);
}


LOCAL void task_set_raid_fault_by_bios(void *data)
{
#define MAX_RETRY_TIMES 10

    gint32 ret = 0;
    TEMP_BIOS_ERRCODE *raid_fault = (TEMP_BIOS_ERRCODE *)data;

    gint32 count = MAX_RETRY_TIMES;
    while (count) {
        vos_task_delay(2000);

        ret = set_raid_fault_status_by_bios(raid_fault->bus, raid_fault->device, raid_fault->function,
            raid_fault->error_code);
        if (ret == RET_OK) {
            debug_log(DLOG_ERROR, "%s: set raid fault task successful.", __FUNCTION__);
            g_free(raid_fault);
            return;
        }
        count--;
    }
    debug_log(DLOG_ERROR, "%s: have tried ten times, set raid fault task fail.", __FUNCTION__);
    g_free(raid_fault);
    return;
}
