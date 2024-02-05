
#include "snmp_method.h"
#include "snmp_manage_app.h"
#include "keyGen.h"
#include "pme_app/kmc/kmc.h"
#include "web_manage.h"
#include "snmp_user_method.h"

SNMP_VERSION_S snmp_version_info[3] =
{
    {"V1",  PROPERTY_SNMP_V1STATE},
    {"V2C", PROPERTY_SNMP_V2CSTATE},
    {"V3",  PROPERTY_SNMP_V3STATUS}
};

const gchar* auth_protocol_str[] =
{
    "MD5",
    "SHA",
    "SHA", 
    "SHA256",
    "SHA384",
    "SHA512"
};

const gchar* priv_protocol_str[] =
{
    "None",
    "DES",
    "AES",
    "AES256"
};

guint8 g_snmpv3_status = SNMP_STATE_ENABLE;

LOCAL void snmp_sync_rw_community(guchar state, const gchar *key);


gint32 RebootSnmp(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret = 0;
    GVariant *value = NULL;
    guint8 snmp_state = 0;
    guint8 *option = NULL;

    ret = dfl_get_property_value(object_handle, PROPERTY_SNMP_STATE, &value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get snmp state failed!\n");
        return RET_ERR;
    }

    snmp_state = g_variant_get_byte(value);
    g_variant_unref(value);

    if (snmp_state == SNMP_STATE_DISABLE) {
        return RET_OK;
    }

    // 当队列中已有重启命令时，不再继续添加，防止不停重启
    if (g_rebootSnmp == TRUE) {
        debug_log(DLOG_DEBUG, "Reboot already in queue");
        return RET_OK;
    }

    option = (guint8 *)g_malloc0(sizeof(guint8));
    if (option == NULL) {
        debug_log(DLOG_ERROR, "Malloc failed!\n");
        return RET_ERR;
    }

    g_rebootSnmp = TRUE;
    *option = REBOOT_SNMP;
    ret = vos_queue_send(g_snmpSendQueue, option);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Send reboot snmp command failed!\n");
        g_free(option);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 snmp_get_encrypt_community(gchar *plain_comm, gchar *encrypt_comm, gsize encrypt_comm_size,
    gsize *encrypt_comm_len)
{
    gint32 ret;

    if (plain_comm == NULL || encrypt_comm == NULL || encrypt_comm_len == NULL || encrypt_comm_size == 0) {
        debug_log(DLOG_ERROR, "%s: input parm is null", __FUNCTION__);
        return RET_ERR;
    }

    
    ret = kmc_get_encrypt_data_if_mm_board(DOMAIN_ID_SNMP_COMM, plain_comm, encrypt_comm, encrypt_comm_size,
        encrypt_comm_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Encrypt comm failed, ret = %d", ret);
        return ret;
    }

    return RET_OK;
}


gint32 snmp_modify_community(gint32 type, const gchar *community)
{
    gint32 iRet = -1;
    gint32 ret = 0;
    FM_FILE_S *fp = NULL;
    gint32 p_start = 0;
    gint32 old_buf_len = 0;
    gint32 new_buf_len = 0;
    gchar key[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    OBJ_HANDLE obj_handle = 0;
    guchar state = 0;

    if (community == NULL) {
        return RET_ERR;
    }

    
    if (dal_get_object_handle_nth(CLASS_SNMP, 0, &obj_handle) != RET_OK) {
        debug_log(DLOG_ERROR, "Get SNMP handle failed!");
        return RET_ERR;
    }

    if (dal_get_property_value_byte(obj_handle, PROPERTY_SNMP_RW_COMMUNITY_STATE, &state) != RET_OK) {
        debug_log(DLOG_ERROR, "Get SNMP roCommunity failed!");
        return RET_ERR;
    }
    

    
    if (type == SNMP_ROCOMMIUNITY_IPV4) {
        
        (void)snprintf_s(key, sizeof(key), sizeof(key) - 1, "%s", COM2SEC_ROCOMM_DEFAULT);
        
    } else if (type == SNMP_RWCOMMIUNITY_IPV4) {
        
        if (state == DISABLED) {
            (void)snprintf_s(key, sizeof(key), sizeof(key) - 1, "#%s", COM2SEC_RWCOMM_DEFAULT);
        } else {
            (void)snprintf_s(key, sizeof(key), sizeof(key) - 1, "%s", COM2SEC_RWCOMM_DEFAULT);
        }

        
    } else if (type == SNMP_ROCOMMIUNITY_IPV6) {
        
        (void)snprintf_s(key, sizeof(key), sizeof(key) - 1, "%s", COM2SEC6_ROCOMM_DEFAULT);
        
    } else if (type == SNMP_RWCOMMIUNITY_IPV6) {
        
        if (state == DISABLED) {
            (void)snprintf_s(key, sizeof(key), sizeof(key) - 1, "#%s", COM2SEC6_RWCOMM_DEFAULT);
        } else {
            (void)snprintf_s(key, sizeof(key), sizeof(key) - 1, "%s", COM2SEC6_RWCOMM_DEFAULT);
        }

        
    } else {
        debug_log(DLOG_ERROR, "%s:%d: community type error!\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    // 修改前先将配置文件中的读写团体名状态与属性同步，防止找不到
    (void)snmp_sync_rw_community(state, COM2SEC_RWCOMM_DEFAULT);
    (void)snmp_sync_rw_community(state, COM2SEC6_RWCOMM_DEFAULT);

    
    g_mutex_lock(&g_snmp_mutex);
    fp = fm_fopen(SNMP_SHORT_FILE_NAME, "r+");
    if (fp == NULL) {
        g_mutex_unlock(&g_snmp_mutex);
        debug_log(DLOG_ERROR, "%s:%d:fail to open %s\n", __FUNCTION__, __LINE__, SNMP_SHORT_FILE_NAME);
        return RET_ERR;
    }

    p_start = fm_ftell(fp);

    
    while (fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, fp) != NULL) {
        if (strstr(buf, key) != NULL) {
            old_buf_len = strlen(buf); // 用户信息长度

            (void)memset_s(buf, sizeof(buf), 0, sizeof(buf));
            
            iRet = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "%s %s\n", key, community);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
            }
            
            new_buf_len = strlen(buf);
            ret = fm_modify(buf, (gsize)new_buf_len, (gsize)p_start, (gsize)old_buf_len, fp);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Call fm_modify failed!\n");
                fm_fclose(fp);
                g_mutex_unlock(&g_snmp_mutex);
                return RET_ERR;
            }

            fm_fclose(fp);
            g_mutex_unlock(&g_snmp_mutex);
            return RET_OK;
        }

        p_start = fm_ftell(fp);
    }

    
    fm_fclose(fp);
    g_mutex_unlock(&g_snmp_mutex);
    return RET_ERR;
}


LOCAL gint32 snmp_set_config_file(const OBJ_HANDLE object_handle, gchar *community, guint32 community_len,
    gchar *prop_name)
{
    gint32 ret = 0;
    gchar community_ct[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gsize community_ct_len = 0;
    gint32 modify_type_v4 = 0;
    gint32 modify_type_v6 = 0;

    if (community == NULL) {
        return RET_ERR;
    }

    
    ret = snmp_get_encrypt_community(community, community_ct, SNMP_CONF_MAXLINE_LEN + 1, &community_ct_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get Snmp encrypt community failed, ret = %d!\r\n", ret);
        return RET_ERR;
    }

    if (g_ascii_strcasecmp(prop_name, PROPERTY_SNMP_ROCOMMUNITY) == 0) {
        modify_type_v4 = SNMP_ROCOMMIUNITY_IPV4;
        modify_type_v6 = SNMP_ROCOMMIUNITY_IPV6;
    } else {
        modify_type_v4 = SNMP_RWCOMMIUNITY_IPV4;
        modify_type_v6 = SNMP_RWCOMMIUNITY_IPV6;
    }

    
    ret = snmp_modify_community(modify_type_v4, community_ct);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Modify Snmp ipv4 community failed!\r\n");
        return RET_ERR;
    }

    ret = snmp_modify_community(modify_type_v6, community_ct);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Modify Snmp ipv6 community failed!\r\n");
        return RET_ERR;
    }

    if (community_len == 0) {
        ret = dal_set_property_value_string(object_handle, prop_name, community, DF_SAVE);
    } else {
        ret = dal_set_property_value_string(object_handle, prop_name, community_ct, DF_SAVE);
    }
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set Snmp community failed!\r\n");
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 snmp_get_plain_community(const gchar *encrypt_comm, gchar *plain_comm, guint32 plain_comm_size)
{
    gint32 ret;
    gsize plaintext_len = 0;

    if (encrypt_comm == NULL || plain_comm == NULL || plain_comm_size == 0) {
        debug_log(DLOG_ERROR, "%s: input parm is null", __FUNCTION__);
        return RET_ERR;
    }

    ret = kmc_get_decrypted_data(DOMAIN_ID_SNMP_COMM, encrypt_comm, plain_comm, plain_comm_size, &plaintext_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "kmc_get_decrypted_data failed, ret = %d", ret);
        return ret;
    }

    return RET_OK;
}


LOCAL gboolean is_customize_flag(void)
{
    guint8 status = DISABLED;

    OBJ_HANDLE handle = 0;
    (void)dal_get_object_handle_nth(CLASS_DFT_STATUS, 0, &handle);
    (void)dal_get_property_value_byte(handle, PROPERTY_DFT_ENABLE, &status);
    return (status == ENABLED) ? TRUE : FALSE;
}


