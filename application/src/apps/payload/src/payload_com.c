

#include <glib.h>
#include <glib/gprintf.h>
#include <stdio.h>
#include <string.h>
#include "pme/common/mscm_vos.h"
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "payload_hs.h"
#include "payload_pwr.h"
#include "payload_com.h"
#include "payload_ipmi.h"
#include "payload_storage.h"
#include "wdt2_main.h"

static guint32 g_manufacture_id_payload = 0;


static gint32 pp_get_and_save_manufacture_id(void)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret_val;
    GVariant *property_data = NULL;

    ret_val = dfl_get_object_handle(BMC_OBJECT_NAME, &obj_handle);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return PAYLOAD_FAIL;
    }

    
    ret_val = dfl_get_property_value(obj_handle, BMC_MANU_ID_NAME, &property_data);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return ret_val;
    }

    if (property_data == NULL) {
        
        debug_log(DLOG_ERROR, DB_STR_GET_PROPERTY_NULL);
        return PAYLOAD_FAIL;
    }

    g_manufacture_id_payload = g_variant_get_uint32(property_data);
    g_variant_unref(property_data);

    return PAYLOAD_SUCCESS;
}


gint32 pp_judge_manu_id_valid_bios(const guint8 *manufacture_id)
{
    guint32 manu_id;
    guint32 property_manu_id = g_manufacture_id_payload;

    g_return_val_if_fail(manufacture_id != NULL, PAYLOAD_FAIL);
    
    manu_id = MAKE_DWORD(0, manufacture_id[2], manufacture_id[1], manufacture_id[0]);
    if (manu_id != property_manu_id) {
        debug_log(DLOG_ERROR, "ManufactureId:%d(ShouldBe:%d) is invalid!\n", manu_id, property_manu_id);
        return PAYLOAD_FAIL;
    }

    return PAYLOAD_SUCCESS;
}


gint32 pp_judge_poweroff_tm_valid(guint32 pwroff_tm)
{
    gint32 result;
    guint32 max_tm;
    guint32 min_tm;
    OBJ_HANDLE object_handle = 0;
    GVariant *property_data = NULL;

    // 更新ChassisPayload对象的PowerRestorePolicy状态
    result = dfl_get_binded_object(CLASS_CHASSISPAYLOAD, 0x00, &object_handle);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)", result);
        return PAYLOAD_FAIL;
    }

    
    result = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_PWROFF_TM_MAX, &property_data);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get ChassisPayload object %s fail!(result=%d)", PROPERTY_PAYLOAD_PWROFF_TM_MAX,
            result);
        return PAYLOAD_FAIL;
    }

    max_tm = g_variant_get_uint32(property_data);
    g_variant_unref(property_data);
    property_data = NULL;

    
    result = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_PWROFF_TM_MIN, &property_data);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get ChassisPayload object %s fail!(result=%d)", PROPERTY_PAYLOAD_PWROFF_TM_MIN,
            result);
        return PAYLOAD_FAIL;
    }

    min_tm = g_variant_get_uint32(property_data);
    g_variant_unref(property_data);

    max_tm *= PAYLOAD_TIME_UNIT;
    min_tm *= PAYLOAD_TIME_UNIT;

    if ((pwroff_tm > max_tm) || (pwroff_tm < min_tm)) {
        debug_log(DLOG_ERROR, "poweroff timeout:%u(Max:%u,Min%u,Unit:100ms) invalid.", pwroff_tm, max_tm, min_tm);
        return PAYLOAD_FAIL;
    }

    return PAYLOAD_SUCCESS;
}


gchar *pp_policy_to_string(guchar restore_policy)
{
    switch (restore_policy) {
        case POWER_RESTORE_POLICY_NO_CHANGED:

            return "no changed";

        case POWER_RESTORE_POLICY_ALWAYS_ON:

            return "always on";

        case POWER_RESTORE_POLICY_PREVIOUS:

            return "previous";

        case POWER_RESTORE_POLICY_ALWAYS_OFF:

            return "always off";

        default:
            return "unknow";
    }
}


