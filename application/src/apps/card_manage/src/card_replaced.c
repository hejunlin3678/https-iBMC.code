
#include "pme_app/pme_app.h"
#include "card_manage_module.h"

#define CARD_REPLACE_DETECT_DELAY (3 * 60 * 1000)
#define CARD_REPLACE_DETECT_PERIOD (60 * 1000)

typedef struct _CARD_REPLACED_INFO_ {
    OBJ_HANDLE card_handle;
    gchar cur_sn[TMP_STR_LEN];
    gchar pre_sn[TMP_STR_LEN];
} CARD_REPLACED_INFO;

typedef gint32 (*pfn_get_replace_info)(OBJ_HANDLE component_handle, gchar *classname, gchar *buff, gint32 length);
typedef void (*pfn_generate_replace_event)(CARD_REPLACED_INFO *replace_info);

typedef struct _CARD_REPLACED_ {
    guint8 device_type;                        // 设备类型
    gchar *class_name;                         // 设备类型对应的类名称
    pfn_get_replace_info get_sn;               // 当前类型板卡获取SN的方法
    pfn_generate_replace_event generate_event; // 事件生成回调方法，主要用于不声明在Card模块声明的板卡
    pfn_generate_replace_event update_info;
} CARD_REPLACED_CFG;

LOCAL gint32 get_elabel_boardserialnumber(OBJ_HANDLE component_handle, gchar *classname, gchar *buff, gint32 length);
LOCAL gint32 get_card_object_sn(OBJ_HANDLE component_handle, gchar *classname, gchar *buff, gint32 length);
LOCAL gint32 get_card_object_serialnumber(OBJ_HANDLE component_handle, gchar *classname, gchar *buff, gint32 length);

LOCAL void generate_card_replace_event(CARD_REPLACED_INFO *replace_info);
LOCAL void generate_psu_replace_event(CARD_REPLACED_INFO *replace_info);
LOCAL void set_card_replace_info(CARD_REPLACED_INFO *replace_info);
LOCAL void set_power_replace_info(CARD_REPLACED_INFO *replace_info);

LOCAL CARD_REPLACED_CFG card_repalce_object_table[] = {
    {
        .device_type = COMPONENT_TYPE_HDD_BACKPLANE,
        .class_name = CLASS_HDDBACKPLANE_NAME,
        .get_sn = get_elabel_boardserialnumber,
        .generate_event = generate_card_replace_event,
        .update_info = set_card_replace_info
    },
    {
        .device_type = COMPONENT_TYPE_PCIE_CARD,
        .class_name = CLASS_PCIECARD_NAME,
        .get_sn = get_elabel_boardserialnumber,
        .generate_event = generate_card_replace_event,
        .update_info = set_card_replace_info
    },
    {
        .device_type = COMPONENT_TYPE_PCIE_RISER,
        .class_name = CLASS_RISERPCIECARD_NAME,
        .get_sn = get_elabel_boardserialnumber,
        .generate_event = generate_card_replace_event,
        .update_info = set_card_replace_info
    },
    {
        .device_type = COMPONENT_TYPE_RAID_CARD,
        .class_name = CLASS_RAIDCARD_NAME,
        .get_sn = get_elabel_boardserialnumber,
        .generate_event = generate_card_replace_event,
        .update_info = set_card_replace_info
    },
    {
        .device_type = COMPONENT_TYPE_MEZZ,
        .class_name = CLASS_MEZZ,
        .get_sn = get_elabel_boardserialnumber,
        .generate_event = generate_card_replace_event,
        .update_info = set_card_replace_info
    },
    {
        .device_type = COMPONENT_TYPE_FAN_BACKPLANE,
        .class_name = CLASS_FANBOARD_NAME,
        .get_sn = get_card_object_serialnumber,
        .generate_event = generate_card_replace_event,
        .update_info = set_card_replace_info
    },
    {
        .device_type = COMPONENT_TYPE_IO_BOARD,
        .class_name = CLASS_IOBOARD_NAME,
        .get_sn = get_elabel_boardserialnumber,
        .generate_event = generate_card_replace_event,
        .update_info = set_card_replace_info
    },
    {
        .device_type = COMPONENT_TYPE_BOARD_CPU,
        .class_name = CLASS_CPUBOARD_NAME,
        .get_sn = get_elabel_boardserialnumber,
        .generate_event = generate_card_replace_event,
        .update_info = set_card_replace_info
    },
    {
        .device_type = COMPONENT_TYPE_ASSET_LOCATE_BRD,
        .class_name = CLASS_ASSET_LOCATE_BOARD,
        .get_sn = get_elabel_boardserialnumber,
        .generate_event = generate_card_replace_event,
        .update_info = set_card_replace_info
    },
    {
        .device_type = COMPONENT_TYPE_EXPANDER,
        .class_name = CLASS_EXPBOARD_NAME,
        .get_sn = get_elabel_boardserialnumber,
        .generate_event = generate_card_replace_event,
        .update_info = set_card_replace_info
    },
    {
        .device_type = COMPONENT_TYPE_EXPBOARD,
        .class_name = CLASS_EXPBOARD_NAME,
        .get_sn = get_elabel_boardserialnumber,
        .generate_event = generate_card_replace_event,
        .update_info = set_card_replace_info
    },
    {
        .device_type = COMPONENT_TYPE_OCP,
        .class_name = CLASS_OCP_CARD,
        .get_sn = get_elabel_boardserialnumber,
        .generate_event = generate_card_replace_event,
        .update_info = set_card_replace_info
    },
    { // 电源声明在power_mgnt模块
        .device_type = COMPONENT_TYPE_PS,
        .class_name = CLASS_NAME_PS,
        .get_sn = get_card_object_sn,
        .generate_event = generate_psu_replace_event,
        .update_info = set_power_replace_info
    },
    { // 备电声明在power_mgnt模块
        .device_type = COMPONENT_TYPE_BATTERY,
        .class_name = CLASS_BBU_MODULE_NAME,
        .get_sn = get_card_object_sn,
        .generate_event = generate_psu_replace_event,
        .update_info = set_power_replace_info
    },
    {
        .device_type = COMPONENT_TYPE_NIC_CARD,
        .class_name = CLASS_NETCARD_NAME,
        .get_sn = get_elabel_boardserialnumber,
        .generate_event = generate_card_replace_event,
        .update_info = set_card_replace_info
    },
    {
        .device_type = COMPONENT_TYPE_GPU_BOARD,
        .class_name = CLASS_GPUBOARD_NAME,
        .get_sn = get_elabel_boardserialnumber,
        .generate_event = generate_card_replace_event,
        .update_info = set_card_replace_info
    }
};