LOCAL gint32 check_differs_from_precommunity(const OBJ_HANDLE object_handle, gchar *community_property,
    gchar *community, guint32 community_len)
{
    gint32 ret;
    OBJ_HANDLE pw_setting_handle = 0;
    guint8 complexity_enable_state = 0;
    gchar community_ct_last[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar community_pt_last[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    
    
    if (is_customize_flag() == FALSE) {
        
        (void)dal_get_object_handle_nth(OBJ_PASSWD_SETTING, 0, &pw_setting_handle);
        (void)dal_get_property_value_byte(pw_setting_handle, PROPERTY_USER_PASSWD_SETTING, &complexity_enable_state);

        if (complexity_enable_state != 0) {
            (void)dal_get_property_value_string(object_handle, community_property, community_ct_last,
                sizeof(community_ct_last));

            if (snmp_get_plain_community(community_ct_last, community_pt_last, sizeof(community_pt_last)) != RET_OK) {
                debug_log(DLOG_ERROR, "%s: snmp_get_plain_community failed!", __FUNCTION__);
                return RET_ERR;
            }

            ret = dal_check_A_differs_from_B(community, community_len, community_pt_last, sizeof(community_pt_last));
            clear_sensitive_info(community_pt_last, sizeof(community_pt_last));
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: dal_check_A_differs_from_B failed!", __FUNCTION__);
                return SNMP_COMMUNITY_PREVIOUS;
            }
        }

        
    }
    
    

    return RET_OK;
}


LOCAL gint32 snmp_check_duplicate_community(const OBJ_HANDLE object_handle, gboolean is_readonly, gchar *community,
    guint32 community_len)
{
    gint32 ret = 0;
    gchar community_property[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar other_community_property[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar other_community_pt[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    GVariant *property_value = NULL;
    gchar *other_community_ct = NULL;

    if (community == NULL || community_len == 0) {
        return RET_ERR;
    }

    ret = snprintf_s(community_property, sizeof(community_property), sizeof(community_property) - 1, "%s",
        (is_readonly ? PROPERTY_SNMP_ROCOMMUNITY : PROPERTY_SNMP_RWCOMMUNITY));
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret:%d", __FUNCTION__, ret);
    }
    ret = snprintf_s(other_community_property, sizeof(other_community_property), sizeof(other_community_property) - 1,
        "%s", (is_readonly ? PROPERTY_SNMP_RWCOMMUNITY : PROPERTY_SNMP_ROCOMMUNITY));
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret:%d", __FUNCTION__, ret);
    }

    ret = check_differs_from_precommunity(object_handle, community_property, community, community_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: check_differs_from_precommunity failed, ret:%d", __FUNCTION__, ret);
        return SNMP_COMMUNITY_PREVIOUS;
    }

    if (dfl_get_property_value(object_handle, other_community_property, &property_value) != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get Snmp %s failed!", __FUNCTION__, other_community_property);
        return RET_ERR;
    }

    other_community_ct = g_variant_dup_string(property_value, 0);
    g_variant_unref(property_value);

    
    ret = snmp_get_plain_community(other_community_ct, other_community_pt, sizeof(other_community_pt));
    free_mem_set_null(other_community_ct);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get Snmp plain %s failed, ret = %d!\r\n", __FUNCTION__, other_community_property,
            ret);
        return RET_ERR;
    }

    if (!strcmp(other_community_pt, community)) {
        debug_log(DLOG_ERROR, "Snmp %s is the same with %s!", community_property, other_community_property);
        clear_sensitive_info(other_community_pt, sizeof(other_community_pt));
        return SNMP_COMMUNITY_FIELD;
    }

    clear_sensitive_info(other_community_pt, sizeof(other_community_pt));
    return RET_OK;
}


LOCAL gint32 snmp_check_weak_passwd(const gchar *passwd)
{
    gint32 ret = 0;
    GSList *input_list = NULL;
    OBJ_HANDLE security_obj = 0;

    (void)dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &security_obj);

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(passwd));
    
    ret = dfl_call_class_method(security_obj, METHOD_SECURITY_SEARCH_WEAK_PWDDICT, NULL, input_list, NULL);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    return ret;
}

LOCAL gint32 snmp_check_community_complexity_pass(const gchar *community)
{
    gint32 ret = 0;

    if (community == NULL) {
        return RET_ERR;
    }

    
    if (strlen(community) < SMMP_COMMUNITY_ATLEASTLEN) {
        return RET_ERR;
    }

    

    ret = dal_check_complexity_status(community);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 snmp_check_community_complexity(const gchar *community)
{
    gint32 ret = 0;
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_value = NULL;
    guint8 complexity_check_state = 0;

    if (community == NULL) {
        return RET_ERR;
    }

    
    ret = dfl_get_object_handle(OBJ_PASSWD_SETTING, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get password setting object handle failed!\n");
        return RET_ERR;
    }

    
    ret = dfl_get_property_value(obj_handle, PROPERTY_USER_PASSWD_SETTING, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get community complexity check state failed!\n");
        return RET_ERR;
    }

    complexity_check_state = g_variant_get_byte(property_value);
    g_variant_unref(property_value);

    
    if (complexity_check_state == 0) {
        return RET_OK;
    }

    ret = snmp_check_community_complexity_pass(community);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Community complexity check failed!\n");
        return SNMP_COMMUNITY_COMPLEXITY_FAIL;
    }

    return RET_OK;
}


gint32 GetSnmpROCommunity(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    gchar community_name[SNMP_CONF_MAXLINE_LEN + 1] = {0};
    GVariant *property_value = NULL;
    gchar *community_ct = NULL;
    
    if (dfl_get_property_value(object_handle, PROPERTY_SNMP_ROCOMMUNITY, &property_value) != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get Snmp %s failed!", __FUNCTION__, PROPERTY_SNMP_ROCOMMUNITY);
        return RET_ERR;
    }
    community_ct = g_variant_dup_string(property_value, 0);
    g_variant_unref(property_value);
 
    ret = snmp_get_plain_community(community_ct, community_name, sizeof(community_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get Snmp plain %s failed, ret = %d", __FUNCTION__, community_ct, ret);
        free_mem_set_null(community_ct);
        return RET_ERR;
    }
    free_mem_set_null(community_ct);
    *output_list = g_slist_append(*output_list, g_variant_new_string(community_name));
    
    return RET_OK;
}
 

gint32 GetSnmpRWCommunity(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    gchar community_name[SNMP_CONF_MAXLINE_LEN + 1] = {0};
    GVariant *property_value = NULL;
    gchar *community_ct = NULL;
        
    if (dfl_get_property_value(object_handle, PROPERTY_SNMP_RWCOMMUNITY, &property_value) != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get Snmp %s failed!", __FUNCTION__, PROPERTY_SNMP_RWCOMMUNITY);
        return RET_ERR;
    }
    community_ct = g_variant_dup_string(property_value, 0);
    g_variant_unref(property_value);
 
    ret = snmp_get_plain_community(community_ct, community_name, sizeof(community_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get Snmp plain %s failed, ret = %d", __FUNCTION__, community_ct, ret);
        free_mem_set_null(community_ct);
        return RET_ERR;
    }
    free_mem_set_null(community_ct);
    *output_list = g_slist_append(*output_list, g_variant_new_string(community_name));
    
    return RET_OK;
}


gint32 SetSnmpROCommunity(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret = 0;
    guint8 is_long_password = 0;
    gsize community_len = 0x00;
    gchar *ROCommunity = NULL;
    gchar slot[SNMP_SERVICE_SLOT] = { 0 };
    gboolean is_readonly = TRUE;
    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Input_list can not be NULL.\r\n");
        return RET_ERR;
    }
    

    ROCommunity = g_variant_dup_string((GVariant *)g_slist_nth_data(input_list, 0), &community_len);
    if (ROCommunity == NULL) {
        debug_log(DLOG_ERROR, "Input NULL pointer!\r\n");
        method_operation_log(caller_info, NULL, "Set SNMP ROCommunity failed\n");
        return RET_ERR;
    }

    
    if (strlen(ROCommunity) == 0) {
        ret = snmp_set_config_file(object_handle, ROCommunity, community_len, PROPERTY_SNMP_ROCOMMUNITY);
        free_mem_set_null(ROCommunity);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Delete Snmp ROCommunity failed!\r\n");
            method_operation_log(caller_info, NULL, "Delete SNMP ROCommunity failed\n");
            return RET_ERR;
        }
        method_operation_log(caller_info, NULL, "Delete SNMP ROCommunity successfully\n");
        goto exit_ok;
    }
    

    // 规范要求团体名不能包含空格
    if (vos_check_incorrect_char(ROCommunity, SNMP_ROCOMMUNITY_LIMIT_CHARACTER, IGNORE_LENGTH)) {
        method_operation_log(caller_info, NULL, "Set SNMP ROCommunity failed\n");
        clear_sensitive_info(ROCommunity, community_len);
        free_mem_set_null(ROCommunity);
        return SNMP_COMMUNITY_SPACE_FAIL;
    }

    
    // 获取超长口令属性
    ret = dal_get_property_value_byte(object_handle, PROPERTY_SNMP_LONG_PASSWORD_ENABLE, &is_long_password);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get Snmp LongPasswordEnable failed!\r\n");
        method_operation_log(caller_info, NULL, "Set SNMP ROCommunity failed\n");
        clear_sensitive_info(ROCommunity, community_len);
        free_mem_set_null(ROCommunity);
        return RET_ERR;
    }

    
    if ((strlen(ROCommunity) < SNMP_ROCOMMUNITY_MIN_LEN) ||
        (is_long_password == 1 && strlen(ROCommunity) < SNMP_LONG_PASSWORD_MIN_LEN) ||
        strlen(ROCommunity) > SNMP_ROCOMMUNITY_MAX_LEN) {
        
        method_operation_log(caller_info, NULL, "Set SNMP ROCommunity failed\n");
        clear_sensitive_info(ROCommunity, community_len);
        free_mem_set_null(ROCommunity);
        return SNMP_COMMUNITY_LENGTH_FAIL;
    }

    
    
    ret = snmp_check_weak_passwd((const gchar *)ROCommunity);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "Set SNMP ROCommunity failed\n");
        clear_sensitive_info(ROCommunity, community_len);
        free_mem_set_null(ROCommunity);
        return SNMP_COMMUNITY_TOO_WEAK;
    }
    

    ret = snmp_check_community_complexity(ROCommunity);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "Set SNMP ROCommunity failed\n");
        clear_sensitive_info(ROCommunity, community_len);
        free_mem_set_null(ROCommunity);
        return SNMP_COMMUNITY_COMPLEXITY_FAIL;
    }

    
    ret = snmp_check_duplicate_community(object_handle, is_readonly, ROCommunity, community_len);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "Set SNMP ROCommunity failed\n");
        clear_sensitive_info(ROCommunity, community_len);
        free_mem_set_null(ROCommunity);
        return ret;
    }

    ret = snmp_set_config_file(object_handle, ROCommunity, community_len, PROPERTY_SNMP_ROCOMMUNITY);
    clear_sensitive_info(ROCommunity, community_len);
    free_mem_set_null(ROCommunity);

    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "Set SNMP ROCommunity failed\n");
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL, "Set SNMP ROCommunity successfully\n");
    
