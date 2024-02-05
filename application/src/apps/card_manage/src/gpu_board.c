

#include "pme_app/pme_app.h"

#include "gpu_board.h"
#include "get_version.h"


LOCAL TASKID g_dynamic_property_trigger_task = 0;

LOCAL TASKID g_repeater_mgnt_task = 0;
#define REPEATER_MGNT_TASK_NAME ("RepeaterMgntTask")

#define DYNAMIC_PROP_TRIGGER_TASK ("DynamicPropertyTriggerTask")
#define REPEATER_STATUS_MAX_RETRY_TIME 3

#define REPEATER_CONFIG_FORBIDDEN 1
#define REPEATER_CONFIG_ALLOWED 0
#define REPEATER_CONFIG_UNKNOWN 0xff


#define MAX_ACCESS_HARDWARE_RETRY_TIME 3

#define REPEATER_NOT_CONFIGURED 0
#define REPEATER_CONFIGURED 1
#define REPEATER_CONFIGURING 0xff

#define DFT_ENABLE 1
#define DFT_DISABLE 0

#define REPEATER_VERIFY_DATA_FAIL 2
#define REPEATER_ACCESS_FAIL 3

typedef struct repeater_mgnt_priv_data_s {
    guint8 config_phase;         // 配置阶段 0:未配置,1:已配置,0xff配置中
    guint8 last_trigger;         // 上一时刻配置条件
    guint8 refresh_status_count; // 刷新Status属性次数
} REPEATER_MGNT_PRIV_DATA;


LOCAL gint32 retrial_get_property_byte(OBJ_HANDLE obj_handle, const gchar *property_name, guint8 *out_val,
    guint8 retry_time)
{
    gint32 ret = 0;
    guint8 i = 0;

    
    if ((obj_handle == 0) || (property_name == NULL) || (out_val == NULL) || (retry_time <= 0)) {
        debug_log(DLOG_ERROR, "%s invalid para!", __FUNCTION__);
        return RET_ERR;
    }

    for (i = 0; i < retry_time; i++) {
        ret = dal_get_extern_value_byte(obj_handle, property_name, out_val, DF_AUTO);
        if (ret == RET_OK) {
            return RET_OK;
        }
        debug_log(DLOG_ERROR, "dal_get_extern_value_byte error(%d), retry %d times!", ret, i);
    }

    debug_log(DLOG_ERROR, "dal_get_extern_value_byte error(%d), retry %d times totally!", ret, retry_time);

    return ret;
}


LOCAL gint32 retrial_set_property_byte(OBJ_HANDLE obj_handle, const gchar *property_name, guint8 in_val,
    guint8 retry_time)
{
    gint32 ret = 0;
    guint8 i = 0;
    guint8 real_val = 0;

    
    if ((obj_handle == 0) || (property_name == NULL) || (retry_time <= 0)) {
        debug_log(DLOG_ERROR, "%s invalid para!", __FUNCTION__);
        return RET_ERR;
    }

    for (i = 0; i < retry_time; i++) {
        ret = dal_set_property_value_byte(obj_handle, property_name, in_val, DF_AUTO);
        if (ret == RET_OK) {
            
            vos_task_delay(10);
            ret = dal_get_extern_value_byte(obj_handle, property_name, &real_val, DF_AUTO);
            if ((ret == RET_OK) && (real_val == in_val)) {
                
                return RET_OK;
            } else {
                debug_log(DLOG_ERROR, "check property fail(%d), expect(%d), real(%d)", ret, in_val, real_val);
            }
        }

        
        vos_task_delay(100);
        debug_log(DLOG_ERROR, "dal_set_property_value_byte error(%d), retry %d times!", ret, i);
    }

    debug_log(DLOG_ERROR, "dal_set_property_value_byte error(%d), retry %d times totally!", ret, retry_time);

    return ret;
}


