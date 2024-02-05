

#include <glib.h>
#include <glib/gprintf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "payload_storage.h"


BOARDCTRL_RESET_INFO_S g_boardctrl_resetreason = { 0 };
LOCAL OBJ_HANDLE g_storage_payload_obj = INVALID_OBJ_HANDLE;
LOCAL TASKID g_PowerOnTid = 0;

LOCAL guint8 g_first_open_bbu_flag = 0;


LOCAL gint32 get_storpayload_property_accessor(const char* property_name)
{
    gchar accessor_name[ACCESSOR_NAME_LEN] = {0};

    if (g_storage_payload_obj == INVALID_OBJ_HANDLE) {
        gint32 ret = dal_get_object_handle_nth(CLASS_NAME_STOR_PAYLOAD, 0, &g_storage_payload_obj);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get OBJECT_STORPAYLOAD object fail!(result=%d).", ret);
            return ret;
        }
    }

    return dfl_get_property_accessor(g_storage_payload_obj, property_name, accessor_name, sizeof(accessor_name));
}


gboolean is_support_storage_payload(void)
{
    OBJ_HANDLE obj_handle = 0;

    gint32 ret = dal_get_object_handle_nth(CLASS_NAME_STOR_PAYLOAD, 0, &obj_handle);
    if (ret == RET_OK) {
        return TRUE;
    }

    return FALSE;
}


gint32 check_first_poweron_cond(void)
{
    gint32 ret;
    guint8 power_flag = 0xff;
    guint8 standby_state = 0xff;

    if (is_support_storage_payload() != TRUE) {
        debug_log(DLOG_ERROR, "%s: it is not pangea peoduct.", __FUNCTION__);
        return VOS_ERR;
    }

    ret = get_storpayload_property_byte(PROPERTY_POWER_OPER_FLAG, &power_flag);
    ret += get_storpayload_property_byte(PROPERTY_STORPAYLOAD_STANDBYSTATE, &standby_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get power_flag or standby_state failed.", __FUNCTION__);
        return VOS_ERR;
    }

    debug_log(DLOG_ERROR, "%s: get power_flag %d , standby_state %d success.", __FUNCTION__, power_flag, standby_state);
    
    if ((power_flag == FALSE) && (standby_state == TRUE)) {
        return RET_OK;
    } else {
        return VOS_ERR;
    }
}


gint32 get_storpayload_property_byte(const char* property_name, guint8* out)
{
    gint32 res;

    if (out == NULL) {
        debug_log(DLOG_ERROR, "Invalid input para.\r\n");
        return VOS_ERR;
    }

    if (g_storage_payload_obj == INVALID_OBJ_HANDLE) {
        res = dfl_get_object_handle(OBJECT_STORPAYLOAD, &g_storage_payload_obj);
        if (res != RET_OK) {
            debug_log(DLOG_ERROR, "Get OBJECT_STORPAYLOAD object fail!(result=%d).", res);
            return VOS_ERR;
        }
    }

    res = dal_get_property_value_byte(g_storage_payload_obj, property_name, out);
    if (res != RET_OK) {
        debug_log(DLOG_ERROR, "Get OBJECT_STORPAYLOAD (%s) fail!(result=%d).", property_name, res);
    }

    return res;
}


gint32 set_storpayload_property_byte(const char* property_name, guint8* value_addr,
    DF_OPTIONS options)
{
    gint32 res;

    if (value_addr == NULL) {
        debug_log(DLOG_ERROR, "Para is invalid!");
        return VOS_ERR;
    }

    if (g_storage_payload_obj == INVALID_OBJ_HANDLE) {
        res = dfl_get_object_handle(OBJECT_STORPAYLOAD, &g_storage_payload_obj);
        if (res != RET_OK) {
            debug_log(DLOG_ERROR, "Get OBJECT_STORPAYLOAD object fail!(result=%d).", res);
            return VOS_ERR;
        }
    }

    res = dal_set_property_value_byte(g_storage_payload_obj, property_name, *value_addr, options);
    if (res != RET_OK) {
        debug_log(DLOG_ERROR, "Set OBJECT_STORPAYLOAD (%s)  fail!(result=%d).", property_name, res);
    }

    return res;
}


LOCAL gint32 hot_get_max_psu_num(guint8 *maximum)
{
    static guint8 max_num = 0;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    if (max_num != 0) {
        *maximum = max_num;
        return RET_OK;
    }

    
    ret = dfl_get_object_handle(OBJ_PRODUCT_COMPONENT, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get object(%s) failed, ret(%d).", OBJ_PRODUCT_COMPONENT, ret);
        return VOS_ERR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPOENT_PS_NUM, &max_num);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get property (%s) failed, ret(%d).", PROPERTY_COMPOENT_PS_NUM, ret);
        return VOS_ERR;
    }

    *maximum = max_num;
    return RET_OK;
}


LOCAL gint32 get_psu_opok_state(guint8 slot_id, guint8 *psu_op_ok_status)
{
    gint32 ret;
    OBJ_HANDLE obj_handle_syspower = 0;
    OBJ_HANDLE obj_handle_psuopok = 0;
    gchar accesor_name[MAX_NAME_SIZE] = {0};
    guchar opok_state = 0;

    (void)dal_get_object_handle_nth(CLASS_NAME_AMMETER, 0, &obj_handle_syspower);

    
    (void)dal_get_refobject_handle_nth(obj_handle_syspower, PROTERY_SYSPOWER_PSU_OPOK_STATUS, slot_id,
        &obj_handle_psuopok);
    ret = dfl_get_property_accessor(obj_handle_psuopok, PROPERTY_REG_VALUE, accesor_name, MAX_NAME_SIZE);
    if (ret == RET_OK) {
        (void)dal_get_extern_value_byte(obj_handle_psuopok, PROPERTY_REG_VALUE, &opok_state, DF_HW);
    } else {
        (void)dal_get_extern_value_byte(obj_handle_psuopok, PROPERTY_REG_VALUE, &opok_state, DF_AUTO);
    }

    *psu_op_ok_status = opok_state;
    return RET_OK;
}


LOCAL gint32 hop_get_psok_count(guint8 *opok_num)
{
    gint32 ret;
    guint8 max_psu_num;
    guint8 ps_id;
    guint8 opok_status = 0;
    guint8 cnt = 0;
    ret = hot_get_max_psu_num(&max_psu_num);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get max psu num failed(%d).", ret);
        return VOS_ERR;
    }

    for (ps_id = 0; ps_id < max_psu_num; ps_id++) {
        ret = get_psu_opok_state(ps_id, &opok_status);
        if ((ret == RET_OK) && (opok_status == TRUE)) {
            cnt++;
        }
    }
    *opok_num = cnt;
    return RET_OK;
}


LOCAL guint8 get_psucnt_according_product(void)
{
    guint8 config_num = 0; 
    gint32 ret;
    
    ret = get_storpayload_property_byte(PROPERTY_PSU_NUM_CONFIG, &config_num);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get property %s failed, ret %d.", PROPERTY_PSU_NUM_CONFIG, ret);
    }
    
    return config_num;
}


LOCAL gint32 hop_get_ps_num_config(guint8 *max_ps_num)
{
    gint32 ret;
    guint8 power_mode = POWER_MODE_SINGLE;
    guint8 psu_cnt;

    ret = get_storpayload_property_byte(PROPERTY_POWER_MODE, &power_mode);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get max psu num failed(%d).", ret);
        return VOS_ERR;
    }

    *max_ps_num = power_mode;
    
    if (power_mode == POWER_MODE_DEFAULT) {
        psu_cnt = get_psucnt_according_product();
        *max_ps_num = psu_cnt;
    }

    return RET_OK;
}


LOCAL gint32 check_power_supply(void)
{
    gint32 retv;
    guint8 working_ps_cnt = 0;
    guint8 max_ps_config = 0; 
    static guint8 cnt = 0;

    
    retv = hop_get_psok_count(&working_ps_cnt);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "Get current working psu num failed! (ret:%d)", retv);
        return VOS_ERR;
    }

    
    retv = hop_get_ps_num_config(&max_ps_config);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "Get max supported psu num failed! (ret:%d)", retv);
        return VOS_ERR;
    }

    debug_log(DLOG_DEBUG, "Current working power(%d) needed(%d).", working_ps_cnt, max_ps_config);
    
    if (working_ps_cnt < max_ps_config) {
        if (cnt == 0) {
            debug_log(DLOG_ERROR, "Current working power(%d) is less than needed(%d).", working_ps_cnt, max_ps_config);
        }
        cnt = 1; 
        return VOS_ERR;
    }
    cnt = 0;
    return RET_OK;
}


LOCAL gboolean is_cooling_fan_status_satised(void)
{
    
    OBJ_HANDLE cooling_handle = 0x0;
    gint32 ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &cooling_handle);
    if (ret != RET_OK) {
        return FALSE;
    }

    guint8 fault_rotor_num = 0xFF;
    ret = dal_get_property_value_byte(cooling_handle, COOLING_FAULT_FAN_ROTOR_CNT, &fault_rotor_num);
    if (ret != RET_OK) {
        return FALSE;
    }

    const guint8 invalid_fault_rotor_num = 0xFF; 
    const guint8 rated_fault_rotor_num = 1;      
    if (fault_rotor_num == invalid_fault_rotor_num) {
        return FALSE;
    }

    if (fault_rotor_num > rated_fault_rotor_num) {
        return FALSE;
    }

    return TRUE;
}


LOCAL gboolean is_cooling_fan_num_satised(void)
{
    
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dal_get_object_handle_nth(CLASS_PRODUCT_COMPONENT, 0, &obj_handle);
    if (ret != RET_OK) {
        return FALSE;
    }

    guint8 rated_fan_count = 0;
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPOENT_FAN_NUM, &rated_fan_count);
    if (ret != RET_OK) {
        return FALSE;
    }

    
    guint32 current_fan_cnt = 0;
    ret = dfl_get_object_count(FANCLASS, &current_fan_cnt);
    if (ret != DFL_OK) {
        return FALSE;
    }

    if (current_fan_cnt != (guint32)rated_fan_count) {
        return FALSE;
    }

    return TRUE;
}


