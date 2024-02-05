
#include "bios_pwd.h"

LOCAL GMutex g_bios_comm_mutex;
LOCAL guint8 g_bios_rollback_state = 0;
LOCAL guint8 g_bios_pwd_update_state = 1;

struct bios_pwd_param {
    gchar pwd_name[MAX_PROPERTY_VALUE_LEN];
    gchar old_pwd[MAX_PROPERTY_VALUE_LEN];
    gchar new_pwd[MAX_PROPERTY_VALUE_LEN];
};


typedef enum {
    BIOS_CHANGE_PWD_OK,
    BIOS_CHANGE_PWD_FAILED
} IPMI_BIOS_CHANGE_PWD_ECODE;

#pragma pack(1)

typedef struct {
    guint8 manu_id[MANUFAC_ID_LEN]; 
    guint8 sub_cmd;                 
    guint8 reserved1;
    guint8 selector;
    guint8 pwd_type;
    union {
        guint8 comp_code;
        guint8 reserved2;
    };
} BIOS_CHANGE_PWD_PARAM;

typedef struct {
    guint8 comp_code;
    guint8 manu_id[MANUFAC_ID_LEN]; 
} BIOS_CHANGE_PWD_RESP;

#pragma pack()

#define BIOS_PWD_RESP_BODY_LEN_MAX ((BIOS_PWD_LEN_MAX + 1) * 2)
#define BIOS_PWD_RESP_LEN_MAX (BIOS_PWD_RESP_BODY_LEN_MAX + sizeof(BIOS_CHANGE_PWD_RESP))

enum BIOS_CHANGE_PWD_SELECTOR {
    BIOS_CHANGE_PWD_SELECTOR_GET,
    BIOS_CHANGE_PWD_SELECTOR_ACK
};

#define BIOS_PWD_NAME_SUPERVISOR "Supervisor"
#define BIOS_PWD_NAME_ADMIN "AdminPassword" // 作用和Supervisor一致，为了兼容标准建议的“AdminPassword”
#define BIOS_PWD_NAME_USER "UserPassword"
enum BIOS_CHANGE_PWD_TYPE {
    BIOS_CHANGE_SUP_PWD_TYPE_SETUP,
    BIOS_CHANGE_USER_PWD_TYPE_SETUP,
    BIOS_CHANGE_PWD_TYPE_NUM
};

enum BIOS_CHANGE_PWD_TASK_STATUS {
    BIOS_CHANGE_PWD_TASK_IDLE,
    BIOS_CHANGE_PWD_TASK_WAIT_ACK 
};

typedef struct {
    const gchar *type_name;
    volatile guint32 status;
} BIOS_CHANGE_PWD_TASK;

LOCAL BIOS_CHANGE_PWD_TASK g_bios_change_pwd_task_info[] = {
    {BIOS_PWD_NAME_SUPERVISOR, BIOS_CHANGE_PWD_TASK_IDLE}, 
    {BIOS_PWD_NAME_USER, BIOS_CHANGE_PWD_TASK_IDLE}        
};

#define BIOS_PWD_ARRAY_SIZE 2
LOCAL const gchar *g_sup_pwd_prop_array[] = {
    PROPERTY_BIOS_SETUP_PWD_OLD,
    PROPERTY_BIOS_SETUP_PWD_NEW
};

LOCAL const gchar *g_user_pwd_prop_array[] = {
    PROPERTY_BIOS_USER_SETUP_PWD_OLD,
    PROPERTY_BIOS_USER_SETUP_PWD_NEW
};

#define ALL_BIOS_PWD_ARRAY_SIZE 4
LOCAL const gchar *g_all_pwd_prop_array[] = {
    PROPERTY_BIOS_SETUP_PWD_OLD,
    PROPERTY_BIOS_SETUP_PWD_NEW,
    PROPERTY_BIOS_USER_SETUP_PWD_OLD,
    PROPERTY_BIOS_USER_SETUP_PWD_NEW
};

