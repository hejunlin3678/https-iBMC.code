
#include "pme/common/mscm_vos.h"
#include "pme/common/mscm_macro.h"
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "payload_hop.h"
#include "payload_hs.h"
#include "payload_pwr.h"
#include "payload_ipmi.h"
#include "payload_com.h"
#include "payload_storage.h"


LOCAL void pp_construct_picmg_resp(PPICMG_RESP_S ppicmg_resp, guchar compcode)
{
    ppicmg_resp->completion = compcode;
    ppicmg_resp->picmg_id = PICMG_IDENTIFIER;
}


gint32 pp_chassis_pwr_on(guchar src_chan_num, guchar restart_cause)
{
    guchar comp_code;
    guchar flag = 0;
    guint8 forbid_poweron_flag = 0;
    gint32 ret;

    if (get_vddq_check_forcepwroff_flag(&flag) == RET_OK) {
        if (flag != 0) {
            // 故障时，不允许上电
            debug_log(DLOG_ERROR, "get_vddq_check_forcepwroff_flag failed, flag=%d, can not power on", flag);
            return COMP_CODE_STATUS_INVALID;
        }
    }

    if ((is_support_storage_payload() == TRUE) && (is_pw_on_condit_satised() != RET_OK)) {
        debug_log(DLOG_ERROR, "Can not read power on condition, forbid power on.\r\n");
        return COMP_CODE_STATUS_INVALID;
    }

    
    ret = hop_get_forbid_poweron_flag(&forbid_poweron_flag);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s hop_get_forbid_poweron_flag failed, ret = %d", __FUNCTION__, ret);
        return COMP_CODE_UNKNOWN;
    }

    if (forbid_poweron_flag == UPGRADE_BIOS_FORBID_POWER_ON) {
        debug_log(DLOG_ERROR, "BIOS upgrade forbid power on.");
        
        return POWER_ON_FAILED_BY_BIOS_UP;
    } else if (forbid_poweron_flag == UPGRADE_CPLD_FORBID_POWER_ON) {
        debug_log(DLOG_ERROR, "CPLD upgrade forbid power on.");
        return POWER_ON_FAILED_BY_CPLD_UP;
    } else if (forbid_poweron_flag == UPGRADE_VRD_FORBID_POWER_ON) {
        debug_log(DLOG_ERROR, "VRD upgrade forbid power on.");
        return POWER_ON_FAILED_BY_VRD_UP;
    }

    
    ret = pp_get_imuforbid_pwron_flag(&flag);
    if (ret == RET_OK && flag != FALSE) {
        debug_log(DLOG_ERROR, "CPU high temperature forbid power on(%d).", flag);
        return COMP_CODE_STATUS_INVALID;
    }
    if (hs_is_auto_management()) {
        debug_log(DLOG_ERROR, "Fru is auto-managed, power on.");
        comp_code = pp_fru_pwr_ctrl(CHASSIS_FRU_ID, POWER_ON);
    } else {
        comp_code = hse_fru_activate_policy(CHASSIS_FRU_ID, 1, 0);
    }

    if (comp_code == COMP_CODE_SUCCESS) {
        pp_set_restart_action(src_chan_num, restart_cause);
        pp_set_restart_cause();
    }

    return comp_code;
}


guchar pp_chassis_pwr_cycle(guchar src_chan_num, guchar restart_cause)
{
    guchar comp_code;

    if (pp_get_pwr_state(CHASSIS_FRU_ID) == PAYLOAD_POWER_STATE_OFF) { 
        comp_code = COMP_CODE_STATUS_INVALID;
    } else {
        comp_code = pp_fru_pwr_ctrl(CHASSIS_FRU_ID, POWER_CYCLE);
        if (comp_code == COMP_CODE_SUCCESS) {
            pp_set_restart_action(src_chan_num, restart_cause);
            pp_set_restart_cause();
        }
    }

    return comp_code;
}


guchar pp_chassis_pwr_cycle_from_wd(guchar src_chan_num)
{
    guchar comp_code;

    if (pp_get_pwr_state(CHASSIS_FRU_ID) == PAYLOAD_POWER_STATE_OFF) { 
        comp_code = COMP_CODE_STATUS_INVALID;
    } else {
        comp_code = pp_fru_pwr_ctrl(CHASSIS_FRU_ID, POWER_CYCLE);
    }

    return comp_code;
}


LOCAL void set_ierr_diag_fail_flag(gboolean flag)
{
    OBJ_HANDLE pfn_handle = 0;
    GSList *input_list = NULL;
    gint32 ret;

    (void)dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &pfn_handle);
    input_list = g_slist_append(input_list, g_variant_new_byte(flag));
    ret = dfl_call_class_method(pfn_handle, METHOD_PFN_SET_IERR_DIAG_FAIL_FLAG, NULL, input_list, NULL);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call method to set ierr-diag-fail flag failed, ret is %d.", __func__, ret);
    }
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
}


guchar pp_chassis_hard_reset(guchar src_chan_num, guchar restart_cause)
{
    guchar comp_code;
    

    if (pp_get_pwr_state(CHASSIS_FRU_ID) == PAYLOAD_POWER_STATE_OFF ||
        
        ((hop_get_bbu_present() == TRUE) && (is_dft_enable_status() == FALSE))) {
        comp_code = COMP_CODE_STATUS_INVALID;
    } else {
        if (restart_cause == RESTART_CAUSE_OEM_IERR_DIAG_FAIL) {
            set_ierr_diag_fail_flag(TRUE);
        }
        hop_chassis_hard_reset();
        pp_set_restart_action(src_chan_num, restart_cause);
        pp_set_restart_cause();
        comp_code = COMP_CODE_SUCCESS;
    }

    return comp_code;
}


guchar pp_chassis_nmi_interrupt(void)
{
    (void)hop_trigger_nmi_interrupt();
    return COMP_CODE_SUCCESS;
}


gint32 pp_ipmi_cmd_set_fru_activation_policy(const void *req_msg, gpointer user_data)
{
    guint8 src_chan_num;
    guchar result;
    PICMG_RESP_S resp_data;
    const guint8 *req_src_data = NULL;
    const SET_ACTIVATION_POLICY_REQ_S *set_activation_policy_req = NULL;
    gint32 ret;
    guchar flag = 0;

    if (req_msg == NULL) {
        return RET_ERR;
    }

    req_src_data = get_ipmi_src_data(req_msg);
    if (req_src_data == NULL) {
        ipmi_operation_log(req_msg, "get_ipmi_src_data req_src_data is NULL.\n");
        debug_log(DLOG_ERROR, "get_ipmi_src_data  req_src_data is NULL.\n");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }
    set_activation_policy_req = (const SET_ACTIVATION_POLICY_REQ_S *)req_src_data;

    // 解锁M1->M2，先判断缓起电路是否故障
    if ((set_activation_policy_req->lock_bit_mask & PBIT0) && ((set_activation_policy_req->lock_bit & PBIT0) == 0)) {
        if ((get_vddq_check_forcepwroff_flag(&flag) == RET_OK) && (flag != 0)) {
            // 故障时，不允许上电
            debug_log(DLOG_ERROR, "get_vddq_check_forcepwroff_flag failed, flag=%d, can not power on", flag);
            result = COMP_CODE_STATUS_INVALID;
            goto _out;
        }
    }

    ret = pp_check_active_option(ACTIVATE_DEACTIVATE_FRU, set_activation_policy_req->fru_id);
    if (ret != RET_OK) {
        ipmi_operation_log(req_msg, "Set FRU%u activation policy failed\r\n", set_activation_policy_req->fru_id);
        return ipmi_send_simple_response(req_msg, COMP_CODE_STATUS_INVALID);
    }

    pp_construct_picmg_resp(&resp_data, COMP_CODE_SUCCESS);

    src_chan_num = get_ipmi_chan_num(req_msg);
    if (set_activation_policy_req->lock_bit_mask & PBIT0) {
        pp_set_restart_action(src_chan_num, RESTART_CAUSE_CHASSISCONTROL);
        pp_set_restart_cause();
    }

    result = hse_fru_activate_policy(set_activation_policy_req->fru_id, set_activation_policy_req->lock_bit_mask,
        set_activation_policy_req->lock_bit);

_out:
    if (result == COMP_CODE_SUCCESS) {
        ipmi_operation_log(req_msg, "Set FRU%u activation policy to (mask:0x%02x|bits:0x%02x) successfully\r\n",
            set_activation_policy_req->fru_id, set_activation_policy_req->lock_bit_mask,
            set_activation_policy_req->lock_bit);
        
        return ipmi_send_response(req_msg, 2, (guint8 *)&resp_data);
    } else {
        ipmi_operation_log(req_msg, "Set FRU%u activation policy failed\r\n", set_activation_policy_req->fru_id);
        return ipmi_send_simple_response(req_msg, result);
    }
}