gboolean is_cooling_satised(void)
{
    
    guint8 software_type = 0xff;
    (void)dal_get_software_type(&software_type);
    if (software_type != MGMT_SOFTWARE_TYPE_PANGEA_ALANTIC_CTRL) {
        return TRUE;
    }

    
    if (!is_cooling_fan_num_satised()) {
        return FALSE;
    }

    
    if (!is_cooling_fan_status_satised()) {
        return FALSE;
    }

    return TRUE;
}


gint32 is_pw_on_condit_satised(void)
{
    if (check_power_supply() != RET_OK || !is_cooling_satised()) {
        
        debug_log(DLOG_ERROR, "Current power and cooling is not met the power up condition.");
        pwr_on_task_command(CREATE_TASK);
        return VOS_ERR;
    }

    debug_log(DLOG_ERROR, "Current power and cooling is met the power up condition.");
    return RET_OK;
}


guint8 get_offing_state_flag(void)
{
    guint8 flag = FALSE;
    gint32 ret;

    ret = get_storpayload_property_byte(PROPERTY_IS_OFFING_STATE, &flag);
    if (ret != RET_OK) {
        
        return FALSE;
    }
    return flag;
}


void set_offing_state_flag(guint8 flag)
{
    gint32 ret;

    ret = set_storpayload_property_byte(PROPERTY_IS_OFFING_STATE, &flag, DF_SAVE_TEMPORARY);
    debug_log_limit_when_fail(DLOG_LIMIT_60, TRUE, "set offing state %d %s(ret %d).", flag,
        (ret == RET_OK) ? "succeeded" : "failed", ret);
    return;
}

LOCAL void do_power_change_action(const char* wp_property_name, const char* act_property_name)
{
    guint8 wp_value = 0;
    guint8 set_value = 1;

    gint32 ret = set_storpayload_property_byte(wp_property_name, (void*)&wp_value, DF_HW);
    ret += set_storpayload_property_byte(act_property_name, (void*)&set_value, DF_HW);
    vos_task_delay(500); // 延时500ms

    wp_value = 1;
    set_value = 0;

    ret += set_storpayload_property_byte(act_property_name, (void*)&set_value, DF_HW);
    ret += set_storpayload_property_byte(wp_property_name, (void*)&wp_value, DF_HW);
    debug_log_limit_when_fail(DLOG_LIMIT_60, TRUE, "Do power change %s action, ret(%d)", act_property_name, ret);
}


LOCAL void push_pwr_switch_button(const char *property_name)
{
    gint32 ret;
    guint8 set_value = 1;
    guint8 get_value = 0xFF;

    ret = set_storpayload_property_byte(property_name, &set_value, DF_HW);
    ret += get_storpayload_property_byte(property_name, &get_value);
    debug_log(DLOG_ERROR, "Set %s high(0x%x), ret=%d", property_name, get_value, ret);

    vos_task_delay(500); // 延时500ms

    set_value = 0;
    get_value = 0xFF;
    ret = set_storpayload_property_byte(property_name, &set_value, DF_HW);
    ret += get_storpayload_property_byte(property_name, &get_value);
    debug_log(DLOG_ERROR, "Set %s low(0x%x), ret=%d", property_name, get_value, ret);

    return;
}


LOCAL void push_button_for_bios_offcmd(void)
{
    
    if (get_storpayload_property_accessor(PROPERTY_POWER_SWITCH) == RET_OK) {
        push_pwr_switch_button(PROPERTY_POWER_SWITCH);
    }

    if (get_storpayload_property_accessor(PROPERTY_POWER_OFF) == RET_OK) {
        do_power_change_action(PROPERTY_POWER_OFF_WP, PROPERTY_POWER_OFF);
    }
    set_offing_state_flag(FALSE);
}


LOCAL gint32 is_power_state_abnormal(void)
{
    gint32 res;
    guint8 power_flag = 0xff;
    guint8 standby_state = 0xff;

    res = get_storpayload_property_byte(PROPERTY_POWER_OPER_FLAG, &power_flag);
    res += get_storpayload_property_byte(PROPERTY_STORPAYLOAD_STANDBYSTATE, &standby_state);
    if (res != RET_OK) {
        debug_log(DLOG_ERROR, "get power flag or standby state failed.");
        return VOS_ERR;
    }

    
    if (power_flag == TRUE && standby_state == TRUE) {
        debug_log(DLOG_ERROR, "Current power state is abnormal."); 
        return RET_OK;
    }
    return VOS_ERR;
}


LOCAL void pp_check_pwr_off_flag(void)
{
    gint32 ret;
    guint8 pwr_off_flag = 0;
    guint8 value;

    ret = get_storpayload_property_byte(PROPERTY_POWER_CMD_FLAG, &pwr_off_flag);
    if ((ret == RET_OK) && (pwr_off_flag == TRUE)) {
        debug_log(DLOG_ERROR, "Detected bios power off flag, power off os directly.");
        
        value = FALSE;
        ret = set_storpayload_property_byte(PROPERTY_POWER_CMD_FLAG, &value, DF_HW);
        handle_bios_off_cmd();
    }
    return;
}


LOCAL void pp_check_ac_state(void)
{
    gint32 ret;
    guint8 ac_evt = 0;

    ret = get_storpayload_property_byte(PROPERTY_ACDOWN_EVT, &ac_evt);
    if ((ret == RET_OK) && (ac_evt == TRUE)) {
        debug_log(DLOG_ERROR, "Detected ac event.");
        maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Detected ac down event.");
        pp_set_power_off_normal_flag(FALSE); 
        hop_set_power_before_ac_lost(1);
        return;
    }

    debug_log(DLOG_ERROR, "Normal power off.");
    pp_set_power_off_normal_flag(TRUE);
    return;
}


gint32 pp_bios_pwroff_handler(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;

    ret = handle_bios_off_cmd();
    if (ret != RET_OK) {
        operation_log("N/A", "N/A", "N/A", NULL, "BIOS sends command to write cpld to power off os failed.");
        debug_log(DLOG_ERROR, "BIOS sends command to write cpld to power off os failed, ret %d.", ret);
    } else {
        operation_log("N/A", "N/A", "N/A", NULL, "BIOS sends command to write cpld to power off os successful.");
        debug_log(DLOG_ERROR, "BIOS sends command to write cpld to power off os successful.");
    }

#ifdef ARM64_HI1711_ENABLED
    file_log_flush();
#endif
    return ret;
}

gint32 pp_method_get_pwroff_flag(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    if (output_list == NULL) {
        
        debug_log(DLOG_ERROR, "output_list can not be null.");
        return RET_ERR;
    }

    guint8 power_off_normal_flag = pp_get_power_off_normal_flag();
    *output_list = g_slist_append(*output_list, g_variant_new_byte(power_off_normal_flag));

    return RET_OK;
}


gint32 handle_bios_off_cmd(void)
{
    gint32 ret;
    guint8 value = 0xff;

    ret = get_storpayload_property_byte(PROPERTY_STORPAYLOAD_STANDBYSTATE, &value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get standby state failed.", __FUNCTION__);
        return VOS_ERR;
    }

    if (value == FALSE) {
        debug_log(DLOG_ERROR, "%s: os is power on state, write cpld to poweroff(reason:bios cmd).", __FUNCTION__);
        pp_check_ac_state();
        ctrl_power_button_handle(CTRL_BUTTON_BIOS);
    } else {
        debug_log(DLOG_ERROR, "%s: os is allready powered off, no need to handle.", __FUNCTION__);
    }
    return RET_OK;
}


LOCAL void check_abnormal_and_poweron(void)
{
    guint8 pwr_abnormal_time = 0;
    guint8 value;
    gint32 ret;
    guint8 retry_times = 2; 

    set_offing_state_flag(FALSE);

    while (retry_times--) {
        if (is_power_state_abnormal() == RET_OK) {
            pwr_abnormal_time++;
        }
        vos_task_delay(50); 
    }

    debug_log(DLOG_ERROR, "power abnormal count %d.", pwr_abnormal_time);
    if (pwr_abnormal_time == 2) { 
        value = TRUE; 
        ret = set_storpayload_property_byte(PROPERTY_CLEAR_ABNORMAL_PWR, &value, DF_HW);
        value = FALSE;
        vos_task_delay(50);  
        ret += set_storpayload_property_byte(PROPERTY_CLEAR_ABNORMAL_PWR, &value, DF_HW);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Write cpld to clear abnormal power state failed.");
        }
    }
    
    if (get_storpayload_property_accessor(PROPERTY_POWER_SWITCH) == RET_OK) {
        push_pwr_switch_button(PROPERTY_POWER_SWITCH);
    }

    if (get_storpayload_property_accessor(PROPERTY_POWER_ON) == RET_OK) {
        do_power_change_action(PROPERTY_POWER_ON_WP, PROPERTY_POWER_ON);
    }
}


LOCAL void write_cpld_sim_acdown(void)
{
    gint32 ret;
    guint8 set_value = 1;
    guint8 get_value = 0xFF;

    
    ret = set_storpayload_property_byte(PROPERTY_POWER_SIM_ACDOWN_WP, &set_value, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Open protection(%s) failed!(result=%d).", PROPERTY_POWER_SIM_ACDOWN_WP, ret);
    }

    
    set_value = 1;
    ret = set_storpayload_property_byte(PROPERTY_POWER_SIM_ACDOWN, &set_value, DF_HW);
    ret += get_storpayload_property_byte(PROPERTY_POWER_SIM_ACDOWN, &get_value);
    debug_log(DLOG_ERROR, "Set %s high(0x%x) to ac down, ret=%d", PROPERTY_POWER_SIM_ACDOWN, get_value, ret);

    vos_task_delay(1000); 

    set_value = 0;
    ret = set_storpayload_property_byte(PROPERTY_POWER_SIM_ACDOWN, &set_value, DF_HW);
    ret += get_storpayload_property_byte(PROPERTY_POWER_SIM_ACDOWN, &get_value);
    debug_log(DLOG_ERROR, "Set %s low(0x%x) to ac down, ret=%d", PROPERTY_POWER_SIM_ACDOWN, get_value, ret);

    
    set_value = 0;
    ret = set_storpayload_property_byte(PROPERTY_POWER_SIM_ACDOWN_WP, &set_value, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Close protection(%s) failed!(result=%d).", PROPERTY_POWER_SIM_ACDOWN_WP, ret);
    }
}


