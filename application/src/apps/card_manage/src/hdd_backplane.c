



#include "pme_app/pme_app.h"

#include "hdd_backplane.h"
#include "get_version.h"

static guint8 g_HDD_Event_Support_Flag = HDD_EVENT_UNSUPPORT;
static void destroy_hddbackplane_private_data_func(gpointer data);

gint32 hdd_backplane_dump_info(const gchar *path)
{
    gint32 iRet = -1;
    gint32 ret = 0;
    guint8 slot_id = 0;
    guint16 board_id = 0;
    const gchar *name = NULL;
    const gchar *pcb_ver = NULL;
    const gchar *logic_ver = NULL;
    const gchar *manufacturer = NULL;
    const gchar *type = NULL;
    const gchar *partnum = NULL;
    gchar file_name[HDD_BACKPLANE_DUMPINFO_MAX_LEN + 1] = {0};
    gchar hdd_backplane_info[HDD_BACKPLANE_DUMPINFO_MAX_LEN + 1] = {0};
    FILE *fp = NULL;

    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    size_t fwrite_back = 0;

    if (path == NULL) {
        return RET_ERR;
    }

    ret = dfl_get_object_list(CLASS_HDDBACKPLANE_NAME, &obj_list);
    if (ret != DFL_OK) {
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            return RET_OK;
        }
        return RET_ERR;
    }
    
    // 在dump_dir目录下创建文件
    iRet =
        snprintf_s(file_name, HDD_BACKPLANE_DUMPINFO_MAX_LEN + 1, HDD_BACKPLANE_DUMPINFO_MAX_LEN, "%s/card_info", path);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        g_slist_free(obj_list);
        return RET_ERR;
    }
    
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);
    

    (void)snprintf_s(hdd_backplane_info, HDD_BACKPLANE_DUMPINFO_MAX_LEN + 1, HDD_BACKPLANE_DUMPINFO_MAX_LEN, "%s",
        "HDD Backplane Info\n");
    fwrite_back = fwrite(hdd_backplane_info, strlen(hdd_backplane_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    (void)memset_s(hdd_backplane_info, HDD_BACKPLANE_DUMPINFO_MAX_LEN + 1, 0, HDD_BACKPLANE_DUMPINFO_MAX_LEN + 1);

    (void)snprintf_s(hdd_backplane_info, HDD_BACKPLANE_DUMPINFO_MAX_LEN + 1, HDD_BACKPLANE_DUMPINFO_MAX_LEN,
        "%-4s | %-10s | %-10s | %-32s | %-10s | %-10s | %-40s | %-8s\n", "Slot", "BoardId", "Name", "Manufacturer",
        "PCB Ver", "Logic Ver", "Type", "Part Num");
    fwrite_back = fwrite(hdd_backplane_info, strlen(hdd_backplane_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        property_name_list = g_slist_append(property_name_list, PROPETRY_HDDBACKPLANE_SLOT);
        property_name_list = g_slist_append(property_name_list, PROPERTY_HDDBACKPLANE_BOARDID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_HDDBACKPLANE_NAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_HDDBACKPLANE_MANUFACTURER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_HDDBACKPLANE_PCBVER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_HDDBACKPLANE_LOGICVER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_HDDBACKPLANE_TYPE);
        // 201705108246 支持PARTNUM带出 20190723 RAOJIANZHONG 00267466
        property_name_list = g_slist_append(property_name_list, PROPERTY_HDDBACKPLANE_PART_NUMBER);
        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value);
        g_slist_free(property_name_list);
        property_name_list = NULL;
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get hdd backplane information failed!");
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
        type = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 6), 0);
        // 201705108246 支持PARTNUM带出 20190723 RAOJIANZHONG 00267466
        partnum = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 7), 0);

        (void)memset_s(hdd_backplane_info, HDD_BACKPLANE_DUMPINFO_MAX_LEN + 1, 0, HDD_BACKPLANE_DUMPINFO_MAX_LEN + 1);
        iRet = snprintf_s(hdd_backplane_info, sizeof(hdd_backplane_info), sizeof(hdd_backplane_info) - 1,
            "%-4u | 0x%-4x     | %-10s | %-32s | %-10s | %-10s | %-40s | %-8s \n", slot_id, board_id, name,
            manufacturer, pcb_ver, logic_ver, type, partnum);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        }
        fwrite_back = fwrite(hdd_backplane_info, strlen(hdd_backplane_info), 1, fp);
        if (fwrite_back != 1) {
            (void)fclose(fp);
            debug_log(DLOG_ERROR, "fwrite failed!");
            g_slist_free(obj_list);
            g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
            return RET_ERR;
        }
        (void)memset_s(hdd_backplane_info, HDD_BACKPLANE_DUMPINFO_MAX_LEN + 1, 0, HDD_BACKPLANE_DUMPINFO_MAX_LEN + 1);
        g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
        property_value = NULL;
    }

    g_slist_free(obj_list);
    (void)snprintf_s(hdd_backplane_info, HDD_BACKPLANE_DUMPINFO_MAX_LEN + 1, HDD_BACKPLANE_DUMPINFO_MAX_LEN, "%s",
        "\n\n");
    
    fwrite_back = fwrite(hdd_backplane_info, strlen(hdd_backplane_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }
    (void)memset_s(hdd_backplane_info, HDD_BACKPLANE_DUMPINFO_MAX_LEN + 1, 0, HDD_BACKPLANE_DUMPINFO_MAX_LEN + 1);
    // 关闭文件
    (void)fclose(fp);

    return RET_OK;
}