gint32 pp_get_server_type(guint8 *server_type)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret_val;
    GVariant *property_data = NULL;

    ret_val = dfl_get_object_handle(BMC_PRODUCT_OBJ_NAME_APP, &obj_handle);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return RET_ERR;
    }

    ret_val = dfl_get_property_value(obj_handle, PROPERTY_SERVER_TYPE, &property_data);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return RET_ERR;
    }

    *server_type = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    return RET_OK;
}


gint32 pp_check_active_option(guchar option, guchar fru_device_id)
{
    gint32 result;
    OBJ_HANDLE fru_handle = 0;
    guint8 temp = 0;

    (void)dal_get_specific_object_byte(CLASS_PAYLOAD, PROPERTY_PAYLOAD_FRUID, fru_device_id, &fru_handle);

    (void)dal_get_property_value_byte(fru_handle, PROPERTY_PAYLOAD_ACTIVE_CTL_OPTION, &temp);

    
    if (option >= 8) {
        return RET_ERR;
    }
    if ((0x1 << option) & temp) {
        result = RET_OK;
    } else {
        result = RET_ERR;
    }
    return result;
}


gint32 pp_check_chassis_ctl_option(guchar option, guchar fru_device_id)
{
    gint32 result;
    OBJ_HANDLE fru_handle = 0;
    guint8 temp = 0;

    (void)dal_get_specific_object_byte(CLASS_PAYLOAD, PROPERTY_PAYLOAD_FRUID, fru_device_id, &fru_handle);

    (void)dal_get_property_value_byte(fru_handle, PROPERTY_PAYLOAD_CHASSIS_CTL_OPTION, &temp);

    
    if (option >= 8) {
        return RET_ERR;
    }

    if ((0x1 << option) & temp) {
        result = RET_OK;
    } else {
        result = COMP_CODE_STATUS_INVALID;
    }
    return result;
}


gint32 hop_check_fru_ctl_option(guchar option, guchar fru_device_id)
{
    gint32 result;
    OBJ_HANDLE fru_handle = 0;
    GVariant *property_value = NULL;
    guint8 temp;

    
    result = dfl_get_binded_object(CLASS_PAYLOAD, fru_device_id, &fru_handle);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "find fru%d obj faile!\r\n", fru_device_id);
        return result;
    }

    result = dfl_get_property_value(fru_handle, PROPERTY_PAYLOAD_CTL_OPTION, &property_value);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d prop FruCtlOption faile!\r\n", fru_device_id);
        return result;
    }
    temp = g_variant_get_byte(property_value);
    g_variant_unref(property_value);

    
    if (option >= 8) {
        return RET_ERR;
    }
    if ((0x1 << option) & temp) {
        result = RET_OK;
    } else {
        result = COMP_CODE_INVALID_FIELD;
    }

    return result;
}


static guchar payload_infra_init(void)
{
    return PAYLOAD_SUCCESS;
}


