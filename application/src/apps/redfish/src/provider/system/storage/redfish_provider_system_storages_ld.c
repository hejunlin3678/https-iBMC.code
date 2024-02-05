
#include "redfish_provider.h"
#include "redfish_provider_system.h"

typedef struct ld_rsc_state_map {
    guint8 raw_state;
    const gchar* rsc_state;
} LD_RSC_STATE_MAP_S;

LOCAL LD_RSC_STATE_MAP_S g_ld_rsc_state_map[] = {
    {LD_STATE_OFFLINE, "StandbyOffline"},
    {LD_STATE_PARTIALLY_DEGRADED, "Quiesced"},
    {LD_STATE_DEGRADED, "Quiesced"},
    {LD_STATE_OPTIMAL, "Enabled"},
    {LD_STATE_FAILED, "Quiesced"},
    {LD_STATE_NOT_CONFIGURED, "Disabled"},
    {LD_STATE_INTERIM_RECOVERY, "Quiesced"},
    {LD_STATE_READY_FOR_RECOVERY, "Quiesced"},
    {LD_STATE_RECOVERYING, "Updating"},
    {LD_STATE_WRONG_DRIVE_REPLACED, "Quiesced"},
    {LD_STATE_DRVIE_IMPROPERLY_CONNECTED, "Quiesced"},
    {LD_STATE_EXPANDING, "Updating"},
    {LD_STATE_NOT_YET_AVAILABLE, "Disabled"},
    {LD_STATE_QUEUED_FOR_EXPANSION, "Enabled"},
    {LD_STATE_DISABLED_FROM_SCSIID_CONFLICT, "Disabled"},
    {LD_STATE_EJECTED, "StandbyOffline"},
    {LD_STATE_ERASE_IN_PROGRESS, "StandbyOffline"},
    {LD_STATE_UNUSED, "Disabled"},
    {LD_STATE_READY_TO_PERFORM_PREDICTIVE_SPARE_REBUILD, "Quiesced"},
    {LD_STATE_RPI_IN_PROGRESS, "Updating"},
    {LD_STATE_RPI_QUEUED, "Enabled"},
    {LD_STATE_ENCRYPTED_VOLUME_WITHOUT_KEY, "StandbyOffline"},
    {LD_STATE_ENCRYPTION_MIGRATION, "Updating"},
    {LD_STATE_ENCRYPTED_VOLUME_REKEYING, "Updating"},
    {LD_STATE_ENCRYPTED_VOLUME_ENCRYPTION_OFF, "StandbyOffline"},
    {LD_STATE_VOLUME_ENCODE_REQUESTED, "Enabled"},
    {LD_STATE_ENCRYPTED_VOLUME_REKEY_REQUESTED, "Enabled"},
    {LD_STATE_UNSUPPORTED_ON_THIS_CONTROLLER, "UnavailableOffline"},
    {LD_STATE_NOT_FORMATTED, "Enabled"},
    {LD_STATE_FORMATTING, "Updating"},
    {LD_STATE_SANITIZING, "Updating"},
    {LD_STATE_INITIALIZING, "Updating"},
    {LD_STATE_INITIALIZEFAIL, "Enabled"},
    {LD_STATE_DELETING, "Updating"},
    {LD_STATE_DELETEFAIL, "Enabled"},
    {LD_STATE_WRITE_PROTECT, "Enabled"},
};

LOCAL gint32 get_systems_storages_ld_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_systems_storages_ld_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_systems_storages_ld_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_systems_storages_ld_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_systems_storages_ld_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_systems_storages_ld_capacity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_systems_storages_ld_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_systems_storages_ld_optimum_io_size(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_systems_storages_ld_strip_size_bytes_cmcc(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_systems_storages_read_cache_policy_cmcc(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_systems_storages_ld_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_systems_storages_ld_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);


gint32 ld_get_member_id(OBJ_HANDLE obj_handle, gchar *o_member_id, gint32 member_id_len, guint8 *raid_id);

gint32 ld_get_uri(OBJ_HANDLE obj_handle, gchar *o_ld_uri, gint32 uri_len);

LOCAL gint32 get_systems_storages_ld_operations(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_systems_storages_ld_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_volume_init(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 volume_full_fgi_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj);

LOCAL PROPERTY_PROVIDER_S g_system_storage_ld_provider[] = {
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_systems_storages_ld_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_systems_storages_ld_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_systems_storages_ld_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_NAME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_systems_storages_ld_name, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_VOLUME_VOLUMETYPE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_systems_storages_ld_type, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_VOLUME_OPTIMUMIOSIZE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_systems_storages_ld_optimum_io_size, set_systems_storages_ld_optimum_io_size, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_VOLUME_STRIPSIZEBYTES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
        get_systems_storages_ld_strip_size_bytes_cmcc, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_VOLUME_READCACHEPOLICY, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
        get_systems_storages_read_cache_policy_cmcc, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_VOLUME_STATUS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_systems_storages_ld_status, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_VOLUME_CAPACITY, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_systems_storages_ld_capacity, set_systems_storages_ld_capacity, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_VOLUME_OPERATIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_systems_storages_ld_operations, NULL, NULL, ETAG_FLAG_DISABLE},
    
    {RFPROP_COMMON_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_systems_storages_ld_oem, set_systems_storages_ld_oem, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_VOLUME_LINK, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_systems_storages_ld_links, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_systems_storages_ld_action, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFACTION_VOLUME_INITIALIZE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, act_volume_init, ETAG_FLAG_ENABLE}
    
};

#define MAX_DRIVE_NUM 32
#define RAID_LEVEL_STR 16
#define RF_STORAGE_LD_URI_LEN 128
#define LD_INVALID_VALUE 0xff


gint32 ld_get_member_id(OBJ_HANDLE obj_handle, gchar *o_member_id, gint32 member_id_len, guint8 *raid_id)
{
    gint32 ret;
    gchar raid_obj_name[MAX_NAME_SIZE] = {0};
    OBJ_HANDLE raid_obj_handle = 0;
    guint16 ld_id = 0;

    return_val_do_info_if_expr(NULL == o_member_id, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    (void)dal_get_property_value_string(obj_handle, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER, raid_obj_name,
        sizeof(raid_obj_name));
    ret = dfl_get_object_handle(raid_obj_name, &raid_obj_handle);
    return_val_do_info_if_fail(DFL_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:dfl_get_object_handle fail", __FUNCTION__));

    (void)dal_get_property_value_byte(raid_obj_handle, PROPERTY_RAID_CONTROLLER_ID, raid_id);
    (void)dal_get_property_value_uint16(obj_handle, PROPERTY_LOGICAL_DRIVE_TARGET_ID, &ld_id);
    
    ret = snprintf_s(o_member_id, member_id_len, member_id_len - 1, RFPROP_SYSTEM_REVOLUME_MEMBERID, ld_id);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));
    
    return VOS_OK;
}


gint32 ld_get_uri(OBJ_HANDLE obj_handle, gchar *o_ld_uri, gint32 uri_len)
{
    int iRet;
    gint32 ret;
    gchar slot[MAX_RSC_ID_LEN] = {0};
    gchar member_id[MAX_MEM_ID_LEN] = {0};
    guint8 raid_id = 0;

    return_val_do_info_if_expr(NULL == o_ld_uri, VOS_ERR, debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "%s: get slot fail.", __FUNCTION__));

    ret = ld_get_member_id(obj_handle, member_id, MAX_MEM_ID_LEN, &raid_id);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "%s: get member id fail.", __FUNCTION__));
    
    iRet = snprintf_s(o_ld_uri, uri_len, uri_len - 1, RFPROP_SYSTEM_REVOLUME_STR_MEM, slot, raid_id, member_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    
    return VOS_OK;
}


LOCAL gint32 op_get_check_para_and_priv(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return VOS_OK;
}


LOCAL gint32 get_systems_storages_ld_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar odata_context[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_ID_LEN] = {0};
    
    gchar ld_member_id[MAX_MEM_ID_LEN] = {0};
    guint8 raid_id = 0;
    
    ret = op_get_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get slot fail.", __FUNCTION__));
    
    
    ret = ld_get_member_id(i_paras->obj_handle, ld_member_id, MAX_MEM_ID_LEN, &raid_id);
    
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "%s: get member id fail.", __FUNCTION__));

    iRet = snprintf_s(odata_context, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, RFPROP_VOLUME_ODATA_CONTEXT, slot, raid_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    
    *o_result_jso = json_object_new_string((const char *)odata_context);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}

LOCAL gint32 get_systems_storages_ld_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar ld_uri[RF_STORAGE_LD_URI_LEN] = {0};

    ret = op_get_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);
    
    ret = ld_get_uri(i_paras->obj_handle, ld_uri, RF_STORAGE_LD_URI_LEN);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:get uri fail", __FUNCTION__));

    *o_result_jso = json_object_new_string(ld_uri);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}

LOCAL gint32 get_systems_storages_ld_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar ld_member_id[MAX_MEM_ID_LEN] = {0};
    guint8 raid_id = 0;

    ret = op_get_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);
    
    ret = ld_get_member_id(i_paras->obj_handle, ld_member_id, MAX_MEM_ID_LEN, &raid_id);
    
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "%s: get member id fail.", __FUNCTION__));

    *o_result_jso = json_object_new_string((const gchar *)ld_member_id);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}

LOCAL gint32 get_systems_storages_ld_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    gint32 ret;
    gchar ld_name[MAX_MEM_ID_LEN] = {0};
    guint8 raid_id = 0;
    ret = op_get_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);
    
    ret = ld_get_member_id(i_paras->obj_handle, ld_name, MAX_MEM_ID_LEN, &raid_id);
    
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "%s: get member id fail.", __FUNCTION__));

    *o_result_jso = json_object_new_string((const gchar *)ld_name);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __FUNCTION__));
    

    return HTTP_OK;
}


LOCAL gchar *redfish_raidlevel_to_volumetype(guint8 raid_level)
{
    gchar *volume_type = NULL;

#define VOLUME_TYPE_NONREDUNDANT "NonRedundant"
#define VOLUME_TYPE_MIRRORED "Mirrored"
#define VOLUME_TYPE_STRIPEDWITHPRITY "StripedWithParity"
#define VOLUME_TYPE_SPANNEDMIRROES "SpannedMirrors"
#define VOLUME_TYPE_SPANNEDSRIPESWITHPARITY "SpannedStripesWithParity"

    switch (raid_level) {
        case RAID_LEVEL_0:
            volume_type = VOLUME_TYPE_NONREDUNDANT;
            break;
        case RAID_LEVEL_1:
        case RAID_LEVEL_1ADM:
        case RAID_LEVEL_1TRIPLE:
            volume_type = VOLUME_TYPE_MIRRORED;
            break;
        case RAID_LEVEL_5:
        case RAID_LEVEL_6:
            volume_type = VOLUME_TYPE_STRIPEDWITHPRITY;
            break;
        case RAID_LEVEL_10:
        case RAID_LEVEL_10ADM:
        case RAID_LEVEL_10TRIPLE:
            volume_type = VOLUME_TYPE_SPANNEDMIRROES;
            break;
        case RAID_LEVEL_50:
        case RAID_LEVEL_60:
            volume_type = VOLUME_TYPE_SPANNEDSRIPESWITHPARITY;
            break;
        default:
            debug_log(DLOG_DEBUG, "Invalid raid level is %d", raid_level);
    }

    return volume_type;
}