gint32 pp_ipmi_cmd_get_fru_activation_policy(const void *req_msg, gpointer user_data)
{
    guchar result;
    PICMG_RESP_S resp_data;
    const guint8 *req_src_data = NULL;
    const GET_ACTIVATION_POLICY_REQ_S *get_activation_policy_req = NULL;
    gint32 ret;

    if (req_msg == NULL) {
        return RET_ERR;
    }

    req_src_data = get_ipmi_src_data(req_msg);
    if (req_src_data == NULL) {
        ipmi_operation_log(req_msg, "get_ipmi_src_data req_src_data is NULL.");
        debug_log(DLOG_ERROR, "get_ipmi_src_data  req_src_data is NULL.\n");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }
    get_activation_policy_req = (const GET_ACTIVATION_POLICY_REQ_S *)req_src_data;

    ret = pp_check_active_option(ACTIVATE_DEACTIVATE_FRU, get_activation_policy_req->fru_id);
    if (ret != RET_OK) {
        return ipmi_send_simple_response(req_msg, COMP_CODE_STATUS_INVALID);
    }

    pp_construct_picmg_resp(&resp_data, COMP_CODE_SUCCESS);
    result = hs_get_activate_policy(get_activation_policy_req->fru_id, &resp_data.data[0]);
    if (result == COMP_CODE_SUCCESS) {
        
        return ipmi_send_response(req_msg, 3, (guint8 *)&resp_data);
    } else {
        return ipmi_send_simple_response(req_msg, result);
    }
}


gint32 pp_ipmi_cmd_set_fru_activation(const void *req_msg, gpointer user_data)
{
    guchar result;
    PICMG_RESP_S resp_data;
    const guint8 *req_src_data = NULL;
    const SET_ACTIVATION_REQ_S *get_activation_req = NULL;
    gint32 ret;

    if (req_msg == NULL) {
        return RET_ERR;
    }

    req_src_data = get_ipmi_src_data(req_msg);
    if (req_src_data == NULL) {
        ipmi_operation_log(req_msg, "get_ipmi_src_data req_src_data is NULL.");
        debug_log(DLOG_ERROR, "get_ipmi_src_data  req_src_data is NULL.\n");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }
    get_activation_req = (const SET_ACTIVATION_REQ_S *)req_src_data;
    ret = pp_check_active_option(ACTIVATE_DEACTIVATE_FRU, get_activation_req->fru_id);
    if (ret != RET_OK) {
        ipmi_operation_log(req_msg, "Set FRU%u activation failed", get_activation_req->fru_id);
        return ipmi_send_simple_response(req_msg, COMP_CODE_STATUS_INVALID);
    }

    pp_construct_picmg_resp(&resp_data, COMP_CODE_SUCCESS);

    result = hse_fru_activate(get_activation_req->fru_id, get_activation_req->activation, TRUE);
    if (result == COMP_CODE_SUCCESS) {
        ipmi_operation_log(req_msg, "Set FRU%u activation to (%s) successfully", get_activation_req->fru_id,
            (get_activation_req->activation == HOTSWAP_ACTIVATE_FRU) ? "active" : "deactive");
        
        return ipmi_send_response(req_msg, 2, (guint8 *)&resp_data);
    } else {
        ipmi_operation_log(req_msg, "Set FRU%u activation failed", get_activation_req->fru_id);
        return ipmi_send_simple_response(req_msg, result);
    }
}


gint32 pp_ipmi_cmd_set_pwr_restore_policy(const void *req_msg, gpointer user_data)
{
    guchar restore_policy;
    guchar resp_data[2]; 
    const guint8 *req_src_data = NULL;
    gint32 ret;

    if (req_msg == NULL) {
        return RET_ERR;
    }

    req_src_data = get_ipmi_src_data(req_msg);
    if (req_src_data == NULL) {
        dal_ipmi_log_with_bios_set_check(req_msg, MLOG_ERROR, FC__PUBLIC_OK, "Set power restore policy failed");
        debug_log(DLOG_ERROR, "get_ipmi_src_data req_src_data is NULL\n");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }
    restore_policy = req_src_data[0];
    resp_data[0] = COMP_CODE_SUCCESS;

    ret = pp_allow_set_power_restore_policy();
    if (ret != RET_OK) {
        dal_ipmi_log_with_bios_set_check(req_msg, MLOG_ERROR, FC__PUBLIC_OK, "do not support set power restore policy");
        return ipmi_send_simple_response(req_msg, COMP_CODE_STATUS_INVALID);
    }

    switch (restore_policy) {
        case POWER_RESTORE_POLICY_NO_CHANGED:
            resp_data[1] = (guchar)(1 << pp_get_restore_policy());
            dal_ipmi_log_with_bios_set_check(req_msg, MLOG_INFO, FC__PUBLIC_OK,
                "Set power restore policy to (%s) successfully", pp_policy_to_string(restore_policy));
            debug_log(DLOG_DEBUG, "power restore policy is not changed");
            break;

        case POWER_RESTORE_POLICY_ALWAYS_ON:
        case POWER_RESTORE_POLICY_PREVIOUS:
        case POWER_RESTORE_POLICY_ALWAYS_OFF:
            pp_set_restore_policy(restore_policy);
            resp_data[1] = (guchar)(1 << restore_policy);

            dal_ipmi_log_with_bios_set_check(req_msg, MLOG_INFO, FC__PUBLIC_OK,
                "Set power restore policy to (%s) successfully", pp_policy_to_string(restore_policy));
            break;

        default:

            dal_ipmi_log_with_bios_set_check(req_msg, MLOG_ERROR, FC__PUBLIC_OK, "Set power restore policy failed");
            return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }
    
    return ipmi_send_response(req_msg, 2, resp_data);
}


gint32 pp_ipmi_cmd_set_pwr_cycle_interval(const void *req_msg, gpointer user_data)
{
    const guint8 *req_src_data = NULL;
    if (req_msg == NULL) {
        return RET_ERR;
    }

    req_src_data = get_ipmi_src_data(req_msg);
    if (req_src_data == NULL) {
        ipmi_operation_log(req_msg, "get_ipmi_src_data req_src_data is error.\n");
        debug_log(DLOG_ERROR, "get_ipmi_src_data req_src_data is NULL\n");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }
    pp_set_power_cycle_interval(req_src_data[0]);
    ipmi_operation_log(req_msg, "Set power cycle interval to (%u) seconds successfully\r\n", req_src_data[0]);
    return ipmi_send_simple_response(req_msg, COMP_CODE_SUCCESS);
}