LOCAL gint32 __bios_get_plain_password(OBJ_HANDLE bios, const gchar *prop, gchar *plain_pwd, gsize pwd_buff_size)
{
    gint32 ret;
    gchar encrypt_pwd[SMALL_BUFFER_SIZE] = {0};
    gsize plaintext_len = 0;
    ret = dal_get_property_value_string(bios, prop, encrypt_pwd, sizeof(encrypt_pwd));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get property %s failed.", __FUNCTION__, prop);
        return RET_ERR;
    }

    if (strlen(encrypt_pwd) == 0) { 
        debug_log(DLOG_DEBUG, "%s: pwd has not been set.", __FUNCTION__);
        return RET_OK;
    }

    ret = kmc_get_decrypted_data(DOMAIN_ID_BIOS, (const gchar *)encrypt_pwd, plain_pwd, pwd_buff_size, &plaintext_len);
    dal_clear_sensitive_info(encrypt_pwd, sizeof(encrypt_pwd));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: kmc_get_decrypted_data failed with ret:%d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL void __bios_clear_encrypt_password(guint8 pwd_type)
{
    gint32 ret;
    OBJ_HANDLE bios = 0;
    guint8 i;
    const char **pwd_prop_array = NULL;

    if (pwd_type == BIOS_CHANGE_SUP_PWD_TYPE_SETUP) {
        pwd_prop_array = g_sup_pwd_prop_array;
    } else if (pwd_type == BIOS_CHANGE_USER_PWD_TYPE_SETUP) {
        pwd_prop_array = g_user_pwd_prop_array;
    } else {
        debug_log(DLOG_ERROR, "%s: password type[%u] is not supported.", __FUNCTION__, pwd_type);
        return;
    }

    ret = dfl_get_object_handle(OBJ_NAME_BIOS, &bios);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get obj_handle for OBJ_NAME_BIOS failed", __FUNCTION__);
        return;
    }

    for (i = 0; i < BIOS_PWD_ARRAY_SIZE; i++) {
        ret = dal_set_property_value_string(bios, pwd_prop_array[i], "", DF_SAVE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: clear property %s failed.", __FUNCTION__, pwd_prop_array[i]);
        }
    }

    return;
}

