

#include "redfish_provider.h"

#define TMP_OPERATELOG_PATH "/tmp/operate_log_download"
#define TMP_OPERATELOG_BAK_PATH "/tmp/md_so_operate_log"
#define TMP_RUNLOG_PATH "/tmp/strategy_log_download"
#define TMP_SECURITYLOG_PATH "/tmp/security_log_download"
#define LOG_SERVICE_NUM 3

extern gint32 get_logservice_operatelog_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
extern gint32 get_logservice_runlog_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
extern gint32 get_logservice_securitylog_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gchar* g_manager_logcollections[LOG_SERVICE_NUM] = {
    RFPROP_MANAGER_LOGSERVICE_COLLECTION_OPERATELOG,
    RFPROP_MANAGER_LOGSERVICE_COLLECTION_RUNLOG,
    RFPROP_MANAGER_LOGSERVICE_COLLECTION_SECURITYLOG
};

LOCAL gint32 get_logservice_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    *o_result_jso = json_object_new_boolean(1);

    return HTTP_OK;
}


LOCAL gint32 get_logservice_export_act(json_object **o_result_jso, const gchar *id_tpl, const gchar *target_tpl,
    const gchar *action_tpl)
{
    int iRet;
    gint32 ret;
    json_object *oem = NULL;
    json_object *huawei = NULL;
    json_object *export_json = NULL;
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    gchar uri_str[MAX_CHARACTER_NUM] = {0};

    return_val_if_expr((o_result_jso == NULL || id_tpl == NULL || target_tpl == NULL || action_tpl == NULL),
        HTTP_INTERNAL_SERVER_ERROR);

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s redfish_get_board_slot fail.", __FUNCTION__));

    *o_result_jso = json_object_new_object();
    return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    oem = json_object_new_object();
    return_val_if_expr(NULL == oem, HTTP_INTERNAL_SERVER_ERROR);
    json_object_object_add(*o_result_jso, RFPROP_COMMON_OEM, oem);

    huawei = json_object_new_object();
    return_val_if_expr(NULL == huawei, HTTP_INTERNAL_SERVER_ERROR);
    json_object_object_add(oem, RFPROP_COMMON_HUAWEI, huawei);

    export_json = json_object_new_object();
    return_val_if_expr(NULL == export_json, HTTP_INTERNAL_SERVER_ERROR);
    json_object_object_add(huawei, id_tpl, export_json);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    iRet = snprintf_s(uri_str, MAX_CHARACTER_NUM, MAX_CHARACTER_NUM - 1, target_tpl, slot_id);
#pragma GCC diagnostic pop
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    json_object_object_add(export_json, RFPROP_TARGET, json_object_new_string(uri_str));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    iRet = snprintf_s(uri_str, MAX_CHARACTER_NUM, MAX_CHARACTER_NUM - 1, action_tpl, slot_id);
#pragma GCC diagnostic pop
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    json_object_object_add(export_json, RFPROP_ACTION_INFO, json_object_new_string(uri_str));

    return HTTP_OK;
}

LOCAL gint32 get_manager_logservice_collection_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_universal_provider_odata_info(i_paras, o_message_jso, o_result_jso,
        RFPROP_MANAGER_LOGSERVICE_COLLECTION_ODATA_ID);
}

LOCAL gint32 get_manager_logservice_collection_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_universal_provider_odata_info(i_paras, o_message_jso, o_result_jso,
        RFPROP_MANAGER_LOGSERVICE_COLLECTION_CONTEXT);
}

LOCAL gint32 get_manager_logservice_collection_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    *o_result_jso = json_object_new_int(LOG_SERVICE_NUM);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_manager_logservice_collection_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet = -1;
    gint32 i;
    gint32 ret;
    gchar odata_id[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_NAME_LEN] = {0};
    json_object *odata_id_json = NULL;
    json_object *mem_id = NULL;

    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_array fail.", __FUNCTION__));

    for (i = 0; i < LOG_SERVICE_NUM; i++) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
        iRet = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, g_manager_logcollections[i], slot_id);
#pragma GCC diagnostic pop

        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

        odata_id_json = json_object_new_object();
        continue_if_expr(NULL == odata_id_json);

        mem_id = json_object_new_string(odata_id);
        continue_do_info_if_expr(NULL == mem_id, json_object_put(odata_id_json));

        (void)json_object_object_add(odata_id_json, RFPROP_ODATA_ID, mem_id);
        (void)json_object_array_add(*o_result_jso, odata_id_json);
    }

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_manager_logservice_collection_provider[] = {
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_logservice_collection_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_logservice_collection_odataid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,  get_manager_logservice_collection_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_logservice_collection_members, NULL, NULL, ETAG_FLAG_ENABLE}
};