LOCAL gint32 verify_reg_data(const guchar *data_set, const gsize data_len, const guint32 offset,
    OBJ_HANDLE reg_config_obj, OBJ_HANDLE ref_chip_obj)
{
    gint32 ret = 0;
    guchar *data_set_buf = NULL;
    GVariant *property_maskset = NULL;
    const guchar *mask_set = NULL;
    gsize mask_len = 0;
    gsize data_index = 0;

    if (data_len == 0) {
        debug_log(DLOG_ERROR, "data_set length error(the value is 0)!");
        return RET_ERR;
    }

    
    ret = dfl_get_property_value(reg_config_obj, PROPERTY_REG_CONFIG_MASKSET, &property_maskset);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get property MaskSet value failed, ret = %d.", ret);
        return RET_ERR;
    }

    mask_set = (const guchar *)g_variant_get_fixed_array(property_maskset, &mask_len, sizeof(guchar));
    
    if (mask_set == NULL) {
        g_variant_unref(property_maskset);
        debug_log(DLOG_ERROR, "%s:%d:mask_set:NULL ", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    if (data_len != mask_len) {
        g_variant_unref(property_maskset);
        debug_log(DLOG_ERROR, "Mask length is not equal with data length!");
        return RET_ERR;
    }

    

    data_set_buf = (guchar *)g_malloc0(data_len);
    
    if (data_set_buf == NULL) {
        g_variant_unref(property_maskset);
        debug_log(DLOG_ERROR, "%s:%d:data_set_buf:NULL ", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    ret = dfl_chip_blkread(dfl_get_object_name(ref_chip_obj), offset, data_len, data_set_buf);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get data from chip failed, ret = %d.", ret);
        g_free(data_set_buf);
        g_variant_unref(property_maskset);
        return REPEATER_ACCESS_FAIL;
    }

    
    for (data_index = 0; data_index < data_len; data_index++) {
        
        if (data_set[data_index] != (data_set_buf[data_index] & mask_set[data_index])) {
            debug_log(DLOG_ERROR,
                "Check chip(%s) addr(%" G_GSIZE_FORMAT ") error, expected[0x%02x], real[0x%02x], mask[0x%02x]",
                dfl_get_object_name(ref_chip_obj), (gsize)(offset + data_index), data_set[data_index],
                data_set_buf[data_index], mask_set[data_index]);

            g_variant_unref(property_maskset);
            g_free(data_set_buf);

            return REPEATER_VERIFY_DATA_FAIL;
        }
    }

    g_variant_unref(property_maskset);
    g_free(data_set_buf);

    return RET_OK;
}


LOCAL gint32 handle_one_config_table(OBJ_HANDLE reg_config_obj, OBJ_HANDLE ref_chip_obj)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret = 0;
    guint8 verify_enable = 0;
    guint32 verify_interval = 0;
    guint8 check_before_write_flag = 0;
    guint32 reg_offset = 0;
    GVariant *property_dataset = NULL;
    const guchar *data_set_tmp = NULL;
    gsize len = 0;
    guint8 data_set[REG_CONFIG_DATA_LEN] = { 0 };

    
    (void)dal_get_extern_value_byte(reg_config_obj, PROPERTY_REG_CONFIG_VERIFY_ENABLE, &verify_enable, DF_AUTO);

    
    (void)dal_get_extern_value_byte(reg_config_obj, PROPERTY_REG_CONFIG_CHECK_BEFORE_WRITE, &check_before_write_flag,
        DF_AUTO);

    
    (void)dal_get_extern_value_uint32(reg_config_obj, PROPERTY_REG_CONFIG_OFFSET, &reg_offset, DF_AUTO);

    
    ret = dfl_get_property_value(reg_config_obj, PROPERTY_REG_CONFIG_DATASET, &property_dataset);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get property DataSet value failed, ret = %d.", ret);
        return RET_ERR;
    }

    data_set_tmp = (const guchar *)g_variant_get_fixed_array(property_dataset, &len, sizeof(guchar));
    if (data_set_tmp == NULL || len <= 0) {
        g_variant_unref(property_dataset);

        debug_log(DLOG_ERROR, DB_STR_GET_PROPERTY_NULL);
        return RET_ERR;
    }

    safe_fun_ret = memmove_s(data_set, sizeof(data_set), data_set_tmp, len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    g_variant_unref(property_dataset);

    if (check_before_write_flag == 1) {
        ret = verify_reg_data(data_set, len, reg_offset, reg_config_obj, ref_chip_obj);
        
        if (ret == RET_OK) {
            return RET_OK;
        }
    }

    ret = dfl_chip_blkwrite(dfl_get_object_name(ref_chip_obj), (gint32)reg_offset, len, data_set);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s][%d]write Chip failed with result:%d", __FUNCTION__, __LINE__, ret);
        return REPEATER_ACCESS_FAIL;
    }

    
    if (verify_enable == 0) {
        return RET_OK;
    }

    (void)dal_get_extern_value_uint32(reg_config_obj, PROPERTY_REG_CONFIG_VERIFY_INTERVAL, &verify_interval, DF_AUTO);

    
    if (verify_interval == 0) {
        verify_interval = 10;
    }

    
    vos_task_delay(verify_interval);

    ret = verify_reg_data(data_set, len, reg_offset, reg_config_obj, ref_chip_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Check RegConfig %s of Chip(%s) failed", dfl_get_object_name(reg_config_obj),
            dfl_get_object_name(ref_chip_obj));
    }

    return ret;
}


LOCAL gint32 config_ref_chip_reg(OBJ_HANDLE obj_handle, const char *property_name, OBJ_HANDLE ref_chip_obj)
{
    gint32 ret = 0;
    guint32 n = 0;
    OBJ_HANDLE ref_config_handle = 0;
    guint8 retry_times = 0;
    guint8 try_time = 0;

    while (1) {
        try_time = 0;

        ret = dal_get_refobject_handle_nth(obj_handle, property_name, n, &ref_config_handle);
        if (ret != RET_OK) {
            break;
        }
        n++;

        (void)dal_get_property_value_byte(ref_config_handle, PROPERTY_REG_CONFIG_RETRY_COUNT, &retry_times);
        
        if (retry_times == 0) {
            retry_times = 1;
        }

        for (try_time = 0; try_time < retry_times; try_time++) {
            ret = handle_one_config_table(ref_config_handle, ref_chip_obj);
            
            if (ret == RET_OK) {
                debug_log(DLOG_INFO, "write Config data(%s) into Chip(%s) success!",
                    dfl_get_object_name(ref_config_handle), dfl_get_object_name(ref_chip_obj));
                break;
            }

            debug_log(DLOG_ERROR, "try to write Config data(%s) into Chip(%s) fail!(%d times total)",
                dfl_get_object_name(ref_config_handle), dfl_get_object_name(ref_chip_obj), try_time + 1);
        }

        
        if (retry_times == try_time && ret != RET_OK) {
            debug_log(DLOG_ERROR, "write Config data(%s) into Chip(%s) fail!", dfl_get_object_name(ref_config_handle),
                dfl_get_object_name(ref_chip_obj));

            return REPEATER_ACCESS_FAIL;
        }
    }

    return RET_OK;
}