LOCAL gint32 get_systems_storages_ld_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 raid_level = 0;
    gchar *volume_type = NULL;

    ret = op_get_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);
    
    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_LOGICAL_DRIVE_RAID_LEVEL, &raid_level);
    
    volume_type = redfish_raidlevel_to_volumetype(raid_level);
    return_val_if_expr(NULL == volume_type, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string((const gchar *)volume_type);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}

LOCAL gint32 get_systems_storages_ld_capacity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint32 out_val = 0;
    gint64 size;

    ret = op_get_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);
    
    (void)dal_get_property_value_uint32(i_paras->obj_handle, PROPERTY_LOGICAL_DRIVE_SIZE, &out_val);
    
    return_val_do_info_if_expr(STORAGE_INFO_INVALID_DWORD == out_val, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:invalid ld size", __FUNCTION__));

    
    size = 1024 * 1024 * (gint64)out_val;

    *o_result_jso = json_object_new_int64(size);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_int64 fail.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL const gchar *get_status_state(OBJ_HANDLE obj_handle)
{
    const gchar *state = NULL;
    guint8 state_num = 0;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_STATUS, &state_num);

    for (guint8 i = 0; i < G_N_ELEMENTS(g_ld_rsc_state_map); i++) {
        if (state_num == g_ld_rsc_state_map[i].raw_state) {
            return g_ld_rsc_state_map[i].rsc_state;
        }
    }

    debug_log(DLOG_DEBUG, "%s: invalid state is %d", __FUNCTION__, state_num);
    return state;
}


LOCAL gchar *get_status_health(OBJ_HANDLE obj_handle)
{
    gchar *health = NULL;
    guint8 health_num = 0;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_HEALTH, &health_num);

    
    guint8 level = LEVEL_UNKNOWN;
    (void)sml_ld_state2str(health_num, &level);
    if (level == LEVEL_NORMAL) {
        health = RF_STATUS_OK;
    } else if (level == LEVEL_WARNING || level == LEVEL_CRITICAL) {
        health = RF_STATUS_WARNING;
    } else {
        debug_log(DLOG_DEBUG, "%s: invalid health is %d", __FUNCTION__, health_num);
    }

    return health;
}


LOCAL void get_status_severity(OBJ_HANDLE obj_handle, json_object **o_result_jso)
{
    gint32 ret;
    gchar *severity = NULL;
    guint8 severity_num = 0;
    json_object *oem = NULL;
    json_object *huawei = NULL;

    return_do_info_if_expr(NULL == o_result_jso || NULL == *o_result_jso,
        debug_log(DLOG_ERROR, "%s:input error", __FUNCTION__));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_HEALTH, &severity_num);
    return_do_info_if_expr(VOS_ERR == ret, debug_log(DLOG_ERROR, "%s:get HealthStatus error", __FUNCTION__));

    
    guint8 level = LEVEL_UNKNOWN;
    (void)sml_ld_state2str(severity_num, &level);
    if (level == LEVEL_NORMAL) {
        severity = INFORMATIONAL_STRING;
    } else if (level == LEVEL_WARNING || level == LEVEL_CRITICAL) {
        severity = MINOR_STRING;
    } else {
        debug_log(DLOG_DEBUG, "%s: invalid severity is %u", __FUNCTION__, severity_num);
    }

    huawei = json_object_new_object();
    if (NULL == huawei) {
        debug_log(DLOG_ERROR, "%s : New object fail", __FUNCTION__);
        return;
    }
    oem = json_object_new_object();
    if (NULL == oem) {
        json_object_put(huawei);
        debug_log(DLOG_ERROR, "%s : New oem fail", __FUNCTION__);
        return;
    }
    if (NULL == severity) {
        json_object_object_add(huawei, RFPROP_SEL_SEVERITY, NULL);
    } else {
        json_object_object_add(huawei, RFPROP_SEL_SEVERITY, json_object_new_string(severity));
    }
    json_object_object_add(oem, RFPROP_COMMON_HUAWEI, huawei);
    json_object_object_add(*o_result_jso, RFPROP_COMMON_OEM, oem);

    return;
}

LOCAL gint32 get_systems_storages_ld_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    const gchar *state = NULL;
    gchar *health = NULL;
    json_object *property = NULL;

    ret = op_get_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__));
    
    state = get_status_state(i_paras->obj_handle);
    
    if (NULL == state) {
        debug_log(DLOG_DEBUG, "%s:invalid state", __FUNCTION__);
    } else {
        property = json_object_new_string(state);
        do_info_if_true(NULL == property, debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__));
    }
    json_object_object_add(*o_result_jso, RFPROP_VOLUME_STATUS_STATE, property);
    property = NULL;
    
    health = get_status_health(i_paras->obj_handle);
    
    if (NULL == health) {
        debug_log(DLOG_DEBUG, "%s:invalid health", __FUNCTION__);
    } else {
        property = json_object_new_string((const char *)health);
        do_info_if_true(NULL == property, debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__));
    }
    json_object_object_add(*o_result_jso, RFPROP_VOLUME_STATUS_HEALTH, property);
    (void)get_status_severity(i_paras->obj_handle, o_result_jso);

    return HTTP_OK;
}

LOCAL gint32 get_systems_storages_ld_strip_size_bytes_cmcc(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
#define OPTIMUM_IO_SIZE_512 512
#define OPTIMUM_IO_SIZE_1K 1024
    gint32 ret;
    guint8 strip_size = 0;
    gint32 optimum_io_size = 0;

    if (!dal_is_customized_by_cmcc()) {
        return HTTP_OK;
    }

    ret = op_get_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }
    
    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_LOGICAL_DRIVE_STRIP_SZIE, &strip_size);
    
    
    if (0 == strip_size) {
        optimum_io_size = OPTIMUM_IO_SIZE_512;
    } else if (strip_size <= 21) { 
        optimum_io_size = OPTIMUM_IO_SIZE_1K * (1 << (strip_size - 1));
    } else {
        debug_log(DLOG_DEBUG, "%s:invalid strip size is %d", __FUNCTION__, strip_size);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_int(optimum_io_size);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_int fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_systems_storages_ld_optimum_io_size(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 strip_size = 0;
    gint32 optimum_io_size = 0;

    ret = op_get_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);
    
    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_LOGICAL_DRIVE_STRIP_SZIE, &strip_size);
    
    if (0 == strip_size) {  // bmc中的条带大小0=512, 1=1K, 2=2K, 3=4K, 7=64K, 8=128K，9=256K，10=512K，11=1MB
        optimum_io_size = 512;  // bmc中的条带大小0=512
    } else if (strip_size <= 21) { // optimum_io_size 最大存放2^31-1,指数31-10 就不会越界
        optimum_io_size = 1024 * (1 << (strip_size - 1)); // optimum_io_size 最大存放2^31-1,指数31-10 就不会越界
    } else {
        debug_log(DLOG_DEBUG, "%s:invalid strip size is %d", __FUNCTION__, strip_size);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_int(optimum_io_size);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_int fail.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL void add_associated_fgi_task(json_object *progress_obj, const gchar *ld_uri)
{
    int iRet;
    errno_t safe_fun_ret;
    gint32 task_id = 0;
    gint32 ret;
    char task_rsc_path[MAX_URI_LENGTH] = {0};
    char action_url[MAX_URI_LENGTH] = {0};
    json_object *odata_obj = NULL;
    const gchar *full_init_body = "{\"Type\":\"FullInit\"}";
    json_object *full_init_body_jso = NULL;

    return_if_expr(NULL == progress_obj || NULL == ld_uri);

    // 根据逻辑盘URL拼接初始化action的URL
    safe_fun_ret = strncpy_s(action_url, sizeof(action_url), ld_uri, sizeof(action_url) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    safe_fun_ret = strncat_s(action_url, sizeof(action_url), RFPROP_SYSTEM_VOLUME_ACTION_SUFFIX,
        strlen(RFPROP_SYSTEM_VOLUME_ACTION_SUFFIX));
    do_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    // 构造完全初始化的请求体
    full_init_body_jso = json_tokener_parse(full_init_body);
    return_if_expr(NULL == full_init_body_jso);

    ret = check_redfish_running_task_exist(volume_full_fgi_status_monitor, full_init_body_jso,
        (const gchar *)action_url, &task_id);
    json_object_put(full_init_body_jso);

    return_if_fail(TRUE == ret);

    odata_obj = json_object_new_object();
    return_if_expr(NULL == odata_obj);

    iRet = snprintf_s(task_rsc_path, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s%d", TASK_COLLECTION_PATH, task_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    json_object_object_add(odata_obj, ODATA_ID, json_object_new_string((const char *)task_rsc_path));
    json_object_object_add(progress_obj, RFPROP_VOLUME_OPERATIONS_ASSOCIATED_TASK, odata_obj);
}


LOCAL void rf_get_volume_init_task_name(OBJ_HANDLE obj_handle, gchar *task_name, guint32 name_len)
{
    guint8 ctrl_id = 0;
    guint16 ld_id = 0;
    gchar ref_ctrl_obj_name[MAX_NAME_SIZE] = {0};
    OBJ_HANDLE ctrl_obj = 0;
    gint32 retval;
    return_if_expr(0 == obj_handle || NULL == task_name);

    (void)dal_get_property_value_uint16(obj_handle, PROPERTY_LOGICAL_DRIVE_TARGET_ID, &ld_id);
    (void)dal_get_property_value_string(obj_handle, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER, ref_ctrl_obj_name,
        sizeof(ref_ctrl_obj_name));

    retval = dfl_get_object_handle(ref_ctrl_obj_name, &ctrl_obj);
    return_do_info_if_expr(DFL_OK != retval, debug_log(DLOG_ERROR, "Get RAID Controller object handle failed"));

    (void)dal_get_property_value_byte(ctrl_obj, PROPERTY_RAID_CONTROLLER_ID, &ctrl_id);
    return_do_info_if_expr(name_len == 0,
        debug_log(DLOG_ERROR, "%s failed:input param error, name_len is 0.", __FUNCTION__));
    retval = snprintf_s(task_name, name_len, name_len - 1, RFTASK_NAME_VOLUME_TEMPLATE, ctrl_id, ld_id);
    do_val_if_expr(retval <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, retval));
}


LOCAL gint32 get_systems_storages_ld_operations(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret_state;
    gint32 ret_progress;
    gint ret = 0;
    guint8 fgi_progress = 0;
    guint8 current_fgi_state = 0;
    json_object *fgi_obj = NULL;

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();
    return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    ret_state =
        dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_LOGICAL_DRIVE_CURRENT_FGI_STATE, &current_fgi_state);
    ret_progress = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_LOGICAL_DRIVE_FGI_PROGRESS, &fgi_progress);
    if (DFL_OK == ret_state && DFL_OK == ret_progress && 1 == current_fgi_state &&
        STORAGE_INFO_INVALID_BYTE != fgi_progress) { // 1代表当前在fgi中
        fgi_obj = json_object_new_object();
        if (fgi_obj) {
            json_object_object_add(fgi_obj, RFPROP_VOLUME_OPERATIONS_NAME,
                json_object_new_string(RFVALUE_VOLUME_OPERATIONS_NAME_FGI));
            json_object_object_add(fgi_obj, RFPROP_VOLUME_OPERATIONS_PERCENTAGE_COMPLETE,
                json_object_new_int(fgi_progress));
            add_associated_fgi_task(fgi_obj, (const gchar *)i_paras->uri);

            ret = json_object_array_add(*o_result_jso, fgi_obj);
            do_if_expr(ret, debug_log(DLOG_ERROR, "%s:array add failed", __FUNCTION__); (void)json_object_put(fgi_obj));
        }
    }

    return HTTP_OK;
}