exit_ok:
    debug_log(DLOG_DEBUG, "begin to readonlycommunity name changed event");
    ret = dal_rf_get_board_slot(slot, sizeof(slot));
    if (ret != RET_OK) {
        return RET_ERR;
    }
    ret = dal_rf_rsc_evt_log(RF_RSC_UPDATED_EVT, FALSE, SNMPSERVICE_PATH, slot);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "send community name changed event failed, ret is %d", ret);
    }
    

    return RET_OK;
}

gint32 SetSnmpState(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret = 0;
    gint32 state = 0;
    guint8 *option = NULL;
    GVariant *property_value = NULL;
    guchar SNMPEnable = 0;
    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Input_list can not be NULL.\r\n");
        return RET_ERR;
    }
    

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_SNMPD, &SNMPEnable);
    
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "%s, %d: get_SnmpdEnable_status fail.\n", __FUNCTION__, __LINE__);
        method_operation_log(caller_info, NULL, "Set SNMP Agent service state failed");
        return ret;
    }
    if (SNMPEnable == SERVICE_STATE_DISABLE) {
        method_operation_log(caller_info, NULL, "Set SNMP Agent service state failed");
        return COMP_CODE_INVALID_CMD;
    }
    
    

    state = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if (state == SNMP_STATE_DISABLE) {
        option = (guint8 *)g_malloc0(sizeof(guint8));
        if (option == NULL) {
            debug_log(DLOG_ERROR, "Malloc failed!\n");
            method_operation_log(caller_info, NULL, "Disable SNMP Agent service failed\n");
            return RET_ERR;
        }

        *option = STOP_SNMP;
        ret = vos_queue_send(g_snmpSendQueue, option);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Send stop snmp command failed!\n");
            method_operation_log(caller_info, NULL, "Disable SNMP Agent service failed\n");
            g_free(option);
            return RET_ERR;
        }
    } else if (state == SNMP_STATE_ENABLE) {
        if (g_snmpd_enable_flag == SNMP_STATE_ENABLE) {
            option = (guint8 *)g_malloc0(sizeof(guint8));
            if (option == NULL) {
                debug_log(DLOG_ERROR, "Malloc failed!\n");
                method_operation_log(caller_info, NULL, "Enable SNMP Agent service failed\n");
                return RET_ERR;
            }

            *option = START_SNMP;
            ret = vos_queue_send(g_snmpSendQueue, option);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Send start snmp command failed!\n");
                method_operation_log(caller_info, NULL, "Enable SNMP Agent service failed\n");
                g_free(option);
                return RET_ERR;
            }
        } else {
            debug_log(DLOG_ERROR, "%s:PMEServiceConfig.SnmpdEnable=0,can not start snmpd", __FUNCTION__);
            method_operation_log(caller_info, NULL, "Enable SNMP Agent service failed\n");
            return RET_ERR;
        }
    } else {
        debug_log(DLOG_ERROR, "Input state error!\r\n");
        method_operation_log(caller_info, NULL, "Set SNMP Agent service state failed\n");
        return RET_ERR;
    }

    property_value = g_variant_new_byte(state);
    ret = dfl_set_property_value(object_handle, PROPERTY_SNMP_STATE, property_value, DF_SAVE);
    g_variant_unref(property_value);

    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set Snmp state (%s) failed!\r\n", state ? "enable" : "disable");
        method_operation_log(caller_info, NULL, "%s SNMP Agent service failed\n", state ? "Enable" : "Disable");
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL, "%s SNMP Agent service successfully\n", state ? "Enable" : "Disable");
    return RET_OK;
}

gint32 SetSnmpRWCommunity(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret = 0;
    guint8 is_long_password = 0;
    gsize community_len = 0x00;
    gchar *RWCommunity = NULL;
    gchar slot[SNMP_SERVICE_SLOT] = { 0 };
    gboolean is_readonly = FALSE;
    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Input_list can not be NULL.\r\n");
        return RET_ERR;
    }
    

    RWCommunity = g_variant_dup_string((GVariant *)g_slist_nth_data(input_list, 0), &community_len);
    if (RWCommunity == NULL) {
        debug_log(DLOG_ERROR, "Input NULL pointer!\r\n");
        method_operation_log(caller_info, NULL, "Set SNMP RWCommunity failed\n");
        return RET_ERR;
    }

    
    if (strlen(RWCommunity) == 0) {
        ret = snmp_set_config_file(object_handle, RWCommunity, community_len, PROPERTY_SNMP_RWCOMMUNITY);
        free_mem_set_null(RWCommunity);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Delete Snmp RWCommunity failed!\r\n");
            method_operation_log(caller_info, NULL, "Delete SNMP RWCommunity failed\n");
            return RET_ERR;
        }
        method_operation_log(caller_info, NULL, "Delete SNMP RWCommunity successfully\n");
        goto exit_ok;
    }
    
    // 规范要求团体名不能包含空格
    if (vos_check_incorrect_char(RWCommunity, SNMP_RWCOMMUNITY_LIMIT_CHARACTER, IGNORE_LENGTH)) {
        debug_log(DLOG_ERROR, "%s failed: check community failed: no space character allowed", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set SNMP RWCommunity failed\n");
        clear_sensitive_info(RWCommunity, community_len);
        free_mem_set_null(RWCommunity);
        return SNMP_COMMUNITY_SPACE_FAIL;
    }

    
    // 获取超长口令属性
    ret = dal_get_property_value_byte(object_handle, PROPERTY_SNMP_LONG_PASSWORD_ENABLE, &is_long_password);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get Snmp LongPasswordEnable failed!\r\n");
        method_operation_log(caller_info, NULL, "Set SNMP RWCommunity failed\n");
        clear_sensitive_info(RWCommunity, community_len);
        free_mem_set_null(RWCommunity);
        return RET_ERR;
    }

    
    if ((strlen(RWCommunity) < SNMP_RWCOMMUNITY_MIN_LEN) ||
        (is_long_password == 1 && strlen(RWCommunity) < SNMP_LONG_PASSWORD_MIN_LEN) ||
        strlen(RWCommunity) > SNMP_RWCOMMUNITY_MAX_LEN) {
        
        debug_log(DLOG_ERROR, "%s failed: check community failed: length invalid", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set SNMP RWCommunity failed\n");
        clear_sensitive_info(RWCommunity, community_len);
        free_mem_set_null(RWCommunity);
        return SNMP_COMMUNITY_LENGTH_FAIL;
    }

    
    
    ret = snmp_check_weak_passwd((const gchar *)RWCommunity);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s failed: check community failed: check weak passwd failed", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set SNMP RWCommunity failed\n");
        clear_sensitive_info(RWCommunity, community_len);
        free_mem_set_null(RWCommunity);
        return SNMP_COMMUNITY_TOO_WEAK;
    }
    

    ret = snmp_check_community_complexity(RWCommunity);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s failed: check community failed: check complexity failed", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set SNMP RWCommunity failed\n");
        clear_sensitive_info(RWCommunity, community_len);
        free_mem_set_null(RWCommunity);
        return SNMP_COMMUNITY_COMPLEXITY_FAIL;
    }

    
    ret = snmp_check_duplicate_community(object_handle, is_readonly, RWCommunity, community_len);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "Set SNMP RWCommunity failed\n");
        clear_sensitive_info(RWCommunity, community_len);
        free_mem_set_null(RWCommunity);
        return ret;
    }

    ret = snmp_set_config_file(object_handle, RWCommunity, community_len, PROPERTY_SNMP_RWCOMMUNITY);
    clear_sensitive_info(RWCommunity, community_len);
    free_mem_set_null(RWCommunity);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "Set SNMP RWCommunity failed\n");
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL, "Set SNMP RWCommunity successfully\n");

exit_ok:
    
    debug_log(DLOG_DEBUG, "begin to readwrite community name changed event");
    ret = dal_rf_get_board_slot(slot, sizeof(slot));
    if (ret != RET_OK) {
        return RET_ERR;
    }
    ret = dal_rf_rsc_evt_log(RF_RSC_UPDATED_EVT, FALSE, SNMPSERVICE_PATH, slot);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "send community name changed event failed, ret is %d", ret);
    }
    
    return RET_OK;
}


gint32 snmp_modify_version_state(guint8 version, guint8 type, guint8 state)
{
    gint32 iRet = -1;
    gint32 ret = 0;
    FM_FILE_S *fp = NULL;
    gint32 p_start = 0;
    gint32 old_buf_len = 0;
    gint32 new_buf_len = 0;
    gchar key[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };

    
    if (type == 0) {
        if (version == 0) {
            (void)snprintf_s(key, sizeof(key), sizeof(key) - 1, "%s", "group   rogroup  v1      rocomm");
        } else {
            (void)snprintf_s(key, sizeof(key), sizeof(key) - 1, "%s", "group   rogroup  v2c     rocomm");
        }
    } else {
        if (version == 0) {
            (void)snprintf_s(key, sizeof(key), sizeof(key) - 1, "%s", "group   rwgroup  v1      rwcomm");
        } else {
            (void)snprintf_s(key, sizeof(key), sizeof(key) - 1, "%s", "group   rwgroup  v2c     rwcomm");
        }
    }
    
    g_mutex_lock(&g_snmp_mutex);
    fp = fm_fopen(SNMP_SHORT_FILE_NAME, "r+");
    if (fp == NULL) {
        g_mutex_unlock(&g_snmp_mutex);
        debug_log(DLOG_ERROR, "%s:%d:fail to open %s\n", __FUNCTION__, __LINE__, SNMP_SHORT_FILE_NAME);
        return RET_ERR;
    }

    p_start = fm_ftell(fp);

    
    while (fm_fgets(buf, 1 + SNMP_CONF_MAXLINE_LEN, fp) != NULL) {
        if (strstr(buf, key) != NULL) {
            old_buf_len = strlen(buf); 

            
            (void)memset_s(buf, SNMP_CONF_MAXLINE_LEN + 1, 0, SNMP_CONF_MAXLINE_LEN + 1);

            
            if (state == 0) {
                iRet = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "%c%s\n", '#', key);
            } else {
                iRet = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "%s\n", key);
            }
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, state = %u, ret = %d", __FUNCTION__, state, iRet);
            }
            

            new_buf_len = strlen(buf);
            ret = fm_modify(buf, (gsize)new_buf_len, (gsize)p_start, (gsize)old_buf_len, fp);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Call fm_modify failed!\n");
                fm_fclose(fp);
                g_mutex_unlock(&g_snmp_mutex);
                return RET_ERR;
            }

            fm_fclose(fp);
            g_mutex_unlock(&g_snmp_mutex);
            return RET_OK;
        }

        p_start = fm_ftell(fp);
    }

    
    fm_fclose(fp);
    g_mutex_unlock(&g_snmp_mutex);
    return RET_ERR;
}