LOCAL gint32 process_each_reg_action(OBJ_HANDLE obj_handle)
{
    gint32 ret = 0;
    OBJ_HANDLE ref_chip_handle = 0;
    guint32 n = 0;
    gchar   device_name[NAME_LEN] = {0};
    GVariant *name_get_value = NULL;
    const gchar **name_array = NULL;
    gsize name_count = 0;
    gboolean config_flag = TRUE;
    guint8 slot_id = 0;
    gulong start_time_stamp = 0;
    gulong end_time_stamp = 0;

    start_time_stamp = vos_tick_get();
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_REG_ACTION_SLOT_ID, &slot_id);

    
    (void)dal_get_property_value_string(obj_handle, PROPERTY_REG_ACTION_DEVICE_NAME, device_name, sizeof(device_name));
    strategy_log(SLOG_INFO, "Start config %s%d chip.", device_name, slot_id);

    ret = dfl_get_property_value(obj_handle, PROPERTY_REG_ACTION_LOGIC_UNIT_ARRAY, &name_get_value);
    if (ret == DFL_OK) {
        name_array = g_variant_get_strv(name_get_value, &name_count);
    }

    while (1) {
        
        const gchar *chip_unit_number = (n < name_count && name_array != NULL) ? name_array[n] : "";

        
        ret = dal_get_refobject_handle_nth(obj_handle, PROPERTY_REG_ACTION_REF_CHIP, n, &ref_chip_handle);
        if (ret != RET_OK) {
            break;
        }
        n++;

        ret = config_ref_chip_reg(obj_handle, PROPERTY_REG_ACTION_CONFIG_TABLE, ref_chip_handle);
        if (ret != RET_OK) {
            config_flag = (ret == REPEATER_ACCESS_FAIL) ? FALSE : TRUE;
            (void)dal_set_property_value_byte(obj_handle, PROPERTY_REG_ACTION_CONFIG_RESULT, REG_CONFIG_ERROR, DF_AUTO);
            debug_log(DLOG_ERROR, "Config Chip %s%d %s Error!", device_name, slot_id, chip_unit_number);
            strategy_log(SLOG_ERROR, "Config %s%d %s Error!\n", device_name, slot_id, chip_unit_number);
            break;
        }

        debug_log(DLOG_INFO, "Config Chip %s%d %s Success!", device_name, slot_id, chip_unit_number);
    }

    if (config_flag == TRUE) {
        (void)dal_set_property_value_byte(obj_handle, PROPERTY_REG_ACTION_CONFIG_RESULT, REG_CONFIG_OK, DF_AUTO);
    }

    g_variant_unref(name_get_value);
    
    if (name_array != NULL) {
        g_free(name_array);
    }
    

    end_time_stamp = vos_tick_get();
    strategy_log((config_flag == TRUE) ? SLOG_INFO : SLOG_ERROR, "Config %s%d %s.(cost %.3f seconds)", device_name,
        slot_id, (config_flag == TRUE) ? "successed" : "failed", (end_time_stamp - start_time_stamp) / 1000.0);

    return (config_flag == TRUE) ? RET_OK : RET_ERR;
}


gint32 process_reg_action_with_start_condition(OBJ_HANDLE obj_handle, gpointer user_data)
{
    guint8 condition = 0;
    guint8 trigger_type = 0;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_REG_ACTION_TRIGGER_TYPE, &trigger_type);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_REG_ACTION_CONDITION_SRC, &condition);

    
    if (!(condition == ACTION_STATUS && (trigger_type & 0x01))) {
        return RET_OK;
    }

    (void)process_each_reg_action(obj_handle);

    
    return RET_OK;
}


gint32 process_reg_action_with_hotswap_condition(OBJ_HANDLE obj_handle, gpointer user_data)
{
    guint8 condition = 0;
    guint8 trigger_type = 0;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_REG_ACTION_TRIGGER_TYPE, &trigger_type);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_REG_ACTION_CONDITION_SRC, &condition);

    
    if (!(condition == ACTION_STATUS && (trigger_type & 0x02))) {
        return RET_OK;
    }

    (void)process_each_reg_action(obj_handle);

    
    return RET_OK;
}


gint32 process_reg_action_with_dynamic_condition(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    guint8 condition = 0;
    guint8 trigger_type = 0;

    condition = g_variant_get_byte(property_value);
    (void)dal_get_property_value_byte(object_handle, PROPERTY_REG_ACTION_TRIGGER_TYPE, &trigger_type);

    
    
    if (!(condition == ACTION_STATUS && (trigger_type & 0x04))) {
        return RET_OK;
    }

    (void)process_each_reg_action(object_handle);

    return RET_OK;
}


gint32 card_manage_start_reg_action(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    return process_each_reg_action(object_handle);
}


LOCAL gint32 check_reg_action_status(OBJ_HANDLE object_handle, guint8 *config_status)
{
    gint32 ret = 0;
    guint8 slot_id = 0;
    gchar device_name[NAME_LEN] = { 0 };
    GVariant *name_get_value = NULL;
    const gchar **name_array = NULL;
    guint32 m = 0, n = 0;
    OBJ_HANDLE ref_chip_handle = 0, ref_config_handle = 0;
    gsize name_count = 0;
    guint32 reg_offset = 0;
    GVariant *property_dataset = NULL;
    const guchar *data_set = NULL;
    gsize len = 0;
    guint8 checked_config_status = REG_CONFIG_OK;
    guint8 is_shield_check = 0;

    (void)dal_get_property_value_byte(object_handle, PROPERTY_REG_ACTION_SLOT_ID, &slot_id);

    (void)dal_get_property_value_string(object_handle, PROPERTY_REG_ACTION_DEVICE_NAME, device_name,
        sizeof(device_name));

    ret = dfl_get_property_value(object_handle, PROPERTY_REG_ACTION_LOGIC_UNIT_ARRAY, &name_get_value);
    if (ret == DFL_OK) {
        name_array = g_variant_get_strv(name_get_value, &name_count);
    }

    while (1) {
        
        const gchar *chip_unit_number = (n < name_count && name_array != NULL) ? name_array[n] : "";
        ret = dal_get_refobject_handle_nth(object_handle, PROPERTY_REG_ACTION_REF_CHIP, n, &ref_chip_handle);
        if (ret != RET_OK) {
            break;
        }
        n++;

        m = 0;
        while (1) {
            ret = dal_get_refobject_handle_nth(object_handle, PROPERTY_REG_ACTION_CONFIG_TABLE, m, &ref_config_handle);
            if (ret != RET_OK) {
                break;
            }
            m++;

            
            (void)dal_get_property_value_byte(ref_config_handle, PROPERTY_REG_CONFIG_IS_SHIELD_CHECK, &is_shield_check);
            if (is_shield_check) {
                debug_log(DLOG_DEBUG, "%s%d: RegConfig %s of Chip %s %s shield check process", device_name, slot_id,
                    dfl_get_object_name(ref_config_handle), dfl_get_object_name(ref_chip_handle), chip_unit_number);
                continue;
            }
            

            
            (void)dal_get_extern_value_uint32(ref_config_handle, PROPERTY_REG_CONFIG_OFFSET, &reg_offset, DF_AUTO);

            
            ret = dfl_get_property_value(ref_config_handle, PROPERTY_REG_CONFIG_DATASET, &property_dataset);
            if (ret != DFL_OK) {
                g_variant_unref(name_get_value);
                if (name_array != NULL) {
                    g_free(name_array);
                }
                debug_log(DLOG_ERROR, "Get property DataSet value failed, ret = %d.", ret);
                return RET_ERR;
            }

            data_set = (const guchar *)g_variant_get_fixed_array(property_dataset, &len, sizeof(guchar));
            if (data_set == NULL || len <= 0) {
                checked_config_status = REG_CONFIG_OK;
                g_variant_unref(property_dataset);

                debug_log(DLOG_ERROR, DB_STR_GET_PROPERTY_NULL);
                break;
            }

            ret = verify_reg_data(data_set, len, reg_offset, ref_config_handle, ref_chip_handle);
            g_variant_unref(property_dataset);

            if (ret != RET_OK) {
                checked_config_status = (ret == REPEATER_ACCESS_FAIL) ? REG_CONFIG_ERROR : REG_CONFIG_OK;
                debug_log(DLOG_ERROR, "%s%d: Check RegConfig %s of Chip %s %s failed", device_name, slot_id,
                    dfl_get_object_name(ref_config_handle), dfl_get_object_name(ref_chip_handle), chip_unit_number);
                break;
            }
        }

        
        if (checked_config_status == REG_CONFIG_ERROR) {
            break;
        }

        debug_log(DLOG_INFO, "%s%d: Check RegConfig of Chip %s %s Success", device_name, slot_id,
            dfl_get_object_name(ref_chip_handle), chip_unit_number);
    }

    g_variant_unref(name_get_value);
    
    if (name_array != NULL) {
        g_free(name_array);
    }
    

    
    strategy_log((checked_config_status == REG_CONFIG_OK) ? SLOG_INFO : SLOG_ERROR, "%s%d config %s!\r\n", device_name,
        slot_id, (checked_config_status == REG_CONFIG_OK) ? "match" : "unmatch");

    
    (void)dal_set_property_value_byte(object_handle, PROPERTY_REG_ACTION_CONFIG_RESULT, checked_config_status, DF_AUTO);

    *config_status = checked_config_status;

    return (ret == REPEATER_ACCESS_FAIL) ? REPEATER_ACCESS_FAIL : RET_OK;
}


