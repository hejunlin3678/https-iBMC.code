


#include "pme_app/pme_app.h"
#include <arpa/inet.h>
#include "cpu_board.h"
#include "get_version.h"
#include "card_ipmi_msg.h"

gint32 cpu_board_dump_info(const gchar *path)
{
    gint32 iRet = -1;
    gint32 ret = 0;
    guint16 board_id = 0;
    const gchar *name = NULL;
    const gchar *pcb_ver = NULL;
    const gchar *logic_ver = NULL;
    const gchar *manufacturer = NULL;
    gchar file_name[CPU_BOARD_DUMPINFO_MAX_LEN + 1] = {0};
    gchar cpu_board_info[CPU_BOARD_DUMPINFO_MAX_LEN + 1] = {0};
    FILE *fp = NULL;

    guint8 slot_id = 0;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    size_t fwrite_back = 0;

    if (path == NULL) {
        return RET_ERR;
    }

    ret = dfl_get_object_list(CLASS_CPUBOARD_NAME, &obj_list);
    if (ret != DFL_OK) {
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            return RET_OK;
        }
        return RET_ERR;
    }
    
    // 在dump_dir目录下创建文件
    iRet = snprintf_s(file_name, CPU_BOARD_DUMPINFO_MAX_LEN + 1, CPU_BOARD_DUMPINFO_MAX_LEN, "%s/card_info", path);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        g_slist_free(obj_list);
        return RET_ERR;
    }
    
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);
    

    (void)snprintf_s(cpu_board_info, CPU_BOARD_DUMPINFO_MAX_LEN + 1, CPU_BOARD_DUMPINFO_MAX_LEN, "%s",
        "CPU board Info\n");
    fwrite_back = fwrite(cpu_board_info, strlen(cpu_board_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    (void)memset_s(cpu_board_info, CPU_BOARD_DUMPINFO_MAX_LEN + 1, 0, CPU_BOARD_DUMPINFO_MAX_LEN + 1);
    iRet = snprintf_s(cpu_board_info, CPU_BOARD_DUMPINFO_MAX_LEN + 1, CPU_BOARD_DUMPINFO_MAX_LEN,
        "%-4s | %-10s | %-10s | %-32s | %-10s | %s\n", "Slot", "BoardId", "Name", "Manufacturer", "PCB Ver",
        "Logic Ver");
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }

    fwrite_back = fwrite(cpu_board_info, strlen(cpu_board_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }
    
    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        property_name_list = g_slist_append(property_name_list, PROPERTY_CPUBOARD_SLOT);
        property_name_list = g_slist_append(property_name_list, PROPERTY_CPUBOARD_BOARDID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_CPUBOARD_NAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_CPUBOARD_MANUFACTURER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_CPUBOARD_PCBVER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_CPUBOARD_LOGICVER);

        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value);
        g_slist_free(property_name_list);
        property_name_list = NULL;
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get cpu board information failed!");
            g_slist_free(obj_list);
            (void)fclose(fp);
            return RET_ERR;
        }
        slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
        board_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 1));
        name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 2), 0);
        manufacturer = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 3), 0);
        pcb_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 4), 0);
        logic_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 5), 0);

        (void)memset_s(cpu_board_info, CPU_BOARD_DUMPINFO_MAX_LEN + 1, 0, CPU_BOARD_DUMPINFO_MAX_LEN + 1);
        
        iRet = snprintf_s(cpu_board_info, sizeof(cpu_board_info), sizeof(cpu_board_info) - 1,
            "%-4u | 0x%-4x     | %-10s | %-32s | %-10s | %s\n", slot_id, board_id, name, manufacturer, pcb_ver,
            logic_ver);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        }
        
        fwrite_back = fwrite(cpu_board_info, strlen(cpu_board_info), 1, fp);
        if (fwrite_back != 1) {
            (void)fclose(fp);
            g_slist_free(obj_list);
            g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_ERROR, "fwrite failed!");
            return RET_ERR;
        }

        (void)memset_s(cpu_board_info, CPU_BOARD_DUMPINFO_MAX_LEN + 1, 0, CPU_BOARD_DUMPINFO_MAX_LEN + 1);
        g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
        property_value = NULL;
    }

    g_slist_free(obj_list);
    
    (void)snprintf_s(cpu_board_info, CPU_BOARD_DUMPINFO_MAX_LEN + 1, CPU_BOARD_DUMPINFO_MAX_LEN, "%s", "\n\n");
    
    fwrite_back = fwrite(cpu_board_info, strlen(cpu_board_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    (void)memset_s(cpu_board_info, CPU_BOARD_DUMPINFO_MAX_LEN + 1, 0, CPU_BOARD_DUMPINFO_MAX_LEN + 1);
    // 关闭文件
    (void)fclose(fp);

    return RET_OK;
}


LOCAL gint32 process_each_cpu_board(OBJ_HANDLE handle, gpointer data)
{
    gint32 ret = 0;

    
    ret = get_pcb_version(handle, PROPERTY_CPUBOARD_PCBID, PROPERTY_CPUBOARD_PCBVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    
    ret = get_logic_version(handle, PROPERTY_CPUBOARD_LOGICVERID, PROPERTY_CPUBOARD_LOGICVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    return RET_OK;
}


gint32 cpu_board_add_object_callback(OBJ_HANDLE object_handle)
{
    card_manage_log_operation_log("Cpu board", object_handle, PROPERTY_CPUBOARD_SLOT, NULL, CARD_PLUG_IN);
    return process_each_cpu_board(object_handle, NULL);
}


gint32 cpu_board_del_object_callback(OBJ_HANDLE object_handle)
{
    card_manage_log_operation_log("Cpu board", object_handle, PROPERTY_CPUBOARD_SLOT, NULL, CARD_PLUG_OUT);
    return RET_OK;
}



#define MAX_IP_LEN 16
LOCAL TASKID g_status_cpu_hotplug_task = 0x00;
LOCAL gboolean g_status_cpu_hotplug_task_exit = FALSE;


#define CPU_ID(cpu_id_1) (((cpu_id_1) % 4 == 0) ? 4 : ((cpu_id_1) % 4))

static guint8 s_cpubrd_slot_id = 0; // Ultra-BMC记录CPU 热插拔的ID
guint32 ras_mission_status = 0;     // 记录热插拔状态，和IPMI命令相对应

// CPU 热插拔对应写CPLD值
guint8 cpu_1_4_ras_event[CPUBRD_HOT_RAS_RESERVE]
        ={0x80, 0xa3, 0xa9, 0xff, 0xff, 0x27, 0x21, 0x25, 0x00};

guint8 cpu_2_3_ras_event[CPUBRD_HOT_RAS_RESERVE]
        ={0x40, 0x54, 0x5a, 0xff, 0xff, 0x18, 0x12, 0x16, 0x00};

LOCAL void *cpubrd_monitor_task(CPU_RAS_EVENT_S *p);


gint32 cpubrd_ras_init_info(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)

{
    ras_mission_status = 0;
    return RET_OK;
}


LOCAL gint32 write_cpld_reg_data(guint8 cpubrd_slot_id, const gchar *property_name, guint8 value)
{
    gint32 ret = RET_OK;
    OBJ_HANDLE object_handle = 0;

    
    ret = dal_get_specific_object_byte(CLASS_CPUBOARD_NAME, PROPERTY_CPUBOARD_SLOT, cpubrd_slot_id, &object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get object_handle fail, result is %d", ret);
        return ret;
    }

    
    ret = dal_set_property_value_byte(object_handle, property_name, value, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get %s fail, result is %d", property_name, ret);
        return ret;
    }

    return ret;
}




LOCAL gint32 read_cpld_reg_data(guint8 cpubrd_slot_id, const gchar *property_name, guint8 *value)
{
    gint32 ret = RET_OK;
    OBJ_HANDLE object_handle = 0;

    if (value == NULL) {
        debug_log(DLOG_ERROR, "input value error!");
        return RET_ERR;
    }

    
    ret = dal_get_specific_object_byte(CLASS_CPUBOARD_NAME, PROPERTY_CPUBOARD_SLOT, cpubrd_slot_id, &object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get object_handle fail, result is %d", ret);
        return ret;
    }

    
    ret = dal_get_property_value_byte(object_handle, property_name, value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get %s fail, result is %d", property_name, ret);
        return ret;
    }

    return ret;
}


LOCAL gint32 write_check_cpld_data(guint8 cpubrd_slot_id, const gchar *property_name, guint8 value)
{
    guint8 count = 0;
    guint8 cpldvalue = 0;
    gint32 write_ret = RET_OK;
    gint32 read_ret = RET_OK;

    if (property_name == NULL) {
        debug_log(DLOG_ERROR, "property_name is null ");
        return RET_ERR;
    }

    for (count = 0; count < 3; count++) {
        write_ret = write_cpld_reg_data(cpubrd_slot_id, property_name, value);
        read_ret = read_cpld_reg_data(cpubrd_slot_id, property_name, &cpldvalue);
        if ((write_ret == RET_OK) && (read_ret == RET_OK)) {
            if (value == cpldvalue) {
                return RET_OK;
            } else {
                debug_log(DLOG_ERROR, "write reg %s cpld error, write value is:0x%x, read value:0x%x ", property_name,
                    value, cpldvalue);
            }
        } else {
            debug_log(DLOG_ERROR, "write or read cpld failded!");
        }
        cpldvalue = 0;
    }
    return RET_ERR;
}



LOCAL gint32 get_ultra_bmc_ip(gchar *bmc_ip, guint32 bmc_ip_len)
{
    errno_t safe_fun_ret = EOK;
    guint32 local_chassis_num = 0;
    gint32 result = RET_OK;
    GVariant *property_value = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint32 ip = 0x82011fac; 
    guint8 *p = (guint8 *)(&ip);
    struct in_addr ip_in_addr = { 0 };
    _cleanup_gfree_ char *temp_ip = NULL;
    guint8 bmc_role = 0;

    
    result = dfl_get_object_handle(OBJECT_NAME_CHASSIS, &obj_handle);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_object_handle fail!result = %d", result);
        return RET_ERR;
    }

    
    result = dfl_get_extern_value(obj_handle, PROPERTY_CHASSIS_CHASSIS_NUM, &property_value, DF_AUTO);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_extern_value %s fail! result = %d", PROPERTY_CHASSIS_CHASSIS_NUM, result);
    }

    local_chassis_num = g_variant_get_uint32(property_value);
    g_variant_unref(property_value);
    property_value = NULL;

    
    result = dfl_get_object_handle(OBJECT_PHYSICAL_PARTITION, &obj_handle);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_object_handle fail!result = %d", result);
    }

    
    result = dfl_get_extern_value(obj_handle, PROPERTY_COMPUTER_PARTITION_ID, &property_value, DF_HW);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_property_data %s fail!", PROPERTY_COMPUTER_PARTITION_ID);
    }

    bmc_role = g_variant_get_byte(property_value);
    g_variant_unref(property_value);

    if (bmc_role % 2) {
        p[3] = 130 + local_chassis_num * 2 - bmc_role - 1;
    } else {
        p[3] = 130 + local_chassis_num * 2 - bmc_role;
    }
    debug_log(DLOG_DEBUG, "get_ultra_bmc_ip, ip = %d", p[3]);

    if (memcpy_s(&ip_in_addr, sizeof(ip_in_addr) != EOK, &ip, sizeof(guint32))) {
        debug_log(DLOG_ERROR, "memcpy_s fail, slot = %d", p[3]);
        return RET_ERR;
    }
    temp_ip = dal_inet_ntop_ipv4(ip_in_addr);
    
    safe_fun_ret = strncpy_s(bmc_ip, bmc_ip_len, temp_ip, bmc_ip_len - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    

    return RET_OK;
}


