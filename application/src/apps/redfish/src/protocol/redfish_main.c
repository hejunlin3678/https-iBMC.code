
#include <malloc.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "redfish_main.h"
#include "redfish_http.h"
#include "redfish_message.h"
#include "redfish_class.h"
#include "redfish_task.h"
#include "redfish_main.h"
#include "redfish_event.h"
#include "redfish_util.h"
#include "redfish_forward.h"
#include "pme_app/kmc/kmc.h"

#define CONFG_TAR_FILE_SHA256_TMP       "/dev/shm/redfish_sha256sum"
#define CONFG_TAR_FILE_SHA256           CONFG_FILE_PATH"/sha256sum"
#define SHA256_STR_SIZE                 64


LOCAL GMutex g_rf_encrypted_data_mutex;
LOCAL guint8 g_rf_encrypted_data_rollback_done = FALSE;
LOCAL guint8 g_rf_encrypted_data_update_state = 1;



LOCAL void redfish_rpc_test_task(void)
{
    gint32 ret;

    (void)prctl(PR_SET_NAME, (uintptr_t) "rfTestTask");
    // 由于升级回退的设置操作与降权限存在并发，此处延迟redfish模块降权限
    vos_task_delay(10000UL);

    do {
        ret = dfl_rpc_selftest();
        if (VOS_OK != ret) {
            vos_task_delay(1000UL);
        }
    } while (VOS_OK != ret);

    
    #if defined(ENABLE_ASAN) || defined(ENABLE_FUZZ_COV)
        change_gid_uid(ROOT_USER_GID, ROOT_USER_UID);
    #else
        change_gid_uid(APPS_USER_GID, REDFISH_USER_UID);
    #endif

    return;
}


LOCAL gint32 _rf_dump_component_info(const gchar *path)
{
    gchar               dump_filename[MAX_FILENAME_LEN] = {0};
    gint32 ret;
    GSList *component_list = NULL;
    GSList *comp_list_iter = NULL;
    json_object *component_dump_jso = NULL;
    OBJ_HANDLE comp_handle = 0;
    gchar               comp_dump_name[MAX_RSC_NAME_LEN] = {0};
    gchar               comp_uri[MAX_URI_LENGTH] = {0};

    ret = snprintf_s(dump_filename, MAX_FILENAME_LEN, MAX_FILENAME_LEN - 1, "%s/component_uri.json", path);
    return_val_do_info_if_fail(ret > 0, VOS_ERR,
        debug_log(DLOG_ERROR, "format redfish component dump path failed, ret is %d", ret));

    ret = dfl_get_object_list(CLASS_COMPONENT, &component_list);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "get object list for class [%s] failed, ret is %d", CLASS_COMPONENT, ret));

    component_dump_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != component_dump_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "alloc new json object failed");
        g_slist_free(component_list));

    for (comp_list_iter = component_list; NULL != comp_list_iter; comp_list_iter = comp_list_iter->next) {
        comp_handle = (OBJ_HANDLE)comp_list_iter->data;

        (void)memset_s(comp_dump_name, MAX_RSC_NAME_LEN, 0, MAX_RSC_NAME_LEN);
        (void)memset_s(comp_uri, MAX_URI_LENGTH, 0, MAX_URI_LENGTH);

        ret = rf_gen_component_uri(comp_handle, comp_uri, MAX_URI_LENGTH);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_DEBUG, "dump component uri info for handle [%s] failed", dfl_get_object_name(comp_handle)));

        ret = rf_validate_rsc_exist(comp_uri, NULL);
        
        do_val_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "validate component uri %s failed", comp_uri));
        

        ret = get_location_devicename(comp_handle, comp_dump_name, MAX_RSC_NAME_LEN);
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR,
            "get location and devicename for handle [%s] failed", dfl_get_object_name(comp_handle)));

        json_object_object_add(component_dump_jso, (const gchar *)comp_dump_name,
            json_object_new_string((const gchar *)comp_uri));
    }
    g_slist_free(component_list);

    ret = json_object_to_file_ext((const gchar *)dump_filename, component_dump_jso,
        JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_SPACED);
    (void)json_object_put(component_dump_jso);

    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "dump component uri jso to path failed, ret is %d", ret));

    
    (void)chmod(dump_filename, (S_IRUSR | S_IRGRP));

    return VOS_OK;
}