gint32 card_manage_check_reg_action(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    guint8 config_status = REG_CONFIG_ERROR;

    ret = check_reg_action_status(object_handle, &config_status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Check RegAction config error(%s)!", dfl_get_object_name(object_handle));
        return ret;
    }

    *output_list = g_slist_append(*output_list, g_variant_new_byte(config_status));

    return RET_OK;
}


LOCAL gint32 handle_repeater_device_check(OBJ_HANDLE obj_handle)
{
    gint32 ret = 0;
    GVariant *property_reg_action_index = NULL;
    const guchar *reg_action_array = NULL;
    gsize len = 0;
    gsize i = 0;
    OBJ_HANDLE reg_action_handle = 0;
    guint8 config_status = 0;

    
    ret = dfl_get_property_value(obj_handle, PROPERTY_REPEATER_MGNT_REG_ACTION_INDEX, &property_reg_action_index);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_property_value error, obj(%s), property(%s).", dfl_get_object_name(obj_handle),
            PROPERTY_REPEATER_MGNT_REG_ACTION_INDEX);
        return ret;
    }

    reg_action_array = (const guchar *)g_variant_get_fixed_array(property_reg_action_index, &len, sizeof(guchar));
    if (reg_action_array == NULL || len <= 0) {
        g_variant_unref(property_reg_action_index);
        debug_log(DLOG_INFO, "There is no related RegAction obj.");
        return ret;
    }

    for (i = 0; i < len; i++) {
        
        ret = dal_get_specific_object_byte(CLASS_REG_ACTION, PROPERTY_REG_ACTION_OVERALL_INDEX, reg_action_array[i],
            &reg_action_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_INFO, "Related RegAction obj of RepeaterMgnt is not present, obj index is  %d.",
                reg_action_array[i]);
            continue;
        }

        
        (void)check_reg_action_status(reg_action_handle, &config_status);
    }

    g_variant_unref(property_reg_action_index);

    return RET_OK;
}


LOCAL gint32 handle_repeater_device_config(OBJ_HANDLE obj_handle)
{
    gint32 ret = 0;
    GVariant *property_reg_action_index = NULL;
    const guchar *reg_action_array = NULL;
    gsize len = 0;
    guint8 trigger = 0;
    gsize i = 0;
    OBJ_HANDLE reg_action_handle = 0;
    guint8 config_status = 0;

    
    ret = retrial_set_property_byte(obj_handle, PROPERTY_REPEATER_MGNT_STATUS, POWER_CTRL_STATUS_FORBID,
        MAX_ACCESS_HARDWARE_RETRY_TIME);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set_property_byte_with_retry_times error, ret(%d), obj(%s), property(%s), value(%d)",
            ret, dfl_get_object_name(obj_handle), PROPERTY_REPEATER_MGNT_STATUS, POWER_CTRL_STATUS_FORBID);
        goto exit;
    }

    
    ret = dfl_get_property_value(obj_handle, PROPERTY_REPEATER_MGNT_REG_ACTION_INDEX, &property_reg_action_index);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_property_value error, obj(%s), property(%s).", dfl_get_object_name(obj_handle),
            PROPERTY_REPEATER_MGNT_REG_ACTION_INDEX);
        goto exit;
    }

    reg_action_array = (const guchar *)g_variant_get_fixed_array(property_reg_action_index, &len, sizeof(guchar));
    if (reg_action_array == NULL || len <= 0) {
        g_variant_unref(property_reg_action_index);
        debug_log(DLOG_INFO, "There is no related RegAction obj.");
        goto exit;
    }

    
    ret = retrial_get_property_byte(obj_handle, PROPERTY_REPEATER_MGNT_TRIGGER, &trigger, DF_AUTO);
    if (ret != RET_OK) {
        g_variant_unref(property_reg_action_index);
        debug_log(DLOG_ERROR, "dal_get_extern_value_byte error, ret(%d), obj(%s), property(%s)", ret,
            dfl_get_object_name(obj_handle), PROPERTY_REPEATER_MGNT_TRIGGER);
        goto exit;
    }

    for (i = 0; i < len; i++) {
        
        ret = dal_get_specific_object_byte(CLASS_REG_ACTION, PROPERTY_REG_ACTION_OVERALL_INDEX, reg_action_array[i],
            &reg_action_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_INFO, "Related RegAction obj of RepeaterMgnt is not present, obj index is  %d.",
                reg_action_array[i]);
            continue;
        }

        
        if (trigger == REPEATER_CONFIG_ALLOWED) {
            
            (void)process_each_reg_action(reg_action_handle);
        } else {
            
            (void)check_reg_action_status(reg_action_handle, &config_status);
        }
    }

    g_variant_unref(property_reg_action_index);