LOCAL gint32 process_each_hdd_backplane(OBJ_HANDLE handle, gpointer data)
{
    gint32 ret = 0;

    
    ret = get_pcb_version(handle, PROPERTY_HDDBACKPLANE_PCBID, PROPERTY_HDDBACKPLANE_PCBVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    
    ret = get_logic_version(handle, PROPERTY_HDDBACKPLANE_LOGICVERID, PROPERTY_HDDBACKPLANE_LOGICVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    return RET_OK;
}

void hdd_backplane_updata_card_info(void)
{
    (void)dfl_foreach_object(CLASS_HDDBACKPLANE_NAME, process_each_hdd_backplane, NULL, NULL);
    return;
}



LOCAL guint8 hdd_get_hdd_event_support_flag(void)
{
    return g_HDD_Event_Support_Flag;
}


LOCAL void hdd_set_hdd_event_support_flag(guint8 flag)
{
    g_HDD_Event_Support_Flag = flag;
}


LOCAL void hdd_check_monitor_plugevent(void)
{
    gint32 ret = RET_ERR;
    GSList *ObjHandleList = NULL;
    GSList *Node = NULL;
    OBJ_HANDLE Object = 0;
    guint8 OwnSlotId = HDD_OWNSLOTID_INIT;

    
    ret = dfl_get_object_list(CLASS_HDD_NAME, &ObjHandleList);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get %s list fail: %d.", CLASS_HDD_NAME, ret);
        return;
    }
    hdd_set_hdd_event_support_flag(HDD_EVENT_UNSUPPORT);
    for (Node = ObjHandleList; Node; Node = Node->next) {
        Object = (OBJ_HANDLE)Node->data;
        (void)dal_get_property_value_byte(Object, PROPERTY_HDD_OWN_SLOT_ID, &OwnSlotId);
        if (OwnSlotId != HDD_OWNSLOTID_INIT) {
            hdd_set_hdd_event_support_flag(HDD_EVENT_SUPPORT);
            break;
        }
    }
    g_slist_free(ObjHandleList);

    return;
}


LOCAL gint32 hdd_get_count_by_class_and_property(const gchar *class_name, const gchar *property_name, guint8 planeid,
    guint8 *count)
{
    gint32 ret = RET_ERR;
    guint8 idcount = 0;
    GSList *objHandleList = NULL;
    GSList *node = NULL;

    OBJ_HANDLE object = 0;
    guint8 id = 0;

    if ((count == NULL) || (class_name == NULL) || (property_name == NULL)) {
        return RET_ERR;
    }

    
    ret = dfl_get_object_list(class_name, &objHandleList);
    if (ret != DFL_OK) {
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            *count = 0;
            return RET_OK;
        } else {
            debug_log(DLOG_ERROR, "Get %s fail: %d.", class_name, ret);
            return RET_ERR;
        }
    }
    for (node = objHandleList; node; node = node->next) {
        object = (OBJ_HANDLE)node->data;
        ret = dal_get_property_value_byte(object, property_name, &id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "get %s of %s fail: %d.", property_name, class_name, ret);
            continue;
        }
        if (planeid == id) {
            idcount++;
        }
    }
    g_slist_free(objHandleList);
    *count = idcount;

    return RET_OK;
}