gint32 pp_ipmi_cmd_get_sys_restart_cause(const void *req_msg, gpointer user_data)
{
    guchar respData[3]; 
    guchar restart_cause = 0;
    guchar restart_src_chan = 0;

    if (req_msg == NULL) {
        return RET_ERR;
    }

    pp_get_restart_cause(&restart_src_chan, &restart_cause);

    respData[0] = COMP_CODE_SUCCESS;
    respData[1] = restart_cause;    
    respData[2] = restart_src_chan; 
    
    return ipmi_send_response(req_msg, 3, (guint8 *)respData);
}


gint32 pp_ipmi_oem_get_sys_restart_cause(const void *req_msg, gpointer user_data)
{
    gint32 ret_val;
    const guint8 *req_src_data = NULL;
    guchar respData[6] = {0}; 
    guchar restart_cause = 0;
    guchar restart_src_chan = 0;
    guchar fru_id;

    if (req_msg == NULL) {
        return RET_ERR;
    }

    req_src_data = get_ipmi_src_data(req_msg);
    if (req_src_data == NULL) {
        debug_log(DLOG_ERROR, "get_ipmi_src_data  req_src_data is NULL");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }
    fru_id = req_src_data[4]; 

    
    ret_val = pp_judge_manu_id_valid_bios(req_src_data);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, DB_STR_MUNU_ID_ERR, ret_val);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }
    if (fru_id == CHASSIS_FRU_ID) {
        pp_get_restart_cause(&restart_src_chan, &restart_cause);
    } else {
        ret_val = payload_get_xcu_restart_cause(&restart_src_chan, &restart_cause, fru_id);
    }

    if (ret_val != RET_OK) {
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    respData[0] = COMP_CODE_SUCCESS;
    respData[1] = req_src_data[0];  
    respData[2] = req_src_data[1];  
    respData[3] = req_src_data[2];  
    respData[4] = restart_cause;    
    respData[5] = restart_src_chan; 
    return ipmi_send_response(req_msg, sizeof(respData), (guint8 *)respData);
}


LOCAL gchar *pp_chassis_cmd_to_string(guchar control_cmd)
{
    switch (control_cmd) {
        case CHASSIS_POWEROFF:
            return "power down";

        case CHASSIS_POWERON:
            return "power on";

        case CHASSIS_POWERCYCLE:
            return "power off the service system, and then power it on";

        case CHASSIS_HARD_RESET:
            return "reset";

        case CHASSIS_DIAG_INT:
            return "diagnostic interrupt";

        case CHASSIS_SOFT_OFF:
            return "soft-shutdown";

        default:
            return "unknow";
    }
}


gint32 pp_ipmi_cmd_chassis_ctrl(const void *req_msg, gpointer user_data)
{
    guchar comp_code;
    const guint8 *req_src_data = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guint32 event_data;

    if (req_msg == NULL) {
        return RET_ERR;
    }

    req_src_data = get_ipmi_src_data(req_msg);
    if (req_src_data == NULL) {
        ipmi_operation_log(req_msg, "get_ipmi_src_data req_src_data is error.\n");
        debug_log(DLOG_ERROR, "get_ipmi_src_data req_src_data is NULL\n");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }
    guint8 src_chan_num = get_ipmi_chan_num(req_msg);
    guchar control = (guchar)(req_src_data[0] & 0x0f);

    ret = pp_check_chassis_ctl_option(control, CHASSIS_FRU_ID);
    if (ret != RET_OK) {
        ipmi_operation_log(req_msg, "Set chassis control to (%s) failed\r\n", pp_chassis_cmd_to_string(control));
        comp_code = COMP_CODE_STATUS_INVALID;
        return ipmi_send_simple_response(req_msg, comp_code);
    }

    switch (control) {
        case CHASSIS_POWEROFF:
            // 取消已经开启的安全重启或POWER循环
            pp_set_cycle_progress_state(CHASSIS_FRU_ID, FALSE);
            pp_set_cycle_interval(CHASSIS_FRU_ID, 0);
            pp_save_cycle_param();
            comp_code = pp_fru_pwr_ctrl(CHASSIS_FRU_ID, POWER_OFF_FORCE);
            break;

        case CHASSIS_POWERON:
            pp_set_power_on_by_ipmi_flag(TRUE);
            comp_code = (guchar)pp_chassis_pwr_on(src_chan_num, RESTART_CAUSE_CHASSISCONTROL);
            break;

        case CHASSIS_POWERCYCLE:
            comp_code = pp_chassis_pwr_cycle(src_chan_num, RESTART_CAUSE_CHASSISCONTROL);
            break;

        case CHASSIS_HARD_RESET:
            comp_code = pp_chassis_hard_reset(src_chan_num, RESTART_CAUSE_CHASSISCONTROL);
            break;

        case CHASSIS_DIAG_INT:
            comp_code = pp_chassis_nmi_interrupt();
            break;

        case CHASSIS_SOFT_OFF:
            // 取消已经开启的安全重启或POWER循环
            pp_set_cycle_progress_state(CHASSIS_FRU_ID, FALSE);
            pp_set_cycle_interval(CHASSIS_FRU_ID, 0);
            pp_save_cycle_param();
            comp_code = pp_fru_pwr_ctrl(CHASSIS_FRU_ID, POWER_OFF);
            break;

        default:
            ipmi_operation_log(req_msg, "Set chassis control failed, unknown control action\r\n");
            comp_code = COMP_CODE_INVALID_FIELD;
            return ipmi_send_simple_response(req_msg, comp_code);
    }
    ipmi_operation_log(req_msg, "Set chassis control to (%s) %s", pp_chassis_cmd_to_string(control),
        (comp_code == RET_OK) ? "successfully" : "failed");
    ret = dal_get_specific_object_byte(CLASS_CHASSISPAYLOAD, PROPERTY_PAYLOAD_FRUID, CHASSIS_FRU_ID, &obj_handle);
    if (ret == RET_OK) {
        event_data = MAKE_DWORD(0x01, 0xff, control, 0x00);
        ret = dal_set_property_value_uint32(obj_handle, PROPERTY_PAYLOAD_CHASSIS_CTRL_ACTION, event_data, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Set %s property %s to %d failed.", dfl_get_object_name(obj_handle),
                PROPERTY_PAYLOAD_CHASSIS_CTRL_ACTION, control);
        }
        
        vos_task_delay(100);

        event_data = MAKE_DWORD(0x00, 0xff, control, 0x00);
        ret = dal_set_property_value_uint32(obj_handle, PROPERTY_PAYLOAD_CHASSIS_CTRL_ACTION, event_data, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Set %s property %s to %d failed.", dfl_get_object_name(obj_handle),
                PROPERTY_PAYLOAD_CHASSIS_CTRL_ACTION, control);
        }
    }

    return ipmi_send_simple_response(req_msg, comp_code);
}


LOCAL gint32 pp_ipmi_cmd_fru_ctrl_warm_reset(const void *req_msg, guchar fru_dev_id)
{
    guint8 src_chan_num;
    guchar result;
    PICMG_RESP_S resp_data;

    if (req_msg == NULL) {
        return RET_ERR;
    }

    if (fru_dev_id >= hs_get_fru_num()) {
        ipmi_operation_log(req_msg, PL_OP_LOG_HARD_RST_F, fru_dev_id);
        debug_log(DLOG_ERROR, "fru_dev_id(%u) is out of range, max_fru_id = %u.", fru_dev_id, hs_get_fru_num() - 1);
        return ipmi_send_simple_response(req_msg, COMP_CODE_OUTOF_RANGE);
    }

    pp_construct_picmg_resp(&resp_data, COMP_CODE_SUCCESS);
    src_chan_num = get_ipmi_chan_num(req_msg);
    if (pp_get_pwr_state(fru_dev_id) == PAYLOAD_POWER_STATE_OFF) { 
        ipmi_operation_log(req_msg, PL_OP_LOG_HARD_RST_F, fru_dev_id);
        return ipmi_send_simple_response(req_msg, COMP_CODE_STATUS_INVALID);
    }
    result = hop_fru_warm_reset(fru_dev_id, src_chan_num, RESTART_CAUSE_CHASSISCONTROL);
    if (result == COMP_CODE_SUCCESS) {
        ipmi_operation_log(req_msg, PL_OP_LOG_HARD_RST_T, fru_dev_id);
        
        return ipmi_send_response(req_msg, 2, (guint8 *)&resp_data);
    } else {
        ipmi_operation_log(req_msg, PL_OP_LOG_HARD_RST_F, fru_dev_id);
        return ipmi_send_simple_response(req_msg, result);
    }
}