gint32 redfish_reboot_request_process(REBOOT_CTRL ctrl)
{
    if (ctrl == REBOOT_PREPARE) {
        
        dal_wait_encrypted_property_rollback(&g_rf_encrypted_data_update_state, &g_rf_encrypted_data_rollback_done);
        
        // 避免版本回退可能导致的问题，需要将RECORD_FILE文件权限回退回operater属组
        proxy_revert_file_mode_on_exit(RECORD_FILE, APACHE_UID, OPERATOR_GID);
        return VOS_OK;
    }

    
    if (ctrl == REBOOT_FORCE || ctrl == REBOOT_PERFORM) {
        
        xmlCleanupParser();
        (void)per_exit();
        return VOS_OK;
    }

    return_val_if_expr(REBOOT_CANCEL == ctrl, VOS_OK);

    return VOS_OK;
}


static gint32 redfish_sp_schema_copy_file(const gchar *file_name)
{
    json_object *u_obj = NULL;
    gsize write_length;
    gchar bmc_file_name[MAX_PATH_LEN] = {0};
    gchar path[MAX_PATH_LEN] = {0};
    guint32 bmc_file_length;
    gchar *bmc_file_data = NULL;
    json_object *json_obj = NULL;
    const gchar *json_data_s = NULL;

    json_object *u_json_obj_definitions = NULL;

    
    gint32 ret = redfish_sp_schema_get_bmc_file(file_name, bmc_file_name, MAX_PATH_LEN);
    
    return_val_do_info_if_expr(VOS_OK != ret, VOS_OK,
        debug_log(DLOG_MASS, "%s, %d:  file_name is %s", __FUNCTION__, __LINE__, file_name));

    
    (void)rf_get_sp_json_file(UMS_TRANS_FILE_ID_SP_SCHEMA, file_name, &u_obj);
    return_val_do_info_if_expr(NULL == u_obj, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:  u_obj is null", __FUNCTION__, __LINE__));

    
    ret = snprintf_s(path, MAX_PATH_LEN, MAX_PATH_LEN - 1, "%s/%s", CONFG_FILE_SCHEMASTORE_EN, bmc_file_name);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret));

    bmc_file_length = vos_get_file_length((const gchar *)path);
    bmc_file_data = (gchar *)g_malloc0(bmc_file_length + 1);
    return_val_do_info_if_expr(NULL == bmc_file_data, VOS_ERR, json_object_put(u_obj);
        debug_log(DLOG_ERROR, "%s, %d:  file_data is null", __FUNCTION__, __LINE__));

    FILE *fp = dal_fopen_check_realpath(path, "rb", path);
    return_val_do_info_if_expr(NULL == fp, VOS_ERR, g_free(bmc_file_data); bmc_file_data = NULL; json_object_put(u_obj);
        debug_log(DLOG_ERROR, "%s, %d:  file_data is null", __FUNCTION__, __LINE__));

    ret = fread(bmc_file_data, 1, bmc_file_length, fp);

    do_val_if_expr(bmc_file_length != ret, debug_log(DLOG_ERROR, "%s, %d:  debug :%s  len %d ret:%d >>>>>>>1 ",
        __FUNCTION__, __LINE__, bmc_file_data, bmc_file_length, ret););

    json_obj = json_tokener_parse((const gchar *)bmc_file_data);
    g_free(bmc_file_data);
    bmc_file_data = NULL;
    (void)fclose(fp);
    fp = NULL;

    do_val_if_expr(NULL == json_obj, goto EXIT_ERR);

    
    ret = json_object_object_get_ex(u_obj, SP_SCHEMA_FILE_DEEFINITIONS, &u_json_obj_definitions);
    do_val_if_expr(TRUE != ret, goto EXIT_ERR);

    (void)json_object_object_add(json_obj, SP_SCHEMA_FILE_DEEFINITIONS, json_object_get(u_json_obj_definitions));

    
    json_data_s = dal_json_object_get_str(json_obj);
    if (json_data_s == NULL) {
        goto EXIT_ERR;
    }
    fp = dal_fopen_check_realpath(path, "wb+", path);
    do_val_if_expr(NULL == fp, goto EXIT_ERR);

    
    write_length = fwrite(json_data_s, strlen(json_data_s), 1, fp);
    do_if_fail(write_length == 1, debug_log(DLOG_ERROR, "%s, %d:fwrite fail", __FUNCTION__, __LINE__));
    
    (void)fclose(fp);
    fp = NULL;

    json_object_put(u_obj);
    json_object_put(json_obj);

    debug_log(DLOG_DEBUG, "%s, %d:  debug :  %s copy ok ", __FUNCTION__, __LINE__, bmc_file_name);
    return VOS_OK;

EXIT_ERR:

    do_val_if_expr(NULL != fp, (void)fclose(fp); fp = NULL);
    // json_object_put 里有判空
    json_object_put(u_obj);
    json_object_put(json_obj);
    return VOS_ERR;
}