LOCAL gint32 get_intra_bmc_ip(guint8 cpubrd_slot_id, gchar *bmc_ip, guint32 bmc_ip_len)
{
    const gint32 ip_addr_len = 4;
    errno_t safe_fun_ret = EOK;
    guint32 local_chassis_num = 0;
    gint32 result = RET_OK;
    GVariant *property_value = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint32 ip = 0x82011fac; 
    guint8 *p = (guint8 *)(&ip);
    guint8 utlta_ip = 0;
    guint8 intra_bmc_id = 0;
    struct in_addr ip_in_addr = {};
    _cleanup_gfree_ char *temp_ip = NULL;

    
    result = dfl_get_object_handle(OBJECT_NAME_CHASSIS, &obj_handle);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_object_handle fail!result = %d", result);
        return RET_ERR;
    }

    
    result = dfl_get_extern_value(obj_handle, PROPERTY_CHASSIS_CHASSIS_NUM, &property_value, DF_AUTO);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_extern_value %s fail! result = %d", PROPERTY_CHASSIS_CHASSIS_NUM, result);
    }

    local_chassis_num = g_variant_get_uint32(property_value);
    g_variant_unref(property_value);

    utlta_ip = 130 + (local_chassis_num - 1) * 2;

    intra_bmc_id = (cpubrd_slot_id - 1) / 4;

    if (intra_bmc_id % 2) {
        p[3] = utlta_ip + intra_bmc_id - 2;
    } else {
        p[3] = utlta_ip + intra_bmc_id;
    }
    debug_log(DLOG_DEBUG, "get_intra_bmc_ip, ip = %d", p[3]);

    if (memcpy_s(&ip_in_addr, ip_addr_len, &ip, ip_addr_len) != EOK) {
        debug_log(DLOG_ERROR, "memcpy_s fail, slot = %d", p[3]);
        return RET_ERR;
    }
    temp_ip = dal_inet_ntop_ipv4(ip_in_addr);
    
    safe_fun_ret = strncpy_s(bmc_ip, bmc_ip_len, temp_ip, bmc_ip_len - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    
    return RET_OK;
}


