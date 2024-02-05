

#include "pme_app/pme_app.h"
#include "cpu_board.h"
#include "mem_board.h"
#include "get_version.h"
#include "card_ipmi_msg.h"

MEMBRD_MIG_PROGRESS g_membrd_migration_progress = { 0 };
LOCAL TASKID g_membrd_migration_task_id = 0x00;
LOCAL TASKID g_membrd_status_scan_task_id = 0x00;
static guint8 last_button_event[MEM_BRD_MAX_NODE_NUM] = {0};

typedef struct {
    guint8 slotId;
    guint16 boardId;
    const gchar *name;
    const gchar *pcbVer;
    const gchar *logicVer;
    const gchar *manufacturer;
    guint8 locality;
} MemBoardInfo;


LOCAL gint32 mem_board_dump_head_info(FILE *fp)
{
    gint32 ret;
    gchar mem_board_info[MEM_BOARD_DUMPINFO_MAX_LEN + 1] = {0};
    size_t fwrite_back;

    ret = snprintf_s(mem_board_info, MEM_BOARD_DUMPINFO_MAX_LEN + 1, MEM_BOARD_DUMPINFO_MAX_LEN, "%s",
        "Memory board Info\n");
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    fwrite_back = fwrite(mem_board_info, strlen(mem_board_info), 1, fp);
    if (fwrite_back != 1) {
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    (void)memset_s(mem_board_info, MEM_BOARD_DUMPINFO_MAX_LEN + 1, 0, MEM_BOARD_DUMPINFO_MAX_LEN + 1);
    ret = snprintf_s(mem_board_info, MEM_BOARD_DUMPINFO_MAX_LEN + 1, MEM_BOARD_DUMPINFO_MAX_LEN,
        "%-4s | %-10s | %-10s | %-32s | %-10s | %s\n", "Slot", "BoardId", "Name", "Manufacturer", "PCB Ver",
        "Logic Ver");
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    fwrite_back = fwrite(mem_board_info, strlen(mem_board_info), 1, fp);
    if (fwrite_back != 1) {
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL void mem_board_get_info(MemBoardInfo *info, GSList *property_value)
{
    info->slotId = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    info->boardId = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 1));
    info->name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 2), 0);
    info->manufacturer = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 3), 0);
    info->pcbVer = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 4), 0);
    info->logicVer = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 5), 0);
    info->locality = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 6));
}


LOCAL gint32 mem_board_dump_content_info(FILE *fp, GSList *obj_list)
{
    gint32 ret;
    GSList *obj_note = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    gchar mem_board_info[MEM_BOARD_DUMPINFO_MAX_LEN + 1] = {0};
    MemBoardInfo info;

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEMBOARD_SLOT);
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEMBOARD_BOARDID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEMBOARD_NAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEMBOARD_MANUFACTURER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEMBOARD_PCBVER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEMBOARD_LOGICVER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEMBOARD_LOCALITY);

        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value);
        g_slist_free(property_name_list);
        property_name_list = NULL;
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get mem board information failed!");
            return RET_ERR;
        }

        (void)memset_s(&info, sizeof(info), 0, sizeof(info));
        mem_board_get_info(&info, property_value);

        
        if (info.locality != 0) {
            g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
            property_value = NULL;
            continue;
        }
        

        (void)memset_s(mem_board_info, MEM_BOARD_DUMPINFO_MAX_LEN + 1, 0, MEM_BOARD_DUMPINFO_MAX_LEN + 1);
        ret = snprintf_s(mem_board_info, sizeof(mem_board_info), sizeof(mem_board_info) - 1,
            "%-4u | 0x%-4x     | %-10s | %-32s | %-10s | %s\n", info.slotId, info.boardId, info.name, info.manufacturer,
            info.pcbVer, info.logicVer);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }

        if (fwrite(mem_board_info, strlen(mem_board_info), 1, fp) != 1) {
            g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_ERROR, "fwrite failed!");
            return RET_ERR;
        }

        (void)memset_s(mem_board_info, MEM_BOARD_DUMPINFO_MAX_LEN + 1, 0, MEM_BOARD_DUMPINFO_MAX_LEN + 1);
        g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
        property_value = NULL;
    }

    return RET_OK;
}


LOCAL void mem_board_dump_tail_info(FILE *fp)
{
    gchar mem_board_info[MEM_BOARD_DUMPINFO_MAX_LEN + 1] = {0};
    size_t fwrite_back;

    (void)snprintf_s(mem_board_info, MEM_BOARD_DUMPINFO_MAX_LEN + 1, MEM_BOARD_DUMPINFO_MAX_LEN, "%s", "\n\n");
    
    fwrite_back = fwrite(mem_board_info, strlen(mem_board_info), 1, fp);
    if (fwrite_back != 1) {
        debug_log(DLOG_ERROR, "fwrite failed!");
    }

    return;
}


gint32 mem_board_dump_info(const gchar *path)
{
    gint32 ret = -1;
    gchar file_name[MEM_BOARD_DUMPINFO_MAX_LEN + 1] = {0};
    FILE *fp = NULL;
    GSList *obj_list = NULL;

    if (path == NULL) {
        return RET_ERR;
    }

    ret = dfl_get_object_list(CLASS_MEMBOARD_NAME, &obj_list);
    if (ret != DFL_OK) {
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            return RET_OK;
        }
        return RET_ERR;
    }
    
    // 在dump_dir目录下创建文件
    ret = snprintf_s(file_name, MEM_BOARD_DUMPINFO_MAX_LEN + 1, MEM_BOARD_DUMPINFO_MAX_LEN, "%s/card_info", path);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        g_slist_free(obj_list);
        return RET_ERR;
    }

    
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);
    
    if (mem_board_dump_head_info(fp) != RET_OK) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        return RET_ERR;
    }

    if (mem_board_dump_content_info(fp, obj_list) != RET_OK) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        return RET_ERR;
    }

    mem_board_dump_tail_info(fp);

    g_slist_free(obj_list);
    // 关闭文件
    (void)fclose(fp);

    return RET_OK;
}


LOCAL gint32 mem_board_set_obj_property(OBJ_HANDLE handle, const gchar *name, const void *indata, MEMBRD_P_TYPE_E type,
    guint8 option)
{
    gint32 result = 0x00;
    gint32 ret_value = RET_OK;
    GVariant *property_data = NULL;

    switch (type) {
        case MEMBRD_P_TYPE_BYTE:
            property_data = g_variant_new_byte(*((const guint8 *)indata));
            break;

        case MEMBRD_P_TYPE_UINT16:
            property_data = g_variant_new_uint16(*((const guint16 *)indata));
            break;

        case MEMBRD_P_TYPE_UINT32:
            property_data = g_variant_new_uint32(*((const guint32 *)indata));
            break;

        case MEMBRD_P_TYPE_DOUBLE:
            property_data = g_variant_new_double(*((const gdouble *)indata));
            break;

        case MEMBRD_P_TYPE_STRING:
            property_data = g_variant_new_string((const gchar *)indata);
            break;

        default:
            return RET_ERR;
    }

    result = dfl_set_property_value(handle, name, property_data, option);
    g_variant_unref(property_data);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Set %s property fail", dfl_get_object_name(handle), name);
        ret_value = RET_ERR;
    }

    return ret_value;
}