exit:
    
    ret = retrial_set_property_byte(obj_handle, PROPERTY_REPEATER_MGNT_STATUS, POWER_CTRL_STATUS_ALLOW,
        MAX_ACCESS_HARDWARE_RETRY_TIME);
    if (ret != RET_OK) {
        strategy_log(SLOG_ERROR, "write RepeaterMgnt(%s) Status fail.", dfl_get_object_name(obj_handle));
    }

    return ret;
}


LOCAL gint32 process_each_gpu_board(OBJ_HANDLE handle, gpointer data)
{
    gint32 ret = 0;

    
    ret = get_pcb_version(handle, PROPERTY_GPUBOARD_PCBID, PROPERTY_GPUBOARD_PCBVER);
    if (ret != RET_OK) {
        
        debug_log(DLOG_ERROR, "get_pcb_version fail, ret=%d", ret);
        
    }

    
    ret = get_logic_version(handle, PROPERTY_GPUBOARD_LOGICVERID, PROPERTY_GPUBOARD_LOGICVER);
    if (ret != RET_OK) {
        
        debug_log(DLOG_ERROR, "get_logic_version fail, ret=%d", ret);
        
    }

    return RET_OK;
}


LOCAL gint32 process_each_sub_gpu_board(OBJ_HANDLE object_handle, gpointer user_data)
{
    gint32 ret = 0;

    
    ret = get_pcb_version(object_handle, PROPERTY_RISERPCIECARD_PCBID, PROPERTY_RISERPCIECARD_PCBVER);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get_pcb_version fail, ret=%d", ret);
    }

    
    ret = get_logic_version(object_handle, PROPERTY_RISERPCIECARD_LOGICVERID, PROPERTY_RISERPCIECARD_LOGICVER);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get_logic_version fail, ret=%d", ret);
    }

    return RET_OK;
}


gint32 init_each_repeater_mgnt(OBJ_HANDLE handle, gpointer data)
{
    gint32 ret = 0;
    REPEATER_MGNT_PRIV_DATA *repeater_priv_data = NULL;

    repeater_priv_data = (REPEATER_MGNT_PRIV_DATA *)g_malloc0(sizeof(REPEATER_MGNT_PRIV_DATA));
    if (repeater_priv_data == NULL) {
        return RET_OK;
    }

    repeater_priv_data->config_phase = REPEATER_NOT_CONFIGURED;
    repeater_priv_data->last_trigger = REPEATER_CONFIG_UNKNOWN;
    repeater_priv_data->refresh_status_count = 0;

    ret = dfl_bind_object_data(handle, (gpointer)repeater_priv_data, g_free);
    if (ret != DFL_OK) {
        g_free(repeater_priv_data);
        debug_log(DLOG_ERROR, "dfl_bind_object_data fail, result is %d", ret);
        return RET_OK;
    }

    return RET_OK;
}


gint32 gpu_board_init(void)
{
    (void)dfl_foreach_object(CLASS_GPUBOARD_NAME, process_each_gpu_board, NULL, NULL);

    if (dfl_foreach_object(CLASS_REPEATER_MGNT, init_each_repeater_mgnt, NULL, NULL) != DFL_OK) {
        debug_log(DLOG_ERROR, "init_each_repeater_mgnt fail");
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 _refresh_repeater_mgnt_trigger(OBJ_HANDLE handle, gpointer data)
{
    gint32 ret = 0;
    guint8 trigger = 0;
    REPEATER_MGNT_PRIV_DATA *repeater_priv_data = NULL;
    guint8 repeater_status = 0;

    ret = dfl_get_binded_data(handle, (gpointer *)&repeater_priv_data);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_binded_data fail, result is %d", ret);
        return RET_OK;
    }

    ret = retrial_get_property_byte(handle, PROPERTY_REPEATER_MGNT_TRIGGER, &trigger, MAX_ACCESS_HARDWARE_RETRY_TIME);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dal_get_extern_value_byte fail, result(%d) property(%s)", ret,
            PROPERTY_REPEATER_MGNT_TRIGGER);
        return RET_OK;
    }

    
    if (repeater_priv_data->last_trigger != trigger) {
        repeater_priv_data->last_trigger = trigger;
        repeater_priv_data->refresh_status_count = 0;
        debug_log(DLOG_ERROR, "Config Repeater dynamic start, trigger value is %d", trigger);

        
        if (trigger == REPEATER_CONFIG_ALLOWED) {
            repeater_priv_data->config_phase = REPEATER_CONFIGURING;
            
            ret = handle_repeater_device_config(handle);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Config Repeater Device(%s) failed(%d).", dfl_get_object_name(handle), ret);
            }
            repeater_priv_data->config_phase = REPEATER_CONFIGURED;
        } else {
            
            ret = handle_repeater_device_check(handle);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Check Repeater Device(%s) failed(%d).", dfl_get_object_name(handle), ret);
            }
        }
    } else {
        repeater_priv_data->refresh_status_count++;

        
        if (repeater_priv_data->refresh_status_count >= 30) {
            debug_log(DLOG_DEBUG, "Start Check refresh status count%d", repeater_priv_data->refresh_status_count);
            repeater_priv_data->refresh_status_count = 0;
            ret = retrial_get_property_byte(handle, PROPERTY_REPEATER_MGNT_STATUS, &repeater_status,
                MAX_ACCESS_HARDWARE_RETRY_TIME);
            if (ret == RET_OK && repeater_status != POWER_CTRL_STATUS_ALLOW) {
                debug_log(DLOG_ERROR, "Write POWER_CTRL_STATUS_ALLOW in device");
                (void)retrial_set_property_byte(handle, PROPERTY_REPEATER_MGNT_STATUS, POWER_CTRL_STATUS_ALLOW,
                    MAX_ACCESS_HARDWARE_RETRY_TIME);
            }
        }
    }

    return RET_OK;
}