LOCAL gint32 set_cpu_ras_finish_event(guint8 cpubrd_slot_id, guint8 event_type)
{
    gint32 ret = RET_OK;
    GSList *input_list = NULL;
    gint32 port = 51000;
    gchar rpc_server_ip[MAX_IP_LEN];
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_data = NULL;

    if (cpubrd_slot_id == 0) {
        ret = get_ultra_bmc_ip(rpc_server_ip, MAX_IP_LEN);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s get ultra-bmc ip fail, result is %d", __FUNCTION__, ret);
            return ret;
        }

        
        property_data = g_variant_new_byte(1);
        ret = dfl_get_specific_object(CLASS_CPUBOARD_NAME, PROPERTY_CPUBOARD_SLOT, property_data, &obj_handle);
        g_variant_unref(property_data);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s get PROPERTY_CPUBOARD_SLOT fail, result is %d", __FUNCTION__, ret);
            return ret;
        }

        input_list = g_slist_append(input_list, g_variant_new_byte(0));
        input_list = g_slist_append(input_list, g_variant_new_byte(event_type));

        
        ret = dfl_call_remote_class_method2(rpc_server_ip, port, dfl_get_object_name(obj_handle), METHOD_SET_RASEVENT,
            NULL, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "sync cpu ras finish event fail ---[%s] with result:%d", __FUNCTION__, ret);
            return ret;
        }
    } else {
        
        ras_mission_status = ((ras_mission_status & 0xFFFFFF00) | 0x02);
    }
    return RET_OK;
}


LOCAL gint32 set_cpu_ras_start_event(guint8 cpubrd_slot_id, guint8 event_type)
{
    gint32 ret = RET_OK;
    CPU_RAS_EVENT_S *cpu_id_event = NULL;
    guint8 cpldvalue = 0;
    OBJ_HANDLE object_handle = 0;

    if ((cpubrd_slot_id < 1) || (cpubrd_slot_id > 4)) {
        debug_log(DLOG_ERROR, "[%s] cpubrd_slot_id %d is invalid", __FUNCTION__, cpubrd_slot_id);
        return RET_ERR;
    }

    if (event_type == CPUBRD_HOT_REMOVE_START) {
        
        ret = dal_get_specific_object_byte(CLASS_CPUBOARD_NAME, PROPERTY_CPUBOARD_SLOT, cpubrd_slot_id, &object_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s get object_handle fail, result is %d", __FUNCTION__, ret);
            return ret;
        }

        
        ret = dal_get_property_value_byte(object_handle, CPU_BRD_MIGRATION_W_SIGNAL, &cpldvalue);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s get CPU_BRD_MIGRATION_W_SIGNAL fail, result is %d", __FUNCTION__, ret);
            return ret;
        }
    }
    if (cpubrd_slot_id == 1 || cpubrd_slot_id == 4) {
        ret = write_check_cpld_data(cpubrd_slot_id, CPU_BRD_MIGRATION_W_SIGNAL,
            (cpu_1_4_ras_event[event_type] | cpldvalue));
    } else {
        ret = write_check_cpld_data(cpubrd_slot_id, CPU_BRD_MIGRATION_W_SIGNAL,
            (cpu_2_3_ras_event[event_type] | cpldvalue));
    }
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s write cpld fail, result is %d", __FUNCTION__, ret);
        return ret;
    }

    if ((event_type == CPUBRD_HOT_REMOVE_CPU_OFFLINE) || (event_type == CPUBRD_HOT_REMOVE_CPUBOARD_POWEROFF) ||
        (event_type == CPUBRD_HOT_PLUG_CPU_ONLINE) || (event_type == CPUBRD_HOT_PLUG_POWERON) ||
        (event_type == CPUBRD_HOT_PLUG_CPU_RESET)) {
        g_status_cpu_hotplug_task_exit = FALSE;
        
        cpu_id_event = (CPU_RAS_EVENT_S *)g_malloc0(sizeof(CPU_RAS_EVENT_S));
        if (cpu_id_event == NULL) {
            debug_log(DLOG_ERROR, "g_malloc0 for cpu_id_event fail");
            return RET_ERR;
        }
        cpu_id_event->cpubrd_slot_id = cpubrd_slot_id;
        cpu_id_event->event = event_type;

        ret = vos_task_create(&g_status_cpu_hotplug_task, "cpu_hotplug", (TASK_ENTRY)cpubrd_monitor_task, cpu_id_event,
            DEFAULT_PRIORITY);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s : create task cpu_hotplug fail", __FUNCTION__);
            g_free(cpu_id_event);
        }
        
    }
    return ret;
}


