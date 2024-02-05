

#include "pme_app/pme_app.h"


#include "get_version.h"

#define GET_PCBVER_COUNT 10

#define LOGICVER_GET_MAX_RETRY_TIME 10


LOCAL gint32 set_logic_version(OBJ_HANDLE handle, const gchar *property_logic_ver, guint8 ver_id)
{
    gint32 ret;
    gchar version_data[CARD_LOGIC_VER_LEN + 1] = { 0 };

    ret = snprintf_s(version_data, CARD_LOGIC_VER_LEN + 1,
        CARD_LOGIC_VER_LEN, "%d.%02d", (ver_id >> 4), (ver_id & 0xF)); 
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "[%s]: snprintf_s failed, ret: %d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    return dal_set_property_value_string(handle, property_logic_ver, version_data, DF_NONE);
}


LOCAL void get_again_logicver(LOGICVER_INFO *logicver_info_p)
{
    gint32 ret;
    guint8 ver_id = 0;

    if (logicver_info_p == NULL) {
        debug_log(DLOG_ERROR, "%s: data is NULL.", __FUNCTION__);
        return;
    }

    for (gint8 i = 0; i < LOGICVER_GET_MAX_RETRY_TIME; i++) {
        ret = dal_get_extern_value_byte(logicver_info_p->obj_handle, logicver_info_p->property_logic_id,
            &ver_id, DF_COPY);
        if (ret == RET_OK) {
            break;
        }
        vos_task_delay(1000); 
    }
 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get %s logic_id failed! ret=%d.", __FUNCTION__,
            dfl_get_object_name(logicver_info_p->obj_handle), ret);
    } else if (ver_id != 0) {
        ret = set_logic_version(logicver_info_p->obj_handle, logicver_info_p->property_logic_ver, ver_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Set %s %s failed, verId: %d, ret: %d.",
                dfl_get_object_name(logicver_info_p->obj_handle), logicver_info_p->property_logic_ver, ver_id, ret);
        }
    }
    g_free(logicver_info_p);
    return;
}

gint32 get_logic_version(OBJ_HANDLE handle, const gchar *property_logic_id, const gchar *property_logic_ver)
{
    gint32 ret = 0;
    GVariant *property_value = NULL;
    guint8 ver_id = 0;
    LOGICVER_INFO* logicver_info_s = NULL;
    TASKID get_logic_ver_thread_id = 0;

    
    ret = dfl_get_extern_value(handle, property_logic_id, &property_value, DF_COPY);
    if (ret != DFL_OK) {
        logicver_info_s = (LOGICVER_INFO *)g_malloc0(sizeof(LOGICVER_INFO));
        if (logicver_info_s == NULL) {
            debug_log(DLOG_ERROR, "%s: malloc fail.", __FUNCTION__);
            return RET_ERR;
        }
        logicver_info_s->obj_handle = handle;
        (void)strncpy_s(logicver_info_s->property_logic_id, sizeof(logicver_info_s->property_logic_id),
            property_logic_id, sizeof(logicver_info_s->property_logic_id) - 1);
        (void)strncpy_s(logicver_info_s->property_logic_ver, sizeof(logicver_info_s->property_logic_ver),
            property_logic_ver, sizeof(logicver_info_s->property_logic_ver) - 1);

        ret = vos_task_create(&get_logic_ver_thread_id, "logicver", (TASK_ENTRY)get_again_logicver,
            (void *)logicver_info_s, DEFAULT_PRIORITY);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Get %s logic_id failed, and creat retry task failed, ret=%d.",
                __FUNCTION__, dfl_get_object_name(handle), ret);
            g_free(logicver_info_s);
            return RET_ERR;
        } else {
            return RET_OK;
        }
    }

    ver_id = g_variant_get_byte(property_value);
    g_variant_unref(property_value);

    if (ver_id != 0) {
        ret = set_logic_version(handle, property_logic_ver, ver_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Set %s %s failed!\n", dfl_get_object_name(handle), property_logic_ver);
            return RET_ERR;
        }
    }
    return RET_OK;
}


