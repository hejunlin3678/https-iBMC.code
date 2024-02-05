
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


void pp_cycle_init(void)
{
#define TASK_CREATE_RETRY_CNT 3
    P_PAYLOAD_S p_payload = NULL;
    gint32 result = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:get fru%d data by alias failed!", __FUNCTION__, CHASSIS_FRU_ID);
        return;
    }

    for (gint32 i = 0; i < TASK_CREATE_RETRY_CNT; i++) {
        result = vos_task_create(&p_payload->m_power_cycle_task_id, "pt_PwrC", (TASK_ENTRY)pp_cycle_task, 0,
            DEFAULT_PRIORITY);
        if (result != RET_OK) {
            debug_log(DLOG_ERROR, "%s:pp_cycle_task create failed,result is %d, cnt is %d.", __FUNCTION__, result, i);
            vos_task_delay(DELAY_ONE_SECOND);
        } else {
            break;
        }
    }
}


LOCAL void pp_do_power_on_ctrl(guchar fru_device_id)
{
    pp_set_pwr_off_lock(fru_device_id, FALSE);
    pp_send_evt(fru_device_id, PAYLOAD_CTRL_POWERON, PAYLOAD_POWER_STATE_UNSPECIFIED);
}

LOCAL void pp_do_power_off_ctrl(guchar fru_device_id)
{
    debug_log(DLOG_DEBUG, "pp_do_power_off_ctrl fru %u.\r\n", fru_device_id);
    pp_send_evt(fru_device_id, PAYLOAD_CTRL_POWEROFF, PAYLOAD_POWER_STATE_UNSPECIFIED);
}


LOCAL guchar pp_do_power_cycle_ctrl(guchar fru_device_id)
{
    FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();
    // 只有上电状态下才能启动电源循环操作
    if (fru_payload->m_pwr_ctrl_cb[fru_device_id].m_pwr_state == PAYLOAD_POWER_STATE_ON) {
        // 如果已经在执行电源循环操作的过程中，则不能再次发起同样的操作
        if (fru_payload->m_pwr_cycle_cb[fru_device_id].cycle_in_progress == FALSE) {
            fru_payload->m_pwr_cycle_cb[fru_device_id].interval = pp_get_power_cycle_interval();
            fru_payload->m_pwr_cycle_cb[fru_device_id].cycle_in_progress = TRUE;

            // 状态发生了改变，需要保存起来
            per_save((guint8 *)fru_payload->m_pwr_cycle_cb);
        } else {
            debug_log(DLOG_ERROR, "%s: power cycle already in progress!", __FUNCTION__);
            return COMP_CODE_BUSY;
        }
    } else {
        debug_log(DLOG_ERROR, "%s: can't do power cycle in power off state!", __FUNCTION__);
        return COMP_CODE_STATUS_INVALID;
    }

    return COMP_CODE_SUCCESS;
}


LOCAL guchar pp_do_force_off_ctrl(guchar fru_device_id)
{
    if (FALSE == hop_force_off(fru_device_id)) {
        return PAYLOAD_FAIL;
    }

    return PAYLOAD_SUCCESS;
}


guchar pp_fru_pwr_ctrl(guchar fru_device_id, guchar ctrl)
{
    guchar comp_code = COMP_CODE_SUCCESS;
    guint8 upgrade_flag = 1;
    OBJ_HANDLE vrd_handle = 0;
    gint32 ret;

    if ((fru_device_id >= hs_get_fru_num()) || (FALSE == hs_is_fru_managed(fru_device_id))) {
        debug_log(DLOG_ERROR, "pp_fru_pwr_ctrl fru:%u fail!\r\n", fru_device_id);
        return COMP_CODE_OUTOF_RANGE;
    }
    if (ctrl == POWER_ON) {
        ret = dal_get_object_handle_nth(CLASS_VRD_UPGRADE_MGNT, 0, &vrd_handle);
        
        if (ret == RET_OK) {
            
            if (hop_get_pwr_signal(CHASSIS_FRU_ID) == HARD_POWER_ON) {
                debug_log(DLOG_DEBUG, "%s: It's already in power on state.", __FUNCTION__);
                
                return comp_code;
            }
            
            (void)dal_get_property_value_byte(vrd_handle, PROPERTY_VRD_UPGRADE_FLAG, &upgrade_flag);
            
            if (upgrade_flag == VRD_UPGRADING) {
                debug_log(DLOG_ERROR, "VRD chip upgrade prevents power on.");
                return COMP_CODE_BUSY;
            }
        }
        
        pp_do_power_on_ctrl(fru_device_id);
    } else if (ctrl == POWER_OFF) {
        
        pwr_on_task_command(DELETE_TASK);
        pp_do_power_off_ctrl(fru_device_id);
    } else if (ctrl == POWER_CYCLE) {
        
        pwr_on_task_command(DELETE_TASK);
        comp_code = pp_do_power_cycle_ctrl(fru_device_id);
    } else if (ctrl == POWER_OFF_FORCE) {
        
        pwr_on_task_command(DELETE_TASK);
        if (PAYLOAD_FAIL == pp_do_force_off_ctrl(fru_device_id)) {
            comp_code = COMP_CODE_UNKNOWN;
        }
    } else {
        comp_code = COMP_CODE_INVALID_FIELD;
    }

    return comp_code;
}


