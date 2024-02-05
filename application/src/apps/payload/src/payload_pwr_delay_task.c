
#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <stdbool.h>
#include "pme/common/mscm_vos.h"
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "payload_hop.h"
#include "payload_ipmi.h"
#include "payload_hs.h"
#include "payload_com.h"
#include "payload_pwr.h"
#include "payload_storage.h"
#include "wdt2.h"

LOCAL guint8 g_pwr_restore_done_flag = FALSE;


LOCAL guchar pp_is_vrd_upgrade_success(void)
{
    gint32 ret_val;
    OBJ_HANDLE vrd_handle = 0;
    guint8 vrd_validate_flag = 0;

    ret_val = dal_get_object_handle_nth(CLASS_VRD_UPGRADE_MGNT, 0, &vrd_handle);
    if (ret_val == RET_OK) {
        (void)dal_get_property_value_byte(vrd_handle, PROPERTY_VRD_VALIDATE_FLAG, &vrd_validate_flag);
        if (VALIDATE_VRD_SUCCESS == vrd_validate_flag) {
            return TRUE;
        }
    }
    return FALSE;
}


LOCAL void clear_vrd_validate_action(void)
{
    gint32 ret_val;
    OBJ_HANDLE vrd_handle = 0;
    ret_val = dal_get_object_handle_nth(CLASS_VRD_UPGRADE_MGNT, 0, &vrd_handle);
    if (ret_val == RET_OK) {
        
        if (TRUE == dfl_is_property_value_saved(vrd_handle, PROPERTY_VRD_ACTION_AFTER_VALIDATE, DF_SAVE)) {
            pp_set_vrd_validate_method(METHOD_VRD_SET_VALIDATE_ACTION, DO_DEFAULT_AFTER_VALIDATE_VRD);
            debug_log(DLOG_ERROR, "Set action after validate vrd(default).");
        }
    }
    return;
}


LOCAL gint32 pp_set_activate_upgrade_bios_flag(guint8 bios_flag)
{
    gint32 ret;
    OBJ_HANDLE pfn_handle = 0;
    GSList *input_list = NULL;
    OBJ_HANDLE payload_obj = 0;
    guint8 power_state;

    (void)dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &pfn_handle);
    input_list = g_slist_append(input_list, g_variant_new_byte(bios_flag));
    ret = dfl_call_class_method(pfn_handle, METHOD_PFN_SET_ACTIVATE_BIOS_UPGRADE_FLAG, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "Failed to call dfl_call_class_method.\r\n");
        return RET_ERR;
    }

    
    (void)dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &payload_obj);
    (void)dal_set_property_value_byte(payload_obj, PROPERTY_PAYLOAD_CHASSPWR_STATE, 1, DF_NONE);
    (void)dal_set_property_value_byte(payload_obj, PROPERTY_PAYLOAD_CHASSPWR_STATE, 0, DF_SAVE);

    power_state = pp_get_pwr_state(CHASSIS_FRU_ID);
    debug_log(DLOG_ERROR, "cuttent powerstate is %s.", (power_state == PAYLOAD_POWER_STATE_OFF) ? "off" : "on");
    
    if (power_state == PAYLOAD_POWER_STATE_OFF) {
        (void)dal_set_property_value_byte(payload_obj, PROPERTY_PAYLOAD_CHASSPWR_STATE, FALSE, DF_SAVE);
    } else {
        (void)dal_set_property_value_byte(payload_obj, PROPERTY_PAYLOAD_CHASSPWR_STATE, TRUE, DF_SAVE);
    }

    debug_log(DLOG_DEBUG, "activate upgrade bios successfully.");
    return RET_OK;
}