LOCAL void generate_card_replace_sel(gpointer arg)
{
    CARD_REPLACED_INFO *replace_info = (CARD_REPLACED_INFO *)arg;
    const gint32 sel_delay_second = 5;

    (void)prctl(PR_SET_NAME, "GenCardReplaceSel");
    // 首先要更新SEL当前信息，以产生SEL日志
    (void)dal_set_property_value_string(replace_info->card_handle, COMMON_REPLACED_CUR_SN, replace_info->cur_sn,
        DF_NONE);
    (void)dal_set_property_value_byte(replace_info->card_handle, COMMON_REPLACED_FLAG, 1, DF_NONE);

    sleep(sel_delay_second); // 延时一定时间，等待SEL日志产生
    // 清除SEL日志产生标志
    (void)dal_set_property_value_byte(replace_info->card_handle, COMMON_REPLACED_FLAG, 0, DF_NONE);

    // 将当前更换后SN，PN更新为更换前SN，PN以为下一次更换做准备
    (void)dal_set_property_value_string(replace_info->card_handle, COMMON_REPLACED_PRE_SN, replace_info->cur_sn,
        DF_SAVE | DF_SAVE_STATIC);
    g_free(replace_info);
}


LOCAL void generate_card_replace_event(CARD_REPLACED_INFO *replace_info)
{
    CARD_REPLACED_INFO *card_replace_info = NULL;
    gint32 ret;
    gulong task_card_replace_id = 0;

    card_replace_info = g_malloc0(sizeof(CARD_REPLACED_INFO));
    if (card_replace_info == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 fail", __FUNCTION__);
        return;
    }

    (void)memcpy_s(card_replace_info, sizeof(CARD_REPLACED_INFO), replace_info, sizeof(CARD_REPLACED_INFO));
    ret = vos_task_create(&task_card_replace_id, "set_card_replace_flag", (TASK_ENTRY)generate_card_replace_sel,
        (gpointer)card_replace_info, DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: create task fail", __FUNCTION__);
        g_free(card_replace_info);
        return;
    }
}


LOCAL void set_power_replace_info(CARD_REPLACED_INFO *replace_info)
{
    gint32 ret;

    ret = dfl_call_class_method(replace_info->card_handle, METHOD_SET_PRE_REPLACE_INFO, NULL, NULL, NULL);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: power mgnt record sel log fail %d", __FUNCTION__, ret);
    }

    return;
}


LOCAL void set_card_replace_info(CARD_REPLACED_INFO *replace_info)
{
    gint32 ret = dal_set_property_value_string(replace_info->card_handle, COMMON_REPLACED_PRE_SN, replace_info->cur_sn,
        DF_SAVE | DF_SAVE_STATIC);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set prop value failed, result=0x%x", __FUNCTION__, ret);
    }
}


LOCAL void generate_psu_replace_event(CARD_REPLACED_INFO *replace_info)
{
    gint32 ret;

    ret = dfl_call_class_method(replace_info->card_handle, METHOD_PS_REPLACE_RECORD_SEL, NULL, NULL, NULL);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: power mgnt record sel log fail %d", __FUNCTION__, ret);
    }

    return;
}