LOCAL gint32 __bios_save_encrypt_password(OBJ_HANDLE bios, const gchar *prop, gchar *plain_pwd)
{
    gint32 ret;
    gchar ciphertext[MAX_CT_LEN] = {0};
    gsize ciphertext_len = 0;

    ret = kmc_get_encrypt_data_if_mm_board(DOMAIN_ID_BIOS, plain_pwd, ciphertext, sizeof(ciphertext), &ciphertext_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get encrypt BIOS password failed, ret = %d!", ret);
        return RET_ERR;
    }

    ret = dal_set_property_value_string(bios, prop, ciphertext, DF_SAVE);
    clear_sensitive_info(ciphertext, ciphertext_len);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 __get_bios_pwd_param(GSList *input_list, struct bios_pwd_param *pwd_param)
{
    GVariant *input_param = NULL;
    const gchar *param_val = NULL;
    gsize param_len = 0;
    gint8 param_idx = 0;
    errno_t sec_rv;

    
    input_param = (GVariant *)g_slist_nth_data(input_list, param_idx++);
    if (input_param == NULL) {
        debug_log(DLOG_ERROR, "%s:param 0 is NULL", __FUNCTION__);
        return RET_ERR;
    }
    param_val = g_variant_get_string(input_param, &param_len);
    sec_rv = strcpy_s(pwd_param->pwd_name, sizeof(pwd_param->pwd_name), param_val);
    if (sec_rv != EOK) {
        debug_log(DLOG_ERROR, "%s:strcpy_s password type failed with %d", __FUNCTION__, sec_rv);
        return RET_ERR;
    }

    
    input_param = (GVariant *)g_slist_nth_data(input_list, param_idx++);
    if (input_param == NULL) {
        debug_log(DLOG_ERROR, "%s:param 1 is NULL", __FUNCTION__);
        return RET_ERR;
    }
    param_val = g_variant_get_string(input_param, &param_len);
    sec_rv = strcpy_s(pwd_param->old_pwd, sizeof(pwd_param->old_pwd), param_val);
    if (sec_rv != EOK) {
        debug_log(DLOG_ERROR, "%s:strcpy_s old password failed with %d", __FUNCTION__, sec_rv);
        return RET_ERR;
    }

    
    input_param = (GVariant *)g_slist_nth_data(input_list, param_idx++);
    if (input_param == NULL) {
        debug_log(DLOG_ERROR, "%s:param 2 is NULL", __FUNCTION__);
        return RET_ERR;
    }
    param_val = g_variant_get_string(input_param, &param_len);
    sec_rv = strcpy_s(pwd_param->new_pwd, sizeof(pwd_param->new_pwd), param_val);
    if (sec_rv != EOK) {
        debug_log(DLOG_ERROR, "%s:strcpy_s new password failed with %d", __FUNCTION__, sec_rv);
        return RET_ERR;
    }

    return RET_OK;
}

gint32 method_bios_change_password(OBJ_HANDLE bios, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    struct bios_pwd_param pwd_param = { 0 };

    ret = __get_bios_pwd_param(input_list, &pwd_param);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get param failed.", __FUNCTION__);
        goto OUT;
    }

    if (strlen(pwd_param.old_pwd) > BIOS_PWD_LEN_MAX || strlen(pwd_param.new_pwd) > BIOS_PWD_LEN_MAX) {
        debug_log(DLOG_ERROR, "%s: password len is invalid", __FUNCTION__);
        ret = METHOD_BIOS_CHANGE_PWD_LEN_INVALID;
        goto OUT;
    }

    if (strcmp(pwd_param.pwd_name, BIOS_PWD_NAME_SUPERVISOR) == 0 ||
        strcmp(pwd_param.pwd_name, BIOS_PWD_NAME_ADMIN) == 0) {
        ret = __bios_save_encrypt_password(bios, PROPERTY_BIOS_SETUP_PWD_OLD, (gchar *)pwd_param.old_pwd);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: save Supervisor old password failed.", __FUNCTION__);
            goto OUT;
        }

        ret = __bios_save_encrypt_password(bios, PROPERTY_BIOS_SETUP_PWD_NEW, (gchar *)pwd_param.new_pwd);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: save Supervisor new password failed.", __FUNCTION__);
        }
    } else if (strcmp(pwd_param.pwd_name, BIOS_PWD_NAME_USER) == 0) {
        ret = __bios_save_encrypt_password(bios, PROPERTY_BIOS_USER_SETUP_PWD_OLD, (gchar *)pwd_param.old_pwd);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: save User old password failed.", __FUNCTION__);
            goto OUT;
        }
        ret = __bios_save_encrypt_password(bios, PROPERTY_BIOS_USER_SETUP_PWD_NEW, (gchar *)pwd_param.new_pwd);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: save User new password failed.", __FUNCTION__);
        }
    } else {
        debug_log(DLOG_ERROR, "%s: unsupported password %s", __FUNCTION__, pwd_param.pwd_name);
        ret = METHOD_BIOS_CHANGE_PWD_UNSUPPORTED;
    }

OUT:
    dal_clear_sensitive_info(pwd_param.old_pwd, sizeof(pwd_param.old_pwd));
    dal_clear_sensitive_info(pwd_param.new_pwd, sizeof(pwd_param.new_pwd));
    if (ret == RET_OK) {
        method_operation_log(caller_info, NULL, "The request for changing BIOS %s password is delivered",
            pwd_param.pwd_name);
    } else {
        method_operation_log(caller_info, NULL, "Failed to deliver the request for changing BIOS %s password",
            pwd_param.pwd_name);
    }
    return ret;
}