LOCAL gint32 mem_board_slotid_logical_to_physical(guint8 logic_id, guint8 *phsical_id)
{
    GSList *list = NULL;
    gint32 ret;
    OBJ_HANDLE obj_handle;
    GVariant *get_value_physical = NULL;
    GVariant *get_value_logical = NULL;
    const guint8 *physicalid = NULL;
    const guint8 *logicalid = NULL;
    guint32 i;
    gsize n, m;
    ret = dfl_get_object_list("NodeIdMap", &list);
    if (ret != DFL_OK) {
        return ret;
    }
    if (list == NULL) {
        return RET_ERR;
    }
    obj_handle = (OBJ_HANDLE)list->data;

    ret = dfl_get_property_value(obj_handle, "PhysicalId", &get_value_physical);
    if (ret != DFL_OK) {
        g_slist_free(list);
        return ret;
    }

    physicalid = (const guint8 *)g_variant_get_fixed_array(get_value_physical, &n, sizeof(guint8));

    ret = dfl_get_property_value(obj_handle, "LogicalId", &get_value_logical);
    if (ret != DFL_OK) {
        g_slist_free(list);
        g_variant_unref(get_value_physical);
        return ret;
    }
    debug_log(DLOG_ERROR, "%s %d", __FUNCTION__, __LINE__);
    logicalid = (const guint8 *)g_variant_get_fixed_array(get_value_logical, &m, sizeof(guint8));
    if ((n != m) || (physicalid == NULL) || (logicalid == NULL)) {
        g_variant_unref(get_value_physical);
        g_variant_unref(get_value_logical);
        g_slist_free(list);
        return RET_ERR;
    }

    for (i = 0; i < m; i++) {
        if (logic_id == logicalid[i]) {
            *phsical_id = physicalid[i];
            goto exit_function;
        }
    }

    // 若在表中查询不到,直接返回逻辑值在后面函数中处理.
    *phsical_id = logic_id;

exit_function:
    g_slist_free(list);
    g_variant_unref(get_value_physical);
    g_variant_unref(get_value_logical);

    return RET_OK;
}