LOCAL gint32 bmc_sync_cpu_ras_event(guint8 cpubrd_slot_id, guint8 event)
{
    guint8 count = 1;
    gint32 ret = RET_OK;
    GSList *input_list = NULL;
    gint32 port = 51000;
    gchar rpc_server_ip[MAX_IP_LEN];
    GVariant *property_data = NULL;
    OBJ_HANDLE obj_handle = 0;

    debug_log(DLOG_DEBUG, "bmc_sync_cpu_ras_event %d %d", cpubrd_slot_id, event);

    if (cpubrd_slot_id <= 4 && cpubrd_slot_id > 0) {
        ret = set_cpu_ras_start_event(cpubrd_slot_id, event);
    } else {
        
        property_data = g_variant_new_byte(CPU_ID(cpubrd_slot_id));
        ret = dfl_get_specific_object(CLASS_CPUBOARD_NAME, PROPERTY_CPUBOARD_SLOT, property_data, &obj_handle);
        g_variant_unref(property_data);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s get PROPERTY_CPUBOARD_SLOT fail, result is %d", __FUNCTION__, ret);
            return ret;
        }

        ret = get_intra_bmc_ip(cpubrd_slot_id, rpc_server_ip, MAX_IP_LEN);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s get intra-bmc ip fail, result is %d", __FUNCTION__, ret);
            return ret;
        }

        input_list = g_slist_append(input_list, g_variant_new_byte(CPU_ID(cpubrd_slot_id)));
        input_list = g_slist_append(input_list, g_variant_new_byte(event));

        
        
        for (count = 1; count <= 3; count++) {
            ret = dfl_call_remote_class_method2(rpc_server_ip, port, dfl_get_object_name(obj_handle),
                METHOD_SET_RASEVENT, NULL, input_list, NULL);
            if (ret == DFL_OK) {
                break;
            }
            debug_log(DLOG_ERROR, "bmc_sync_cpu_ras_event error, retry %d times", count);
            vos_task_delay(30 * 1000);
        }
        
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s get sync intra bmc fail, result is %d", __FUNCTION__, ret);
            return ret;
        }
    }
    return ret;
}


LOCAL void *cpubrd_monitor_task(CPU_RAS_EVENT_S *p)
{
    GVariant *property_data = NULL;
    OBJ_HANDLE object_handle = 0;
    gint32 ret = RET_OK;
    guint8 cpldvalue = 0;
    guint8 value = 0;

    (void)prctl(PR_SET_NAME, (gulong) "CpuHotTask");

    while (g_status_cpu_hotplug_task_exit == FALSE) {
        vos_task_delay(500);

        
        property_data = g_variant_new_byte(p->cpubrd_slot_id);
        ret = dfl_get_specific_object(CLASS_CPUBOARD_NAME, PROPERTY_CPUBOARD_SLOT, property_data, &object_handle);
        g_variant_unref(property_data);
        property_data = NULL;
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s get object_handle fail, result is %d", __FUNCTION__, ret);
            continue;
        }

        
        ret = dfl_get_property_value(object_handle, CPU_BRD_MIGRATION_R_SIGNAL, &property_data);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s get CPU_BRD_MIGRATION_R_SIGNAL fail, result is %d", __FUNCTION__, ret);
            continue;
        }
        cpldvalue = g_variant_get_byte(property_data);
        g_variant_unref(property_data);
        property_data = NULL;

        debug_log(DLOG_DEBUG, "cpubrd_monitor_task value is %d", cpldvalue);
        if ((p->event == CPUBRD_HOT_REMOVE_CPU_OFFLINE) || (p->event == CPUBRD_HOT_REMOVE_CPUBOARD_POWEROFF)) {
            cpldvalue = (guint8)(~cpldvalue);
        }

        if ((p->cpubrd_slot_id == 1) || (p->cpubrd_slot_id == 4)) {
            if (CPUBRD_HOT_REMOVE_CPU_OFFLINE == p->event) {
                value = 0x08;
            } else {
                value = 0x02;
            }
        } else {
            if (CPUBRD_HOT_REMOVE_CPU_OFFLINE == p->event) {
                value = 0x04;
            } else {
                value = 0x01;
            }
        }

        if (cpldvalue & value) {
            debug_log(DLOG_DEBUG, "cpubrd_monitor_task value is %d %d", cpldvalue, value);
            g_status_cpu_hotplug_task_exit = TRUE;
            set_cpu_ras_finish_event(s_cpubrd_slot_id, p->event);
        }
    }
    g_free(p);
    return NULL;
}