LOCAL gint32 __bios_change_pwd_resp_info_get(guint8 pwd_type, guint8 *pwd_info, gsize info_len_max)
{
    gint32 ret;
    OBJ_HANDLE bios = 0;
    errno_t sec_rv;
    gchar plain_pwd[MAX_PROPERTY_VALUE_LEN] = {0};
    gsize plain_pwd_len;
    guint8 info_idx = 0;
    guint8 i;
    const char **pwd_prop_array = NULL;

    if (pwd_type == BIOS_CHANGE_SUP_PWD_TYPE_SETUP) {
        pwd_prop_array = g_sup_pwd_prop_array;
    } else if (pwd_type == BIOS_CHANGE_USER_PWD_TYPE_SETUP) {
        pwd_prop_array = g_user_pwd_prop_array;
    } else {
        debug_log(DLOG_ERROR, "%s: password type[%u] is not supported.", __FUNCTION__, pwd_type);
        return -1;
    }

    ret = dfl_get_object_handle(OBJ_NAME_BIOS, &bios);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get obj_handle for OBJ_NAME_BIOS failed", __FUNCTION__);
        return -1;
    }

    for (i = 0; i < BIOS_PWD_ARRAY_SIZE; i++) {
        ret = __bios_get_plain_password(bios, pwd_prop_array[i], plain_pwd, sizeof(plain_pwd));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get %s failed", __FUNCTION__, pwd_prop_array[i]);
            dal_clear_sensitive_info(plain_pwd, sizeof(plain_pwd));
            return -1;
        }
        plain_pwd_len = strlen(plain_pwd);

        pwd_info[info_idx++] = plain_pwd_len;
        sec_rv = memcpy_s(&pwd_info[info_idx], info_len_max - info_idx, plain_pwd, plain_pwd_len);
        if (sec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s failed with %d", __FUNCTION__, sec_rv);
            dal_clear_sensitive_info(plain_pwd, sizeof(plain_pwd));
            return -1;
        }
        info_idx += plain_pwd_len;
    }

    dal_clear_sensitive_info(plain_pwd, sizeof(plain_pwd));
    return info_idx;
}

LOCAL void __bios_change_pwd_timeout_task(guint8 pwd_type)
{
    volatile BIOS_CHANGE_PWD_TASK *task_info = &g_bios_change_pwd_task_info[pwd_type];
    const gulong change_pwd_timeout_ms = 10000; 

    vos_task_delay(change_pwd_timeout_ms);

    __bios_clear_encrypt_password(pwd_type);

    if (task_info->status == BIOS_CHANGE_PWD_TASK_IDLE) {
        return;
    }

    debug_log(DLOG_ERROR, "%s: bios changes %s pwd timeout.", __FUNCTION__, task_info->type_name);
    task_info->status = BIOS_CHANGE_PWD_TASK_IDLE;
    return;
}

LOCAL gint32 __handle_bios_change_pwd_get(const void *req_msg, const BIOS_CHANGE_PWD_PARAM *param)
{
    gint32 ret;
    gulong task_id;
    BIOS_CHANGE_PWD_RESP *resp = NULL;
    gint32 body_len;
    guint8 pwd_type;
    volatile guint32 *task_status = &g_bios_change_pwd_task_info[param->pwd_type].status;

    if (*task_status != BIOS_CHANGE_PWD_TASK_IDLE) {
        debug_log(DLOG_ERROR, "%s: request is not supported in present state.", __FUNCTION__);
        return ipmi_send_simple_response(req_msg, COMP_CODE_STATUS_INVALID);
    }

    resp = (BIOS_CHANGE_PWD_RESP *)g_malloc0(BIOS_PWD_RESP_LEN_MAX);
    if (resp == NULL) {
        return ipmi_send_simple_response(req_msg, COMP_CODE_MEMOUT);
    }

    ret = dal_set_ipmi_resp_huawei_iana(resp->manu_id, sizeof(resp->manu_id));
    if (ret == FALSE) {
        debug_log(DLOG_ERROR, "%s: set manufacture ID failed.", __FUNCTION__);
    }

    body_len = __bios_change_pwd_resp_info_get(param->pwd_type, (guint8 *)resp + sizeof(BIOS_CHANGE_PWD_RESP),
        BIOS_PWD_RESP_BODY_LEN_MAX);
    if (body_len < 0) {
        dal_clear_sensitive_info(resp, BIOS_PWD_RESP_LEN_MAX);
        g_free(resp);
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }

    *task_status = BIOS_CHANGE_PWD_TASK_WAIT_ACK;
    pwd_type = param->pwd_type;
    ret = vos_task_create(&task_id, "bios_change_pwd_timeout", (TASK_ENTRY)__bios_change_pwd_timeout_task,
        INT8_TO_POINTER(pwd_type), 1);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: create bios_change_pwd_timeout task failed[%d].", __FUNCTION__, ret);
    }

    ret = ipmi_send_response(req_msg, sizeof(BIOS_CHANGE_PWD_RESP) + body_len, (guint8 *)resp);
    dal_clear_sensitive_info(resp, BIOS_PWD_RESP_LEN_MAX);
    g_free(resp);
    return ret;
}