static gint32 redfish_sp_schema_copy(void)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *file_data = NULL;
    gchar **split_array = NULL;
    gint32 length = 0;
    gint32 index_id = 0;

    ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle  fail", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_byte(UMS_TRANS_FILE_ID_SP_SCHEMA));

    
    ret = uip_call_class_method_redfish(REDFISH_REQ_FROM_OTHER_CLIENT, "", "", obj_handle, SP_UMS_CLASS_NAME,
        UMS_METHOD_GET_FILE_LIST, AUTH_DISABLE, USERROLE_BASICSETTING, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    

    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:  ret : %d  fail", __FUNCTION__, __LINE__, ret));

    file_data = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    return_val_do_info_if_expr(NULL == file_data, VOS_ERR, uip_free_gvariant_list(output_list);
        debug_log(DLOG_ERROR, "%s, %d:  file_data is null", __FUNCTION__, __LINE__));

    split_array = g_strsplit(file_data, SP_FILE_SPLITE_FLAG, 0);
    if (NULL != split_array) {
        length = g_strv_length(split_array);

        for (index_id = 0; index_id < length; index_id++) {
            if (strlen(split_array[index_id]) > strlen(".json") &&
                TRUE != g_str_has_suffix(split_array[index_id], "Collection.json")) {
                ret = redfish_sp_schema_copy_file(split_array[index_id]) == VOS_OK ? ret : VOS_ERR;
            }
        }

        g_strfreev(split_array);
    }

    uip_free_gvariant_list(output_list);

    return ret;
}


static gint32 redfish_copy_sp_cfg_file(void)
{
    gint32 ret = 0;
    gint32 count = 0;

    (void)prctl(PR_SET_NAME, (uintptr_t) "rfCopySpCfg");
    return_val_if_expr(rf_support_sp_service() != TRUE, VOS_OK);

    while (count < 10) {
        ret = redfish_sp_schema_copy();
        if (ret == VOS_OK) {
            break;
        } else {
            count++;
            vos_task_delay(1000 * 10);
        }
    }

    return ret;
}


static void redfish_cfg_sp_file_init(void *data)
{
#define CFG_FILE_DECOMPRESSED 1
    gint32 ret;
    guchar decompressed = 0;
    OBJ_HANDLE obj_handle = 0;

    ret = dfl_get_object_handle(OBJECT_REDFISH_NAME, &obj_handle);
    return_do_info_if_expr(ret != VOS_OK,
        debug_log(DLOG_ERROR, "%s: get Redfish object failed, ret = %d", __FUNCTION__, ret));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_REDFISH_DECOMPRESSEWD_CFG_FILE, &decompressed);
    return_do_info_if_expr(ret != VOS_OK,
        debug_log(DLOG_ERROR, "%s: get decompressed failed, ret = %d", __FUNCTION__, ret));

    return_if_expr(decompressed == CFG_FILE_DECOMPRESSED);

    ret = redfish_copy_sp_cfg_file();
    return_do_info_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s: sp schema copy fail", __FUNCTION__));

    decompressed = CFG_FILE_DECOMPRESSED;
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_REDFISH_DECOMPRESSEWD_CFG_FILE, decompressed, DF_NONE);
    do_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s: set decompressed failed, ret = %d", __FUNCTION__, ret));

    return;
}


LOCAL void redfish_cfg_file_init(void)
{
    guchar decompressed = 0;
    OBJ_HANDLE obj_handle = 0;
 
    (void)dfl_get_object_handle(OBJECT_REDFISH_NAME, &obj_handle);
    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_REDFISH_DECOMPRESSEWD_CFG_FILE, &decompressed);
    if (ret == DFL_OK && decompressed == CFG_FILE_DECOMPRESSED) {
        return;
    }

    
    backup_event_shield_resources_conf();

    
    if (vos_get_file_accessible(CONFG_FILE_PATH_OLD) == TRUE) {
        (void)vos_rm_filepath(CONFG_FILE_PATH_OLD);
    }
    
    if (vos_get_file_accessible(CONFG_FILE_PATH) == TRUE) {
        (void)vos_rm_filepath(CONFG_FILE_PATH);
    }

    ret = tar_unzip((const guchar *)CONFG_FILE_FLASH, (const guchar *)CONFG_FILE_PREFIX);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: tar %s to %s fail", __FUNCTION__, CONFG_FILE_FLASH, CONFG_FILE_PREFIX);
        return;
    }

    
    restore_event_shield_resources_conf();

    return;
}