LOCAL void get_oem_volume_state(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 state_num = 0;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_STATUS, &state_num);
    const gchar *state = sml_ld_state2str(state_num, NULL);
    if (g_strcmp0(state, STORAGE_INFO_INVALID_STRING) == 0) {
        json_object_object_add(result, RFPROP_VOLUME_STATUS_STATE, NULL);
    } else {
        json_object_object_add(result, RFPROP_VOLUME_STATUS_STATE, json_object_new_string(state));
    }

    return;
}


LOCAL void get_oem_volume_name(OBJ_HANDLE obj_handle, json_object *result)
{
    gchar ld_name[SML_LD_NAME_LENGTH] = {0};

    (void)dal_get_property_value_string(obj_handle, PROPERTY_LOGICAL_DRIVE_NAME, ld_name, sizeof(ld_name));

    json_object *property = json_object_new_string((const gchar *)ld_name);
    if (property == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
    }

    json_object_object_add(result, RFPROP_VOLUME_NAME, property);
    return;
}

LOCAL void get_oem_volume_ref_controller_id(OBJ_HANDLE obj_handle, json_object *result)
{
    gint32 ret;
    gchar raid_controller_name[MAX_NAME_SIZE] = {0};
    guint8 raid_id = 0;
    OBJ_HANDLE raid_obj_handle = 0;

    (void)dal_get_property_value_string(obj_handle, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER, raid_controller_name,
        sizeof(raid_controller_name));
    ret = dfl_get_object_handle(raid_controller_name, &raid_obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:dfl_get_object_handle fail", __FUNCTION__);
        json_object_object_add(result, RFPROP_VOLUME_REF_RAIDCONTROLLER_ID, NULL);
        return;
    }

    (void)dal_get_property_value_byte(raid_obj_handle, PROPERTY_RAID_CONTROLLER_ID, &raid_id);

    json_object *property = json_object_new_int((gint32)raid_id);
    if (property == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_int fail", __FUNCTION__);
    }

    json_object_object_add(result, RFPROP_VOLUME_REF_RAIDCONTROLLER_ID, property);
    return;
}

LOCAL gint32 raid_level_num_to_str(guint8 level, gchar *level_string, gint32 raid_str_len)
{
    return_val_do_info_if_expr(NULL == level_string, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    if (RAID_LEVEL_1E == level) {
        (void)snprintf_s(level_string, raid_str_len, raid_str_len - 1, "RAID1E");
    } else if (level == RAID_LEVEL_1ADM) {
        (void)snprintf_s(level_string, raid_str_len, raid_str_len - 1, "RAID1(ADM)");
    } else if (level == RAID_LEVEL_10ADM) {
        (void)snprintf_s(level_string, raid_str_len, raid_str_len - 1, "RAID10(ADM)");
    } else if (level == RAID_LEVEL_1TRIPLE) {
        (void)snprintf_s(level_string, raid_str_len, raid_str_len - 1, "RAID1Triple");
    } else if (level == RAID_LEVEL_10TRIPLE) {
        (void)snprintf_s(level_string, raid_str_len, raid_str_len - 1, "RAID10Triple");
    } else if (level <= RAID_LEVEL_60) {
        (void)snprintf_s(level_string, raid_str_len, raid_str_len - 1, "RAID%u", level);
    } else {
        debug_log(DLOG_DEBUG, "%s:invalid raid level is %d", __FUNCTION__, level);
        return VOS_ERR;
    }

    return VOS_OK;
}

void get_oem_raid_level(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 raid_level = RAID_LEVEL_UNKNOWN;
    gint32 ret;
    gchar raid_level_string[RAID_LEVEL_STR] = {0}; 

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_RAID_LEVEL, &raid_level);

    ret = raid_level_num_to_str(raid_level, raid_level_string, RAID_LEVEL_STR);
    if (ret != RET_OK) {
        json_object_object_add(result, RFPROP_VOLUME_RAID_LEVEL, NULL);
        return;
    }

    json_object *property = json_object_new_string((const gchar *)raid_level_string);
    if (property == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
    }

    json_object_object_add(result, RFPROP_VOLUME_RAID_LEVEL, property);
    return;
}

LOCAL gchar *read_policy_num_to_str(guint8 read_policy_num)
{
    gchar *read_policy_str = NULL;

    if (read_policy_num == LD_CACHE_NO_READ_AHEAD) {
        read_policy_str = VOLUME_READ_POLICY_NOREADAHEAD;
    } else if (read_policy_num == LD_CACHE_READ_AHEAD) {
        read_policy_str = VOLUME_READ_POLICY_READAHEAD;
    } else if (read_policy_num == LD_CACHE_READ_AHEAD_ADAPTIVE) {
        read_policy_str = VOLUME_READ_POLICY_READAHEAD_ADAPTIVE;
    } else {
        debug_log(DLOG_DEBUG, "%s: invalid read policy is %d", __FUNCTION__, read_policy_num);
    }

    return read_policy_str;
}

LOCAL void get_oem_default_read_policy(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 default_read_policy_num = 0xFF;
    gchar *default_read_policy_str = NULL;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_DEF_READ_POLICY, &default_read_policy_num);

    default_read_policy_str = read_policy_num_to_str(default_read_policy_num);
    if (default_read_policy_str == NULL) {
        json_object_object_add(result, RFPROP_VOLUME_DEFAULT_READ_POLICY, NULL);
        return;
    }

    json_object *property = json_object_new_string((const gchar *)default_read_policy_str);
    if (property == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
    }

    json_object_object_add(result, RFPROP_VOLUME_DEFAULT_READ_POLICY, property);
    return;
}

LOCAL gint32 get_systems_storages_read_cache_policy_cmcc(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 read_policy_num = 0xFF;
    gchar *read_policy_str = NULL;

    if (!dal_is_customized_by_cmcc()) {
        return HTTP_OK;
    }

    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_LOGICAL_DRIVE_CUR_READ_POLICY, &read_policy_num);

    read_policy_str = read_policy_num_to_str(read_policy_num);
    if (read_policy_str == NULL) {
        debug_log(DLOG_ERROR, "%s: ReadCachePolicy is null", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)read_policy_str);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gchar *write_policy_num_to_str(guint8 write_policy_num)
{
    gchar *write_policy_str = NULL;

    if (LD_CACHE_WRITE_THROUGH == write_policy_num) {
        write_policy_str = VOLUME_WRITE_POLICY_WRITETHROUGH;
    } else if (LD_CACHE_WRITE_BACK == write_policy_num) {
        write_policy_str = VOLUME_WRITE_POLICY_WRITEBACKWITHBBU;
    } else if (LD_CACHE_WRITE_CACHE_IF_BAD_BBU == write_policy_num) {
        write_policy_str = VOLUME_WRITE_POLICY_WRITEBACK;
    } else if (write_policy_num == LD_CACHE_READ_ONLY) {
        write_policy_str = VOLUME_WRITE_POLICY_READONLY;
    } else {
        debug_log(DLOG_DEBUG, "%s: invalid write policy is %d", __FUNCTION__, write_policy_num);
    }

    return write_policy_str;
}

LOCAL void get_oem_default_write_policy(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 default_write_policy_num = 0xFF;
    gchar *default_write_policy_str = NULL;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_DEF_WRITE_POLICY, &default_write_policy_num);

    default_write_policy_str = write_policy_num_to_str(default_write_policy_num);
    if (default_write_policy_str == NULL) {
        json_object_object_add(result, RFPROP_VOLUME_DEFAULT_WRITE_POLICY, NULL);
        return;
    }

    json_object *property = json_object_new_string((const gchar *)default_write_policy_str);
    if (property == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
    }

    json_object_object_add(result, RFPROP_VOLUME_DEFAULT_WRITE_POLICY, property);
    return;
}

LOCAL gchar *cache_policy_num_to_str(guint8 cache_policy_num)
{
    gchar *cache_policy_str = NULL;

    if (LD_CACHE_CACHED_IO == cache_policy_num) {
        cache_policy_str = VOLUME_CACHE_POLICY_CACHEIO;
    } else if (LD_CACHE_DIRECT_IO == cache_policy_num) {
        cache_policy_str = VOLUME_CACHE_POLICY_DRECTIO;
    } else {
        debug_log(DLOG_DEBUG, "%s: invalid cache policy is %d", __FUNCTION__, cache_policy_num);
    }

    return cache_policy_str;
}

LOCAL void get_oem_default_cache_policy(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 default_cache_policy_num = 0xFF;
    gchar *default_cache_policy_str = NULL;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_DEF_CACHE_POLICY, &default_cache_policy_num);

    default_cache_policy_str = cache_policy_num_to_str(default_cache_policy_num);
    if (default_cache_policy_str == NULL) {
        json_object_object_add(result, RFPROP_VOLUME_DEFAULT_CACHE_POLICY, NULL);
        return;
    }

    json_object *property = json_object_new_string((const gchar *)default_cache_policy_str);
    if (property == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
    }

    json_object_object_add(result, RFPROP_VOLUME_DEFAULT_CACHE_POLICY, property);
    return;
}

LOCAL void get_oem_consistency_check(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 consistency_check = 0xFF;
    json_object *property = NULL;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_CC_STATE, &consistency_check);

    if ((0 == consistency_check) || (1 == consistency_check)) {
        property = json_object_new_boolean((json_bool)consistency_check);
        if (property == NULL) {
            debug_log(DLOG_ERROR, "%s:json_object_new_boolean fail", __FUNCTION__);
        }
    } else {
        debug_log(DLOG_DEBUG, "%s:invalid consistency_check value is %d", __FUNCTION__, consistency_check);
    }

    json_object_object_add(result, RFPROP_VOLUME_CONSISTENCY_CHECK, property);
    return;
}

LOCAL void get_oem_span_number(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 span_num = LD_INVALID_VALUE;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_SPAN_DEPTH, &span_num);
    if (span_num == LD_INVALID_VALUE) {
        debug_log(DLOG_DEBUG, "%s:invalid span number is %d fail", __FUNCTION__, span_num);
        json_object_object_add(result, RFPROP_VOLUME_SPAN_NUMBER, NULL);
        return;
    }

    json_object *property = json_object_new_int(span_num);
    if (property == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_boolean fail", __FUNCTION__);
    }

    json_object_object_add(result, RFPROP_VOLUME_SPAN_NUMBER, property);
    return;
}