LOCAL gint32 pp_ipmi_cmd_fru_ctrl_cold_reset(const void *req_msg, guchar fru_dev_id)
{
    guint8 src_chan_num;
    guchar result;
    PICMG_RESP_S resp_data;

    if (req_msg == NULL) {
        return RET_ERR;
    }
    if (fru_dev_id >= hs_get_fru_num()) {
        ipmi_operation_log(req_msg, PL_OP_LOG_RST_F, fru_dev_id);
        debug_log(DLOG_ERROR, "fru_dev_id(%u) is out of range, max_fru_id = %u.", fru_dev_id, hs_get_fru_num() - 1);
        return ipmi_send_simple_response(req_msg, COMP_CODE_OUTOF_RANGE);
    }

    pp_construct_picmg_resp(&resp_data, COMP_CODE_SUCCESS);
    src_chan_num = get_ipmi_chan_num(req_msg);
    if (pp_get_pwr_state(fru_dev_id) == PAYLOAD_POWER_STATE_OFF) {
        ipmi_operation_log(req_msg, PL_OP_LOG_RST_F, fru_dev_id);
        return ipmi_send_simple_response(req_msg, COMP_CODE_STATUS_INVALID);
    }

    result = hop_reset(fru_dev_id);
    if (result == COMP_CODE_SUCCESS) {
        
        pp_set_restart_action(src_chan_num, RESTART_CAUSE_CHASSISCONTROL);
        pp_set_restart_cause();

        ipmi_operation_log(req_msg, PL_OP_LOG_RST_T, fru_dev_id);
        
        return ipmi_send_response(req_msg, 2, (guint8 *)&resp_data);
    } else {
        ipmi_operation_log(req_msg, PL_OP_LOG_RST_F, fru_dev_id);
        return ipmi_send_simple_response(req_msg, COMP_CODE_STATUS_INVALID);
    }
}


LOCAL guchar pp_ipmi_cmd_fru_ctrl_retrans(guint8 last_rq_seq, guint8 rq_seq, gulong last_req_timestamp,
    gulong req_timestamp)
{
#define MAX_TIME 3000
    if ((last_rq_seq != 0xFF && last_rq_seq == rq_seq) &&
        (((req_timestamp >= last_req_timestamp) && (req_timestamp - last_req_timestamp < MAX_TIME)) ||
        ((req_timestamp < last_req_timestamp) && (G_MAXUINT32 - last_req_timestamp + req_timestamp < MAX_TIME)))) {
        return TRUE;
    } else {
        return FALSE;
    }
}


LOCAL gint32 pp_ipmi_cmd_fru_ctrl_graceful_reboot(const void *req_msg, guchar fru_dev_id)
{
    guint8 src_chan_num;
    guchar result;
    PICMG_RESP_S resp_data;
    static guint8 last_rq_seq = 0xFF;
    static gulong last_req_timestamp = 0;
    guint8 rq_seq;
    gulong req_timestamp;
    guint8 rq_target;
    guchar is_retrans;
    gint32 retv;

    if (req_msg == NULL) {
        return RET_ERR;
    }

    pp_construct_picmg_resp(&resp_data, COMP_CODE_SUCCESS);
    src_chan_num = get_ipmi_chan_num(req_msg);
    if (fru_dev_id != CHASSIS_FRU_ID) {
        ipmi_operation_log(req_msg, PL_OP_LOG_COLD_RST_F, fru_dev_id);
        retv = ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
        return retv;
    }

    if (pp_get_pwr_state(CHASSIS_FRU_ID) == PAYLOAD_POWER_STATE_OFF) {
        ipmi_operation_log(req_msg, PL_OP_LOG_COLD_RST_F, fru_dev_id);
        retv = ipmi_send_simple_response(req_msg, COMP_CODE_STATUS_INVALID);
        return retv;
    }

    result = hop_powercycle(fru_dev_id);
    if (result == COMP_CODE_SUCCESS) {
        pp_set_restart_action(src_chan_num, RESTART_CAUSE_CHASSISCONTROL);
        ipmi_operation_log(req_msg, PL_OP_LOG_COLD_RST_T, fru_dev_id);

        rq_target = get_ipmi_target_type(req_msg);
        if (rq_target == IPMI_SMM) {
            last_req_timestamp = vos_tick_get();
            last_rq_seq = get_ipmi_src_seq(req_msg);
        }
        
        return ipmi_send_response(req_msg, 2, (guint8 *)&resp_data);
    } else if (result == COMP_CODE_BUSY) {
        // 3秒之内如果sequence相同，则认为是MM的重试命令
        rq_target = get_ipmi_target_type(req_msg);
        if (rq_target == IPMI_SMM) {
            req_timestamp = vos_tick_get();
            rq_seq = get_ipmi_src_seq(req_msg);
            is_retrans = pp_ipmi_cmd_fru_ctrl_retrans(last_rq_seq, rq_seq, last_req_timestamp, req_timestamp);
            if ((is_retrans == TRUE)) {
                return ipmi_send_response(req_msg, 2, (guint8 *)&resp_data); 
            }
        }
        ipmi_operation_log(req_msg, PL_OP_LOG_COLD_RST_F, fru_dev_id);
        return ipmi_send_simple_response(req_msg, result);
    } else {
        ipmi_operation_log(req_msg, PL_OP_LOG_COLD_RST_F, fru_dev_id);
        return ipmi_send_simple_response(req_msg, result);
    }
}


LOCAL gint32 pp_ipmi_cmd_fru_ctrl_issuediagnosticinterrupt(const void *req_msg, guchar fru_dev_id)
{
    guchar result;
    if (req_msg == NULL) {
        return RET_ERR;
    }

    result = hop_diag_interrupt(fru_dev_id);
    if (result == COMP_CODE_SUCCESS) {
        ipmi_operation_log(req_msg, PL_OP_LOG_DIAG_INTR_T, fru_dev_id);
    } else {
        ipmi_operation_log(req_msg, PL_OP_LOG_DIAG_INTR_F, fru_dev_id);
    }
    return ipmi_send_simple_response(req_msg, result);
}

gint32 pp_ipmi_cmd_fru_ctrl(const void *req_msg, gpointer user_data)
{
    guchar fru_dev_id;
    guchar option;
    gint32 retv;
    const guint8 *req_src_data = NULL;

    if (req_msg == NULL) {
        return RET_ERR;
    }

    req_src_data = get_ipmi_src_data(req_msg);
    if (req_src_data == NULL) {
        ipmi_operation_log(req_msg, "get_ipmi_src_data req_src_data is NULL.");
        debug_log(DLOG_ERROR, "get_ipmi_src_data  req_src_data is NULL.\n");
        retv = ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
        return retv;
    }
    fru_dev_id = req_src_data[1]; 
    option = req_src_data[2];     
    switch (option) {
        case FRU_CONTROL_WARM_RESET:
            return pp_ipmi_cmd_fru_ctrl_warm_reset(req_msg, fru_dev_id);

        case FRU_CONTROL_COLD_RESET:
            return pp_ipmi_cmd_fru_ctrl_cold_reset(req_msg, fru_dev_id);

        case FRU_CONTROL_GRACEFUL_REBOOT:
            return pp_ipmi_cmd_fru_ctrl_graceful_reboot(req_msg, fru_dev_id);

        case FRU_CONTROL_ISSUEDIAGNOSTICINTERRUPT:
            return pp_ipmi_cmd_fru_ctrl_issuediagnosticinterrupt(req_msg, fru_dev_id);

        default:
            debug_log(DLOG_ERROR, "fru control (%u) is undefined.", option);
            return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }
}