LOCAL void _encrypt_old_event_subscription_header(void)
{
    gint32 ret;
    GSList *evt_subscription_handle_list = NULL;
    GSList *list_iter = NULL;
    OBJ_HANDLE list_iter_handle;
    gchar               old_http_headers[MAX_EVT_HEADERS_SIZE] = {0};
    GVariant *encrypted_data_var = NULL;
    const gchar *obj_handle_name = NULL;

    ret = dfl_get_object_list(CLASS_EVT_SUBSCRIPTION, &evt_subscription_handle_list);
    return_if_fail(VOS_OK == ret);

    for (list_iter = evt_subscription_handle_list; NULL != list_iter; list_iter = list_iter->next) {
        list_iter_handle = (OBJ_HANDLE)list_iter->data;

        obj_handle_name = dfl_get_object_name(list_iter_handle);

        ret = dal_get_property_value_string(list_iter_handle, PROPERTY_EVT_SUBSCRIPTION_HTTP_HEADERS, old_http_headers,
            MAX_EVT_HEADERS_SIZE);
        continue_if_fail((VOS_OK == ret) && ('\0' != old_http_headers[0]));

        ret = rf_get_property_encrypted_data(old_http_headers, &encrypted_data_var);
        clear_sensitive_info(old_http_headers, MAX_EVT_HEADERS_SIZE);

        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s:get property encrypted data failed, ret is %d", __FUNCTION__, ret));

        debug_log(DLOG_INFO, "encrypt http headers for [%s]", obj_handle_name);

        
        ret = dfl_set_property_value(list_iter_handle, PROPERTY_EVT_SUBSCRIPTION_ENCRYPTED_HTTP_HEADERS,
            encrypted_data_var, DF_SAVE);
        g_variant_unref(encrypted_data_var);

        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s:set property value for [%s:%s] failed, ret is %d", __FUNCTION__, obj_handle_name,
            PROPERTY_EVT_SUBSCRIPTION_ENCRYPTED_HTTP_HEADERS, ret));

        
        ret = dal_set_property_value_string(list_iter_handle, PROPERTY_EVT_SUBSCRIPTION_HTTP_HEADERS,
            (const gchar *)old_http_headers, DF_SAVE);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s:set property value for [%s:%s] failed, ret is %d", __FUNCTION__, obj_handle_name,
            PROPERTY_EVT_SUBSCRIPTION_HTTP_HEADERS, ret));
    }
    g_slist_free(evt_subscription_handle_list);

    return;
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


LOCAL void init_pme_mmap_g_mem(void)
{
    OBJ_HANDLE bmc_obj;
    gint32 ret = dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &bmc_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get BMC object failed.(ret=%d)", ret);
        return;
    }

    guint8 cpld_ver;
    (void)dal_get_property_value_byte(bmc_obj, PROPERTY_BMC_CPLD_VER_REG, &cpld_ver);
}


gint32 redfish_init(void)
{
    
    #ifdef ENABLE_ASAN
        if (signal(44, mem_leak_handler) == SIG_ERR) {  // 44 表示执行ASAN检测的信号
            debug_log(DLOG_ERROR, "Could not set signal handler");
        }
    #endif

    init_gcov_signal_for_test(); 
    guint8 software_type = 0;
    TASKID refresh_mk_mask = 0;
    TASKID agent_load_task = 0;

    
    
    mallopt(M_ARENA_MAX, 1);
    
    mallopt(M_TRIM_THRESHOLD, 4 * 1024);

    redfish_cfg_file_init();
    redfish_http_init();

    redfish_task_init();

    
    g_mutex_init(&g_rf_encrypted_data_mutex);
    g_rf_encrypted_data_rollback_done = 0;

    _encrypt_old_event_subscription_header();
    

    
    gint32 ret = redfish_event_init();
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "redfish event init failed, ret is %d", ret);
    }
    

    
    
    redfish_message_init();
    

    
    redfish_system_lockdown_init();

    (void)dal_get_software_type(&software_type);
    if (MGMT_SOFTWARE_TYPE_EM == software_type) { // 管理板初始化透传数据结构
        redfish_forward_init();
    }
    
    if (!dal_check_if_mm_board()) {
        KmcKsfName *ksf_name = (KmcKsfName *)g_malloc0(sizeof(KmcKsfName));
        if (ksf_name == NULL) {
            return VOS_ERR;
        }

        ksf_name->keyStoreFile[0] = (gchar *)KEYSTORE_REDFISH;
        ksf_name->keyStoreFile[1] = (gchar *)KEYSTORE_REDFISH_BKP;

        ret = vos_task_create(&agent_load_task, "kmc_agent_load_task", (TASK_ENTRY)kmc_agent_load_task,
            (void *)ksf_name, DEFAULT_PRIORITY);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "create kmc_agent_load_task failed, ret = %d", ret);
            g_free(ksf_name);
            return ret;
        }

        
        ret = vos_task_create(&refresh_mk_mask, "refresh_mk_mask", (TASK_ENTRY)kmc_refresh_mk_mask, NULL,
            DEFAULT_PRIORITY);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "create refresh_mk_mask task failed, ret = %d", ret);
        }
    }

    return ret;
}