LOCAL void push_button_normal_down(void)
{
    set_offing_state_flag(TRUE);
    if (get_storpayload_property_accessor(PROPERTY_POWER_SOFT_OFF_WP) == RET_OK) {
        do_power_change_action(PROPERTY_POWER_SOFT_OFF_WP, PROPERTY_POWER_SOFT_OFF);
    } else {
        push_pwr_switch_button(PROPERTY_POWER_SOFT_OFF);
    }
}


LOCAL void push_button_force_off(void)
{
    guint8 force_off_type = 0;
    gint32 ret = get_storpayload_property_byte(PROPERTY_FORCE_POWER_OFF_TYPE, &force_off_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get property(%s) failed!(result=%d).", PROPERTY_FORCE_POWER_OFF_TYPE, ret);
    }

    if (force_off_type == SIM_ACDOWN) {
        set_offing_state_flag(TRUE);
        write_cpld_sim_acdown();
    } else if (force_off_type == WRITE_POWEROFF_REGISTER) {
        push_button_for_bios_offcmd();
        bbu_running_state_operate(CLOSE_BBU);
    }
}


void ctrl_power_button_handle(CTRL_BUTTON_HANDLE_E button_event)
{
    debug_log_limit_when_fail(DLOG_LIMIT_60, TRUE, "event %d, write cpld reg.", button_event);
    switch (button_event) {
        // 1、上电 2、BIOS备电完成  直接写短按钮
        case CTRL_BUTTON_UP:
            pp_set_power_off_normal_flag(FALSE);
            prepare_before_poweron();
            check_abnormal_and_poweron();
            break;
        case CTRL_BUTTON_BIOS:
            push_button_for_bios_offcmd();
            break;
        case CTRL_BUTTON_OFF:
            pp_set_power_off_normal_flag(TRUE);
            pp_operate_shining_led(LED_OPEN);
            push_button_normal_down();
            break;
        case CTRL_BUTTON_FORCE_OFF:
            pp_set_power_off_normal_flag(TRUE);
            pp_operate_shining_led(LED_OPEN);
            push_button_force_off();
            break;
        case CTRL_BUTTON_BUTT:
            break;
        default:
            debug_log(DLOG_ERROR, "Invalid event, not handle.");
            break;
    }
}


void sim_acdown_for_power_cycle()
{
    set_offing_state_flag(TRUE);
    write_cpld_sim_acdown();
}


gint32 ctrl_bbu_running_state(gint32 bbu_id, guint8 operation)
{
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle;
    guint8 read_data = 0;

    ret = dal_get_specific_object_byte(CLASS_BBU_MODULE_NAME, PROPERTY_BBU_MODULE_ID, bbu_id, &obj_handle);
    if (ret == RET_OK) { 
         
        input_list = g_slist_append(input_list, g_variant_new_byte(operation));
        ret = dfl_call_class_method(obj_handle, METHOD_BBU_MODULE_OPERATE_BBU, NULL, input_list, NULL);
        uip_free_gvariant_list(input_list);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s : call OperateBbu failed, ret = %d", __FUNCTION__, ret);
        }
    }

    
    for (guint8 i = 0; i < RETRY_3TIMES; i++) {
        ret = set_storpayload_property_byte(PROPERTY_BBU_DISCHARGE_SWITCH, &operation, DF_AUTO);
        if (ret != RET_OK) {
            continue;
        }

        vos_task_delay(10); 

        
        ret = get_storpayload_property_byte(PROPERTY_BBU_DISCHARGE_SWITCH, &read_data);
        if ((ret != RET_OK) || (read_data != operation)) {
            debug_log(DLOG_ERROR, "Operate(%d)(1:kill 0:active) bbu through cpld failed! ret = %d", operation, ret);
        } else {
            break;
        }
    }

    debug_log(DLOG_ERROR, "%s bbu[%d] %s", (operation == OPEN_BBU) ? "Open" : "Close",
              bbu_id, (ret == RET_OK) ? "success" : "fail");

    return ret;
}


void pp_operate_shining_led(guint8 operate)
{
    guint8 led_state = 0;
    gint32 ret;
    OBJ_HANDLE handle = 0;
    GSList *input_list = NULL;

    ret = dfl_get_object_handle(SYSTEM_OFFING_LED, &handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get SYSTEM_OFFING_LED object fail!(result=%d).", ret);
        return;
    }

    ret = dal_get_property_value_byte(handle, CONTROL_VALUE, &led_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get current led state fail!(result=%d).", ret);
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(operate));
    ret = dfl_call_class_method(handle, METHOD_SET_STATE, NULL, input_list, NULL);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Call METHOD_SET_STATE failed ,ret %d!", ret);
    }

    if (input_list != NULL) {
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    }

    debug_log_limit_when_fail(DLOG_LIMIT_60, TRUE, "Origin led state(%d), operate(%d) board led %s.", led_state,
        operate, (ret == RET_OK) ? "succeed" : "failed");
    return;
}


void prepare_before_poweron(void)
{
    if (is_support_storage_payload() != TRUE) {
        return;
    }
    debug_log(DLOG_ERROR, "product is pangea storage, will try to open bbu.\r\n");
    if (ctrl_bbu_running_state(0, OPEN_BBU) == VOS_ERR) {
        debug_log(DLOG_ERROR, "SYSCTRL active BBU fail");
    }
    
    g_first_open_bbu_flag = 1;
}


void prepare_after_poweroff(void)
{
    if (is_support_storage_payload() != TRUE) {
        return;
    }
    debug_log(DLOG_ERROR, "product is pangea storage, will try to kill bbu.\r\n");

    if (hop_get_pwr_signal(CHASSIS_FRU_ID) == HARD_POWER_OFF) {
        debug_log(DLOG_DEBUG, "%s:line:%d standby status, try to close bbu\n", __FUNCTION__, __LINE__);
        (void)ctrl_bbu_running_state(0, CLOSE_BBU);
    }
}


void payload_switch_loadline(guint8 action)
{
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;

    
    if (dal_match_board_id(PRODUCT_ID_PANGEA_V6, CST0221V6_BOARD_ID) != TRUE) {
        return;
    }

    guint32 ret = dal_get_object_handle_nth(CLASS_CHIP_VRD_PARAMETER_CONFIG, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get obj handle fail ret = %d.", ret);
        return;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(action));
    ret = dfl_call_class_method(obj_handle, METHOD_SWITCH_LOADLINE, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_call_class_method %s failed(ret = %d).", METHOD_SWITCH_LOADLINE, ret);
    }
}


LOCAL void pp_handle_acevt(void)
{
    gint32 ret;
    guint8 stb_state = 0xff; 

    
    payload_switch_loadline(CLOSE_LOADLINE);

    ret = get_storpayload_property_byte(PROPERTY_STORPAYLOAD_STANDBYSTATE, &stb_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get property %s failed.", PROPERTY_STORPAYLOAD_STANDBYSTATE);
        return;
    }

    if (stb_state == FALSE) {
        
        pp_set_power_off_normal_flag(FALSE); 
        pp_operate_shining_led(LED_OPEN);
    }
    return;
}


LOCAL void pp_check_acdown_task(void)
{
    gint32 ret;
    guint8 ac_event = 0;
    guint8 last_ac_state = 0;

    for (;;) {
        vos_task_delay(DELAY_200_MSEC);
        
        ret = get_storpayload_property_byte(PROPERTY_ACDOWN_EVT, &ac_event);
        if (ret != RET_OK) {
            vos_task_delay(DELAY_500_MSEC);
            continue;
        }

        
        if (last_ac_state != ac_event) {
            if (ac_event == TRUE) {
                debug_log(DLOG_ERROR, "Detected AC down event.");
                pp_handle_acevt();
            }
            last_ac_state = ac_event;
        }
    }
}


LOCAL void pp_check_peer_smm_pwrstate(void)
{
    gint32 ret;
    guint8 last_peer_pwrstate = POWER_OFF; 
    guint8 cur_peer_pwrstate = POWER_OFF;  
    guint8 software_type = MGMT_SOFTWARE_TYPE_INVALID;
    guint8 need_pwron_flag = 0;

    (void)dal_get_software_type(&software_type);
    if (software_type != MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        return;
    }

    for (;;) {
        vos_task_delay(DELAY_200_MSEC);

        
        ret = get_storpayload_property_byte(PROPERTY_PEER_PWR_STATE, &cur_peer_pwrstate);
        if (ret != RET_OK) {
            continue;
        }

        if (last_peer_pwrstate == POWER_OFF && cur_peer_pwrstate == POWER_ON) {
            need_pwron_flag = 1;
        }

        last_peer_pwrstate = cur_peer_pwrstate;
        if (need_pwron_flag) {
            
            ret = pp_fru_pwr_ctrl(CHASSIS_FRU_ID, POWER_ON);
            debug_log(DLOG_ERROR, "Peer SmmBoard power state changed, local SmmBoard power on %s, ret %d.",
                (ret == RET_OK) ? "successful" : "failed", ret);
            maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK,
                "Peer SmmBoard power state changed, local SmmBoard power on %s.",
                (ret == RET_OK) ? "successful" : "failed");
            if (ret != RET_OK) {
                vos_task_delay(20000); 
            } else {
                
                need_pwron_flag = 0;
            }
        }
    }
}


LOCAL void sys_handle_long_button(void)
{
    
    debug_log(DLOG_ERROR, "%s: pacific or atlantic dectected long button event.", __FUNCTION__);
}