guchar pp_get_pwr_state(guchar fru_device_id)
{
    FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();
    if (fru_payload->m_pwr_ctrl_cb[fru_device_id].m_ctrl_state != PAYLOAD_CTRL_STATE_NOACTION) {
        return PAYLOAD_POWER_STATE_IN_CTRL;
    }
    
    return fru_payload->m_pwr_ctrl_cb[fru_device_id].m_pwr_state;
}


void pp_set_restart_cause(void)
{
    P_PAYLOAD_S p_payload = NULL;
    gint32 ret;

    ret = dfl_get_data_by_alias(CLASS_PAYLOAD, CHASSIS_FRU_ID, (gpointer *)&p_payload);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d data by alias failed!\r\n", CHASSIS_FRU_ID);
        return;
    }

    p_payload->m_restart_cause.restart_cause = p_payload->m_restart_cause.restart_action;
    p_payload->m_restart_cause.restart_chan = p_payload->m_restart_cause.restart_action_chan;
    per_save((guint8 *)&p_payload->m_restart_cause);
}

LOCAL gboolean get_cpld_validing_state(void)
{
    guint8 valid_flag = 0;
    OBJ_HANDLE pfn_handle = 0;
    gint32 ret_val;

    ret_val = dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &pfn_handle);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, "Failed to get %s class obj. ret=%d", PFN_CLASS_NAME, ret_val);
        return FALSE;
    }

    ret_val = dal_get_property_value_byte(pfn_handle, PROTERY_UP_VALID_CPLD, &valid_flag);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "get object property failed! ret=%d", ret_val);
        return FALSE;
    }

    return valid_flag == 0 ? FALSE : TRUE;
}


LOCAL void pp_execute_pwr_cycle_on(guchar fru_device_id)
{
    guint32 power_on_timeout;
    gint32 ret;
    OBJ_HANDLE vrd_handle = 0;
    guint8 upgrade_flag = 0;

    // 如果单板已经处于下电状态
    if (PAYLOAD_POWER_STATE_OFF == pp_get_pwr_state(fru_device_id)) {
        if (get_cpld_validing_state() == TRUE) {
            // cpld生效中 bmc即将复位 不应该上电
            debug_log(DLOG_ERROR, "cpld valid, bmc will reset soon, don't need do poweron.");
        } else if (hs_is_auto_management() == TRUE) {
            // 上下电资管理产品，即不受控上电产品，直接上电
            ret = pp_fru_pwr_ctrl(fru_device_id, POWER_ON);
            if (ret == COMP_CODE_BUSY) {
                
                ret = dal_get_object_handle_nth(CLASS_VRD_UPGRADE_MGNT, 0, &vrd_handle);
                if (ret == RET_OK) {
                    while (upgrade_flag != VRD_UPGRADTED) {
                        (void)dal_get_property_value_byte(vrd_handle, PROPERTY_VRD_UPGRADE_FLAG, &upgrade_flag);
                        vos_task_delay(2 * 1000); 
                    }
                    (void)pp_fru_pwr_ctrl(fru_device_id, POWER_ON);
                }
            }
        } else {
            // 受控上电的产品，需要通过请求激活来上电
            (void)hse_fru_activate_policy(fru_device_id, 1, 0);
        }
        
        power_on_timeout = LOOKUP_POWER_ON_RETRY_NUM * 3UL;
        while (power_on_timeout != 0) {
            
            if (HARD_POWER_ON == hop_get_pwr_signal(fru_device_id)) {
                pp_set_restart_cause();
                // 成功上电
                break;
            }
            if (FALSE == pp_is_cycle_in_progress(fru_device_id)) {
                debug_log(DLOG_ERROR, "already cancle power cycle from looking up power on status!\r\n");
                break;
            }
            vos_task_delay(1000); 
            power_on_timeout--;
        }
        // 超时还未成功上电，记录日志
        if (power_on_timeout == 0) {
            g_printf("power on timeout from power cycle!\r\n");
            debug_log(DLOG_ERROR, "power on timeout!\r\n");
        }
    } else {
        debug_log(DLOG_ERROR, "already power on from power cycle!\r\n");
    }
    return;
}