gint32 cpubrd_hot_operate_sync_event(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = RET_OK;
    guint8 event_type = 0;
    guint8 cpubrd_slot_id = 0;

    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "[%s]: input_list is null", __FUNCTION__);
        return RET_ERR;
    }

    
    if (g_slist_length(input_list) != 2) {
        debug_log(DLOG_ERROR, " [%s][%d] failed with input_list length is not 1", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    cpubrd_slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    event_type = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));

    debug_log(DLOG_ERROR, "cpubrd_hot_operate_sync_event %d %d", cpubrd_slot_id, event_type);
    if (cpubrd_slot_id == 0) {
        if ((event_type == CPUBRD_HOT_REMOVE_CPU_OFFLINE) || (event_type == CPUBRD_HOT_REMOVE_CPUBOARD_POWEROFF) ||
            (event_type == CPUBRD_HOT_PLUG_CPU_ONLINE) || (event_type == CPUBRD_HOT_PLUG_POWERON) ||
            (event_type == CPUBRD_HOT_PLUG_CPU_RESET)) {
            
            ras_mission_status = ((ras_mission_status & 0xFFFFFF00) | 0x02);
        }
    } else {
        ret = set_cpu_ras_start_event(cpubrd_slot_id, event_type);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "set_cpu_ras_start_event  fail ---[%s] with result:%d", __FUNCTION__, ret);
            return ret;
        }
    }
    return ret;
}


LOCAL gint32 cpubrd_hot_operate_start(guint8 cpubrd_slot_id, guint8 operate)
{
    gint32 ret = RET_OK;

    if ((cpubrd_slot_id < 2) || (cpubrd_slot_id == 4) || (cpubrd_slot_id > 32)) {
        debug_log(DLOG_ERROR, "[%s] cpubrd_slot_id %d is invalid", __FUNCTION__, cpubrd_slot_id);
        return RET_ERR;
    }
    debug_log(DLOG_DEBUG, "cpubrd_hot_operate_start %d %d", cpubrd_slot_id, operate);
    s_cpubrd_slot_id = cpubrd_slot_id;
    ras_mission_status = 0;
    ras_mission_status = ras_mission_status | (0x01 << 24) | (operate << 16) | ((cpubrd_slot_id - 1) << 8);

    
    ret = write_check_cpld_data(CPU_ID(cpubrd_slot_id), CPU_BRD_MIGRATION_CTRL, 1);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "write ctrl cpld fail ---[%s] with result:%d", __FUNCTION__, ret);
        return ret;
    }

    ret = write_check_cpld_data(CPU_ID(cpubrd_slot_id), CPU_BRD_MIGRATION_CTRL, 0);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "write ctrl cpld fail ---[%s] with result:%d", __FUNCTION__, ret);
        return ret;
    }

    
    if (operate == FALSE) {
        strategy_log(SLOG_INFO, "CMC issued cpu hot remove operation.\r\n");
    } else {
        strategy_log(SLOG_INFO, "CMC issued cpu hot plug operation.\r\n");
    }
    return RET_OK;
}


LOCAL gint32 cpubrd_board_hot_remove(const guint8 *device_status, const guint8 *req_data)
{
    errno_t safe_fun_ret = EOK;
    guint8 oper_code = 0;
    guint8 progress_code = 0;
    guint8 progress_event = 0;
    guint8 cpu_id_1 = 0;
    IPMI_HOT_OPER_STATUS_S ipmi_msg;
    gint32 ret = RET_OK;
    gsize cpy_data_size;

    if (device_status == NULL) {
        debug_log(DLOG_ERROR, "[%s] Parameter is NULL", __FUNCTION__);
        return RET_ERR;
    }
    (void)memset_s(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), 0, sizeof(IPMI_HOT_OPER_STATUS_S));

    oper_code = device_status[1];
    cpu_id_1 = device_status[2];
    progress_code = device_status[4];
    cpy_data_size = get_ipmi_src_data_len(req_data);
    cpy_data_size = (cpy_data_size > sizeof(IPMI_HOT_OPER_STATUS_S)) ? sizeof(IPMI_HOT_OPER_STATUS_S) : cpy_data_size;

    debug_log(DLOG_DEBUG, "cpubrd_board_hot_remove  %d %d %d", oper_code, cpu_id_1, progress_code);

    switch (oper_code) {
        case CPUBRD_HOT_REMOVE_TYPE0:
            
            ret = cpubrd_hot_operate_start(cpu_id_1, FALSE);
            break;
        case CPUBRD_HOT_REMOVE_TYPE1:
            
            safe_fun_ret = memcpy_s(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), req_data, cpy_data_size);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            }

            ret = card_mgmt_send_smm_ipmi_msg(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), 3);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "send hot remove ras event failed, result is %d", ret);
                return ret;
            }

            progress_event = CPUBRD_HOT_REMOVE_START;
            ret = bmc_sync_cpu_ras_event(cpu_id_1 + 1, progress_event);
            break;

        case CPUBRD_HOT_REMOVE_TYPE4:
            
            ras_mission_status = 0;
            
            safe_fun_ret = memcpy_s(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), req_data, cpy_data_size);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            }

            ret = card_mgmt_send_smm_ipmi_msg(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), 3);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "send hot remove ras event failed, result is %d", ret);
                return ret;
            }
            break;

        case CPUBRD_HOT_REMOVE_TYPE3:
            safe_fun_ret = memcpy_s(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), req_data, cpy_data_size);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            }

            ret = card_mgmt_send_smm_ipmi_msg(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), 3);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "send hot remove ras event failed, result is %d", ret);
                return ret;
            }

            break;

        case CPUBRD_HOT_REMOVE_TYPE2:
            safe_fun_ret = memcpy_s(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), req_data, cpy_data_size);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            }

            ret = card_mgmt_send_smm_ipmi_msg(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), 3);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "send hot remove ras event failed, result is %d", ret);
                return ret;
            }

            ras_mission_status = ((ras_mission_status & 0xFFFFFF00) | 0x01);
            if (progress_code == 0x5) {
                progress_event = CPUBRD_HOT_REMOVE_CPU_OFFLINE;
            } else if (progress_code == 0x6) {
                progress_event = CPUBRD_HOT_REMOVE_CPUBOARD_POWEROFF;
            } else {
                return RET_OK;
            }

            ret = bmc_sync_cpu_ras_event(cpu_id_1 + 1, progress_event);
            break;

        default:
            ret = RET_ERR;
            debug_log(DLOG_ERROR, "[%s] oper_code [%d] is invalid", __FUNCTION__, oper_code);
            break;
    }

    return ret;
}


