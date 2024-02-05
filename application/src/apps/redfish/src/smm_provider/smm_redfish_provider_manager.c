
#include "redfish_provider.h"
#include "redfish_forward.h"
#include "dirent.h"
#include "redfish_provider_managers_payload.h"
LOCAL gint32 smm_get_manager_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_get_manager_common_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_get_manager_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 smm_get_manager_uuid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 smm_get_manager_model(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 smm_get_manager_date_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_set_manager_date_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_get_manager_timezone(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_set_manager_timezone(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_get_manager_firmware_version(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_get_manager_redundancy(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 redfish_del_event_from_monitor_foreach(OBJ_HANDLE handle, gpointer input_list);
LOCAL gint32 redfish_set_event_severity(guint8 target, const gchar *severity_str, PROVIDER_PARAS_S *i_paras);
LOCAL gint32 smm_get_manager_removed_severity(json_object **o_result_jso);
LOCAL gint32 smm_set_manager_removed_severity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object *o_result_jso);
LOCAL void change_severity_num_to_str(guint8 severity, const gchar **severity_str);
LOCAL void change_severity_str_to_num(const gchar *severity_str, guint8 *severity);
LOCAL gint32 smm_set_severity_and_supportflag(OBJ_HANDLE info_object_handle, OBJ_HANDLE plcy_object_handle,
    guint8 severity, PROVIDER_PARAS_S *i_paras);
LOCAL gint32 smm_get_manager_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_get_manager_property(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);
LOCAL gint32 smm_get_manager_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 smm_get_manager_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 smm_set_manager_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 smm_set_oem_check_hmm_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso);
extern void get_manager_oem_language_set(json_object **oem_property);
extern void get_manager_oem_permit_rule(json_object **oem_property);
extern gint32 set_manager_loginrule(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_power_on_permit(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 smm_action_manager_power_on(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 smm_action_manager_reset(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
extern gint32 act_manager_export_dump(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_action_manager_dump(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
extern gint32 act_manager_delete_language(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_act_manager_delete_language(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
extern gint32 act_manager_general_download(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_act_manager_general_download(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_act_manager_rollback(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);
LOCAL gint32 smm_action_shelf_payload_state(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);
LOCAL gint32 smm_action_manager_failover(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_action_manager_del_swi_profile(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL void smm_get_manager_reachable_smm_name(PROVIDER_PARAS_S *i_paras, gchar *smm_name, guint32 length);


LOCAL gint32 smm_action_manager_res_swi_profile(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_action_manager_par_swi_profile(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 check_swi_model(const gchar *swi_model);
LOCAL gint32 _check_action_manager_parameter(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, gint action_type);
LOCAL gint32 _find_action_manager_res_swi_profile(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    gint flag_time);


LOCAL gboolean check_swi_exist(void);

LOCAL PROPERTY_PROVIDER_S smm_manager_provider[] = {

    { RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_manager_odata_id, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_COMMON_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_manager_common_id, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_STATUS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_manager_status, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_UUID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_manager_uuid, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_MODEL, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_manager_model, NULL, NULL, ETAG_FLAG_DISABLE},
    { RFPROP_MANAGER_DATATIME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_FORBID, smm_get_manager_date_time, smm_set_manager_date_time, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_MANAGER_TIMEZONE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_FORBID, smm_get_manager_timezone, smm_set_manager_timezone, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_FIRMWARE_VERSION, CLASS_BMC, BMC_MANA_VER_NAME,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_manager_firmware_version, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_REDUNDANCY, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_manager_redundancy, NULL, NULL, ETAG_FLAG_ENABLE},
    { RFPROP_MANAGER_ACTION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_manager_actions, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_ETHERNETINTERFACES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_manager_property, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_NETWORK_PROTOCOL, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_manager_property, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_LOGSERVICES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_manager_property, NULL, NULL, ETAG_FLAG_ENABLE },
    { PRPROP_MANAGER_STMPSERVICES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_manager_property, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_LINKS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, smm_get_manager_links, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_FORBID, smm_get_manager_oem, smm_set_manager_oem, NULL, ETAG_FLAG_ENABLE},
    
    { RFPROP_MANAGER_RESET, CLASS_BMC, METHD_BMC_REBOOTPME,
        USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, NULL, NULL, smm_action_manager_reset, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_EXPORT_DUMP, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_DIAGNOSEMGNT, SYS_LOCKDOWN_ALLOW, NULL, NULL, smm_action_manager_dump, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_FORCE_FAILOVER, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_BASICSETTING, SYS_LOCKDOWN_ALLOW, NULL, NULL, smm_action_manager_failover, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_DELETE_SWI_PROFILE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, NULL, NULL, smm_action_manager_del_swi_profile, ETAG_FLAG_ENABLE },
    
    { RFPROP_MANAGER_RESTORE_SWI_PROFILE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID,  NULL, NULL, smm_action_manager_res_swi_profile, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_PARSE_SWI_PROFILE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, smm_action_manager_par_swi_profile, ETAG_FLAG_ENABLE },
    
    { RFPROP_MANAGER_POWER_ON, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, NULL, NULL, smm_action_manager_power_on, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_DELETE_LANGUAGE, CLASS_BMC, PROPERTY_BMC_LANGUAGESET,
        USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, NULL,	NULL, smm_act_manager_delete_language, ETAG_FLAG_ENABLE	},
    { RFPROP_MANAGER_GENERAL_DOWNLOAD, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_BASICSETTING, SYS_LOCKDOWN_ALLOW, NULL, NULL, smm_act_manager_general_download, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_ROLLBACK, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, NULL, NULL, smm_act_manager_rollback, ETAG_FLAG_ENABLE },
    { RFPROP_MANAGER_SHELF_POWER_CONTROL, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, NULL, NULL, smm_action_shelf_payload_state, ETAG_FLAG_ENABLE },
};
LOCAL const gchar* g_board_remove_info[] = {
    PS_REMOVEED_INFO,
    FAN_REMOVEED_INFO,
    BLADE_REMOVEED_INFO,
    SWI_REMOVEED_INFO,
    SMM_REMOVEED_INFO,
};
#define BOARD_TYPE_NUM 5
#define SEVERITY_INVALID 0xff
#define SWI_NAME_LEN 4


LOCAL void smm_get_manager_reachable_smm_name(PROVIDER_PARAS_S *i_paras, gchar *smm_name, guint32 length)
{
    guint8 as_status = ASM_STAND_BY;
    OBJ_HANDLE handle = 0;
    guint8 slot_id = 0;
    int iRet;

    if (smm_name == NULL) {
        debug_log(DLOG_ERROR, "%s: get uri failed, cause smm_name is null.", __FUNCTION__);
        return;
    }
    (void)dfl_get_object_handle(ASM_OBJECT_NAME, &handle);
    (void)dal_get_property_value_byte(handle, PROPERTY_ASM_AS_STATUS, &as_status);
    if (as_status == ASM_STAND_BY) { 
        iRet = get_other_smm_slotID(&slot_id);
        do_val_if_expr(iRet != DFL_OK,
            debug_log(DLOG_ERROR, "%s: get active slotid failed, ret = %d", __FUNCTION__, iRet));
        iRet = sprintf_s(smm_name, length, "HMM%d", slot_id - 36); 
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    } else { 
        get_smm_name_from_redfish_uri(RF_MANAGERS_URI, i_paras->uri, smm_name, length);
    }
    return;
}


LOCAL gboolean is_local_hmm_index(const gchar *prefix, const gchar *uri)
{
    guint8 smm_index = 0;
    gchar smm_name[MAX_NAME_SIZE] = { 0 };

    (void)get_smm_index(&smm_index);
    get_smm_name_from_redfish_uri(prefix, uri, smm_name, sizeof(smm_name));
    
    return (smm_name[3] - '0') == smm_index;
}


LOCAL gint32 input_parameter_check(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso)
{
    
    if (o_message_jso == NULL || i_paras == NULL || i_paras->val_jso == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if (!(i_paras->user_role_privilege & USERROLE_BASICSETTING)) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    if (is_local_hmm_index(RF_MANAGERS_URI, i_paras->uri) != TRUE) {
        create_message_info(MSGID_CURRENT_BOARD_ABSENT, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "Not match current HMM.");
        return HTTP_BAD_REQUEST;
    }
    return RET_OK;
}


LOCAL gint32 smm_check_hmm_active(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso)
{
    guint8 as_status = ASM_STAND_BY;

    
    (void)get_board_active_state(&as_status);
    if (as_status != ASM_ACTIVE) {
        create_message_info(MSGID_CURRENT_MM_NOT_ACTIVE, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "Current HMM is not active.");
        return HTTP_BAD_REQUEST;
    }

    if (is_local_hmm_index(RF_MANAGERS_URI, i_paras->uri) != TRUE) {
        create_message_info(MSGID_CURRENT_BOARD_ABSENT, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "Not match current HMM.");
        return HTTP_BAD_REQUEST;
    }
    return RET_OK;
}


LOCAL gint32 get_each_blade_power_on_control_status(json_object **o_result_jso)
{
    GSList *list = NULL;
    GSList *tmp = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint8 state = 0;
    guint8 slot_id = 0;
    gchar odata_id[MAX_URI_LEN] = { 0 };
    json_object *manager_for_blade = NULL;
    json_object *obj = NULL;

    return_val_if_expr(NULL == o_result_jso, VOS_ERR);

    (void)dfl_get_object_list(CLASS_NAME_IPMBETH_BLADE, &list);

    manager_for_blade = json_object_new_array();

    for (tmp = list; tmp; tmp = tmp->next) {
        obj_handle = (OBJ_HANDLE)tmp->data;

        if (!is_compute_node(obj_handle)) { // 非计算节点不设置受控上电
            continue;
        }
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_IPMBETH_BLADE_ID, &slot_id);
        (void)dal_get_property_value_byte(obj_handle, PROTERY_IPMBETH_BLADE_CONTROLLED_POWER_ON, &state);

        (void)snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, "/redfish/v1/Managers/blade%u", slot_id);

        obj = json_object_new_object();
        json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string(odata_id));
        json_object_object_add(obj, RFPROP_MANAGER_PWON_CONTROL_ENABLE, json_object_new_boolean(state));
        json_object_array_add(manager_for_blade, obj);
    }

    json_object_object_add(*o_result_jso, RFPROP_MANAGER_PWON_PERMIT_STATUS, manager_for_blade);

    g_slist_free(list);
    return VOS_OK;
}


LOCAL gint32 smm_get_manager_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gchar odata_id[MAX_URI_LEN] = { 0 };
    gchar smm_name[MAX_NAME_SIZE] = { 0 }; // HmmN
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    get_smm_name_from_redfish_uri(RF_MANAGERS_URI, i_paras->uri, smm_name, sizeof(smm_name));
    iRet = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, URI_FORMAT_MANAGER, smm_name);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    *o_result_jso = json_object_new_string(odata_id);
    return HTTP_OK;
}

LOCAL gint32 smm_get_manager_common_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar smm_name[MAX_NAME_SIZE] = { 0 }; // HmmN
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    get_smm_name_from_redfish_uri(RF_MANAGERS_URI, i_paras->uri, smm_name, sizeof(smm_name));
    *o_result_jso = json_object_new_string(smm_name);
    return HTTP_OK;
}

LOCAL gint32 smm_get_manager_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE handle = 0;
    gint32 ret = 0;
    guint8 smm_health = 0;
    gchar *state_str = NULL;
    guint8 as_status = 0;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    if (is_local_hmm_index(RF_MANAGERS_URI, i_paras->uri)) { // 查询为本板
        // 主备信息和健康信息
        (void)redfish_get_smm_health(&smm_health);
        // 生成状态信息
        ret = dfl_get_object_handle(ASM_OBJECT_NAME, &handle);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "[%s] Get AsmObject handle failed, err code %d", __func__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        (void)dal_get_property_value_byte(handle, PROPERTY_ASM_AS_STATUS, &as_status);
        if (as_status == ASM_ACTIVE) {
            state_str = ENABLED_STRING;
        } else {
            state_str = "StandbySpare";
        }
        get_resource_status_property(&smm_health, NULL, state_str, o_result_jso, TRUE);
    } else { // 当透传到备板失败时，会进入此处处理
        *o_result_jso = json_object_new_object();
        json_object_object_add(*o_result_jso, "State", json_object_new_string("Absent"));
        json_object_object_add(*o_result_jso, "Health", NULL);
    }
    return HTTP_OK;
}

LOCAL gint32 smm_get_manager_uuid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    if (is_local_hmm_index(RF_MANAGERS_URI, i_paras->uri)) { // 查询为本板
        return redfish_get_uuid(i_paras, o_message_jso, o_result_jso);
    }
    return HTTP_OK;
}

LOCAL gint32 smm_get_manager_model(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    if (is_local_hmm_index(RF_MANAGERS_URI, i_paras->uri)) { // 查询为本板
        *o_result_jso = json_object_new_string("HMM");
    }
    return HTTP_OK;
}

LOCAL gint32 smm_get_manager_firmware_version(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    if (is_local_hmm_index(RF_MANAGERS_URI, i_paras->uri) != TRUE) { // 查询不为本板
        return HTTP_OK;
    }
    return redfish_get_property_value_string(&smm_manager_provider[i_paras->index], i_paras, o_message_jso,
        o_result_jso);
}

LOCAL gint32 smm_get_manager_date_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint16 time_zone = 0;
    guint32 time_now = 0;
    char str_buffer[STRING_LEN_MAX] = { 0 };

    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    if (is_local_hmm_index(RF_MANAGERS_URI, i_paras->uri) != TRUE) { // 查询不为本板
        return HTTP_OK;
    }
    ret = redfish_get_format_time(i_paras, &time_now, &time_zone);

    return_val_do_info_if_expr(RFERR_INSUFFICIENT_PRIVILEGE == ret, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get datetime error", __FUNCTION__));
    ret = redfish_fill_format_time(str_buffer, sizeof(str_buffer), &time_now, &time_zone);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: fill datetime error", __FUNCTION__));
    *o_result_jso = json_object_new_string(str_buffer);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string error", __FUNCTION__));
    return HTTP_OK;
}

LOCAL gint32 smm_check_date_time(guint16 year, guint8 month, guint8 day, guint8 hour, guint8 minute, guint8 second)
{
    // 判断时间范围是否正确
    if ((MAX_HOUR_DAY < hour) || (MAX_MINUTE < minute) || (MAX_SECOND < second) ||
        (!((MONTH_DATE_MIN <= day) && (MONTH_DATE_MAX >= day))) || (!((MONTH_JAN <= month) && (MONTH_DEC >= month)))) {
        return VOS_ERR;
    }
    // 判断4,6,9,11月的天数是否正确
    if ((MONTH_APIR == month) || (MONTH_JUNE == month) || (MONTH_SEP == month) || (MONTH_NOV == month)) {
        if (MONTH_DATE_30 < day) {
            return VOS_ERR;
        }
    }
    // 判断2月份天数是否正确
    else if (MONTH_FEB == month) {
        if ((0 == ((year) % YEAR_LEAP_4)) &&
            ((0 != ((year) % YEAR_LEAP_100)) || (0 == ((year) % YEAR_LEAP_400)))) { // 闰年
            if (LEAP_MONTH_DAY < day) {
                return VOS_ERR;
            }
        } else { // 非闰年
            if (MONTH_DATE_28 < day) {
                return VOS_ERR;
            }
        }
    }
    return VOS_OK;
}

LOCAL gint32 smm_set_manager_date_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    errno_t safe_fun_ret;
    OBJ_HANDLE ntp_handle = 0;
    guchar ntp_enable = 0;
    gint32 ret;
    const gchar *time_str = NULL;
    gchar time_str_var[MAX_STRBUF_LEN] = { 0 };
    struct tm time_st = { 0 }; 
    guint16 year = 0;
    guint8 month = 0;
    guint8 day = 0;
    guint8 hour = 0;
    guint8 minute = 0;
    guint8 second = 0;
    guint32 timestamp = 0;
    gchar *cur_ptr = NULL;
    OBJ_HANDLE rtc_handle = 0;
    GSList *input_list = NULL;
    gchar *data = NULL;

    return_val_do_info_if_expr((VOS_OK != provider_paras_check(i_paras)) || (NULL == o_message_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_DATATIME, o_message_jso,
        RFPROP_MANAGER_DATATIME));
    // 检查NTP是否开启
    ret = dfl_get_object_handle(OBJECT_NTP, &ntp_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get ntp object handle failed, err code %d", __func__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    (void)dal_get_property_value_byte(ntp_handle, PROPERTY_NTP_ENABLE_STATUS, &ntp_enable);
    if (ntp_enable) {
        (void)create_message_info(MSGID_MODIFY_FAILED_WITH_NTP_MODE, RFPROP_MANAGER_DATATIME, o_message_jso,
            RFPROP_MANAGER_DATATIME);
        return HTTP_FORBIDDEN;
    }
    // 读取时间
    time_str = dal_json_object_get_str(i_paras->val_jso);
    safe_fun_ret = strncpy_s(time_str_var, sizeof(time_str_var), time_str, sizeof(time_str_var) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    // year
    data = strtok_r(time_str_var, "-", &cur_ptr);
    if (data == NULL || strlen(data) != 4) {
        goto PARAM_ERR_EXIT;
    }
    ret = vos_str2int(data, 10, &year, NUM_TPYE_USHORT);
    if (VOS_OK != ret) {
        goto PARAM_ERR_EXIT;
    }
    // month
    data = strtok_r(cur_ptr, "-", &cur_ptr);
    if (data == NULL || strlen(data) != 2) {
        goto PARAM_ERR_EXIT;
    }
    ret = vos_str2int(data, 10, &month, NUM_TPYE_UCHAR);
    if (VOS_OK != ret) {
        goto PARAM_ERR_EXIT;
    }
    // day
    data = strtok_r(cur_ptr, "T", &cur_ptr);
    if (data == NULL || strlen(data) != 2) {
        goto PARAM_ERR_EXIT;
    }
    ret = vos_str2int(data, 10, &day, NUM_TPYE_UCHAR);
    if (VOS_OK != ret) {
        goto PARAM_ERR_EXIT;
    }
    // hour
    data = strtok_r(cur_ptr, ":", &cur_ptr);
    if (data == NULL || strlen(data) != 2) {
        goto PARAM_ERR_EXIT;
    }
    ret = vos_str2int(data, 10, &hour, NUM_TPYE_UCHAR);
    if (VOS_OK != ret) {
        goto PARAM_ERR_EXIT;
    }
    // minute
    data = strtok_r(cur_ptr, ":", &cur_ptr);
    if (data == NULL || strlen(data) != 2) {
        goto PARAM_ERR_EXIT;
    }
    ret = vos_str2int(data, 10, &minute, NUM_TPYE_UCHAR);
    if (VOS_OK != ret) {
        goto PARAM_ERR_EXIT;
    }
    // second
    if (cur_ptr == NULL || strlen(cur_ptr) != 2) {
        goto PARAM_ERR_EXIT;
    }
    ret = vos_str2int(cur_ptr, 10, &second, NUM_TPYE_UCHAR);
    if (VOS_OK != ret) {
        goto PARAM_ERR_EXIT;
    }
    // 校验时间
    ret = smm_check_date_time(year, month, day, hour, minute, second);
    if (ret != VOS_OK) {
        goto PARAM_ERR_EXIT;
    }
    time_st.tm_year = year - YEAR_1900;
    time_st.tm_mon = month - 1;
    time_st.tm_mday = day;
    time_st.tm_hour = hour;
    time_st.tm_min = minute;
    time_st.tm_sec = second;
    // 设置时间
    ret = dal_get_object_handle_nth(CLASS_ON_BOARD_RTC, 0, &rtc_handle);
    if (ret != VOS_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    timestamp = (guint32)mktime(&time_st);
    input_list = g_slist_append(input_list, g_variant_new_uint32(timestamp));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, rtc_handle,
        CLASS_ON_BOARD_RTC, METHOD_SET_ON_BOARD_RTC_TIME, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Call class[%s] method[%s] failed, err code %d", __func__, CLASS_ON_BOARD_RTC,
            METHOD_SET_ON_BOARD_RTC_TIME, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
PARAM_ERR_EXIT:
    (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_DATATIME, o_message_jso, time_str,
        RFPROP_MANAGER_DATATIME);
    return HTTP_BAD_REQUEST;
}

LOCAL gint32 smm_get_manager_timezone(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar str_buffer[TIME_ZONE_STRING_LEN] = { 0 };
    OBJ_HANDLE obj_handle = 0;

    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso) && (NULL != i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));
    return_val_do_info_if_expr(0 == (i_paras->user_role_privilege & USERROLE_READONLY), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    if (is_local_hmm_index(RF_MANAGERS_URI, i_paras->uri)  != TRUE) { // 查询不为本板
        return HTTP_OK;
    }
    ret = dfl_get_object_handle(OBJECT_BMC, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get obj_handle error", __FUNCTION__));
    (void)dal_get_property_value_string(obj_handle, PROPERTY_BMC_TIMEZONE_STR, str_buffer, sizeof(str_buffer));
    *o_result_jso = json_object_new_string(str_buffer);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string error", __FUNCTION__));
    return HTTP_OK;
}

LOCAL gint32 smm_set_manager_timezone(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    errno_t safe_fun_ret;
    const gchar *timezone_str = NULL;
    gint32 ret;
    GSList *input_list = NULL;
    gchar _timezone[MAX_TIME_STRING_LEN] = { 0 };

    return_val_do_info_if_expr((VOS_OK != provider_paras_check(i_paras)) || (NULL == o_message_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_TIMEZONE, o_message_jso,
        RFPROP_MANAGER_TIMEZONE));

    timezone_str = dal_json_object_get_str(i_paras->val_jso);
    if (timezone_str[0] == '-' || timezone_str[0] == '+') { // 带符号数字偏移
        (void)strncpy_s(_timezone, sizeof(_timezone), "UTC", strlen("UTC"));
    } else if (timezone_str[0] >= '0' && timezone_str[0] <= '9') { // 数字偏移
        (void)strncpy_s(_timezone, sizeof(_timezone), "UTC+", strlen("UTC+"));
    } // else 时区名称
    safe_fun_ret = strncat_s(_timezone, sizeof(_timezone), timezone_str, strlen(timezone_str));
    do_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    input_list = g_slist_append(input_list, g_variant_new_string(_timezone));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_NULL,
        CLASS_BMC, METHOD_BMC_TIMEZONE_STR, 0, i_paras->user_role_privilege, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret == VOS_OK) {
        return HTTP_OK;
    } else if (ret == RFERR_WRONG_PARAM) {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_MANAGER_TIMEZONE, o_message_jso, timezone_str,
            RFPROP_MANAGER_TIMEZONE);
        return HTTP_BAD_REQUEST;
    }
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 smm_get_manager_redundancy(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    json_object *obj = NULL;
    json_object *obj2 = NULL;
    json_object *obj3 = NULL;
    gchar odata_id[MAX_URI_LEN] = { 0 };
    gchar smm_name[STRING_LEN_MAX] = { 0 };
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    get_smm_name_from_redfish_uri(RF_MANAGERS_URI, i_paras->uri, smm_name, sizeof(smm_name));
    *o_result_jso = json_object_new_array();
    obj = json_object_new_object();
    json_object_array_add(*o_result_jso, obj);
    iRet =
        snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, "/redfish/v1/Managers/%s#/Redundancy/0", smm_name);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string(odata_id));
    json_object_object_add(obj, MEMBERID_STRING, json_object_new_string("0"));
    json_object_object_add(obj, "Mode", json_object_new_string("Failover"));
    json_object_object_add(obj, "MaxNumSupported", json_object_new_int(2));
    json_object_object_add(obj, "MinNumNeeded", json_object_new_int(1));
    // Status
    obj2 = json_object_new_object();
    json_object_object_add(obj2, "State", json_object_new_string("Enabled"));
    json_object_object_add(obj2, "Health", json_object_new_string("OK"));
    json_object_object_add(obj, "Status", obj2);
    // RedundancySet
    obj2 = json_object_new_array();
    obj3 = json_object_new_object();
    json_object_object_add(obj3, RFPROP_ODATA_ID, json_object_new_string("/redfish/v1/Managers/HMM1"));
    json_object_array_add(obj2, obj3);
    obj3 = json_object_new_object();
    json_object_object_add(obj3, RFPROP_ODATA_ID, json_object_new_string("/redfish/v1/Managers/HMM2"));
    json_object_array_add(obj2, obj3);
    json_object_object_add(obj, "RedundancySet", obj2);
    return HTTP_OK;
}
LOCAL void change_severity_num_to_str(guint8 severity, const gchar **severity_str)
{
    switch (severity) {
        case SEVERITY_NORMAL:
            *severity_str = NORMAL_STRING;
            break;
        case SEVERITY_MINOR:
            *severity_str = MINOR_STRING;
            break;
        case SEVERITY_MAJOR:
            *severity_str = MAJOR_STRING;
            break;
        case SEVERITY_CRITICAL:
            *severity_str = CRITICAL_STRING;
            break;
        default:
            *severity_str = "Unknown";
            debug_log(DLOG_ERROR, "Unknown level.");
            break;
    }
}
LOCAL void change_severity_str_to_num(const gchar *severity_str, guint8 *severity)
{
    if (strcmp(NORMAL_STRING, severity_str) == 0) {
        *severity = SEVERITY_NORMAL;
    } else if (strcmp(MINOR_STRING, severity_str) == 0) {
        *severity = SEVERITY_MINOR;
    } else if (strcmp(MAJOR_STRING, severity_str) == 0) {
        *severity = SEVERITY_MAJOR;
    } else if (strcmp(CRITICAL_STRING, severity_str) == 0) {
        *severity = SEVERITY_CRITICAL;
    } else {
        *severity = SEVERITY_INVALID;
        debug_log(DLOG_ERROR, "Unknown level.");
    }
}
LOCAL gint32 smm_get_manager_removed_severity(json_object **o_result_jso)
{
    json_object *obj = NULL;
    gchar severity_str[BOARD_TYPE_NUM][STRING_LEN_MAX] = {0};
    errno_t safe_fun_ret = EOK;
    gint32 ret = 0;
    gint32 i;
    
    OBJ_HANDLE info_object_handle[BOARD_TYPE_NUM] = {0};
    guint8 severity = 0;
    const gchar *severity_str_tmp = NULL;

    return_val_do_info_if_fail(o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    
    for (i = 0; i < BOARD_TYPE_NUM; i++) {
        ret = dfl_get_object_handle(g_board_remove_info[i], &info_object_handle[i]); 
        continue_do_info_if_expr(ret != VOS_OK,
            debug_log(DLOG_ERROR, "get info_object_handle[%d] error. ret = %d", i, ret));

        ret = dal_get_property_value_byte(info_object_handle[i], PROPERTY_EVENT_INFORMATION_SEVERITY, &severity);
        continue_do_info_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "get Severity error. ret = %d", ret));
        change_severity_num_to_str(severity, &severity_str_tmp);
        safe_fun_ret = strcpy_s(severity_str[i], STRING_LEN_MAX, severity_str_tmp);
        do_val_if_expr(safe_fun_ret != EOK, debug_log(DLOG_ERROR, "memcpy_s fail, ret = %d", safe_fun_ret));
    }
    obj = json_object_new_object();
    json_object_object_add(obj, "PSU", json_object_new_string(severity_str[0]));
    json_object_object_add(obj, "Fan", json_object_new_string(severity_str[1])); 
    json_object_object_add(obj, "Blade", json_object_new_string(severity_str[2])); 
    if (check_swi_exist() == TRUE) {
        json_object_object_add(obj, "Swi", json_object_new_string(severity_str[3])); 
    }
    json_object_object_add(obj, "SMM", json_object_new_string(severity_str[4])); 

    json_object_object_add(*o_result_jso, RFPROP_MANAGER_REMOVED_SEVERITY, obj);

    return VOS_OK;
}

LOCAL gint32 smm_set_manager_removed_severity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object *o_result_jso)
{
    const gchar* board_type[] = {
        "PSU",
        "Fan",
        "Blade",
        "Swi",
        "SMM",
    };
    guint ret = 0;
    gint32 result;
    guint bret = 0;
    guint8 type_index;
    json_object *o_message_jso_temp = NULL;
    json_object *severity_jso = NULL;

    gint32 return_array[NUMBER_NETWORK_PORT_ATTRIBUTES_OEM] = {0};
    gint32 arr_subscript = 0;
    const gchar *severity_str = NULL;

    if ((o_result_jso == NULL) || (provider_paras_check(i_paras) != VOS_OK) || (o_message_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (!(i_paras->user_role_privilege & USERROLE_BASICSETTING)) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_REMOVED_SEVERITY, &o_message_jso_temp,
            RFPROP_MANAGER_REMOVED_SEVERITY);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        return HTTP_FORBIDDEN;
    }

    for (type_index = 0; type_index < BOARD_TYPE_NUM; type_index++) {
        ret = (guint)json_object_object_get_ex(o_result_jso, board_type[type_index], &severity_jso);
        bret += ret;
        if (severity_jso) {
            severity_str = dal_json_object_get_str(severity_jso);
            result = redfish_set_event_severity(type_index, severity_str, i_paras);
            return_array[arr_subscript] = result;
            arr_subscript++;
        }
    }
    return_val_if_expr(!bret, HTTP_BAD_REQUEST); 

    result = return_value_judgment(arr_subscript, return_array);
    return result;
}
LOCAL gint32 smm_set_severity_and_supportflag(OBJ_HANDLE info_object_handle, OBJ_HANDLE plcy_object_handle,
    guint8 severity, PROVIDER_PARAS_S *i_paras)
{
    GSList *input_list = NULL;
    gint32 ret;
    guint8 deassert_flag;

    input_list = g_slist_append(input_list, g_variant_new_byte(severity));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, info_object_handle,
        CLASS_EVENT_INFORMATION, METHOD_EVENT_INFORMATION_SET_SEVERITY, 0, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);
    input_list = NULL;
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Call class[%s] method[%s] failed, err code %d", __func__, CLASS_EVENT_INFORMATION,
            METHOD_EVENT_INFORMATION_SET_SEVERITY, ret);
        return VOS_ERR;
    }
    deassert_flag = (severity == 0) ? EVENT_OBJ_STATE_DEASSERT : EVENT_OBJ_STATE_ASSERT;
    input_list = g_slist_append(input_list, g_variant_new_byte(deassert_flag));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, plcy_object_handle,
        CLASS_EVENT_POLICY, METHOD_EVENT_POLICY_SET_POLICY_DEASSERT_FLAG, 0, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Call class[%s] method[%s] failed, err code %d", __func__, CLASS_EVENT_POLICY,
            METHOD_EVENT_POLICY_SET_POLICY_DEASSERT_FLAG, ret);
        return VOS_ERR;
    }
    return VOS_OK;
}

LOCAL gint32 redfish_set_event_severity(guint8 target, const gchar *severity_str, PROVIDER_PARAS_S *i_paras)
{
    const gchar* board_remove_plcy[] = {
        PS_REMOVEED_POLICY,
        FAN_REMOVEED_POLICY,
        BLADE_REMOVEED_POLICY,
        SWI_REMOVEED_POLICY,
        SMM_REMOVEED_POLICY,
    };
    gint32 ret;
    OBJ_HANDLE info_object_handle = 0;
    OBJ_HANDLE plcy_object_handle = 0;
    errno_t safe_fun_ret;
    guint8 severity;
    GSList *input_list = NULL;
    guint8 old_severity;
    gchar policy_name[STRING_LEN_MAX] = {0};

    change_severity_str_to_num(severity_str, &severity);
    return_val_if_expr(severity == SEVERITY_INVALID, VOS_ERR);
    ret = dfl_get_object_handle(g_board_remove_info[target], &info_object_handle);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "Get remove_info handle failed, ret = %d", ret));
    ret = dfl_get_object_handle(board_remove_plcy[target], &plcy_object_handle);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "Get remove_plcy handle failed, ret = %d", ret));
    safe_fun_ret = strcpy_s(policy_name, sizeof(policy_name), board_remove_plcy[target]);
    return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
        debug_log(DLOG_ERROR, "strcpy_s failed. ret = %d", safe_fun_ret));

    (void)dal_get_property_value_byte(info_object_handle, PROPERTY_EVENT_INFORMATION_SEVERITY, &old_severity);
    return_val_if_expr(severity == old_severity, HTTP_OK);
    if ((severity == SEVERITY_NORMAL) && (old_severity != SEVERITY_NORMAL)) {
        input_list = g_slist_append(input_list, g_variant_new_string(policy_name));
        input_list = g_slist_append(input_list, g_variant_new_byte(i_paras->is_from_webui));
        input_list = g_slist_append(input_list, g_variant_new_string(i_paras->user_name));
        input_list = g_slist_append(input_list, g_variant_new_string(i_paras->client));
        input_list = g_slist_append(input_list, g_variant_new_byte(i_paras->user_role_privilege));
        ret = dfl_foreach_object(CLASS_EVENT_MONITOR, redfish_del_event_from_monitor_foreach, input_list, NULL);
        uip_free_gvariant_list(input_list);
        return_val_do_info_if_expr(ret != VOS_OK, ret, debug_log(DLOG_ERROR, "Del event failed."));
    }
    ret = smm_set_severity_and_supportflag(info_object_handle, plcy_object_handle, severity, i_paras);
    return_val_do_info_if_expr(ret != VOS_OK, ret, debug_log(DLOG_ERROR, "set event severity failed."));

    return HTTP_OK;
}