LOCAL void pp_execute_waite_pwr_off(guint8 fru_device_id)
{
#define TRIPLE_TIME 3
#define TAISHAN_TIME 150
    guint32 power_off_timeout;
    guint8 is_arm = 0;

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &is_arm);
    if (is_arm) {
        power_off_timeout = TAISHAN_TIME * POWER_OFF_DEFAULT_TIMEOUT;
    } else {
        // 强制下电正常情况下8s能够执行完成,此处做了预留3倍的超时时间,预防强制下电失败后while循环卡死
        power_off_timeout = TRIPLE_TIME * POWER_OFF_DEFAULT_TIMEOUT;
    }

    if (dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
        
        power_off_timeout = 900; 
    }

    while (power_off_timeout) {
        // 循环检测系统电源信号，等待系统下电
        if (HARD_POWER_OFF == hop_get_pwr_signal(fru_device_id)) {
            // 如果系统顺利下电，下电过程结束
            break;
        }

        if (FALSE == pp_is_cycle_in_progress(fru_device_id)) {
            debug_log(DLOG_ERROR, "already cancle power cycle from looking up power off status!");
            break;
        }
        vos_task_delay(1000); 

        power_off_timeout--;
    }
    
    if (power_off_timeout == 0) {
        g_printf("power off timeout from power cycle!");
        debug_log(DLOG_ERROR, "power off timeout!");
    }
}


LOCAL void pp_execute_pwr_cycle_off(guchar fru_device_id, guint32 *pwr_off_interval)
{
    gint32 ret;
    gulong tick_cnt_pwr_btn = 0;
    guint32 count = 0;
    gulong tick_cnt_pwr_off;
    guint8 condition = 0;
    guint32 prod_ver = 0;

    if (pwr_off_interval == NULL) {
        debug_log(DLOG_ERROR, "%s : Input parameter error", __FUNCTION__);
        return;
    }
    *pwr_off_interval = 0;

    // 单板处于上电状态
    if (PAYLOAD_POWER_STATE_ON == pp_get_pwr_state(fru_device_id)) {
        g_push_pwr_btn_flag = PUSH_PWR_BTN_WAIT;
        
        // 先控制其下电
        ret = pp_fru_pwr_ctrl(fru_device_id, POWER_OFF_FORCE);
        
        (void)dal_get_product_version_num(&prod_ver);
        if (prod_ver >= PRODUCT_VERSION_V5 && ret == COMP_CODE_SUCCESS) {
            while (count < 100) { 
                count++;
                if (g_push_pwr_btn_flag == PUSH_PWR_BTN_YES) {
                    debug_log(DLOG_INFO, "Power button is pushed");
                    condition = 1;
                    tick_cnt_pwr_btn = vos_tick_get();
                    break;
                } else if (g_push_pwr_btn_flag == PUSH_PWR_BTN_NO) {
                    break;
                }
                vos_task_delay(100); 
            }
        }
        g_push_pwr_btn_flag = PUSH_PWR_BTN_WAIT;
        pp_execute_waite_pwr_off(fru_device_id);
        if (condition) {
            tick_cnt_pwr_off = vos_tick_get();
            *pwr_off_interval =
                (guint32)(tick_cnt_pwr_off - tick_cnt_pwr_btn); 
        }
    } else {
        debug_log(DLOG_ERROR, "already power off from power cycle!");
    }

    return;
}