LOCAL gint32 mem_board_recover_migration_fail_status(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret = 0;
    guint8 default_value = 0;

    ret = mem_board_set_obj_property(obj_handle, MEM_BRD_MIGRATION_FAIL, &default_value, MEMBRD_P_TYPE_BYTE,
        DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set memory board migrate fail default value failed!");
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 mem_board_update_spare_status(guint8 node_id, guint8 operate)
{
    gint32 ret = RET_OK;
    guint16 spare_id_mask = 0;
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_value = NULL;
    guint8 offset = 0;
    // 目前只支持16块内存板
    if (MEM_BRD_MAX_NODE_NUM < node_id) {
        debug_log(DLOG_ERROR, "mem_board_update_spare_status,node_id is out of range, id = %d!", node_id);
        return RET_ERR;
    }
    offset = node_id - 1;
    ret = dfl_get_object_handle(ISMEMBOARD_OBJECT_NAME, &obj_handle);
    if (ret != DFL_OK) {
        return RET_ERR;
    }
    ret = dfl_get_property_value(obj_handle, PROPERTY_ISMEMBOARD_SPARE_ID_MASK, &property_value);
    if (ret != DFL_OK) {
        return RET_ERR;
    }

    spare_id_mask = g_variant_get_uint16(property_value);
    g_variant_unref(property_value);
    debug_log(DLOG_DEBUG, "mem_board_update_spare_status,node_id %d operate %d mask %x", node_id, operate,
        spare_id_mask);

    if (operate == MEM_BRD_SPARE_ENABLE) {
        spare_id_mask |= (guint16)(1 << offset);
    } else {
        spare_id_mask &= ~((guint16)(1 << offset));
    }

    debug_log(DLOG_DEBUG, "mem_board_update_spare_status, update mask %x", spare_id_mask);

    ret = mem_board_set_obj_property(obj_handle, PROPERTY_ISMEMBOARD_SPARE_ID_MASK, &spare_id_mask,
        MEMBRD_P_TYPE_UINT16, DF_SAVE_PERMANENT);
    return ret;
}


LOCAL void mem_board_migration_set_status(guint8 type, guint8 val)
{
    switch (type) {
        case MEMBRD_MIGRATION_TMOUT_COUNT:
            g_membrd_migration_progress.tmout_count = val;
            break;
        case MEMBRD_MIGRATION_PGS_STATUS:
            g_membrd_migration_progress.migration_state = val;
            break;
        case MEMBRD_MIGRATION_PERCENTAGE:
            g_membrd_migration_progress.percentage = val;
            break;
        case MEMBRD_MIGRATION_SRC_NODE:
            g_membrd_migration_progress.cur_node_id = val;
            break;
        case MEMBRD_MIGRATION_DES_NODE:
            g_membrd_migration_progress.cur_spare_id = val;
            break;

        default:
            debug_log(DLOG_ERROR, "mem_board_migration_set_status,type out of range!");
            break;
    }

    debug_log(DLOG_DEBUG, "mem_board_migration_set_status,type %d val %d", type, val);
    return;
}


LOCAL guint8 mem_board_migration_get_status(guint8 type)
{
    guint8 val = 0;
    switch (type) {
        case MEMBRD_MIGRATION_TMOUT_COUNT:
            val = g_membrd_migration_progress.tmout_count;
            break;
        case MEMBRD_MIGRATION_PGS_STATUS:
            val = g_membrd_migration_progress.migration_state;
            break;
        case MEMBRD_MIGRATION_PERCENTAGE:
            val = g_membrd_migration_progress.percentage;
            break;
        case MEMBRD_MIGRATION_SRC_NODE:
            val = g_membrd_migration_progress.cur_node_id;
            break;
        case MEMBRD_MIGRATION_DES_NODE:
            val = g_membrd_migration_progress.cur_spare_id;
            break;
        default:
            debug_log(DLOG_ERROR, "mem_board_migration_get_status,type out of range!");
            break;
    }

    debug_log(DLOG_MASS, "mem_board_migration_get_status,type %d val %d", type, val);

    return val;
}


LOCAL gint32 mem_board_get_button_status(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret = RET_OK;
    GVariant *property_value = NULL;
    guint8 cur_button_event = 0;
    guint8 button_test_mode = 0;
    guint8 value = 0;
    guint8 slot = 0;
    gchar accesor_name[MAX_NAME_SIZE] = {0};

    ret = dfl_get_extern_value(obj_handle, PROPERTY_MEMBOARD_BTN_EVENT, &property_value, DF_COPY);
    if (ret != DFL_OK) {
        return RET_OK;
    }
    cur_button_event = g_variant_get_byte(property_value);
    g_variant_unref(property_value);
    property_value = NULL;

    ret = dfl_get_property_value(obj_handle, PROPERTY_MEMBOARD_BTN_TEST_MODE, &property_value);
    if (ret != DFL_OK) {
        return RET_OK;
    }
    button_test_mode = g_variant_get_byte(property_value);
    g_variant_unref(property_value);
    property_value = NULL;

    ret = dfl_get_property_value(obj_handle, PROPERTY_MEMBOARD_SLOT, &property_value);
    if (ret != DFL_OK) {
        return RET_OK;
    }
    slot = g_variant_get_byte(property_value) - 1;
    g_variant_unref(property_value);

    if (cur_button_event == last_button_event[slot]) {
        return RET_OK;
    }
    last_button_event[slot] = cur_button_event;

    if ((cur_button_event == 0) || (button_test_mode == 1)) {
        return RET_OK;
    }

    property_value = g_variant_new_byte(value);
    ret = dfl_get_property_accessor(obj_handle, PROPERTY_MEMBOARD_CLEAR_BTN_EVENT, accesor_name, MAX_NAME_SIZE);
    if (ret == DFL_OK) {
        debug_log(DLOG_DEBUG, "mem_board_get_button_status, set slot%d accessor prop!", slot);
        card_manage_log_operation_log("MemoryBoard", obj_handle, PROPERTY_MEMBOARD_SLOT, NULL, CARD_BUTTON_PRESS);
        vos_task_delay(200UL);
        ret = dfl_set_property_value(obj_handle, PROPERTY_MEMBOARD_CLEAR_BTN_EVENT, property_value, DF_HW);
    } else {
        debug_log(DLOG_DEBUG, "mem_board_get_button_status, set slot%d obj proporty!", slot);
        card_manage_log_operation_log("MemoryBoard", obj_handle, PROPERTY_MEMBOARD_SLOT, NULL, CARD_BUTTON_PRESS);
        ret = dfl_set_property_value(obj_handle, PROPERTY_MEMBOARD_CLEAR_BTN_EVENT, property_value, DF_NONE);
    }
    g_variant_unref(property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "mem_board_get_button_status, set slot%d obj proporty fail, ret = %d!", slot, ret);
    }

    return RET_OK;
}


LOCAL void mem_board_status_monitor_task(void)
{
    gint32 ret = RET_OK;

    (void)prctl(PR_SET_NAME, (gulong) "StatusMonitor");

    while (1) {
        ret = dfl_foreach_object(CLASS_MEMBOARD_NAME, mem_board_get_button_status, NULL, NULL);
        if (ret != DFL_OK && ret != ERRCODE_OBJECT_NOT_EXIST) {
            debug_log(DLOG_ERROR, "mem_board_status_monitor_task, get button status fail! ret = %d", ret);
        }
        vos_task_delay(100UL);
    }
}


LOCAL void mem_board_migration_status_task(void)
{
    guint8 count = 0;
    guint8 migration_status = 0;
    gint32 ret = RET_OK;
    guint8 src_node_id = 0;
    guint8 dst_node_id = 0;
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_value = NULL;
    guint8 event_dir = 0;

    (void)prctl(PR_SET_NAME, (gulong) "MigrationTask");

    while (1) {
        // 查询当前迁移状态
        migration_status = mem_board_migration_get_status(MEMBRD_MIGRATION_PGS_STATUS);
        debug_log(DLOG_MASS, "mem_board_migration_status_task, waiting for migrate........ current status = %d",
            migration_status);

        // 触发迁移时 若不需要online 第一条上报的命令为迁移进度否则为online inprogress的命令
        while ((migration_status == MIGRATION_START) || (migration_status == MIGRATION_PATROLSCRUB_INPROGRESS) ||
            (migration_status == MIGRATION_BEGIN_ONLINE) || (migration_status == MIGRATION_ONLINE_SUCCESS) ||
            (migration_status == MIGRATION_INPROGRESS)) {
            // patrolscrub 过程不需记录超时
            if (migration_status == MIGRATION_PATROLSCRUB_INPROGRESS) {
                migration_status = mem_board_migration_get_status(MEMBRD_MIGRATION_PGS_STATUS);
                debug_log(DLOG_DEBUG, "mem_board_migration_status_task, patrol scrub........");
                vos_task_delay(1000 * 60);
                continue;
            }

            // 查询计数是否被其他模块清零
            count = mem_board_migration_get_status(MEMBRD_MIGRATION_TMOUT_COUNT);
            debug_log(DLOG_DEBUG, "mem_board_migration_status_task, migrating........ get tmout count = %d", count);

            if (count > MEM_BRD_MIGRATION_OVERTIME) {
                // BIOS挂死后无响应导致超时则清计时器同时设置迁移失败状态
                mem_board_migration_set_status(MEMBRD_MIGRATION_TMOUT_COUNT, 0);
                mem_board_migration_set_status(MEMBRD_MIGRATION_PGS_STATUS, MIGRATION_FAIL);
                src_node_id = mem_board_migration_get_status(MEMBRD_MIGRATION_SRC_NODE);
                dst_node_id = mem_board_migration_get_status(MEMBRD_MIGRATION_DES_NODE);
                event_dir = 1;

                property_value = g_variant_new_byte(src_node_id);
                ret = dfl_get_specific_object(CLASS_MEMBOARD_NAME, PROPERTY_MEMBOARD_SLOT, property_value, &obj_handle);
                g_variant_unref(property_value);
                property_value = NULL;
                if (ret == DFL_OK) {
                    ret = mem_board_set_obj_property(obj_handle, MEM_BRD_MIGRATION_FAIL, &event_dir, MEMBRD_P_TYPE_BYTE,
                        DF_SAVE_TEMPORARY);
                    if (ret != RET_OK) {
                        debug_log(DLOG_ERROR, "Set memory board migrate fail default value failed!");
                    }
                    maintenance_log_v2(MLOG_WARN, FC_CARD_MNG_MIGRATE_FAIL,
                        "BIOS, MemoryBoard, Board%d migrate to Board%d fail, Migrating Timeout.\r\n", src_node_id,
                        dst_node_id);
                }
                break;
            }

            // 更新超时计数
            count++;
            mem_board_migration_set_status(MEMBRD_MIGRATION_TMOUT_COUNT, count);
            // 重新查询迁移状态失败后退出计数
            migration_status = mem_board_migration_get_status(MEMBRD_MIGRATION_PGS_STATUS);
            debug_log(DLOG_DEBUG, "mem_board_migration_status_task, migrating........ set tmout count = %d", count);
            vos_task_delay(1000 * 60);
        }

        vos_task_delay(1000);
    }
}


LOCAL gint32 mem_board_set_migration_status(OBJ_HANDLE handle, guint8 slot_id, guint8 event_dir,
    const guint8 *event_data)
{
    gint32 ret = RET_OK;
    guint8 event_type = 0;
    guint8 log_backup_node = 0;
    guint8 phy_backup_node = 0;
    guint8 mig_percentage = 0;

    log_backup_node = event_data[0];
    event_type = event_data[1];

    ret = mem_board_slotid_logical_to_physical(log_backup_node, &phy_backup_node);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    // 不管迁移失败或成功,还是在迁移中都进行计数清零 保证线程不超时
    mem_board_migration_set_status(MEMBRD_MIGRATION_TMOUT_COUNT, 0);
    mem_board_migration_set_status(MEMBRD_MIGRATION_SRC_NODE, slot_id);
    mem_board_migration_set_status(MEMBRD_MIGRATION_DES_NODE, phy_backup_node);
    debug_log(DLOG_DEBUG, "mem_board_set_migration_status,slot_id %d phy_backup_node %d", slot_id, phy_backup_node);
    switch (event_type) {
        case MEMBRD_MIGRATION_STAT0:
            mem_board_migration_set_status(MEMBRD_MIGRATION_PGS_STATUS, MIGRATION_SUCCESS);
            mem_board_migration_set_status(MEMBRD_MIGRATION_PERCENTAGE, 100);
            ret = mem_board_set_obj_property(handle, MEM_BRD_MIGRATION_SUCC, &event_dir, MEMBRD_P_TYPE_BYTE, DF_NONE);
            if (ret != RET_OK) {
                return RET_ERR;
            }
            // 迁移成功后需要重新更新内存板备份状态为非备份模式
            ret = mem_board_update_spare_status(phy_backup_node, MEM_BRD_SPARE_DISABLE);
            if (ret != RET_OK) {
                return RET_ERR;
            }
            // 迁移成功后需要清除原有迁移失败内存板的告警 即只要成功一次 原有迁移失败告警则恢复
            ret = dfl_foreach_object(CLASS_MEMBOARD_NAME, mem_board_recover_migration_fail_status, NULL, NULL);
            if (ret != DFL_OK) {
                return RET_ERR;
            }
            strategy_log(SLOG_INFO, "BIOS, MemoryBoard, Board%d migrate to Board%d success.\r\n", slot_id,
                phy_backup_node);
            break;

        case MEMBRD_MIGRATION_STAT1:
            mem_board_migration_set_status(MEMBRD_MIGRATION_PGS_STATUS, MIGRATION_FAIL);
            ret = mem_board_set_obj_property(handle, MEM_BRD_MIGRATION_FAIL, &event_dir, MEMBRD_P_TYPE_BYTE,
                DF_SAVE_TEMPORARY);
            if (ret != RET_OK) {
                return RET_ERR;
            }

            if (phy_backup_node == MEM_BRD_SPARE_NOT_EXIST) {
                strategy_log(SLOG_WARN, "BIOS, MemoryBoard, Board%d migrate fail, Spare board not exist.\r\n", slot_id);
            }

            if (phy_backup_node != MEM_BRD_SPARE_NOT_EXIST) {
                strategy_log(SLOG_WARN, "BIOS, MemoryBoard, Board%d migrate to Board%d fail, Config error.\r\n",
                    slot_id, phy_backup_node);
            }

            break;
        case MEMBRD_MIGRATION_STAT2:
            mem_board_migration_set_status(MEMBRD_MIGRATION_PGS_STATUS, MIGRATION_FAIL);
            ret = mem_board_set_obj_property(handle, MEM_BRD_MIGRATION_FAIL, &event_dir, MEMBRD_P_TYPE_BYTE,
                DF_SAVE_TEMPORARY);
            if (ret != RET_OK) {
                return RET_ERR;
            }
            strategy_log(SLOG_WARN, "BIOS, MemoryBoard, Board%d migrate to Board%d fail, None-NUMA mode.\r\n", slot_id,
                phy_backup_node);
            break;
        case MEMBRD_MIGRATION_STAT3:
            mem_board_migration_set_status(MEMBRD_MIGRATION_PGS_STATUS, MIGRATION_FAIL);
            ret = mem_board_set_obj_property(handle, MEM_BRD_MIGRATION_FAIL, &event_dir, MEMBRD_P_TYPE_BYTE,
                DF_SAVE_TEMPORARY);
            if (ret != RET_OK) {
                return RET_ERR;
            }
            strategy_log(SLOG_WARN, "BIOS, MemoryBoard, Board%d migrate to Board%d fail, Less memory size.\r\n",
                slot_id, phy_backup_node);
            break;
        case MEMBRD_MIGRATION_STAT4:
            mem_board_migration_set_status(MEMBRD_MIGRATION_PGS_STATUS, MIGRATION_FAIL);
            ret = mem_board_set_obj_property(handle, MEM_BRD_MIGRATION_FAIL, &event_dir, MEMBRD_P_TYPE_BYTE,
                DF_SAVE_TEMPORARY);
            if (ret != RET_OK) {
                return RET_ERR;
            }
            strategy_log(SLOG_WARN, "BIOS, MemoryBoard, Board%d migrate to Board%d fail, On-line failure.\r\n", slot_id,
                phy_backup_node);
            break;
        case MEMBRD_MIGRATION_STAT5:
            mem_board_migration_set_status(MEMBRD_MIGRATION_PGS_STATUS, MIGRATION_FAIL);
            ret = mem_board_set_obj_property(handle, MEM_BRD_MIGRATION_FAIL, &event_dir, MEMBRD_P_TYPE_BYTE,
                DF_SAVE_TEMPORARY);
            if (ret != RET_OK) {
                return RET_ERR;
            }
            strategy_log(SLOG_WARN, "BIOS, MemoryBoard, Board%d migrate to Board%d fail, Copy data failure.\r\n",
                slot_id, phy_backup_node);
            break;

        case MEMBRD_MIGRATION_STAT6:
            mem_board_migration_set_status(MEMBRD_MIGRATION_PGS_STATUS, MIGRATION_BEGIN_ONLINE);
            debug_log(DLOG_DEBUG, "BIOS, MemoryBoard, Board%d migrate to Board%d, online inprogress.", slot_id,
                phy_backup_node);
            break;

        case MEMBRD_MIGRATION_STAT7:
            mem_board_migration_set_status(MEMBRD_MIGRATION_PGS_STATUS, MIGRATION_ONLINE_SUCCESS);
            strategy_log(SLOG_INFO, "BIOS, MemoryBoard, Board%d migrate to Board%d, On-line success.\r\n", slot_id,
                phy_backup_node);
            break;

        case MEMBRD_MIGRATION_STAT8:
            mig_percentage = event_data[2];
            mem_board_migration_set_status(MEMBRD_MIGRATION_PGS_STATUS, MIGRATION_INPROGRESS);
            mem_board_migration_set_status(MEMBRD_MIGRATION_PERCENTAGE, mig_percentage);
            debug_log(DLOG_DEBUG, "BIOS, MemoryBoard, Board%d migrate to Board%d inprogress, precentage is %d.",
                slot_id, phy_backup_node, mig_percentage);
            break;

        case MEMBRD_MIGRATION_STAT9:
            mem_board_migration_set_status(MEMBRD_MIGRATION_PGS_STATUS, MIGRATION_START);
            strategy_log(SLOG_INFO, "BIOS, MemoryBoard, Board%d migrate start.\r\n", slot_id);
            break;

        case MEMBRD_MIGRATION_STAT10:
            mem_board_migration_set_status(MEMBRD_MIGRATION_PGS_STATUS, MIGRATION_END);
            strategy_log(SLOG_INFO, "BIOS, MemoryBoard, Board%d migrate cancel.\r\n", slot_id);
            break;

        case MEMBRD_MIGRATION_STAT11:
            mem_board_migration_set_status(MEMBRD_MIGRATION_PGS_STATUS, MIGRATION_PATROLSCRUB_INPROGRESS);
            strategy_log(SLOG_INFO, "BIOS, MemoryBoard, Board%d migrate to Board%d, patrol scrub start.\r\n", slot_id,
                phy_backup_node);
            break;

        case MEMBRD_MIGRATION_STAT12:
            mem_board_migration_set_status(MEMBRD_MIGRATION_PGS_STATUS, MIGRATION_PATROLSCRUB_INPROGRESS);
            strategy_log(SLOG_INFO, "BIOS, MemoryBoard, Board%d migrate to Board%d, patrol scrub end.\r\n", slot_id,
                phy_backup_node);
            break;

        default:
            return RET_ERR;
    }

    return ret;
}


gint32 mem_board_get_migration_status(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint8 migration_status = 0;
    guint8 mig_percentage = 0;
    guint8 src_node_id = 0;
    guint8 des_node_id = 0;
    gint32 result = 0;
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_data = NULL;
    guint16 spare_id_mask = 0;
    gchar string_tmp[MEM_BRD_MAX_STRING_LENGTH] = {0};
    guint8 bit_num = 0;
    guint8 flag = 0;
    gint32 offset = 0;

    
    if (output_list == NULL) {
        debug_log(DLOG_ERROR, "Get mem_board migration status failed, output_list can not be NULL.");
        return RET_ERR;
    }
    

    migration_status = mem_board_migration_get_status(MEMBRD_MIGRATION_PGS_STATUS);
    mig_percentage = mem_board_migration_get_status(MEMBRD_MIGRATION_PERCENTAGE);
    src_node_id = mem_board_migration_get_status(MEMBRD_MIGRATION_SRC_NODE);
    des_node_id = mem_board_migration_get_status(MEMBRD_MIGRATION_DES_NODE);

    
    
    offset = snprintf_s(string_tmp, MEM_BRD_MAX_STRING_LENGTH, MEM_BRD_MAX_STRING_LENGTH - 1,
        "{ \"status\": %u, \"percentage\": %u, \"src_node_id\": %u, \"des_node_id\": %u, \"spare_node_id\": [",
        migration_status, mig_percentage, src_node_id, des_node_id);

    result = dfl_get_object_handle(ISMEMBOARD_OBJECT_NAME, &obj_handle);
    if (result != DFL_OK) {
        return RET_ERR;
    }
    result = dfl_get_property_value(obj_handle, PROPERTY_ISMEMBOARD_SPARE_ID_MASK, &property_data);
    if (result != DFL_OK) {
        return RET_ERR;
    }
    spare_id_mask = g_variant_get_uint16(property_data);
    debug_log(DLOG_DEBUG, "[%s] spare_id_mask = %x", __FUNCTION__, spare_id_mask);
    g_variant_unref(property_data);

    while (spare_id_mask) {
        bit_num++;
        if (spare_id_mask & 0x0001) {
            if (flag == 0) {
                offset += snprintf_truncated_s(string_tmp + offset, MEM_BRD_MAX_STRING_LENGTH - offset, "%u", bit_num);
                flag = 1;
            } else {
                offset += snprintf_truncated_s(string_tmp + offset, MEM_BRD_MAX_STRING_LENGTH - offset, ",%u", bit_num);
            }
        }

        spare_id_mask >>= 1;
    }

    offset += snprintf_truncated_s(string_tmp + offset, MEM_BRD_MAX_STRING_LENGTH - offset, "]}");
    
    debug_log(DLOG_DEBUG, "[%s] string length = %d string: %s", __FUNCTION__, offset, string_tmp);

    *output_list = g_slist_append(*output_list, g_variant_new_string(string_tmp));

    return RET_OK;
}


gint32 mem_board_migration_slotid_valid_check(guint8 src_node_id, guint8 dest_node_id)
{
    gint32 ret = RET_OK;
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_data = NULL;
    guint16 spare_id_mask = 0;

    // 内存板id不能大于支持内存板总数
    if ((src_node_id > MEM_BRD_MAX_NODE_NUM) || (dest_node_id > MEM_BRD_MAX_NODE_NUM)) {
        debug_log(DLOG_ERROR, "%s, src_node%d and des_node%d out of range!", __FUNCTION__, src_node_id, dest_node_id);
        return RET_ERR;
    }

    // 迁移内存板与目的内存板id不能相同
    if (src_node_id == dest_node_id) {
        debug_log(DLOG_ERROR, "%s, src_node id can not equal with des_node id, src_node_id %d des_node_id %d!",
            __FUNCTION__, src_node_id, dest_node_id);
        return RET_ERR;
    }

    // 迁移内存板不是备份板 目的内存板属于备份板
    ret = dfl_get_object_handle(ISMEMBOARD_OBJECT_NAME, &obj_handle);
    if (ret != DFL_OK) {
        return RET_ERR;
    }
    ret = dfl_get_property_value(obj_handle, PROPERTY_ISMEMBOARD_SPARE_ID_MASK, &property_data);
    if (ret != DFL_OK) {
        return RET_ERR;
    }
    spare_id_mask = g_variant_get_uint16(property_data);
    debug_log(DLOG_ERROR, "[%s] spare_id_mask = %x", __FUNCTION__, spare_id_mask);
    g_variant_unref(property_data);
    if ((spare_id_mask >> (src_node_id - 1)) & 0x1) {
        debug_log(DLOG_ERROR, "%s, src_node%d is spare board, can not migrate!", __FUNCTION__, src_node_id);
        return RET_ERR;
    }
    if (!((spare_id_mask >> (dest_node_id - 1)) & 0x1)) {
        debug_log(DLOG_ERROR, "%s, des_node_id%d is not spare board, can not migrate!", __FUNCTION__, dest_node_id);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 mem_board_set_migration_enable(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = RET_OK;
    GVariant *property_data = NULL;
    guint8 src_node_id = 0;
    guint8 dest_node_id = 0;
    gchar accesor_name[MAX_NAME_SIZE] = {0};
    guint8 value = 0;
    OBJ_HANDLE obj_handle = 0;
    guint8 migration_status = 0;

    if (input_list == NULL) {
        
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        method_operation_log(caller_info, NULL, "Start memory-board migration failed");
        return RET_ERR;
    }
    if (g_slist_length(input_list) != 2) {
        debug_log(DLOG_ERROR, "[%s][%d] failed with input_list length is not 2", __FUNCTION__, __LINE__);
        method_operation_log(caller_info, NULL, "Start memory-board migration failed");
        return RET_ERR;
    }

    migration_status = mem_board_migration_get_status(MEMBRD_MIGRATION_PGS_STATUS);
    if ((migration_status != MIGRATION_END) && (migration_status != MIGRATION_SUCCESS) &&
        (migration_status != MIGRATION_FAIL)) {
        debug_log(DLOG_ERROR, "migration is inprogress, please wait!");
        method_operation_log(caller_info, NULL, "Start memory-board migration failed");
        return RET_ERR;
    }
    // 不论8p 4p始终上报物理的丝印信息
    src_node_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    dest_node_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
    ret = mem_board_migration_slotid_valid_check(src_node_id, dest_node_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, mem_board_migration_slotid_valid_check failed, slotid %d", __FUNCTION__,
            src_node_id);
        method_operation_log(caller_info, NULL, "Start memory-board%d migration failed", src_node_id);
        return ret;
    }
    property_data = g_variant_new_byte(src_node_id);
    ret = dfl_get_specific_object(CLASS_MEMBOARD_NAME, PROPERTY_MEMBOARD_SLOT, property_data, &obj_handle);
    g_variant_unref(property_data);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get node%d obj failed, result is %d", __FUNCTION__, ret, src_node_id);
        method_operation_log(caller_info, NULL, "Start memory-board%d migration failed", src_node_id);
        return ret;
    }
    ret = dfl_get_property_accessor(obj_handle, MEM_BRD_MIGRATION_CTRL, accesor_name, MAX_NAME_SIZE);
    debug_log(DLOG_DEBUG, "[%s] with result:%d", __FUNCTION__, ret);
    // 若未配置关联accessor, 将通过agentapp进行配置
    if (ret == DFL_OK) {
        value = 1;
        property_data = g_variant_new_byte(value);
        ret = dfl_set_property_value(obj_handle, MEM_BRD_MIGRATION_CTRL, property_data, DF_HW);
        debug_log(DLOG_DEBUG, "[%s] with result:%d", __FUNCTION__, ret);
        g_variant_unref(property_data);
    } else {
        ret = dfl_call_remote_class_method(PARTITION_OBJECT_NAME, METHOD_SET_MEM_BRD_PROP, NULL, input_list, NULL);
        debug_log(DLOG_DEBUG, "[%s] with result:%d", __FUNCTION__, ret);
    }
    if (ret == DFL_OK) {
        // 设置寄存器属性成功后置迁移开始标识位
        mem_board_migration_set_status(MEMBRD_MIGRATION_PGS_STATUS, MIGRATION_START);
        mem_board_migration_set_status(MEMBRD_MIGRATION_PERCENTAGE, 0);
        mem_board_migration_set_status(MEMBRD_MIGRATION_TMOUT_COUNT, 0);

        mem_board_migration_set_status(MEMBRD_MIGRATION_SRC_NODE, src_node_id);
        mem_board_migration_set_status(MEMBRD_MIGRATION_DES_NODE, dest_node_id);
        method_operation_log(caller_info, NULL, "Start memory-board%d migration successfully", src_node_id);
        return RET_OK;
    } else {
        method_operation_log(caller_info, NULL, "Start memory-board%d migration failed", src_node_id);
        return RET_ERR;
    }
}


gint32 mem_board_set_migration_refresh(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint8 refresh_flag = 0;
    if (input_list == NULL) {
        
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        method_operation_log(caller_info, NULL, "Migration finished, 'OK' clicked failed");
        return RET_ERR;
    }
    if (g_slist_length(input_list) != 1) {
        debug_log(DLOG_ERROR, "[%s][%d] failed with input_list length is not 1", __FUNCTION__, __LINE__);
        method_operation_log(caller_info, NULL, "Migration finished, 'OK' clicked failed");
        return RET_ERR;
    }

    refresh_flag = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if (refresh_flag == 1) {
        mem_board_migration_set_status(MEMBRD_MIGRATION_TMOUT_COUNT, 0);
        mem_board_migration_set_status(MEMBRD_MIGRATION_PGS_STATUS, 0);
        mem_board_migration_set_status(MEMBRD_MIGRATION_PERCENTAGE, 0);
        mem_board_migration_set_status(MEMBRD_MIGRATION_SRC_NODE, 0);
        mem_board_migration_set_status(MEMBRD_MIGRATION_DES_NODE, 0);
        method_operation_log(caller_info, NULL, "Migration finished, 'OK' clicked successfully");
    } else {
        method_operation_log(caller_info, NULL, "Migration finished, 'OK' clicked failed");
    }

    return RET_OK;
}


LOCAL void mem_board_operate_mirror_status(guint8 event_dir, guint8 phy_slot_id)
{
    gint32 ret = RET_OK;
    OBJ_HANDLE obj_handle = 0;
    guint8 light_led_on = 0;
    guint8 count = 0;

    ret = dal_get_specific_object_byte(CLASS_MEMBOARD_NAME, PROPERTY_MEMBOARD_SLOT, phy_slot_id, &obj_handle);

    
    light_led_on = event_dir > 0 ? 0 : 1;

    if (ret == RET_OK) {
        debug_log(DLOG_DEBUG, "[%s][%d] set MemBrd%d Value [%d]", __FUNCTION__, __LINE__, phy_slot_id, light_led_on);

        do {
            ret = dal_set_property_value_byte(obj_handle, MEM_BRD_MIRROR_STATUS, light_led_on, DF_AUTO);
            count++;
        } while ((ret != RET_OK) && (count <= 3));
    } else {
        debug_log(DLOG_ERROR, "[%s][%d] get MemBrd%d Failed", __FUNCTION__, __LINE__, phy_slot_id);
    }

    return;
}


gint32 mem_board_sync_set_dev_status(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = RET_OK;
    guint16 cfg_error_code = 0;
    guint8 dev_stat = 0;
    guint8 log_slot_id = 0;
    guint8 phy_slot_id = 0;
    guint8 event_dir = 0;
    guint8 event_data[3] = {0};
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_value = NULL;
    guint16 mirror_id_mask = 0;

    guint8 bit_num = 0;

    if (input_list == NULL) {
        
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    event_dir = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    dev_stat = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
    log_slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 2));
    event_data[0] = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 3));
    event_data[1] = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 4));
    event_data[2] = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 5));
    debug_log(DLOG_ERROR, "mem_board_sync_set_dev_status %d %d %d %d %d %d", event_dir, dev_stat, log_slot_id,
        event_data[0], event_data[1], event_data[2]);

    ret = mem_board_slotid_logical_to_physical(log_slot_id, &phy_slot_id);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    // 仅内存板镜像不需要设置属性
    if ((dev_stat != MEMBRD_DEV_STAT_TYPE5) && (dev_stat != MEMBRD_DEV_STAT_TYPE2)) {
        property_value = g_variant_new_byte(phy_slot_id);
        ret = dfl_get_specific_object(CLASS_MEMBOARD_NAME, PROPERTY_MEMBOARD_SLOT, property_value, &obj_handle);
        g_variant_unref(property_value);
        property_value = NULL;
        if (ret != DFL_OK) {
            return RET_ERR;
        }
    }

    switch (dev_stat) {
        case MEMBRD_DEV_STAT_TYPE0: // 迁移
            ret = mem_board_set_migration_status(obj_handle, phy_slot_id, event_dir, (const guint8 *)event_data);
            if (ret != RET_OK) {
                return RET_ERR;
            }
            break;

        case MEMBRD_DEV_STAT_TYPE1: // 内存板热插过程中发生错误
            ret = mem_board_set_obj_property(obj_handle, MEM_BRD_ADD_FAIL, &event_dir, MEMBRD_P_TYPE_BYTE, DF_NONE);
            if (ret != RET_OK) {
                return RET_ERR;
            }
            strategy_log(SLOG_ERROR, "BIOS, MemoryBoard, Board%d Hot-Plug failed.\r\n", phy_slot_id);
            break;

        case MEMBRD_DEV_STAT_TYPE2: // 内存板备份板上报
            ret = mem_board_update_spare_status(phy_slot_id, MEM_BRD_SPARE_ENABLE);
            if (ret != RET_OK) {
                return RET_ERR;
            }
            strategy_log(SLOG_INFO, "BIOS, MemoryBoard, Board%d to spare mode for migration successfully.\r\n",
                phy_slot_id);
            break;

        case MEMBRD_DEV_STAT_TYPE3: // 内存板Online
            ret = mem_board_set_obj_property(obj_handle, MEM_BRD_ONLINE, &event_dir, MEMBRD_P_TYPE_BYTE, DF_NONE);
            if (ret != RET_OK) {
                return RET_ERR;
            }
            strategy_log(SLOG_INFO, "BIOS, MemoryBoard, Board%d transition to on-line successfully.\r\n", phy_slot_id);
            break;

        case MEMBRD_DEV_STAT_TYPE4: // 内存板Offline成功
            ret = mem_board_set_obj_property(obj_handle, MEM_BRD_OFFLINE, &event_dir, MEMBRD_P_TYPE_BYTE, DF_NONE);
            if (ret != RET_OK) {
                return RET_ERR;
            }
            strategy_log(SLOG_INFO, "BIOS, MemoryBoard, Board%d transition to off-line successfully.\r\n", phy_slot_id);
            break;

        case MEMBRD_DEV_STAT_TYPE5: // 内存板镜像板上报
            mirror_id_mask = MAKE_WORD(event_data[0], log_slot_id);
            while (mirror_id_mask) {
                if (mirror_id_mask & 0x0001) {
                    ret = mem_board_slotid_logical_to_physical(bit_num, &phy_slot_id);
                    if (ret != RET_OK) {
                        return RET_ERR;
                    }
                    strategy_log(SLOG_INFO, "BIOS, MemoryBoard, Board%d to mirror mode successfully.\r\n", phy_slot_id);

                    
                    mem_board_operate_mirror_status(event_dir, phy_slot_id);
                    
                }
                mirror_id_mask >>= 1;
                ++bit_num;
            }
            break;

        case MEMBRD_DEV_STAT_TYPE6: // 内存板上内存存在预警
            ret = mem_board_set_obj_property(obj_handle, MEM_BRD_ERR_PREDICTION, &event_dir, MEMBRD_P_TYPE_BYTE,
                DF_SAVE_PERMANENT);
            if (ret != RET_OK) {
                return RET_ERR;
            }
            strategy_log(SLOG_WARN, "BIOS, MemoryBoard, Board%d to error prediction.\r\n", phy_slot_id);
            break;

        
        
        case MEMBRD_DEV_STAT_TYPE254:
            cfg_error_code = MAKE_WORD(event_data[1], event_data[2]);
            ret = mem_board_set_obj_property(obj_handle, PROPERTY_MEMBOARD_CONFIG_ERROR_CODE, &cfg_error_code,
                MEMBRD_P_TYPE_UINT16, DF_SAVE_PERMANENT);
            if (ret != RET_OK) {
                return RET_ERR;
            }
            break;
            

        default:
            return RET_ERR;
    }

    return RET_OK;
}