LOCAL gint32 redfish_del_event_from_monitor_foreach(OBJ_HANDLE handle, gpointer input_list)
{
    guint8 state = 0;
    gchar policy_name[STRING_LEN_MAX] = {0};
    gchar user_policy_name[STRING_LEN_MAX] = {0};
    gchar user_name[SESSION_USER_NAME_MAX_LEN + 1] = {0};
    gchar request_ip[SESSION_IP_LEN] = {0};

    return_val_do_info_if_fail(input_list != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));
    const gchar *user_data = g_variant_get_string((GVariant *)g_slist_nth_data((GSList *)input_list, 0), NULL);
    gint32 result = strcpy_s(user_policy_name, sizeof(user_policy_name), user_data);
    return_val_do_info_if_fail(result == EOK, result,
        debug_log(DLOG_ERROR, "memcpy_s policy_name fail, ret = %d\n", result));

    guint8 from_webui_flag = g_variant_get_byte((GVariant *)g_slist_nth_data((GSList *)input_list, 1));

    user_data = g_variant_get_string((GVariant *)g_slist_nth_data((GSList *)input_list, 2),
        NULL); 
    result = strcpy_s(user_name, sizeof(user_name), user_data);
    return_val_do_info_if_fail(result == EOK, result,
        debug_log(DLOG_ERROR, "memcpy_s user_name fail, ret = %d\n", result));

    user_data = g_variant_get_string((GVariant *)g_slist_nth_data((GSList *)input_list, 3),
        NULL); 
    result = strcpy_s(request_ip, sizeof(request_ip), user_data);
    return_val_do_info_if_fail(result == EOK, result,
        debug_log(DLOG_ERROR, "memcpy_s request_ip fail, ret = %d\n", result));

    guint8 user_privilege =
        g_variant_get_byte((GVariant *)g_slist_nth_data((GSList *)input_list, 4)); 

    (void)dal_get_property_value_string(handle, PROPERTY_EVENT_MONITOR_POLICY_OBJ, policy_name, sizeof(policy_name));

    (void)dal_get_property_value_byte(handle, PROPERTY_EVENT_MONITOR_STATE, &state);
    if ((state == EVENT_OBJ_STATE_ASSERT) && (strcmp(user_policy_name, policy_name) == 0)) {
        result = uip_call_class_method_redfish(from_webui_flag, user_name, request_ip, handle, CLASS_EVENT_MONITOR,
            METHOD_EVENT_MONITOR_DEL_EVENT_FROM_MONITOR, 0, user_privilege, NULL, NULL);
        if (result != VOS_OK) {
            debug_log(DLOG_ERROR, "%s: uip_call_class_method_redfish fail, ret = %d\n", __FUNCTION__, result);
            return result;
        }
    }
    return VOS_OK;
}