LOCAL gint32 get_card_object_sn(OBJ_HANDLE component_handle, gchar *classname, gchar *buff, gint32 length)
{
    OBJ_HANDLE card_handle = 0;

    // 获取和Component对象同级别的板卡对象
    if (dal_get_specific_object_position(component_handle, classname, &card_handle) != RET_OK) {
        return RET_ERR;
    }

    return dal_get_property_value_string(card_handle, PROTERY_PS_SN, buff, length);
}


LOCAL gint32 get_card_object_serialnumber(OBJ_HANDLE component_handle, gchar *classname, gchar *buff, gint32 length)
{
    OBJ_HANDLE card_handle = 0;

    // 获取和Component对象同级别的板卡对象
    if (dal_get_specific_object_position(component_handle, classname, &card_handle) != RET_OK) {
        return RET_ERR;
    }

    return dal_get_property_value_string(card_handle, PROPERTY_PCIESSDCARD_SN, buff, length);
}


LOCAL gint32 get_elabel_boardserialnumber(OBJ_HANDLE component_handle, gchar *classname, gchar *buff, gint32 length)
{
    return dal_get_card_sn(component_handle, buff, length);
}


LOCAL CARD_REPLACED_CFG *get_specified_card_cfg(guint8 component_type)
{
    gint32 i = 0;

    for (; i < G_N_ELEMENTS(card_repalce_object_table); i++) {
        if (card_repalce_object_table[i].device_type == component_type) {
            return (CARD_REPLACED_CFG *)(&card_repalce_object_table[i]);
        }
    }

    return NULL;
}


LOCAL void card_replace_compare_info(OBJ_HANDLE component_handle, CARD_REPLACED_CFG *replace_cfg)
{
    CARD_REPLACED_INFO replace_info = { 0 };
    gint32 ret;

    if (replace_cfg->generate_event == NULL || replace_cfg->get_sn == NULL) {
        return;
    }

    // 获取和Component对象同级别的板卡对象
    if (dal_get_specific_object_position(component_handle, replace_cfg->class_name, &replace_info.card_handle) !=
        RET_OK) {
        return;
    }

    
    ret = replace_cfg->get_sn(component_handle, replace_cfg->class_name, replace_info.cur_sn,
        sizeof(replace_info.cur_sn));
    if (ret != RET_OK || strlen(replace_info.cur_sn) == 0 || g_strcmp0(replace_info.cur_sn, INVALID_DATA_STRING) == 0) {
        return;
    }

    ret = dal_get_property_value_string(replace_info.card_handle, COMMON_REPLACED_PRE_SN, replace_info.pre_sn,
        sizeof(replace_info.pre_sn));
    // 获取更换前的板卡SN
    if (ret != RET_OK || strlen(replace_info.pre_sn) == 0 || g_strcmp0(replace_info.pre_sn, INVALID_DATA_STRING) == 0) {
        if (replace_cfg->update_info != NULL) {
            replace_cfg->update_info(&replace_info);
        }
        return;
    }

    if (g_strcmp0(replace_info.cur_sn, replace_info.pre_sn) != 0) {
        replace_cfg->generate_event(&replace_info);
        return;
    }

    return;
}


LOCAL void card_replace_detect(OBJ_HANDLE component_handle)
{
    guint8 component_type = 0;
    guint8 presence = 0;
    CARD_REPLACED_CFG *card_replace_cfg = NULL;

    // 获取DeviceType
    if (dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type) != RET_OK) {
        return;
    }

    // 获取在位状态
    if (dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_PRESENCE, &presence) != RET_OK) {
        return;
    }

    // 不在位不进行检测
    if (presence == 0) {
        return;
    }

    // 根据Devicetype获取指定板卡对象信息
    card_replace_cfg = get_specified_card_cfg(component_type);
    if (card_replace_cfg == NULL) {
        return;
    }

    card_replace_compare_info(component_handle, card_replace_cfg);
    return;
}


void card_replace_record_detect(void)
{
    GSList *handle_list = NULL;
    GSList *list_item = NULL;
    gint32 ret;
    static gboolean startup_delay = TRUE;
    static gulong start_time = 0;
    static gulong last_time = 0;
    gulong cur_time;

    cur_time = vos_tick_get();
    if (startup_delay == TRUE) { // 启动延时
        if (start_time == 0) {
            start_time = cur_time;
        }

        if (cur_time - start_time < CARD_REPLACE_DETECT_DELAY) {
            return;
        }
        startup_delay = FALSE;
    } else { // 固化扫描频率使扫描周期大于事件产生flag置起时间
        if (cur_time - last_time < CARD_REPLACE_DETECT_PERIOD) {
            return;
        }
        last_time = cur_time;
    }

    ret = dfl_get_object_list(CLASS_COMPONENT, &handle_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: get %s object failed", __FUNCTION__, CLASS_COMPONENT);
        return;
    }

    list_item = handle_list;
    for (list_item = handle_list; list_item != NULL; list_item = g_slist_next(list_item)) {
        card_replace_detect((OBJ_HANDLE)list_item->data);
    }

    g_slist_free(handle_list);
    handle_list = NULL;
    return;
}