LOCAL void pp_long_button_check_task(void)
{
    guchar power_button_status = 0;
    guchar acpistatus;
    gint32 ret;
    guint8 value;

    for (;;) {
        vos_task_delay(1000); // 延时1000ms
        // 逻辑自检，避免先烧后贴场景出现逻辑清空后，逻辑值随机导致一直误处理长按钮
        if (dal_get_cpld_selftest_status() != 0) { // 逻辑自检失败
            debug_log(DLOG_DEBUG, "%s: cpld self test failed.", __FUNCTION__);
            vos_task_delay(5000); // 延时5000ms,逻辑自检不过，延时长一些
            continue;
        }

        // 查询是否有按钮事件
        ret = get_storpayload_property_byte(PROPERTY_PHYSICAL_LONG_BUTTON, &power_button_status);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get long button event failed.", __FUNCTION__);
            continue;
        }

        debug_log(DLOG_DEBUG, "read cpld long button value %d.", power_button_status);
        if (!power_button_status) { // 无事件
            continue;
        }

        (void)hop_get_acpi_status(CHASSIS_FRU_ID, &acpistatus);
        // 下电状态不需要处理长按钮，记录日志打印
        if (acpistatus != ALL_POWER_GOOD_OK) {
            g_printf("Detected physical long button, but os state is  off, not handle button.\r\n");
            debug_log(DLOG_DEBUG, "Detected physical long button, but os state is off, not handle button.");
        } else {
            sys_handle_long_button();
        }

        // 先写1再写0，清除长按钮事件
        value = TRUE;
        ret = set_storpayload_property_byte(PROPERTY_PHYSICAL_LONG_BUTTON_CL, &value, DF_HW);
        vos_task_delay(100); // 延时100ms
        value = FALSE;
        ret += set_storpayload_property_byte(PROPERTY_PHYSICAL_LONG_BUTTON_CL, &value, DF_HW);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get storpayload property failed.");
        }

        debug_log(DLOG_ERROR, "write cpld clear long button.");
    }
}


LOCAL void handle_board_insert(void)
{
    gint32 ret;
    guint8 stdby_state = 0;
    OBJ_HANDLE bmc_obj = 0;
    guint8 value = 1;
    
    ret = get_storpayload_property_byte(PROPERTY_STORPAYLOAD_STANDBYSTATE, &stdby_state);
    if ((ret == RET_OK) && (stdby_state == TRUE)) {
        (void)set_storpayload_property_byte(PROPERTY_ACTION_AFTER_INSERT, &value, DF_SAVE_TEMPORARY);
    }

    
    ret = dfl_get_object_handle(CLASS_NAME_BMC, &bmc_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get object of %s failed, ret:%d.", CLASS_NAME_BMC, ret);
        return;
    }

    ret = dfl_call_class_method(bmc_obj, METHD_BMC_REBOOTPME, NULL, NULL, NULL);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Call reboot method fail, ret:%d.", ret);
    }

    debug_log(DLOG_ERROR, "Board is pluged in, try rebooting.");
    maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Board is pluged in, try rebooting.\n");
    return;
}


LOCAL void pp_board_inset_check_task(void)
{
    guchar board_present;
    guchar present_change;
    gint32 ret;
    guint8 value;

    
    if (dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_ALANTIC_CTRL) != TRUE) {
        return;
    }

    vos_task_delay(30000); 
    for (;;) {
        vos_task_delay(1000); 
        
        if (dal_get_cpld_selftest_status() != 0) { 
            debug_log(DLOG_DEBUG, "%s: cpld self test failed.", __FUNCTION__);
            vos_task_delay(5000); 
            continue;
        }

        
        ret = get_storpayload_property_byte(PROPERTY_LOCAL_PRESENT_CHANGE, &present_change);
        if (ret != RET_OK || present_change == FALSE) { 
            continue;
        }

        ret = get_storpayload_property_byte(PROPERTY_LOCAL_BOARD_PRESENT, &board_present);
        if (ret != RET_OK) {
            continue;
        }

        debug_log(DLOG_ERROR, "Board is pluged %s.", (board_present == FALSE) ? "in" : "out");
        maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Board is pluged %s.", (board_present == FALSE) ? "in" : "out");
        if (board_present == 0) {
            
            handle_board_insert();
        }

        
        value = FALSE;
        ret = set_storpayload_property_byte(PROPERTY_LOCAL_PRESENT_CHANGE, &value, DF_HW);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Clear board present change event failed, %d.", ret);
        }
    }
}


LOCAL void pp_bbu_work_state_init(void)
{
    
    if (g_first_open_bbu_flag != 0) {
        debug_log(DLOG_ERROR, "Detect power on, no need to close bbu.");
        return;
    }

    if (hop_get_pwr_signal(CHASSIS_FRU_ID) != HARD_POWER_ON) {
        
        ctrl_bbu_running_state(0, CLOSE_BBU);
    }
}

LOCAL void pp_clear_offing_state_flag(void)
{
    guint32 timeout;
    static guint32 wait_time = 0;

    if (pp_get_pwr_state(CHASSIS_FRU_ID) == PAYLOAD_POWER_STATE_OFF) { 
        wait_time = 0;
        if (get_offing_state_flag() == TRUE) {
            set_offing_state_flag(FALSE);
        }
    } else if (get_offing_state_flag() == TRUE) { 
        wait_time += 1;
        timeout = pp_get_pwr_off_timeout(CHASSIS_PAYLOAD_FRUID);
        if (wait_time >= (timeout + DELAY_2_MINUTES) / DELAY_1000_MSEC) {
            set_offing_state_flag(FALSE);
            wait_time = 0;
            debug_log(DLOG_ERROR, "Wait for %u s to be powered off and clear offing flag.",
                (timeout + DELAY_2_MINUTES) / DELAY_1000_MSEC);
        }
    }
}


LOCAL void pp_pwroff_flag_check_task(void)
{
    for (;;) {
        vos_task_delay(1000); 
        
        if (dal_get_cpld_selftest_status() != 0) { 
            debug_log(DLOG_DEBUG, "%s: cpld self test failed.", __FUNCTION__);
            vos_task_delay(5000); 
            continue;
        }
        
        pp_check_pwr_off_flag();

        
        pp_clear_offing_state_flag();
    }
}


LOCAL void pp_short_button_check_task(void)
{
    OBJ_HANDLE obj_handle = 0;
    gchar accesor_name[MAX_NAME_SIZE] = { 0 };
    gint32 ret = dfl_get_binded_object(CLASS_PAYLOAD, 0, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get payload handle failed ret = %d.", ret);
        return;
    }

    while (TRUE) {
        guint8 short_button_status = 0;
        guint8 acpi_status = ALL_POWER_GOOD_FAIL;
        vos_task_delay(1000);  
        if (dal_get_cpld_selftest_status() != 0) {
            vos_task_delay(5000);  
            continue;
        }

        ret = dfl_get_property_accessor(obj_handle, PROPERTY_PAYLOAD_PHY_BTN_SHORT_EVT, accesor_name, MAX_NAME_SIZE);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "association short button accessor failed, ret = %d.", ret);
            return;
        }

        (void)dal_get_property_value_byte(obj_handle, PROPERTY_PAYLOAD_PHY_BTN_SHORT_EVT, &short_button_status);
        if (short_button_status != TRUE) {
            continue;
        }

        (void)hop_get_acpi_status(CHASSIS_FRU_ID, &acpi_status);
        if (acpi_status == ALL_POWER_GOOD_OK) {
            operation_log("PANEL", "N/A", "127.0.0.1", NULL,
                "The power button is pressed. No action is required because the system has been powered on.");
        } else {
            ret = pp_method_pwr_on_ext(obj_handle, NULL, NULL, NULL);
            if (ret != RET_OK) {
                debug_log(DLOG_DEBUG, "send power on method failed.");
                continue;
            }
            operation_log("PANEL", "N/A", "127.0.0.1", NULL,
                "The power button is pressed, and the system starts to power on.");
        }

        ret = dal_set_property_value_byte(obj_handle, PROPERTY_PAYLOAD_PHY_BTN_SHORT_CLEAR, TRUE, DF_HW);
        vos_task_delay(500); 
        ret += dal_set_property_value_byte(obj_handle, PROPERTY_PAYLOAD_PHY_BTN_SHORT_CLEAR, FALSE, DF_HW);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "clear short button event failed. ret = %d", ret);
        }
    }
}


LOCAL void short_button_check_task(void)
{
    TASKID tid = 0;
    gint32 ret = vos_task_create(&tid, "CheckShortButtonTask", (TASK_ENTRY)pp_short_button_check_task,
        NULL, DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Create power off check task failed, ret = %d.", ret);
    }
}


void pp_event_check_init(void)
{
    gulong tid;
    gint32 ret;

    if (is_support_storage_payload() != TRUE) {
        debug_log(DLOG_ERROR, "It is not pangea product, no need to create task for checking button.");
        short_button_check_task();
        return;
    }

    
    pp_bbu_work_state_init();
    
    ret = vos_task_create(&tid, "CheckPowerOffFlag", (TASK_ENTRY)pp_pwroff_flag_check_task, 0, DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Create power off check task failed, ret %d.", ret);
    }

    
    ret = vos_task_create(&tid, "CheckLongbutton", (TASK_ENTRY)pp_long_button_check_task, 0, DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Create long button task failed, ret %d.", ret);
    }

    
    ret = vos_task_create(&tid, "CheckBoardInset", (TASK_ENTRY)pp_board_inset_check_task, 0, DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Create board insert task failed, ret %d.", ret);
    }

    ret = vos_task_create(&tid, "CheckACdownEvent", (TASK_ENTRY)pp_check_acdown_task, 0, DEFAULT_PRIORITY);
    debug_log(DLOG_ERROR, "Create thread to check ac down event %s (%d).", (ret == RET_OK) ? "succeeded" : "failed",
        ret);

    ret = vos_task_create(&tid, "CheckPeerSmmPowerState", (TASK_ENTRY)pp_check_peer_smm_pwrstate, 0, DEFAULT_PRIORITY);
    debug_log(DLOG_ERROR, "Create thread to check peer smmboard %s, ret %d.", (ret == RET_OK) ? "succeeded" : "failed",
        ret);
}


LOCAL void do_pwr_on_monit(void)
{
    gint32 ret;
    guint8 stdby_state = 0xff;
    guint8 val;

    while (1) {
        vos_task_delay(1000); 
        ret = get_storpayload_property_byte(PROPERTY_STORPAYLOAD_STANDBYSTATE, &stdby_state);
        
        if (ret == RET_OK && stdby_state == FALSE) {
            debug_log(DLOG_ERROR, "OS is already powered on.");
            g_PowerOnTid = 0;
            return;
        }

        
        if (check_power_supply() == RET_OK && is_cooling_satised()) {
            debug_log(DLOG_ERROR, "Current power and cooling is met the power up condition and start to power up.");
            break;
        }
    }

    
    val = pp_fru_pwr_ctrl(CHASSIS_FRU_ID, POWER_ON);
    debug_log(DLOG_ERROR, "Do power on %s, ret %d.", (val == 0) ? "success" : "fail", val);
    g_PowerOnTid = 0;
    return;
}