LOCAL gint32 get_while_action_url(json_object** o_result_jso)
{
    gchar slot_name[STRING_LEN_MAX] = {0};
    gchar manager_uri[MAX_URI_LENGTH] = {0};
    const gchar* oem_action_array[] = {
        RFPROP_MANAGER_EXPORT_DUMP,
        RFPROP_MANAGER_DELETE_SWI_PROFILE,
        RFPROP_MANAGER_RESTORE_SWI_PROFILE,
        RFPROP_MANAGER_PARSE_SWI_PROFILE,
        RFPROP_MANAGER_POWER_ON,
        RFPROP_MANAGER_DELETE_LANGUAGE,
        RFPROP_MANAGER_GENERAL_DOWNLOAD,
        RFPROP_MANAGER_SHELF_POWER_CONTROL,
    };

    
    gint32 ret = redfish_get_board_slot(slot_name, sizeof(slot_name));
    if (ret != RET_OK) {
        return RET_ERR;
    }

    ret = snprintf_s(manager_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT_MANAGER, slot_name);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    
    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        return RET_ERR;
    }

    
    rf_add_action_prop(*o_result_jso, (const gchar*)manager_uri, RFPROP_MANAGER_RESET);
    rf_add_action_prop(*o_result_jso, (const gchar*)manager_uri, RFPROP_MANAGER_FORCE_FAILOVER);

    json_object* hw_jso = json_object_new_object();
    if (hw_jso == NULL) {
        (void)json_object_put(*o_result_jso);
        return RET_ERR;
    }

    for (guint32 i = 0; i < G_N_ELEMENTS(oem_action_array); i++) {
        rf_add_action_prop(hw_jso, (const gchar*)manager_uri, oem_action_array[i]);
    }

    
    if (dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_MGNT) == TRUE) {
        rf_add_action_prop(hw_jso, (const gchar*)manager_uri, RFPROP_MANAGER_ROLLBACK);
    }

    json_object* oem_jso = json_object_new_object();
    if (oem_jso == NULL) {
        (void)json_object_put(*o_result_jso);
        (void)json_object_put(hw_jso);
        return RET_ERR;
    }

    json_object_object_add(oem_jso, RFPROP_COMMON_HUAWEI, hw_jso);
    json_object_object_add(*o_result_jso, RFPROP_COMMON_OEM, oem_jso);

    return RET_OK;
}