gint32 SetSnmpV1VersionState(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    guint8 state = 0;
    GVariant *property_value = NULL;
    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Input_list can not be NULL.\r\n");
        return RET_ERR;
    }
    

    state = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if ((state != SNMP_STATE_DISABLE && state != SNMP_STATE_ENABLE)) {
        debug_log(DLOG_ERROR, "Input state error!\r\n");
        method_operation_log(caller_info, NULL, "Set SNMP V1 state failed");
        return RET_ERR;
    }

    
    ret = snmp_modify_version_state(0, SNMP_COMMUNITY_RO, state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Modify Snmp V1 ROCommunity state(%s) failed!\r\n", state ? "enable" : "disable");
        method_operation_log(caller_info, NULL, "%s SNMP V1 failed", state ? "Enable" : "Disable");
        return RET_ERR;
    }

    ret = snmp_modify_version_state(0, SNMP_COMMUNITY_RW, state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Modify Snmp V1 RWCommunity state(%s) failed!\r\n", state ? "enable" : "disable");
        method_operation_log(caller_info, NULL, "%s SNMP V1 failed", state ? "Enable" : "Disable");
        return RET_ERR;
    }

    property_value = g_variant_new_byte(state);
    ret = dfl_set_property_value(object_handle, PROPERTY_SNMP_V1STATE, property_value, DF_SAVE);
    g_variant_unref(property_value);

    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set Snmp V1 state(%s) failed!\r\n", state ? "enable" : "disable");
        method_operation_log(caller_info, NULL, "%s SNMP V1 failed", state ? "Enable" : "Disable");
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL, "%s SNMP V1 successfully", state ? "Enable" : "Disable");
    return RET_OK;
}

gint32 SetSnmpV2CVersionState(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    guint8 state = 0;
    GVariant *property_value = NULL;
    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Input_list can not be NULL.\r\n");
        return RET_ERR;
    }
    

    state = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if ((state != SNMP_STATE_DISABLE && state != SNMP_STATE_ENABLE)) {
        debug_log(DLOG_ERROR, "Input state error!\r\n");
        method_operation_log(caller_info, NULL, "Set SNMP V2C state failed");
        return RET_ERR;
    }

    
    ret = snmp_modify_version_state(1, SNMP_COMMUNITY_RO, state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Modify Snmp V2C ROCommunity state(%s) failed!\r\n", state ? "enable" : "disable");
        method_operation_log(caller_info, NULL, "%s SNMP V2C failed", state ? "Enable" : "Disable");
        return RET_ERR;
    }

    ret = snmp_modify_version_state(1, SNMP_COMMUNITY_RW, state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Modify Snmp V2C RWCommunity state(%s) failed!\r\n", state ? "enable" : "disable");
        method_operation_log(caller_info, NULL, "%s SNMP V2C failed", state ? "Enable" : "Disable");
        return RET_ERR;
    }

    property_value = g_variant_new_byte(state);
    ret = dfl_set_property_value(object_handle, PROPERTY_SNMP_V2CSTATE, property_value, DF_SAVE);
    g_variant_unref(property_value);

    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set Snmp V2C state(%s) failed!\r\n", state ? "enable" : "disable");
        method_operation_log(caller_info, NULL, "%s SNMP V2C failed", state ? "Enable" : "Disable");
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL, "%s SNMP V2C successfully", state ? "Enable" : "Disable");
    return RET_OK;
}


LOCAL gint32 snmp_modify_state(guint8 state, guint8 version, GSList *caller_info)
{
    gint32 ret;
    
    ret = snmp_modify_version_state(version, SNMP_COMMUNITY_RO, state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Modify Snmp %s ROCommunity state(%s) failed!", snmp_version_info[version].name,
            state ? "enable" : "disable");
        method_operation_log(caller_info, NULL, "%s SNMP %s failed", state ? "Enable" : "Disable",
            snmp_version_info[version].name);
        return RET_ERR;
    }
    ret = snmp_modify_version_state(version, SNMP_COMMUNITY_RW, state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Modify Snmp %s RWCommunity state(%s) failed!", snmp_version_info[version].name,
            state ? "enable" : "disable");
        method_operation_log(caller_info, NULL, "%s SNMP %s failed", state ? "Enable" : "Disable",
            snmp_version_info[version].name);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 foreach_add_snmp_userinfo(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret;
    gchar username[USER_NAME_MAX_LEN + 1] = { 0 };

    ret = dal_get_property_value_string(obj_handle, PROPERTY_USER_NAME, username, sizeof(username));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get %s.%s failed, ret %d", dfl_get_object_name(obj_handle), PROPERTY_USER_NAME, ret);
        return RET_OK;
    }

    if (strlen(username) != 0 && snmp_check_username_exist(username) != SNMP_USERNAME_EXIST) {
        ret = snmp_add_username_info(username); 
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "add username to file failed, username(%s).", username);
        }
    }
    return RET_OK;
}


LOCAL gint32 snmp_add_all_userinfo_to_file(void)
{
    gint32 ret;

    ret = dfl_foreach_object(CLASS_USER, foreach_add_snmp_userinfo, NULL, NULL);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "add all snmp userinfo failed, ret %d", ret);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 __check_del_snmp_userinfo(void)
{
    guint8 match_flag;
    gint32 ret;
    do {
        match_flag = FALSE;
        ret = del_snmp_conf_from_file(NULL, &match_flag);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: del_snmp_conf_from_file fail, ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    } while (match_flag == TRUE);
    return RET_OK;
}


LOCAL gint32 foreach_del_snmp_userinfo(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret;
    gchar username[USER_NAME_MAX_LEN + 1] = { 0 };

    ret = dal_get_property_value_string(obj_handle, PROPERTY_USER_NAME, username, sizeof(username));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get %s.%s failed, ret %d", dfl_get_object_name(obj_handle), PROPERTY_USER_NAME, ret);
        return RET_OK;
    }

    if (strlen(username) != 0 && snmp_check_username_exist(username) == SNMP_USERNAME_EXIST) {
        ret = snmp_del_username_from_file(username); 
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "delete username form file failed, username(%s).", username);
        }
    }
    return RET_OK;
}


gint32 snmp_del_all_userinfo_from_file(void)
{
    gint32 ret;

    ret = dfl_foreach_object(CLASS_USER, foreach_del_snmp_userinfo, NULL, NULL);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "delete all snmp userinfo failed, ret %d", ret);
        return RET_ERR;
    }

    ret = __check_del_snmp_userinfo();
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "check delete username form file failed.");
    }
    return RET_OK;
}


gint32 SetSnmpV3VersionState(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    guint8 state = 0;
    guint8 cur_state = 0;
    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Input_list can not be NULL.");
        return RET_ERR;
    }
    

    state = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if (state != SNMP_STATE_ENABLE && state != SNMP_STATE_DISABLE) {
        debug_log(DLOG_ERROR, "Input state error!\r\n");
        method_operation_log(caller_info, NULL, "Set SNMP V3 state failed");
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(object_handle, PROPERTY_SNMP_V3STATUS, &cur_state);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "Set SNMP V3 state failed");
        return RET_ERR;
    }
    if (cur_state == state) { 
        method_operation_log(caller_info, NULL, "%s SNMP V3 successfully", state ? "Enable" : "Disable");
        return RET_OK;
    }

    ret = dal_set_property_value_byte(object_handle, PROPERTY_SNMP_V3STATUS, state, DF_SAVE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set Snmp V3 state(enable) failed!");
        method_operation_log(caller_info, NULL, "%s SNMP V3 failed", state ? "Enable" : "Disable");
        return RET_ERR;
    }
    g_snmpv3_status = state;

    
    if (state == SNMP_STATE_ENABLE) {
        ret = snmp_add_all_userinfo_to_file();
    } else if (state == SNMP_STATE_DISABLE) {
        ret = snmp_del_all_userinfo_from_file();
    }
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "%s SNMP V3 failed", state ? "Enable" : "Disable");
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL, "%s SNMP V3 successfully", state ? "Enable" : "Disable");
    return RET_OK;
}


LOCAL gint32 set_snmpV3_state_to_file(guint8 state, guint8 version, GSList *caller_info)
{
    gint32 ret = 0;
    g_snmpv3_status = state;
    
    if (state == SNMP_STATE_ENABLE) {
        ret = snmp_add_all_userinfo_to_file();
    } else if (state == SNMP_STATE_DISABLE) {
        ret = snmp_del_all_userinfo_from_file();
    }
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "%s SNMP %s failed", state ? "Enable" : "Disable",
            snmp_version_info[version].name);
        return RET_ERR;
    }
    return RET_OK;
}


gint32 SetSnmpVersionState(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    guint8 state = 0;
    guint8 version = 0;
    GVariant *property_value = NULL;
    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Input_list can not be NULL.\r\n");
        return RET_ERR;
    }
    

    version = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    state = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
    if (state != SNMP_STATE_DISABLE && state != SNMP_STATE_ENABLE) {
        debug_log(DLOG_ERROR, "Input state error!\r\n");
        method_operation_log(caller_info, NULL, "Set SNMP version state failed");
        return RET_ERR;
    }

    if (version != SNMP_VERSION_V1 && version != SNMP_VERSION_V2C && version != SNMP_VERSION_V3) {
        debug_log(DLOG_ERROR, "Input version error!\r\n");
        method_operation_log(caller_info, NULL, "Set SNMP version state failed");
        return RET_ERR;
    }

    if (version != SNMP_VERSION_V3) {
        
        ret = snmp_modify_state(state, version, caller_info);
        if (ret != RET_OK) {
            return RET_ERR;
        }
    } else {
        ret = set_snmpV3_state_to_file(state, version, caller_info);
        if (ret != RET_OK) {
            return RET_ERR;
        }
    }

    property_value = g_variant_new_byte(state);
    ret = dfl_set_property_value(object_handle, (const gchar *)snmp_version_info[version].property_name, property_value,
        DF_SAVE);
    g_variant_unref(property_value);

    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set Snmp %s state(%s) failed!\r\n", snmp_version_info[version].name,
            state ? "enable" : "disable");
        method_operation_log(caller_info, NULL, "%s SNMP %s failed", state ? "Enable" : "Disable",
            snmp_version_info[version].name);
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL, "%s SNMP %s successfully", state ? "Enable" : "Disable",
        snmp_version_info[version].name);
    return RET_OK;
}