LOCAL gint32 get_ld_ref_drive_list(OBJ_HANDLE obj_handle, guint8 *pd_id_list, guint32 *pd_num)
{
    int ret;
    GVariant *slot_property = NULL;
    const guchar *pd_slot_id_in_raid = NULL;
    guint32 i; 
    gchar raid_object_name[MAX_NAME_SIZE] = {0};
    GVariant *enclosure_property = NULL;
    const guint16 *pd_enclosure_ids = NULL;
    gsize parm_temp = 0;

    return_val_do_info_if_expr((NULL == pd_id_list) || (NULL == pd_num), VOS_ERR,
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));
    
    ret = dfl_get_property_value(obj_handle, PROPERTY_LOGICAL_DRIVE_PARTICIPATED_PD_ENCLOSURE, &enclosure_property);
    return_val_do_info_if_expr((VOS_OK != ret) || (NULL == enclosure_property), VOS_ERR,
        debug_log(DLOG_ERROR, "%s:dfl_get_property_value fail", __FUNCTION__));

    pd_enclosure_ids = (const guint16 *)g_variant_get_fixed_array(enclosure_property, &parm_temp, sizeof(guint16));
    *pd_num = (guint32)parm_temp;
    return_val_do_info_if_expr(NULL == pd_enclosure_ids, VOS_ERR, g_variant_unref(enclosure_property);
        enclosure_property = NULL; debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__));

    ret = dfl_get_property_value(obj_handle, PROPERTY_LOGICAL_DRIVE_PARTICIPATED_PD_SLOT, &slot_property);
    return_val_do_info_if_expr((VOS_OK != ret) || (NULL == slot_property), VOS_ERR, g_variant_unref(enclosure_property);
        enclosure_property = NULL; debug_log(DLOG_ERROR, "%s:dfl_get_property_value fail", __FUNCTION__));

    pd_slot_id_in_raid = (const guint8 *)g_variant_get_fixed_array(slot_property, &parm_temp, sizeof(guint8));
    *pd_num = (guint32)parm_temp;
    if (pd_slot_id_in_raid == NULL || *pd_num > MAX_DRIVE_NUM) {
        g_variant_unref(enclosure_property);
        g_variant_unref(slot_property);
        debug_log(DLOG_ERROR, "%s: NULL pointer or pd_num out of range, pd_num=%u", __FUNCTION__, *pd_num);
        return VOS_ERR;
    }

    (void)dal_get_property_value_string(obj_handle, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER, raid_object_name,
        sizeof(raid_object_name));

    for (i = 0; i < *pd_num; i++) {
        
        pd_id_list[i] = 0xff;
        dal_get_pd_id_by_slot_and_enclosure(raid_object_name, pd_slot_id_in_raid[i], pd_enclosure_ids[i],
            &pd_id_list[i]);
        
        do_if_expr(pd_id_list[i] == G_MAXUINT8, debug_log(DLOG_DEBUG, "%s:Invalid slot_id=%u,e_id=%u", __FUNCTION__,
            pd_slot_id_in_raid[i], pd_enclosure_ids[i]));
        
    }

    g_variant_unref(slot_property);
    g_variant_unref(enclosure_property);
    
    return VOS_OK;
}

LOCAL gint32 get_pd_uri_by_id(guint8 pd_id, gchar *o_pd_uri, gint32 uri_len)
{
    gint32 ret;
    OBJ_HANDLE pd_obj_handle = 0;
    gchar device_name[STORAGE_CTRL_INFO_LENGTH] = {0};
    gchar pd_location[STORAGE_CTRL_INFO_LENGTH] = {0};
    gchar *location = NULL;
    
    gchar *devicename = NULL;
    
    gchar slot[MAX_RSC_ID_LEN] = {0};

    return_val_do_info_if_expr(NULL == o_pd_uri, VOS_ERR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    (void)dal_get_specific_object_byte(CLASS_HDD_NAME, PROPERTY_HDD_ID, pd_id, &pd_obj_handle);

    (void)dal_get_property_value_string(pd_obj_handle, PROPERTY_HDD_PYSICAL_LOCATION, pd_location, sizeof(pd_location));
    (void)dal_get_property_value_string(pd_obj_handle, PROPERTY_HDD_DEVICENAME, device_name, sizeof(device_name));
    location = dal_clear_string_blank(pd_location, sizeof(pd_location));
    
    devicename = dal_clear_string_blank(device_name, sizeof(device_name));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "%s: get slot fail.", __FUNCTION__));
    
    ret = snprintf_s(o_pd_uri, uri_len, uri_len - 1, CHASSIS_DRIVES_URI, slot, location, devicename);
    return_val_do_info_if_expr(0 >= ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s %d: snprintf_s fail.", __FUNCTION__, __LINE__));
    
    
    return VOS_OK;
}

LOCAL gint32 get_logical_drive_pds_with_single_span(OBJ_HANDLE obj_handle, json_object *property_array)
{
    gint32 ret;
    guint8 pd_id_list[MAX_DRIVE_NUM] = {0};
    guint32 pd_num = 0;
    json_object *span_info = NULL;
    json_object *span_name = NULL;
    json_object *drives = NULL;
    json_object *odata_id_object = NULL;
    json_object *odata_id = NULL;
    guint32 i;
    gchar pd_uri[MAX_URI_LENGTH] = {0};

    return_val_do_info_if_expr(NULL == property_array, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    
    ret = get_ld_ref_drive_list(obj_handle, pd_id_list, &pd_num);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s:json_object_new_object fail", __FUNCTION__));

    span_info = json_object_new_object();
    return_val_do_info_if_expr(NULL == span_info, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__));

    span_name = json_object_new_string("Span0");
    do_info_if_true(NULL == span_name, debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__));
    json_object_object_add(span_info, RFPROP_VOLUME_DRIVE_SPAN_NAME, span_name);

    
    drives = json_object_new_array();
    return_val_do_info_if_expr(NULL == drives, VOS_ERR,
        json_object_object_add(span_info, RFPROP_VOLUME_DRIVE_SPAN_DRIVES, NULL);
        (void)json_object_array_add(property_array, span_info);
        debug_log(DLOG_ERROR, "%s:json_object_new_array fail", __FUNCTION__));

    for (i = 0; i < pd_num; i++) {
        
        continue_if_expr(G_MAXUINT8 == pd_id_list[i]);
        

        ret = get_pd_uri_by_id(pd_id_list[i], pd_uri, MAX_URI_LENGTH);
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "%s:get pd uri fail", __FUNCTION__));

        odata_id = json_object_new_string(pd_uri);
        continue_do_info_if_fail(NULL != odata_id,
            debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__));

        (void)memset_s(pd_uri, MAX_URI_LENGTH, 0, MAX_URI_LENGTH);

        odata_id_object = json_object_new_object();
        continue_do_info_if_fail(NULL != odata_id, (void)json_object_put(odata_id);
            debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__));
        json_object_object_add(odata_id_object, RFPROP_ODATA_ID, odata_id);
        odata_id = NULL;

        ret = json_object_array_add(drives, odata_id_object);
        do_if_expr(ret, (void)json_object_put(odata_id_object);
            debug_log(DLOG_ERROR, "%s:json_object_array_add fail", __FUNCTION__));
    }
    json_object_object_add(span_info, RFPROP_VOLUME_DRIVE_SPAN_DRIVES, drives);

    ret = json_object_array_add(property_array, span_info);
    return_val_do_info_if_expr(ret, VOS_ERR, (void)json_object_put(span_info);
        debug_log(DLOG_ERROR, "%s:json_object_array_add fail", __FUNCTION__));

    return VOS_OK;
}


LOCAL json_object *__get_span_drives(const gchar *array_ref_ctrl, const guint8 *pd_slot_list,
    const guint16 *pd_enclosure_list, guint32 pd_count)
{
    gchar pd_uri[MAX_URI_LENGTH] = {0};
    json_object *odata_id = NULL;

    
    json_object *drives = json_object_new_array();
    if (drives == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_array fail", __FUNCTION__);
        return NULL;
    }

    for (guint32 i = 0; i < pd_count; i++) {
        
        guint8 pd_id = G_MAXUINT8;
        dal_get_pd_id_by_slot_and_enclosure(array_ref_ctrl, pd_slot_list[i],
            pd_enclosure_list[i], &pd_id);
        if (pd_id == G_MAXUINT8) {
            continue;
        }

        json_object *odata_id_object = json_object_new_object();
        if (odata_id_object == NULL) {
            debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__);
            (void)json_object_array_add(drives, NULL);
            continue;
        }

        
        gint32 ret = get_pd_uri_by_id(pd_id, pd_uri, MAX_URI_LENGTH);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: get pd uri fail", __FUNCTION__);
        } else {
            odata_id = json_object_new_string((const gchar *)pd_uri);
            if (odata_id == NULL) {
                debug_log(DLOG_ERROR, "%s: json_object_new_string fail", __FUNCTION__);
            }
            (void)memset_s(pd_uri, MAX_URI_LENGTH, 0, MAX_URI_LENGTH);
        }
        json_object_object_add(odata_id_object, RFPROP_ODATA_ID, odata_id);
        odata_id = NULL;

        ret = json_object_array_add(drives, odata_id_object);
        if (ret != RET_OK) {
            (void)json_object_put(odata_id_object);
            debug_log(DLOG_ERROR, "%s: json_object_array_add fail", __FUNCTION__);
        }
    }

    return drives;
}