gint32 manager_provider_logservice_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gboolean ret;

    ret = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_if_fail(ret, HTTP_NOT_FOUND);

    *prop_provider = g_manager_logservice_collection_provider;
    *count = sizeof(g_manager_logservice_collection_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}

LOCAL gint32 get_logservice_operatelog_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_universal_provider_odata_info(i_paras, o_message_jso, o_result_jso,
        RFPROP_MANAGER_LOGSERVICE_COLLECTION_OPERATELOG);
}

LOCAL gint32 get_manager_logservice_members_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_universal_provider_odata_info(i_paras, o_message_jso, o_result_jso,
        RFPROP_MANAGER_LOGSERVICE_MEMBERS_CONTEXT);
}

gint32 get_logservice_operatelog_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 log_count = 0;

    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    return_val_do_info_if_expr((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_file_lines(OPERATE_LOG_PATH, &log_count);
    
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d: Get operate log fail.\n", __FUNCTION__, __LINE__));
    
    *o_result_jso = json_object_new_int(log_count);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_int fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 get_logservice_operatelog_entries(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *entry_jso = NULL;

    *o_result_jso = json_object_new_object();
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return_val_do_info_if_expr((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_universal_provider_odata_info(i_paras, o_message_jso, &entry_jso,
        RFPROP_MANAGER_LOGSERVICE_OPERATELOG_ENTRIES);
    return_val_if_expr(ret != HTTP_OK, ret);

    json_object_object_add(*o_result_jso, RFPROP_ODATA_ID, entry_jso);

    return HTTP_OK;
}

LOCAL gint32 get_logservice_operatelog_act(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    return_val_do_info_if_expr((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return get_logservice_export_act(o_result_jso, RFPROP_LOGSERVICE_EXPORT, RFPROP_OPERATELOG_EXPORT_TARGET,
        RFPROP_OPERATELOG_EXPORT_ACTION);
}


LOCAL gint32 generate_operatelog(EXPORT_ACTION_ARGS *action_args, json_object **o_message_jso)
{
    gint32 ret;
    gchar cmd_buff[CMD_MAX_LEN] = {0};
    gchar *cmd[6];

    if (o_message_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (vos_get_file_accessible(OPERATE_LOG_BAK_PATH) == TRUE) {  
        cmd[0] = "/bin/tar";
        cmd[1] = "-zxf";
        cmd[2] = OPERATE_LOG_BAK_PATH;
        cmd[3] = "-C";
        cmd[4] = TMP_PATH_NO_SLASH;
        cmd[5] = NULL;
        ret = vos_system_s("/bin/tar", cmd);
        if (ret != VOS_OK) {   
            debug_log(DLOG_ERROR, "%s:Decompress operation log package failed, ret = %d", __FUNCTION__, ret);
            (void)vos_file_copy(TMP_OPERATELOG_PATH, OPERATE_LOG_PATH);
        } else {
            if (vos_get_file_accessible(TMP_OPERATELOG_BAK_PATH) == TRUE) {
                (void)snprintf_s(cmd_buff, sizeof(cmd_buff), sizeof(cmd_buff)-1, "/bin/cat %s %s > %s",
                    TMP_OPERATELOG_BAK_PATH, OPERATE_LOG_PATH, TMP_OPERATELOG_PATH);
                cmd[0] = "/bin/sh";
                cmd[1] = "-c";
                cmd[2] = cmd_buff;
                cmd[3] = NULL;
                cmd[4] = NULL;
                cmd[5] = NULL;
                (void)vos_system_s("/bin/sh", cmd);
                vos_rm_filepath(TMP_OPERATELOG_BAK_PATH); 
            } else {  
                (void)vos_file_copy(TMP_OPERATELOG_PATH, OPERATE_LOG_PATH);
            }
        }
    } else { 
        (void)vos_file_copy(TMP_OPERATELOG_PATH, OPERATE_LOG_PATH);
    }

    if (vos_get_file_accessible(TMP_OPERATELOG_PATH) != TRUE) {
        create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    (void)dal_set_file_owner(TMP_OPERATELOG_PATH, REDFISH_USER_UID, APPS_USER_GID);
    (void)dal_set_file_mode(TMP_OPERATELOG_PATH, 660);

    return RET_OK;
}


LOCAL gint32 act_export_operatelog(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    EXPORT_TRANSACTION_INFO_S export_info = { 0 };

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: input param error.", __FUNCTION__, __LINE__));
    export_info.file_id = FILE_ID_OPERATELOG;
    export_info.permission = USERROLE_SECURITYMGNT;
    export_info.need_del = 1;
    export_info.get_status = NULL;
    export_info.generate_file = generate_operatelog;
    (void)strncpy_s(export_info.src, sizeof(export_info.src), TMP_OPERATELOG_PATH, strlen(TMP_OPERATELOG_PATH));
    (void)strncpy_s(export_info.file_category, sizeof(export_info.file_category), FILE_CATEGORY_OPERATE_LOG,
        strlen(FILE_CATEGORY_OPERATE_LOG));

    return create_export_transaction(i_paras, o_message_jso, o_result_jso, &export_info);
}

LOCAL PROPERTY_PROVIDER_S g_logservice_operatelog_provider[] = {
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_logservice_members_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_operatelog_odataid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MAX_LOG_RECORDS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_operatelog_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LOGSERVICE_ENABLE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_enable, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LOGSERVICE_DATE_TIME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_datetime, NULL, NULL, ETAG_FLAG_DISABLE},
    {RFPROP_LOGSERVICE_TIME_OFFSET, CLASS_BMC, PROPERTY_BMC_TIMEZONE_STR, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_time_offset, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LOGSERVICE_ENTYIES_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_operatelog_entries, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_operatelog_act, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LOGSERVICE_EXPORT_URI, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_export_operatelog, ETAG_FLAG_ENABLE}
};

gint32 manager_provider_logservice_operatelog_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;

    ret = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_if_fail(ret, HTTP_NOT_FOUND);

    *prop_provider = g_logservice_operatelog_provider;
    *count = sizeof(g_logservice_operatelog_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}

LOCAL gint32 get_logservice_runlog_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_universal_provider_odata_info(i_paras, o_message_jso, o_result_jso,
        RFPROP_MANAGER_LOGSERVICE_COLLECTION_RUNLOG);
}

gint32 get_logservice_runlog_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 log_count = 0;

    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    ret = get_file_lines(RUN_LOG_PATH, &log_count);
    
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d: Get operate log fail.\n", __FUNCTION__, __LINE__));
    
    *o_result_jso = json_object_new_int(log_count);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_int fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 get_logservice_runlog_entries(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *entry_jso = NULL;

    *o_result_jso = json_object_new_object();
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    ret =
        get_universal_provider_odata_info(i_paras, o_message_jso, &entry_jso, RFPROP_MANAGER_LOGSERVICE_RUNLOG_ENTRIES);
    return_val_if_expr(ret != HTTP_OK, ret);

    json_object_object_add(*o_result_jso, RFPROP_ODATA_ID, entry_jso);

    return HTTP_OK;
}

LOCAL gint32 get_logservice_runlog_act(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    return get_logservice_export_act(o_result_jso, RFPROP_LOGSERVICE_EXPORT, RFPROP_RUNLOG_EXPORT_TARGET,
        RFPROP_RUNLOG_EXPORT_ACTION);
}


LOCAL gint32 generate_runlog(EXPORT_ACTION_ARGS *action_args, json_object **o_message_jso)
{
    return_val_if_expr(o_message_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    (void)vos_file_copy(TMP_RUNLOG_PATH, RUN_LOG_PATH);

    if (TRUE != vos_get_file_accessible(TMP_RUNLOG_PATH)) {
        create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)dal_set_file_owner(TMP_RUNLOG_PATH, REDFISH_USER_UID, APPS_USER_GID);
    (void)dal_set_file_mode(TMP_RUNLOG_PATH, 660);

    return VOS_OK;
}


LOCAL gint32 act_export_runlog(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    EXPORT_TRANSACTION_INFO_S export_info = { 0 };

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: input param error.", __FUNCTION__, __LINE__));
    export_info.file_id = FILE_ID_RUNLOG;
    export_info.permission = USERROLE_READONLY;
    export_info.need_del = 1;
    export_info.get_status = NULL;
    export_info.generate_file = generate_runlog;
    (void)strncpy_s(export_info.src, sizeof(export_info.src), TMP_RUNLOG_PATH, strlen(TMP_RUNLOG_PATH));
    (void)strncpy_s(export_info.file_category, sizeof(export_info.file_category), FILE_CATEGORY_RUN_LOG,
        strlen(FILE_CATEGORY_RUN_LOG));

    return create_export_transaction(i_paras, o_message_jso, o_result_jso, &export_info);
}

LOCAL PROPERTY_PROVIDER_S g_logservice_runlog_provider[] = {
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_logservice_members_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_runlog_odataid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MAX_LOG_RECORDS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_runlog_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LOGSERVICE_ENABLE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_enable, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LOGSERVICE_DATE_TIME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_datetime, NULL, NULL, ETAG_FLAG_DISABLE},
    {RFPROP_LOGSERVICE_TIME_OFFSET, CLASS_BMC, PROPERTY_BMC_TIMEZONE_STR, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_time_offset, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LOGSERVICE_ENTYIES_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_runlog_entries, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_runlog_act, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LOGSERVICE_EXPORT_URI, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_export_runlog, ETAG_FLAG_ENABLE}
};

gint32 manager_provider_logservice_runlog_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;

    ret = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_if_fail(ret, HTTP_NOT_FOUND);

    *prop_provider = g_logservice_runlog_provider;
    *count = sizeof(g_logservice_runlog_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}

LOCAL gint32 get_logservice_securitylog_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_universal_provider_odata_info(i_paras, o_message_jso, o_result_jso,
        RFPROP_MANAGER_LOGSERVICE_COLLECTION_SECURITYLOG);
}

gint32 get_logservice_securitylog_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 log_count = 0;

    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    return_val_do_info_if_expr((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_file_lines(SECURITY_LOG_PATH, &log_count);
    
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d: Get operate log fail.\n", __FUNCTION__, __LINE__));
    
    *o_result_jso = json_object_new_int(log_count);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_int fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 get_logservice_securitylog_entries(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *entry_jso = NULL;

    *o_result_jso = json_object_new_object();
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return_val_do_info_if_expr((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_universal_provider_odata_info(i_paras, o_message_jso, &entry_jso,
        RFPROP_MANAGER_LOGSERVICE_SECURITYLOG_ENTRIES);
    return_val_if_expr(ret != HTTP_OK, ret);

    json_object_object_add(*o_result_jso, RFPROP_ODATA_ID, entry_jso);

    return HTTP_OK;
}

LOCAL gint32 get_logservice_securitylog_act(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    return_val_do_info_if_expr((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return get_logservice_export_act(o_result_jso, RFPROP_LOGSERVICE_EXPORT, RFPROP_SECURITYLOG_EXPORT_TARGET,
        RFPROP_SECURITYLOG_EXPORT_ACTION);
}


LOCAL gint32 generate_securitylog(EXPORT_ACTION_ARGS *action_args, json_object **o_message_jso)
{
    int iRet = -1;
    gchar cmd_buff[256] = {0};
    gchar *cmd[4];

    return_val_if_expr(o_message_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    if (vos_get_file_accessible(SECURITY_LOG_BAK_PATH) == TRUE) {
        iRet = snprintf_s(cmd_buff, sizeof(cmd_buff), sizeof(cmd_buff) - 1, "/bin/cat %s %s > %s",
            SECURITY_LOG_BAK_PATH, SECURITY_LOG_PATH, TMP_SECURITYLOG_PATH);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        cmd[0] = "/bin/sh";
        cmd[1] = "-c";
        cmd[2] = cmd_buff;
        cmd[3] = NULL;
        (void)vos_system_s("/bin/sh", cmd);
    } else {
        (void)vos_file_copy(TMP_SECURITYLOG_PATH, SECURITY_LOG_PATH);
    }

    if (vos_get_file_accessible(TMP_SECURITYLOG_PATH) != TRUE) {
        create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)dal_set_file_owner(TMP_SECURITYLOG_PATH, REDFISH_USER_UID, APPS_USER_GID);
    (void)dal_set_file_mode(TMP_SECURITYLOG_PATH, 660);

    return VOS_OK;
}


LOCAL gint32 act_export_securitylog(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    EXPORT_TRANSACTION_INFO_S export_info = { 0 };

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: input param error.", __FUNCTION__, __LINE__));
    export_info.file_id = FILE_ID_SECURITYLOG;
    export_info.permission = USERROLE_SECURITYMGNT;
    export_info.need_del = 1;
    export_info.get_status = NULL;
    export_info.generate_file = generate_securitylog;
    (void)strncpy_s(export_info.src, sizeof(export_info.src), TMP_SECURITYLOG_PATH, strlen(TMP_SECURITYLOG_PATH));
    (void)strncpy_s(export_info.file_category, sizeof(export_info.file_category), FILE_CATEGORY_SECURITY_LOG,
        strlen(FILE_CATEGORY_SECURITY_LOG));

    return create_export_transaction(i_paras, o_message_jso, o_result_jso, &export_info);
}

LOCAL PROPERTY_PROVIDER_S g_logservice_securitylog_provider[] = {
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_logservice_members_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_securitylog_odataid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MAX_LOG_RECORDS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_securitylog_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LOGSERVICE_ENABLE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_enable, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LOGSERVICE_DATE_TIME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_datetime, NULL, NULL, ETAG_FLAG_DISABLE},
    {RFPROP_LOGSERVICE_TIME_OFFSET, CLASS_BMC, PROPERTY_BMC_TIMEZONE_STR, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_time_offset, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LOGSERVICE_ENTYIES_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_securitylog_entries, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_securitylog_act, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LOGSERVICE_EXPORT_URI, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_export_securitylog, ETAG_FLAG_ENABLE}
};

gint32 manager_provider_logservice_securitylog_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;

    ret = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_if_fail(ret, HTTP_NOT_FOUND);

    *prop_provider = g_logservice_securitylog_provider;
    *count = sizeof(g_logservice_securitylog_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