LOCAL gint32 membrd_hot_operate_start(guint8 membrd_slot_id)
{
    gint32 ret = RET_OK;
    GVariant *property_data = NULL;
    guint8 value = 0;
    OBJ_HANDLE obj_handle = 0;

    if ((membrd_slot_id < 1) || (membrd_slot_id > 8)) {
        debug_log(DLOG_ERROR, "[%s] membrd_slot_id %d is invalid", __FUNCTION__, membrd_slot_id);
        return RET_ERR;
    }

    debug_log(DLOG_ERROR, "membrd_hot_operate_start  %d %d", membrd_slot_id, __LINE__);

    property_data = g_variant_new_byte(membrd_slot_id);
    ret = dfl_get_specific_object(CLASS_MEMBOARD_NAME, PROPERTY_MEMBOARD_SLOT, property_data, &obj_handle);
    g_variant_unref(property_data);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get node%d obj failed, result is %d", __FUNCTION__, membrd_slot_id, ret);
        return ret;
    }

    
    value = 1;
    property_data = g_variant_new_byte(value);
    ret = dfl_set_property_value(obj_handle, MEM_BRD_MIGRATION_CTRL, property_data, DF_HW);
    debug_log(DLOG_DEBUG, "[%s] with result:%d", __FUNCTION__, ret);
    g_variant_unref(property_data);
    return ret;
}