LOCAL guchar pp_execute_power_cycle(guchar fru_device_id, guint32 interval)
{
    guint32 pwr_off_interval = 0;
    OBJ_HANDLE pfn_handle = 0;
    guint8 power_policies = 0;

    // 可管理fru才能执行此操作
    if ((fru_device_id >= hs_get_fru_num()) || (FALSE == hs_is_fru_managed(fru_device_id))) {
        return PAYLOAD_FAIL;
    }
    // 如果fru不在位，返回错误状态
    if (FALSE == hop_is_fru_present(fru_device_id)) {
        debug_log(DLOG_ERROR, "pp_execute_power_cycle: fru%d not present!\r\n", fru_device_id);
        return PAYLOAD_FAIL;
    }
    // 1.先执行下电操作
    debug_log(DLOG_ERROR, "perform power cycle off part, interval 0x%x.", interval);
    pp_execute_pwr_cycle_off(fru_device_id, &pwr_off_interval);
    if (FALSE == pp_is_cycle_in_progress(fru_device_id)) {
        debug_log(DLOG_ERROR, "already cancle power cycle before cycle interval!\r\n");
        return PAYLOAD_SUCCESS;
    }

    (void)dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &pfn_handle);
    (void)dal_get_property_value_byte(pfn_handle, PFN_ACTIVATE_UPGRADE_BIOS_FLAG, &power_policies);
    if (power_policies == POWER_ON_AFTER_UP_BIOS) {
        
        pp_set_cycle_progress_state(CHASSIS_FRU_ID, FALSE);
        pp_set_cycle_interval(CHASSIS_FRU_ID, 0);
        pp_save_cycle_param();
        debug_log(DLOG_ERROR, "Cancle power cycle after set ActivateBIOSUpgradeFlag = poweron!");
    }

    // 2.下电后才开始计时，计时结束后再上电
    if (pwr_off_interval < interval) {
        
        interval -= pwr_off_interval;
        debug_log(DLOG_INFO, "pwr_off_interval = %d, interval = %d", pwr_off_interval, interval);
    }
    vos_task_delay(interval);
    if (FALSE == pp_is_cycle_in_progress(fru_device_id)) {
        debug_log(DLOG_ERROR, "already cancle power cycle after cycle interval!\r\n");
        return PAYLOAD_SUCCESS;
    }
    // 3.计时已完成，重新上电
    debug_log(DLOG_ERROR, "0x%xms later, perform power cycle on part.", interval);
    pp_execute_pwr_cycle_on(fru_device_id);
    return PAYLOAD_SUCCESS;
}


void pp_set_vrd_validate_method(const gchar *method_name, guint8 method_value)
{
    gint32 ret_val;
    OBJ_HANDLE vrd_handle = 0;
    GSList *input_list = NULL;

    ret_val = dal_get_object_handle_nth(CLASS_VRD_UPGRADE_MGNT, 0, &vrd_handle);
    if (ret_val != RET_OK) {
        debug_log(DLOG_DEBUG, "get obj_handle fail, ret is %d\n", ret_val);
        return;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(method_value));

    ret_val = dfl_call_class_method(vrd_handle, method_name, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR,
            "Failed to call dfl_call_class_method. method_name =%s, method_value=%d, ret_val=%d. \r\n", method_name,
            method_value, ret_val);
    }

    return;
}

LOCAL gboolean check_vrd_version_change(void)
{
    gint32 ret;
    OBJ_HANDLE vrd_handle = 0;
    guint8 version_change = 0;

    ret = dal_get_object_handle_nth(CLASS_VRD_UPGRADE_MGNT, 0, &vrd_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Failed to get object of class[%s], ret=%d", __FUNCTION__, CLASS_VRD_UPGRADE_MGNT,
            ret);
        return FALSE;
    }

    (void)dal_get_property_value_byte(vrd_handle, PROPERTY_VRD_VERSION_CHANGE_FLAG, &version_change);
    if (version_change == VRD_VERSION_CHANGED) {
        return TRUE;
    } else {
        return FALSE;
    }
}


LOCAL void pp_set_vrd_validate_action(void)
{
    gint32 ret_val;
    OBJ_HANDLE vrd_handle = 0;
    
    if (check_vrd_version_change() != TRUE) {
        debug_log(DLOG_ERROR, "VRD version not change, don't need to set valid action.");
        return;
    }

    
    ret_val = dal_get_object_handle_nth(CLASS_VRD_UPGRADE_MGNT, 0, &vrd_handle);
    if (ret_val == RET_OK) {
        pp_set_vrd_validate_method(METHOD_VRD_SET_VALIDATE_ACTION, DO_POWER_ON_AFTER_VALIDATE_VRD);
        debug_log(DLOG_ERROR, "Set action after validate vrd(power on).");
    }
    return;
}