LOCAL gint32 alwayson_activate_upgrade_bios(void)
{
    gint32 ret;
    P_PAYLOAD_S p_payload = NULL;

    ret = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return RET_ERR;
    }
    
    if ((p_payload->m_pwr_restore_policy == PAYLOAD_ALWAYS_POWER_UP) ||
        (PAYLOAD_ALWAYS_POWER_UP_RO == p_payload->m_pwr_restore_policy) ||
        (p_payload->m_pwr_restore_policy == PAYLOAD_POWER_RESTORED &&
        p_payload->m_pwr_state_before_ac_lost == PAYLOAD_POWER_STATE_ON)) {
        
        if ((TRUE == vos_get_file_accessible(CACHE_BIOS_HPM_FILE)) ||
            (TRUE == vos_get_file_accessible(CACHE_BIOS_COMP_FILE)) || (RET_OK == pp_get_remote_upgrade_bios_state())) {
            
            OBJ_HANDLE pfn_handle = 0;
            guint8 power_policies = 0;
            (void)dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &pfn_handle);
            (void)dal_get_property_value_byte(pfn_handle, PFN_ACTIVATE_UPGRADE_BIOS_FLAG, &power_policies);
            power_policies = power_policies == POWER_ON_AFTER_UP_BIOS ? POWER_ON_AFTER_UP_BIOS :
                PWR_REST_POLICY_AFTER_UP_BIOS;
            debug_log(DLOG_ERROR, "The BIOS cache package exists, excuting power policy is = %u.", power_policies);
            return pp_set_activate_upgrade_bios_flag(power_policies);
        } else {
            return RET_ERR;
        }
    } else {
        return RET_ERR;
    }
}


LOCAL guchar pp_is_need_do_vrd_action(void)
{
    gint32 ret_val;
    OBJ_HANDLE vrd_handle = 0;
    guint8 vrd_action = 0;

    ret_val = dal_get_object_handle_nth(CLASS_VRD_UPGRADE_MGNT, 0, &vrd_handle);
    if (ret_val == RET_OK) {
        (void)dal_get_property_value_byte(vrd_handle, PROPERTY_VRD_ACTION_AFTER_VALIDATE, &vrd_action);
        if (DO_POWER_ON_AFTER_VALIDATE_VRD == vrd_action) {
            return TRUE;
        }
    }
    return FALSE;
}

LOCAL void clear_action_after_upgrade(OBJ_HANDLE pfn_handle)
{
    gint32 ret;
    GSList *input_list = NULL;
    
    if (dfl_is_property_value_saved(pfn_handle, PFN_ACTION_AFTER_UPGRADE, DF_SAVE) == TRUE) {
        input_list = g_slist_append(input_list, g_variant_new_byte(PWR_REST_AFTER_AC));
        ret = dfl_call_class_method(pfn_handle, METHOD_PFN_SET_ACTION_AFTER_UPGRADE, NULL, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s Failed to call METHOD_PFN_SET_ACTION_AFTER_UPGRADE,ret(%d).", __FUNCTION__, ret);
        }
    }
}

LOCAL gboolean check_action_after_upgrade(void)
{
    gint32 ret;
    OBJ_HANDLE pfn_handle = 0;
    guint8 activate_after_upgrade = PWR_REST_AFTER_AC;

    ret = dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &pfn_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get pfn class name failed,ret (%d)", __FUNCTION__, ret);
        return FALSE;
    }
    (void)dal_get_property_value_byte(pfn_handle, PFN_ACTION_AFTER_UPGRADE, &activate_after_upgrade);
    if (activate_after_upgrade == POWER_OFF_AFTER_AC) {
        debug_log(DLOG_ERROR, "%s : Do special activation after upgrade [power off]", __FUNCTION__);
        clear_action_after_upgrade(pfn_handle);
        return TRUE;
    }
    if (activate_after_upgrade == POWER_ON_AFTER_AC) {
        debug_log(DLOG_ERROR, "%s : Do special activation after upgrade[power on]", __FUNCTION__);
        ret = pp_method_pwr_on(pfn_handle, NULL, NULL, NULL);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s :  Do special activation after upgrade[power on] error(%d)", __FUNCTION__, ret);
        }
        clear_action_after_upgrade(pfn_handle);
        return TRUE;
    }
    return FALSE;
}

