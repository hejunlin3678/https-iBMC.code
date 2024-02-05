


#include "pme_app/pme_app.h"


#include "riser_card.h"
#include "get_version.h"

#define SN_STR_SIZE 128


LOCAL gint32 print_riser_header(FILE *fp)
{
    gchar header_info[RISER_CARD_DUMPINFO_MAX_LEN + 1] = {0};
    (void)snprintf_s(header_info, RISER_CARD_DUMPINFO_MAX_LEN + 1, RISER_CARD_DUMPINFO_MAX_LEN, "%s",
        "Riser Card Info\n");
    size_t fwrite_back = fwrite(header_info, strlen(header_info), 1, fp);
    if (fwrite_back != 1) {
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }
    (void)memset_s(header_info, RISER_CARD_DUMPINFO_MAX_LEN + 1, 0, RISER_CARD_DUMPINFO_MAX_LEN + 1);
    (void)snprintf_s(header_info, RISER_CARD_DUMPINFO_MAX_LEN + 1, RISER_CARD_DUMPINFO_MAX_LEN,
        "%-4s | %-10s | %-10s | %-32s | %-32s | %-10s | %-8s | %-32s\n",
        "Slot", "BoardId", "Name", "Manufacturer", "Type", "PartNum", "PCBVer", "SerialNum");
    fwrite_back = fwrite(header_info, strlen(header_info), 1, fp);
    if (fwrite_back != 1) {
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 print_riser_end(FILE *fp)
{
    gchar riser_end[RISER_CARD_DUMPINFO_MAX_LEN + 1] = {0};
    (void)snprintf_s(riser_end, RISER_CARD_DUMPINFO_MAX_LEN + 1, RISER_CARD_DUMPINFO_MAX_LEN, "%s", "\n\n");
    size_t fwrite_back = fwrite(riser_end, strlen(riser_end), 1, fp);
    if (fwrite_back != 1) {
        debug_log(DLOG_ERROR, "fwrite end failed!");
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL void get_riser_card_sn(OBJ_HANDLE obj_handle, gchar *serialnum, guint8 size)
{
    OBJ_HANDLE comp_handle = 0;
    gint32 ret = dal_get_specific_object_position(obj_handle, CLASS_COMPONENT, &comp_handle);
    if (ret != RET_OK || comp_handle == 0) {
        debug_log(DLOG_ERROR, "Component OBJ of %s does not exist, ret=%d", dfl_get_object_name(obj_handle), ret);
        (void)strcpy_s(serialnum, size, "N/A");
        return;
    }
    ret = dal_get_card_sn(comp_handle, serialnum, size);
    if (ret != RET_OK || strlen(serialnum) == 0) {
        debug_log(DLOG_DEBUG, "Serial Number of %s does not exist, ret=%d", dfl_get_object_name(comp_handle), ret);
        (void)strcpy_s(serialnum, size, "N/A");
        return;
    }
    return;
}


LOCAL gint32 dump_riser_card(FILE *fp, GSList *obj_list)
{
    if (print_riser_header(fp) != RET_OK) {
        return RET_ERR;
    }
    for (GSList *obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        _cleanup_gvariant_slist_full_ GSList *prop_value = NULL;
        _cleanup_gslist_ GSList *prop_list = NULL;
        prop_list = g_slist_append(prop_list, PROPETRY_RISERPCIECARD_SLOT);
        prop_list = g_slist_append(prop_list, PROPERTY_RISERPCIECARD_BOARDID);
        prop_list = g_slist_append(prop_list, PROPERTY_RISERPCIECARD_NAME);
        prop_list = g_slist_append(prop_list, PROPERTY_RISERPCIECARD_MANUFACTURER);
        prop_list = g_slist_append(prop_list, PROPERTY_RISERPCIECARD_TYPE);
        prop_list = g_slist_append(prop_list, PROPERTY_RISERPCIECARD_PART_NUMBER);
        prop_list = g_slist_append(prop_list, PROPERTY_RISERPCIECARD_PCBVER);
        gint32 ret = dfl_multiget_property_value((OBJ_HANDLE)obj_node->data, prop_list, &prop_value);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get riser card information failed!");
            return RET_ERR;
        }
        guint8 slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(prop_value, 0));
        guint16 board_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(prop_value, 1));
        const gchar *name = g_variant_get_string((GVariant *)g_slist_nth_data(prop_value, 2), 0);
        const gchar *manufacturer = g_variant_get_string((GVariant *)g_slist_nth_data(prop_value, 3), 0);
        const gchar *type = g_variant_get_string((GVariant *)g_slist_nth_data(prop_value, 4), 0);
        const gchar *partnum = g_variant_get_string((GVariant *)g_slist_nth_data(prop_value, 5), 0);
        const gchar *pcbver = g_variant_get_string((GVariant *)g_slist_nth_data(prop_value, 6), 0);
        gchar serialnum[SN_STR_SIZE] = {0};
        get_riser_card_sn((OBJ_HANDLE)obj_node->data, serialnum, sizeof(serialnum));
        gchar riser_card_info[RISER_CARD_DUMPINFO_MAX_LEN + 1] = {0};
        ret = snprintf_s(riser_card_info, sizeof(riser_card_info), sizeof(riser_card_info) - 1,
            "%-4u | 0x%-10x | %-10s | %-32s | %-32s | %-10s | %-8s | %-32s\n",
            slot_id, board_id, name, manufacturer, type, partnum, pcbver, serialnum);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        size_t fwrite_back = fwrite(riser_card_info, strlen(riser_card_info), 1, fp);
        if (fwrite_back != 1) {
            debug_log(DLOG_ERROR, "fwrite failed!");
            return RET_ERR;
        }
    }
    if (print_riser_end(fp) != RET_OK) {
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 print_ieu_header(FILE *fp)
{
    gchar header_info[RISER_CARD_DUMPINFO_MAX_LEN + 1] = {0};
    (void)snprintf_s(header_info, RISER_CARD_DUMPINFO_MAX_LEN + 1, RISER_CARD_DUMPINFO_MAX_LEN, "%s",
        "IO Extension Unit Info\n");
    size_t fwrite_back = fwrite(header_info, strlen(header_info), 1, fp);
    if (fwrite_back != 1) {
        debug_log(DLOG_ERROR, "fwrite header failed!");
        return RET_ERR;
    }
    (void)memset_s(header_info, RISER_CARD_DUMPINFO_MAX_LEN + 1, 0, RISER_CARD_DUMPINFO_MAX_LEN + 1);
    (void)snprintf_s(header_info, RISER_CARD_DUMPINFO_MAX_LEN + 1, RISER_CARD_DUMPINFO_MAX_LEN,
        "%-16s | %-10s | %-16s | %-4s | %-32s | %-32s | %-10s | %-10s | %-10s | %-10s | %-10s\n",
        "Name", "Position", "Manufacturer", "Slot", "Description", "UID", "SerialNum", "PartNum", "PCBVersion",
        "CPLDVersion", "HWSRVersion");
    fwrite_back = fwrite(header_info, strlen(header_info), 1, fp);
    if (fwrite_back != 1) {
        debug_log(DLOG_ERROR, "fwrite header failed!");
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 dump_io_extension_unit(FILE *fp, GSList *obj_list)
{
    if (print_ieu_header(fp) != RET_OK) {
        return RET_ERR;
    }
    for (GSList *obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        _cleanup_gvariant_slist_full_ GSList *prop_value = NULL;
        _cleanup_gslist_ GSList *prop_list = NULL;
        prop_list= g_slist_append(prop_list, PROPERTY_RISERPCIECARD_NAME);
        prop_list = g_slist_append(prop_list, PROPERTY_RISERPCIECARD_MANUFACTURER);
        prop_list = g_slist_append(prop_list, PROPETRY_RISERPCIECARD_SLOT);
        prop_list = g_slist_append(prop_list, PROPERTY_RISERPCIECARD_TYPE);
        prop_list = g_slist_append(prop_list, PROPERTY_COMPONENT_UNIQUE_ID);
        prop_list = g_slist_append(prop_list, PROPERTY_RISERPCIECARD_PCBVER);
        prop_list = g_slist_append(prop_list, PROPERTY_RISERPCIECARD_PART_NUMBER);
        prop_list = g_slist_append(prop_list, PROPERTY_RISERPCIECARD_LOGICVER);
        prop_list = g_slist_append(prop_list, COMMON_HWSR_VERSION);
        gint32 ret = dfl_multiget_property_value((OBJ_HANDLE)obj_node->data, prop_list, &prop_value);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get ieu information failed!");
            return RET_ERR;
        }
        const gchar *name = g_variant_get_string((GVariant *)g_slist_nth_data(prop_value, 0), 0);
        const gchar *manufacturer = g_variant_get_string((GVariant *)g_slist_nth_data(prop_value, 1), 0);
        guint8 slot =  g_variant_get_byte((GVariant *)g_slist_nth_data(prop_value, 2));
        const gchar *desc = g_variant_get_string((GVariant *)g_slist_nth_data(prop_value, 3), 0);
        const gchar *uid = g_variant_get_string((GVariant *)g_slist_nth_data(prop_value, 4), 0);
        const gchar *pcbver = g_variant_get_string((GVariant *)g_slist_nth_data(prop_value, 5), 0);
        const gchar *partnum = g_variant_get_string((GVariant *)g_slist_nth_data(prop_value, 6), 0);
        const gchar *cpldver = g_variant_get_string((GVariant *)g_slist_nth_data(prop_value, 7), 0);
        const gchar *hwsr = g_variant_get_string((GVariant *)g_slist_nth_data(prop_value, 8), 0);
        gchar serialnum[SN_STR_SIZE] = {0};
        get_riser_card_sn((OBJ_HANDLE)obj_node->data, serialnum, sizeof(serialnum));
        gchar ieu_info[RISER_CARD_DUMPINFO_MAX_LEN + 1] = {0};
        ret = snprintf_s(ieu_info, sizeof(ieu_info), sizeof(ieu_info) - 1,
            "%-16s | %-10s | %-16s | %-4u | %-32s | %-32s | %-10s | %-10s | %-10s | %-10s | %-10s\n",
            name, "-", manufacturer, slot, desc, uid, serialnum, partnum, pcbver, cpldver, hwsr);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        size_t fwrite_back = fwrite(ieu_info, strlen(ieu_info), 1, fp);
        if (fwrite_back != 1) {
            debug_log(DLOG_ERROR, "fwrite failed!");
            return RET_ERR;
        }
    }
    if (print_riser_end(fp) != RET_OK) {
        return RET_ERR;
    }
    return RET_OK;
}



gint32 riser_card_dump_info(const gchar *path)
{
    gchar file_name[RISER_CARD_DUMPINFO_MAX_LEN + 1] = {0};
    FILE *fp = NULL;
    GSList *obj_list = NULL;
    gint32 ret;
    if (path == NULL) {
        return RET_ERR;
    }
    ret = dfl_get_object_list(CLASS_RISERPCIECARD_NAME, &obj_list);
    if (ret != DFL_OK) {
        if (ERRCODE_OBJECT_NOT_EXIST == ret) {
            return RET_OK;
        }
        return RET_ERR;
    }
    // 在dump_dir目录下创建文件
    ret = snprintf_s(file_name, RISER_CARD_DUMPINFO_MAX_LEN + 1, RISER_CARD_DUMPINFO_MAX_LEN, "%s/card_info", path);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        g_slist_free(obj_list);
        return RET_ERR;
    }

    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);
    if (dal_match_product_id(PRODUCT_ID_TIANCHI) == TRUE) {
        ret = dump_io_extension_unit(fp, obj_list);
    } else {
        ret = dump_riser_card(fp, obj_list);
    }
    g_slist_free(obj_list);
    (void)fclose(fp);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dump card info failed ret=%d", ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 riser_card_set_type(OBJ_HANDLE object_handle)
{
    gint32 ret = 0;
    guint8 tpye_value = 0;
    GVariant *property_data = NULL;

    ret = dfl_get_property_value(object_handle, PROPERTY_RISERPCIECARD_TYPEVALUE, &property_data);
    if (ret != DFL_OK) {
        return RET_ERR;
    }

    tpye_value = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    
    property_data = g_variant_new_byte(tpye_value);
    ret = dfl_set_property_value(object_handle, PROPERTY_RISERPCIECARD_TYPESET, property_data, DF_HW);
    g_variant_unref(property_data);
    if (ret != DFL_OK) {
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 process_each_risercard(OBJ_HANDLE object_handle, gpointer user_data)
{
    gint32 ret = 0;

    
    ret = riser_card_set_type(object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set riser card type failed!");
    }

    
    ret = get_pcb_version(object_handle, PROPERTY_RISERPCIECARD_PCBID, PROPERTY_RISERPCIECARD_PCBVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    
    
    ret = get_logic_version(object_handle, PROPERTY_RISERPCIECARD_LOGICVERID, PROPERTY_RISERPCIECARD_LOGICVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    

    return RET_OK;
}

gint32 riser_card_add_object_callback(OBJ_HANDLE object_handle)
{
    return process_each_risercard(object_handle, NULL);
}

gint32 riser_card_del_object_callback(OBJ_HANDLE object_handle)
{
    return RET_OK;
}


gint32 riser_card_init(void)
{
    (void)dfl_foreach_object(CLASS_RISERPCIECARD_NAME, process_each_risercard, NULL, NULL);

    return RET_OK;
}


gint32 riser_card_start(void)
{
    return RET_OK;
}
