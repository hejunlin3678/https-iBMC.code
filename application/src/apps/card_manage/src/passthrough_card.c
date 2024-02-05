



#include "pme_app/pme_app.h"
#include "passthrough_card.h"
#include "get_version.h"



gint32 passthrough_card_dump_info(const gchar *path)
{
    gint32 iRet = -1;
    gint32 ret = 0;
    guint16 board_id = 0;
    const gchar *name = NULL;
    const gchar *pcb_ver = NULL;
    const gchar *manufacturer = NULL;
    const gchar *type = NULL;
    const gchar *partnum = 0;
    gchar file_name[FILE_NAME_BUFFER_SIZE] = {0};
    gchar pass_card_info[MAX_CONFIG_LEN] = {0};
    FILE *fp = NULL;

    guint8 slot_id = 0;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    size_t fwrite_back = 0;

    gchar logic_ver[FIRMWARE_VERSION_STRING_LEN] = {0};

    if (path == NULL) {
        return RET_ERR;
    }

    ret = dfl_get_object_list(CLASS_PASSTHROUGH_CARD, &obj_list);
    if (ret != DFL_OK) {
        return RET_ERR;
    }

    // 在dump_dir目录下创建文件
    iRet = snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1, "%s/card_info", path);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }

    fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        g_slist_free(obj_list);
        return RET_ERR;
    }

    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);

    (void)snprintf_s(pass_card_info, sizeof(pass_card_info), sizeof(pass_card_info) - 1, "%s",
        "Pass Through Card Info\n");
    fwrite_back = fwrite(pass_card_info, strlen(pass_card_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    (void)memset_s(pass_card_info, sizeof(pass_card_info), 0, sizeof(pass_card_info));

    // 先写标题栏
    // 201705108246 suport partnum, 20190723 RAOJIANZHONG 00267466
    (void)snprintf_s(pass_card_info, sizeof(pass_card_info), sizeof(pass_card_info) - 1,
        "%-4s | %-10s | %-16s | %-32s | %-10s | %-10s | %-20s | %-8s\n", "Slot", "BoardId", "ProductName",
        "Manufacturer", "PCB Ver", "Logic Ver", "Type", "PartNum");

    fwrite_back = fwrite(pass_card_info, strlen(pass_card_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_SLOT);
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_BOARDID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_NAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_MANUFACTURER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_PCBVER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_DESC);
        // 201705108246 suport partnum? 20190723 RAOJIANZHONG 00267466
        property_name_list = g_slist_append(property_name_list, PROPERTY_PASSTHROUGH_CARD_PART_NUMBER);

        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value);
        g_slist_free(property_name_list);
        property_name_list = NULL;

        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get pass through card information failed!");
            g_slist_free(obj_list);
            (void)fclose(fp);
            return RET_ERR;
        }

        slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
        board_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 1));
        name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 2), 0);
        manufacturer = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 3), 0);
        pcb_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 4), 0);
        type = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 5), 0);
        // 201705108246 suport partnum, 20190723 RAOJIANZHONG 00267466
        partnum = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 6), 0);

        (void)memset_s(pass_card_info, sizeof(pass_card_info), 0, sizeof(pass_card_info));

        // 标题栏下填充值
        (void)snprintf_s(pass_card_info, sizeof(pass_card_info), sizeof(pass_card_info) - 1,
            "%-4u | 0x%-4x     | %-16s | %-32s | %-10s | %-10s | %-20s | %-8s\n", slot_id, board_id, name, manufacturer,
            pcb_ver, logic_ver, type, partnum);

        fwrite_back = fwrite(pass_card_info, strlen(pass_card_info), 1, fp);
        if (fwrite_back != 1) {
            (void)fclose(fp);
            g_slist_free(obj_list);
            g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_ERROR, "fwrite failed!");
            return RET_ERR;
        }

        (void)memset_s(pass_card_info, sizeof(pass_card_info), 0, sizeof(pass_card_info));
        g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
        property_value = NULL;
    }

    g_slist_free(obj_list);
    (void)snprintf_s(pass_card_info, sizeof(pass_card_info), sizeof(pass_card_info) - 1, "%s", "\n\n");

    fwrite_back = fwrite(pass_card_info, strlen(pass_card_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }
    (void)memset_s(pass_card_info, sizeof(pass_card_info), 0, sizeof(pass_card_info));
    // 关闭文件
    (void)fclose(fp);

    return RET_OK;
}


LOCAL gint32 process_each_passthroughcard(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret = 0;

    
    ret = get_pcb_version(obj_handle, PROPERTY_CARD_PCBID, PROPERTY_CARD_PCBVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    return RET_OK;
}


gint32 passthroughcard_add_object_callback(OBJ_HANDLE obj_handle)
{
    if (obj_handle == 0) {
        return RET_OK;
    }

    return process_each_passthroughcard(obj_handle, NULL);
}


gint32 passthroughcard_del_object_callback(OBJ_HANDLE obj_handle)
{
    return RET_OK;
}



gint32 passthroughcard_init(void)
{
    (void)dfl_foreach_object(CLASS_PASSTHROUGH_CARD, process_each_passthroughcard, NULL, NULL);

    return RET_OK;
}


gint32 passthroughcard_start(void)
{
    return RET_OK;
}
