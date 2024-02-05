

#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "common_macro.h"
#include "protocol.h"
#include "media/e2p_large.h"
#include "media/e2p_compact.h"
#include "media/port_rate.h"


#define SPEED_MAX_CPU_SET 4
guint8 g_board_port_speed = 0;
guint8 g_cpu_speed_info[SPEED_MAX_CPU_SET] = {0};

PER_S g_storage_per_flag[] = {
    
    { "BaordPortSpeed", PER_MODE_POWER_OFF, (guint8 *)&g_board_port_speed, (sizeof(g_board_port_speed))},
    { "cpu_speed_info", PER_MODE_POWER_OFF, (guint8 *)&g_cpu_speed_info,
        (sizeof(g_cpu_speed_info[0])*SPEED_MAX_CPU_SET)},
};



void init_storage_persis_prop(void)
{
    gint32 ret;

    if (!dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
        debug_log(DLOG_INFO, "No need to init persistance property.");
        return;
    }

    ret = per_init(g_storage_per_flag, sizeof(g_storage_per_flag) / sizeof(PER_S));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Persistent g_board_port_speed fail.");
    }
}


guint8 get_board_port_speed(void)
{
    return g_board_port_speed;
}


guint8 get_lom_port_speed_by_cpuid(guint8 cpuid)
{
    return g_cpu_speed_info[cpuid];
}


gint32 set_board_port_speed(guint8 port_speed)
{
    gint32 ret;
    guint8 pre_val;

    ret = set_lom_speed_to_cpld(0, port_speed); 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set baord_port_speed to cpld failed. ret = %d", ret);
        return ret;
    }

    if (g_board_port_speed == port_speed) {
        return RET_OK;
    }

    pre_val = g_board_port_speed; 
    g_board_port_speed = port_speed;
    ret = per_save(&g_board_port_speed); 
    if (ret == RET_OK) {
        return ret;
    }

    debug_log(DLOG_ERROR, "Persist baord_port_speed to flash failed. ret(%d).", ret);
    g_board_port_speed = pre_val;
    ret = set_lom_speed_to_cpld(0, pre_val); 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set baord port speed to cpld failed. ret(%d).", ret);
    }

    
    return RET_ERR;
}

void pyload_set_key_info(void)
{
    gint32 ret = RET_ERR;
    guint8 cpu_num;

    if (!dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
        return;
    }

    cpu_num = get_cpu_num();
    
    if (cpu_num == 1) {
        ret = set_lom_speed_to_cpld(0, g_board_port_speed); 
        debug_log(DLOG_ERROR, "Set board port speed to cpld, ret = %d", ret);
        return;
    }

    for (int i = 0; i < cpu_num; i++) {
        ret += set_lom_speed_to_cpld(i, g_cpu_speed_info[i]); 
    }
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set board port speed to cpld, ret = %d", ret);
    }
    return;
}

gint32 payload_set_eeprom_key_info(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    guint8 value;

    value = g_variant_get_byte(property_value);
    if (value == 1) {
        pyload_set_key_info();
    }

    return RET_OK;
}

gint32 method_set_eep_stop_routine_flag(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint8 is_stop = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    debug_log(DLOG_INFO, "%s, set eep routine flag, stop=%u", __FUNCTION__, is_stop);

    set_eeprom_stop_routine(is_stop);
    return RET_OK;
}

gint32 method_set_eeprom_status(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret;
    guint8 state = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));

    ret = dal_set_property_value_byte(object_handle, PROPERTY_EEPROM_STATUS, state, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, set eep status to %u failed.", __FUNCTION__, state);
    }
    return ret;
}


typedef struct tag_board_speed_addr_map {
    guint8 cpuid;
    guint32 offset;
    guint32 mask;
} BOARD_SPEED_ADDR_MAP_INFO;

gint32 set_lom_speed_to_cpld(guint8 cpuid, guint speed)
{
    guint32 offset;
    guint32 mask;
    gint32 ret;
    const gchar *chip_name = NULL;
    OBJ_HANDLE cpld_handle = INVALID_OBJ_HANDLE;

    ret = dfl_get_object_handle(OBJECT_CPLD1, &cpld_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get object cpld1 handle fail, ret=%d.", ret);
        return ret;
    }
    chip_name = dfl_get_object_name(cpld_handle);
    if (chip_name == NULL) {
        return RET_ERR;
    }

    const BOARD_SPEED_ADDR_MAP_INFO addr_maps[] = {
        { 0, CPLD_BOARD_PORT_SPEED_ADDR, 0xF0},
        { 1, CPLD_BOARD_CPU1_PORT_SPEED_ADDR, 0x0F}
    };

    for (int i = 0; i < sizeof(addr_maps) / sizeof(BOARD_SPEED_ADDR_MAP_INFO); i++) {
        if (cpuid == addr_maps[i].cpuid) {
            offset = addr_maps[i].offset;
            mask = addr_maps[i].mask;
            ret = dfl_chip_bytewrite(chip_name, offset, mask, &speed);
            break;
        }
    }
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Write to %s failed, offset:%x, mask:%x, speed%x", chip_name, offset, mask, speed);
    }
    return ret;
}

guint32 get_cpu_num(void)
{
    guint32 obj_cnt = 0;
    gint32 ret;
    ret = dfl_get_object_count(CLASS_CPU, &obj_cnt); 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get cpu num info failed, ret=%d", ret);
        return 0; 
    }
    return obj_cnt;
}

gint32 set_cpu_lom_speed(guint8 cpuid, guint speed)
{
    gint32 ret = RET_ERR;
    guint8 pre_val;

    if (cpuid >= get_cpu_num()) { 
        debug_log(DLOG_ERROR, "cpuid is not valid, cpuid=%d, speed=%d, ret(%d).", cpuid, speed, ret);
        return ret;
    }

    ret = set_lom_speed_to_cpld(cpuid, speed);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set port speed to cpld failed, cpuid=%d, speed=%d, ret(%d).", cpuid, speed, ret);
        return ret;
    }
    
    if (g_cpu_speed_info[cpuid] == speed) {
        return RET_OK;
    }

    pre_val = g_cpu_speed_info[cpuid]; 
    g_cpu_speed_info[cpuid] = speed;
    ret = per_save(g_cpu_speed_info); 
    if (ret == RET_OK) {
        return ret;
    }

    debug_log(DLOG_ERROR, "persist port speed to flash failed��cpuid=%d, speed=%d, ret(%d).", cpuid, speed, ret);
    g_cpu_speed_info[cpuid] = pre_val;
    ret = set_lom_speed_to_cpld(cpuid, pre_val); 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set port speed to cpld failed, cpuid=%d, speed=%d, ret(%d).", cpuid, pre_val, ret);
    }
    
    return RET_ERR;
}

gint32 method_set_cpld_status_to_fail(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret;
    OBJ_HANDLE mother_board_obj = 0;
    
    ret = dfl_get_object_handle(BMC_MOTHER_BOARD_FROM_FRU, &mother_board_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get %s object handle failed, ret=%d", BMC_MOTHER_BOARD_FROM_FRU, ret);
        return RET_ERR;
    }
    ret = dal_set_property_value_byte(mother_board_obj, PROPERTY_FRU_CPLD_STATUS, 1, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set %s.%s fialed", dfl_get_object_name(mother_board_obj), PROPERTY_FRU_CPLD_STATUS);
        return RET_ERR;
    }
    return RET_OK;
}