gint32 pp_ipmi_cmd_set_power_off_timeout(const void *req_msg, gpointer user_data)
{
    guint32 pwr_off_timeout;
    const guint8 *req_src_data = NULL;

    if (req_msg == NULL) {
        return RET_ERR;
    }

    req_src_data = get_ipmi_src_data(req_msg);
    if (req_src_data == NULL) {
        ipmi_operation_log(req_msg, "get_ipmi_src_data req_src_data is NULL.\n");
        debug_log(DLOG_ERROR, "get_ipmi_src_data req_src_data is NULL\n");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }
    
    pwr_off_timeout = (((guint32)req_src_data[2]) << 8) + ((guint32)req_src_data[1]);
    if ((pwr_off_timeout != 0) && (pp_judge_poweroff_tm_valid(pwr_off_timeout) != PAYLOAD_SUCCESS)) {
        ipmi_operation_log(req_msg, "Set graceful shutdown timeout seconds failed\r\n");
        debug_log(DLOG_ERROR, "pp_judge_poweroff_tm_valid fail.\r\n");
        return ipmi_send_simple_response(req_msg, COMP_CODE_OUTOF_RANGE);
    }
    
    pwr_off_timeout = pwr_off_timeout / 10;

    pp_set_pwr_off_timeout(pwr_off_timeout);
    ipmi_operation_log(req_msg, "Set graceful shutdown timeout to (%u) seconds successfully\r\n", pwr_off_timeout);
    return ipmi_send_simple_response(req_msg, COMP_CODE_SUCCESS);
}

gchar *pp_delay_mode_to_string(guint8 delay_mode)
{
    switch (delay_mode) {
        case DELAY_MODE_SHORT:
            return POWER_ON_DELAY_DEFAULTMODE;

        case DELAY_MODE_HALF:
            return POWER_ON_DELAY_HALFMODE;

        case DELAY_MODE_ALL:
            return POWER_ON_DELAY_FIXEDMODE;

        case DELAY_MODE_RANDOM:
            return POWER_ON_DELAY_RANDOMMODE;

        default:
            return INVALID_DATA_STRING_VALUE;
    }
}