gint32 redfish_start(void)
{
    gulong task_id = 0;
    gulong rpc_test_task_id = 0;

    
    xmlInitParser();

    
    init_pme_mmap_g_mem();

    // redfish降权限后，文件版本兼容性处理(版本升级，老版本root权限执行，并且持久化文件已存在)
    // data/opt/pme/save/sposinstallpara
    if (TRUE == vos_get_file_accessible(SP_OSINSTALL_PARA_PATH)) {
        (void)chown(SP_OSINSTALL_PARA_PATH, REDFISH_USER_UID, ROOT_USER_GID);
    }

    if (vos_get_file_accessible(RECORD_FILE) == TRUE) {
        
        (void)chown(RECORD_FILE, APACHE_UID, APPS_USER_GID);
        (void)chmod(RECORD_FILE, 0664);
    }

    // 监听Redfish模块的RPC服务是否已启动(RPC服务启动后降权限)
    (void)vos_task_create(&rpc_test_task_id, "redfish rpc test thread", (TASK_ENTRY)redfish_rpc_test_task, NULL,
        DEFAULT_PRIORITY);

    rf_init_rsc_provider_tree_info();

    
    (void)redfish_event_start();

    (void)vos_task_create(&task_id, "redfish sp cfg init thread", (TASK_ENTRY)redfish_cfg_sp_file_init, NULL,
        DEFAULT_PRIORITY);
    

    return VOS_OK;
}


gint32 method_copy_sp_schema(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gulong task_id = 0;

    (void)vos_task_create(&task_id, "redfish copy sp cfg thread", (TASK_ENTRY)redfish_copy_sp_cfg_file, NULL,
        DEFAULT_PRIORITY);

    return VOS_OK;
}


gint32 redfish_dump_info(const gchar *path)
{
    gint32 ret;

    if (path == NULL || strlen(path) >= MAX_CMD_LENGTH) {
        debug_log(DLOG_ERROR, "%s: path is incorrect", __FUNCTION__);
        return RET_OK;
    }

    
    if (strstr(path, "&") || strstr(path, "||") || strstr(path, ";")) {
        debug_log(DLOG_ERROR, "%s: path is wrong", __FUNCTION__);
        return RET_OK;
    }

    
    ret = _rf_dump_component_info(path);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "redfish dump component uri info failed");
    }

    
    ret = rf_dump_evt_sender_info(path);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "redfish dump event sender info failed");
    }

    return RET_OK;
}


gint32 redfish_add_object_callback(const gchar *class_name, OBJ_HANDLE object_handle)
{
    return VOS_OK;
}


gint32 redfish_del_object_callback(const gchar *class_name, OBJ_HANDLE object_handle)
{
    return VOS_OK;
}


gint32 redfish_add_complete_callback(void)
{
    return VOS_OK;
}


gint32 redfish_del_complete_callback(void)
{
    return VOS_OK;
}


