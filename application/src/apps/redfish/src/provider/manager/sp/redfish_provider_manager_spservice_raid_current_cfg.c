
#include "redfish_provider.h"
















LOCAL PROPERTY_PROVIDER_S  g_manager_sp_raid_current_provider[] = {
     }
;


gint32 manager_sp_raid_current_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    return_val_do_info_if_expr(FALSE == rf_support_sp_service(), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s, %d: not support", __FUNCTION__, __LINE__));

    if (redfish_check_manager_uri_valid(i_paras->uri)) {
        *prop_provider = g_manager_sp_raid_current_provider;
        *count = 0;
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}


gint32 get_sp_raid_current_file_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    errno_t safe_fun_ret;
    gint32 ret;
    gchar file_name[PROP_VAL_MAX_LENGTH] = {0};

    (void)get_sp_memberid_odata_context_id(o_rsc_jso, MANAGERS_SPSERVICE_RAID_CURRENT_METADATA,
        MANAGER_SPSERVICE_RAID_CURRENT_URI, i_paras->member_id);

    (void)strncpy_s(file_name, sizeof(file_name), i_paras->member_id, sizeof(file_name) - 1);
    
    safe_fun_ret =
        strncat_s(file_name, sizeof(file_name), SPRAID_EXPORT_CFG_FILE_SUFFIX, strlen(SPRAID_EXPORT_CFG_FILE_SUFFIX));
    return_val_do_info_if_expr(safe_fun_ret != EOK, RF_FAILED,
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    ret = get_sp_file_data(i_paras, UMS_TRANS_FILE_ID_SP_RAID_EXPORT, file_name, o_rsc_jso);
    return_val_do_info_if_expr(VOS_OK != ret, RF_RSC_NOT_FOUND,
        debug_log(DLOG_DEBUG, "%s: get sp raid export %s file failed", __FUNCTION__, file_name));

    return RF_OK;
}