LOCAL gint32 cpubrd_board_hot_plug(const guint8 *device_status, const guint8 *req_data)
{
    guint8 oper_code = 0;
    guint8 cpu_id_1 = 0;
    IPMI_HOT_OPER_STATUS_S ipmi_msg;
    gint32 ret = RET_OK;
    guint8 progress_code = 0;
    guint8 progress_event = 0;
    errno_t safe_fun_ret = EOK;
    gsize cpy_data_size;

    if (device_status == NULL) {
        debug_log(DLOG_ERROR, "[%s] Parameter is NULL", __FUNCTION__);
        return RET_ERR;
    }
    (void)memset_s(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), 0, sizeof(IPMI_HOT_OPER_STATUS_S));

    oper_code = device_status[1];
    cpu_id_1 = device_status[2];
    progress_code = device_status[4];
    cpy_data_size = get_ipmi_src_data_len(req_data);
    cpy_data_size = (cpy_data_size > sizeof(IPMI_HOT_OPER_STATUS_S)) ? sizeof(IPMI_HOT_OPER_STATUS_S) : cpy_data_size;

    debug_log(DLOG_DEBUG, "cpubrd_board_hot_plug  %d %d", oper_code, cpu_id_1);

    switch (oper_code) {
        case CPUBRD_HOT_PLUG_TYPE0:
            
            ret = cpubrd_hot_operate_start(cpu_id_1, TRUE);
            break;

        case CPUBRD_HOT_PLUG_TYPE1:
            safe_fun_ret = memcpy_s(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), req_data, cpy_data_size);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            }

            ret = card_mgmt_send_smm_ipmi_msg(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), 3);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "send hot plug ras event failed, result is %d", ret);
                return ret;
            }
            break;

        case CPUBRD_HOT_PLUG_TYPE2:
            safe_fun_ret = memcpy_s(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), req_data, cpy_data_size);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            }

            ret = card_mgmt_send_smm_ipmi_msg(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), 3);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "send hot plug ras event failed, result is %d", ret);
                return ret;
            }

            ras_mission_status = ((ras_mission_status & 0xFFFFFF00) | 0x01);
            if (progress_code == 0x2) {
                progress_event = CPUBRD_HOT_PLUG_POWERON;
            } else if (progress_code == 0x3) {
                progress_event = CPUBRD_HOT_PLUG_CPU_ONLINE;
            } else if (progress_code == 0x4) {
                progress_event = CPUBRD_HOT_PLUG_CPU_RESET;
            } else {
                return RET_OK;
            }
            ret = bmc_sync_cpu_ras_event(cpu_id_1 + 1, progress_event);
            break;

        case CPUBRD_HOT_PLUG_TYPE3:
            
            safe_fun_ret = memcpy_s(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), req_data, cpy_data_size);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            }

            ret = card_mgmt_send_smm_ipmi_msg(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), 3);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "send hot plug ras event failed, result is %d", ret);
                return ret;
            }
            break;
        case CPUBRD_HOT_PLUG_TYPE4:
            
            ras_mission_status = 0;
            
            safe_fun_ret = memcpy_s(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), req_data, cpy_data_size);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            }

            ret = card_mgmt_send_smm_ipmi_msg(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), 3);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "send hot plug ras event failed, result is %d", ret);
                return ret;
            }
            if (progress_code == 0x0) {
                progress_event = CPUBRD_HOT_PLUG_FINISH;
                ret = bmc_sync_cpu_ras_event(cpu_id_1 + 1, progress_event);
            }
            break;

        default:
            ret = RET_ERR;
            debug_log(DLOG_ERROR, "[%s] oper_code [%d] is invalid", __FUNCTION__, oper_code);
            break;
    }

    return ret;
}