LOCAL gint32 hdd_get_backplane_slot_count(guint8 planeid, guint8 *slotcnt)
{
    gint32 ret = RET_ERR;
    guint8 count = 0;
    guint8 hddcount = 0;
    guint8 sddcount = 0;

    if (slotcnt == NULL) {
        return RET_ERR;
    }
    ret = hdd_get_count_by_class_and_property(CLASS_HDD_NAME, PROPERTY_HDD_PLANE_ID, planeid, &hddcount);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get hdd object count fail: %d", ret);
        return RET_ERR;
    }
    debug_log(DLOG_ERROR, "the planeid: %d, hddcount: %d", planeid, hddcount);
    ret = hdd_get_count_by_class_and_property(CLASS_DISK_SLOT_PST_INFO, PROPERTY_DISK_PLANE_ID, planeid, &sddcount);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get diskslotpst object count fail: %d", ret);
        return RET_ERR;
    }
    debug_log(DLOG_ERROR, "the planeid: %d, sddcount: %d", planeid, sddcount);

    count = hddcount + sddcount;

    *slotcnt = count;
    return RET_OK;
}


LOCAL gint32 hdd_init_each_object(OBJ_HANDLE handle, gpointer data)
{
    gint32 ret = RET_ERR;
    guint8 slotid = 0;
    guint8 slotcnt = 0;
    HDD_BACKPLANE_INFO_S *hddbackplanepriv = NULL;

    (void)data;
    ret = dal_get_property_value_byte(handle, PROPETRY_HDDBACKPLANE_SLOT, &slotid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get hddbackplane slot fail: %d", ret);
        return RET_OK;
    }

    ret = hdd_get_backplane_slot_count(slotid, &slotcnt);
    debug_log(DLOG_ERROR, "The planeid: %d, conut: %d", slotid, slotcnt);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get hddbackplane slot count fail: %d", ret);
        return RET_OK;
    }

    if (slotcnt == 0) {
        debug_log(DLOG_ERROR, "slot count err.");
        return RET_OK;
    }

    
    hddbackplanepriv = (HDD_BACKPLANE_INFO_S *)g_malloc0(sizeof(HDD_BACKPLANE_INFO_S));
    if (hddbackplanepriv == NULL) {
        debug_log(DLOG_ERROR, "malloc fail.");
        return RET_OK;
    }

    hddbackplanepriv->DiskCnt = slotcnt;
    hddbackplanepriv->PlaneId = slotid;
    hddbackplanepriv->LastPst = (guint8 *)g_malloc0(sizeof(guint8) * slotcnt);
    if (hddbackplanepriv->LastPst == NULL) {
        goto INIT_FAIL;
    }
    hddbackplanepriv->CurPst = (guint8 *)g_malloc0(sizeof(guint8) * slotcnt);
    if (hddbackplanepriv->CurPst == NULL) {
        goto INIT_FAIL;
    }
    hddbackplanepriv->PlugInCnt = (guint8 *)g_malloc0(sizeof(guint8) * slotcnt);
    if (hddbackplanepriv->PlugInCnt == NULL) {
        goto INIT_FAIL;
    }
    hddbackplanepriv->PlugOutCnt = (guint8 *)g_malloc0(sizeof(guint8) * slotcnt);
    if (hddbackplanepriv->PlugOutCnt == NULL) {
        goto INIT_FAIL;
    }
    
    
    ret = dfl_bind_object_data(handle, (gpointer)hddbackplanepriv, destroy_hddbackplane_private_data_func);
    
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_bind_object_data fail: %d", ret);
        goto INIT_FAIL;
    } else {
        return RET_OK;
    }
INIT_FAIL:
    debug_log(DLOG_ERROR, "malloc fail.");
    if (hddbackplanepriv->PlugOutCnt != NULL) {
        g_free(hddbackplanepriv->PlugOutCnt);
        hddbackplanepriv->PlugOutCnt = NULL;
    }
    if (hddbackplanepriv->CurPst != NULL) {
        g_free(hddbackplanepriv->CurPst);
        hddbackplanepriv->CurPst = NULL;
    }
    if (hddbackplanepriv->LastPst != NULL) {
        g_free(hddbackplanepriv->LastPst);
        hddbackplanepriv->LastPst = NULL;
    }
    if (hddbackplanepriv->PlugInCnt != NULL) {
        g_free(hddbackplanepriv->PlugInCnt);
        hddbackplanepriv->PlugInCnt = NULL;
    }
    if (hddbackplanepriv != NULL) {
        g_free(hddbackplanepriv);
    }

    return RET_ERR;
}