guint8 get_snmpv3_status(void)
{
    return g_snmpv3_status;
}


void get_snmpv3_initial_status(guint8 *status)
{
    status = &g_snmpv3_status;
}


LOCAL gboolean snmp_port_check(guint16 port)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *p_input_list = NULL;
    gint32 retv = 0;
    guchar port_index = SNMP_SERVICE_PORT_INDEX;

    retv = dfl_get_object_handle(SSH_SERVER_PORT_OBJ_NAME, &obj_handle); 
    if (retv != DFL_OK) {
        debug_log(DLOG_ERROR, "Can't find object %s. Ret is %d\n", MODULE_NAME_SNMP, retv);
        return FALSE;
    }

    p_input_list = g_slist_append(p_input_list, g_variant_new_uint16(port));
    p_input_list = g_slist_append(p_input_list, g_variant_new_byte(port_index));
    retv = dfl_call_class_method(obj_handle, SERVER_PORT_METHOD_PORT_CHECK, NULL, p_input_list, NULL);
    if (retv != DFL_OK) {
        g_slist_free_full(p_input_list, (GDestroyNotify)g_variant_unref);
        return FALSE;
    }

    g_slist_free_full(p_input_list, (GDestroyNotify)g_variant_unref);
    return TRUE;
}


LOCAL void print_snmp_udp6_addr_str(SNMP_AGENT_ADDR_S *agent_addr, gchar *udp6_addr, guint32 len)
{
    gint32 ret;
    gchar str_tmp[SNMP_CONF_MAXLINE_LEN] = { 0 };
    guint i;
    gboolean is_slaac = FALSE;

    if (agent_addr == NULL || udp6_addr == NULL) {
        return;
    }

    IP_ETHNAME_MAP_INFO_S map_info[] = {
        {(const gchar*)agent_addr->ipv6_out, ""},
        {(const gchar*)agent_addr->ipv6_bma,
         strcmp(VHOST_DEFAULT_SMS_IPADDR, (const gchar*)agent_addr->ipv6_bma) == 0 ? "veth" : ""},
        {(const gchar*)agent_addr->ipv6_float, ""},
        {(const gchar*)agent_addr->ip_local_link, (const gchar*)agent_addr->eth_name}
    };

    ret = is_ipv6_slaac_mode(&is_slaac);
    // 如果检查是否为slaac模式失败或者开启了slaac模式, 则监听所有ipv6地址
    if (ret != RET_OK || is_slaac == TRUE) {
        ret = snprintf_truncated_s(udp6_addr, len, "udp6:%d", agent_addr->port);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s:snprintf_truncated_s failed, ret:%d", __FUNCTION__, ret);
        }
        return;
    }

    if (strlen(agent_addr->ipv6_out) == 0 && strlen(agent_addr->ipv6_bma) == 0 && strlen(agent_addr->ipv6_float) == 0 &&
        strlen(agent_addr->ip_local_link) == 0) {
        // 如果4个ipv6地址全为空,则不监听ipv6地址
        (void)memset_s(udp6_addr, len, 0, len);
        return;
    }

    for (i = 0; i < G_N_ELEMENTS(map_info); i++) {
        if (strlen(map_info[i].ip) == 0) {
            continue;
        }

        (void)memset_s(str_tmp, sizeof(str_tmp), 0, sizeof(str_tmp));
        if (strlen(udp6_addr) == 0) {
            ret = snprintf_truncated_s(str_tmp, sizeof(str_tmp), "udp6:[%s%s%s]:%d", map_info[i].ip,
                (strlen(map_info[i].eth_name) != 0) ? "%" : "", map_info[i].eth_name, agent_addr->port);
        } else {
            ret = snprintf_truncated_s(str_tmp, sizeof(str_tmp), ",udp6:[%s%s%s]:%d", map_info[i].ip,
                (strlen(map_info[i].eth_name) != 0) ? "%" : "", map_info[i].eth_name, agent_addr->port);
        }

        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s:snprintf_truncated_s failed, ret:%d", __FUNCTION__, ret);
            continue;
        }

        ret = strncat_s(udp6_addr, len, str_tmp, strlen(str_tmp));
        if (ret != EOK) {
            debug_log(DLOG_ERROR, "%s:strncat_s failed, ret:%d", __FUNCTION__, ret);
            continue;
        }
    }
}


LOCAL void print_snmp_udp_addr_str(SNMP_AGENT_ADDR_S *agent_addr, gchar *udp_addr, guint32 len)
{
    gint32 ret;
    gchar str_tmp[SNMP_CONF_MAXLINE_LEN] = { 0 };
    guint i;

    if (agent_addr == NULL || udp_addr == NULL) {
        return;
    }

    const gchar *ips[] = {(const gchar*)agent_addr->ipv4_out, (const gchar*)agent_addr->ipv4_bma,
                          (const gchar*)agent_addr->ipv4_float, (const gchar*)agent_addr->ipv4_maint, NULL};

    if (strlen(agent_addr->ipv4_out) == 0 && strlen(agent_addr->ipv4_bma) == 0 && strlen(agent_addr->ipv4_float) == 0 &&
        strlen(agent_addr->ipv4_maint) == 0) {
        // 如果ipv4地址全为空,则不监听ipv4地址
        (void)memset_s(udp_addr, len, 0, len);
        return;
    }

    for (i = 0; ips[i] != NULL; i++) {
        if (strlen(ips[i]) == 0) {
            continue;
        }

        (void)memset_s(str_tmp, sizeof(str_tmp), 0, sizeof(str_tmp));
        if (strlen(udp_addr) == 0) {
            ret = snprintf_truncated_s(str_tmp, sizeof(str_tmp), "udp:%s:%d", ips[i], agent_addr->port);
        } else {
            ret = snprintf_truncated_s(str_tmp, sizeof(str_tmp), ",udp:%s:%d", ips[i], agent_addr->port);
        }

        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s:snprintf_truncated_s failed, ret:%d", __FUNCTION__, ret);
            continue;
        }

        ret = strncat_s(udp_addr, len, str_tmp, strlen(str_tmp));
        if (ret != EOK) {
            debug_log(DLOG_ERROR, "%s:strncat_s failed, ret:%d", __FUNCTION__, ret);
            continue;
        }
    }
}

LOCAL gint32 __get_ipv4_maint(SNMP_AGENT_ADDR_S *agent_addr)
{
    guchar net_type = NET_TYPE_DEDICATED;
    OBJ_HANDLE group_obj = 0;
    // 获取对外管理网口类型
    if (dal_get_specific_object_byte(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_OUT_TYPE, OUT_ETHERNET, &group_obj) ==
        RET_OK) {
        (void)dal_get_property_value_byte(group_obj, ETH_GROUP_ATTRIBUTE_TYPE, &net_type);
    }

    
    guint8 irm_heartbeat_enable = 0;
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_IRM_HEART_BEAT_ENABLE, &irm_heartbeat_enable);
    // 非iRM不需要增加此IP地址
    if (irm_heartbeat_enable == 0 || net_type == NET_TYPE_DEDICATED) {
        return RET_OK;
    }

    gint32 ret = dal_get_bmc_service_ipaddr(MAINT_DEDICATED_ETHERNET, agent_addr->ipv4_maint,
        sizeof(agent_addr->ipv4_maint), NULL, 0);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get maint dedicated ethernet failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    debug_log(DLOG_INFO, "%s: add %s to snmp listen list", __FUNCTION__, agent_addr->ipv4_maint);
    return RET_OK;
}


LOCAL void print_snmp_agent_addr_str(gint32 port, const gchar *key, gchar *addr_str, guint32 str_len)
{
    gint32 ret;
    gchar udp_addr[SNMP_CONF_MAXLINE_LEN] = { 0 };
    gchar udp6_addr[SNMP_CONF_MAXLINE_LEN] = { 0 };
    guchar software_type = 0;
    guchar as_status = ASM_ACTIVE;
    SNMP_AGENT_ADDR_S agent_addr;
    guchar host_enable = 0;

    if (addr_str == NULL || str_len <= 1 || key == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid parameters", __FUNCTION__);
        return;
    }

    (void)memset_s(&agent_addr, sizeof(SNMP_AGENT_ADDR_S), 0, sizeof(SNMP_AGENT_ADDR_S));
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_X86, &host_enable);

    ret = dal_get_bmc_service_ipaddr(OUTTER_GROUP_TYPE, agent_addr.ipv4_out, sizeof(agent_addr.ipv4_out),
        agent_addr.ipv6_out, sizeof(agent_addr.ipv6_out));
    if (host_enable == ENABLE) {
        ret += dal_get_inband_net_service_ipaddr(agent_addr.ipv4_bma, sizeof(agent_addr.ipv4_bma), agent_addr.ipv6_bma,
            sizeof(agent_addr.ipv6_bma));
    }

    ret += __get_ipv4_maint(&agent_addr);

    // 获取本地链路地址
    get_local_link_addr_and_ethname(agent_addr.ip_local_link, sizeof(agent_addr.ip_local_link), agent_addr.eth_name,
        sizeof(agent_addr.eth_name));

    // 获取HMM浮动IP
    (void)dal_get_software_type(&software_type);
    (void)get_board_active_state(&as_status);
    // 只有HMM的主用板才监听浮动IP
    if (software_type == MGMT_SOFTWARE_TYPE_EM && as_status == ASM_ACTIVE) {
        ret += dal_get_bmc_service_ipaddr(FLOAT_GROUP_TYPE, agent_addr.ipv4_float, sizeof(agent_addr.ipv4_float),
            agent_addr.ipv6_float, sizeof(agent_addr.ipv6_float));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:get HMM float ip failed", __FUNCTION__);
        }
    }

    // 如果获取服务地址失败或管理网口ipv4/ipv6均获取为空则监听任意ip
    if (ret != RET_OK || ((strlen(agent_addr.ipv4_out) + strlen(agent_addr.ipv6_out)) == 0)) {
        ret = snprintf_s(addr_str, str_len, str_len - 1, "%s udp:%d,udp6:%d\n", key, port, port);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s:snprintf_s failed, ret:%d", __FUNCTION__, ret);
        }
        return;
    }

    agent_addr.port = port;
    // 打印ipv4监听地址字符串
    print_snmp_udp_addr_str(&agent_addr, udp_addr, sizeof(udp_addr));

    // 打印ipv6监听地址字符串
    print_snmp_udp6_addr_str(&agent_addr, udp6_addr, sizeof(udp6_addr));

    // 拼接完整的服务监听地址
    if (strlen(udp_addr) == 0 || strlen(udp6_addr) == 0) {
        ret =
            snprintf_s(addr_str, str_len, str_len - 1, "%s %s\n", key, (strlen(udp_addr) == 0) ? udp6_addr : udp_addr);
    } else {
        ret = snprintf_s(addr_str, str_len, str_len - 1, "%s %s,%s\n", key, udp_addr, udp6_addr);
    }

    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s:snprintf_s failed, ret:%d", __FUNCTION__, ret);
    }
}