LOCAL gint32 get_pcbver_by_pcbid(OBJ_HANDLE handle, const gchar *property_pcb_id, const gchar *property_pcb_ver)
{
    gint32 ret = 0;
    guint8 pcb_id = 0;
    gchar pcb_ver[CARD_PCB_VER_LEN + 1] = {0};
    gchar obj_name[MAX_NAME_SIZE] = {0};
    gchar property_name[MAX_NAME_SIZE] = {0};

    if ((property_pcb_id == NULL) || (property_pcb_ver == NULL)) {
        debug_log(DLOG_ERROR, "%s: Get  pcb id failed! data is NULL.", __FUNCTION__);
        return RET_ERR;
    }
    ret = dfl_get_referenced_property(handle, property_pcb_ver, obj_name, property_name,
        MAX_NAME_SIZE, MAX_NAME_SIZE);
    
    if (ret == RET_OK) {
        return RET_OK;
    }

    ret = dal_get_extern_value_byte(handle, property_pcb_id, &pcb_id, DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: Get %s pcb id failed.", __FUNCTION__, dfl_get_object_name(handle));
        return RET_ERR;
    }

    
    if (pcb_id + 'A' - 0x01 > 'Z') {
        debug_log(DLOG_ERROR, "%s: %s pcb id(%d) is out of range.", __FUNCTION__, dfl_get_object_name(handle), pcb_id);
        return RET_OK;
    }

    (void)snprintf_s(pcb_ver, CARD_PCB_VER_LEN + 1, CARD_PCB_VER_LEN, "%s%c", ".",
        (gchar)((pcb_id == 0) ? 'A' : (pcb_id + 'A' - 0x01)));

    ret = dal_set_property_value_string(handle, property_pcb_ver, pcb_ver, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Set %s pcb version failed! ret=%d.", __FUNCTION__, dfl_get_object_name(handle),
            ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL void get_again_pcbver(void *data)
{
    gint32 i = 0;
    gint32 ret = 0;
    PCBVER_INFO *pcbver_info_p = NULL;

    if (data == NULL) {
        debug_log(DLOG_ERROR, "%s: Get  pcb id failed! data is NULL.", __FUNCTION__);
        return;
    }

    pcbver_info_p = (PCBVER_INFO *)data;

    for (i = 0; i < GET_PCBVER_COUNT; i++) {
        ret = get_pcbver_by_pcbid(pcbver_info_p->obj_handle, pcbver_info_p->property_pcb_id,
            pcbver_info_p->property_pcb_ver);
        if (ret == RET_OK) {
            break;
        }

        vos_task_delay(1000);
    }

    if (i == GET_PCBVER_COUNT) {
        debug_log(DLOG_ERROR, "%s: Get %s pcb id failed! ret=%d.", __FUNCTION__,
            dfl_get_object_name(pcbver_info_p->obj_handle), ret);
    }
    g_free(pcbver_info_p);
    return;
}


gint32 get_pcb_version(OBJ_HANDLE handle, const gchar *property_pcb_id, const gchar *property_pcb_ver)
{
    gint32 ret = 0;
    PCBVER_INFO *pcbver_info_s = NULL;
    TASKID get_pcbver_thread_id = 0;

    ret = get_pcbver_by_pcbid(handle, property_pcb_id, property_pcb_ver);
    if (ret != RET_OK) {
        pcbver_info_s = (PCBVER_INFO *)g_malloc0(sizeof(PCBVER_INFO));
        if (pcbver_info_s == NULL) {
            debug_log(DLOG_ERROR, "%s: pcbver_info_s g_malloc0 fail.", __FUNCTION__);
            return RET_ERR;
        }

        pcbver_info_s->obj_handle = handle;
        (void)strncpy_s(pcbver_info_s->property_pcb_id, sizeof(pcbver_info_s->property_pcb_id), property_pcb_id,
            sizeof(pcbver_info_s->property_pcb_id) - 1);
        (void)strncpy_s(pcbver_info_s->property_pcb_ver, sizeof(pcbver_info_s->property_pcb_ver), property_pcb_ver,
            sizeof(pcbver_info_s->property_pcb_ver) - 1);

        ret = vos_task_create(&get_pcbver_thread_id, "pcbver", (TASK_ENTRY)get_again_pcbver, pcbver_info_s,
            DEFAULT_PRIORITY);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Get %s pcb id failed! and creat task failed ret=%d.", __FUNCTION__,
                dfl_get_object_name(handle), ret);
            g_free(pcbver_info_s);
            return RET_ERR;
        } else {
            debug_log(DLOG_INFO, "%s: Get %s First pcb id failed! and creat task successed ret=%d.", __FUNCTION__,
                dfl_get_object_name(handle), ret);
            return RET_OK;
        }
    }

    return RET_OK;
}

void card_manage_log_operation_log(gchar *card_name, OBJ_HANDLE object_handle, const gchar *property_name,
    const gchar *device_name_property, guint8 option)
{
    gint32 iRet = -1;
    gint32 ret = 0;
    guint8 slot = 0;
    GVariant *property_value = NULL;
    GSList *caller_info_list = NULL;
    gchar devicename[PROP_VAL_LENGTH] = {0};
    gchar board_name[PROP_VAL_LENGTH*2] = {0};

    if (card_name == NULL || property_name == NULL) {
        return;
    }

    ret = dfl_get_property_value(object_handle, property_name, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "card_manage_log_operation_log: get prop error, ret is %d", ret);
        return;
    }
    slot = g_variant_get_byte(property_value);
    g_variant_unref(property_value);

    
    if (device_name_property != NULL) {
        ret = dal_get_property_value_string(object_handle, device_name_property, devicename, sizeof(devicename));
        if ((ret == RET_OK) && ((strlen(devicename)) != 0)) {
            iRet = snprintf_s(board_name, sizeof(board_name), sizeof(board_name) - 1, "%s%u (%s)", card_name, slot,
                devicename);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
            }
        } else {
            iRet = snprintf_s(board_name, sizeof(board_name), sizeof(board_name) - 1, "%s%u", card_name, slot);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
            }
        }
    } else {
        iRet = snprintf_s(board_name, sizeof(board_name), sizeof(board_name) - 1, "%s%u", card_name, slot);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        }
    }
    
    caller_info_list = g_slist_append(caller_info_list, g_variant_new_string("N/A"));
    caller_info_list = g_slist_append(caller_info_list, g_variant_new_string("N/A"));
    caller_info_list = g_slist_append(caller_info_list, g_variant_new_string("127.0.0.1"));

    switch (option) {
        case CARD_PLUG_IN:
        case CARD_PLUG_OUT:
            method_operation_log(caller_info_list, NULL, "%s %s successfully\n", board_name,
                (option == CARD_PLUG_IN) ? "plug in" : "plug out");
            break;

        case CARD_BUTTON_PRESS:
            method_operation_log(caller_info_list, NULL, "%s button pressed successfully\n", board_name);
            break;

        default:
            debug_log(DLOG_ERROR, "card_manage_log_operation_log: option(%d) is out of range!", option);
            break;
    }
    g_slist_free_full(caller_info_list, (GDestroyNotify)g_variant_unref);
    return;
}