LOCAL gint32 smm_get_manager_actions(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    
    if (o_result_jso == NULL || o_message_jso == NULL || i_paras == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    if (get_while_action_url(o_result_jso) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get action url fail!", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


LOCAL gint32 smm_get_manager_property(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    int iRet;
    gchar smm_name[MAX_NAME_SIZE] = { 0 }; 
    gchar odata_id[MAX_URI_LEN] = { 0 };

    if (o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    if (dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_MGNT)) {
        iRet = redfish_get_board_slot(smm_name, sizeof(smm_name));
        if (iRet != RET_OK) {
            debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    } else {
        if (!g_strcmp0(smm_manager_provider[i_paras->index].property_name, RFPROP_MANAGER_ETHERNETINTERFACES)) {
            get_smm_name_from_redfish_uri(RF_MANAGERS_URI, i_paras->uri, smm_name, sizeof(smm_name));
        } else {
            smm_get_manager_reachable_smm_name(i_paras, smm_name, sizeof(smm_name));
        }
    }
    iRet = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1,
        RF_MANAGERS_URI_S, smm_name, smm_manager_provider[i_paras->index].property_name);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    *o_result_jso = json_object_new_object();
    json_object_object_add(*o_result_jso, RFPROP_ODATA_ID, json_object_new_string(odata_id));
    return HTTP_OK;
}


LOCAL gint32 get_smm_sys_ntp_switch_odata(json_object* o_result_jso)
{
    guint8 smm_index = 0;
    guint8 as_status = 0;
    gchar odata_id[MAX_URI_LEN] = { 0 };

    if (o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    gint32 ret = get_smm_index(&smm_index);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get smm_index error, ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_MGNT) != TRUE) {
        (void)get_board_active_state(&as_status);
        if (as_status != ACTIVE_STATE) { 
            smm_index = (smm_index == 1) ? 2 : 1; 
        }
    }

    // Syslog
    (void)snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, "/redfish/v1/Managers/HMM%u/SyslogService", smm_index);
    json_object *obj = json_object_new_object();
    json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string(odata_id));
    json_object_object_add(o_result_jso, RFPROP_MANAGER_SYSLOG, obj);

    // NTP
    (void)snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, "/redfish/v1/Managers/HMM%u/NtpService", smm_index);
    obj = json_object_new_object();
    json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string(odata_id));
    json_object_object_add(o_result_jso, RFPROP_MANAGER_NTP, obj);

    // SwitchProfiles
    (void)snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, "/redfish/v1/Managers/HMM%u/SwitchProfiles", smm_index);
    obj = json_object_new_object();
    json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string(odata_id));
    json_object_object_add(o_result_jso, RFPROP_MANAGER_SWITCH_PROFILE, obj); 

    (void)snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1,
        "/redfish/v1/Managers/HMM%u/SecurityService", smm_index);
    obj = json_object_new_object();
    json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string(odata_id));
    json_object_object_add(o_result_jso, RFPROP_MANAGER_SECURITY_SVC, obj);
    return RET_OK;
}


LOCAL gint32 smm_get_manager_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet;
    json_object *manager_for_chassis = NULL;
    json_object *manager_in_chassis = NULL;
    json_object *obj = NULL;
    gchar odata_id[MAX_URI_LEN] = { 0 };
    gchar smm_name[STRING_LEN_MAX] = { 0 };
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso) && (NULL != i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_object();
    // Manager for chassis
    manager_for_chassis = json_object_new_array();
    obj = json_object_new_object();
    json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string("/redfish/v1/Chassis/Enclosure"));
    json_object_array_add(manager_for_chassis, obj);
    json_object_object_add(*o_result_jso, "ManagerForChassis", manager_for_chassis);
    // Manager in chassis

    if (dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_MGNT)) { // 大西洋直通板只获取本板资源
        (void)redfish_get_board_slot(smm_name, sizeof(smm_name));
    } else {
        get_smm_name_from_redfish_uri(RF_MANAGERS_URI, i_paras->uri, smm_name, sizeof(smm_name));
    }

    manager_in_chassis = json_object_new_object();
    iRet = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, "/redfish/v1/Chassis/%s", smm_name);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    json_object_object_add(manager_in_chassis, RFPROP_ODATA_ID, json_object_new_string(odata_id));
    json_object_object_add(*o_result_jso, "ManagerInChassis", manager_in_chassis);
    return HTTP_OK;
}
LOCAL void smm_get_manager_lldp(guint8 smm_index, json_object *huawei)
{
    json_object *obj = NULL;
    gchar odata_id[MAX_URI_LEN] = { 0 };
    guchar lldp_state = 0;

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_LLDP, &lldp_state);
    if (lldp_state == SERVICE_NETCONFIG_ENABLE) {
        gint32 ret = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1,
            "/redfish/v1/Managers/HMM%u/LldpService", smm_index);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
            return;
        }
        obj = json_object_new_object();
        json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string((const char*)odata_id));
        json_object_object_add(huawei, RFPROP_MANAGER_LLDP, obj);
    }
}