LOCAL gint32 get_logical_drive_pds_with_multi_span(OBJ_HANDLE obj_handle, json_object *property_array, gsize array_num,
    const guint16 *array)
{
    guint16 array_id = 0xFFFF;
    gchar ld_ref_ctrl[MAX_NAME_SIZE] = {0};
    gchar array_ref_ctrl[MAX_NAME_SIZE] = {0};
    GVariant *array_ref_pd_slot = NULL;
    GSList *array_obj_list = NULL;
    GSList *array_node = NULL;
    gchar span_string[10] = {0}; 
    GVariant *array_ref_pd_enclosure = NULL;
    guint32 used_space = 0;
    guint32 free_space = 0;
    gsize parm_temp = 0;

    return_val_do_info_if_expr((NULL == property_array) || (NULL == array), VOS_ERR,
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    (void)dal_get_property_value_string(obj_handle, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER, ld_ref_ctrl,
        sizeof(ld_ref_ctrl));

    gint32 ret = dal_get_array_list_by_ctrl(ld_ref_ctrl, &array_obj_list);
    return_val_do_info_if_expr((DFL_OK != ret) || (NULL == array_obj_list), VOS_ERR,
        debug_log(DLOG_ERROR, "%s:dfl_get_object_list fail", __FUNCTION__));
    
    guint32 span_index = 0;
    for (guint32 i = 0; i < array_num; i++) {
        
        for (array_node = array_obj_list; array_node; array_node = array_node->next) {
            
            (void)dal_get_property_value_string((OBJ_HANDLE)(array_node->data), PROPERTY_DISK_ARRAY_REF_RAID_CONTROLLER,
                array_ref_ctrl, sizeof(array_ref_ctrl));
            (void)dal_get_property_value_uint16((OBJ_HANDLE)(array_node->data), PROPERTY_DISK_ARRAY_ID, &array_id);
            
            
            if (g_strcmp0(ld_ref_ctrl, array_ref_ctrl) != 0 || array[i] != array_id) {
                continue;
            }
            
            
            json_object *span_info = json_object_new_object();
            if (span_info == NULL) {
                debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__);
                continue;
            }

            
            (void)snprintf_s(span_string, sizeof(span_string), sizeof(span_string) - 1, "Span%u", span_index);
            json_object *span_name = json_object_new_string((const char *)span_string);
            if (span_name == NULL) {
                debug_log(DLOG_ERROR, "%s: json_object_new_string fail", __FUNCTION__);
                (void)json_object_put(span_info);
                continue;
            }
            json_object_object_add(span_info, RFPROP_VOLUME_DRIVE_SPAN_NAME, span_name);

            
            json_object_object_add(span_info, RFPROP_VOLUME_DRIVE_SPAN_ARRAY_ID,
                json_object_new_int(array_id & ~BIT(15))); // 确保bit15不置位
            (void)dal_get_property_value_uint32((OBJ_HANDLE)(array_node->data), PROPERTY_DISK_ARRAY_USED_SPACE,
                &used_space);
            (void)dal_get_property_value_uint32((OBJ_HANDLE)(array_node->data),
                PROPERTY_DISK_ARRAY_TOTAL_FREE_SPACE, &free_space);
            json_object_object_add(span_info, RFPROP_VOLUME_DRIVE_SPAN_USED_SPACE, json_object_new_int(used_space));
            json_object_object_add(span_info, RFPROP_VOLUME_DRIVE_SPAN_FREE_SPACE, json_object_new_int(free_space));
            get_free_blocks_space((OBJ_HANDLE)(array_node->data), 1, span_info);
            

            
            ret = dfl_get_property_value((OBJ_HANDLE)(array_node->data), PROPERTY_DISK_ARRAY_REF_PD_ENCLOSURES,
                &array_ref_pd_enclosure);
            break_do_info_if_fail(DFL_OK == ret,
                debug_log(DLOG_ERROR, "%s:dfl_get_property_value fail", __FUNCTION__));
            const guint16 *pd_enclosure_list =
                (const guint16 *)g_variant_get_fixed_array(array_ref_pd_enclosure, &parm_temp, sizeof(guint16));
            guint32 pd_count = (guint32)parm_temp;
            
            continue_do_info_if_expr(NULL == pd_enclosure_list, g_variant_unref(array_ref_pd_enclosure);
                array_ref_pd_enclosure = NULL);
            

            
            ret = dfl_get_property_value((OBJ_HANDLE)(array_node->data), PROPERTY_DISK_ARRAY_REF_PD_SLOTS,
                &array_ref_pd_slot);
            break_do_info_if_fail(DFL_OK == ret,
                debug_log(DLOG_ERROR, "%s:dfl_get_property_value fail", __FUNCTION__);
                g_variant_unref(array_ref_pd_enclosure););

            const guint8 *pd_slot_list =
                (const guint8 *)g_variant_get_fixed_array(array_ref_pd_slot, &parm_temp, sizeof(guint8));
            pd_count = (guint32)parm_temp;
            if (NULL == pd_slot_list || (1 == pd_count && 0xff == pd_slot_list[0])) { // 槽位号为0xff表示无效
                json_object_object_add(span_info, RFPROP_VOLUME_DRIVE_SPAN_DRIVES, NULL);
                debug_log(DLOG_DEBUG, "%s:invalid slot id", __FUNCTION__);
            } else {
                json_object *drives =
                    __get_span_drives(array_ref_ctrl, pd_slot_list, pd_enclosure_list, pd_count);
                
                json_object_object_add(span_info, RFPROP_VOLUME_DRIVE_SPAN_DRIVES, drives);
            }
            g_variant_unref(array_ref_pd_slot);
            array_ref_pd_slot = NULL;
            g_variant_unref(array_ref_pd_enclosure);
            array_ref_pd_enclosure = NULL;
            

            
            ret = json_object_array_add(property_array, span_info);
            if (ret != RET_OK) {
                (void)json_object_put(span_info);
                debug_log(DLOG_ERROR, "%s: json_object_array_add fail", __FUNCTION__);
            }
            span_index++;
        }
    }

    g_slist_free(array_obj_list);
    return VOS_OK;
}

LOCAL void get_oem_drives_per_span(OBJ_HANDLE obj_handle, json_object *result)
{
    gint32 ret;
    guint8 span_num = 0;

    
    json_object *property = json_object_new_array();
    if (property == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_array fail", __FUNCTION__);
        goto EXIT;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_SPAN_DEPTH, &span_num);
    return_do_info_if_expr(STORAGE_INFO_INVALID_BYTE == span_num,
        debug_log(DLOG_DEBUG, "%s: invalid span number is %d", __FUNCTION__, span_num));

    
    if (1 == span_num) {
        ret = get_logical_drive_pds_with_single_span(obj_handle, property);
        if (VOS_OK != ret) {
            debug_log(DLOG_ERROR, "%s:get pds fail", __FUNCTION__);
        }
    }
    
    else {
        GVariant *ld_ref_array_list = NULL;
        const guint16 *ld_array_list = NULL;
        gsize array_num = 0;

        
        ret = dfl_get_property_value(obj_handle, PROPERTY_LOGICAL_DRIVE_REF_ARRAY, &ld_ref_array_list);
        return_do_info_if_expr((DFL_OK != ret) || (NULL == ld_ref_array_list),
            debug_log(DLOG_ERROR, "%s:dfl_get_property_value fail", __FUNCTION__));

        ld_array_list = (const guint16 *)g_variant_get_fixed_array(ld_ref_array_list, &array_num, sizeof(guint16));
        return_do_info_if_expr(NULL == ld_array_list, g_variant_unref(ld_ref_array_list);
            debug_log(DLOG_ERROR, "%s:g_variant_get_fixed_array fail", __FUNCTION__));

        ret = get_logical_drive_pds_with_multi_span(obj_handle, property, array_num, ld_array_list);
        if (VOS_OK != ret) {
            debug_log(DLOG_DEBUG, "%s:get pds fail", __FUNCTION__);
        }

        g_variant_unref(ld_ref_array_list);
    }

EXIT:
    json_object_object_add(result, RFPROP_VOLUME_DRIVE_SPANS, property);
    return;
}

LOCAL void get_oem_num_drive_per_span(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 drive_num_per_span = LD_INVALID_VALUE;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_NUMDRIVE_PER_SPAN, &drive_num_per_span);
    if (drive_num_per_span == LD_INVALID_VALUE) {
        debug_log(DLOG_ERROR, "%s:invalid drive number per span is %d fail", __FUNCTION__, drive_num_per_span);
        json_object_object_add(result, PROPERTY_LOGICAL_DRIVE_NUMDRIVE_PER_SPAN, NULL);
        return;
    }

    json_object *property = json_object_new_int(drive_num_per_span);
    if (property == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_int fail", __FUNCTION__);
    }

    json_object_object_add(result, PROPERTY_LOGICAL_DRIVE_NUMDRIVE_PER_SPAN, property);
    return;
}

LOCAL void get_oem_current_read_policy(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 read_policy_num = 0xFF;
    gchar *read_policy_str = NULL;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_CUR_READ_POLICY, &read_policy_num);

    read_policy_str = read_policy_num_to_str(read_policy_num);
    if (read_policy_str == NULL) {
        json_object_object_add(result, RFPROP_VOLUME_CURRENT_READ_POLICY, NULL);
        return;
    }

    json_object *property = json_object_new_string((const gchar *)read_policy_str);
    if (property == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
    }

    json_object_object_add(result, RFPROP_VOLUME_CURRENT_READ_POLICY, property);
    return;
}

LOCAL void get_oem_current_write_policy(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 current_write_policy_num = 0xFF;
    gchar *current_write_policy_str = NULL;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_CUR_WRITE_POLICY, &current_write_policy_num);

    current_write_policy_str = write_policy_num_to_str(current_write_policy_num);
    if (current_write_policy_str == NULL) {
        json_object_object_add(result, RFPROP_VOLUME_CURRENT_WRITE_POLICY, NULL);
        return;
    }

    json_object *property = json_object_new_string((const gchar *)current_write_policy_str);
    if (property == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
    }

    json_object_object_add(result, RFPROP_VOLUME_CURRENT_WRITE_POLICY, property);
    return;
}

LOCAL void get_oem_current_cache_policy(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 current_cache_policy_num = 0xFF;
    gchar *current_cache_policy_str = NULL;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_CUR_CACHE_POLICY, &current_cache_policy_num);

    current_cache_policy_str = cache_policy_num_to_str(current_cache_policy_num);
    if (current_cache_policy_str == NULL) {
        json_object_object_add(result, RFPROP_VOLUME_CURRENT_CACHE_POLICY, NULL);
        return;
    }

    json_object *property = json_object_new_string((const gchar *)current_cache_policy_str);
    if (property == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
    }

    json_object_object_add(result, RFPROP_VOLUME_CURRENT_CACHE_POLICY, property);
    return;
}

LOCAL gchar *access_policy_num_to_str(guint8 access_policy_num)
{
    gchar *access_policy_str = NULL;

    if (LD_ACCESS_RW == access_policy_num) {
        access_policy_str = VOLUME_ACCESS_POLICY_READWRITE;
    } else if (LD_ACCESS_READ_ONLY == access_policy_num) {
        access_policy_str = VOLUME_ACCESS_POLICY_READONLY;
    } else if (LD_ACCESS_BLOCKED == access_policy_num) {
        access_policy_str = VOLUME_ACCESS_POLICY_BLOCKED;
    } else if (LD_ACCESS_HIDDEN == access_policy_num) {
        access_policy_str = VOLUME_ACCESS_POLICY_HIDDEN;
    } else {
        debug_log(DLOG_DEBUG, "%s:invalid access policy is %d", __FUNCTION__, access_policy_num);
    }

    return access_policy_str;
}

LOCAL void get_oem_access_policy(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 access_policy_number = 0xFF;
    gchar *access_policy_string = NULL;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_ACCESS_POLICY, &access_policy_number);

    access_policy_string = access_policy_num_to_str(access_policy_number);
    if (access_policy_string == NULL) {
        json_object_object_add(result, RFPROP_VOLUME_ACCESS_POLICY, NULL);
        return;
    }

    json_object *property = json_object_new_string((const gchar *)access_policy_string);
    if (property == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
    }

    json_object_object_add(result, RFPROP_VOLUME_ACCESS_POLICY, property);
    return;
}

LOCAL void get_oem_boot_priority(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 boot_able = INVALID_DATA_BYTE;
    guint8 boot_priority = INVALID_DATA_BYTE;
    json_object *prop_bootable = NULL;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_BOOTABLE, &boot_able);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_BOOT_PRIORITY, &boot_priority);

    const gchar* boot_priority_name = sml_ctrl_boot_priority_num2str(boot_priority);
    prop_bootable = json_object_new_boolean((json_bool)boot_able);
    if (prop_bootable == NULL) {
        debug_log(DLOG_ERROR, "%s: json function fail", __FUNCTION__);
    }

    if (g_strcmp0(boot_priority_name, STORAGE_INFO_INVALID_STRING) == 0) {
        json_object_object_add(result, RFPROP_VOLUME_BOOT_PRIORITY, NULL);
    } else {
        json_object_object_add(result, RFPROP_VOLUME_BOOT_PRIORITY, json_object_new_string(boot_priority_name));
    }

    json_object_object_add(result, RFPROP_VOLUME_BOOT_ABLE, prop_bootable);

    return;
}

LOCAL void get_oem_bgi_enable(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 bgi_enable = 0xFF;
    json_object *property = NULL;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_BGI_ENABLED, &bgi_enable);

    if ((0 == bgi_enable) || (1 == bgi_enable)) {
        property = json_object_new_boolean((json_bool)bgi_enable);
        if (property == NULL) {
            debug_log(DLOG_ERROR, "%s:json_object_new_boolean fail", __FUNCTION__);
        }
    } else {
        debug_log(DLOG_DEBUG, "%s:invalid boot able value is %d", __FUNCTION__, bgi_enable);
    }

    json_object_object_add(result, RFPROP_VOLUME_BGI_ENABLE, property);
    return;
}