void pwr_on_task_command(guint8 cmd)
{
    gint32 ret;
    if (is_support_storage_payload() != TRUE) {
        return;
    }

    switch (cmd) {
        case CREATE_TASK:
            if (g_PowerOnTid != 0) {
                
                debug_log(DLOG_ERROR, "Power on task is already exist.");
                return;
            }

            ret = vos_task_create(&g_PowerOnTid, "PowerOnCheckTask", (TASK_ENTRY)do_pwr_on_monit, 0, DEFAULT_PRIORITY);
            debug_log(DLOG_ERROR, "Create PowerOnCheckTask %s, ret %d.", (ret == RET_OK) ? "success" : "fail", ret);
            break;

        case DELETE_TASK:
            pp_set_pwr_state_before_ac_lost(CHASSIS_FRU_ID, PAYLOAD_POWER_STATE_OFF);
            if (g_PowerOnTid == 0) {
                
                return;
            }

            ret = vos_task_delete(g_PowerOnTid);
            g_PowerOnTid = 0;
            debug_log(DLOG_ERROR, "Delete PowerOnCheckTask %s, ret %d.", (ret == RET_OK) ? "success" : "fail", ret);
            break;

        default:
            break;
    }
    return;
}


gint32 pp_set_imuforbid_pwron_flag(guint8 flag)
{
    gint32 ret;
    
    ret = set_storpayload_property_byte(PROPERTY_FORBID_PWRON, &flag, DF_SAVE_TEMPORARY);
    debug_log(DLOG_ERROR, "Set imu forbid power on flag(%d), ret:%d", flag, ret);
    return ret;
}


gint32 pp_get_imuforbid_pwron_flag(guint8 *val)
{
    guint8 value = 0;
    gint32 ret;

    if (val == NULL) {
        debug_log(DLOG_ERROR, "Input para is invalid.");
        return VOS_ERR;
    }

    ret = get_storpayload_property_byte(PROPERTY_FORBID_PWRON, &value);
    if (ret != RET_OK) {
        
        *val = FALSE;
    } else {
        *val = value;
    }
    debug_log(DLOG_DEBUG, "Get IMU forbid flag(%d), ret(%d).", value, ret);
    return ret;
}


LOCAL void handle_imu_forbid_pwron_event(void)
{
    guint8 forbid_pwron_flag = 0;
    gint32 ret;
    guint8 value;
    // 查询是否有imu禁止上电事件
    ret = get_storpayload_property_byte(PROPERTY_IMU_FORBID_PWRUP_FLAG, &forbid_pwron_flag);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get IMU forbid pwron flag failed.");
        return;
    }

    if (forbid_pwron_flag == IMU_POWER_FLAG_FORBID) {
        
        (void)pp_set_imuforbid_pwron_flag(TRUE);

        
        value = TRUE;
        ret = set_storpayload_property_byte(PROPERTY_CLEAR_ABNORMAL_PWR, &value, DF_HW);
        value = FALSE;
        vos_task_delay(50);  
        ret += set_storpayload_property_byte(PROPERTY_CLEAR_ABNORMAL_PWR, &value, DF_HW);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Write cpld to clear abnormal power state failed.");
        }

        
        (void)set_storpayload_property_byte(PROPERTY_IMU_FORBID_PWRUP_FLAG, &value, DF_HW);
    }
}


LOCAL void pp_high_temp_check_task(void)
{
    for (;;) {
        vos_task_delay(1000); // 延时1000ms
        // 逻辑自检，避免先烧后贴场景出现逻辑清空后，逻辑值随机导致一直误处理长按钮
        if (dal_get_cpld_selftest_status() != 0) { // 逻辑自检失败
            debug_log(DLOG_ERROR, "%s: cpld self test failed.", __FUNCTION__);
            vos_task_delay(5000); // 延时5000ms,逻辑自检不过，延时长一些
            continue;
        }

        
        handle_imu_forbid_pwron_event();
    }
}


void pp_high_temperature_check_init(void)
{
    gulong tid;
    gint32 ret;

    
    if (is_support_storage_payload() != TRUE) {
        debug_log(DLOG_ERROR, "It is not pangea product, no need to create task for checking high temperature.");
        return;
    }

    
    ret = vos_task_create(&tid, "HighTemperature", (TASK_ENTRY)pp_high_temp_check_task, 0, DEFAULT_PRIORITY);
    debug_log(DLOG_ERROR, "Create high temperature task %s.", (ret == RET_OK) ? "succedded" : "failed");
}


void pp_printf_abnormal_pwrstate(void)
{
    gint32 ret;
    guint8 pwr_fail_vpp;
    guint8 pwr_fail_vtt;
    guint8 pwr_fail_vcc;
    guint8 pwr_fail_debug;

    if (is_support_storage_payload() != TRUE) {
        return;
    }

    ret = get_storpayload_property_byte(PROPERTY_PWR_FAIL_REG_VPP, &pwr_fail_vpp);
    ret += get_storpayload_property_byte(PROPERTY_PWR_FAIL_REG_VTT, &pwr_fail_vtt);
    ret += get_storpayload_property_byte(PROPERTY_PWR_FAIL_REG_VCC, &pwr_fail_vcc);
    ret += get_storpayload_property_byte(PROPERTY_PWR_FAIL_REG_DEBUG, &pwr_fail_debug);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get storpayload property failed.");
        return;
    }

    debug_log(DLOG_ERROR, "Cpld reg addr:0x30 val:0x%x, addr:0x3f, val:0x%x, addr:0x40 val:0x%x, addr:0x41 val:0x%x.",
              pwr_fail_debug, pwr_fail_vpp, pwr_fail_vtt, pwr_fail_vcc);
    return;
}


LOCAL void boardctrl_resetreason_record(guint8 reg_value, guint8 reg_type)
{
    guint32 i;
    const char *desc = NULL;
    const BOARDCTRL_RESET_REASON_S reasons[] = {
        { 0x02, "Wdt" },
        { 0x03, "SoftS3" },
        { 0x04, "ShutDown" },
        { 0x06, "Unknown" },
        { 0x0a, "PowerOn" },
        { 0x0c, "BiosSwitch" },
        { 0x0d, "CpldUpgrade" },
        { 0x0f, "Cold Reset" },
        { 0xa7, "Microsystem BMC" },
        { 0xb0, "OSPowerAbnormal" },
        { 0xb1, "ClockCfgReset" },
    };

    for (i = 0; i < ARRAY_SIZE(reasons); i++) {
        if (reasons[i].reset_value == reg_value) {
            desc = reasons[i].reset_desc;
            break;
        }
    }

    
    if (desc == NULL) {
        debug_log(DLOG_ERROR, "CtrlBoard undetected reset reason, register value 0x%02x.", reg_value);
        return;
    }

    
    if (reg_type == BOARDCTRL_CUR_RESET) {
        debug_log(DLOG_ERROR, "CtrlBoard current reset reason: %s.", desc);
    } else if (reg_type == BOARDCTRL_LAST_RESET) {
        debug_log(DLOG_ERROR, "CtrlBoard last reset reason: %s.", desc);
    } else if (reg_type == BOARDCTRL_LLAST_RESET) {
        debug_log(DLOG_ERROR, "CtrlBoard last last reset reason: %s.", desc);
    } else {
        debug_log(DLOG_ERROR, "CtrlBoard unknown register type %u.", reg_type);
    }
}


LOCAL void boardctrl_shutdown_record(guint8 bit_value)
{
    
    switch (bit_value) {
        case 0x00:
            debug_log(DLOG_ERROR, "CtrlBoard shutdown reason: NULL");
            break;
        case 0x01:
            debug_log(DLOG_ERROR, "CtrlBoard shutdown reason: AC lost");
            break;
        case 0x02:
            debug_log(DLOG_ERROR, "CtrlBoard shutdown reason: Power button");
            break;
        case 0x03:
            debug_log(DLOG_ERROR, "CtrlBoard shutdown reason: Sofware shutdown");
            break;
        default:
            debug_log(DLOG_ERROR, "CtrlBoard shutdown reason: Unknown reason 0x%2x", bit_value);
            break;
    }
}


LOCAL void boardctrl_watchdog_record(guint8 bit_value)
{
    
    switch (bit_value) {
        case 0x00:
            debug_log(DLOG_ERROR, "CtrlBoard watchdog reset reason: NULL");
            break;
        case 0x01:
            debug_log(DLOG_ERROR, "CtrlBoard watchdog reset reason: BIOS boot failed");
            break;
        case 0x02:
            debug_log(DLOG_ERROR, "CtrlBoard watchdog reset reason: OS boot failed");
            break;
        case 0x03:
            debug_log(DLOG_ERROR, "CtrlBoard watchdog reset reason: Software boot failed");
            break;
        case 0x07:
            debug_log(DLOG_ERROR, "CtrlBoard watchdog reset reason: CPU_RST OUT trigger");
            break;
        default:
            debug_log(DLOG_ERROR, "CtrlBoard watchdog reset reason: Unknown reason 0x%2x", bit_value);
            break;
    }
}


LOCAL void boardctrl_resetassis_record(guint8 reg_value)
{
    guint8 bit_value;

    debug_log(DLOG_ERROR, "CtrlBoard reset assit register value 0x%2x.", reg_value);

    
    bit_value = reg_value & BMC_MASK_3;
    boardctrl_shutdown_record(bit_value);

    
    bit_value = (reg_value & BMC_MASK_1C) >> BIT2;
    boardctrl_watchdog_record(bit_value);

    
    bit_value = (reg_value & 0xE0) >> BIT5;
    debug_log(DLOG_ERROR, "CtrlBoard BIOS reset system reason: 0x%2x", bit_value);

    
    boardctrl_recordsys_coldreboot();
    
}