LOCAL gint32 pp_get_remote_cached_upgrade_bios_path(gchar *file_path, guint32 sz)
{
    errno_t safe_fun_ret;
    gint32 result;
    GSList *output_list = NULL;
    guint8 status;
    const gchar *file_path_tmp = NULL;

    if (file_path == NULL || sz == 0) {
        return RET_ERR;
    }
    
    result =
        dfl_call_remote_class_method(OBJECT_PFN, METHOD_PFN_GET_CACHE_UPGRADE_FILE_STATUS, NULL, NULL, &output_list);
    if (result != RET_OK) {
        return RET_ERR;
    }
    status = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    if (status == 0) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return RET_ERR;
    }
    file_path_tmp = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), 0);
    if (file_path_tmp == NULL) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return RET_ERR;
    }
    safe_fun_ret = strncpy_s(file_path, sz, file_path_tmp, sz - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    if (strlen(file_path) == 0) {
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL void pp_get_node_info(guint8 *partition, guint8 *node_mode)
{
    gint32 result;
    OBJ_HANDLE object_handle = 0;

    if (partition == NULL || node_mode == NULL) {
        return;
    }
    result = dfl_get_object_handle("xmlPartition", &object_handle);
    if (result != DFL_OK) {
        return;
    }
    
    result = dal_get_property_value_byte(object_handle, XML_PARTITION, partition);
    if (result != RET_OK) {
        return;
    }
    result = dal_get_property_value_byte(object_handle, NODE_MODE, node_mode);
    if (result != RET_OK) {
        return;
    }

    return;
}

gint32 pp_get_remote_upgrade_bios_state(void)
{
    guint8 xml_partition = 0;
    guint8 node_mode = 0;
    gint32 result;
    gchar cache_bios_file[MAX_FILEPATH_LENGTH] = {0};
    OBJ_HANDLE object_handle = 0;

    
    result = dfl_get_object_handle(CLASS_COMPUTER_PARTITION, &object_handle);
    if (result == DFL_OK) {
        return RET_ERR;
    }
    pp_get_node_info(&xml_partition, &node_mode);

    if ((xml_partition == RH8100_SINGLE_SYS) && (node_mode == RH8100_MASTER_BMC)) {
        result = pp_get_remote_cached_upgrade_bios_path(cache_bios_file, MAX_FILEPATH_LENGTH);
        return result;
    }

    return RET_ERR;
}


LOCAL void pp_action_before_powercycle(void)
{
    gint32 ret;
    OBJ_HANDLE pfn_handle = 0;
    GSList *input_list = NULL;

    
    if ((TRUE == vos_get_file_accessible(CACHE_BIOS_HPM_FILE)) ||
        (TRUE == vos_get_file_accessible(CACHE_BIOS_COMP_FILE)) || (RET_OK == pp_get_remote_upgrade_bios_state())) {
        ret = dfl_get_object_handle(OBJECT_PFN, &pfn_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Failed to call dfl_get_object_handle.\r\n");
            return;
        }
        
        input_list = g_slist_append(input_list, g_variant_new_byte(POWER_ON_AFTER_UP_BIOS));
        ret = dfl_call_class_method(pfn_handle, METHOD_PFN_SET_ACTIVATE_BIOS_UPGRADE_FLAG, NULL, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Failed to call dfl_call_class_method("METHOD_PFN_SET_ACTIVATE_BIOS_UPGRADE_FLAG").");
        }

        debug_log(DLOG_ERROR, "Do power cycle action, and set power on after up bios");
    }
}

static gint32 _fru_mgnt_get(OBJ_HANDLE object_handle, gpointer user_data)
{
    gint32 result;
    guint8 fru_id = 0;
    guint8 is_mgnt = 0;
    guint8 type = 0;
    guint8 *out_mgnt = (guint8 *)user_data;

    result = dal_get_property_value_byte(object_handle, PROPERTY_FRU_ID, &fru_id);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "dfl_get_property_value FRUID fail!(result=%d)\r\n", result);
        return RET_OK;
    }

    result = dal_get_property_value_byte(object_handle, PROPERTY_FRU_TYPE, &type);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "get fru type fail!(result=%d)", result);
        return RET_OK;
    }

    if (fru_id == 0 && type == DEVICE_TYPE_PCIE_CARD) {
        debug_log(DLOG_ERROR, "type is pcie card, but fruid is 0!");
        return RET_OK;
    }

    result = dal_get_property_value_byte(object_handle, PROPERTY_FRU_IS_MANAGED, &is_mgnt);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "dfl_get_property_value ismgnt fail!(result=%d)\r\n", result);
        return RET_OK;
    }
    out_mgnt[fru_id] = is_mgnt;
    return RET_OK;
}