LOCAL void pp_do_poweron_vrd_action(void)
{
    gint32 ret;
    OBJ_HANDLE pfn_handle = 0;

    (void)dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &pfn_handle);
    
    
    ret = pp_method_pwr_on(pfn_handle, NULL, NULL, NULL);
    debug_log(DLOG_ERROR, "Do VRD validated action[power on], Set action after validate vrd(default).");

    pp_set_vrd_validate_method(METHOD_VRD_SET_VALIDATE_ACTION, DO_DEFAULT_AFTER_VALIDATE_VRD);
    pp_set_vrd_validate_method(METHOD_VRD_SET_VALIDATE_FLAG, VALIDATE_VRD_NONE_SUCCESS);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "pp_do_poweron_vrd_action failed, ret = %d.", ret);
    }
    return;
}


LOCAL void clear_vrd_validate_flag(void)
{
    gint32 ret_val;
    OBJ_HANDLE vrd_handle = 0;
    ret_val = dal_get_object_handle_nth(CLASS_VRD_UPGRADE_MGNT, 0, &vrd_handle);
    if (ret_val == RET_OK) {
        
        if (TRUE == dfl_is_property_value_saved(vrd_handle, PROPERTY_VRD_VALIDATE_FLAG, DF_SAVE)) {
            pp_set_vrd_validate_method(METHOD_VRD_SET_VALIDATE_FLAG, VALIDATE_VRD_NONE_SUCCESS);
        }
    }

    return;
}


LOCAL guchar pp_is_need_do_insert_action(void)
{
    gint32 ret;
    guint8 value = 0;

    ret = get_storpayload_property_byte(PROPERTY_ACTION_AFTER_INSERT, &value);
    if ((ret == RET_OK) && (value == TRUE)) {
        return TRUE;
    }
    return FALSE;
}


LOCAL void pp_clear_action_after_insert(void)
{
    guint8 value = FALSE;

    (void)set_storpayload_property_byte(PROPERTY_ACTION_AFTER_INSERT, &value, DF_SAVE_TEMPORARY);
    return;
}