LOCAL gint32 mem_board_hot_remove(const guint8 *device_status, guint32 len, const guint8 *req_data)
{
    guint8 oper_code = 0;
    guint8 node_id_1 = 0;
    IPMI_HOT_OPER_STATUS_S ipmi_msg;
    gint32 ret = RET_OK;
    errno_t safe_fun_ret = EOK;
    gsize cpy_data_size;

    if (device_status == NULL) {
        debug_log(DLOG_ERROR, "[%s] Parameter is NULL", __FUNCTION__);
        return RET_ERR;
    }
    if (len != MEM_BRD_DEVICE_STATUS_LEN) {
        debug_log(DLOG_ERROR, "[%s] Parameter is Invalid", __FUNCTION__);
        return RET_ERR;
    }
    (void)memset_s(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), 0, sizeof(IPMI_HOT_OPER_STATUS_S));

    oper_code = device_status[1];
    node_id_1 = device_status[2];
    cpy_data_size = get_ipmi_src_data_len(req_data);
    cpy_data_size = (cpy_data_size > sizeof(IPMI_HOT_OPER_STATUS_S)) ? sizeof(IPMI_HOT_OPER_STATUS_S) : cpy_data_size;

    debug_log(DLOG_DEBUG, "mem_board_hot_remove  %d %d", oper_code, node_id_1);
    switch (oper_code) {
        case MEMBRD_HOT_REMOVE_TYPE0:
            
            
            ras_mission_status = 0;
            if (node_id_1 % 8 == 0) {
                ret = membrd_hot_operate_start(8);
            } else {
                ret = membrd_hot_operate_start((node_id_1 % 8));
            }
            
            strategy_log(SLOG_INFO, "CMC issued mem board hot remove operation.\r\n");
            break;
        case MEMBRD_HOT_REMOVE_TYPE1:
        case MEMBRD_HOT_REMOVE_TYPE2:
        case MEMBRD_HOT_REMOVE_TYPE3:
        case MEMBRD_HOT_REMOVE_TYPE4:
            safe_fun_ret = memcpy_s(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), req_data, cpy_data_size);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            }
            ret = card_mgmt_send_smm_ipmi_msg(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), 3);
            break;
        default:
            ret = RET_ERR;
            debug_log(DLOG_ERROR, "[%s] oper_code [%d] is invalid", __FUNCTION__, oper_code);
            break;
    }

    return ret;
}