gint32 snmp_modify_port(gint32 port_id)
{
    gint32 ret = 0;
    FM_FILE_S *fp = NULL;
    gint32 p_start = 0;
    gint32 old_buf_len = 0;
    gint32 new_buf_len = 0;
    gchar key[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };

    
    (void)snprintf_s(key, sizeof(key), sizeof(key) - 1, "%s", "agentaddress");
    

    g_mutex_lock(&g_snmp_mutex);
    fp = fm_fopen(SNMP_SHORT_FILE_NAME, "r+");
    if (fp == NULL) {
        g_mutex_unlock(&g_snmp_mutex);
        debug_log(DLOG_ERROR, "%s:%d:fail to open %s\n", __FUNCTION__, __LINE__, SNMP_SHORT_FILE_NAME);
        return RET_ERR;
    }

    p_start = fm_ftell(fp);

    
    while (fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, fp) != NULL) {
        if (strstr(buf, key) != NULL) {
            old_buf_len = strlen(buf); // 用户信息长度

            
            (void)memset_s(buf, SNMP_CONF_MAXLINE_LEN + 1, 0, SNMP_CONF_MAXLINE_LEN + 1);
            
            print_snmp_agent_addr_str(port_id, (const gchar *)key, buf, sizeof(buf));
            new_buf_len = strlen(buf);
            if (new_buf_len == 0) {
                debug_log(DLOG_ERROR, "%s: print_snmp_agent_addr failed", __FUNCTION__);
            }
            

            ret = fm_modify(buf, (gsize)new_buf_len, (gsize)p_start, (gsize)old_buf_len, fp);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "failed to call fm_modify!\n");
                fm_fclose(fp);
                g_mutex_unlock(&g_snmp_mutex);
                return RET_ERR;
            }

            fm_fclose(fp);
            g_mutex_unlock(&g_snmp_mutex);
            return RET_OK;
        }

        p_start = fm_ftell(fp);
    }

    
    fm_fclose(fp);
    g_mutex_unlock(&g_snmp_mutex);
    return RET_ERR;
}


gint32 SetSnmpPortID(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret = 0;
    gint32 PortID = 0;
    GVariant *property_value = NULL;
    guchar SNMPEnable = 0;
    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Input_list can not be NULL.\r\n");
        return RET_ERR;
    }
    

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_SNMPD, &SNMPEnable);
    
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "%s, %d: get_SnmpdEnable_status fail.\n", __FUNCTION__, __LINE__);
        method_operation_log(caller_info, NULL, "Set SNMP Agent service port failed");
        return ret;
    }
    if (SNMPEnable == SERVICE_STATE_DISABLE) {
        method_operation_log(caller_info, NULL, "Set SNMP Agent service port failed");
        return COMP_CODE_INVALID_CMD;
    }
    
    

    PortID = g_variant_get_int32((GVariant *)g_slist_nth_data(input_list, 0));
    if (PortID < 1 || PortID > 65535) {
        debug_log(DLOG_ERROR, "Input parameter error!\r\n");
        method_operation_log(caller_info, NULL, "Set SNMP Agent service port failed\r\n");
        return RET_ERR;
    }

    
    ret = dfl_get_property_value(object_handle, PROPERTY_SNMP_PORTID, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:dfl_get_property_value failed", __func__);
        return RET_ERR;
    }

    gint32 current_port = g_variant_get_int32(property_value);
    g_variant_unref(property_value);

    if (current_port == PortID) {
        method_operation_log(caller_info, NULL, "Set SNMP Agent service port to (%d) successfully\r\n", PortID);
        return RET_OK;
    }

    ret = snmp_port_check(PortID);
    if (ret == FALSE) {
        debug_log(DLOG_ERROR, "%s:%d:snmp_port_check failed\n", __FUNCTION__, __LINE__);
        method_operation_log(caller_info, NULL, "Set SNMP Agent service port failed\r\n");
        return RET_ERR;
    }

    
    ret = snmp_modify_port(PortID);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Modify Snmp port(%d) failed!\r\n", PortID);
        method_operation_log(caller_info, NULL, "Set SNMP Agent service port failed\r\n");
        return RET_ERR;
    }

    property_value = g_variant_new_int32(PortID);
    ret = dfl_set_property_value(object_handle, PROPERTY_SNMP_PORTID, property_value, DF_SAVE);
    g_variant_unref(property_value);

    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set Snmp port id(%d) failed!\r\n", PortID);
        method_operation_log(caller_info, NULL, "Set SNMP Agent service port failed\r\n");
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL, "Set SNMP Agent service port to (%d) successfully\r\n", PortID);
    return RET_OK;
}

gint32 SetSnmpAuthProtocol(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    gint32 auth_protocol = 0;
    GVariant *property_value = NULL;

    auth_protocol = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if ((auth_protocol != USE_SHA1) && (auth_protocol != USE_SHA) && (auth_protocol != USE_MD5) &&
        (auth_protocol != USE_SHA256)) {
        debug_log(DLOG_ERROR, "Input parameter error!\r\n");
        method_operation_log(caller_info, NULL, "Set SNMP authentication protocol failed\r\n");
        return RET_ERR;
    }

    property_value = g_variant_new_byte(auth_protocol);
    ret = dfl_set_property_value(object_handle, PROPERTY_SNMP_AUTHPROTOCOL, property_value, DF_SAVE);
    g_variant_unref(property_value);

    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set Snmp authenticaion protocol failed!\r\n");
        method_operation_log(caller_info, NULL, "Set SNMP authentication protocol failed\r\n");
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL, "Set SNMP authentication protocol to %s successfully\r\n",
        auth_protocol_str[auth_protocol]);
    return RET_OK;
}

gint32 SetSnmpPrivProtocol(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    gint32 priv_protocol = 0;
    GVariant *property_value = NULL;

    priv_protocol = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if (priv_protocol != USE_DES && priv_protocol != USE_AES) {
        debug_log(DLOG_ERROR, "Input parameter error!\r\n");
        method_operation_log(caller_info, NULL, "Set SNMP privacy protocol failed\r\n");
        return RET_ERR;
    }

    property_value = g_variant_new_byte(priv_protocol);
    ret = dfl_set_property_value(object_handle, PROPERTY_SNMP_PRIVPROTOCOL, property_value, DF_SAVE);
    g_variant_unref(property_value);

    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set Snmp privacy protocol failed!\r\n");
        method_operation_log(caller_info, NULL, "Set SNMP privacy protocol failed\r\n");
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL, "Set SNMP privacy protocol to %s successfully\r\n",
        priv_protocol_str[priv_protocol]);
    return RET_OK;
}


gint32 SetLongPasswordEnable(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    guint8 is_open = 0;

    if ((input_list == NULL) || (caller_info == NULL)) {
        debug_log(DLOG_ERROR, "%s:parameter error", __func__);
        method_operation_log(caller_info, NULL, "Set long password enable failed");
        return RET_ERR;
    }

    is_open = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if ((is_open != 0) && (is_open != 1)) {
        debug_log(DLOG_ERROR, "%s:parameter error", __func__);
        method_operation_log(caller_info, NULL, "Set long password enable failed");
        return RET_ERR;
    }

    ret = dal_set_property_value_byte(object_handle, PROPERTY_SNMP_LONG_PASSWORD_ENABLE, is_open, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s：set %s property %s failed, return %d", __func__, dfl_get_object_name(object_handle),
            PROPERTY_SNMP_LONG_PASSWORD_ENABLE, ret);
        method_operation_log(caller_info, NULL, "Set long password enable failed");
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL, "%s long password enable successfully", (is_open ? "Open" : "Close"));

    return RET_OK;
}


LOCAL void snmp_sync_rw_community(guchar state, const gchar *key)
{
    gint32 iRet = -1;

    gint32 ret = 0;
    FM_FILE_S *fp = NULL;
    gint32 p_start = 0;
    gint32 old_buf_len = 0;
    gint32 new_buf_len = 0;

    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar new_buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };

    g_mutex_lock(&g_snmp_mutex);
    fp = fm_fopen(SNMP_SHORT_FILE_NAME, "r+");
    if (fp == NULL) {
        g_mutex_unlock(&g_snmp_mutex);
        debug_log(DLOG_ERROR, "%s:%d:fail to open %s\n", __FUNCTION__, __LINE__, SNMP_SHORT_FILE_NAME);
        return;
    }

    p_start = fm_ftell(fp);

    
    while (fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, fp) != NULL) {
        if (strstr(buf, key) != NULL) {
            old_buf_len = strlen(buf);
            if (state == ENABLED && buf[0] == '#') {
                
                iRet = snprintf_s(new_buf, sizeof(new_buf), sizeof(new_buf) - 1, "%s", (buf + 1));
                if (iRet <= 0) {
                    debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
                }
            } else if (state == DISABLED && buf[0] != '#') {
                
                iRet = snprintf_s(new_buf, sizeof(new_buf), sizeof(new_buf) - 1, "%c%s", '#', buf);
                if (iRet <= 0) {
                    debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
                }
            } else {
                
                fm_fclose(fp);
                g_mutex_unlock(&g_snmp_mutex);
                return;
            }

            new_buf_len = strlen(new_buf);
            ret = fm_modify(new_buf, (gsize)new_buf_len, (gsize)p_start, (gsize)old_buf_len, fp);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Call fm_modify failed!\n");
            }

            
            fm_fclose(fp);
            g_mutex_unlock(&g_snmp_mutex);
            return;
        }

        (void)memset_s(buf, SNMP_CONF_MAXLINE_LEN, 0, SNMP_CONF_MAXLINE_LEN);

        p_start = fm_ftell(fp);
    }

    
    fm_fclose(fp);
    g_mutex_unlock(&g_snmp_mutex);
    return;
    
}