LOCAL gint32 get_manager_shelf_power_state(json_object **oem_property)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 power_state = 0;
    gchar buf[MAX_STATUS_LEN] = {0};

    
    gint32 ret = dal_get_object_handle_nth(CLASS_SHELF_PAYLOAD, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get obj_handle fail, ret = %d.", ret);
        return ret;
    }

    
    ret = dal_get_property_value_byte(obj_handle, PROTERY_SHELF_POWER_STATE, &power_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get shelf power state fail, ret = %d.", ret);
        return ret;
    }

    ret = sprintf_s(buf, MAX_STATUS_LEN, "%s", power_state == 1 ? "On" : "Off");
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "snprintf_s fail, ret = %d", ret);
        return RET_ERR;
    }

    
    *oem_property = json_object_new_string((const gchar *)buf);
    if (*oem_property == NULL) {
        debug_log(DLOG_ERROR, "new json obj fail.");
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL void smm_manager_oem_object_add(json_object **o_result_jso, guint8 smm_index)
{
    json_object *obj = NULL;
    gint32 timeout = 0;

    json_object *huawei = json_object_new_object();
    json_object_object_add(*o_result_jso, RFPROP_OEM_HUAWEI, huawei);
    // Language set
    get_manager_oem_language_set(&obj);
    json_object_object_add(huawei, RFPROP_MANAGER_LANGUAGE_SET, obj);
    // SystemManagerInfo
    obj = NULL;
    get_manager_oem_system_manager_info(&obj);
    json_object_object_add(huawei, RFPROP_SYSTEM_MANAGER_INFO, obj);
    // Login rule
    obj = NULL;
    get_manager_oem_permit_rule(&obj);
    json_object_object_add(huawei, "LoginRule", obj);

    obj = NULL;
    get_manager_oem_product_id(&obj);
    json_object_object_add(huawei, RFPROP_PRODUCT_UNIQUE_ID, obj);

    // lldp
    smm_get_manager_lldp(smm_index, huawei);
    gint32 ret = get_smm_sys_ntp_switch_odata(huawei);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get url path error ret = %d", __FUNCTION__, ret);
        return;
    }
    // 受控上电
    (void)get_power_on_controll_timeout(&timeout);
    json_object_object_add(huawei, RFPROP_MANAGER_PWON_PERMIT_TMOUT_MINUTES, json_object_new_int(timeout));
    
    obj = json_object_new_object();
    ret = smm_get_manager_removed_severity(&obj);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: smm_get_manager_removed_severity failed", __FUNCTION__);
    }
    json_object_object_add(huawei, RFPROP_MANAGER_EVENT_SEVERITY_CFG, obj);

    (void)get_each_blade_power_on_control_status(&huawei);

    ret = get_manager_shelf_power_state(&obj);
    if (ret == RET_OK) {
        json_object_object_add(huawei, RFPROP_MANAGER_SHELF_POWER_STATE, obj);
    }

    ret = get_shelf_power_button_mode(&obj);
    if (ret == RET_OK) {
        json_object_object_add(huawei, RFPROP_MANAGER_SHELF_POWER_BUTTON_MODE, obj);
    }
}

LOCAL gint32 smm_get_manager_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guint8 smm_index = 0;

    if ((o_message_jso == NULL) || (o_result_jso == NULL) || (i_paras == NULL)) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if (!(i_paras->is_satisfy_privi)) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }
    *o_result_jso = json_object_new_object();
    (void)get_smm_index(&smm_index);

    
    if (!dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_MGNT) && !is_smm_active()) {
        json_object_object_add(*o_result_jso, RFPROP_OEM_HUAWEI, NULL);
        return HTTP_OK;
    }
    smm_manager_oem_object_add(o_result_jso, smm_index);
    return HTTP_OK;
}


LOCAL gint32 set_power_on_permit(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE shelf_manage_handle = 0;
    gint32 rf_ret = HTTP_BAD_REQUEST;
    gint32 timeout = 0;
    json_object *huawei_obj = NULL;
    json_object *obj = NULL;
    json_object *blade_control_pwon_status = NULL;
    json_object *blade_control_pwon_item = NULL;
    gint32 blade_count = 0;
    gint32 array_len = 0;
    gchar info_str[MAX_STRBUF_LEN] = { 0 };
    json_object *msg_jso = NULL;
    guint8 *slot_id_array = NULL;
    guint8 *status_array = NULL;
    guint8 blade_index = 0;
    guint8 i = 0;
    GSList *input_list = NULL;

    gint32 ret = dal_get_object_handle_nth(CLASS_NAME_SHELF_MANAGE, 0, &shelf_manage_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "[%s] Get shelfmanage object handle failed, err code %d", __func__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_jso);
        (void)json_object_array_add(*o_message_jso, msg_jso));

    ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_OEM_HUAWEI, &huawei_obj);
    return_val_do_info_if_expr(0 == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "[%s] Request body is not right", __func__));

    ret = json_object_object_get_ex(huawei_obj, RFPROP_MANAGER_PWON_PERMIT_TMOUT_MINUTES, &obj);
    if (0 != ret) {
        timeout = json_object_get_int(obj);
        input_list = g_slist_append(input_list, g_variant_new_int32(timeout));
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
            shelf_manage_handle, CLASS_NAME_SHELF_MANAGE, METHOD_SET_CONTROLLED_POWER_ON_TMOUT, AUTH_DISABLE,
            i_paras->user_role_privilege, input_list, NULL);

        uip_free_gvariant_list(input_list);
        input_list = NULL;

        if (ret == VOS_OK) {
            rf_ret = HTTP_OK;
        } else {
            debug_log(DLOG_ERROR, "[%s] Call method[%s] of class[%s] failed, err code %d", __func__,
                METHOD_SET_CONTROLLED_POWER_ON_TMOUT, CLASS_NAME_SHELF_MANAGE, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_jso);
            (void)json_object_array_add(*o_message_jso, msg_jso);
            rf_ret = HTTP_BAD_REQUEST;
        }
    }

    ret = json_object_object_get_ex(huawei_obj, RFPROP_MANAGER_PWON_PERMIT_STATUS, &blade_control_pwon_status);
    if (0 != ret) {
        ret = get_compute_node_count(&blade_count);
        return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "[%s]get_compute_node_count failed.err_code:%d", __func__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_jso);
            (void)json_object_array_add(*o_message_jso, msg_jso));

        array_len = json_object_array_length(blade_control_pwon_status);
        return_val_do_info_if_expr(0 == array_len || blade_count < array_len, HTTP_BAD_REQUEST,
            (void)snprintf_s(info_str, sizeof(info_str), sizeof(info_str) - 1, "1 ~ %d", blade_count);
            (void)create_message_info(MSGID_PROP_WRONG_ARRAY_LENGTH, "Oem/Huawei/PowerOnPermitStatus", &msg_jso,
            info_str);
            (void)json_object_array_add(*o_message_jso, msg_jso));

        slot_id_array = (guint8 *)g_malloc0(sizeof(guint8) * array_len);
        return_val_do_info_if_expr(NULL == slot_id_array, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "[%s] Malloc failed", __func__);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_jso);
            (void)json_object_array_add(*o_message_jso, msg_jso));

        status_array = (guint8 *)g_malloc0(sizeof(guint8) * array_len);
        return_val_do_info_if_expr(NULL == status_array, HTTP_INTERNAL_SERVER_ERROR, g_free(slot_id_array);
            debug_log(DLOG_ERROR, "[%s] Malloc failed", __func__);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_jso);
            (void)json_object_array_add(*o_message_jso, msg_jso));

        (void)memset_s(slot_id_array, sizeof(guint8) * array_len, 0, sizeof(guint8) * array_len);
        (void)memset_s(status_array, sizeof(guint8) * array_len, 0, sizeof(guint8) * array_len);

        json_object_array_foreach(blade_control_pwon_status, blade_control_pwon_item)
        {
            blade_index++;
            ret = json_object_object_get_ex(blade_control_pwon_item, RFPROP_MANAGER_PWON_CONTROL_ENABLE, &obj);
            if (ret) {
                if (json_type_boolean != json_object_get_type(obj)) {
                    (void)snprintf_s(info_str, sizeof(info_str), sizeof(info_str) - 1,
                        "Oem/Huawei/PowerOnPermitStatus/blade%u/ControlledEnabled", blade_index);
                    (void)create_message_info(MSGID_PROP_TYPE_ERR, info_str, &msg_jso, dal_json_object_get_str(obj),
                        info_str);
                    json_object_array_add(*o_message_jso, msg_jso);

                    g_free(slot_id_array);
                    g_free(status_array);

                    return HTTP_BAD_REQUEST;
                } else {
                    slot_id_array[i] = blade_index;
                    status_array[i] = json_object_get_boolean(obj) ? 1 : 0;
                    i++;
                }
            }
        }

        input_list = g_slist_append(input_list,
            g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, slot_id_array, i, sizeof(guint8)));
        input_list =
            g_slist_append(input_list, g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, status_array, i, sizeof(guint8)));
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
            shelf_manage_handle, CLASS_NAME_SHELF_MANAGE, METHOD_SET_BLADE_CONTROLLED_POWER_ON_INFO, AUTH_DISABLE,
            i_paras->user_role_privilege, input_list, NULL);

        uip_free_gvariant_list(input_list);
        g_free(slot_id_array);
        g_free(status_array);

        if (ret == VOS_OK) {
            rf_ret = HTTP_OK;
        } else {
            debug_log(DLOG_ERROR, "[%s] Call method[%s] of class[%s] failed, err code %d", __func__,
                METHOD_SET_BLADE_CONTROLLED_POWER_ON_INFO, CLASS_NAME_SHELF_MANAGE, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_jso);
            (void)json_object_array_add(*o_message_jso, msg_jso);
            rf_ret = HTTP_BAD_REQUEST;
        }
    }

    return rf_ret;
}


LOCAL gint32 smm_set_manager_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 rf_ret = HTTP_BAD_REQUEST;
    gboolean retval;
    json_object *login_rule_jso = NULL;
    json_object *huawei = NULL;
    json_object *remove_severity_jso = NULL;
    json_object *event_severity_cfg_jso = NULL;
    json_object *system_manager_info_jso = NULL;

    if ((o_result_jso == NULL) || (provider_paras_check(i_paras) != VOS_OK) || (o_message_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_OEM, o_message_jso,
        RFPROP_MANAGER_OEM));

    // 本板为备用，不可操作
    gint32 ret = smm_set_oem_check_hmm_status(i_paras, o_message_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    (*o_message_jso) = json_object_new_array(); // 存储所有出错的消息，数组的形式

    
    ret = set_power_on_permit(i_paras, o_message_jso, o_result_jso);
    do_info_if_true(ret == HTTP_OK, (rf_ret = HTTP_OK));

    
    (void)json_object_object_get_ex(i_paras->val_jso, RFPROP_OEM_HUAWEI, &huawei);
    retval = json_object_object_get_ex(huawei, RFPROP_MANAGER_PATCH_LOGIN, &login_rule_jso);
    
    if (retval && login_rule_jso) {
        ret = set_manager_loginrule(i_paras, o_message_jso, &login_rule_jso);
        do_info_if_true(ret == VOS_OK, (rf_ret = HTTP_OK));
    }
    retval = json_object_object_get_ex(huawei, RFPROP_MANAGER_EVENT_SEVERITY_CFG, &event_severity_cfg_jso);
    
    if (retval && event_severity_cfg_jso) {
        retval =
            json_object_object_get_ex(event_severity_cfg_jso, RFPROP_MANAGER_REMOVED_SEVERITY, &remove_severity_jso);
        if (retval && remove_severity_jso) {
            ret = smm_set_manager_removed_severity(i_paras, o_message_jso, remove_severity_jso);
            do_info_if_true(ret == HTTP_OK, (rf_ret = HTTP_OK));
        }
    }

    
    retval = json_object_object_get_ex(huawei, RFPROP_SYSTEM_MANAGER_INFO, &system_manager_info_jso);
    
    if (retval && system_manager_info_jso) {
        ret = set_system_manager_info(i_paras, o_message_jso, &system_manager_info_jso);
        do_info_if_true(ret == VOS_OK, (rf_ret = HTTP_OK));
    }

    
    ret = set_shelf_power_button_mode(i_paras, o_message_jso);
    if (ret == HTTP_OK) {
        rf_ret = HTTP_OK;
    }

    return rf_ret;
}