gint32 hdd_backplane_init(void)
{
    (void)dfl_foreach_object(CLASS_HDDBACKPLANE_NAME, process_each_hdd_backplane, NULL, NULL);
    
    hdd_check_monitor_plugevent();
    if (hdd_get_hdd_event_support_flag() == HDD_EVENT_SUPPORT) {
        (void)dfl_foreach_object(CLASS_HDDBACKPLANE_NAME, hdd_init_each_object, NULL, NULL);
    }
    
    return RET_OK;
}




LOCAL gint32 _hdd_backplane_update_plug_event_by_class(HDD_BACKPLANE_INFO_S *HddBackPlanePriv, gchar *class_name,
    const gchar *slot_property_name, const gchar *plane_property_name, const gchar *pst_property_name)
{
    gint32 Ret = RET_ERR;
    GSList *HddObjList = NULL;
    GSList *Node = NULL;
    OBJ_HANDLE HddObjectHandle = 0;
    guint8 OwnSlotId = 0xff;
    guint8 PlaneId = 0xff;
    guint8 Pst = 0;

    Ret = dfl_get_object_list(class_name, &HddObjList);
    if (Ret != DFL_OK) {
        if (Ret == ERRCODE_OBJECT_NOT_EXIST) {
            return RET_OK;
        } else {
            debug_log(DLOG_ERROR, "Get %s fail: %d", class_name, Ret);
            return RET_ERR;
        }
    }
    for (Node = HddObjList; Node; Node = Node->next) {
        HddObjectHandle = (OBJ_HANDLE)Node->data;
        (void)dal_get_property_value_byte(HddObjectHandle, slot_property_name, &OwnSlotId);
        (void)dal_get_property_value_byte(HddObjectHandle, plane_property_name, &PlaneId);
        (void)dal_get_property_value_byte(HddObjectHandle, pst_property_name, &Pst);

        if ((HddBackPlanePriv->PlaneId != PlaneId) || (OwnSlotId == HDD_OWNSLOTID_INIT)) {
            continue;
        }
        if (OwnSlotId >= HddBackPlanePriv->DiskCnt) {
            continue;
        }
        HddBackPlanePriv->CurPst[OwnSlotId] = Pst;
        if (HddBackPlanePriv->CurPst[OwnSlotId] != HddBackPlanePriv->LastPst[OwnSlotId]) {
            if (HddBackPlanePriv->CurPst[OwnSlotId] == DISK_PRESENT) {
                
                HddBackPlanePriv->PlugInCnt[OwnSlotId]++;
            } else {
                
                HddBackPlanePriv->PlugOutCnt[OwnSlotId]++;
            }
            HddBackPlanePriv->LastPst[OwnSlotId] = HddBackPlanePriv->CurPst[OwnSlotId];
        }
    }
    g_slist_free(HddObjList);

    return RET_OK;
}