gint32 pp_ipmi_cmd_set_power_on_delay(const void *req_msg, gpointer user_data)
{
    guint32 iana;
    guint32 delay_count;
    const guint8 *req_src_data = NULL;
    guint8 pwr_on_delay_mode;
    guint32 manu_id;

    if (req_msg == NULL) {
        return RET_ERR;
    }

    req_src_data = get_ipmi_src_data(req_msg);
    if (req_src_data == NULL) {
        ipmi_operation_log(req_msg, "get_ipmi_src_data  req_src_data is NULL .\n");
        debug_log(DLOG_ERROR, "%s: get_ipmi_src_data req_src_data is NULL.\n", __FUNCTION__);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    manu_id = dal_get_manu_id();

    
    iana = MAKE_DWORD(0, req_src_data[3], req_src_data[2], req_src_data[1]);
    if (dal_check_manu_id(manu_id, iana) != TRUE) {
        ipmi_operation_log(req_msg, "Set power up delay failed\r\n");
        debug_log(DLOG_ERROR, "%s: iana=%u\r\n", __FUNCTION__, iana);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }
    
    pwr_on_delay_mode = req_src_data[4];
    if (pwr_on_delay_mode >= DELAY_MODE_INVALID) {
        ipmi_operation_log(req_msg, "Set power up delay failed\r\n");
        debug_log(DLOG_ERROR, "%s: pwr_on_delay_mode=%u\r\n", __FUNCTION__, pwr_on_delay_mode);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }
    
    delay_count = MAKE_DWORD(req_src_data[8], req_src_data[7], req_src_data[6], req_src_data[5]);

    pp_set_pwr_on_delay_param(pwr_on_delay_mode, delay_count);

    ipmi_operation_log(req_msg, "Set power up delay to (%s, %.1f s) successfully\r\n",
        pp_delay_mode_to_string(pwr_on_delay_mode), delay_count / DOUBLE_TEN);

    return ipmi_send_simple_response(req_msg, COMP_CODE_SUCCESS);
}


gint32 pp_ipmi_cmd_get_power_off_timeout(const void *req_msg, gpointer user_data)
{
    guchar respData[5] = {0}; 
    guint8 set_selector;
    guint8 block_selector;
    guint32 pwr_off_timeout;
    const guint8 *req_src_data = NULL;

    if (req_msg == NULL) {
        return RET_ERR;
    }

    req_src_data = get_ipmi_src_data(req_msg);
    if (req_src_data == NULL) {
        debug_log(DLOG_ERROR, "get_ipmi_src_data req_src_data is NULL");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }

    set_selector = req_src_data[1];   
    block_selector = req_src_data[2]; 
    if ((set_selector != 0x00) || (block_selector != 0x00)) {
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    pwr_off_timeout = pp_get_pwr_off_timeout(CHASSIS_FRU_ID);
    
    pwr_off_timeout = pwr_off_timeout * 10;

    respData[0] = COMP_CODE_SUCCESS;
    respData[1] = PARAM_VERSION;
    respData[2] = OEM_PARAM_PWR_OFF_TIMEOUT; 
    respData[3] = LONGB0(pwr_off_timeout);   
    respData[4] = LONGB1(pwr_off_timeout);   
    // 原来此命令只支持2个字节，不能表示超过110分钟的时间,
    // 所以在事件超过110分钟是会出现通过smm查询不到的问题
    // 这需要在未来联合解决
    
    return ipmi_send_response(req_msg, 5, (guint8 *)respData);
}


gint32 pp_ipmi_cmd_get_power_on_delay(const void *req_msg, gpointer user_data)
{
    guchar respData[11] = {0}; 
    guint8 set_selector;
    guint8 block_selector;
    guint32 delay_count = 0;
    guint8 pwr_on_delay_mode = DELAY_MODE_SHORT;
    const guint8 *req_src_data = NULL;
    guint32 manu_id;

    if (req_msg == NULL) {
        return RET_ERR;
    }

    req_src_data = get_ipmi_src_data(req_msg);
    if (req_src_data == NULL) {
        debug_log(DLOG_ERROR, "%s: get_ipmi_src_data req_src_data is NULL\n", __FUNCTION__);
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }

    set_selector = req_src_data[1];
    block_selector = req_src_data[2]; 
    if ((set_selector != 0x00) || (block_selector != 0x00)) {
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    pp_get_pwr_on_delay_param(&pwr_on_delay_mode, &delay_count);

    manu_id = dal_get_manu_id();

    respData[0] = COMP_CODE_SUCCESS;
    respData[1] = PARAM_VERSION;
    respData[2] = OEM_PARAM_PWR_DELAY;  
    respData[3] = LONGB0(manu_id);      
    respData[4] = LONGB1(manu_id);      
    respData[5] = LONGB2(manu_id);      
    respData[6] = pwr_on_delay_mode;    
    respData[7] = LONGB0(delay_count);  
    respData[8] = LONGB1(delay_count);  
    respData[9] = LONGB2(delay_count);  
    respData[10] = LONGB3(delay_count); 
    
    return ipmi_send_response(req_msg, 11, (guint8 *)respData);
}

gint32 pp_set_power_level(const void *req_msg, gpointer user_data)
{
    gint32 ret_val;
    PICMG_RESP_S resp_data = { 0 };
    const guint8 *req_src_data = NULL;
    guint8 fru_id;
    guint8 level_option;
    guint8 power_level;

    if (req_msg == NULL) {
        return RET_ERR;
    }

    pp_construct_picmg_resp(&resp_data, COMP_CODE_SUCCESS);

    req_src_data = get_ipmi_src_data(req_msg);
    if (req_src_data == NULL) {
        ipmi_operation_log(req_msg, "get_ipmi_src_data req_src_data is NULL.");
        debug_log(DLOG_ERROR, "get_ipmi_src_data  req_src_data is NULL.\n");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }
    if (req_src_data[0] != PICMG_IDENTIFIER) {
        ipmi_operation_log(req_msg, PL_OP_LOG_SET_PWR_LEL_F);
        debug_log(DLOG_ERROR, "PICMG_IDENTIFIER:%d(Should:%d) invalid!\r\n", req_src_data[0], PICMG_IDENTIFIER);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    // 只有底板处理该消息，其它fru直接返回C9
    fru_id = req_src_data[1];
    if (fru_id != CHASSIS_FRU_ID) {
        ipmi_operation_log(req_msg, PL_OP_LOG_SET_PWR_LEL_F);
        debug_log(DLOG_ERROR, "fru_id:%d(Should:%d) invalid!\r\n", fru_id, CHASSIS_FRU_ID);
        return ipmi_send_simple_response(req_msg, COMP_CODE_OUTOF_RANGE);
    }

    level_option = req_src_data[3]; 
    if (level_option > POWER_LEVEL_COPY_DESIRED_LEVELS_TO_PRESENT_LEVELS) {
        ipmi_operation_log(req_msg, PL_OP_LOG_SET_PWR_LEL_F);
        debug_log(DLOG_ERROR, "level_option:%d(Max:%d) invalid!\r\n", level_option,
            POWER_LEVEL_COPY_DESIRED_LEVELS_TO_PRESENT_LEVELS);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    power_level = req_src_data[2]; 

    // 为了兼容旧SMM软件，需要区分刀片与服务器单板
    ret_val = pp_action_according_to_powerlevel(fru_id, power_level, IPMI_SET_POWER);
    if (ret_val != RET_OK) {
        ipmi_operation_log(req_msg, PL_OP_LOG_SET_PWR_LEL_F);
        debug_log(DLOG_ERROR, "pp_action_according_to_powerlevel fail(PowerLevel:%u)!\r\n", power_level);
        return ipmi_send_simple_response(req_msg, ret_val);
    }

    ipmi_operation_log_limit(DLOG_LIMIT_10MIN, req_msg, "Set power level to (RAW:%02xh-%02xh-%02xh) successfully",
        fru_id, power_level, level_option);
    
    return ipmi_send_response(req_msg, 2, (guint8 *)&resp_data);
}

LOCAL gint32 __get_resp_data_power_state(guint8 power_type, guint8 fruid, guint8 *resp_data)
{
    guchar pwr_state;
    gint32 ret;

    switch (power_type) {
        case POWER_STEADY_STATE_POWERDRAW_LEVELS:
            pwr_state = pp_get_pwr_state(fruid);
            
            resp_data[2] = (pwr_state == PAYLOAD_POWER_STATE_ON) ? PAYLOAD_POWER_STATE_ON : PAYLOAD_POWER_STATE_OFF;
            break;

        case POWER_DESIRED_STEADY_STATE_DRAW_LEVELS:
             
            resp_data[2] = 1;
            break;

        case POWER_STEADY_POWER_STATE:
            ret = dal_get_powerstate(&resp_data[2]); 
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "get power state failed, fru_id = %u, ret = %d", fruid, ret);
                return RET_ERR;
            }
            break;

        case POWER_EARLY_POWER_DRAW_LEVELS: // 暂不支持early power
        case POWER_DESIRED_EARLY_LEVELS:
        default:
            debug_log(DLOG_ERROR, "power_type:%u invalid.", power_type);
            return RET_ERR;
    }

    return RET_OK;
}


gint32 pp_get_power_level(const void *req_msg, gpointer user_data)
{
    guint32 i;
    const guint8 *req_data = NULL;
    guint8 resp_data[IPMI_DATA_FILTER_LEN] = {0};
    guint8 fruid;
    guint8 power_type;
    guint8 server_flag = 0;

    if (req_msg == NULL) {
        return RET_ERR;
    }

    req_data = get_ipmi_src_data(req_msg);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "get_ipmi_src_data req_data is NULL\n");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }
    if (req_data[0] != PICMG_IDENTIFIER) {
        debug_log(DLOG_ERROR, "PICMG_IDENTIFIER:%u(Should:%u) invalid.\r\n", req_data[0], PICMG_IDENTIFIER);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }
    fruid = req_data[1];
    if (fruid > CHASSIS_FRU_ID) {
        debug_log(DLOG_ERROR, "fruid:%u(Should <= %u) invalid.\r\n", fruid, CHASSIS_FRU_ID);
        return ipmi_send_simple_response(req_msg, COMP_CODE_OUTOF_RANGE);
    }
    
    if (hs_get_cur_state(fruid) == FRU_HS_STATE_M0) {
        
        debug_log(DLOG_ERROR, "cur_state is M0,not support get_power_level.\r\n");
        return ipmi_send_simple_response(req_msg, COMP_CODE_STATUS_INVALID);
    }
    if (pp_get_server_type(&server_flag) != RET_OK) {
        debug_log(DLOG_ERROR, "pp_get_server_type fail\r\n");
        return ipmi_send_simple_response(req_msg, COMP_CODE_CANNOT_RESPONSE);
    }
    
    power_type = req_data[2];
    if (__get_resp_data_power_state(power_type, fruid, resp_data) != RET_OK) {
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    resp_data[0] = COMP_CODE_SUCCESS; 
    resp_data[1] = PICMG_IDENTIFIER;
    resp_data[3] = g_get_power_info.delay_to_stable;  
    resp_data[4] = g_get_power_info.power_multiplier; 
    if (g_get_power_info.max_power_level > MAX_POWER_DRAW_NUM) {
        debug_log(DLOG_ERROR, "max_power_level:%u(Should <= %u) invalid.", g_get_power_info.max_power_level,
            MAX_POWER_DRAW_NUM);
        return ipmi_send_simple_response(req_msg, COMP_CODE_OUTOF_RANGE);
    }
    for (i = 0; i < g_get_power_info.max_power_level; i++) {
        resp_data[5 + i] = g_get_power_info.power_draw[i]; 
    }
    
    return ipmi_send_response(req_msg, 5 + g_get_power_info.max_power_level, resp_data);
}


gint32 ipmi_get_aclost_status(const void *req_msg, gpointer user_data)
{
    gint32 ret_val;
    const guint8 *req_src_data = NULL;
    guchar respData[6] = {0}; 
    guchar ACLost_status = 0;

    if (req_msg == NULL) {
        return RET_ERR;
    }

    req_src_data = get_ipmi_src_data(req_msg);
    if (req_src_data == NULL) {
        debug_log(DLOG_ERROR, "get_ipmi_src_data  req_src_data is NULL");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }
    
    ret_val = pp_judge_manu_id_valid_bios(req_src_data);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, DB_STR_MUNU_ID_ERR, ret_val);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    (void)payload_get_aclost_status(&ACLost_status);
    
    respData[0] = COMP_CODE_SUCCESS; 
    respData[1] = req_src_data[0];   
    respData[2] = req_src_data[1];   
    respData[3] = req_src_data[2];   
    respData[4] = ACLost_status;     
    return ipmi_send_response(req_msg, sizeof(respData), (guint8 *)respData);
}


LOCAL gchar *pp_aclost_status_to_string(guint8 status)
{
    switch (status) {
        case PAYLOAD_POWER_STATE_ON:
            return "power on";
        case PAYLOAD_POWER_STATE_OFF:
            return "power off";
        default:
            return "unknow";
    }
}


gint32 ipmi_set_aclost_status(const void *req_msg, gpointer user_data)
{
    gint32 ret_val;
    const guint8 *req_src_data = NULL;
    guint8 status;

    if (req_msg == NULL) {
        return RET_ERR;
    }

    req_src_data = get_ipmi_src_data(req_msg);
    if (req_src_data == NULL) {
        debug_log(DLOG_ERROR, "get_ipmi_src_data  req_src_data is NULL");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }

    
    ret_val = pp_judge_manu_id_valid_bios(req_src_data);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, DB_STR_MUNU_ID_ERR, ret_val);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }
    
    status = req_src_data[4];

    (void)payload_set_aclost_status(status);
    ipmi_operation_log(req_msg, "Set power state before AC lost to (%s) successfully",
        pp_aclost_status_to_string(status));

    return ipmi_send_simple_response(req_msg, COMP_CODE_SUCCESS);
}