LOCAL gint32 _refresh_reg_action_condition(OBJ_HANDLE handle, gpointer data)
{
    guint8 condition_src = 0;

    
    (void)dal_get_extern_value_byte(handle, PROPERTY_REG_ACTION_CONDITION_SRC, &condition_src, DF_AUTO);
    (void)dal_set_property_value_byte(handle, PROPERTY_REG_ACTION_CONDITION, condition_src, DF_AUTO);

    return RET_OK;
}


LOCAL void _dynamic_property_trigger_task(void *p)
{
    
    gint32 refresh_version_count = 0;
    

    (void)prctl(PR_SET_NAME, (gulong) "RepeatMgntProp");

    
    while (1) {
        (void)dfl_foreach_object(CLASS_REG_ACTION, _refresh_reg_action_condition, NULL, NULL);

        
        
        refresh_version_count++;
        if (refresh_version_count >= 5) {
            (void)dfl_foreach_object(CLASS_GPUBOARD_NAME, process_each_gpu_board, NULL, NULL);
            // 刷新GPU扣板的版本信息
            (void)dfl_foreach_object(CLASS_RISERPCIECARD_NAME, process_each_sub_gpu_board, NULL, NULL);
            refresh_version_count = 0;
        }
        

        vos_task_delay(1000);
    }
}


LOCAL void _repeater_mgnt_task(void *p)
{
    while (1) {
        (void)dfl_foreach_object(CLASS_REPEATER_MGNT, _refresh_repeater_mgnt_trigger, NULL, NULL);

        vos_task_delay(1000);
    }
}


gint32 gpu_board_start(void)
{
    
    OBJ_HANDLE computer_handle = 0;
    if (dal_get_object_handle_nth(CLASS_COMPUTER_PARTITION, 0, &computer_handle) == RET_OK) {
        return RET_OK;
    }
    

    (void)dfl_foreach_object(CLASS_REG_ACTION, process_reg_action_with_start_condition, NULL, NULL);

    debug_log(DLOG_INFO, "Config Repeater start");

    
    if (vos_task_create(&g_dynamic_property_trigger_task, DYNAMIC_PROP_TRIGGER_TASK,
        (TASK_ENTRY)_dynamic_property_trigger_task, (void *)NULL, DEFAULT_PRIORITY) != RET_OK) {
        debug_log(DLOG_ERROR, "create power ctrl device mgnt fail!");
    }

    
    if (vos_task_create(&g_repeater_mgnt_task, REPEATER_MGNT_TASK_NAME, (TASK_ENTRY)_repeater_mgnt_task, (void *)NULL,
        DEFAULT_PRIORITY) != RET_OK) {
        debug_log(DLOG_ERROR, "create power ctrl device mgnt fail!");
    }

    return RET_OK;
}


gint32 gpu_board_add_object_callback(OBJ_HANDLE object_handle)
{
    card_manage_log_operation_log("GPU board", object_handle, PROPERTY_GPUBOARD_SLOT, NULL, CARD_PLUG_IN);
    return process_each_gpu_board(object_handle, NULL);
}


gint32 gpu_board_del_object_callback(OBJ_HANDLE object_handle)
{
    card_manage_log_operation_log("GPU board", object_handle, PROPERTY_GPUBOARD_SLOT, NULL, CARD_PLUG_OUT);
    return RET_OK;
}