LOCAL gint32 smm_set_oem_check_hmm_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso)
{
    guint8 as_status = ASM_STAND_BY;

    // 本板为备用，不可操作
    (void)get_board_active_state(&as_status);
    return_val_do_info_if_expr(as_status != ASM_ACTIVE, HTTP_BAD_REQUEST,
        create_message_info(MSGID_CURRENT_MM_NOT_ACTIVE, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: set oem failed, cause current HMM is not active.", __FUNCTION__));

    // 访问非本板
    if (is_local_hmm_index(RF_MANAGERS_URI, i_paras->uri) != TRUE) {
        create_message_info(MSGID_CURRENT_MM_NOT_ACTIVE, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: set oem failed, cause current HMM is not active.", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }

    return VOS_OK;
}


LOCAL gint32 smm_action_manager_power_on(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet = -1;
    gint32 ret;
    json_object *poweron_obj = NULL;
    json_object *slot_id_array_obj = NULL;
    json_object *slot_item = NULL;
    guint32 slot_id;
    gint32 array_len;
    guint8 *slot_id_array = NULL;
    guint8 i = 0;
    OBJ_HANDLE shelf_manage = 0;
    OBJ_HANDLE blade_handle = 0;
    GSList *input_list = NULL;
    guint8 power_on_enabled = 0;
    gint32 blade_count = 0;
    gchar info_str[MAX_STRBUF_LEN] = { 0 };
    const gchar *value_str = NULL;

    
    return_val_do_info_if_fail((o_message_jso != NULL) && (i_paras != NULL) && (i_paras->val_jso != NULL),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    ret = smm_check_hmm_active(i_paras, o_message_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret = json_object_object_get_ex(i_paras->val_jso, RFACTION_MANAGER_ACTION_POWER_ON_PERMIT, &poweron_obj);
    return_val_do_info_if_expr(ret == 0, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_MISSING, RFPROP_MANAGER_PWON_PERMIT_STATUS, o_message_jso,
        RFPROP_MANAGER_PWON_PERMIT_STATUS));

    value_str = dal_json_object_get_str(poweron_obj);
    if (0 == g_strcmp0(value_str, "On")) {
        power_on_enabled = 1;
    } else if (0 == g_strcmp0(value_str, "Off")) {
        power_on_enabled = 0;
    }

    ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_MANAGER_POWER_ON_SLOT_ID, &slot_id_array_obj);
    if (!ret) {
        (void)create_message_info(MSGID_PROP_MISSING, RFPROP_MANAGER_POWER_ON_SLOT_ID, o_message_jso,
            RFPROP_MANAGER_POWER_ON_SLOT_ID);
        return HTTP_BAD_REQUEST;
    }
    ret = get_compute_node_count(&blade_count);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "[%s]get_compute_node_count failed.err_code:%d", __func__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    array_len = json_object_array_length(slot_id_array_obj);
    if (array_len == 0 || blade_count < array_len) {
        (void)snprintf_s(info_str, sizeof(info_str), sizeof(info_str) - 1, "1 ~ %d", blade_count);
        (void)create_message_info(MSGID_PROP_WRONG_ARRAY_LENGTH, "Oem/Huawei/Manager.PowerOnPermit", o_message_jso,
            info_str);
        return HTTP_BAD_REQUEST;
    }

    slot_id_array = (guint8 *)g_malloc0(sizeof(guint8) * array_len);
    return_val_do_info_if_expr(slot_id_array == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "[%s] Malloc failed", __func__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    (void)memset_s(slot_id_array, sizeof(guint8) * array_len, 0xff, sizeof(guint8) * array_len);

    json_object_array_foreach(slot_id_array_obj, slot_item)
    {
        if (json_type_int != json_object_get_type(slot_item)) {
            iRet = snprintf_s(info_str, sizeof(info_str), sizeof(info_str) - 1, "%s",
                "Oem/Huawei/Manager.PowerOnPermit/SlotIds");
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            (void)create_message_info(MSGID_PROP_TYPE_ERR, info_str, o_message_jso, dal_json_object_get_str(slot_item),
                info_str);
            g_free(slot_id_array);
            return HTTP_BAD_REQUEST;
        }

        slot_id = (guint32)json_object_get_int(slot_item);
        ret = dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROPERTY_IPMBETH_BLADE_ID, slot_id & 0xff,
            &blade_handle);
        return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR, g_free(slot_id_array);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "[%s]get ipmbethblade faild.slotid:%d, ret:%d", __func__, slot_id, ret));

        slot_id_array[i] = (slot_id & 0xff);
        i++;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(power_on_enabled));
    input_list =
        g_slist_append(input_list, g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, slot_id_array, i, sizeof(guint8)));

    (void)dal_get_object_handle_nth(CLASS_NAME_SHELF_MANAGE, 0, &shelf_manage);
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, shelf_manage,
        CLASS_NAME_SHELF_MANAGE, METHOD_SET_BLADE_POWER_STATE, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        NULL);

    uip_free_gvariant_list(input_list);
    g_free(slot_id_array);
    debug_log(DLOG_ERROR, "[%s]set slots:%s poweron state:%s,ret:%d", __func__,
        json_object_to_json_string_ext(slot_id_array_obj, JSON_C_TO_STRING_PLAIN), value_str, ret);
    // 成功
    return_val_do_info_if_expr(ret == VOS_OK, HTTP_OK, (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso));
    // 无权限
    return_val_do_info_if_expr(ret == RFERR_INSUFFICIENT_PRIVILEGE, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    debug_log(DLOG_ERROR, "set power on enabled failed, error code:%d", ret);
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return HTTP_BAD_REQUEST;
}


LOCAL gint32 smm_action_manager_reset(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    const gchar *reset_type_str = NULL;

    gint32 ret = input_parameter_check(i_paras, o_message_jso);
    if (ret != RET_OK) {
        return ret;
    }
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_NULL,
        CLASS_BMC, METHD_BMC_REBOOTPME, AUTH_ENABLE, i_paras->user_role_privilege, NULL, NULL);
    // 成功
    return_val_do_info_if_expr((VOS_OK == ret), HTTP_OK, (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso));
    // 无权限
    return_val_do_info_if_expr((RFERR_INSUFFICIENT_PRIVILEGE == ret), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    debug_log(DLOG_ERROR, "reset failed, error code:%#x", ret);
    (void)get_element_str(i_paras->val_jso, RFACTION_PARAM_RESET_TYPE, &reset_type_str);
    (void)create_message_info(MSGID_RESET_NOT_ALLOWED, NULL, o_message_jso, reset_type_str);

    return HTTP_BAD_REQUEST;
}

LOCAL gint32 smm_action_manager_dump(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guint8 as_status = 0;
    gchar blade_name[MAX_NAME_SIZE] = {0};
    guint8 ipmb_addr = 0;
    guint8 presence = 0;
    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_SECURITYMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    gchar *ptr = i_paras->uri + strlen(RF_MANAGERS_URI);
    if (g_ascii_strncasecmp(ptr, "hmm", strlen("hmm")) == 0) {
        
        if (is_local_hmm_index(RF_MANAGERS_URI, i_paras->uri) != TRUE) {
            create_message_info(MSGID_CURRENT_BOARD_ABSENT, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        }
    } else {
        gint32 ret = strncpy_s(blade_name, MAX_NAME_SIZE, ptr, SWI_NAME_LEN);
        if (ret != EOK) {
            debug_log(DLOG_ERROR, "[%s] snprintf_s fail, ret = %d\n", __func__, ret);
        }
        ret = get_board_slave_addr(blade_name, &ipmb_addr); // 查询ipmb地址
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "[%s] Get blade(%s) ipmb addr failed, err code %d", __func__, blade_name, ret);
            create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        ret = get_board_presence(ipmb_addr, &presence);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "[%s] Get blade %u presence failed", __func__, ipmb_addr);
            create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        if (presence != BOARD_PRESENT) {
            create_message_info(MSGID_BLADE_NOT_PRESENT, NULL, o_message_jso);
            return HTTP_FORBIDDEN;
        }
    }

    (void)get_board_active_state(&as_status);
    
    return_val_do_info_if_expr(!dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_MGNT) && as_status != ASM_ACTIVE,
        HTTP_BAD_REQUEST, create_message_info(MSGID_CURRENT_MM_NOT_ACTIVE, NULL, o_message_jso));

    return act_manager_export_dump(i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 smm_act_manager_delete_language(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 as_status = 0;
    gint32 ret = input_parameter_check(i_paras, o_message_jso);
    if (ret != RET_OK) {
        return ret;
    }

    (void)get_board_active_state(&as_status);

    if (as_status != ASM_ACTIVE) { // 本板为备用，不可操作
        create_message_info(MSGID_CURRENT_MM_NOT_ACTIVE, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    return act_manager_delete_language(i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 smm_act_manager_general_download(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 as_status = 0;
    gint32 ret = input_parameter_check(i_paras, o_message_jso);
    if (ret != RET_OK) {
        return ret;
    }

    (void)get_board_active_state(&as_status);

    if (as_status != ASM_ACTIVE) { // 本板为备用，不可操作
        create_message_info(MSGID_CURRENT_MM_NOT_ACTIVE, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    return act_manager_general_download(i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 smm_action_manager_failover(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    guint8 as_status = 0;
    guint32 err_code;
    const gchar *message_str = NULL;

    gint32 ret = input_parameter_check(i_paras, o_message_jso);
    if (ret != RET_OK) {
        return ret;
    }
    (void)get_board_active_state(&as_status);
    if (ASM_ACTIVE != as_status) { // 本板为备用，不可操作
        create_message_info(MSGID_CURRENT_MM_NOT_ACTIVE, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    // 请求本板降备，本板为主，执行降备操作
    input_list = g_slist_append(input_list, g_variant_new_uint32(ASM_FAILOVER_OCCURRED_BY_USER_COMMAND));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_NULL,
        ASM_CLASS_NAME, METHOD_ASM_FAILOVER, AUTH_DISABLE, i_paras->user_role_privilege, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    // 调用失败
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Call method[%s] of object[%s] failed, err code %d", __func__, METHOD_ASM_FAILOVER,
            ASM_CLASS_NAME, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    err_code = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 0));
    if (VOS_OK == err_code) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso);
        return HTTP_OK;
    }
    // 填充错误原因
    message_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), NULL);
    (void)create_message_info(MSGID_FORCE_FAILOVER_ERROR, NULL, o_message_jso, message_str);
    debug_log(DLOG_ERROR, "[%s] failover failed, err code %d", __func__, err_code);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return HTTP_FORBIDDEN;
}


LOCAL gint32 smm_action_shelf_payload_state(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    json_object *power_state_obj = NULL;
    const gchar *value_str = NULL;
    guint8 power_control = 0;

    gint32 ret = input_parameter_check(i_paras, o_message_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_MANAGER_SHELF_POWER_STATE, &power_state_obj);
    if (ret == 0) {
        (void)create_message_info(MSGID_PROP_MISSING, RFPROP_MANAGER_PWON_PERMIT_STATUS, o_message_jso,
            RFPROP_MANAGER_PWON_PERMIT_STATUS);
        return HTTP_BAD_REQUEST;
    }

    value_str = dal_json_object_get_str(power_state_obj);
    if (g_strcmp0(value_str, "On") == 0) {
        power_control = 1;
    } else {
        power_control = 0;
    }

    ret = dfl_get_object_handle(CLASS_SHELF_PAYLOAD, &obj_handle);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(power_control));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SHELF_PAYLOAD, METHOD_SHELF_POWER_CONTROL, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

        
        case RFERR_NO_RESOURCE:
            debug_log(DLOG_ERROR, "The resource does not exist.");
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_NOT_FOUND;

        
        default:
            debug_log(DLOG_ERROR, "unknown err, ret = %d", ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 smm_act_manager_rollback(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    if (dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_MGNT) != TRUE) {
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, o_message_jso, i_paras->uri);
        return HTTP_BAD_REQUEST;
    }

    debug_log(DLOG_DEBUG, "[%s]: smm_act_manager_rollback", __FUNCTION__);

    if (o_message_jso == NULL || i_paras == NULL || i_paras->val_jso == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dfl_get_object_handle(OBJ_NAME_UPGRADE, &obj_handle);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    debug_log(DLOG_DEBUG, "[%s]: Later your system will rollback to the previous version!", __FUNCTION__);

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_UPGRADE, METHOD_UPGRADE_ROLLBACK, AUTH_ENABLE, i_paras->user_role_privilege, NULL, NULL);

    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

        
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return RFERR_INSUFFICIENT_PRIVILEGE;

        
        case RFERR_NO_RESOURCE:
            debug_log(DLOG_ERROR, "[%s]:%d smm_act_manager_rollback", __FUNCTION__, __LINE__);
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_NOT_FOUND;

        
        default:
            debug_log(DLOG_ERROR, "%s: unknown err", __FUNCTION__);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 check_swi_model(const gchar *swi_model)
{
    const gchar *swi_model_array[] = { "CX910", "CX911", "CX912", "CX915", "CX916", "CX916L", "CX310", "CX311", "CX312", "CX110", "CX111", "CX210", "CX220", "CX710", "CX920", "CX320", "CX930" };
    guint32 i;
    if (swi_model == NULL) {
        return VOS_ERR;
    }
    for (i = 0; i < sizeof(swi_model_array) / sizeof(gchar *); ++i) {
        if (strcmp(swi_model_array[i], swi_model) == 0) {
            return VOS_OK;
        }
    }
    return VOS_ERR;
}


LOCAL gint32 check_slot_id(guint8 slot_id)
{
#define BLADE_TYPE_SWI 0xc0
    OBJ_HANDLE handle = 0;
    guint8 blade_type = 0;
    gint32 ret;

    ret = dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROPERTY_IPMBETH_BLADE_ID, slot_id, &handle);
    if (VOS_OK != ret) { 
        debug_log(DLOG_DEBUG, "[%s] get blade obj by id %u failed, ret %d", __func__, slot_id, ret); 
        return VOS_ERR;
    }
    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_BLADETYPE, &blade_type);
    if (BLADE_TYPE_SWI == blade_type) { 
        return VOS_OK;
    }
    return VOS_ERR;
#undef BLADE_TYPE_SWI
}


LOCAL gboolean check_swi_exist(void)
{
#define BLADE_TYPE_SWI  0xC0
    OBJ_HANDLE handle = 0;
    guint8 blade_type = BLADE_TYPE_SWI;
    gint32 ret;
    static gboolean is_check = FALSE;
    static gboolean is_exist = FALSE;

    if (is_check == FALSE) {
        ret = dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_BLADETYPE,
            blade_type, &handle);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "[%s] get blade obj by id %u failed, ret %d", __func__, blade_type, ret);
            is_exist = FALSE;
        } else {
            is_exist = TRUE;
        }

        is_check = TRUE;
    }

    return is_exist;
#undef BLADE_TYPE_SWI
}