LOCAL gint32 hdd_backplane_collect_plug_event(OBJ_HANDLE handle, gpointer data)
{
    HDD_BACKPLANE_INFO_S *HddBackPlanePriv = NULL;
    gint32 Ret = RET_ERR;

    Ret = dfl_get_binded_data(handle, (void **)(&HddBackPlanePriv));
    if (Ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get hddbackplane private fail: %d", Ret);
        return RET_ERR;
    }
    Ret = _hdd_backplane_update_plug_event_by_class(HddBackPlanePriv, CLASS_HDD_NAME, PROPERTY_HDD_OWN_SLOT_ID,
        PROPERTY_HDD_PLANE_ID, PROPERTY_HDD_PRESENCE);
    if (Ret != RET_OK) {
        debug_log(DLOG_ERROR, "update current pst according to Hdd fail: %d", Ret);
        return RET_ERR;
    }
    Ret = _hdd_backplane_update_plug_event_by_class(HddBackPlanePriv, CLASS_DISK_SLOT_PST_INFO,
        PROPERTY_DISK_OWN_SLOT_ID, PROPERTY_DISK_PLANE_ID, PROPERTY_DISK_CONNECTOR_PRESENT);
    if (Ret != RET_OK) {
        debug_log(DLOG_ERROR, "update current pst according to diskslotpst fail: %d", Ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL void hdd_backplane_disk_plug_event_task(void)
{
    (void)prctl(PR_SET_NAME, (gulong) "HddPlugTask");

    for (;;) {
        vos_task_delay(1000);
        (void)dfl_foreach_object(CLASS_HDDBACKPLANE_NAME, hdd_backplane_collect_plug_event, NULL, NULL);
    }
}


gint32 hdd_backplane_add_object_callback(OBJ_HANDLE object_handle)
{
    gint32 ret = 0;
    card_manage_log_operation_log("Hdd backplane", object_handle, PROPETRY_HDDBACKPLANE_SLOT, NULL, CARD_PLUG_IN);
    ret = process_each_hdd_backplane(object_handle, NULL);
    
    if (hdd_get_hdd_event_support_flag() == HDD_EVENT_SUPPORT) {
        ret = hdd_init_each_object(object_handle, NULL);
    }
    
    return ret;
}


static void destroy_hddbackplane_private_data_func(gpointer data)
{
    HDD_BACKPLANE_INFO_S *hddbackplanepriv = NULL;

    if (data == NULL) {
        debug_log(DLOG_ERROR, "%s:%d Input pointer is NULL.", __FUNCTION__, __LINE__);
        return;
    }

    hddbackplanepriv = (HDD_BACKPLANE_INFO_S *)data;

    if (hddbackplanepriv->PlugOutCnt != NULL) {
        g_free(hddbackplanepriv->PlugOutCnt);
        hddbackplanepriv->PlugOutCnt = NULL;
    }
    if (hddbackplanepriv->LastPst != NULL) {
        g_free(hddbackplanepriv->LastPst);
        hddbackplanepriv->LastPst = NULL;
    }
    if (hddbackplanepriv->CurPst != NULL) {
        g_free(hddbackplanepriv->CurPst);
        hddbackplanepriv->CurPst = NULL;
    }
    if (hddbackplanepriv->PlugInCnt != NULL) {
        g_free(hddbackplanepriv->PlugInCnt);
        hddbackplanepriv->PlugInCnt = NULL;
    }

    g_free(hddbackplanepriv);
}


gint32 hdd_backplane_del_object_callback(OBJ_HANDLE object_handle)
{
    card_manage_log_operation_log("Hdd backplane", object_handle, PROPETRY_HDDBACKPLANE_SLOT, NULL, CARD_PLUG_OUT);

    return RET_OK;
}




gint32 hdd_backplane_start(void)
{
    gulong ret = 0;
    gulong task_get_disk_plug_event_id = 0;

    
    if (hdd_get_hdd_event_support_flag() == HDD_EVENT_SUPPORT) {
        ret = vos_task_create(&task_get_disk_plug_event_id, "task_get_disk_plug_event",
            (TASK_ENTRY)hdd_backplane_disk_plug_event_task, 0, DEFAULT_PRIORITY);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Creat task_get_disk_plug_event task failed!");
            return RET_ERR;
        }
    }
    
    return RET_OK;
}



LOCAL gint32 _backplane_slot_compare(gconstpointer a, gconstpointer b)
{
    const HDD_BACKPLANE_INFO_S *connector_a = (const HDD_BACKPLANE_INFO_S *)a;
    const HDD_BACKPLANE_INFO_S *connector_b = (const HDD_BACKPLANE_INFO_S *)b;

    return (connector_a->PlaneId > connector_b->PlaneId) ? TRUE : FALSE;
}


gint32 ipmi_command_get_hdd_plug_cnt(const void *msg_data, gpointer user_data)
{
#define RSP_LEN 128
    gint32 ret = 0;
    const guint8 *req_data = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint32 manufactureid = 0;
    guint32 imana = 0;
    guint8 resp_data[RSP_LEN] = {0};
    guint32 count = 0;
    GSList *list = NULL;
    guint8 destlen = 0;
    HDD_BACKPLANE_INFO_S *HddBackPlanePriv = NULL;
    guint8 i = 0;
    GSList *HddObjList = NULL;
    GSList *Node = NULL;
    OBJ_HANDLE HddObjectHandle = 0;
    guint8 len = 0;
    errno_t securec_rv;

    
    if (msg_data == NULL) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    
    req_data = get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    
    ret = dfl_get_object_handle(BMC_OBJECT_NAME, &obj_handle);
    if (ret != DFL_OK) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_CANNOT_RESPONSE);
    }

    ret = dal_get_property_value_uint32(obj_handle, BMC_MANU_ID_NAME, &manufactureid);
    if (ret != RET_OK) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_CANNOT_RESPONSE);
    }

    
    imana = MAKE_DWORD(0, req_data[2], req_data[1], req_data[0]);
    if (manufactureid != imana) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }
    if (hdd_get_hdd_event_support_flag() != HDD_EVENT_SUPPORT) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_CANNOT_RESPONSE);
    }
    if ((req_data[4] != PLUGIN_EVENT) && (req_data[4] != PLUGOUT_EVENT)) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    ret = dfl_get_object_count(CLASS_HDDBACKPLANE_NAME, &count);
    if (ret != DFL_OK) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_CANNOT_RESPONSE);
    }

    resp_data[0] = COMP_CODE_SUCCESS;
    resp_data[1] = LONGB0(manufactureid);
    resp_data[2] = LONGB1(manufactureid);
    resp_data[3] = LONGB2(manufactureid);
    resp_data[4] = (guint8)(count);

    ret = dfl_get_object_list(CLASS_HDDBACKPLANE_NAME, &HddObjList);
    if (ret != DFL_OK) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_CANNOT_RESPONSE);
    }
    for (Node = HddObjList; Node; Node = Node->next) {
        HddObjectHandle = (OBJ_HANDLE)Node->data;
        ret = dfl_get_binded_data(HddObjectHandle, (void **)(&HddBackPlanePriv));
        if (ret != DFL_OK) {
            g_slist_free(HddObjList);
            return ipmi_send_simple_response(msg_data, COMP_CODE_CANNOT_RESPONSE);
        }
        list = g_slist_insert_sorted(list, HddBackPlanePriv, _backplane_slot_compare);
    }
    g_slist_free(HddObjList);
    i = 5;
    len = 5;
    for (Node = list; Node; Node = Node->next) {
        HddBackPlanePriv = (HDD_BACKPLANE_INFO_S *)Node->data;
        resp_data[i] = HddBackPlanePriv->DiskCnt;
        destlen = RSP_LEN - (i + 1);
        
        if (req_data[4] == PLUGIN_EVENT) {
            securec_rv = memcpy_s(&resp_data[++i], destlen, HddBackPlanePriv->PlugInCnt, HddBackPlanePriv->DiskCnt);
            if (securec_rv != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, securec_rv);
            }
        } else {
            securec_rv = memcpy_s(&resp_data[++i], destlen, HddBackPlanePriv->PlugOutCnt, HddBackPlanePriv->DiskCnt);
            if (securec_rv != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, securec_rv);
            }
        }

        i = i + HddBackPlanePriv->DiskCnt;
        len = len + 1 + HddBackPlanePriv->DiskCnt;
    }
    g_slist_free(list);

    return ipmi_send_response(msg_data, len, resp_data);
}