LOCAL gint32 forced_power_cycle(void)
{
    OBJ_HANDLE obj_handle_tmp = 0;
    // 如果系统未上电，直接返回ERR
    if (hop_get_pwr_signal(CHASSIS_FRU_ID) == HARD_POWER_OFF) {
        debug_log(DLOG_ERROR, "power off and can't warm reset action.");
        return COMP_CODE_STATUS_INVALID;
    }

    gint32 ret = dal_get_object_handle_nth(CLASS_CHASSISPAYLOAD, 0, &obj_handle_tmp);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get obj [class:%s] handle failed! ret = %d", CLASS_CHASSISPAYLOAD, ret);
        return RET_ERR;
    }

    ret = dal_set_property_value_byte(obj_handle_tmp, PROPERTY_FORCED_POWER_CYCLE, 1, DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Set property(%s) failed, ret = %d",
            dfl_get_object_name(obj_handle_tmp), PROPERTY_FORCED_POWER_CYCLE, ret);
        return RET_ERR;
    }
    
    vos_task_delay(100);
    ret = dal_set_property_value_byte(obj_handle_tmp, PROPERTY_FORCED_POWER_CYCLE, 0, DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Set property(%s) failed, ret = %d",
            dfl_get_object_name(obj_handle_tmp), PROPERTY_FORCED_POWER_CYCLE, ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL void pp_power_cycle_action(guchar fru_device_id, FRU_PAYLOAD_S *fru_payload)
{
    guint32 interval;
    if (fru_payload->m_pwr_cycle_cb[fru_device_id].cycle_in_progress == TRUE) {
        interval = fru_payload->m_pwr_cycle_cb[fru_device_id].interval;
        
        g_printf("Fru%d Power cycle progressing(%d seconds)...", fru_device_id, interval / 1000);
        debug_log(
            
            DLOG_DEBUG, "Fru%d Power cycle progressing(%d seconds)...", fru_device_id, interval / 1000);
        
        pp_action_before_powercycle();
        if (fru_device_id == CHASSIS_FRU_ID) {
            
            pp_set_vrd_validate_action();
        }
        
        if (dal_is_pangea_enterprise_board() == TRUE) {
            (void)forced_power_cycle();
        } else {
            
            
            (void)pp_execute_power_cycle(fru_device_id, interval);
        }
        
        fru_payload->m_pwr_cycle_cb[fru_device_id].interval = 0;
        fru_payload->m_pwr_cycle_cb[fru_device_id].cycle_in_progress = FALSE;
        
        per_save((guint8 *)fru_payload->m_pwr_cycle_cb);
        g_printf("\r\nFru%d Power cycle control done.\r\n", fru_device_id);
        debug_log(DLOG_ERROR, "Fru%d Power cycle control done.", fru_device_id);
    }
}


void pp_cycle_task(void)
{
    guchar fru_device_id;
    guint32 fru_num;
    guint8 is_mgnt[256] = {0}; 

    fru_num = hs_get_fru_num();

    (void)prctl(PR_SET_NAME, (gulong)"PpcycleTask");

    for (;;) {
        (void)memset_s(is_mgnt, sizeof(is_mgnt), 0, sizeof(is_mgnt));
        (void)dfl_foreach_object(CLASS_FRU, _fru_mgnt_get, (gpointer)is_mgnt, NULL);
        FRU_PAYLOAD_S *fru_payload = pp_relate_fru_payload_info();

        for (fru_device_id = 0; fru_device_id < fru_num; fru_device_id++) {
            
            if (is_mgnt[fru_device_id] == TRUE) {
                
                
                
                pp_power_cycle_action(fru_device_id, fru_payload);
            }
        }
        vos_task_delay(500); 
    }
}