LOCAL void get_oem_ssd_cachecade_volume(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 is_ssd_cache_cade = 0xFF;
    json_object *property = NULL;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_IS_SSCD, &is_ssd_cache_cade);

    if ((0 == is_ssd_cache_cade) || (1 == is_ssd_cache_cade)) {
        property = json_object_new_boolean((json_bool)is_ssd_cache_cade);
        if (property == NULL) {
            debug_log(DLOG_ERROR, "%s:json_object_new_boolean fail", __FUNCTION__);
        }
    } else {
        debug_log(DLOG_DEBUG, "%s:invalid is ssd cachecade volume value is %d", __FUNCTION__, is_ssd_cache_cade);
    }

    json_object_object_add(result, RFPROP_VOLUME_CACHECADE_VOLUME, property);
    return;
}

LOCAL void get_oem_ssd_caching_enable(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 ssd_caching = 0xFF;
    json_object *property = NULL;

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_CACHECADE_LD, &ssd_caching);

    if ((1 == ssd_caching) || (0 == ssd_caching)) {
        property = json_object_new_boolean((json_bool)ssd_caching);
        if (property == NULL) {
            debug_log(DLOG_ERROR, "%s:json_object_new_boolean fail", __FUNCTION__);
        }
    } else {
        debug_log(DLOG_DEBUG, "%s:invalid is  cachecade caching enable value is %d", __FUNCTION__, ssd_caching);
    }

    json_object_object_add(result, RFPROP_VOLUME_SSD_CACHING_ENABLE, property);
    return;
}


LOCAL gsize get_associated_lds(OBJ_HANDLE obj_handle, const gchar *property, guint16 *result_list,
    gsize result_list_size)
{
    GVariant *property_value = NULL;

    gint32 ret = dfl_get_property_value(obj_handle, property, &property_value);
    if (ret != DFL_OK || property_value == NULL) {
        return 0;
    }

    gsize n_elements = 0;
    const guint16 *lds = (const guint16 *)g_variant_get_fixed_array(property_value, &n_elements, sizeof(guint16));
    if (lds == NULL || n_elements == 0 || (n_elements == 1 && lds[0] == STORAGE_INFO_INVALID_WORD) ||
        n_elements > result_list_size) {
        g_variant_unref(property_value);
        return 0;
    }

    errno_t securec_rv = memcpy_s(result_list, result_list_size * sizeof(guint16), lds, n_elements * sizeof(guint16));
    if (securec_rv != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s failed, ret = %d", __FUNCTION__, securec_rv);
    }

    g_variant_unref(property_value);
    return n_elements;
}


LOCAL json_object *get_oem_cachecade_ld_property(OBJ_HANDLE ld_obj, guint16 *result_list, gsize count)
{
    gchar slot[MAX_RSC_ID_LEN] = {0};

    if (redfish_get_board_slot(slot, sizeof(slot)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get slot fail.", __FUNCTION__);
        return NULL;
    }

    json_object *property = json_object_new_array();
    if (property == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_array fail", __FUNCTION__);
        return NULL;
    }

    gchar target_ld_ref_ctrl_name[MAX_NAME_SIZE] = {0};
    (void)dal_get_property_value_string(ld_obj, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER, target_ld_ref_ctrl_name,
        sizeof(target_ld_ref_ctrl_name));

    OBJ_HANDLE ctrl_handle = 0;
    gint32 ret = dfl_get_object_handle(target_ld_ref_ctrl_name, &ctrl_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:dfl_get_object_handle fail", __FUNCTION__);
        return NULL;
    }

    guint8 ctrl_id = STORAGE_INFO_INVALID_BYTE;
    (void)dal_get_property_value_byte(ctrl_handle, PROPERTY_RAID_CONTROLLER_ID, &ctrl_id);

    gchar ld_uri[RF_STORAGE_LD_URI_LEN] = {0};
    
    for (gsize i = 0; i < count; i++) {
        int ret = snprintf_s(ld_uri, sizeof(ld_uri), sizeof(ld_uri) - 1, RFPROP_SYSTEM_REVOLUME, slot, ctrl_id,
            result_list[i]);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
            continue;
        }

        json_object *odata_id = json_object_new_string((const char *)ld_uri);
        if (odata_id == NULL) {
            debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
            continue;
        }

        json_object *odata_id_object = json_object_new_object();
        if (odata_id_object == NULL) {
            (void)json_object_put(odata_id);
            debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__);
        }
        json_object_object_add(odata_id_object, RFPROP_ODATA_ID, odata_id);

        (void)json_object_array_add(property, odata_id_object);
    }

    return property;
}


LOCAL void get_oem_cachecade_ld(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 is_cachecade = 0;
    guint16 result_list[SML_MAX_LOGIC_DRIVES] = {0};
    const gchar* property = NULL;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_IS_SSCD, &is_cachecade);
    if (is_cachecade) {
        property = PROPERTY_LOGICAL_DRIVE_ASSOCIATED_LD;
    } else {
        property = PROPERTY_LOGICAL_DRIVE_ASSOCIATED_CACHECADE;
    }

    gsize count = get_associated_lds(obj_handle, property, result_list, G_N_ELEMENTS(result_list));
    json_object *rf_property = get_oem_cachecade_ld_property(obj_handle, result_list, count);
    json_object *empty_array = json_object_new_array();
    if (empty_array == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_array fail.", __FUNCTION__);
    }

    if (is_cachecade) {
        json_object_object_add(result, REPROP_VOLUME_ASSOCIATED_LD, rf_property);
        json_object_object_add(result, RFPROP_VOLUME_CACHECADE_LD, empty_array);
    } else {
        json_object_object_add(result, REPROP_VOLUME_ASSOCIATED_LD, empty_array);
        json_object_object_add(result, RFPROP_VOLUME_CACHECADE_LD, rf_property);
    }
    return;
}

LOCAL gchar *drive_cache_policy_num_to_str(guint8 drive_cache_policy_num)
{
    gchar *drive_cache_policy_str = NULL;

    if (PD_CACHE_UNCHANGED == drive_cache_policy_num) {
        drive_cache_policy_str = VOLUME_DISK_CACHE_POLICY_UNCHANGED;
    } else if (PD_CACHE_ENABLE == drive_cache_policy_num) {
        drive_cache_policy_str = VOLUME_DISK_CACHE_POLICY_ENABLE;
    } else if (PD_CACHE_DISABLE == drive_cache_policy_num) {
        drive_cache_policy_str = VOLUME_DISK_CACHE_POLICY_DISABLE;
    } else {
        debug_log(DLOG_DEBUG, "%s:invalid drive cache policy is %d", __FUNCTION__, drive_cache_policy_num);
    }

    return drive_cache_policy_str;
}

LOCAL void get_oem_drive_cache_policy(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 drive_cache_policy_num = 0xFF;
    gchar *drive_cache_policy = NULL;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_DISK_CACHE_POLICY, &drive_cache_policy_num);

    drive_cache_policy = drive_cache_policy_num_to_str(drive_cache_policy_num);
    if (drive_cache_policy == NULL) {
        json_object_object_add(result, RFPROP_VOLUME_DRIVE_CACHE_POLICY, NULL);
        return;
    }

    json_object *property = json_object_new_string((const gchar *)drive_cache_policy);
    if (property == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
    }

    json_object_object_add(result, RFPROP_VOLUME_DRIVE_CACHE_POLICY, property);
    return;
}


LOCAL void get_oem_ld_drivename_in_os(OBJ_HANDLE obj_handle, json_object *result)
{
    gchar drive_name[PROP_VAL_LENGTH] = {0};

    (void)dal_get_property_value_string(obj_handle, PROPERTY_LOGICAL_DRIVE_OS_DRIVE_NAME, drive_name, PROP_VAL_LENGTH);
    if (g_strcmp0(STORAGE_INFO_INVALID_STRING, drive_name) == 0) {
        debug_log(DLOG_MASS, "%s:invalid ld drive name", __FUNCTION__);
        json_object_object_add(result, RFPROP_VOLUME_OS_DRIVE_NAME, NULL);
        return;
    }

    json_object *property = json_object_new_string((const char *)drive_name);
    if (property == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
    }

    json_object_object_add(result, RFPROP_VOLUME_OS_DRIVE_NAME, property);
    return;
}


LOCAL void get_oem_ld_init_mode(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 init_state = 0xFF;
    gchar *init_state_str = NULL;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_INIT_STATE, &init_state);

    if (LD_INIT_STATE_NO_INIT == init_state) {
        init_state_str = LD_INITMODE_UN;
    } else if (LD_INIT_STATE_QUICK_INIT == init_state) {
        init_state_str = LD_INITMODE_QUICK;
    } else if (LD_INIT_STATE_FULL_INIT == init_state) {
        init_state_str = LD_INITMODE_FULL;
    } else {
        debug_log(DLOG_MASS, "%s:invalid ld init state is %d", __FUNCTION__, init_state);
        json_object_object_add(result, RFPROP_VOLUME_INITMODE, NULL);
        return;
    }

    json_object *property = json_object_new_string(init_state_str);
    json_object_object_add(result, RFPROP_VOLUME_INITMODE, property);
}

LOCAL void get_oem_ld_accelerator(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 acceleration_method = 0xFF;
    const gchar* acc_method_str = NULL;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_ACCELERATION_METHOD, &acceleration_method);

    if (acceleration_method == LD_ACCELERATOR_NONE) {
        acc_method_str = LD_ACC_METHOD_NONE;
    } else if (acceleration_method == LD_ACCELERATOR_CACHE) {
        acc_method_str = LD_ACC_METHOD_CACHE;
    } else if (acceleration_method == LD_ACCELERATOR_IOBYPASS) {
        acc_method_str = LD_ACC_METHOD_IOBYPASS;
    } else if (acceleration_method == LD_ACCELERATOR_MAXCACHE) {
        acc_method_str = LD_ACC_METHOD_MAXCACHE;
    } else {
        json_object_object_add(result, REPROP_VOLUME_ACCELERATION_METHOD, NULL);
        return;
    }

    json_object *property = json_object_new_string(acc_method_str);
    json_object_object_add(result, REPROP_VOLUME_ACCELERATION_METHOD, property);
    return;
}

LOCAL void get_oem_ld_cache_line_size(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 cache_line_size = 0xFF;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_CACHE_LINE_SIZE, &cache_line_size);

    if (cache_line_size == 0xFF) {
        json_object_object_add(result, REPROP_VOLUME_CACHE_LINE_SIZE, NULL);
        return;
    }

    json_object *property = json_object_new_int(cache_line_size * 64);  // 64k/128k/256k
    json_object_object_add(result, REPROP_VOLUME_CACHE_LINE_SIZE, property);
    return;
}

LOCAL void get_oem_ld_rebuildstate(OBJ_HANDLE obj_handle, json_object *result)
{
    guint8 rebuildstate = INVALID_DATA_BYTE;
    json_object *new_rebuildstate = NULL;

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_REBUILD_STATE, &rebuildstate);

    
    switch (rebuildstate) {
        case 0:
            new_rebuildstate = json_object_new_string(RF_REBUILDSTATE_REBUILDORNOREBUILD);
            break;

        case 1:
            new_rebuildstate = json_object_new_string(RF_REBUILDSTATE_REBUILDING);
            break;

        default:
            debug_log(DLOG_DEBUG, "%s: the value of rebuildstate is invalid.", __FUNCTION__);
    }

    json_object_object_add(result, REPROP_VOLUME_REBUILDSTATE, new_rebuildstate);
    return;
}