gint32 cpu_board_ipmi_set_device_status(const void *msg_data, gpointer user_data)
{
    gint32 ret = 0;
    const guint8 *req_data = NULL;
    guint8 resp_data[255] = {0};
    guint32 manufactureid = 0;
    guint32 imana = 0;
    guint8 device_status[6] = {0};
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_value = NULL;
    guint8 req_data_len;

    
    if (msg_data == NULL) {
        return RET_ERR;
    }

    

    
    req_data = get_ipmi_src_data(msg_data);
    req_data_len = get_ipmi_src_data_len(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "get ipmi src data fail!");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    
    ret = dfl_get_object_handle(BMC_OBJECT_NAME, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "ret = %d", ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_CANNOT_RESPONSE);
    }

    ret = dfl_get_property_value(obj_handle, BMC_MANU_ID_NAME, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "ret = %d", ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_CANNOT_RESPONSE);
    }
    

    manufactureid = g_variant_get_uint32(property_value);
    g_variant_unref(property_value);

    
    imana = MAKE_DWORD(0, req_data[2], req_data[1], req_data[0]);
    if (manufactureid != imana) {
        debug_log(DLOG_ERROR, "%s: manufactureid(%d) is invalid", __FUNCTION__, imana);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    device_status[DEV_STAT_1_INDEX] = req_data[DEV_STAT_1_IPMI_OFFSET];
    device_status[DEV_STAT_2_INDEX] = req_data[DEV_STAT_2_IPMI_OFFSET];
    device_status[DEV_STAT_3_INDEX] = req_data[DEV_STAT_3_IPMI_OFFSET];
    device_status[DEV_STAT_4_INDEX] = req_data[DEV_STAT_4_IPMI_OFFSET];
    device_status[DEV_STAT_5_INDEX] = (req_data_len > DEV_STAT_5_IPMI_OFFSET) ? req_data[DEV_STAT_5_IPMI_OFFSET] : 0;
    device_status[DEV_STAT_6_INDEX] = (req_data_len > DEV_STAT_6_IPMI_OFFSET) ? req_data[DEV_STAT_6_IPMI_OFFSET] : 0;

    switch (device_status[0]) {
        case CPUBRD_HW_STAT_TYPE0:
            resp_data[0] = COMP_CODE_SUCCESS;
            resp_data[1] = LONGB0(manufactureid);
            resp_data[2] = LONGB1(manufactureid);
            resp_data[3] = LONGB2(manufactureid);
            ipmi_send_response(msg_data, 4, resp_data);

            ret = cpubrd_board_hot_remove(device_status, req_data);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "cpu hot remove fail ---[%s] with result:%d", __FUNCTION__, ret);
                ipmi_operation_log(msg_data, "Set CPU board status failed");
                return ret;
            }
            break;

        case CPUBRD_HW_STAT_TYPE1:
            resp_data[0] = COMP_CODE_SUCCESS;
            resp_data[1] = LONGB0(manufactureid);
            resp_data[2] = LONGB1(manufactureid);
            resp_data[3] = LONGB2(manufactureid);
            ipmi_send_response(msg_data, 4, resp_data);

            ret = cpubrd_board_hot_plug(device_status, req_data);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "cpu hot plug fail ---[%s] with result:%d", __FUNCTION__, ret);
                ipmi_operation_log(msg_data, "Set CPU board status failed");
                return ret;
            }
            break;

        default:
            debug_log(DLOG_ERROR, "%s: invalid device status(%d)", __FUNCTION__, device_status[0]);
            return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    ipmi_operation_log(msg_data, "Set CPU board status successfully");
    return RET_OK;
}


#define RSP_LENGTH 4
gint32 ipmi_get_ras_mission_status(const void *msg_data, gpointer user_data)
{
    gint32 ret = 0;
    const guint8 *req_data = NULL;
    guint32 manufactureid = 0;
    guint32 imana = 0;
    guint8 resp_data[255] = {0};
    static guint8 cmc_ras_status = RAS_EVENT_INVALID_STAT;
    static guint8 num = 1;
    OBJ_HANDLE obj_handle = 0;
    errno_t safe_fun_ret = EOK;
    
    if (msg_data == NULL) {
        return RET_ERR;
    }

    
    req_data = get_ipmi_src_data(msg_data);
    
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "get ipmi src data fail!");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }
    

    
    ret = dfl_get_object_handle(BMC_OBJECT_NAME, &obj_handle);
    
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "ret = %d", ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_CANNOT_RESPONSE);
    }
    

    
    ret = dal_get_property_value_uint32(obj_handle, BMC_MANU_ID_NAME, &manufactureid);
    
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "ret = %d", ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_CANNOT_RESPONSE);
    }
    

    

    
    imana = MAKE_DWORD(0, req_data[2], req_data[1], req_data[0]);
    if (manufactureid != imana) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    num++;
    resp_data[0] = COMP_CODE_SUCCESS;
    resp_data[1] = LONGB0(manufactureid);
    resp_data[2] = LONGB1(manufactureid);
    resp_data[3] = LONGB2(manufactureid);

    
    if (req_data[4] == RAS_EVENT_CPU_HOT_PLUG) {
        cmc_ras_status = RAS_EVENT_INVALID_STAT;
        resp_data[4] = (guint8)((ras_mission_status >> 24) & 0xff);
        resp_data[5] = (guint8)((ras_mission_status >> 16) & 0xff);
        resp_data[6] = (guint8)((ras_mission_status >> 8) & 0xff);
        resp_data[7] = (guint8)(ras_mission_status & 0xff);
    } else if (cmc_ras_status == RAS_EVENT_MEM_HOT_PLUG || cmc_ras_status == RAS_EVENT_BUSY) {
        resp_data[4] = cmc_ras_status;
        resp_data[5] = 0;
        resp_data[6] = 0;
        resp_data[7] = 0;
        cmc_ras_status = RAS_EVENT_INVALID_STAT;
    } else {
        resp_data[4] = 0xff;
        resp_data[5] = 0xff;
        resp_data[6] = 0xff;
        resp_data[7] = 0xff;
        ipmi_send_response(msg_data, 8, resp_data);

        if (num % 3 == 1) {
            IPMI_HOT_OPER_RAS_MSG_S ipmi_msg;
            guint8 rsp_data[RSP_LENGTH] = {0};
            num = 0;
            safe_fun_ret =
                memcpy_s(&ipmi_msg, sizeof(IPMI_HOT_OPER_RAS_MSG_S), req_data, sizeof(IPMI_HOT_OPER_RAS_MSG_S));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            }

            ret = card_mgmt_get_smm_ras_status(&ipmi_msg, sizeof(IPMI_HOT_OPER_RAS_MSG_S), rsp_data, RSP_LENGTH, 3);
            if (ret == RET_OK) {
                cmc_ras_status = rsp_data[0];
                debug_log(DLOG_ERROR, "ipmi_get_ras_mission_status 0x%x 0x%x 0x%x 0x%x", rsp_data[0], rsp_data[1],
                    rsp_data[2], rsp_data[3]);
            }
        }
        return ret;
    }
    debug_log(DLOG_ERROR, "ipmi_get_ras_mission_status 0x%x 0x%x 0x%x 0x%x", resp_data[4], resp_data[5], resp_data[6],
        resp_data[7]);
    return ipmi_send_response(msg_data, 8, resp_data);
}