LOCAL gint32 smm_action_manager_del_swi_profile(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
#define FRU_BASE 1
#define FRU_FABRIC 2
    gint32 ret;
    json_object *slot_id_jso = NULL;
    guint8 slot_id = 0;
    json_object *swi_model_jso = NULL;
    const gchar *swi_model_str = NULL;
    json_object *fru_id_jso = NULL;
    guint8 fru_id = 0;
    json_object *reboot_jso = NULL;
    guint8 reboot;
    json_object *req_body = NULL;
    guint8 is_param_ok = 1;
    OBJ_HANDLE swi_handle = 0;
    GSList *input_list = NULL;
    json_object *msg_jso = NULL;

    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = smm_check_hmm_active(i_paras, o_message_jso);
    if (ret != RET_OK) {
        return ret;
    }

    req_body = i_paras->val_jso;
    (void)json_object_object_get_ex(req_body, RFPROP_MANAGER_SLOT_ID, &slot_id_jso);
    (void)json_object_object_get_ex(req_body, RFPROP_MANAGER_SWI_MODEL, &swi_model_jso);
    (void)json_object_object_get_ex(req_body, RFPROP_MANAGER_FRU_ID, &fru_id_jso);
    (void)json_object_object_get_ex(req_body, RFPROP_MANAGER_REBOOT, &reboot_jso);
    *o_message_jso = json_object_new_array();
    ret = vos_str2int(dal_json_object_get_str(slot_id_jso), 10, &slot_id, NUM_TPYE_UCHAR);
    if (ret != VOS_OK) { 
        create_message_info(MSGID_PROPERTY_VALUE_ERR, RFPROP_MANAGER_SLOT_ID, &msg_jso, RFPROP_MANAGER_SLOT_ID);
        json_object_array_add(*o_message_jso, msg_jso);
        is_param_ok = 0;
    } else {
        ret = check_slot_id(slot_id);
        if (VOS_OK != ret) {
            create_message_info(MSGID_PROPERTY_VALUE_ERR, RFPROP_MANAGER_SLOT_ID, &msg_jso, RFPROP_MANAGER_SLOT_ID);
            json_object_array_add(*o_message_jso, msg_jso);
            is_param_ok = 0;
        }
    }
    swi_model_str = dal_json_object_get_str(swi_model_jso);
    if (VOS_OK != check_swi_model(swi_model_str)) {
        create_message_info(MSGID_PROPERTY_VALUE_ERR, RFPROP_MANAGER_SWI_MODEL, &msg_jso, RFPROP_MANAGER_SWI_MODEL);
        json_object_array_add(*o_message_jso, msg_jso);
        is_param_ok = 0;
    }
    if (json_object_get_type(fru_id_jso) != json_type_int) {
        create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_FRU_ID, &msg_jso, dal_json_object_get_str(fru_id_jso),
            RFPROP_MANAGER_FRU_ID);
        json_object_array_add(*o_message_jso, msg_jso);
        is_param_ok = 0;
    } else {
        fru_id = (guint8)json_object_get_int(fru_id_jso); 
        if (fru_id != FRU_BASE && fru_id != FRU_FABRIC) { 
            create_message_info(MSGID_PROPERTY_VALUE_ERR, RFPROP_MANAGER_FRU_ID, &msg_jso, RFPROP_MANAGER_FRU_ID);
            json_object_array_add(*o_message_jso, msg_jso);
            is_param_ok = 0;
        }
    }
    if (0 == is_param_ok) {
        return HTTP_BAD_REQUEST;
    }
    json_object_put(*o_message_jso);
    *o_message_jso = NULL;
    reboot = json_object_get_boolean(reboot_jso) ? 1 : 0;
    
    ret = dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROPERTY_IPMBETH_BLADE_ID, slot_id, &swi_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get swi[%u] handle failed, ret %d", __func__, slot_id, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    input_list = g_slist_append(input_list, g_variant_new_string(swi_model_str));
    input_list = g_slist_append(input_list, g_variant_new_byte(fru_id));
    input_list = g_slist_append(input_list, g_variant_new_byte(reboot));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, swi_handle,
        CLASS_NAME_IPMBETH_BLADE, METHOD_DELETE_SWI_CFG_FILE, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "[%s] call %s failed, ret %d", __func__, METHOD_DELETE_SWI_CFG_FILE, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
#undef FRU_BASE
#undef FRU_FABRIC
}


LOCAL gint32 smm_action_manager_res_swi_profile(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *slot_id_jso = NULL;
    json_object *swi_model_jso = NULL;
    json_object *fru_id_jso = NULL;
    json_object *backup_time_jso = NULL;
    OBJ_HANDLE swi_handle = 0;
    GSList *input_list = NULL;
    guint8 slot_id;

    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    
    return_val_do_info_if_fail(
        ((i_paras->user_role_privilege & USERROLE_BASICSETTING) && (i_paras->user_role_privilege & USERROLE_POWERMGNT)),
        HTTP_FORBIDDEN, (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = smm_check_hmm_active(i_paras, o_message_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret = _check_action_manager_parameter(i_paras, o_message_jso, FALSE);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_DEBUG, "[%s] _check_action_manager_parameter failed", __func__));

    (void)json_object_object_get_ex(i_paras->val_jso, RFPROP_MANAGER_SLOT_ID, &slot_id_jso);
    (void)json_object_object_get_ex(i_paras->val_jso, RFPROP_MANAGER_SWI_MODEL, &swi_model_jso);
    (void)json_object_object_get_ex(i_paras->val_jso, RFPROP_MANAGER_FRU_ID, &fru_id_jso);
    (void)json_object_object_get_ex(i_paras->val_jso, RFPROP_MANAGER_BACKUP_TIME, &backup_time_jso);

    slot_id = 32 + json_object_get_int(slot_id_jso);
    debug_log(DLOG_DEBUG, "[%s] the slot id  is %d", __func__, slot_id);
    
    ret = dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROPERTY_IPMBETH_BLADE_ID, slot_id, &swi_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get PROPERTY_IPMBETH_BLADE_ID [%u] handle failed, ret is %d", __func__, slot_id,
            ret);
        create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(dal_json_object_get_str(swi_model_jso)));
    input_list = g_slist_append(input_list, g_variant_new_byte(json_object_get_int(fru_id_jso)));
    input_list = g_slist_append(input_list, g_variant_new_string(dal_json_object_get_str(backup_time_jso)));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, swi_handle,
        CLASS_NAME_IPMBETH_BLADE, METHOD_RESTORE_SWI_CFG_FILE, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "[%s] uip_call_class_method_redfish, ret  is %d", __func__, ret);
        (void)create_message_info(MSGID_FILE_NOT_EXIST, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    return HTTP_OK;
}


