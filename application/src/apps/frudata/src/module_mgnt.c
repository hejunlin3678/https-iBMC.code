

#include "module_mgnt.h"
#include "common.h"
#include "media/e2p_large.h"
#include "component/module_init.h"
#include "component/base.h"
#include "elabel/elabel.h"
#include "elabel/base.h"
#include "elabel/module_init.h"
#include "prepare_fru.h"
#include "intf/adapter.h"
#include "media/port_rate.h"

typedef struct tag_cpld_test_status_s {
    gchar *cpld_test_reg; 
    gchar *cpld_status;   
    guint8 cnt;           
} CPLD_TEST_STATUS;

G_LOCK_DEFINE(g_shared_cpld_scan_dis_lock);
LOCAL guint8 g_shared_cpld_scan_disable = 0;
gint32 method_fru_stop_shared_cpld_scan(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint8 is_stop = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    debug_log(DLOG_INFO, "%s, set shared cpld scan routine flag, stop=%u", __FUNCTION__, is_stop);

    G_LOCK(g_shared_cpld_scan_dis_lock);
    g_shared_cpld_scan_disable = is_stop;
    G_UNLOCK(g_shared_cpld_scan_dis_lock);

    return RET_OK;
}


LOCAL gboolean cpld_need_scan(OBJ_HANDLE object_handle)
{
    guint8 fru_type = 0;

    gint32 ret = dal_get_property_value_byte(object_handle, PROPERTY_FRU_TYPE, &fru_type);
    if (ret != DFL_OK) {
        return TRUE;
    }

    
    if (dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_ARCTIC_CTRL) &&
        (fru_type == COMPONENT_TYPE_EPM || fru_type == COMPONENT_TYPE_FAN_MODULE)) {
        G_LOCK(g_shared_cpld_scan_dis_lock);
        guint8 flag = g_shared_cpld_scan_disable;
        G_UNLOCK(g_shared_cpld_scan_dis_lock);
        if (flag) {
            return FALSE;
        }
        return is_smm_active();
    }

    return TRUE;
}

G_LOCK_DEFINE_STATIC(g_cpld_scan_mutex);


LOCAL gboolean _is_cpld_obj_scaned(OBJ_HANDLE object_handle)
{
    const gchar *fru_name = dfl_get_object_name(object_handle);
    gboolean ret = FALSE;
    static GHashTable *cpld_table = NULL;

    G_LOCK(g_cpld_scan_mutex);
    if (cpld_table == NULL) {
        cpld_table = g_hash_table_new(g_str_hash, g_str_equal);
    }

    if (g_hash_table_contains(cpld_table, (gconstpointer)fru_name) == TRUE) {
        ret = TRUE;
        debug_log(DLOG_INFO, "%s: [%s] CPLD already scanned, do nothing.", __FUNCTION__, fru_name);
    } else {
        g_hash_table_insert(cpld_table, (gpointer)g_strdup(fru_name), (gpointer)0);
        debug_log(DLOG_INFO, "%s: [%s] CPLD need to be scanned.", __FUNCTION__, fru_name);
    }
    G_UNLOCK(g_cpld_scan_mutex);

    return ret;
}


LOCAL gboolean is_cpld_validating(void)
{
    OBJ_HANDLE upg_obj = 0;
    if (dfl_get_object_handle(OBJECT_PFN, &upg_obj) != RET_OK) {
        debug_log(DLOG_ERROR, "get upg obj fialed!");
        return FALSE;
    }
    guchar validating_cpld_flag = FALSE;
    
    (void)dal_get_property_value_byte(upg_obj, PFN_VALIDATING_CPLD_NAME, &validating_cpld_flag);

    return validating_cpld_flag == TRUE;
}


LOCAL gint32 test_cpld_reg(OBJ_HANDLE object_handle, const gchar* prop_reg, guint8 val_write)
{
    const gchar *obj_name = dfl_get_object_name(object_handle);
    
    gint32 ret = dal_set_property_value_byte(object_handle, prop_reg, val_write, DF_HW);
    debug_log(DLOG_INFO, "cpld scan %s, set %s 0x%x, ret %d.", obj_name, prop_reg, val_write, ret);
 
    vos_task_delay(500); 
 
    
    guint8 val_read = 0;
    ret = dal_get_property_value_byte(object_handle, prop_reg, &val_read);
    debug_log(DLOG_INFO, "cpld scan %s, get %s 0x%x, ret %d.", obj_name, prop_reg, val_read, ret);
 
    return val_write == val_read;
}


LOCAL void __cpld_scan_task(OBJ_HANDLE object_handle)
{
    gchar accessor[MAX_NAME_SIZE];
    const gchar *obj_name = dfl_get_object_name(object_handle);

    CPLD_TEST_STATUS test_status[] = {
        { PROPERTY_FRU_CPLD_TEST_REG, PROPERTY_FRU_CPLD_STATUS, 0 },
        { PROPERTY_FRU_CPLD2_TEST_REG, PROPERTY_FRU_CPLD2_STATUS, 0 },
        { PROPERTY_FRU_CPLD3_TEST_REG, PROPERTY_FRU_CPLD3_STATUS, 0 }
    };

    if (_is_cpld_obj_scaned(object_handle)) {
        return;
    }

    (void)prctl(PR_SET_NAME, (unsigned long)"CpldScanTask");

    for (;;) {
        // cpld自检通用周期5s
        if (dal_is_aggregation_management_chassis_product() != TRUE) {
            vos_task_delay(TASK_DELAY_ONE_SECOND * 5);
        } else {
            
            vos_task_delay(TASK_DELAY_ONE_SECOND * 2);
        }

        
        if (cpld_need_scan(object_handle) == FALSE || is_cpld_validating() == TRUE) {
            continue;
        }

        for (guint8 i = 0; i < sizeof(test_status) / sizeof(CPLD_TEST_STATUS); i++) {
            gchar *prop_reg = test_status[i].cpld_test_reg;
            
            if (dfl_get_property_accessor(object_handle, prop_reg, accessor, MAX_NAME_SIZE) != DFL_OK) {
                continue;
            }

            if (test_cpld_reg(object_handle, prop_reg, 0x55) && test_cpld_reg(object_handle, prop_reg, 0xAA)) {
                
                test_status[i].cnt = 0;
                (void)dal_set_property_value_byte(object_handle, test_status[i].cpld_status, 0, DF_NONE);
                continue;
            }

            
            test_status[i].cnt += ((test_status[i].cnt < 0xFF) ? 1 : 0); 
            if (test_status[i].cnt == 3) { 
                debug_log(DLOG_ERROR, "cpld scan %s, self test failed.", obj_name);
                (void)dal_set_property_value_byte(object_handle, test_status[i].cpld_status, 1, DF_NONE);
            }
        }
    }
}