LOCAL void boardctrl_resetreason_record_task(void)
{
    gint32 result;
    guint8 reg_value = 0;
    guint32 heart_beat = HEART_BEAT_CNT_ONEDAY;
    OBJ_HANDLE ps_obj;

    for (;;) {
        
        if (heart_beat == HEART_BEAT_CNT_ONEDAY) {
            heart_beat = 0;
            debug_log(DLOG_ERROR, "CtrlBoard heart beat.");
        } else {
            heart_beat++;
        }

        result = dfl_get_object_handle(OBJECT_STORPAYLOAD, &ps_obj);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "Get %s object failed, ret(%d).", OBJECT_STORPAYLOAD, result);
            return;
        }

        (void)dal_get_property_value_byte(ps_obj, PROPERTY_RESTART_COUNT, &reg_value);

        
        if (g_boardctrl_resetreason.reset_reason_cnt != reg_value) {
            
            g_boardctrl_resetreason.reset_reason_cnt = reg_value;
            debug_log(DLOG_ERROR, "CtrlBoard reset count %u.", reg_value);
            reg_value = 0;

            (void)dal_get_property_value_byte(ps_obj, PROPERTY_CURRENT_RESTART_CAUSE, &reg_value);
            g_boardctrl_resetreason.reset_reason_cur = reg_value;
            boardctrl_resetreason_record(reg_value, BOARDCTRL_CUR_RESET);
            reg_value = 0;

            (void)dal_get_property_value_byte(ps_obj, PROPERTY_LAST_RESTART_CAUSE, &reg_value);
            g_boardctrl_resetreason.reset_reason_last = reg_value;
            boardctrl_resetreason_record(reg_value, BOARDCTRL_LAST_RESET);
            reg_value = 0;

            (void)dal_get_property_value_byte(ps_obj, PROPERTY_BEFORE_LAST_RESTART_CAUSE, &reg_value);
            g_boardctrl_resetreason.reset_reason_llast = reg_value;
            boardctrl_resetreason_record(reg_value, BOARDCTRL_LLAST_RESET);
            reg_value = 0;

            (void)dal_get_property_value_byte(ps_obj, PROPERTY_RESTART_CAUSE_ASSIST_INFO, &reg_value);
            g_boardctrl_resetreason.reset_reason_assit = reg_value;
            boardctrl_resetassis_record(reg_value);
        }

        vos_task_delay(BOARDCTRL_RESETRESRECORD_CYCLE);
    }
}


void boardctrl_resetreason_record_init(void)
{
    gulong tid;
    gint32 ret;

    if (is_support_storage_payload() != TRUE) {
        debug_log(DLOG_ERROR, "Not pangea product, no need to create CtrlBoard reset reason record task.");
        return;
    }

    
    ret = vos_task_create(&tid, "CtrlBoard_rstrsn_record_task", (TASK_ENTRY)boardctrl_resetreason_record_task, 0,
        DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Create CtrlBoard reset reason record task failed.");
        return;
    }

    debug_log(DLOG_ERROR, "CtrlBoard reset reason record task start sucess.");
}


void boardctrl_recordsys_coldreboot(void)
{
    guint8 ram_data = 0;
    guint16 ram_addr = CPLD_RAM_SYS_COLD_REBOOT_ADDR;

    dal_get_cpld_ram(ram_addr, &ram_data);

    if (ram_data == CPLD_RAM_SYS_COLD_REBOOT_DATA_AA) {
        debug_log(DLOG_ERROR, "Os cold reboot(0x%04x),ram_data(0x%x)", ram_addr, ram_data);
    } else {
        debug_log(DLOG_ERROR, "dal_get_cpld_ram get ram_data(0x%x)", ram_data);
    }

    return;
}


LOCAL guint8 check_dft_mode(void)
{
    gint32 ret;
    guint8 read_data = 0;

    
    ret = dal_read_cpld_bytes(1, DFT_MODE_CPLD_ADDR, BYTE1, &read_data);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s, call dal_read_cpld_bit failed. ret %d \r\n", __FUNCTION__, ret);
        return FALSE;
    }

    if (read_data != NOT_DFT_MODE_FLAG) {
        debug_log(DLOG_INFO, "%s, call dal_read_cpld_bytes success. read_data %d. DFT mode \r\n", __FUNCTION__,
            read_data);
        return TRUE;
    }

    return FALSE;
}


LOCAL guint8 check_watch_dog_necessity(void)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    guint32 board_id = 0;

    ret = dfl_get_object_handle(BMC_MOTHER_BOARD_FROM_FRU, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get board obj failed. ret %d", __FUNCTION__, ret);
        return DOG_NONEED_OPERATE;
    }
    ret = dal_get_property_value_uint32(obj_handle, BMC_BOARD_ID_NAME, &board_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get board id failed. ret %d", __FUNCTION__, ret);
        return DOG_NONEED_OPERATE;
    }

    
    switch (board_id) {
        case ATLANTIC_BOARD_ID:
            return DOG_NEED_OPEN;

        case PACIFIC_BOARD_ID:
            return DOG_NEED_OPEN;

        default:
            debug_log(DLOG_ERROR, "%s, The board id is %d , unknown.\r\n", __FUNCTION__, board_id);
            break;
    }
    return DOG_NONEED_OPERATE; 
}


void config_os_watch_dog(void)
{
    gint32 ret;
    guint8 state_now = WATCH_DOG_CLOSED;
    guint8 if_necessity;
    guint8 if_dft_model = check_dft_mode();
    
    if (if_dft_model) {
        if_necessity = DOG_NEED_CLOSE;
    } else {
        if_necessity = check_watch_dog_necessity();
    }

    ret = dal_read_cpld_bit(1, WATCH_DOG_CPLD_DIR, BIT0, &state_now);  
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s, call dal_read_cpld_bit failed. ret %d \r\n", __FUNCTION__, ret);
        return;
    }

    switch (if_necessity) {
        case DOG_NEED_OPEN:
            if (state_now == WATCH_DOG_OPEN) {
                debug_log(DLOG_INFO, "%s, no need to change the dog state to CPLD. The state now is: %d", __FUNCTION__,
                    state_now);
            } else {
                ret = dal_write_cpld_bit(1, WATCH_DOG_CPLD_DIR, BIT0, WATCH_DOG_OPEN);
            }
            break;

        case DOG_NEED_CLOSE:
            if (state_now == WATCH_DOG_CLOSED) {
                ret = dal_write_cpld_bit(1, WATCH_DOG_CPLD_DIR, BIT0, WATCH_DOG_CLOSED);
            } else {
                debug_log(DLOG_INFO, "%s, no need to change the dog state to CPLD. The state now is: %d", __FUNCTION__,
                    state_now);
            }
            break;

        case DOG_NONEED_OPERATE:

        default:
            debug_log(DLOG_ERROR, "%s, The if_necessity is %d, state now is %d. no operate.\r\n", __FUNCTION__,
                if_necessity, state_now);
            break;
    }

    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: set os dog (%d) to CPLD failed.", __FUNCTION__, ret);
    }
}

static DFT_RESULT_S g_os_reset_test_result = {};
static TASKID g_os_reset_test_task_id = 0;
static POS_RESET_PARA_S g_p_reset_par = NULL;


LOCAL gint32 dft_boardctrl_resetreason_analyse(guint8 reset_type, const gchar* prop_name)
{
    gint32 ret;
    guint8 reg_value;
    OBJ_HANDLE obj_handle = 0;

    debug_log(DLOG_ERROR, "Check %s reset reason start.", prop_name);

    ret = dfl_get_object_handle(OBJECT_STORPAYLOAD, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get %s object failed, ret(%d).", OBJECT_STORPAYLOAD, ret);
        return TEST_FAILED;
    }

    ret = dal_get_property_value_byte(obj_handle, prop_name, &reg_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get %s object failed in ctrlboard reset reason record.", prop_name);
        return TEST_FAILED;
    }

    if (reg_value == reset_type) {
        debug_log(DLOG_ERROR, "CtrlBoard reset reason: %x. meet expectations.", reg_value);
        return TEST_SUCCEED;
    }

    debug_log(DLOG_ERROR, "CtrlBoard reset reason: unknown %x. reset_type: %x", reg_value, reset_type);

    return TEST_FAILED;
}


LOCAL void dft_write_disk_power_state(guint8 power_state)
{
    gint32 ret;

    ret = set_storpayload_property_byte(PROPERTY_SYS_DISK0_POW_STATE, &power_state, DF_HW);
    ret += set_storpayload_property_byte(PROPERTY_SYS_DISK1_POW_STATE, &power_state, DF_HW);
    debug_log(DLOG_ERROR, "set system disk power state %d %s(ret %d).",
        power_state, (ret == RET_OK) ? "succeeded" : "failed", ret);

    return;
}


LOCAL void reset_fail_handle(guint8 test_type)
{
    gint32 ret;
    guint8 value = 0;

    g_os_reset_test_result.result = TEST_FAILED;
    g_os_reset_test_result.status = COMP_CODE_COMPLETE;

    if (g_p_reset_par) {
        g_free(g_p_reset_par);
        g_p_reset_par = NULL;
    }

    switch (test_type) {
        case DFT_WARM_RESET:
            break;

        case DFT_HARD_RESET:
            vos_task_delay(RESET_WAIT_TIME);                   
            dft_write_disk_power_state(SYS_DISK_POW_STATE_ON); 

            ret = get_storpayload_property_byte(PROPERTY_STORPAYLOAD_STANDBYSTATE, &value);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "get standby state failed.");
                return;
            }

            if (value == TRUE) {
                debug_log(DLOG_ERROR, "standby state %x, start power on.", value);
                ctrl_power_button_handle(CTRL_BUTTON_UP); 
            }
            break;

        default:
            return;
    }

    return;
}


