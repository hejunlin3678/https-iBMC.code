

#include "pme_app/pme_app.h"
#include "main_board.h"
#include "get_version.h"


LOCAL gint32 main_board_get_other_info(gchar *location, gint32 l_len, gchar *devicename, gint32 d_len,
    gchar *partnumber, gint32 p_len, guint16 board_id)
{
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    gint32 ret = 0;
    guchar fruid = 0;
    guint16 com_board_id = 0;
    guchar com_board_type = 0;
    OBJ_HANDLE fru_handle = 0;
    OBJ_HANDLE ref_elabel_handle = 0;

    if (location == NULL || devicename == NULL || partnumber == NULL) {
        debug_log(DLOG_ERROR, "%s, %d input param error!", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_object_list failed!");
        return ret;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        (void)dal_get_property_value_uint16((OBJ_HANDLE)obj_note->data, PROPERTY_COMPONENT_BOARDID, &com_board_id);
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_COMPONENT_DEVICE_TYPE, &com_board_type);

        if (com_board_type != COMPONENT_TYPE_MAINBOARD) {
            continue;
        }
        if (com_board_id != board_id) {
            continue;
        }

        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_COMPONENT_FRUID, &fruid);

        if (fruid != 0xFF) {
            ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, fruid, &fru_handle);
            if (ret != RET_OK) {
                debug_log(DLOG_MASS, "%s, %d: get specific object fail.", __FUNCTION__, __LINE__);
                break;
            }

            ret = dfl_get_referenced_object(fru_handle, PROPERTY_FRU_ELABEL_RO, &ref_elabel_handle);
            if (ret != DFL_OK) {
                debug_log(DLOG_MASS, "%s, %d: get referenced object fail.", __FUNCTION__, __LINE__);
                break;
            }

            (void)dal_get_property_value_string(ref_elabel_handle, PROPERTY_ELABEL_BOARD_PN, partnumber, p_len);

            (void)dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_COMPONENT_LOCATION, location,
                l_len);

            (void)dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_COMPONENT_DEVICE_NAME, devicename,
                d_len);

            break;
        }
    }

    g_slist_free(obj_list);

    return RET_OK;
}

gint32 main_board_dump_info(const gchar *path)
{
    gint32 ret = 0;
    guint16 board_id = 0;
    guint8 bom_id = 0;
    const gchar *name = NULL;
    const gchar *pcb_ver = NULL;
    gchar file_name[MAIN_BOARD_DUMPINFO_MAX_LEN + 1] = {0};
    gchar main_board_info[MAIN_BOARD_DUMPINFO_MAX_LEN + 1] = {0};
    FILE *fp = NULL;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    size_t fwrite_back = 0;

    gchar location[MAIN_BOARD_DUMPINFO_MAX_LEN] = {0};
    gchar devicename[MAIN_BOARD_DUMPINFO_MAX_LEN] = {0};
    gchar partnumber[MAIN_BOARD_DUMPINFO_MAX_LEN] = {0};

    if (path == NULL) {
        return RET_ERR;
    }

    ret = dfl_get_object_list(CLASS_MAINBOARD_NAME, &obj_list);
    if (ret != DFL_OK) {
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            return RET_OK;
        }
        return RET_ERR;
    }

    
    // 在dump_dir目录下创建文件
    ret = snprintf_s(file_name, MAIN_BOARD_DUMPINFO_MAX_LEN + 1, MAIN_BOARD_DUMPINFO_MAX_LEN, "%s/card_info", path);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        g_slist_free(obj_list);
        return RET_ERR;
    }
    
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);
    
    (void)snprintf_s(main_board_info, MAIN_BOARD_DUMPINFO_MAX_LEN + 1, MAIN_BOARD_DUMPINFO_MAX_LEN, "%s",
        "Main board Info\n");
    fwrite_back = fwrite(main_board_info, strlen(main_board_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    
    (void)memset_s(main_board_info, MAIN_BOARD_DUMPINFO_MAX_LEN + 1, 0, MAIN_BOARD_DUMPINFO_MAX_LEN + 1);
    (void)snprintf_s(main_board_info, MAIN_BOARD_DUMPINFO_MAX_LEN + 1, MAIN_BOARD_DUMPINFO_MAX_LEN,
        "%-10s | %-10s | %-10s | %-10s | %-15s | %-15s | %-15s \n", "BoardId", "Name", "PCB Ver", "Bom Id", "Location",
        "Device Name", "Part Number");
    

    fwrite_back = fwrite(main_board_info, strlen(main_board_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        property_name_list = g_slist_append(property_name_list, PROPERTY_MAINBOARD_BOARDID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_MAINBOARD_NAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_MAINBOARD_PCBVER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_MAINBOARD_BOMID);

        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value);
        g_slist_free(property_name_list);
        property_name_list = NULL;
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get main board information failed!");
            g_slist_free(obj_list);
            (void)fclose(fp);
            return RET_ERR;
        }

        board_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 0));
        name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 1), 0);
        pcb_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 2), 0);
        bom_id = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 3));

        
        (void)main_board_get_other_info(location, MAIN_BOARD_DUMPINFO_MAX_LEN, devicename, MAIN_BOARD_DUMPINFO_MAX_LEN,
            partnumber, MAIN_BOARD_DUMPINFO_MAX_LEN, board_id);

        (void)memset_s(main_board_info, MAIN_BOARD_DUMPINFO_MAX_LEN + 1, 0, MAIN_BOARD_DUMPINFO_MAX_LEN + 1);
        ret = snprintf_s(main_board_info, sizeof(main_board_info), sizeof(main_board_info) - 1,
            "0x%-4x     | %-10s | %-10s | %-10u | %-15s | %-15s | %-15s \n", board_id, name, pcb_ver, bom_id, location,
            devicename, partnumber);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        

        fwrite_back = fwrite(main_board_info, strlen(main_board_info), 1, fp);
        if (fwrite_back != 1) {
            (void)fclose(fp);
            g_slist_free(obj_list);
            g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_ERROR, "fwrite failed!");
            return RET_ERR;
        }

        (void)memset_s(main_board_info, MAIN_BOARD_DUMPINFO_MAX_LEN + 1, 0, MAIN_BOARD_DUMPINFO_MAX_LEN + 1);
        g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
        property_value = NULL;
    }

    g_slist_free(obj_list);
    (void)snprintf_s(main_board_info, MAIN_BOARD_DUMPINFO_MAX_LEN + 1, MAIN_BOARD_DUMPINFO_MAX_LEN, "%s", "\n\n");
    
    fwrite_back = fwrite(main_board_info, strlen(main_board_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    (void)memset_s(main_board_info, MAIN_BOARD_DUMPINFO_MAX_LEN + 1, 0, MAIN_BOARD_DUMPINFO_MAX_LEN + 1);
    // 关闭文件
    (void)fclose(fp);

    return RET_OK;
}


LOCAL gint32 process_each_main_board(OBJ_HANDLE handle, gpointer data)
{
    gint32 ret = 0;

    
    ret = get_pcb_version(handle, PROPERTY_MAINBOARD_PCBID, PROPERTY_MAINBOARD_PCBVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    return RET_OK;
}


gint32 main_board_add_object_callback(OBJ_HANDLE object_handle)
{
    return process_each_main_board(object_handle, NULL);
}


gint32 main_board_del_object_callback(OBJ_HANDLE object_handle)
{
    return RET_OK;
}


gint32 main_board_init(void)
{
    (void)dfl_foreach_object(CLASS_MAINBOARD_NAME, process_each_main_board, NULL, NULL);

    return RET_OK;
}


gint32 main_board_start(void)
{
    return RET_OK;
}