gint32 payload_init(void)
{
    OBJ_HANDLE obj_product_handle = 0;
    guint8 server_type = 0;
    TASKID osca_hotswap_fix_task_id = 0x00;
    gint32 ret;

    payload_orig_before_ac_check();
    if (PAYLOAD_SUCCESS == payload_infra_init()) {
        
        if (PAYLOAD_SUCCESS != pp_payload_init()) {
            debug_log(DLOG_ERROR, "%s: abort payload init due to pp_payload_init init fail\r\n", __FUNCTION__);
            return PAYLOAD_FAIL;
        }

        if (PAYLOAD_SUCCESS != hs_fru_init()) {
            debug_log(DLOG_ERROR, "%s: abort payload init due to hs_fru_init init fail\r\n", __FUNCTION__);
            return PAYLOAD_FAIL;
        }
        
        if (PAYLOAD_SUCCESS != hs_init()) {
            debug_log(DLOG_ERROR, "%s: abort payload init due to hs_init init fail\r\n", __FUNCTION__);
            return PAYLOAD_FAIL;
        }
        
        if (PAYLOAD_SUCCESS != pp_init()) {
            debug_log(DLOG_ERROR, "%s: abort payload init due to pp_init init fail\r\n", __FUNCTION__);
            return PAYLOAD_FAIL;
        }
        
        if (PAYLOAD_SUCCESS != wdt_init()) {
            debug_log(DLOG_ERROR, "%s: abort payload init due to wdt_init init fail\r\n", __FUNCTION__);
            return PAYLOAD_FAIL;
        }

        
        hs_process_init(); // move to init

        
        hs_thread_init(); // move to init

        pp_ctrl_init(); // move to init

        if (PAYLOAD_SUCCESS != pp_get_and_save_manufacture_id()) {
            debug_log(DLOG_ERROR, "%s: pp_get_and_save_manufacture_id fail\r\n", __FUNCTION__);
        }

        
        
        hs_start();
    } else {
        
        debug_log(DLOG_ERROR, "payload_init: skip payload init due to payload queue init fail\r\n");
    }
    
    (void)dal_get_object_handle_nth(CLASS_NAME_PME_PRODUCT, 0, &obj_product_handle);
    (void)dal_get_property_value_byte(obj_product_handle, PROPERTY_MGMT_SOFTWARE_TYPE, &server_type);
    if (server_type == MGMT_SOFTWARE_TYPE_BLADE || server_type == MGMT_SOFTWARE_TYPE_SWITCH_BLADE ||
        server_type == MGMT_SOFTWARE_TYPE_EM_X86_BMC) {
        ret = vos_task_create(&osca_hotswap_fix_task_id, "oscahotswapfix", (TASK_ENTRY)osca_hotswap_fix_task, 0,
            DEFAULT_PRIORITY);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "osca hotswap fix task create err!\n");
        }
    }

    return PAYLOAD_SUCCESS;
}


gint32 payload_start(void)
{
    if (dal_get_cpld_selftest_status() == TRUE) {
        debug_log(DLOG_ERROR, "%s: cpld self test failed.\r\n", __FUNCTION__);
        g_printf("----------cpld self test failed.--------\r\n");
        return RET_OK;
    }
    hs_state_restore(); // move to init

    
    pp_start();

    
    wdt_start();

    
    boardctrl_resetreason_record_init();

    return RET_OK;
}

void payload_orig_before_ac_check(void)
{
    gint32 ret;
    guchar status = 0;
    OBJ_HANDLE object_handle;
    gchar accesor_name[MAX_NAME_SIZE] = {0};

    ret = dal_get_specific_object_byte(CLASS_PAYLOAD, PROPERTY_PAYLOAD_FRUID, CHASSIS_FRU_ID, &object_handle);
    if (ret == DFL_OK) {
        (void)dal_get_property_value_byte(object_handle, PROPERTY_PAYLOAD_PWR_ORIG_BEF_AC, &status);
    } else {
        debug_log(DLOG_ERROR, "payload_init get ChassisPayload object fail!(result=%d)\r\n", ret);
    }
    debug_log(DLOG_ERROR, "%s ChassisPayload OriginalPwrBeforeACLost status=%d.", __FUNCTION__, status);
    if (status == 1) { 
        ret = dfl_get_property_accessor(object_handle, PROPERTY_PAYLOAD_PWR_BEF_AC, accesor_name, MAX_NAME_SIZE);
        if (ret == DFL_OK) {
            ret = dal_set_property_value_byte(object_handle, PROPERTY_PAYLOAD_PWR_BEF_AC, status, DF_HW);
        } else {
            ret = dal_set_property_value_byte(object_handle, PROPERTY_PAYLOAD_PWR_BEF_AC, status, DF_NONE);
        }
        debug_log(DLOG_ERROR, "%s Set ChassisPayload PwrBeforeACLost ret(%d).", __FUNCTION__, ret);
    }
    ret = dal_set_property_value_byte(object_handle, PROPERTY_PAYLOAD_PWR_ORIG_BEF_AC, 1, DF_HW); 
    vos_task_delay(TASK_DELAY_50MS);
    
    ret = dal_set_property_value_byte(object_handle, PROPERTY_PAYLOAD_PWR_ORIG_BEF_AC, 0, DF_HW);
    debug_log(DLOG_ERROR, "%s Set ChassisPayload OriginalPwrBeforeACLost ret(%d).", __FUNCTION__, ret);
    return;
}