LOCAL void kmc_update_sp_file(const gchar *file_path, guint8 encrypt_type)
{
    
#define SP_CACHEDATA_MAX_ITEM_NUM (SP_CACHEDATA_MAX_LENGTH / (ENCRYPTED_DATA_MAX_LEN + 1))

    FILE *fp = NULL;
    gint32 fd;
    gchar old_ciphertext[MAX_ENCRYPTO_DATA_LEN] = {0};
    gsize item_count = 0;
    gsize i;
    gchar* new_ciphertext[SP_CACHEDATA_MAX_ITEM_NUM] = {NULL};
    gsize new_ciphertext_len;
    gint32 ret;

    return_do_info_if_expr(file_path == NULL, debug_log(DLOG_ERROR, "%s: invalid input.", __FUNCTION__));

    
    return_if_expr(vos_get_file_accessible(file_path) == FALSE);

    fp = g_fopen(file_path, "r+");
    return_do_info_if_expr(fp == NULL, debug_log(DLOG_ERROR, "%s: open sp %s file failed.", __FUNCTION__, file_path));

    fd = fileno(fp);
    (void)flock(fd, LOCK_EX);

    
    while (vos_fgets_s(old_ciphertext, MAX_ENCRYPTO_DATA_LEN, fp) != NULL && item_count < SP_CACHEDATA_MAX_ITEM_NUM) {
        new_ciphertext[item_count] = (gchar *)g_malloc0(MAX_ENCRYPTO_DATA_LEN + 1);
        goto_label_do_info_if_expr(new_ciphertext[item_count] == NULL, exit,
            debug_log(DLOG_ERROR, "%s: g_malloc0 failed.", __FUNCTION__));

        ret = kmc_update_ciphertext(DOMAIN_ID_RD_EVT_SUBSCRIPTION, old_ciphertext, new_ciphertext[item_count],
            MAX_ENCRYPTO_DATA_LEN + 1, &new_ciphertext_len, encrypt_type);
        goto_label_do_info_if_expr(ret != VOS_OK, exit, g_free(new_ciphertext[item_count]);
            new_ciphertext[item_count] = NULL;
            debug_log(DLOG_ERROR, "%s: update sp failed.", __FUNCTION__));

        clear_sensitive_info(old_ciphertext, sizeof(old_ciphertext));
        item_count++;
    }

    
    ret = ftruncate(fd, 0);
    goto_label_do_info_if_expr(ret != 0, exit,
        debug_log(DLOG_ERROR, "%s: ftruncate sp failed.", __FUNCTION__));

    
    ret = fseek(fp, 0, SEEK_SET);
    goto_label_do_info_if_expr(ret != 0, exit,
        debug_log(DLOG_ERROR, "%s: fseek sp failed.", __FUNCTION__));

    
    for (i = 0; i < item_count; i++) {
        ret = fwrite(new_ciphertext[i], strlen(new_ciphertext[i]), 1, fp);
        goto_label_do_info_if_expr(ret != 1, exit,
            debug_log(DLOG_ERROR, "%s: write sp file failed.", __FUNCTION__));

        ret = fwrite("\n", 1, 1, fp);
        goto_label_do_info_if_expr(ret != 1, exit,
            debug_log(DLOG_ERROR, "%s: write sp file failed.", __FUNCTION__));
    }

exit:
    (void)flock(fd, LOCK_UN);
    (void)fclose(fp);

    
    i = 0;
    while (i < item_count && i < SP_CACHEDATA_MAX_ITEM_NUM && new_ciphertext[i] != NULL) {
        clear_sensitive_info(new_ciphertext[i], MAX_ENCRYPTO_DATA_LEN + 1);
        g_free(new_ciphertext[i]);
        i++;
    }

    clear_sensitive_info(old_ciphertext, sizeof(old_ciphertext));
    return;
}


LOCAL void kmc_update_sp_files(guint8 encrypt_type)
{
    kmc_update_sp_file(SP_OSINSTALL_PARA_PATH, encrypt_type);
    return;
}


gint32 kmc_update_redfish_ciphertext(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret;
    GSList *rf_subscription_handle_list = NULL;
    GSList *handle_iter = NULL;
    OBJ_HANDLE subscription_handle;
    guint32 new_key_id;
    OBJ_HANDLE redfish_obj = 0;
    KmcKsfName ksf_name;

    g_mutex_lock(&g_rf_encrypted_data_mutex);

    return_val_do_info_if_expr(g_rf_encrypted_data_rollback_done == TRUE, VOS_OK,
        g_mutex_unlock(&g_rf_encrypted_data_mutex));

    g_rf_encrypted_data_update_state = 0;

    new_key_id = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 0));

    ret = dfl_get_object_handle(OBJECT_REDFISH_NAME, &redfish_obj);
    goto_label_do_info_if_expr(ret != VOS_OK, exit, debug_log(DLOG_ERROR, "Get Redfish object failed, ret = %d", ret));

    if (dal_check_mk_update(redfish_obj, PROPERTY_REDFISH_MASTERKEY_ID, new_key_id) == FALSE) {
        goto exit;
    }

    if (kmc_check_load_status() == FALSE) {
        debug_log(DLOG_ERROR, "%s: KMC not load.", __FUNCTION__);
        ret = VOS_ERR;
        goto exit;
    }

    ksf_name.keyStoreFile[0] = (gchar *)KEYSTORE_REDFISH;
    ksf_name.keyStoreFile[1] = (gchar *)KEYSTORE_REDFISH_BKP;
    
    ret = kmc_reset(&ksf_name);
    goto_label_do_info_if_expr(ret != WSEC_SUCCESS, exit, ret = VOS_ERR;
        debug_log(DLOG_ERROR, "KMC reset failed, ret = %d", ret));

    
    (void)dfl_get_object_list(CLASS_EVT_SUBSCRIPTION, &rf_subscription_handle_list);

    for (handle_iter = rf_subscription_handle_list; handle_iter != NULL; handle_iter = handle_iter->next) {
        subscription_handle = (OBJ_HANDLE)handle_iter->data;

        
        ret = kmc_update_redfish_prop(subscription_handle, PROPERTY_EVT_SUBSCRIPTION_ENCRYPTED_HTTP_HEADERS,
            ENCRYPT_TYPE_KMC);
        continue_do_info_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "Update %s %s failed, ret = %d",
            dfl_get_object_name(subscription_handle), PROPERTY_EVT_SUBSCRIPTION_ENCRYPTED_HTTP_HEADERS, ret));
    }

    
    kmc_update_sp_files(ENCRYPT_TYPE_KMC);

    
    ret = dal_save_property_value_uint32_remote_with_retry(redfish_obj, PROPERTY_REDFISH_MASTERKEY_ID, new_key_id,
        DF_SAVE, MAX_SET_PROP_TRY_COUNT);
    goto_label_do_info_if_expr(ret != VOS_OK, exit, debug_log(DLOG_ERROR, "Set masterkey id failed, ret = %d", ret));