gint32 SetSnmpRWCommunityState(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 retv = RET_OK;
    guchar state = 0;
    gchar key_ipv4[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar key_ipv6[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };

    GVariant *list_value = NULL;

    
    if (input_list == NULL || caller_info == NULL) {
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __FUNCTION__);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set RWCommunityState failed");
        return RET_ERR;
    }

    
    list_value = (GVariant *)g_slist_nth_data(input_list, 0);
    if (list_value == NULL) {
        debug_log(DLOG_ERROR, "%s:list_value is NULL", __FUNCTION__);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set RWCommunityState failed");
        return RET_ERR;
    }

    state = g_variant_get_byte(list_value);
    if (state != ENABLE && state != DISABLED) {
        debug_log(DLOG_ERROR, "%s:list_value is NULL", __FUNCTION__);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set RWCommunityState failed");
        return RET_ERR;
    }

    
    retv = dal_set_property_value_byte(obj_handle, PROPERTY_SNMP_RW_COMMUNITY_STATE, state, DF_SAVE);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "%s:dal_set_property_value_byte error", __FUNCTION__);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "%s SNMP RWCommunity failed", state ? "Enable" : "Disable");
        return RET_ERR;
    }

    
    (void)snprintf_s(key_ipv4, sizeof(key_ipv4), sizeof(key_ipv4) - 1, "%s", "com2sec rwcomm default");
    (void)snprintf_s(key_ipv6, sizeof(key_ipv6), sizeof(key_ipv6) - 1, "%s", "com2sec6 rwcomm default");
    (void)snmp_sync_rw_community(state, COM2SEC_RWCOMM_DEFAULT);
    (void)snmp_sync_rw_community(state, COM2SEC6_RWCOMM_DEFAULT);

    retv = RebootSnmp(obj_handle, NULL, NULL, NULL);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Reboot snmp failed. ", __FUNCTION__);
    }

    if (retv == RET_OK) {
        method_operation_log(caller_info, MODULE_NAME_SNMP, "%s SNMP RWCommunity successfully",
            state ? "Enable" : "Disable");
    } else {
        method_operation_log(caller_info, MODULE_NAME_SNMP, "%s SNMP RWCommunity failed", state ? "Enable" : "Disable");
    }

    return retv;
}


LOCAL gint32 __snmp_set_v1v2c_permit_id(const OBJ_HANDLE object_handle, guint8 permit_id, GSList *caller_info)
{
    guint8 old_permit_id = 0;
    GVariant *property_value = NULL;
    gint32 ret;
    
    guint32 i;
    

    if (permit_id > 7) {
        debug_log(DLOG_ERROR, "%s:input_list is out of range", __func__);
        return 1;
    }

    
    ret = dfl_get_property_value(object_handle, PROPERTY_SNMP_V1V2C_PERMIT, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get obj:%s property:%s failed!\r\n", dfl_get_object_name(object_handle),
            PROPERTY_SNMP_V1V2C_PERMIT);
        return 1;
    }

    old_permit_id = g_variant_get_byte(property_value);
    g_variant_unref(property_value);

    
    property_value = g_variant_new_byte(permit_id);
    ret = dfl_set_property_value(object_handle, PROPERTY_SNMP_V1V2C_PERMIT, property_value, DF_SAVE);
    g_variant_unref(property_value);

    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s set_property_value failed obj_name %s, property:PermitRuleIds", __func__,
            dfl_get_object_name(object_handle));
        return 1;
    }

    
    for (i = 0; i < MAX_RULE_COUNT; i++) {
        if ((1 & ((permit_id ^ old_permit_id) >> i)) == 1) {
            method_operation_log(caller_info, NULL, "%s snmp v1v2c login rule (%d) successfully\n",
                ((1 & (permit_id >> i)) == 1) ? "Enable" : "Disable", i + 1);
        }
    }

    return 0;
}


gint32 snmp_method_set_permit_rule(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint8 permit_rule_ids;
    gint32 ret;

    
    if (output_list == NULL) {
        debug_log(DLOG_ERROR, "Set snmp login rule failed, output_list can not be NULL.\r\n");
        return RET_ERR;
    }
    
    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __func__);
        method_operation_log(caller_info, NULL, "Set enabled snmpv1v2c login rule id failed");
        *output_list = g_slist_append(*output_list, g_variant_new_int32(1));
        ;
        return 1;
    }

    permit_rule_ids = (guint8)g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    ret = __snmp_set_v1v2c_permit_id(object_handle, permit_rule_ids, caller_info);
    if (ret != 0) {
        debug_log(DLOG_ERROR, "method set permit_rule_ids:%d ret:%d fail!\r\n", permit_rule_ids, ret);
        method_operation_log(caller_info, NULL, "Set enabled snmpv1v2c login rule id failed");
        *output_list = g_slist_append(*output_list, g_variant_new_int32(1));
    } else {
        
        *output_list = g_slist_append(*output_list, g_variant_new_int32(0));
    }

    
    return 0;
}



gint32 SetSnmpStadardSysContact(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    errno_t securec_rv;
    gint32 retv;
    const gchar *buf = NULL;
    guint32 buf_len = 0;
    gsize parm_temp = 0;
    GVariant *list_value = NULL;
    gchar sys_contact[SNMP_STANDARD_SYS_CONTACT_MAX_LEN + 1] = { 0 };

    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __FUNCTION__);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set systemContact failed");
        return RET_ERR;
    }

    
    list_value = (GVariant *)g_slist_nth_data(input_list, 0);
    if (list_value == NULL) {
        debug_log(DLOG_ERROR, "%s:list_value is NULL", __FUNCTION__);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set systemContact failed");
        return RET_ERR;
    }

    buf = g_variant_get_string(list_value, &parm_temp);
    buf_len = (guint32)parm_temp;

    
    if (buf == NULL) {
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __FUNCTION__);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set SystemContact failed");
        return RET_ERR;
    }
    

    if (buf_len > SNMP_STANDARD_SYS_CONTACT_MAX_LEN) {
        debug_log(DLOG_ERROR, "%s:input parameter range error:buf_len is %d", __FUNCTION__, buf_len);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set systemContact failed");
        return ERR_EXCEED_MAX_LEN;
    }

    (void)memset_s(sys_contact, sizeof(sys_contact), 0, sizeof(sys_contact));
    securec_rv = strncpy_s(sys_contact, sizeof(sys_contact), buf, buf_len);
    if (securec_rv != EOK) {
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set SystemContact failed");
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        return RET_ERR;
    }

    
    retv = dal_check_string_is_valid_ascii((const gchar *)sys_contact);
    if (retv != RET_OK) {
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set SystemContact failed");
        debug_log(DLOG_ERROR, "%s: check string is valid ascii fail, ret = %d\n", __FUNCTION__, retv);
        return RFERR_WRONG_PARAM;
    }

    retv = dal_set_property_value_string(obj_handle, PROPERTY_SNMP_SYS_CONTACT, sys_contact, DF_SAVE);
    if (retv == RET_OK) {
        
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set systemContact successfully");
        
    } else {
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set systemContact failed");
    }

    return retv;
}

gint32 SetSnmpStadardSysName(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    errno_t safe_fun_ret = EOK;
    gint32 retv = RET_OK;
    const gchar *buf = NULL;
    guint32 buf_len = 0;
    gsize parm_temp = 0;
    GVariant *list_value = NULL;
    gchar sys_name[SNMP_STANDARD_SYS_NAME_MAX_LEN + 1] = { 0 };

    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __FUNCTION__);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set sysName failed");
        return RET_ERR;
    }

    
    list_value = (GVariant *)g_slist_nth_data(input_list, 0);
    if (list_value == NULL) {
        debug_log(DLOG_ERROR, "%s:list_value is NULL", __FUNCTION__);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set sysName failed");
        return RET_ERR;
    }

    buf = g_variant_get_string(list_value, &parm_temp);
    buf_len = (guint32)parm_temp;

    if (buf == NULL) {
        debug_log(DLOG_ERROR, "%s:list_value is NULL", __FUNCTION__);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set sysName failed");
        return RET_ERR;
    }

    if (buf_len > SNMP_STANDARD_SYS_NAME_MAX_LEN) {
        debug_log(DLOG_ERROR, "%s:input parameter range error:buf_len is %d", __FUNCTION__, buf_len);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set sysName failed");
        return RET_ERR;
    }

    (void)memset_s(sys_name, sizeof(sys_name), 0, sizeof(sys_name));
    safe_fun_ret = strncpy_s(sys_name, sizeof(sys_name), buf, buf_len);
    if (safe_fun_ret != EOK) {
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set sysName failed");
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    retv = dal_set_property_value_string(obj_handle, PROPERTY_SNMP_SYS_NAME, sys_name, DF_SAVE);
    if (retv == RET_OK) {
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set sysName to (%s) successfully", sys_name);
    } else {
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set sysName failed");
    }

    return retv;
}