LOCAL void get_oem_ld_rebuildprogress(OBJ_HANDLE obj_handle, json_object *result)
{
    gint32 ret;
    guint8 rebuildstate = INVALID_DATA_BYTE;
    guint8 rebuildprogress = INVALID_DATA_BYTE;
    json_object *new_rebuildprogress = NULL;
    gchar str_buf[MAX_STRBUF_LEN + 1] = {0};

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_REBUILD_STATE, &rebuildstate);
    if (rebuildstate != DRIVE_REBUILTING_VALUE) {
        return;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LOGICAL_DRIVE_REBUILDPROGRESS, &rebuildprogress);

    
    if (rebuildprogress == DRIVE_INVALID_VALUE) {
        ret =
            snprintf_s(str_buf, sizeof(str_buf), sizeof(str_buf) - 1, "%s", RF_EBUILDPROGRESS_UNKNOWNREBUILDPROGRESS);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
    } else {
        // sizeof(str_buf)大小为128+1，远大于格式化后的字符串长度，无需判断返回值
        (void)snprintf_s(str_buf, sizeof(str_buf), sizeof(str_buf) - 1, "%u%%", rebuildprogress);
    }

    new_rebuildprogress = json_object_new_string(str_buf);
    json_object_object_add(result, REPROP_VOLUME_REBUILDPROGRESS, new_rebuildprogress);
    return;
}


LOCAL gint32 get_systems_storages_ld_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    gint32 ret;
    json_object *huawei = NULL;
    OBJ_HANDLE obj_handle = 0;

    ret = op_get_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);
    
    obj_handle = i_paras->obj_handle;
    
    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__));

    huawei = json_object_new_object();
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_OK,
        debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__));

    
    get_oem_volume_state(obj_handle, huawei);
    

    
    get_oem_volume_name(obj_handle, huawei);

    
    get_oem_volume_ref_controller_id(obj_handle, huawei);

    
    get_oem_raid_level(obj_handle, huawei);

    
    get_oem_default_read_policy(obj_handle, huawei);

    
    get_oem_default_write_policy(obj_handle, huawei);

    
    get_oem_default_cache_policy(obj_handle, huawei);

    
    get_oem_consistency_check(obj_handle, huawei);

    
    get_oem_span_number(obj_handle, huawei);

    
    get_oem_num_drive_per_span(obj_handle, huawei);

    
    get_oem_drives_per_span(obj_handle, huawei);

    
    get_oem_current_read_policy(obj_handle, huawei);

    
    get_oem_current_write_policy(obj_handle, huawei);

    
    get_oem_current_cache_policy(obj_handle, huawei);

    
    get_oem_access_policy(obj_handle, huawei);

    
    get_oem_boot_priority(obj_handle, huawei);

    
    get_oem_bgi_enable(obj_handle, huawei);

    
    get_oem_ssd_cachecade_volume(obj_handle, huawei);

    
    get_oem_ssd_caching_enable(obj_handle, huawei);

    
    get_oem_cachecade_ld(obj_handle, huawei);

    
    get_oem_drive_cache_policy(obj_handle, huawei);

    
    
    get_oem_ld_drivename_in_os(obj_handle, huawei);
    

    
    get_oem_ld_init_mode(obj_handle, huawei);
    

    get_oem_ld_accelerator(obj_handle, huawei);

    get_oem_ld_cache_line_size(obj_handle, huawei);

    get_oem_ld_rebuildstate(obj_handle, huawei);

    get_oem_ld_rebuildprogress(obj_handle, huawei);

    
    json_object_object_add(*o_result_jso, RFPROP_COMMON_MANUFACTURER, huawei);

    return HTTP_OK;
}

LOCAL gint32 get_links_drives(OBJ_HANDLE obj_handle, json_object *drives)
{
    gint32 ret;
    OBJ_HANDLE pd_obj_handle = 0;
    json_object *drive_object = NULL;
    json_object *odata_id = NULL;
    guint8 pd_list[MAX_DRIVE_NUM] = {0};
    guint32 slot_num = 0;
    guint32 i;
    gchar pd_uri[RF_STORAGE_LD_URI_LEN] = {0};
    gchar pd_location[STORAGE_CTRL_INFO_LENGTH] = {0};
    gchar *location = NULL;
    
    gchar *devicename = NULL;
    
    gchar device_name[STORAGE_CTRL_INFO_LENGTH] = {0};
    gchar slot[MAX_RSC_ID_LEN] = {0};

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "%s:get slot id fail", __FUNCTION__));

    
    ret = get_ld_ref_drive_list(obj_handle, pd_list, &slot_num);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_DEBUG, "%s:get pd list fail", __FUNCTION__));

    
    for (i = 0; i < slot_num; i++) {
        continue_do_info_if_fail(0xff != pd_list[i], debug_log(DLOG_DEBUG, "%s:invalid pd id", __FUNCTION__));

        (void)dal_get_specific_object_byte(CLASS_HDD_NAME, PROPERTY_HDD_ID, pd_list[i], &pd_obj_handle);
        (void)dal_get_property_value_string(pd_obj_handle, PROPERTY_HDD_PYSICAL_LOCATION, pd_location,
            sizeof(pd_location));
        (void)dal_get_property_value_string(pd_obj_handle, PROPERTY_HDD_DEVICENAME, device_name, sizeof(device_name));

        location = dal_clear_string_blank(pd_location, sizeof(pd_location));
        
        devicename = dal_clear_string_blank(device_name, sizeof(device_name));
        
        ret = snprintf_s(pd_uri, sizeof(pd_uri), sizeof(pd_uri) - 1, CHASSIS_DRIVES_URI, slot, location, devicename);
        do_if_expr(0 >= ret, debug_log(DLOG_MASS, "%s %d: snprintf_s fail.", __FUNCTION__, __LINE__); continue);
        
        
        odata_id = json_object_new_string((const char *)pd_uri);
        continue_do_info_if_fail(NULL != odata_id,
            debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__));

        drive_object = json_object_new_object();
        continue_do_info_if_fail(NULL != drive_object, (void)json_object_put(odata_id);
            debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__));
        json_object_object_add(drive_object, RFPROP_ODATA_ID, odata_id);

        ret = json_object_array_add(drives, drive_object);
        do_info_if_true(ret, (void)json_object_put(drive_object);
            debug_log(DLOG_ERROR, "%s:json_object_array_add fail", __FUNCTION__));
    }

    return VOS_OK;
}

LOCAL gint32 get_systems_storages_ld_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint32 drive_num = 0;
    guint8 drive_list[MAX_DRIVE_NUM] = {0};
    json_object *drives = NULL;

    ret = op_get_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__));

    ret = get_ld_ref_drive_list(i_paras->obj_handle, drive_list, &drive_num);
    json_object_object_add(*o_result_jso, RFPROP_VOLUME_LINK_DRIVES_COUNT,
        (ret == VOS_OK) ? json_object_new_int((gint32)drive_num) : NULL);

    drives = json_object_new_array();
    if (NULL != drives) {
        
        ret = get_links_drives(i_paras->obj_handle, drives);
        
        do_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "%s:get drive link fail", __FUNCTION__));
    } else {
        debug_log(DLOG_ERROR, "%s:json_object_new_array fail", __FUNCTION__);
    }
    json_object_object_add(*o_result_jso, RFPROP_VOLUME_LINK_DRIVES, drives);

    return HTTP_OK;
}


LOCAL gint32 get_systems_storages_ld_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar ld_uri[RF_STORAGE_LD_URI_LEN] = {0};
    json_object *action_obj = NULL;

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = ld_get_uri(i_paras->obj_handle, ld_uri, RF_STORAGE_LD_URI_LEN);
    return_val_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR);

    action_obj = json_object_new_object();
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:alloc new object failed", __FUNCTION__));

    rf_add_action_prop(action_obj, (const gchar *)ld_uri, RFACTION_VOLUME_INITIALIZE);

    *o_result_jso = action_obj;

    return HTTP_OK;
}


LOCAL gint32 init_type_str2num(const gchar *init_type_str, guint8 *init_type_num)
{
    guint8 init_type = 0;

    return_val_if_expr(NULL == init_type_str || NULL == init_type_num, VOS_ERR);

    if (0 == g_strcmp0(LD_INITMODE_QUICK, init_type_str)) {
        init_type = LD_INIT_STATE_QUICK_INIT;
    } else if (0 == g_strcmp0(LD_INITMODE_FULL, init_type_str)) {
        init_type = LD_INIT_STATE_FULL_INIT;
    } else if (0 == g_strcmp0(RFACTION_FORMAT_CANCEL_INIT, init_type_str)) {
        return VOS_OK;
    } else {
        return VOS_ERR;
    }

    *init_type_num = init_type;

    return VOS_OK;
}


LOCAL gint32 parse_volume_init_result_code(gint32 result, const gchar *method, json_object **message_jso)
{
    gint32 ret = HTTP_BAD_REQUEST;

    // method指针只是为了记日志，不用入参检测
    return_val_if_expr(NULL == message_jso, HTTP_INTERNAL_SERVER_ERROR);

    switch (result) {
        case VOS_OK:
            (void)create_message_info(MSGID_SUCCESS, NULL, message_jso);
            ret = HTTP_OK;
            break;

        case SML_ERR_LD_INIT_IN_PROGRESS:
            (void)create_message_info(MSGID_VOLUME_INITIALIZING, NULL, message_jso);
            break;
 
        case SM_CODE_NOT_SUPPORT_IN_PRESENT_STATE:
        case SML_ERR_LD_OPERATION_NOT_SUPPORT:
        case SML_ERR_LD_INVALID_TARGET_ID:
        case SML_ERR_I2C_READ_WRITE_FAILED:
        case SML_ERR_CTRL_STATUS_INVALID:
            (void)create_message_info(MSGID_OPERATION_FAILED, NULL, message_jso);
            break;
        case SM_CODE_OPERATION_IN_PROGRESS:
            (void)create_message_info(VMM_MESSAGE_OPERATIONINPROCESS, NULL, message_jso);
            break;
        default:
            debug_log(DLOG_ERROR, "%s:%s result value is 0x%x", __FUNCTION__, method, result);
            ret = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_jso);
            break;
    }

    return ret;
}


LOCAL TASK_MONITOR_INFO_S *full_fgi_monitor_info_new(const gchar *user_name, const gchar *client, OBJ_HANDLE obj_handle)
{
    TASK_MONITOR_INFO_S *monitor_info = NULL;
    
    VOLUME_INIT_TASK_MONITOR_INFO_S *monitor_res = NULL;
    

    return_val_if_expr(NULL == user_name || NULL == client, NULL);

    monitor_info = task_monitor_info_new((GDestroyNotify)g_free);
    if (NULL == monitor_info) {
        debug_log(DLOG_ERROR, "task_monitor_info_new failed");
        return NULL;
    }

    monitor_res = (VOLUME_INIT_TASK_MONITOR_INFO_S *)g_malloc0(sizeof(VOLUME_INIT_TASK_MONITOR_INFO_S));
    if (NULL == monitor_res) {
        debug_log(DLOG_ERROR, "alloc a new json object failed");
        task_monitor_info_free(monitor_info);
        return NULL;
    }

    (void)strncpy_s(monitor_res->user_name, PROP_VAL_LENGTH, user_name, PROP_VAL_LENGTH - 1);
    (void)strncpy_s(monitor_res->client, PROP_VAL_LENGTH, client, PROP_VAL_LENGTH - 1);
    monitor_res->obj_handle = obj_handle;
    monitor_res->get_progress_failed_count = 0;
    monitor_res->before_success = FALSE;

    monitor_info->user_data = (void *)monitor_res;

    return monitor_info;
}