LOCAL gint32 __handle_bios_change_pwd_ack(const void *req_msg, const BIOS_CHANGE_PWD_PARAM *param)
{
    gint32 ret;
    BIOS_CHANGE_PWD_RESP resp = { 0 };
    volatile BIOS_CHANGE_PWD_TASK *task_info = &g_bios_change_pwd_task_info[param->pwd_type];

    if (task_info->status != BIOS_CHANGE_PWD_TASK_WAIT_ACK) {
        debug_log(DLOG_ERROR, "%s: request is not supported in present state.", __FUNCTION__);
        return ipmi_send_simple_response(req_msg, COMP_CODE_STATUS_INVALID);
    }
    task_info->status = BIOS_CHANGE_PWD_TASK_IDLE;

    // 清除密码
    __bios_clear_encrypt_password(param->pwd_type);

    if (param->comp_code == BIOS_CHANGE_PWD_OK) {
        ipmi_operation_log(req_msg, "Change BIOS %s password successfully", task_info->type_name);
    } else {
        ipmi_operation_log(req_msg, "Change BIOS %s password failed", task_info->type_name);
    }

    ret = dal_set_ipmi_resp_huawei_iana(resp.manu_id, sizeof(resp.manu_id));
    if (ret == FALSE) {
        debug_log(DLOG_ERROR, "%s: set manufacture ID failed.", __FUNCTION__);
    }

    return ipmi_send_response(req_msg, sizeof(resp), (guint8 *)&resp);
}

LOCAL gboolean __ipmi_bios_change_pwd_ctx_check(const void *req_msg)
{
    gint32 ret;
    guint8 src_chan_num;
    OBJ_HANDLE bios = 0;
    guint8 bios_status = BIOS_STARTUP_STATE_OFF;

    src_chan_num = get_ipmi_chan_num(req_msg);
    if (src_chan_num != SYS_CHAN_NUM) {
        debug_log(DLOG_ERROR, "%s: channel[%d] is not supported.", __FUNCTION__, src_chan_num);
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_BIOS, 0, &bios);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get bios failed with %d", __FUNCTION__, ret);
        return FALSE;
    }

    ret = dal_get_property_value_byte(bios, PROPERTY_BIOS_STARTUP_STATE, &bios_status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get bios status failed with %d", __FUNCTION__, ret);
        return FALSE;
    }

    if (bios_status == BIOS_STARTUP_POST_STAGE_FINISH) {
        debug_log(DLOG_ERROR, "%s: bios status[%#x] is not supported.", __FUNCTION__, bios_status);
        return FALSE;
    }

    return TRUE;
}