LOCAL gint32 mem_board_hot_plug(const guint8 *device_status, const guint8 *req_data)
{
    guint8 oper_code = 0;
    guint8 node_id_1 = 0;
    IPMI_HOT_OPER_STATUS_S ipmi_msg;
    gint32 ret = RET_OK;
    gsize cpy_data_size;
    errno_t safe_fun_ret = EOK;

    if (device_status == NULL) {
        debug_log(DLOG_ERROR, "[%s] Parameter is NULL", __FUNCTION__);
        return RET_ERR;
    }
    (void)memset_s(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), 0, sizeof(IPMI_HOT_OPER_STATUS_S));

    oper_code = device_status[1];
    node_id_1 = device_status[2];
    cpy_data_size = get_ipmi_src_data_len(req_data);
    cpy_data_size = (cpy_data_size > sizeof(IPMI_HOT_OPER_STATUS_S)) ? sizeof(IPMI_HOT_OPER_STATUS_S) : cpy_data_size;

    debug_log(DLOG_ERROR, "mem_board_hot_plug  %d %d", oper_code, node_id_1);
    switch (oper_code) {
        case MEMBRD_HOT_PLUG_TYPE0:
            
            
            ras_mission_status = 0;
            if (node_id_1 % 8 == 0) {
                ret = membrd_hot_operate_start(8);
            } else {
                ret = membrd_hot_operate_start(node_id_1 % 8);
            }
            strategy_log(SLOG_INFO, "CMC issued mem board hot plug operation.\r\n");
            break;
        case MEMBRD_HOT_PLUG_TYPE1:
        case MEMBRD_HOT_PLUG_TYPE2:
        case MEMBRD_HOT_PLUG_TYPE3:
        case MEMBRD_HOT_PLUG_TYPE4:
            safe_fun_ret = memcpy_s(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), req_data, cpy_data_size);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            }
            ret = card_mgmt_send_smm_ipmi_msg(&ipmi_msg, sizeof(IPMI_HOT_OPER_STATUS_S), 3);
            break;
        default:
            ret = RET_ERR;
            debug_log(DLOG_ERROR, "[%s] oper_code [%d] is invalid", __FUNCTION__, oper_code);
            break;
    }
    return ret;
}