gint32 SetSnmpStadardSysLocation(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    errno_t securec_rv;
    gint32 retv;
    const gchar *buf = NULL;
    guint32 buf_len;
    gsize parm_temp = 0;
    GVariant *list_value = NULL;
    gchar sys_location[SNMP_STANDARD_SYS_LOCATION_MAX_LEN + 1] = { 0 };

    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __FUNCTION__);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set sysLocation failed");
        return RET_ERR;
    }

    
    list_value = (GVariant *)g_slist_nth_data(input_list, 0);
    if (list_value == NULL) {
        debug_log(DLOG_ERROR, "%s:list_value is NULL", __FUNCTION__);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set sysLocation failed");
        return RET_ERR;
    }

    buf = g_variant_get_string(list_value, &parm_temp);
    buf_len = (guint32)parm_temp;

    
    if (buf == NULL) {
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __FUNCTION__);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set sysLocation failed");
        return RET_ERR;
    }
    

    if (buf_len > SNMP_STANDARD_SYS_LOCATION_MAX_LEN) {
        debug_log(DLOG_ERROR, "%s:input parameter range error:buf_len is %d", __FUNCTION__, buf_len);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set sysLocation failed");
        return ERR_EXCEED_MAX_LEN;
    }

    (void)memset_s(sys_location, sizeof(sys_location), 0, sizeof(sys_location));
    securec_rv = strncpy_s(sys_location, sizeof(sys_location), buf, buf_len);
    if (securec_rv != EOK) {
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set sysLocation failed");
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        return RET_ERR;
    }

    
    retv = dal_check_string_is_valid_ascii((const gchar *)sys_location);
    if (retv != RET_OK) {
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set sysLocation failed");
        debug_log(DLOG_ERROR, "%s: check string is valid ascii fail, ret = %d\n", __FUNCTION__, retv);
        return RFERR_WRONG_PARAM;
    }

    retv = dal_set_property_value_string(obj_handle, PROPERTY_SNMP_SYS_LOCATION, sys_location, DF_SAVE);
    if (retv == RET_OK) {
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set sysLocation to (%s) successfully", sys_location);
    } else {
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set sysLocation failed");
    }

    return retv;
}



gint32 SetIsUpdateEngineID(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret = 0;
    OBJ_HANDLE obj_snmp = 0;
    GVariant *list_value = NULL;
    guchar state = 0;

    const guchar UPDATE_ENGINEID = 1;
    const guchar DO_NOT_UPDATE_ENGINEID = 0;

    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __FUNCTION__);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set the SNMP engine ID update failed");
        return RET_ERR;
    }

    
    list_value = (GVariant *)g_slist_nth_data(input_list, 0);
    if (list_value == NULL) {
        debug_log(DLOG_ERROR, "%s:list_value is NULL", __FUNCTION__);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set the SNMP engine ID update failed");
        return RET_ERR;
    }

    state = g_variant_get_byte(list_value);
    if (state != UPDATE_ENGINEID && state != DO_NOT_UPDATE_ENGINEID) {
        debug_log(DLOG_ERROR, "Input state(%d) error!", state);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set the SNMP engine ID update failed");
        return RET_ERR;
    }

    ret = dal_get_object_handle_nth(CLASS_SNMP, 0, &obj_snmp);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get snmp obj failed(ret = %d).", __FUNCTION__, ret);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "%s the SNMP engine ID update failed",
            state ? "Enabled" : "Disabled");
        return RET_ERR;
    }

    // 掉电持久化保存设置的值
    ret = dal_set_property_value_byte(obj_snmp, PROPERTY_SNMP_IS_UPDATE_ENGINEID, state, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set property failed(ret = %d).", __FUNCTION__, ret);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "%s the SNMP engine ID update failed",
            state ? "Enabled" : "Disabled");
        return RET_ERR;
    }

    method_operation_log(caller_info, MODULE_NAME_SNMP, "%s the SNMP engine ID update successfully",
        state ? "Enabled" : "Disabled");

    return RET_OK;
}


gint32 SetHostNameSyncFlag(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    GVariant *list_value = NULL;
    guchar cur_flag = 0;
    guchar flag = 0;
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMC"};

    
    if (input_list == NULL || caller_info == NULL) {
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __FUNCTION__);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set sysName synchronize from HostName failed.");
        return RET_ERR;
    }

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMC_NAME, sizeof(custom_name)) != RET_OK) {
        method_operation_log(caller_info, MODULE_NAME_SNMP, "dal_get_custom_string_name failed.");
    }

    
    list_value = (GVariant *)g_slist_nth_data(input_list, 0);
    if (list_value == NULL) {
        debug_log(DLOG_ERROR, "%s:list_value is NULL", __FUNCTION__);
        method_operation_log(caller_info, MODULE_NAME_SNMP,
            "Set sysName synchronize from HostName failed, invalid parameter.");
        return RET_ERR;
    }
    flag = g_variant_get_byte(list_value);
    if (flag != ENABLE && flag != DISABLED) {
        debug_log(DLOG_ERROR, "%s:IsSyncHostName flag %d is invalid", __FUNCTION__, flag);
        method_operation_log(caller_info, MODULE_NAME_SNMP,
            "Set sysName synchronize from HostName failed, invalid parameter.");
        return RET_ERR;
    }

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SNMP_HOSTNAME_SYNC_FLAG, &cur_flag);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Get IsSyncHostName failed, Ret %d", __FUNCTION__, ret);
        method_operation_log(caller_info, MODULE_NAME_SNMP, "Set sysName synchronize from HostName %s failed.",
            flag ? "Enable" : "Disable");
        return RET_ERR;
    }

    
    if (cur_flag != flag) {
        
        if (flag == DISABLE) {
            ret = dal_set_property_value_string(obj_handle, PROPERTY_SNMP_SYS_NAME, DEFAULT_SYSNAME, DF_SAVE);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s set sysName to default(%s) failed", __FUNCTION__, custom_name);
            }
        }
        ret = dal_set_property_value_byte(obj_handle, PROPERTY_SNMP_HOSTNAME_SYNC_FLAG, flag, DF_SAVE);
    }
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Set IsSyncHostName %s failed, Ret %d", __FUNCTION__, flag ? "Enable" : "Disable",
            ret);
    }
    method_operation_log(caller_info, MODULE_NAME_SNMP, "Set sysName synchronize from HostName %s %s.",
        flag ? "Enable" : "Disable", (ret != VOS_OK) ? "failed" : "successfully");

    return ret;
}


LOCAL gint32 snmp_modify_community_entry(const gchar *property, const gchar *ct)
{
    gint32 ret;
    if (g_strcmp0(property, PROPERTY_SNMP_ROCOMMUNITY) == 0) {
        ret = snmp_modify_community(SNMP_ROCOMMIUNITY_IPV4, ct);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Modify Snmp ipv4 rocommunity failed");
            return ret;
        }

        ret = snmp_modify_community(SNMP_ROCOMMIUNITY_IPV6, ct);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Modify Snmp ipv6 rocommunity failed");
            return ret;
        }
    } else {
        ret = snmp_modify_community(SNMP_RWCOMMIUNITY_IPV4, ct);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Modify Snmp ipv4 rocommunity failed");
            return ret;
        }

        ret = snmp_modify_community(SNMP_RWCOMMIUNITY_IPV6, ct);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Modify Snmp ipv6 rocommunity failed");
            return ret;
        }
    }
    return ret;
}


gint32 kmc_update_community(OBJ_HANDLE snmp_obj, const gchar *property, guint8 encrypt_type)
{
    gint32 ret;
    gchar ct[MAX_CT_LEN] = { 0 };
    gchar new_ct[MAX_CT_LEN] = { 0 };
    gsize ct_len = 0;

    if (property == NULL) {
        debug_log(DLOG_ERROR, "%s: input parm is null", __FUNCTION__);
        return RET_ERR;
    }

    
    ret =
        kmc_get_new_ciphertext_property(DOMAIN_ID_SNMP_COMM, snmp_obj, property, ct, sizeof(ct), &ct_len, encrypt_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Update ct failed, ret = %d", ret);
        goto exit;
    }

    
    ret = dal_set_property_value_string(snmp_obj, property, ct, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set property failed, ret = %d", ret);
        goto exit;
    }

    ret = dal_get_property_value_string(snmp_obj, property, new_ct, sizeof(new_ct));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get property failed, ret = %d", __FUNCTION__, ret);
        goto exit;
    }

    if (g_strcmp0(ct, new_ct) != 0) {
        debug_log(DLOG_ERROR, "%s: ct g_strcmp0 failed", __FUNCTION__);
        ret = RET_ERR;
        goto exit;
    }

    
    ret = snmp_modify_community_entry(property, (const gchar *)ct);

exit:
    clear_sensitive_info(ct, ct_len);
    clear_sensitive_info(new_ct, sizeof(new_ct));
    return ret;
}


gint32 kmc_update_snmp_comm(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    guint32 new_key_id;
    OBJ_HANDLE snmp_obj = 0;
    KmcKsfName ksf_name;

    g_mutex_lock(&g_snmpcomm_mutex);

    if (g_rollback_state == 1) {
        g_mutex_unlock(&g_snmpcomm_mutex);
        return RET_OK;
    }

    g_snmpcomm_update_state = 0;

    new_key_id = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 0));

    
    ret = dfl_get_object_handle(OBJ_NAME_SNMP, &snmp_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get Snmp obj failed, ret = %d", ret);
        goto exit;
    }

    if (dal_check_mk_update(snmp_obj, PROPERTY_SNMP_MASTERKEY_ID, new_key_id) == FALSE) {
        goto exit;
    }

    if (kmc_check_load_status() == FALSE) {
        debug_log(DLOG_ERROR, "%s: KMC not load.", __FUNCTION__);
        ret = RET_ERR;
        goto exit;
    }

    kmc_get_ksf_actual_path(&ksf_name);

    
    ret = kmc_reset(&ksf_name);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "KMC reset failed, ret = %d", ret);
        goto exit;
    }

    
    ret = kmc_update_community(snmp_obj, PROPERTY_SNMP_ROCOMMUNITY, ENCRYPT_TYPE_KMC);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Update rocommunity failed, ret = %d", ret);
        goto exit;
    }

    
    ret = kmc_update_community(snmp_obj, PROPERTY_SNMP_RWCOMMUNITY, ENCRYPT_TYPE_KMC);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Update rwcommunity failed, ret = %d", ret);
        goto exit;
    }

    
    ret = dal_set_property_value_uint32(snmp_obj, PROPERTY_SNMP_MASTERKEY_ID, new_key_id, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set masterkey id failed, ret = %d", ret);
    }

exit:
    g_snmpcomm_update_state = 1;
    g_mutex_unlock(&g_snmpcomm_mutex);
    return ret;
}


gint32 snmp_config_sync_handler(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 diff_count = 0;
    gint32 ret = 0;

    diff_count = property_sync_common_handler(obj_handle, caller_info, input_list, output_list);
    if (diff_count > 0) {
        ret = snmp_configuration_init();
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:%d:SNMP configuration init failed!\n", __FUNCTION__, __LINE__);
        }
    }

    return diff_count;
}