LOCAL gint32 volume_full_fgi_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret1;
    gint32 ret2;
    guint8 full_fgi_progress = 0;
    guint8 current_fgi_state = 0;
    VOLUME_INIT_TASK_MONITOR_INFO_S *monitor_res = NULL;

    return_val_do_info_if_fail((NULL != origin_obj_path) && (NULL != monitor_fn_data) && (NULL != message_obj),
        RF_FAILED, debug_log(DLOG_ERROR, "input param error"));

    monitor_res = (VOLUME_INIT_TASK_MONITOR_INFO_S *)monitor_fn_data->user_data;

    ret1 = dal_get_property_value_byte(monitor_res->obj_handle, PROPERTY_LOGICAL_DRIVE_CURRENT_FGI_STATE,
        &current_fgi_state);
    ret2 =
        dal_get_property_value_byte(monitor_res->obj_handle, PROPERTY_LOGICAL_DRIVE_FGI_PROGRESS, &full_fgi_progress);
    if (VOS_OK != ret1 || VOS_OK != ret2 || 1 != current_fgi_state || STORAGE_INFO_INVALID_BYTE == full_fgi_progress) {
        monitor_res->get_progress_failed_count += 1;

        // 控制器正常刷新属性间隔是30S，这里置为40S
        if (monitor_res->get_progress_failed_count * REDFISH_TASK_INTERVAL_IN_SEC > 40) {
            if (monitor_res->before_success) {
                // 如果之前成功过，现在失败,认为是完成状态。有2种情况
                // 1:手动执行了取消.  2:初始化已完成
                monitor_fn_data->task_state = RF_TASK_COMPLETED;
                monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
            } else {
                monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            }
        }
        goto exit;
    }

    // 读成功了将计数器清空
    monitor_res->get_progress_failed_count = 0;
    monitor_res->before_success = TRUE;

    if (full_fgi_progress <= RF_FINISH_PERCENTAGE) {
        monitor_fn_data->task_state = RF_TASK_RUNNING;
        monitor_fn_data->task_progress = full_fgi_progress;

        if (full_fgi_progress == RF_FINISH_PERCENTAGE) {
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
        }

        (void)create_message_info(MSGID_VOLUME_INITIALIZING, NULL, message_obj);
        goto exit;
    } else {
        debug_log(DLOG_ERROR, "%s:invalid fgi progress=%d", __FUNCTION__, full_fgi_progress);
        goto exception_exit;
    }

exception_exit:
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    monitor_fn_data->task_progress = TASK_NO_PROGRESS;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
exit:
    return RF_OK;
}


LOCAL gint32 act_volume_full_init(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, guint8 init_type)
{
    gint32 ret;
    GSList *input_slist = NULL;
    gint32 available_task_id;
    TASK_MONITOR_INFO_S *full_fgi_monitor_data = NULL;
    gchar task_name[MAX_RSC_NAME_LEN] = {0};

    return_val_do_info_if_expr((NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras) || (LD_INIT_STATE_FULL_INIT != init_type)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s NULL pointer,or invalid init type", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    // 调用full init方法
    input_slist = g_slist_append(input_slist, (gpointer)g_variant_new_byte(init_type));
    ret =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->obj_handle,
        CLASS_LOGICAL_DRIVE_NAME, METHOD_LOGICAL_DRIVE_START_FGI, AUTH_DISABLE, 0, input_slist, NULL);

    uip_free_gvariant_list(input_slist);
    if (VOS_OK != ret) {
        return parse_volume_init_result_code(ret, METHOD_LOGICAL_DRIVE_START_FGI, o_message_jso);
    }

    // 判断是否有可用的任务ID
    available_task_id = find_available_task_id();
    return_val_do_info_if_fail(RF_TASK_LIMIT_EXCEED != available_task_id, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_TASK_LIMIT_EXCEED, NULL, o_message_jso));

    // 申请任务监控资源
    full_fgi_monitor_data = full_fgi_monitor_info_new(i_paras->user_name, i_paras->client, i_paras->obj_handle);
    return_val_do_info_if_expr(NULL == full_fgi_monitor_data, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    // 构造任务名字
    rf_get_volume_init_task_name(i_paras->obj_handle, task_name, sizeof(task_name));

    // 创建redfish任务
    full_fgi_monitor_data->task_progress = TASK_NO_PROGRESS;
    full_fgi_monitor_data->rsc_privilege = USERROLE_BASICSETTING;
    ret = create_new_task(task_name, volume_full_fgi_status_monitor, full_fgi_monitor_data, i_paras->val_jso,
        i_paras->uri, o_message_jso);
    if (RF_OK != ret) {
        task_monitor_info_free(full_fgi_monitor_data);
        debug_log(DLOG_ERROR, "create new task failed");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_ACCEPTED;
}


LOCAL gint32 act_volume_init(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    const gchar *init_type = NULL;
    guint8 num_init_type = 0;
    const gchar *pme_method = NULL;
    GSList *input_list = NULL;

    return_val_do_info_if_expr((NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s NULL pointer", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)get_element_str(i_paras->val_jso, RFACTION_PARAM_TYPE, &init_type);

    ret = init_type_str2num(init_type, &num_init_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    if (0 == g_strcmp0(init_type, LD_INITMODE_QUICK)) {
        pme_method = METHOD_LOGICAL_DRIVE_START_FGI;
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(num_init_type));
    } else if (0 == g_strcmp0(init_type, LD_INITMODE_FULL)) {
        ret = act_volume_full_init(i_paras, o_message_jso, num_init_type);
        return ret;
    } else if (0 == g_strcmp0(init_type, RFACTION_FORMAT_CANCEL_INIT)) {
        pme_method = METHOD_LOGICAL_DRIVE_CANCEL_FGI;
    } else {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_LOGICAL_DRIVE_NAME, pme_method, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);

    return parse_volume_init_result_code(ret, pme_method, o_message_jso);
}


gint32 redfish_check_storages_ld_uri_effective(const gchar *uri, OBJ_HANDLE *obj_handle, gboolean is_action_info)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    gchar ld_uri[RF_STORAGE_LD_URI_LEN] = {0};
    gchar ld_action_url[RF_STORAGE_LD_URI_LEN] = {0};
    gchar ld_action_info_url[RF_STORAGE_LD_URI_LEN] = {0};
    guint8 ld_raid_level = 0;

    return_val_do_info_if_expr(NULL == uri, VOS_ERR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    ret = dfl_get_object_list(CLASS_LOGICAL_DRIVE_NAME, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "%s: get obj_list fail.\n", __FUNCTION__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_RAID_LEVEL,
            &ld_raid_level);
        continue_if_expr(RAID_LEVEL_DELETED == ld_raid_level);
        
        ret = ld_get_uri((OBJ_HANDLE)obj_node->data, ld_uri, RF_STORAGE_LD_URI_LEN);
        continue_if_expr_true(VOS_OK != ret);
        
        
        (void)strncpy_s(ld_action_url, sizeof(ld_action_url), ld_uri, sizeof(ld_action_url) - 1);
        (void)strncat_s(ld_action_url, sizeof(ld_action_url), RFPROP_SYSTEM_VOLUME_ACTION_SUFFIX,
            strlen(RFPROP_SYSTEM_VOLUME_ACTION_SUFFIX));
        if (is_action_info) {
            (void)strncpy_s(ld_action_info_url, sizeof(ld_action_info_url), ld_uri, sizeof(ld_action_info_url) - 1);
            (void)strncat_s(ld_action_info_url, sizeof(ld_action_info_url), RFPROP_SYSTEM_VOLUME_ACTION_INFO_SUFFIX,
                strlen(RFPROP_SYSTEM_VOLUME_ACTION_INFO_SUFFIX));
        }
        if (0 == g_ascii_strcasecmp(ld_uri, uri) || 0 == g_ascii_strcasecmp(ld_action_url, uri) ||
            (is_action_info && 0 == g_ascii_strcasecmp(ld_action_info_url, uri))) {
            
            if (NULL != obj_handle) {
                *obj_handle = (OBJ_HANDLE)obj_node->data;
            }

            g_slist_free(obj_list);

            return VOS_OK;
        }
        
    }
    g_slist_free(obj_list);

    return VOS_ERR;
}

gint32 system_storage_ld_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;

    return_val_do_info_if_expr((VOS_OK != provider_paras_check(i_paras)) || (NULL == count), VOS_ERR,
        debug_log(DLOG_ERROR, "input param error"));

    
    guint8 board_type = 0;
    OBJ_HANDLE obj_handle = 0;

    ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_expr((VOS_OK != ret) || (DISABLE == board_type), HTTP_NOT_FOUND,
        debug_log(DLOG_DEBUG, "%s:The board is switch", __FUNCTION__));
    

    
    ret = redfish_check_storages_ld_uri_effective(i_paras->uri, &obj_handle, FALSE);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_FOUND,
        debug_log(DLOG_INFO, "%s:invalid uri is %s", __FUNCTION__, i_paras->uri));

    i_paras->obj_handle = obj_handle;
    

    *prop_provider = g_system_storage_ld_provider;
    *count = G_N_ELEMENTS(g_system_storage_ld_provider);

    return VOS_OK;
}


LOCAL gint32 get_systems_storages_ld_acinfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar ld_acinfo_uri[RF_STORAGE_LD_URI_LEN] = {0};

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 这里是逻辑盘的标准URI
    ret = ld_get_uri(i_paras->obj_handle, ld_acinfo_uri, RF_STORAGE_LD_URI_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:get uri fail", __FUNCTION__));

    // 在拼接actioninfo的标准后缀
    (void)strncat_s(ld_acinfo_uri, sizeof(ld_acinfo_uri), RFPROP_SYSTEM_VOLUME_ACTION_INFO_SUFFIX,
        strlen(RFPROP_SYSTEM_VOLUME_ACTION_INFO_SUFFIX));

    *o_result_jso = json_object_new_string(ld_acinfo_uri);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}

// 需要代码更新的属性
LOCAL PROPERTY_PROVIDER_S g_system_storage_ld_acinfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_systems_storages_ld_acinfo_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};

gint32 system_storage_ld_init_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    // URL示例
    //   /redfish/v1/Systems/1/Storages/RAIDStorage0/Volumes/LogicalDrive1/InitializeActionInfo
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    return_val_if_expr(NULL == i_paras || NULL == prop_provider || NULL == count, HTTP_INTERNAL_SERVER_ERROR);

    ret = redfish_check_system_uri_valid(i_paras->uri);
    return_val_if_fail(TRUE == ret, HTTP_NOT_FOUND);

    ret = redfish_check_storages_ld_uri_effective(i_paras->uri, &obj_handle, TRUE);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);

    i_paras->obj_handle = obj_handle;

    *prop_provider = g_system_storage_ld_acinfo_provider;
    *count = G_N_ELEMENTS(g_system_storage_ld_acinfo_provider);

    return HTTP_OK;
}