LOCAL gint32 dft_system_disk_power_test(guint8 reset_system_disk)
{
    gint32 ret;
    guint8 state_disk1 = SYS_DISK_POW_STATE_OFF;
    guint8 state_disk0 = SYS_DISK_POW_STATE_OFF;
    guint32 disk_reset_count;

    if (reset_system_disk != DFT_SYSTEM_DISK_RESET_TEST) {
        vos_task_delay(RESET_WAIT_TIME); 
        return RET_OK;
    }

    for (disk_reset_count = NUMBER_0; disk_reset_count < SYS_DISK_RESET_MAX_TIME; disk_reset_count++) {
        
        dft_write_disk_power_state(SYS_DISK_POW_STATE_OFF);
        vos_task_delay(DELAY_1000_MSEC); 

        
        ret = get_storpayload_property_byte(PROPERTY_SYS_DISK0_POW_STATE, &state_disk0);
        ret += get_storpayload_property_byte(PROPERTY_SYS_DISK1_POW_STATE, &state_disk1);
        if ((ret != RET_OK) || (state_disk0 != SYS_DISK_POW_STATE_OFF) || (state_disk1 != SYS_DISK_POW_STATE_OFF)) {
            debug_log(DLOG_ERROR, "system disk power disk donot meet expectations[off]. disk0: %x, disk1: %x. ret :%d",
                state_disk0, state_disk1, ret);
            return TEST_FAILED;
        }

        
        dft_write_disk_power_state(SYS_DISK_POW_STATE_ON);
        vos_task_delay(DELAY_1000_MSEC); 

        
        ret = get_storpayload_property_byte(PROPERTY_SYS_DISK0_POW_STATE, &state_disk0);
        ret += get_storpayload_property_byte(PROPERTY_SYS_DISK1_POW_STATE, &state_disk1);
        if ((ret != RET_OK) || (state_disk0 != SYS_DISK_POW_STATE_ON) || (state_disk1 != SYS_DISK_POW_STATE_ON)) {
            debug_log(DLOG_ERROR, "system disk power disk donot meet expectations[on]. disk0: %x, disk1: %x. ret :%d",
                state_disk0, state_disk1, ret);
            return TEST_FAILED;
        }
    }

    return RET_OK;
}


LOCAL gint32 dft_os_hard_reset_action(guint8 reset_system_disk)
{
    guint8 value = 0;
    gint32 ret;

    ret = get_storpayload_property_byte(PROPERTY_STORPAYLOAD_STANDBYSTATE, &value);
    if (ret != RET_OK) {
        reset_fail_handle(DFT_HARD_RESET);
        debug_log(DLOG_ERROR, "get %s failed, return [%d].", PROPERTY_STORPAYLOAD_STANDBYSTATE, ret);
        return TEST_FAILED;
    }

    
    if (value == FALSE) {
        debug_log(DLOG_ERROR, "standby state %x, start power off.", value);
        ctrl_power_button_handle(CTRL_BUTTON_UP); 
    }

    
    if (dft_system_disk_power_test(reset_system_disk) != TEST_SUCCEED) { 
        reset_fail_handle(DFT_HARD_RESET);
        return TEST_FAILED;
    }

    ret = get_storpayload_property_byte(PROPERTY_STORPAYLOAD_STANDBYSTATE, &value);
    if (ret != RET_OK) {
        reset_fail_handle(DFT_HARD_RESET);
        debug_log(DLOG_ERROR, "get %s failed, return [%d].", PROPERTY_STORPAYLOAD_STANDBYSTATE, ret);
        return TEST_FAILED;
    }

    
    if (value == TRUE) {
        debug_log(DLOG_ERROR, "standby state %x, start power on.", value);
        ctrl_power_button_handle(CTRL_BUTTON_UP); 
    } else {
        return TEST_FAILED;
    }

    return TEST_SUCCEED;
}


LOCAL gint32 dft_get_bios_reset_flag(void)
{
    guint8 ram_data;
    guint16 ram_addr = CPLD_RAM_BIOS_START_ADDR;

    dal_get_cpld_ram(ram_addr, &ram_data);

    if (ram_data != CPLD_RAM_BIOS_START_DATA_55) {
        debug_log(DLOG_ERROR, "bios start flag addr: (0x%04x), ram_data(0x%x)", ram_addr, ram_data);
        return VOS_ERR;
    }

    return VOS_OK;
}

LOCAL gboolean is_hard_reset_reason_normal(void)
{
    gint32 ret;
    gint32 ret_last;

    
    if (is_arm_enable()) {
        ret = dft_boardctrl_resetreason_analyse(DFT_HARD_RESET_REASON, PROPERTY_CURRENT_RESTART_CAUSE);
        return (ret == TEST_SUCCEED);
    }

    
    ret = dft_boardctrl_resetreason_analyse(DFT_BIOS_WARM_RESET, PROPERTY_CURRENT_RESTART_CAUSE);
    if (ret != TEST_SUCCEED) {
        return FALSE;
    }

    ret = dft_boardctrl_resetreason_analyse(DFT_HARD_RESET_REASON, PROPERTY_LAST_RESTART_CAUSE);
    if (ret == TEST_SUCCEED) {
        return TRUE;
    }
    ret = dft_boardctrl_resetreason_analyse(DFT_BIOS_WARM_RESET, PROPERTY_LAST_RESTART_CAUSE);
    ret_last = dft_boardctrl_resetreason_analyse(DFT_HARD_RESET_REASON, PROPERTY_BEFORE_LAST_RESTART_CAUSE);
    return ((ret == TEST_SUCCEED) && (ret_last == TEST_SUCCEED));
}


LOCAL void dft_os_hard_reset_test(guint8 reset_times, guint8 reset_system_disk)
{
    guint32 os_reset_count = 0;
    guint32 time_out_cnt = 0;

    debug_log(DLOG_ERROR, "hard reset task start. reset_times %x. reset_system_disk %x ",
              reset_times, reset_system_disk);
    dal_set_cpld_ram(CPLD_RAM_BIOS_START_ADDR, 0x0);
    if (dft_os_hard_reset_action(reset_system_disk) != TEST_SUCCEED) {
        debug_log(DLOG_ERROR, "hard reset task failed. has test times: 0.");
        return;
    }

    while (1) {
        if (os_reset_count >= reset_times) {
            g_os_reset_test_result.result = TEST_SUCCEED;
            g_os_reset_test_result.status = COMP_CODE_COMPLETE;
            debug_log(DLOG_ERROR, "os hard reset test over times[%d], finish.", os_reset_count);
            return;
        }

        if (dft_get_bios_reset_flag() == RET_OK) {
            dal_set_cpld_ram(CPLD_RAM_BIOS_START_ADDR, 0x0);
            if (is_hard_reset_reason_normal() == FALSE) { 
                reset_fail_handle(DFT_HARD_RESET);
                return;
            }
            os_reset_count++;
            time_out_cnt = 0;  
            debug_log(DLOG_ERROR, "os hard reset test times[%d].", os_reset_count);

            if (dft_os_hard_reset_action(reset_system_disk) != TEST_SUCCEED) {
                reset_fail_handle(DFT_HARD_RESET);
                debug_log(DLOG_ERROR, "hard reset task failed. has test times: %d.", os_reset_count);
                return;
            }
        } else {
            vos_task_delay(1000); 
            time_out_cnt++;
            if (time_out_cnt >= 240) { 
                reset_fail_handle(DFT_HARD_RESET);
                debug_log(DLOG_ERROR, "BIOS start time out 4 minutes. test failed, has reset times: %d",
                          os_reset_count);
                return;
            }
        }
    }
}


LOCAL gint32 dft_os_warm_reset_first(guint8 reset_times, OBJ_HANDLE *obj_handle)
{
    gint32 ret = dal_get_object_handle_nth(CLASS_FRUPAYLOAD, 0, obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dal_get_object_handle_nth %s failed, ret(%d).", CLASS_FRUPAYLOAD, ret);
        return ret;
    }

    
    debug_log(DLOG_ERROR, "Os Soft Reset Task start. reset_times %d", reset_times);
    dal_set_cpld_ram(CPLD_RAM_BIOS_START_ADDR, 0x0);
    ret = dfl_call_class_method(*obj_handle, METHOD_COLD_RESET, NULL, NULL, NULL);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "First time to reset os failed, ret(%d).", ret);
    }

    return ret;
}


LOCAL void dft_os_warm_reset_test(guint8 reset_times)
{
    guint8 os_reset_count = 0;
    guint32 time_out_cnt = 0;
    OBJ_HANDLE obj_handle = 0;

    gint32 ret = dft_os_warm_reset_first(reset_times, &obj_handle);
    if (ret != RET_OK) {
        reset_fail_handle(DFT_WARM_RESET);
        return;
    }
    ret = dfl_call_class_method(obj_handle, METHOD_COLD_RESET, NULL, NULL, NULL);
    if (ret != RET_OK) {
        reset_fail_handle(DFT_WARM_RESET);
        debug_log(DLOG_ERROR, "First time to reset os failed, return[%d].", ret);
        return;
    }

    for (;;) {
        vos_task_delay(1000); 
        if (os_reset_count >= reset_times) {
            debug_log(DLOG_ERROR, "BMC reset os over times[%d], finish.", os_reset_count);
            g_os_reset_test_result.result = TEST_SUCCEED;
            g_os_reset_test_result.status = COMP_CODE_COMPLETE;
            return;
        }

        if (dft_get_bios_reset_flag() == RET_OK) {
            ret = dft_boardctrl_resetreason_analyse(DFT_WARM_RESET_REASON, PROPERTY_CURRENT_RESTART_CAUSE);
            if (ret != TEST_SUCCEED) { 
                reset_fail_handle(DFT_WARM_RESET);
                return;
            }

            os_reset_count++;
            time_out_cnt = 0; 
            debug_log(DLOG_ERROR, "BMC reset os times[%d].", os_reset_count);

            dal_set_cpld_ram(CPLD_RAM_BIOS_START_ADDR, 0x0);
            ret = dfl_call_class_method(obj_handle, METHOD_COLD_RESET, NULL, NULL, NULL);
            if (ret != RET_OK) {
                reset_fail_handle(DFT_WARM_RESET);
                debug_log(DLOG_ERROR, "BMC reset os times[%d] failed, return[%d].", os_reset_count, ret);
                return;
            }
        } else {
            time_out_cnt++;
            if (time_out_cnt >= 120) { 
                reset_fail_handle(DFT_WARM_RESET);
                debug_log(DLOG_ERROR, "BIOS start timeout, has reset times: %d", os_reset_count);
                return;
            }
        }
    }
}