gint32 cpu_board_init(void)
{
    (void)dfl_foreach_object(CLASS_CPUBOARD_NAME, process_each_cpu_board, NULL, NULL);

    return RET_OK;
}


gint32 cpu_board_start(void)
{
    return RET_OK;
}


gint32 cpu_board_get_bios_fw_ver(const void *req_msg, gpointer user_data)
{
#define CPU_BOARD_BIOS_VER_MAX_LEN 32
#define CPU_BIOS_MSG_HEAD_MIN_LEN 4
#define CPU_BIOS_MSG_OFFSET_LEN 6
    errno_t safe_fun_ret;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    const guint8 *req_data = NULL;
    guint8 resp_len;
    gchar version_data[CPU_BOARD_BIOS_VER_MAX_LEN + 1] = {0};
    guint8 resp_data[RESP_MSG_MAX_DATA_LEN] = {0};
    guint8 index_id = 0;
    guint8 slot;

    req_data = get_ipmi_src_data(req_msg);
    if (req_data == NULL) {
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    slot = req_data[CPU_BIOS_MSG_HEAD_MIN_LEN + 1];
    ret = dal_get_specific_object_byte(CLASS_CPUBOARD_NAME, PROPERTY_CPUBOARD_SLOT, slot, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret);
        return ipmi_send_simple_response(req_msg, COMP_CODE_CANNOT_RESPONSE);
    }
    ret = dal_get_property_value_string(obj_handle, PROPERTY_CPUBOARD_BIOSVER, version_data, sizeof(version_data));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get cpuboard firmware version object failed", __FUNCTION__);
        return ipmi_send_simple_response(req_msg, COMP_CODE_CANNOT_RESPONSE);
    }

    resp_data[index_id++] = COMP_CODE_SUCCESS;
    resp_data[index_id++] = (guint8)strlen(version_data);

    safe_fun_ret = memmove_s(resp_data + index_id, sizeof(resp_data) - index_id, version_data, strlen(version_data));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    resp_len = (guint8)(strlen(version_data) + index_id);

    return ipmi_send_response(req_msg, resp_len, resp_data);
}


gint32 cpu_board_set_bios_fw_ver(const void *req_msg, gpointer user_data)
{
    errno_t safe_fun_ret;
    OBJ_HANDLE obj_handle = 0;
    gchar bios_ver[CPU_BOARD_BIOS_VER_MAX_LEN + 1] = {0};
    guint8 resp_data[CPU_BIOS_MSG_HEAD_MIN_LEN] = {0};
    guint32 manufactureid = OEM_MANUFACTURE_ID;
    const guint8 *src_data = NULL;
    guint8 index_id = 0;
    guint32 valid_len;
    gint32 ret_val;
    guint8 slot;

    src_data = get_ipmi_src_data(req_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, DB_STR_GET_SRC_DATA_FAIL);
        ipmi_operation_log(req_msg, "Set cpuboard firmware version failed");
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    
    valid_len = get_ipmi_src_data_len(req_msg) - (CPU_BIOS_MSG_OFFSET_LEN);
    if ((valid_len == 0) || (valid_len > CPU_BOARD_BIOS_VER_MAX_LEN)) {
        debug_log(DLOG_ERROR, "Request msg len:%d is invalid!", valid_len);
        ipmi_operation_log(req_msg, "Set cpuboard firmware version failed");
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    
    safe_fun_ret =
        memmove_s(bios_ver, sizeof(bios_ver), (const gchar *)&(src_data[CPU_BIOS_MSG_OFFSET_LEN]), valid_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    
    bios_ver[valid_len] = '\0';

    
    slot = src_data[CPU_BIOS_MSG_HEAD_MIN_LEN + 1];
    ret_val = dal_get_specific_object_byte(CLASS_CPUBOARD_NAME, PROPERTY_CPUBOARD_SLOT, slot, &obj_handle);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        ipmi_operation_log(req_msg, "Set cpuboard firmware version failed");
        return ipmi_send_simple_response(req_msg, COMP_CODE_CANNOT_RESPONSE);
    }

    ret_val = dal_set_property_value_string(obj_handle, PROPERTY_CPUBOARD_BIOSVER, bios_ver, DF_SAVE_TEMPORARY);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        ipmi_operation_log(req_msg, "Set cpuboard firmware version failed");
        return ipmi_send_simple_response(req_msg, COMP_CODE_CANNOT_RESPONSE);
    }

    
    resp_data[index_id++] = COMP_CODE_SUCCESS;
    resp_data[index_id++] = LONGB0(manufactureid);
    resp_data[index_id++] = LONGB1(manufactureid);
    resp_data[index_id++] = LONGB2(manufactureid);

    ipmi_operation_log(req_msg, "Set cpuboard %d firmware version to %s successfully", slot, bios_ver);
    return ipmi_send_response(req_msg, CPU_BIOS_MSG_HEAD_MIN_LEN, resp_data);
}