gint32 mem_board_ipmi_set_device_status(const void *msg_data, gpointer user_data)
{
    const guint8 *req_data = NULL;
    guint8 resp_data[255] = {0};
    guint32 manufactureid = 0;
    guint32 imana = 0;
    guint8 event_dir = 0;
    guint8 device_status[MEM_BRD_DEVICE_STATUS_LEN] = {0};
    GVariant *property_value = NULL;
    guint8 phy_slot_id = 0;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = 0;
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

    
    ret = dal_get_property_value_uint32(obj_handle, BMC_MANU_ID_NAME, &manufactureid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "ret = %d", ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_CANNOT_RESPONSE);
    }

    
    

    
    imana = MAKE_DWORD(0, req_data[2], req_data[1], req_data[0]);
    if (imana != manufactureid) {
        debug_log(DLOG_ERROR, "%s: invalid manufactureid(%d)", __FUNCTION__, imana);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    event_dir = !req_data[5]; // 兼容以1作为触发告警的方案
    device_status[DEV_STAT_1_INDEX] = req_data[DEV_STAT_1_IPMI_OFFSET];
    device_status[DEV_STAT_2_INDEX] = req_data[DEV_STAT_2_IPMI_OFFSET];
    device_status[DEV_STAT_3_INDEX] = req_data[DEV_STAT_3_IPMI_OFFSET];
    device_status[DEV_STAT_4_INDEX] = req_data[DEV_STAT_4_IPMI_OFFSET];
    device_status[DEV_STAT_5_INDEX] = (req_data_len > DEV_STAT_5_IPMI_OFFSET) ? req_data[DEV_STAT_5_IPMI_OFFSET] : 0;
    device_status[DEV_STAT_6_INDEX] = (req_data_len > DEV_STAT_6_IPMI_OFFSET) ? req_data[DEV_STAT_6_IPMI_OFFSET] : 0;
    debug_log(DLOG_ERROR,
        "mem_card_ipmi_set_device_status(%d) %s, dev_type(%d) node_id (%d), channel_id (%d), dimm_id (%d), chip_id "
        "(%d)",
        device_status[0], (event_dir == 1) ? "Asserted" : "Deasserted", device_status[0], device_status[1],
        device_status[2], device_status[3], device_status[4]);

    ret = mem_board_slotid_logical_to_physical(device_status[1], &phy_slot_id);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Set memory board device status failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    debug_log(DLOG_ERROR, "mem_board_ipmi_set_device_status  %d", device_status[0]);
    switch (device_status[0]) {
        // PC: Power Chip
        case MEMBRD_HW_STAT_TYPE1:
            property_value = g_variant_new_byte(phy_slot_id);
            ret = dfl_get_specific_object(CLASS_MEMBOARD_NAME, PROPERTY_MEMBOARD_SLOT, property_value, &obj_handle);
            g_variant_unref(property_value);
            if (ret != DFL_OK) {
                ipmi_operation_log(msg_data, "Set memory board device status failed");
                return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
            }

            ret = mem_board_set_obj_property(obj_handle, MEM_BRD_HW_PC_FAULT, &event_dir, MEMBRD_P_TYPE_BYTE,
                DF_SAVE_TEMPORARY);
            if (ret != RET_OK) {
                ipmi_operation_log(msg_data, "Set memory board device status failed");
                return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
            }
            maintenance_log_v2(MLOG_ERROR, FC_CARD_MNG_PWR_CHIP_FAIL,
                "BIOS, MemoryBoard, Board%d power-chip-id%d fail.\r\n", phy_slot_id, device_status[4]);
            break;
        case MEMBRD_HW_STAT_TYPE2:
            ret = mem_board_hot_remove(device_status, MEM_BRD_DEVICE_STATUS_LEN, req_data);
            if (ret != RET_OK) {
                ipmi_operation_log(msg_data, "Set memory board device status failed");
                return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
            }
            break;
        case MEMBRD_HW_STAT_TYPE3:
            ret = mem_board_hot_plug(device_status, req_data);
            if (ret != RET_OK) {
                ipmi_operation_log(msg_data, "Set memory board device status failed");
                return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
            }
            break;
        default:
            debug_log(DLOG_ERROR, "%s: invalid device_status(%d)", __FUNCTION__, device_status[0]);
            return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }
    resp_data[0] = COMP_CODE_SUCCESS;
    resp_data[1] = LONGB0(manufactureid);
    resp_data[2] = LONGB1(manufactureid);
    resp_data[3] = LONGB2(manufactureid);
    ipmi_operation_log(msg_data, "Set memory board device status successfully");
    return ipmi_send_response(msg_data, 4, resp_data);
}