exit:
    g_rf_encrypted_data_update_state = 1;
    g_mutex_unlock(&g_rf_encrypted_data_mutex);

    do_val_if_expr(rf_subscription_handle_list != NULL, g_slist_free(rf_subscription_handle_list));
    return ret;
}


LOCAL gint32 old_update_encrypted_data(const gchar *new_workkey_buf)
{
    gint32 ret;
    guint8              old_component[COMPONENT_SIZE] = {0};
    guint8              new_component[COMPONENT_SIZE] = {0};
    gchar   old_workkey_buf[ENCRYPT_WORKKEY_STR_SIZE] = {0};
    GSList *rf_subscription_handle_list = NULL;
    GSList *handle_iter = NULL;
    OBJ_HANDLE subscription_handle;

    
    ret = dal_get_encryptkey_workkey(OBJECT_REDFISH_NAME, PROPERTY_REDFISH_WORKKEY, old_workkey_buf,
        sizeof(old_workkey_buf));
    goto_label_do_info_if_fail(ret == VOS_OK, __ERROR_PROC,
        debug_log(DLOG_ERROR, "get old workkey failed, ret is %d", ret));

    
    ret = dal_get_encryptkey_component(OBJECT_REDFISH_NAME, PROPERTY_REDFISH_ENCRYPTO_COMPONENT, old_component,
        COMPONENT_SIZE);
    goto_label_do_info_if_fail(ret == VOS_OK, __ERROR_PROC,
        debug_log(DLOG_ERROR, "get old component failed, ret is %d", ret));

    
    ret = dal_get_encryptkey_component(OBJ_ENCRYPTKEY_REDFISH, PROPERTY_ENCRYPTKEY_COMPONENT, new_component,
        COMPONENT_SIZE);
    goto_label_do_info_if_fail(ret == VOS_OK, __ERROR_PROC,
        debug_log(DLOG_ERROR, "Get new component failed, ret = %d", ret));

    
    if (memcmp(new_component, (const void *)old_component, COMPONENT_SIZE) != 0) {
        ret = dal_set_encryptkey_component(OBJECT_REDFISH_NAME, PROPERTY_REDFISH_ENCRYPTO_COMPONENT, new_component,
            COMPONENT_SIZE);
        goto_label_do_info_if_fail(ret == VOS_OK, __ERROR_PROC,
            debug_log(DLOG_ERROR, "update redfish encrypto component failed, ret is %d", ret));
    }

    
    if (memcmp(new_workkey_buf, (const void *)old_workkey_buf, ENCRYPT_WORKKEY_STR_SIZE) != 0) {
        ret = dal_set_encryptkey_workkey(OBJECT_REDFISH_NAME, PROPERTY_REDFISH_WORKKEY, new_workkey_buf);
        goto_label_do_info_if_fail(ret == VOS_OK, __ERROR_PROC,
            debug_log(DLOG_ERROR, "update redfish encrypto workkey failed, ret is %d", ret));
    }

    
    (void)dfl_get_object_list(CLASS_EVT_SUBSCRIPTION, &rf_subscription_handle_list);

    for (handle_iter = rf_subscription_handle_list; handle_iter != NULL; handle_iter = handle_iter->next) {
        subscription_handle = (OBJ_HANDLE)handle_iter->data;

        ret = dal_update_encrypt_data(subscription_handle, PROPERTY_EVT_SUBSCRIPTION_ENCRYPTED_HTTP_HEADERS,
            old_component, sizeof(old_component), new_component, sizeof(new_component), new_workkey_buf);
        continue_do_info_if_fail(ret == VOS_OK,
            debug_log(DLOG_ERROR, "update encrypted property data for [%s:%s] failed, ret is %d",
            dfl_get_object_name(subscription_handle), PROPERTY_EVT_SUBSCRIPTION_ENCRYPTED_HTTP_HEADERS, ret));
    }
    g_slist_free(rf_subscription_handle_list);

__ERROR_PROC:

    clear_sensitive_info(old_workkey_buf, sizeof(old_workkey_buf));
    clear_sensitive_info(old_component, sizeof(old_component));
    clear_sensitive_info(new_component, sizeof(new_component));

    return ret;
}