gint32 ipmi_bios_change_pwd(const void *req_msg, gpointer user_data)
{
    gint32 ret;
    const BIOS_CHANGE_PWD_PARAM *req_param = NULL;

    if (req_msg == NULL) {
        return RET_ERR;
    }

    req_param = (const BIOS_CHANGE_PWD_PARAM *)get_ipmi_src_data(req_msg);
    if (req_param == NULL) {
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    if (__ipmi_bios_change_pwd_ctx_check(req_msg) != TRUE) {
        return ipmi_send_simple_response(req_msg, COMP_CODE_STATUS_INVALID);
    }

    ret = dal_check_ipmi_req_huawei_iana(req_param->manu_id, sizeof(req_param->manu_id));
    if (ret == FALSE) {
        debug_log(DLOG_ERROR, "%s: check request manufacture ID failed.", __FUNCTION__);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    if (req_param->pwd_type >= ARRAY_SIZE(g_bios_change_pwd_task_info)) {
        debug_log(DLOG_ERROR, "%s: password type[%u] is not supported.", __FUNCTION__, req_param->pwd_type);
        return ipmi_send_simple_response(req_msg, COMP_CODE_PARA_NOT_SUPPORT);
    }

    switch (req_param->selector) {
        case BIOS_CHANGE_PWD_SELECTOR_GET:
            ret = __handle_bios_change_pwd_get(req_msg, req_param);
            break;
        case BIOS_CHANGE_PWD_SELECTOR_ACK:
            ret = __handle_bios_change_pwd_ack(req_msg, req_param);
            break;
        default:
            debug_log(DLOG_ERROR, "%s: selector[%u] is out of range.", __FUNCTION__, req_param->selector);
            return ipmi_send_simple_response(req_msg, COMP_CODE_OUTOF_RANGE);
    }

    return ret;
}

void init_bios_rollback_mutex(void)
{
    g_mutex_init(&g_bios_comm_mutex);
}

gint32 roll_back_bios_pwd(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name, GVariant *property_value)
{
    gint32 ret;
    OBJ_HANDLE bios_handle = 0;
    guint8 rollback_flag;

    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &bios_handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    rollback_flag = g_variant_get_byte(property_value);
    if (rollback_flag == 0) {
        return RET_OK;
    }

    g_mutex_lock(&g_bios_comm_mutex);

    if (g_bios_rollback_state == 1) {
        g_mutex_unlock(&g_bios_comm_mutex);
        return RET_OK;
    }

    
    if (dal_check_if_mm_board()) {
        g_bios_rollback_state = 1;
        g_mutex_unlock(&g_bios_comm_mutex);
        return RET_OK;
    }

#ifndef ARM64_HI1711_ENABLED
    if (dal_check_if_masterkey_id_set(bios_handle, PROPERTY_BIOS_MASTERKEY_ID) == FALSE) {
        goto EXIT;
    }

    
    guint8 i;
    for (i = 0; i < ALL_BIOS_PWD_ARRAY_SIZE; i++) {
        ret = kmc_update_ciphertext_str_prop(DOMAIN_ID_BIOS, bios_handle, g_all_pwd_prop_array[i], ENCRYPT_TYPE_REMOTE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Update ct failed for %s, ret = %d", __FUNCTION__, g_all_pwd_prop_array[i], ret);
            goto EXIT;
        }
    }

    ret = dal_set_property_value_uint32(bios_handle, PROPERTY_BIOS_MASTERKEY_ID, 0, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Set masterkey id failed, ret = %d", __FUNCTION__, ret);
    }

EXIT:
#endif
    g_bios_rollback_state = 1;
    g_mutex_unlock(&g_bios_comm_mutex);
    return ret;
}

gint32 kmc_update_bios_ciphertext(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret;
    KmcKsfName ksf_name;
    OBJ_HANDLE bios_obj = 0;
    guint32 new_key_id;

    g_mutex_lock(&g_bios_comm_mutex);
    if (g_bios_rollback_state == 1) {
        g_mutex_unlock(&g_bios_comm_mutex);
        return RET_OK;
    }

    g_bios_pwd_update_state = 0;

    new_key_id = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 0));

    ret = dfl_get_object_handle(BIOS_CLASS_NAME, &bios_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get object handle failed, ret = %d", ret);
        goto EXIT;
    }

    if (dal_check_mk_update(bios_obj, PROPERTY_BIOS_MASTERKEY_ID, new_key_id) == FALSE) {
        goto EXIT;
    }

    if (kmc_check_load_status() == FALSE) {
        debug_log(DLOG_ERROR, "%s: KMC not load.", __FUNCTION__);
        ret = RET_ERR;
        goto EXIT;
    }

    kmc_get_ksf_actual_path(&ksf_name);
    ret = kmc_reset(&ksf_name);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "KMC reset failed, ret = %d", ret);
        goto EXIT;
    }

    
    guint8 i;
    for (i = 0; i < ALL_BIOS_PWD_ARRAY_SIZE; i++) {
        ret = kmc_update_ciphertext_str_prop(DOMAIN_ID_BIOS, bios_obj, g_all_pwd_prop_array[i], ENCRYPT_TYPE_KMC);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: kmc failed for (%s), ret = %d", __FUNCTION__, g_all_pwd_prop_array[i], ret);
            goto EXIT;
        }
    }

    ret = dal_set_property_value_uint32(bios_obj, PROPERTY_BIOS_MASTERKEY_ID, new_key_id, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set masterkey id failed, ret = %d", ret);
    }

EXIT:
    g_bios_pwd_update_state = 1;
    g_mutex_unlock(&g_bios_comm_mutex);
    return ret;
}

void wait_encrypted_biospwd_rollback(void)
{
    dal_wait_encrypted_property_rollback(&g_bios_pwd_update_state, &g_bios_rollback_state);
}