LOCAL gint32 mem_board_recover_device_status(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret = 0;
    guint8 default_value = 0;

    ret = mem_board_set_obj_property(obj_handle, MEM_BRD_MIGRATION_SUCC, &default_value, MEMBRD_P_TYPE_BYTE,
        DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set memory board migrate successful default value failed!");
    }

    ret = mem_board_set_obj_property(obj_handle, MEM_BRD_MIGRATION_FAIL, &default_value, MEMBRD_P_TYPE_BYTE,
        DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set memory board migrate fail default value failed!");
    }

    ret =
        mem_board_set_obj_property(obj_handle, MEM_BRD_ADD_FAIL, &default_value, MEMBRD_P_TYPE_BYTE, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set memory board install fail default value failed!");
    }

    ret = mem_board_set_obj_property(obj_handle, MEM_BRD_ONLINE, &default_value, MEMBRD_P_TYPE_BYTE, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set memory board online default value failed!");
    }

    ret =
        mem_board_set_obj_property(obj_handle, MEM_BRD_OFFLINE, &default_value, MEMBRD_P_TYPE_BYTE, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set memory board offline default value failed!");
    }

    ret = mem_board_set_obj_property(obj_handle, MEM_BRD_HW_PC_FAULT, &default_value, MEMBRD_P_TYPE_BYTE,
        DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set memory board pc fault default value failed!");
    }

    ret = mem_board_set_obj_property(obj_handle, MEM_BRD_ERR_PREDICTION, &default_value, MEMBRD_P_TYPE_BYTE,
        DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set memory board err prediction default value failed!");
    }

    
    ret = dal_set_property_value_uint16(obj_handle, PROPERTY_MEMBOARD_CONFIG_ERROR_CODE, 0x0000, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set memory board config error code failed: %d!", ret);
    }
    
    return RET_OK;
}


LOCAL gint32 mem_board_migration_recover_default_status(void)
{
    gint32 ret = RET_OK;
    guint16 spare_id_mask = 0;
    OBJ_HANDLE obj_handle = 0;

    mem_board_migration_set_status(MEMBRD_MIGRATION_TMOUT_COUNT, 0);
    mem_board_migration_set_status(MEMBRD_MIGRATION_PGS_STATUS, 0);
    mem_board_migration_set_status(MEMBRD_MIGRATION_PERCENTAGE, 0);
    mem_board_migration_set_status(MEMBRD_MIGRATION_SRC_NODE, 0);
    mem_board_migration_set_status(MEMBRD_MIGRATION_DES_NODE, 0);

    ret = dfl_get_object_handle(ISMEMBOARD_OBJECT_NAME, &obj_handle);
    if (ret != DFL_OK) {
        return ret;
    }

    ret = mem_board_set_obj_property(obj_handle, PROPERTY_ISMEMBOARD_SPARE_ID_MASK, &spare_id_mask,
        MEMBRD_P_TYPE_UINT16, DF_SAVE_PERMANENT);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Recover memory board spare id mask value failed!");
        return ret;
    }

    return RET_OK;
}


void mem_board_set_device_status_default_value(void)
{
    gint32 ret = 0;

    ret = dfl_foreach_object(CLASS_MEMBOARD_NAME, mem_board_recover_device_status, NULL, NULL);
    if (ret != DFL_OK) {
        if (ret != ERRCODE_OBJECT_NOT_EXIST) {
            debug_log(DLOG_ERROR, "Recover memory board default value failed!");
        }
    }

    ret = mem_board_migration_recover_default_status();
    if (ret != RET_OK) {
        if (ret != ERRCODE_OBJECT_NOT_EXIST) {
            debug_log(DLOG_ERROR, "Recover memory board migration default value failed!");
        }
    }

    return;
}


LOCAL gint32 process_each_mem_board(OBJ_HANDLE handle, gpointer data)
{
    gint32 ret = 0;

    
    ret = get_pcb_version(handle, PROPERTY_MEMBOARD_PCBID, PROPERTY_MEMBOARD_PCBVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    
    ret = get_logic_version(handle, PROPERTY_MEMBOARD_LOGICVERID, PROPERTY_MEMBOARD_LOGICVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    return RET_OK;
}


gint32 mem_board_add_object_callback(OBJ_HANDLE object_handle)
{
    card_manage_log_operation_log("Memory board", object_handle, PROPERTY_MEMBOARD_SLOT, NULL, CARD_PLUG_IN);
    return process_each_mem_board(object_handle, NULL);
}


gint32 mem_board_del_object_callback(OBJ_HANDLE object_handle)
{
    card_manage_log_operation_log("Memory board", object_handle, PROPERTY_MEMBOARD_SLOT, NULL, CARD_PLUG_OUT);
    return RET_OK;
}


gint32 mem_board_init(void)
{
    (void)dfl_foreach_object(CLASS_MEMBOARD_NAME, process_each_mem_board, NULL, NULL);

    return RET_OK;
}


gint32 mem_board_start(void)
{
    
    OBJ_HANDLE computer_handle = 0;
    gint32 result = RET_OK;
    guint32 product_id = 0;
    if (dal_get_object_handle_nth(CLASS_COMPUTER_PARTITION, 0, &computer_handle) == RET_OK) {
        result = dal_get_property_value_uint32(computer_handle, PROPERTY_COMPUTER_PARTITION_RHPRODUCTID, &product_id);
        if ((result == RET_OK) && (product_id == COMPUTER_ID_9008V5_ADVANCE)) {
            return RET_OK;
        }
    }
    

    (void)vos_task_create(&g_membrd_migration_task_id, "membrd_migration_status",
        (TASK_ENTRY)mem_board_migration_status_task, NULL, DEFAULT_PRIORITY);
    (void)vos_task_create(&g_membrd_status_scan_task_id, "membrd_status_monitor_task",
        (TASK_ENTRY)mem_board_status_monitor_task, NULL, DEFAULT_PRIORITY);
    return RET_OK;
}