gint32 update_redfish_encrypted_data(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    errno_t safe_fun_ret;
    gint32 ret;
    gchar               new_workkey_buf[ENCRYPT_WORKKEY_STR_SIZE] = {0};
    const gchar *new_workkey = NULL;
    const gchar *object_name = NULL;

    
    return_val_if_expr(dal_check_if_mm_board() == FALSE, VOS_OK);

    object_name = dfl_get_object_name(object_handle);
    return_val_do_info_if_expr(object_name == NULL, VOS_ERR, debug_log(DLOG_ERROR, "dfl_get_object_name failed.\n"));

    return_val_if_expr(strcmp(OBJ_ENCRYPTKEY_REDFISH, object_name), VOS_OK);

    g_mutex_lock(&g_rf_encrypted_data_mutex);

    if (g_rf_encrypted_data_rollback_done == TRUE) {
        g_mutex_unlock(&g_rf_encrypted_data_mutex);
        return VOS_OK;
    }

    
    g_rf_encrypted_data_update_state = 0;

    
    new_workkey = g_variant_get_string(property_value, 0);
    
    safe_fun_ret = strncpy_s(new_workkey_buf, sizeof(new_workkey_buf), new_workkey, ENCRYPT_WORKKEY_STR_SIZE - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    ret = old_update_encrypted_data(new_workkey_buf);
    do_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "Update encrypted data failed"));

    g_rf_encrypted_data_update_state = 1;
    g_mutex_unlock(&g_rf_encrypted_data_mutex);

    clear_sensitive_info(new_workkey_buf, sizeof(new_workkey_buf));
    return ret;
}


gint32 rollback_redfish_encrypted_data(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    gint32 ret = VOS_OK;
    guint8 rollback_flag;

    rollback_flag = g_variant_get_byte(property_value);
    if (rollback_flag == 0) {
        return VOS_OK;
    }

    g_mutex_lock(&g_rf_encrypted_data_mutex);

    if (g_rf_encrypted_data_rollback_done == TRUE) {
        g_mutex_unlock(&g_rf_encrypted_data_mutex);
        return VOS_OK;
    }

    
    if (dal_check_if_mm_board()) {
        g_rf_encrypted_data_rollback_done = TRUE;
        g_mutex_unlock(&g_rf_encrypted_data_mutex);
        return VOS_OK;
    }

#ifndef ARM64_HI1711_ENABLED
    OBJ_HANDLE redfish_obj = 0;
    GSList *rf_subscription_handle_list = NULL;
    GSList *handle_iter = NULL;
    OBJ_HANDLE subscription_handle;

    ret = dfl_get_object_handle(OBJECT_REDFISH_NAME, &redfish_obj);
    goto_label_do_info_if_expr(ret != VOS_OK, exit, debug_log(DLOG_ERROR, "Get Redfish object failed, ret = %d", ret));

    if (dal_check_if_masterkey_id_set(redfish_obj, PROPERTY_REDFISH_MASTERKEY_ID) == FALSE) {
        goto exit;
    }

    
    (void)dfl_get_object_list(CLASS_EVT_SUBSCRIPTION, &rf_subscription_handle_list);

    for (handle_iter = rf_subscription_handle_list; handle_iter != NULL; handle_iter = handle_iter->next) {
        subscription_handle = (OBJ_HANDLE)handle_iter->data;

        
        ret = kmc_update_redfish_prop(subscription_handle, PROPERTY_EVT_SUBSCRIPTION_ENCRYPTED_HTTP_HEADERS,
            ENCRYPT_TYPE_REMOTE);
        continue_do_info_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "Update %s %s failed, ret = %d",
            dfl_get_object_name(subscription_handle), PROPERTY_EVT_SUBSCRIPTION_ENCRYPTED_HTTP_HEADERS, ret));
    }

    do_val_if_expr(rf_subscription_handle_list != NULL, g_slist_free(rf_subscription_handle_list);
        rf_subscription_handle_list = NULL);

    
    kmc_update_sp_files(ENCRYPT_TYPE_REMOTE);

    
    kmc_sync_local_property(DOMAIN_ID_RD_EVT_SUBSCRIPTION, redfish_obj);
    goto_label_do_info_if_expr(ret != VOS_OK, exit, debug_log(DLOG_ERROR, "Sync property failed, ret = %d", ret));

exit:
#endif
    g_rf_encrypted_data_rollback_done = TRUE;
    g_mutex_unlock(&g_rf_encrypted_data_mutex);
    return ret;
}


gint32 redfish_data_sync(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    (void)property_sync_common_handler_ext(object_handle, caller_info, input_list, output_list, DF_SAVE);
    return VOS_OK;
}