LOCAL void __dump_gpu_board_cpld(const gchar *path)
{
    gint32 iRet = -1;
    gchar               cpld_dump_file_path[MAX_FILE_NAME_LEN] = {0};
    gint32 ret;
    GSList *gpu_board_list = NULL;
    GSList *gpu_board_iter = NULL;
    OBJ_HANDLE gpu_board_handle;
    guint32 reg_info_size = 0;
    const gchar *obj_name = NULL;
    guchar *reg_data = NULL;
    gchar *file_buf = NULL;
    guchar gpu_board_slot = 0;
    guint16 board_id = 0;
    guint32 i;
    FILE *fp = NULL;
    guint32 max_retry_times = 3;

    ret = dfl_get_object_list(CLASS_GPUBOARD_NAME, &gpu_board_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_INFO, "%s: no gpu board handle found", __FUNCTION__);
        return;
    }

    file_buf = (gchar *)g_malloc0(SIZE_4K);
    if (file_buf == NULL) {
        debug_log(DLOG_ERROR, "%s failed: alloc new memory failed", __FUNCTION__);
        g_slist_free(gpu_board_list);
        return;
    }

    for (gpu_board_iter = gpu_board_list; gpu_board_iter != NULL; gpu_board_iter = gpu_board_iter->next) {
        gpu_board_handle = (OBJ_HANDLE)gpu_board_iter->data;

        obj_name = dfl_get_object_name(gpu_board_handle);
        if (obj_name == NULL) {
            debug_log(DLOG_ERROR, "%s: cann't get object name for %" OBJ_HANDLE_FORMAT, __FUNCTION__, gpu_board_handle);
            continue;
        }

        reg_info_size = 0;
        ret = dal_get_property_value_uint32(gpu_board_handle, PROPERTY_GPUBOARD_DUMPSIZE, &reg_info_size);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get property value for [%s:%s] failed, ret is %d", __FUNCTION__, obj_name,
                PROPERTY_GPUBOARD_DUMPSIZE, ret);
            continue;
        }

        // continue if size is invalid
        if (reg_info_size == 0) {
            continue;
        }

        reg_data = (guchar *)g_malloc0(reg_info_size);
        if (reg_data == NULL) {
            debug_log(DLOG_ERROR, "%s failed: alloc buffer for %s failed", __FUNCTION__, obj_name);
            continue;
        }

        // read all dump reg
        i = max_retry_times;
        while (i--) {
            ret =
                dfl_block_read(gpu_board_handle, PROPERTY_GPUBOARD_DUMPACCESSOR, 0, reg_info_size, (gpointer)reg_data);
            if (ret == DFL_OK) {
                break;
            }

            vos_task_delay(100);
            debug_log(DLOG_ERROR, "%s: dump gpu board cpld info for %s failed, ret is %d, current retry time:%d",
                __FUNCTION__, dfl_get_object_name(gpu_board_handle), ret, max_retry_times - i);
        }

        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: read property value for [%s:%s] failed, ret is %d", __FUNCTION__, obj_name,
                PROPERTY_GPUBOARD_DUMPACCESSOR, ret);
            g_free(reg_data);
            continue;
        }

        (void)dal_get_property_value_uint16(gpu_board_handle, PROPERTY_GPUBOARD_BOARDID, &board_id);
        (void)dal_get_property_value_byte(gpu_board_handle, PROPERTY_GPUBOARD_SLOT, &gpu_board_slot);

        // write header
        iRet = snprintf_s(file_buf + strlen(file_buf), SIZE_4K - strlen(file_buf), SIZE_4K - strlen(file_buf) - 1,
            GPU_REG_INFO_HEADER_FORMAT, gpu_board_slot, board_id);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        }
        (void)snprintf_s(file_buf + strlen(file_buf), SIZE_4K - strlen(file_buf), SIZE_4K - strlen(file_buf) - 1,
            "********************************************************************\r\n\r\n");

        (void)snprintf_s(file_buf + strlen(file_buf), SIZE_4K - strlen(file_buf), SIZE_4K - strlen(file_buf) - 1,
            "offset 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");

        (void)snprintf_s(file_buf + strlen(file_buf), SIZE_4K - strlen(file_buf), SIZE_4K - strlen(file_buf) - 1,
            "------------------------------------------------------\r\n");

        for (i = 0; i < reg_info_size; i++) {
            // print addr offset
            if ((i % 16) == 0) {
                (void)snprintf_s(file_buf + strlen(file_buf), SIZE_4K - strlen(file_buf),
                    SIZE_4K - strlen(file_buf) - 1, "%02x:    %02x", i, reg_data[i]);
            } else if ((i % 16) == 15) { // print line end
                (void)snprintf_s(file_buf + strlen(file_buf), SIZE_4K - strlen(file_buf),
                    SIZE_4K - strlen(file_buf) - 1, " %02x\r\n", reg_data[i]);
            } else {
                (void)snprintf_s(file_buf + strlen(file_buf), SIZE_4K - strlen(file_buf),
                    SIZE_4K - strlen(file_buf) - 1, " %02x", reg_data[i]);
            }
        }

        // cpld end line
        (void)snprintf_s(file_buf + strlen(file_buf), SIZE_4K - strlen(file_buf), SIZE_4K - strlen(file_buf) - 1,
            "\r\n\r\n");

        g_free(reg_data);
    }

    g_slist_free(gpu_board_list);

    ret = snprintf_s(cpld_dump_file_path, sizeof(cpld_dump_file_path), sizeof(cpld_dump_file_path) - 1, "%s/%s", path,
        GPU_REG_INFO_DUMP_FILE_NAME);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: format GPU Board collection file name failed", __FUNCTION__);
        g_free(file_buf);
        return;
    }

    // write to file
    fp = dal_fopen_check_realpath(cpld_dump_file_path, "w+", cpld_dump_file_path);
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "%s failed: cann't open file", __FUNCTION__);
        g_free(file_buf);
        return;
    }

    ret = fwrite(file_buf, strlen(file_buf), 1, fp);
    g_free(file_buf);

    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s failed: write gpu board cpld reg data to file failed, ret is %d, errno(%d)",
            __FUNCTION__, ret, errno);
        (void)fclose(fp);
        return;
    }

    
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);
    
    (void)fclose(fp);

    return;
}