LOCAL gboolean check_upgrade_state_before_policy(void)
{
    
    if (alwayson_activate_upgrade_bios() == RET_OK) {
        debug_log(DLOG_ERROR, "Activate upgrade BIOS, delay to excute power restore policy.");
        return RET_ERR;
    } else if (pp_is_vrd_upgrade_success() == TRUE) {
        maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Upgrade VRD chip and activate successfully");
        
        clear_vrd_validate_flag();
        if (pp_is_need_do_vrd_action() == TRUE) {
            
            pp_do_poweron_vrd_action();
            return RET_ERR;
        } else {
            
            return RET_OK;
        }
    }
    // 升级后触发 AC 上电 策略 检查
    if (check_action_after_upgrade()) {
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 smm_pwr_restore_by_soft_reset(void)
{
    guint8 software_type = 0;
    guint8 encl_work_state;
    gint32 ret_val;

    (void)dal_get_software_type(&software_type);
    
    if (software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        
        ret_val = get_storpayload_property_byte(PROPERTY_ENCL_WORK_STATE, &encl_work_state);
        if ((ret_val == RET_OK) && (encl_work_state == FALSE)) {
            return FALSE;
        }
    }
    return TRUE;
}


LOCAL gboolean pp_is_need_do_pwr_restore(void)
{
    
    if (pp_is_need_do_insert_action() == TRUE) {
        pp_clear_action_after_insert();
        debug_log(DLOG_ERROR, "Board is inserted into enclosure, do power restore policy.");
        return TRUE;
    }
    
    if (check_first_poweron_cond() == RET_OK) {
        debug_log(DLOG_ERROR, "Reach first power on condition, pangea product power on system.");
        return TRUE;
    }
    return FALSE;
}


void pp_do_pwr_restore(guint8 ser_type)
{
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE payload_obj = 0;
    guint32 exceptions;

    
    if (hop_get_imana_reset_type() == SOFT_RESET) {
        
        
        if (FALSE == pp_is_vrd_upgrade_success()) {
            debug_log(DLOG_ERROR, "Clear vrd validate action.");
            clear_vrd_validate_action();
        }
        
        gint32 ret_val = dfl_get_object_handle(OBJECT_PFN, &obj_handle);
        if (ret_val != DFL_OK) {
            
            debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
            return;
        }

        if (smm_pwr_restore_by_soft_reset() == FALSE) {
            debug_log(DLOG_ERROR, "Smm no need power restore policy.");
            return;
        }

        ret_val = dfl_call_class_method(obj_handle, METHOD_PFN_GET_CPLD_VALID, NULL, NULL, &output_list);
        debug_log(DLOG_ERROR, "bmc reset finish, clear cpld valid flag! ret = %d", ret_val);
        if (ret_val != DFL_OK) {
            return;
        }
        
        guint8 reboot_src = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

        if (reboot_src == FALSE && pp_is_need_do_pwr_restore() == FALSE) {
            debug_log(DLOG_ERROR, "soft reset and cpld not upgrade don't need power restore policy.\r\n");
            return;
        }
        debug_log(DLOG_ERROR, "soft reset and cpld upgrade need power restore policy.\r\n");
    }

    if (check_upgrade_state_before_policy() != RET_OK) {
        debug_log(DLOG_ERROR, "upgrading, don't execute power restore policy.");
        return;
    }

    (void)dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &payload_obj);
    (void)dal_get_property_value_uint32(payload_obj, PROPTERY_POWER_ON_STRATEGY_EXCEPTIONS, &exceptions);
    if (GET_BIT_GUINT32_VALUE(exceptions, POWER_ON_STRATEGY_INDEX)) {
        (void)dal_set_property_value_uint32(payload_obj, PROPTERY_POWER_ON_STRATEGY_EXCEPTIONS, 0, DF_SAVE);
        return;
    }
    pp_do_pwr_restore_process();
}


void pp_task_power(void *param)
{
    guint8 ser_type = 0;
    gint32 ret;
    guint8 upgrade_flag = 0;
    OBJ_HANDLE vrd_handle = 0;

    (void)prctl(PR_SET_NAME, (gulong)"PpTaskPower");
    // 等待VRD升级完成再进行上电
    while (1) {
        // 如果OS已经上电，则直接退出。
        if (HARD_POWER_ON == hop_get_pwr_signal(CHASSIS_FRU_ID)) {
            
            break;
        }

        ret = dal_get_object_handle_nth(CLASS_VRD_UPGRADE_MGNT, 0, &vrd_handle);
        
        if (ret != RET_OK) {
            break;
        }

        
        (void)dal_get_property_value_byte(vrd_handle, PROPERTY_VRD_UPGRADE_FLAG, &upgrade_flag);
        
        if (upgrade_flag == VRD_UPGRADTED) {
            break;
        }
        
        (void)vos_task_delay(5 * 1000);
    }

    (void)pp_get_server_type(&ser_type);
    pp_do_pwr_restore(ser_type);
    g_pwr_restore_done_flag = TRUE;

    
    if (ser_type == SERVER_TYPE_CHASSIS) {
        (void)hop_synch_pwr_button_lock();
    }
}


gint32 pwr_restore_action_done_method(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    if (output_list == NULL) {
        debug_log(DLOG_ERROR, "[%s]: output_list is null\n", __FUNCTION__);
        return RET_ERR;
    }
    *output_list = g_slist_append(*output_list, g_variant_new_byte(g_pwr_restore_done_flag));
    return RET_OK;
}