gint32 refresh_pwrbutton_shield_state(void)
{
    gint32 ret;
    guint8 property_val;
    guint8 accessor_val;
    OBJ_HANDLE object_handle;
    GVariant *property = NULL;

    // 通过ChassisPayload对象获取关联CPLD寄存器的属性值PanelPwrButtonShield
    ret = dal_get_specific_object_byte(CLASS_PAYLOAD, PROPERTY_PAYLOAD_FRUID, CHASSIS_FRU_ID, &object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get ChassisPayload object handle failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    // 获取持久化的属性值
    ret = dfl_get_saved_value(object_handle, PROPERTY_PAYLOAD_PWR_BTN_SHIEID, &property, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get ButtonShield state failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    property_val = g_variant_get_byte(property);
    g_variant_unref(property);
    // 获取寄存器里的值
    ret = dal_get_extern_value_byte(object_handle, PROPERTY_PAYLOAD_PWR_BTN_SHIEID, &accessor_val, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get ButtonShield state failed, ret = %d", ret);
        return RET_ERR;
    }

    if (accessor_val == property_val) {
        return RET_OK;
    }
    // 不相等重新写入寄存器
    ret = dal_set_property_value_byte(object_handle, PROPERTY_PAYLOAD_PWR_BTN_SHIEID, property_val, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set PanelPwrButtonShield accessor failed, ret = %d", ret);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 set_pwrbutton_shield_state(IPMI_SET_PWRBUTTON_SHIELD req_data, guint8 *err_code)
{
#define SET_BUTTON_TYPE_PARAM_MAX 0
#define SET_CMD_PARAM_DATA_LEN    3
#define SHIELD_STATE_PARAM_MAX    1
#define PWR_BTN_SHIEID_SUPPORTED  1

    OBJ_HANDLE object_handle = 0;
    gint32 ret;
    guint8 get_state = 0;
    guint8 shield_support = 0;

    // 参数检查
    if (req_data.param.fru_id > CHASSIS_FRU_ID || req_data.param.button_type > SET_BUTTON_TYPE_PARAM_MAX ||
        req_data.param_len != SET_CMD_PARAM_DATA_LEN || req_data.param.shield_state > SHIELD_STATE_PARAM_MAX) {
        debug_log(DLOG_ERROR, "Parameter is out of range.");
        *err_code = COMP_CODE_OUTOF_RANGE;
        return RET_ERR;
    }

    // 通过ChassisPayload对象获取关联CPLD寄存器的属性值PanelPwrButtonShield
    ret = dal_get_specific_object_byte(CLASS_PAYLOAD, PROPERTY_PAYLOAD_FRUID, CHASSIS_FRU_ID, &object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get ChassisPayload object handle failed, ret = %d", ret);
        *err_code = COMP_CODE_UNKNOWN;
        return ret;
    }

    // 依据标志位判断是否支持屏蔽，不支持返回0xd5
    ret = dal_get_property_value_byte(object_handle, PROPERTY_PAYLOAD_PWR_BTN_SHIEID_SUPPORT, &shield_support);
    if (ret != RET_OK || shield_support != PWR_BTN_SHIEID_SUPPORTED) {
        debug_log(DLOG_ERROR, "%s: ButtonShield is not supported, ret(%d), %s:PwrButtonShieldSupport(%d)", __FUNCTION__,
            ret, dfl_get_object_name(object_handle), shield_support);
        *err_code = COMP_CODE_STATUS_INVALID;
        return RET_ERR;
    }

    // 获取CPLD寄存器当前值，无法获取则认为不支持此功能
    ret = dal_get_extern_value_byte(object_handle, PROPERTY_PAYLOAD_PWR_BTN_SHIEID, &get_state, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get ButtonShield state failed, ret = %d", ret);
        *err_code = COMP_CODE_STATUS_INVALID;
        return ret;
    }

    // 短按 对应CPLD寄存器bit0，0:不屏蔽 1:屏蔽
    debug_log(DLOG_DEBUG, "%s:set_state is 0x%x", __FUNCTION__, req_data.param.shield_state);

    // 将值更新到属性并写寄存器
    ret = dal_set_property_value_byte(object_handle, PROPERTY_PAYLOAD_PWR_BTN_SHIEID, req_data.param.shield_state,
        DF_SAVE | DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set PanelPwrButtonShield failed, ret = %d", ret);
        *err_code = COMP_CODE_UNKNOWN;
    }

    return ret;
}

gint32 ipmi_cmd_set_pwrbutton_shield_state(const void *req_msg, gpointer user_data)
{
    guint8 resp_data[CMD_MAX_LEN] = {0};
    gint32 ret;
    guint32 manufactureid = OEM_MANUFACTURE_ID;
    guint8 index_id = 0;
    guint8 err_code = 0;
    const guint8 *ret_value = NULL;
    IPMI_SET_PWRBUTTON_SHIELD req_data = { 0 };

    // 入参检查
    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, "req_msg is null");
        err_code = COMP_CODE_INVALID_FIELD;
        goto ERROR_EXIT;
    }
    ret_value = get_ipmi_src_data(req_msg);
    if (ret_value == NULL) {
        debug_log(DLOG_ERROR, "%s:get_ipmi_src_data is null", __FUNCTION__);
        err_code = COMP_CODE_INVALID_FIELD;
        goto ERROR_EXIT;
    }
    ret = memcpy_s(&req_data, sizeof(IPMI_SET_PWRBUTTON_SHIELD), ret_value, sizeof(IPMI_SET_PWRBUTTON_SHIELD));
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "memcpy_s is failed");
        err_code = COMP_CODE_INVALID_FIELD;
        goto ERROR_EXIT;
    }
    // 厂商判断
    if (dal_check_ipmi_req_huawei_iana(req_data.manu_id, MANUFAC_ID_LEN) == FALSE) {
        err_code = COMP_CODE_INVALID_FIELD;
        goto ERROR_EXIT;
    }

    if (set_pwrbutton_shield_state(req_data, &err_code) != RET_OK) {
        goto ERROR_EXIT;
    }

    resp_data[index_id++] = COMP_CODE_SUCCESS;
    resp_data[index_id++] = LONGB0(manufactureid);
    resp_data[index_id++] = LONGB1(manufactureid);
    resp_data[index_id++] = LONGB2(manufactureid);

    ret = ipmi_send_response(req_msg, index_id, resp_data);
    ipmi_operation_log(req_msg, "Successfully %s the function of shielding the short-press power button",
        (req_data.param.shield_state == ENABLE) ? "Enable" : "Disable");
    return ret;

ERROR_EXIT:

    ipmi_operation_log(req_msg, "Failed to %s the function of shielding the short-press power button",
        (req_data.param.shield_state == ENABLE) ? "Enable" : "Disable");
    return ipmi_send_simple_response(req_msg, err_code);
}