LOCAL void os_reset_task(void)
{
    gint8 reset_type;
    gint8 reset_times;
    gint8 reset_system_disk;
    gint8 len_par;

    len_par = g_p_reset_par->para_num; 

    
    g_os_reset_test_result.status = COMP_CODE_TESTING;
    g_os_reset_test_result.result = TEST_NON;

    if (len_par < 4) { 
        reset_type = DFT_WARM_RESET;            // 热复位or上下电，默认进行热复位测试
        reset_times = DFT_WARM_RESET_MAX_TIMES; // 测试循环次数
        reset_system_disk = 0;                  // 默认不启动系统盘上下电
        debug_log(DLOG_ERROR, "not enough parameter %d. Start warm reset test default\n", len_par);
    } else {
        reset_type = g_p_reset_par->reset_type;               // 热复位or上下电
        reset_system_disk = g_p_reset_par->reset_system_disk; // 是否启动系统盘上下电测试
        reset_times = g_p_reset_par->reset_times;             // 测试循环次数
        debug_log(DLOG_ERROR, "reset_type: %x, reset_system_disk: %x, reset_times: %x, len_par: %d\n", reset_type,
            reset_system_disk, reset_times, len_par);
    }

    switch (reset_type) {
        case DFT_WARM_RESET:
            dft_os_warm_reset_test(reset_times);
            break;
        case DFT_HARD_RESET:
            dft_os_hard_reset_test(reset_times, reset_system_disk);
            break;
        default:
            debug_log(DLOG_ERROR, "unknown test type. unsupported %d\n", reset_type);
            break;
    }
    g_os_reset_test_task_id = 0;
    return;
}


LOCAL void dft_get_os_reset_result(DFT_RESULT_S *dft_data, gpointer user_data)
{
    gint32 ret;

    ret = memmove_s(dft_data, sizeof(DFT_RESULT_S), &g_os_reset_test_result, sizeof(DFT_RESULT_S));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "call memmove_s failed. ret %d\n", ret);
    }
}


LOCAL gint32 dft_start_os_reset_test(gsize para_size, gconstpointer para_data)
{
    gint32 ret;

    if ((para_size == NUMBER_0) || (para_data == NULL)) {
        debug_log(DLOG_ERROR, "input param error. \
            Os reset test must input [test_type, reset_system_disk, reset_times].\n");
        return VOS_ERR;
    }

    g_p_reset_par = (POS_RESET_PARA_S)g_malloc0(sizeof(OS_RESET_PARA_S));
    if (!g_p_reset_par) {
        reset_fail_handle(DFT_WARM_RESET);
        debug_log(DLOG_ERROR, "malloc g_p_reset_par failed");
        return VOS_ERR;
    }

    (void)memset_s(g_p_reset_par, sizeof(OS_RESET_PARA_S), 0, sizeof(OS_RESET_PARA_S)); 

    if (para_size < 4) { 
        ret = memcpy_s(g_p_reset_par, sizeof(OS_RESET_PARA_S), para_data,
            para_size); 
        if (ret != RET_OK) {
            reset_fail_handle(DFT_WARM_RESET);
            debug_log(DLOG_ERROR, "call memcpy_s fail. ret: %d\n", ret);
            return ret;
        }
        g_p_reset_par->para_num = (guint8)(para_size) + 1; 
    } else {
        reset_fail_handle(DFT_WARM_RESET);
        debug_log(DLOG_ERROR, "too many parameter para_size: %u.\n", (unsigned int)para_size);
        return VOS_ERR;
    }

    ret = vos_task_create(&g_os_reset_test_task_id, "dft_os_reset_test", (TASK_ENTRY)os_reset_task, NULL,
        DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        reset_fail_handle(DFT_WARM_RESET);
        debug_log(DLOG_ERROR, "pthread os reset test create fail. ret: %d\n", ret);
        return ret;
    }

    return RET_OK;
}


LOCAL gint32 dft_os_reset_test(guint32 command, gsize para_size, gconstpointer para_data, DFT_RESULT_S *dft_data,
    gpointer user_data)
{
    gint32 ret;

    if (dft_data == NULL) {
        debug_log(DLOG_ERROR, "input param error.\n");
        return VOS_ERR;
    }

    switch (command) {
        case TEST_START:
            
            if (g_os_reset_test_result.result == TEST_SUCCEED) {
                debug_log(DLOG_ERROR, "os reset test has success\n");
                return RET_OK;
            }

            ret = dft_start_os_reset_test(para_size, para_data);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "dft_start_os_reset_test start failed. ret: %d\n", ret);
            }

            break;

        case TEST_STOP:
            if (g_os_reset_test_task_id != 0) {
                (void)vos_task_delete(g_os_reset_test_task_id);
                g_os_reset_test_task_id = COMP_CODE_COMPLETE;
            }

            g_os_reset_test_result.status = COMP_CODE_UNSTART;
            g_os_reset_test_result.result = TEST_NON;

            break;

        case TEST_GET_RESULT:
            dft_get_os_reset_result(dft_data, user_data);
            break;

        default:
            return VOS_ERR;
    }
    return RET_OK;
}


gint32 register_os_reset_dft_item(OBJ_HANDLE handle, guint32 dft_id)
{
    gint32 retv;
    guint32 slot_id = 0;
    guint32 dev_id = 0;

    retv = dal_get_property_value_uint32(handle, SLOT_ID, &slot_id);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "dal_get_property_value_uint32 failed:obj_name is %s, retv is %d\n",
            dfl_get_object_name(handle), retv);
        return retv;
    }

    retv = dal_get_property_value_uint32(handle, DEV_ID, &dev_id);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "dal_get_property_value_uint32 failed:obj_name is %s, retv is %d\n",
            dfl_get_object_name(handle), retv);
        return retv;
    }

    
    retv = dfl_register_dft_item(dft_id, slot_id, dev_id, dft_os_reset_test, (gpointer)handle);
    debug_log(DLOG_DEBUG, "regist dft item[%d] result:%d\n", dft_id, retv);

    g_os_reset_test_result.result = TEST_NON;
    g_os_reset_test_result.status = COMP_CODE_UNSTART;

    return retv;
}


gint32 pp_method_dft_reset_os(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    guint list_len = g_slist_length(input_list);
    gpointer user_data = NULL;
    DFT_RESULT_S dft_data;
    gint32 ret;
    static gint32 is_inited = 0;

    if (is_inited == 0) {
        g_os_reset_test_result.result = TEST_NON;
        g_os_reset_test_result.status = COMP_CODE_UNSTART;
        is_inited = 1;
    }

    if (input_list == NULL || output_list == NULL || list_len <= 1) {
        debug_log(DLOG_ERROR, "[%s] param invalid, list_len = %d.", __FUNCTION__, list_len);
        return RET_ERR;
    }

    guint32 command = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 0)); // input list中第一个为命令
    debug_log(DLOG_DEBUG, "[%s] reset-os, len = %u, command = %u.", __FUNCTION__, list_len, command);

    gsize para_size = (gsize)list_len - 1;  //  input list中从第2个开始为真实的测试参数
    guint8 *para_data = (guint8*)g_malloc0(para_size);
    if (para_data == NULL) {
        debug_log(DLOG_ERROR, "[%s] malloc para_data memory failed.", __FUNCTION__);
        return RET_ERR;
    }

    for (gsize i = 0; i < para_size; i++) {
        para_data[i] = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, i + 1));
        debug_log(DLOG_DEBUG, "[%s] reset-os, para[%u] = 0x%x.", __FUNCTION__, (unsigned)i, para_data[i]);
    }

    (void)memset_s(&dft_data, sizeof(dft_data), 0, sizeof(dft_data));
    ret = dft_os_reset_test(command, para_size, (gconstpointer)para_data, &dft_data, user_data);
    debug_log(DLOG_DEBUG, "[%s] reset-os, dft_os_reset_test = %u.", __FUNCTION__, ret);

    *output_list = g_slist_append(*output_list, g_variant_new_int32(ret));
    for (int i = 0; i < sizeof(dft_data); i++) {
        *output_list = g_slist_append(*output_list, g_variant_new_byte(*((guint8 *)(&dft_data) + i)));
        debug_log(DLOG_DEBUG, "reset-os: ori dft_data[%u]=0x%x.", i, *((guint8 *)(&dft_data) + i));
    }
    g_free(para_data);

    return ret;
}


void bbu_running_state_operate(guint8 operate)
{
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0xff;

    if ((operate != OPEN_BBU) && (operate != CLOSE_BBU)) {
        debug_log(DLOG_ERROR, "Operate(%d) (1:kill 0:active) error.", operate);
        return;
    }

    gint32 ret = dal_get_object_handle_nth(CLASS_BBU_MODULE_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dfl_get_object_handle failed, result is %d", ret);
        return;
    }
    
    input_list = g_slist_append(input_list, g_variant_new_byte(operate));
    ret = dfl_call_class_method(obj_handle, METHOD_BBU_MODULE_OPERATE_BBU, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    debug_log(DLOG_ERROR, "Operate(%d) (1:kill 0:active) bbu result(%d).", operate, ret);

    return;
}


gint32 handle_sys_power_off_ipmi(gconstpointer req_msg, gpointer user_data)
{
    const POWER_CTRL_MODE_IPMI *power_ctrl_msg = NULL;
    
    power_ctrl_msg = (const POWER_CTRL_MODE_IPMI *)get_ipmi_src_data(req_msg);
    if (power_ctrl_msg == NULL) {
        debug_log(DLOG_ERROR, "get_ipmi_src_data req_data is NULL");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }

    if ((power_ctrl_msg->power_ctrl_mode == DAL_BOARD_ABNORMAL_DOWN) ||
        (power_ctrl_msg->power_ctrl_mode == DAL_SYS_ABNORMAL_DOWN)) {
        pp_set_power_off_normal_flag(FALSE);
        hop_set_power_before_ac_lost(1);
    } else {
        pp_set_power_off_normal_flag(TRUE);
    }

    push_button_for_bios_offcmd();
    bbu_running_state_operate(CLOSE_BBU);

    ipmi_operation_log(req_msg, "Sys power off successfully, ctrl_mode=%d.", power_ctrl_msg->power_ctrl_mode);

    return ipmi_send_simple_response(req_msg, COMP_CODE_SUCCESS);
}