gint32 hdd_backplane_power_ctrl(const guint16 device_num, const guint8 cmd_code, guint8 *comp_code)
{
    gint32 ret = 0;
    GSList *bp_list = NULL;
    GSList *bp_node = NULL;
    gchar               accesor_name[ACCESSOR_NAME_LEN] = {0};

    
    if (device_num != 0xffff || ((POWER_CONTROL_CMD_POWER_OFF != cmd_code) && (POWER_CONTROL_CMD_NORMAL != cmd_code))) {
        *comp_code = COMP_CODE_OUTOF_RANGE;
        return RET_ERR;
    }

    ret = dfl_get_object_list(CLASS_HDDBACKPLANE_NAME, &bp_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_object_list failed(%d)", ret);
        *comp_code = COMP_CODE_UNKNOWN;
        return RET_ERR;
    }

    for (bp_node = bp_list; bp_node; bp_node = g_slist_next(bp_node)) {
        
        ret = dfl_get_property_accessor((OBJ_HANDLE)bp_node->data, PROPERTY_HDDBACKPLANE_DISKS_POWERCTRL, accesor_name,
            ACCESSOR_NAME_LEN);
        if (ret != DFL_OK) {
            continue;
        }

        
        ret = dal_set_property_value_byte((OBJ_HANDLE)bp_node->data, PROPERTY_HDDBACKPLANE_DISKS_POWERCTRL, cmd_code,
            DF_HW);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "dal_set_property_value_byte failed(%d), obj(%s)", ret,
                dfl_get_object_name((OBJ_HANDLE)bp_node->data));
            *comp_code = COMP_CODE_UNKNOWN;
            break;
        }

        *comp_code = COMP_CODE_SUCCESS;
    }

    g_slist_free(bp_list);

    return ret;
}