gint32 gpu_board_dump_info(const gchar *path)
{
    gint32 iRet = -1;
    gint32 ret = 0;
    guint16 board_id = 0;
    const gchar *name = NULL;
    const gchar *pcb_ver = NULL;
    const gchar *logic_ver = NULL;
    const gchar *manufacturer = NULL;
    gchar file_name[GPU_BOARD_DUMPINFO_MAX_LEN + 1] = {0};
    gchar gpu_board_info[GPU_BOARD_DUMPINFO_MAX_LEN + 1] = {0};
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

    ret = dfl_get_object_list(CLASS_GPUBOARD_NAME, &obj_list);
    if (ret != DFL_OK) {
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            return RET_OK;
        }
        return RET_ERR;
    }

    
    __dump_gpu_board_cpld(path);
    

    // 在dump_dir目录下创建文件
    iRet = snprintf_s(file_name, GPU_BOARD_DUMPINFO_MAX_LEN + 1, GPU_BOARD_DUMPINFO_MAX_LEN, "%s/card_info", path);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        g_slist_free(obj_list);
        return RET_ERR;
    }
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);
    fwrite_back = fwrite("GPU board Info\n", strlen("GPU board Info\n"), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    (void)memset_s(gpu_board_info, GPU_BOARD_DUMPINFO_MAX_LEN + 1, 0, GPU_BOARD_DUMPINFO_MAX_LEN + 1);
    (void)snprintf_s(gpu_board_info, GPU_BOARD_DUMPINFO_MAX_LEN + 1, GPU_BOARD_DUMPINFO_MAX_LEN,
        "%-4s | %-10s | %-10s | %-32s | %-10s | %s\n", "Slot", "BoardId", "Name", "Manufacturer", "PCB Ver",
        "Logic Ver");
    fwrite_back = fwrite(gpu_board_info, strlen(gpu_board_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        property_name_list = g_slist_append(property_name_list, PROPERTY_GPUBOARD_SLOT);
        property_name_list = g_slist_append(property_name_list, PROPERTY_GPUBOARD_BOARDID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_GPUBOARD_NAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_GPUBOARD_MANUFACTURER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_GPUBOARD_PCBVER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_GPUBOARD_LOGICVER);

        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value);
        g_slist_free(property_name_list);
        property_name_list = NULL;
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get GPU board information failed!");
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

        (void)memset_s(gpu_board_info, GPU_BOARD_DUMPINFO_MAX_LEN + 1, 0, GPU_BOARD_DUMPINFO_MAX_LEN + 1);
        iRet = snprintf_s(gpu_board_info, sizeof(gpu_board_info), sizeof(gpu_board_info) - 1,
            "%-4u | 0x%-4x     | %-10s | %-32s | %-10s | %s\n", slot_id, board_id, name, manufacturer, pcb_ver,
            logic_ver);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        }
        fwrite_back = fwrite(gpu_board_info, strlen(gpu_board_info), 1, fp);
        if (fwrite_back != 1) {
            (void)fclose(fp);
            g_slist_free(obj_list);
            g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_ERROR, "fwrite failed!");
            return RET_ERR;
        }

        (void)memset_s(gpu_board_info, GPU_BOARD_DUMPINFO_MAX_LEN + 1, 0, GPU_BOARD_DUMPINFO_MAX_LEN + 1);
        g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
        property_value = NULL;
    }

    g_slist_free(obj_list);
    fwrite_back = fwrite("\n\n", strlen("\n\n"), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }
    (void)memset_s(gpu_board_info, GPU_BOARD_DUMPINFO_MAX_LEN + 1, 0, GPU_BOARD_DUMPINFO_MAX_LEN + 1);
    // 关闭文件
    (void)fclose(fp);

    return RET_OK;
}


LOCAL gint32 set_component_power_ctrl(OBJ_HANDLE obj_handle)
{
    gint32 ret = 0;
    OBJ_HANDLE ref_chip_handle = 0;

    ret = dfl_get_referenced_object(obj_handle, PROPERTY_COMPONENT_POWER_CTRL_REFCHIP, &ref_chip_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get the referenced object fail(%d)", ret);
        return RET_ERR;
    }

    ret = config_ref_chip_reg(obj_handle, PROPERTY_COMPONENT_POWER_CTRL_CONFIGTABLE, ref_chip_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "config_ref_chip_reg fail(%d), chip name(%s)", ret,
            dfl_get_object_name(ref_chip_handle));
        return RET_ERR;
    }
    return RET_OK;
}


gint32 component_default_power_ctrl(guint8 comp_device_type, guint16 device_num, guint32 device_pos, guint8 cmd_code,
    guint8 *comp_code)
{
    gint32 ret = 0;
    OBJ_HANDLE obj_handle = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 xml_condition = 0;
    guint8 xml_dev_type = 0;
    guint16 xml_dev_num = 0;
    guint32 xml_dev_pos = 0;
    guint8 xml_cmd_code = 0;
    guint8 cmd_excute_flag = 0;

    
    ret = dfl_get_object_list(CLASS_COMPONENT_POWER_CTRL, &obj_list);
    if (ret != DFL_OK || obj_list == NULL) {
        *comp_code = COMP_CODE_OUTOF_RANGE;
        debug_log(DLOG_DEBUG, "Get class (%s) handle list failed", CLASS_COMPONENT_POWER_CTRL);
        return RET_ERR;
    }

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        obj_handle = (OBJ_HANDLE)obj_node->data;

        // 增加装备模式的判断
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_POWER_CTRL_CONDITION, &xml_condition);

        if (xml_condition == 0) {
            debug_log(DLOG_INFO, "The operation does not meet the expected conditions,current condition :%d ",
                xml_condition);
            continue;
        }

        
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_POWER_CTRL_COMPTYPE, &xml_dev_type);
        (void)dal_get_property_value_uint16(obj_handle, PROPERTY_COMPONENT_POWER_CTRL_SLOT, &xml_dev_num);
        (void)dal_get_property_value_uint32(obj_handle, PROPERTY_COMPONENT_POWER_CTRL_POSITION, &xml_dev_pos);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_POWER_CTRL_OPERATETYPE, &xml_cmd_code);

        if (xml_dev_type == comp_device_type && device_num == 0xffff && xml_cmd_code == cmd_code) {
            if (device_pos == INVALID_DEVICE_POSITION || xml_dev_pos == device_pos) {
                cmd_excute_flag = 1;
                ret = set_component_power_ctrl(obj_handle);
                if (ret != RET_OK) {
                    debug_log(DLOG_ERROR, "Set component power ctrl failed ");
                    *comp_code = COMP_CODE_UNKNOWN;
                    g_slist_free(obj_list);
                    return RET_ERR;
                }
            }
        }

        else if (xml_dev_type == comp_device_type && xml_dev_num == device_num && xml_cmd_code == cmd_code) {
            if (device_pos == INVALID_DEVICE_POSITION || xml_dev_pos == device_pos) {
                cmd_excute_flag = 1;
                ret = set_component_power_ctrl(obj_handle);
                if (ret != RET_OK) {
                    debug_log(DLOG_ERROR, "Set component power ctrl failed ");
                    *comp_code = COMP_CODE_UNKNOWN;
                    g_slist_free(obj_list);
                    return RET_ERR;
                }
            }
        }
    }
    g_slist_free(obj_list);

    if (cmd_excute_flag == 1) {
        debug_log(DLOG_INFO, "Set component power ctrl Success ");
        *comp_code = COMP_CODE_SUCCESS;
        return RET_OK;
    } else {
        debug_log(DLOG_INFO, "Set component power ctrl Success ");
        *comp_code = COMP_CODE_PARA_NOT_SUPPORT;
        return RET_ERR;
    }
}