gint32 create_cpld_scan_task(OBJ_HANDLE object_handle, gpointer user_data)
{
    gchar acc_name[MAX_NAME_SIZE] = {0};
    gint32 ret = dfl_get_property_accessor(object_handle, PROPERTY_FRU_CPLD_TEST_REG, acc_name, MAX_NAME_SIZE);
    if (ret != DFL_OK) {
        return RET_OK;
    }

    gulong cpld_scan_task_id = 0;
    (void)vos_task_create(&cpld_scan_task_id, "CpldScanTask", (TASK_ENTRY)__cpld_scan_task, (void *)object_handle,
        DEFAULT_PRIORITY);

    debug_log(DLOG_INFO, "%s success to creat cpld task for (%s)", __FUNCTION__, dfl_get_object_name(object_handle));
    return RET_OK;
}


LOCAL void __wait_all_fru_initialized(void)
{
    gint32 retry_times = 0;

    for (;;) {
        if (retry_times++ >= 45) {
            debug_log(DLOG_ERROR, "%s check all fru timeout", __FUNCTION__);
            break;
        }

        vos_task_delay(1000);

        GSList *obj_handle_list = NULL;
        gint32 ret = dfl_get_object_list(CLASS_FRU, &obj_handle_list);
        if (ret != DFL_OK) {
            retry_times++;
            continue;
        }

        gboolean finished = TRUE;
        for (GSList *node = obj_handle_list; node != NULL; node = node->next) {
            if (!is_fru_initialized((OBJ_HANDLE)node->data)) {
                finished = FALSE;
                break;
            }
        }

        g_slist_free(obj_handle_list);

        if (finished) {
            break;
        }
    }

    return;
}


gint32 frudata_exit(REBOOT_CTRL ctrl)
{
    if (ctrl == REBOOT_PREPARE) {
        return RET_OK;
    }

    
    if (ctrl == REBOOT_FORCE || ctrl == REBOOT_PERFORM) {
        check_eeprom_write_status();
        (void)per_exit();
        return RET_OK;
    }

    if (ctrl == REBOOT_CANCEL) {
        return RET_OK;
    }

    return RET_OK;
}

gint32 fru_elabel_init(void)
{
    
    gint32 ret = dfl_foreach_object(CLASS_FRU, init_fru, NULL, NULL);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "call dfl_foreach_object failed(class = %s), error = %d.", CLASS_FRU, ret);
        return ret;
    }

    
    __wait_all_fru_initialized();

    
    ret = dfl_foreach_object(CLASS_FRU, fru_elabel_product_combine, NULL, NULL);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "call dfl_foreach_object failed(class = %s), error = %d.", CLASS_FRU, ret);
        return ret;
    }

    
    ret = dfl_foreach_object(CLASS_FRU, fru_elabel_manufacture_syn_hook, NULL, NULL);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "call dfl_foreach_object failed(class = %s), error = %d.", CLASS_FRU, ret);
        return ret;
    }

    return ret;
}


#ifdef ENABLE_ASAN
int __lsan_do_recoverable_leak_check(void);
LOCAL void mem_leak_handler(int sig)
{
    debug_log(DLOG_ERROR, "mem_leak_handler start");
    __lsan_do_recoverable_leak_check();
    debug_log(DLOG_ERROR, "mem_leak_handler end");
}
#endif


gint32 frudata_init(void)
{
    
    #ifdef ENABLE_ASAN
        if (signal(44, mem_leak_handler) == SIG_ERR) {  // 44 表示执行ASAN检测的信号
            debug_log(DLOG_ERROR, "Could not set signal handler");
        }
    #endif

    init_gcov_signal_for_test();
    disable_fru_init();

    init_prepare_fru();

    elabel_module_init();

    component_module_init();

    (void)create_component_obj_add_task();

    
    gint32 ret = dfl_foreach_object(CLASS_COMPONENT, init_component, NULL, NULL);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "call dfl_foreach_object failed(class = %s), error = %d.", CLASS_COMPONENT, ret);
    }

    
    ret = dfl_foreach_object(CLASS_FRU, create_cpld_scan_task, NULL, NULL);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "call dfl_foreach_object failed(class = %s), error = %d", CLASS_FRU, ret);
    }

    return RET_OK;
}


gint32 frudata_start(void)
{
    TASKID get_chassis_sn_task = 0;

#ifdef DFT_ENABLED
    
    (void)dfl_foreach_object(DFT_ITEM_CLASS, eeprom_self_test_dft_hook, NULL, NULL);
#endif

    (void)vos_task_create(&get_chassis_sn_task, "getChassisSN", (TASK_ENTRY)task_get_chassis_sn,
        NULL, DEFAULT_PRIORITY);

    return RET_OK;
}