LOCAL gint32 get_pwrbutton_shield_state(guint8 *get_state, guint8 *com_code)
{
    OBJ_HANDLE object_handle = 0;
    gint32 ret;
    guint8 shield_support = 0;

    // 通过ChassisPayload对象获取关联CPLD寄存器的属性值PanelPwrButtonShield
    ret = dal_get_specific_object_byte(CLASS_PAYLOAD, PROPERTY_PAYLOAD_FRUID, CHASSIS_FRU_ID, &object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get Payload object handle failed, ret = %d", ret);
        *com_code = COMP_CODE_UNKNOWN;
        return ret;
    }
    // 依据标志位判断是否支持屏蔽，不支持返回0xd5
    ret = dal_get_property_value_byte(object_handle, PROPERTY_PAYLOAD_PWR_BTN_SHIEID_SUPPORT, &shield_support);
    if (ret != RET_OK || shield_support != PWR_BTN_SHIEID_SUPPORTED) {
        debug_log(DLOG_ERROR, "%s: ButtonShield is not supported, ret(%d), %s:PwrButtonShieldSupport(%d)", __FUNCTION__,
            ret, dfl_get_object_name(object_handle), shield_support);
        *com_code = COMP_CODE_STATUS_INVALID;
        return RET_ERR;
    }

    // 获取CPLD寄存器当前值，获取不到则认为不支持此功能
    ret = dal_get_extern_value_byte(object_handle, PROPERTY_PAYLOAD_PWR_BTN_SHIEID, get_state, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get ButtonShield state failed, ret = %d", ret);
        *com_code = COMP_CODE_STATUS_INVALID;
        return ret;
    }

    return RET_OK;
}

gint32 ipmi_cmd_get_pwrbutton_shield_state(const void *req_msg, gpointer user_data)
{
    IPMI_GET_PWRBUTTON_SHIELD req_data = { 0 };
    guint8 resp_data[CMD_MAX_LEN] = {0};
    gint32 ret;
    guint32 manufactureid = OEM_MANUFACTURE_ID;
    guint8 index_id = 0;
    guint8 get_state = 0;
    guint8 com_code = 0;
    const guint8 *ret_value = NULL;
#define GET_BUTTON_TYPE_PARAM_MAX 0
#define GET_CMD_PARAM_DATA_LEN 2

    // 入参检查
    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, "req_msg is failed");
        com_code = COMP_CODE_INVALID_FIELD;
        goto ERROR_EXIT;
    }
    ret_value = get_ipmi_src_data(req_msg);
    if (ret_value == NULL) {
        debug_log(DLOG_ERROR, "%s:get_ipmi_src_data is null", __FUNCTION__);
        com_code = COMP_CODE_INVALID_FIELD;
        goto ERROR_EXIT;
    }
    ret = memcpy_s(&req_data, sizeof(IPMI_GET_PWRBUTTON_SHIELD), ret_value, sizeof(IPMI_GET_PWRBUTTON_SHIELD));
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "memcpy_s is null");
        com_code = COMP_CODE_INVALID_FIELD;
        goto ERROR_EXIT;
    }
    // 厂商判断
    if (dal_check_ipmi_req_huawei_iana(req_data.manu_id, MANUFAC_ID_LEN) == FALSE) {
        com_code = COMP_CODE_INVALID_FIELD;
        goto ERROR_EXIT;
    }
    // 参数检查
    if (req_data.param.fru_id > CHASSIS_FRU_ID || req_data.param.button_type > GET_BUTTON_TYPE_PARAM_MAX ||
        req_data.param_len != GET_CMD_PARAM_DATA_LEN) {
        debug_log(DLOG_ERROR, "Parameter is out of range.");
        com_code = COMP_CODE_OUTOF_RANGE;
        goto ERROR_EXIT;
    }

    if (get_pwrbutton_shield_state(&get_state, &com_code) != RET_OK) {
        goto ERROR_EXIT;
    }

    resp_data[index_id++] = COMP_CODE_SUCCESS;
    resp_data[index_id++] = LONGB0(manufactureid);
    resp_data[index_id++] = LONGB1(manufactureid);
    resp_data[index_id++] = LONGB2(manufactureid);

    // 短按 对应CPLD寄存器bit0，0:不屏蔽 1:屏蔽
    resp_data[index_id++] = get_state;

    return ipmi_send_response(req_msg, index_id, resp_data);

ERROR_EXIT:
    return ipmi_send_simple_response(req_msg, com_code);
}


gint32 pp_ipmi_cmd_set_watch_dog_status(const void *req_msg, gpointer user_data)
{
    const guint8 *req_src_data = get_ipmi_src_data(req_msg);
    if (req_src_data == NULL) {
        debug_log(DLOG_ERROR, "req_src_data is NULL");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }

    
    gint32 ret = pp_judge_manu_id_valid_bios(req_src_data);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, DB_STR_MUNU_ID_ERR, ret);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }
    
    
    guint8 enable_type = DISABLE;
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_X86, &enable_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Invalid boards ret = %d", ret);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }
    if (enable_type != ENABLE) {
        debug_log(DLOG_ERROR, "X86 state error. state = %d", enable_type);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    OBJ_HANDLE obj_handle = 0;
    ret = dal_get_object_handle_nth(CLASS_NAME_STOR_PAYLOAD, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get obj [class:%s] handle failed! ret = %d", CLASS_NAME_STOR_PAYLOAD, ret);
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }

    guint8 dog_state = req_src_data[6]; 
    if (dog_state != 0 && dog_state != 1) {
        debug_log(DLOG_ERROR, "Invalid Argument %d!", dog_state);
        return ipmi_send_simple_response(req_msg, COMP_CODE_DATA_NOT_AVAILABLE);
    }

    ret = dal_set_property_value_byte(obj_handle, PROPERTY_WATCH_DOG_STATUS, dog_state, DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set watch dog status failed. ret = %d", ret);
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }
    ipmi_operation_log(req_msg, "%s the watch dog successfully.", dog_state ? "Open" : "Close");
    
    guint8 resp_data[4] = {0x00, 0xdb, 0x07, 0x00};
    return ipmi_send_response(req_msg, sizeof(resp_data), resp_data);
}


gint32 ipmi_set_board_mode(gconstpointer msg_data, gpointer user_data)
{
    const guint8 *rev_data = NULL;
    guint8 board_test_mode;
    OBJ_HANDLE obj_handle;
    gint32 ret;

    
    if (msg_data == NULL) {
        debug_log(DLOG_ERROR, "Input parameter is null.");
        return RET_ERR;
    }

    rev_data = get_ipmi_src_data(msg_data);
    if (rev_data == NULL) {
        debug_log(DLOG_ERROR, "Get ipmi src data failed!");
        ipmi_operation_log(msg_data, "Set mainboard dft mode fail.");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }
    
    board_test_mode = rev_data[5]; 

    ret = dfl_get_object_handle(OBJECT_STORPAYLOAD, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get OBJECT_STORPAYLOAD object fail!(ret=%d).", ret);
        ipmi_operation_log(msg_data, "Set mainboard to %s mode fail.", (board_test_mode == 0) ? "normal" : "dft");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    ret = dal_set_property_value_byte(obj_handle, PROPERTY_BOARD_EQUIP_MODE, board_test_mode, DF_HW);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Set mainboard to %s mode fail.", (board_test_mode == 0) ? "normal" : "dft");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    ipmi_operation_log(msg_data, "Set mainboard to %s mode success.", (board_test_mode == 0) ? "normal" : "dft");
    return ipmi_send_simple_response(msg_data, COMP_CODE_SUCCESS);
}