LOCAL gint32 _check_action_manager_parameter(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, gint action_type)
{
#define FRU_BASE 1
#define FRU_FABRIC 2
    gint32 ret;
    json_object *req_body = NULL;
    json_object *slot_id_jso = NULL;
    json_object *swi_model_jso = NULL;
    const gchar *swi_model_str = NULL;
    json_object *fru_id_jso = NULL;
    json_object *backup_time_jso = NULL;
    gint32 swi_slot_id;
    const gchar *backup_time_str = NULL;

    guint8 fru_id = 0;
#define REGEX_BACK_TIME                                                                                              \
    "^(?:(?!0000)[0-9]{4}-(?:(?:0[1-9]|1[0-2])-(?:0[1-9]|1[0-9]|2[0-8])|(?:0[13-9]|1[0-2])-(?:29|30)|(?:0[13578]|1[" \
    "02])-31)|(?:[0-9]{2}(?:0[48]|[2468][048]|[13579][26])|(?:0[48]|[2468][048]|[13579][26])00)-02-29)$"

    // 入参检查
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    // 获取请求信息
    req_body = i_paras->val_jso;
    (void)json_object_object_get_ex(req_body, RFPROP_MANAGER_SLOT_ID, &slot_id_jso);
    (void)json_object_object_get_ex(req_body, RFPROP_MANAGER_SWI_MODEL, &swi_model_jso);
    (void)json_object_object_get_ex(req_body, RFPROP_MANAGER_FRU_ID, &fru_id_jso);
    (void)json_object_object_get_ex(req_body, RFPROP_MANAGER_BACKUP_TIME, &backup_time_jso);

    // 进行各个参数的检查和校验
    if (json_object_get_type(slot_id_jso) != json_type_int) {
        create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_SLOT_ID, o_message_jso,
            dal_json_object_get_str(slot_id_jso), RFPROP_MANAGER_SLOT_ID);
        return HTTP_BAD_REQUEST;
    }
    swi_slot_id = json_object_get_int(slot_id_jso);
    swi_slot_id = swi_slot_id + 32;
    ret = check_slot_id(swi_slot_id);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "[%s] check the slot id  %d failed", __func__, ret);
        create_message_info(MSGID_PROPERTY_VALUE_ERR, RFPROP_MANAGER_SLOT_ID, o_message_jso, RFPROP_MANAGER_SLOT_ID);
        return HTTP_BAD_REQUEST;
    }

    swi_model_str = dal_json_object_get_str(swi_model_jso);
    if (VOS_OK != check_swi_model(swi_model_str)) {
        create_message_info(MSGID_PROPERTY_VALUE_ERR, RFPROP_MANAGER_SWI_MODEL, o_message_jso,
            RFPROP_MANAGER_SWI_MODEL);
        return HTTP_BAD_REQUEST;
    }

    if (json_object_get_type(fru_id_jso) != json_type_int) {
        create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_FRU_ID, o_message_jso,
            dal_json_object_get_str(fru_id_jso), RFPROP_MANAGER_FRU_ID);
        return HTTP_BAD_REQUEST;
    } else {
        fru_id = (guint8)json_object_get_int(fru_id_jso); 
        if (fru_id != FRU_BASE && fru_id != FRU_FABRIC) { 
            create_message_info(MSGID_PROPERTY_VALUE_ERR, RFPROP_MANAGER_FRU_ID, o_message_jso, RFPROP_MANAGER_FRU_ID);
            return HTTP_BAD_REQUEST;
        }
    }

    // 进行匹配current 时间
    backup_time_str = dal_json_object_get_str(backup_time_jso);
    if (TRUE == action_type) {
        if (0 == g_strcmp0(backup_time_str, "current")) {
            return VOS_OK;
        }
    }

    // 进行时间的正则校验
    if (TRUE != g_regex_match_simple(REGEX_BACK_TIME, backup_time_str, (GRegexCompileFlags)0, (GRegexMatchFlags)0)) {
        create_message_info(MSGID_PROPERTY_VALUE_ERR, RFPROP_MANAGER_BACKUP_TIME, o_message_jso,
            RFPROP_MANAGER_BACKUP_TIME);
        return HTTP_BAD_REQUEST;
    }

    // 查找列表中匹配的日期
    ret = _find_action_manager_res_swi_profile(i_paras, o_message_jso, FALSE);
    if (ret != VOS_OK) {
        create_message_info(MSGID_PROPERTY_VALUE_ERR, RFPROP_MANAGER_BACKUP_TIME, o_message_jso,
            RFPROP_MANAGER_BACKUP_TIME);
        return HTTP_BAD_REQUEST;
    }

    return VOS_OK;
#undef FRU_BASE
#undef FRU_FABRIC
}

LOCAL gint32 _find_action_manager_res_swi_profile(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    gint flag_time)
{
    int iRet;
    gint32 ret;
    json_object *req_body = NULL;
    json_object *backup_time_jso = NULL;
    json_object *slot_id_jso = NULL;
    const gchar *sw_name = NULL;
    gchar backdirname[MAX_IPMICMD_FILTER_SIZE] = {0};
    OBJ_HANDLE obj_handle = 0;
    guint8 slaveaddr_start = SWI_SLAVEADDR_BASE;
    guint8 slaveaddr;

    DIR *dirp = NULL;
    struct dirent *ptr = NULL;
    const gchar *backup_time_str = NULL;
    gint32 swi_slot_id;

    // 入参检查
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    req_body = i_paras->val_jso;
    (void)json_object_object_get_ex(req_body, RFPROP_MANAGER_BACKUP_TIME, &backup_time_jso);
    (void)json_object_object_get_ex(req_body, RFPROP_MANAGER_SLOT_ID, &slot_id_jso);

    backup_time_str = dal_json_object_get_str(backup_time_jso);
    swi_slot_id = json_object_get_int(slot_id_jso);
    slaveaddr = slaveaddr_start + swi_slot_id * 2;

    ret =
        dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_SLAVEADDR, slaveaddr, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_DEBUG, "[%s] get PROTERY_IPMBETH_BLADE_SLAVEADDR   failed", __func__));

    sw_name = dfl_get_object_name(obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_DEBUG, "[%s] dfl_get_object_name   failed", __func__));

    iRet = snprintf_s(backdirname, sizeof(backdirname), sizeof(backdirname) - 1, "%s/%s/%s/", CFG_FILE_PATH, sw_name,
        SWITCH_CFG_FILE_BAK_PATH);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    dirp = opendir(backdirname);
    return_val_do_info_if_expr(NULL == dirp, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:backdirname is %s:opendir error", __FUNCTION__, backdirname));

    while ((ptr = readdir(dirp)) != NULL) {
        if (0 == g_strcmp0(backup_time_str, ptr->d_name)) {
            debug_log(DLOG_DEBUG, "[%s] backup_time_str is %s   ptr->d_name  is %s ", __func__, backup_time_str,
                ptr->d_name);
            closedir(dirp);
            return VOS_OK;
        }
    }
    closedir(dirp);
    return VOS_ERR;
}


LOCAL gint32 smm_action_manager_par_swi_profile(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *slot_id_jso = NULL;
    json_object *swi_model_jso = NULL;
    json_object *fru_id_jso = NULL;
    json_object *filetime_jso = NULL;

    OBJ_HANDLE swi_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    guint8 slot_id;

    const gchar *path_file_name_str = NULL;

    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    // 判断是否主备在位
    ret = smm_check_hmm_active(i_paras, o_message_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret = _check_action_manager_parameter(i_paras, o_message_jso, TRUE);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_DEBUG, "[%s] _check_action_manager_parameter failed", __func__));

    (void)json_object_object_get_ex(i_paras->val_jso, RFPROP_MANAGER_SLOT_ID, &slot_id_jso);
    (void)json_object_object_get_ex(i_paras->val_jso, RFPROP_MANAGER_SWI_MODEL, &swi_model_jso);
    (void)json_object_object_get_ex(i_paras->val_jso, RFPROP_MANAGER_FRU_ID, &fru_id_jso);
    (void)json_object_object_get_ex(i_paras->val_jso, RFPROP_MANAGER_BACKUP_TIME, &filetime_jso);

    slot_id = 32 + json_object_get_int(slot_id_jso);
    debug_log(DLOG_DEBUG, "[%s] get blade obj by id is %d ", __func__, slot_id);
    
    ret = dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROPERTY_IPMBETH_BLADE_ID, slot_id, &swi_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "[%s] get swi[%u] handle failed, ret %d", __func__, slot_id, ret);
        (void)create_message_info(MSGID_FILE_NOT_EXIST, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }
    input_list = g_slist_append(input_list, g_variant_new_string(dal_json_object_get_str(swi_model_jso)));
    input_list = g_slist_append(input_list, g_variant_new_byte(json_object_get_int(fru_id_jso)));
    input_list = g_slist_append(input_list, g_variant_new_string(dal_json_object_get_str(filetime_jso)));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, swi_handle,
        CLASS_NAME_IPMBETH_BLADE, METHOD_PARDE_SWI_CFG_FILE, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "[%s] call %s failed, ret %d", __func__, METHOD_RESTORE_SWI_CFG_FILE, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    debug_log(DLOG_DEBUG, "[%s] call %s, ret %d", __func__, METHOD_RESTORE_SWI_CFG_FILE, ret);
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    path_file_name_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    if (TRUE != vos_get_file_accessible(path_file_name_str)) {
        debug_log(DLOG_DEBUG, "%s: file no exit.", __FUNCTION__);
        (void)create_message_info(MSGID_FILE_NOT_EXIST, NULL, o_message_jso);
        uip_free_gvariant_list(output_list);
        return HTTP_BAD_REQUEST;
    }

    // redfish模块降权限后，调用代理方法修改文件属主信息（解析接口会将解析后的文件放到/tmp/web目录下）
    (void)proxy_modify_file_mode(i_paras->is_from_webui, i_paras->user_name, i_paras->client, path_file_name_str,
        APACHE_UID, APACHE_GID);

    i_paras->custom_header = json_object_new_object();
    json_object_object_add(i_paras->custom_header, REQ_HEADER_CONTENT_TYPE,
        json_object_new_string(RF_CONTENT_TYPE_OCTET_STREAM));
    json_object_object_add(i_paras->custom_header, REQ_HEADER_CONTENT_DISPOSITION,
        json_object_new_string(path_file_name_str));

    uip_free_gvariant_list(output_list);

    return HTTP_OK;
}


gint32 smm_manager_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    
    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);
    *prop_provider = smm_manager_provider;
    *count = sizeof(smm_manager_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
