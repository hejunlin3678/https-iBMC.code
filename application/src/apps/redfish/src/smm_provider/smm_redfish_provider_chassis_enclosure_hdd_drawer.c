

#include "redfish_provider.h"

#define POSITION_HDD_DRAWER   16
#define FRU_ID_HDD_DRAWER   15
#define MAX_PROPERTY_STR_VAL 512

LOCAL gint32 smm_get_hdd_drawer_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_get_hdd_drawer_elabel(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_chassis_enclosure_hdd_drawer_provider[] = {
    { RFPROP_CHASSIS_STATUS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, smm_get_hdd_drawer_status, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_CHASSIS_PN, MAP_PROPERTY_NULL, PROPERTY_ELABEL_PRODUCT_PN, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, smm_get_hdd_drawer_elabel, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_CHASSIS_SN, MAP_PROPERTY_NULL, PROPERTY_ELABEL_PRODUCT_SN, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, smm_get_hdd_drawer_elabel, NULL, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_CHASSIS_MFER, MAP_PROPERTY_NULL, PROPERTY_ELABEL_PRODUCT_MFG_NAME, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, smm_get_hdd_drawer_elabel, NULL, NULL, ETAG_FLAG_ENABLE },
};

gint32 smm_chassis_enclosure_hdd_drawer_entry(PROVIDER_PARAS_S* i_paras,
    PROPERTY_PROVIDER_S** prop_provider, guint32* count)
{
    if (!redfish_check_smm_chassis_uri_valid(i_paras->uri)) {
        return HTTP_NOT_FOUND;
    }

    
    if (dal_is_compute_storage_smm_board() == FALSE) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_chassis_enclosure_hdd_drawer_provider;
    *count = sizeof(g_chassis_enclosure_hdd_drawer_provider) / sizeof(PROPERTY_PROVIDER_S);
    
    return VOS_OK;
}


LOCAL gint32 smm_get_hdd_drawer_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    OBJ_HANDLE handle = 0;
    gint32 ret = dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_FRUID, FRU_ID_HDD_DRAWER, &handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get hdd drawer component failed, err code %d", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    guint8 hdd_drawer_health = 0;
    ret = dal_get_property_value_byte(handle, PROPERTY_COMPONENT_HEALTH, &hdd_drawer_health);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get hdd drawer property[%s] failed, err code %d", PROPERTY_COMPONENT_HEALTH, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    get_resource_status_property(&hdd_drawer_health, NULL, ENABLED_STRING, o_result_jso, TRUE);
    return HTTP_OK;
}


LOCAL gint32 smm_get_hdd_drawer_elabel(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    OBJ_HANDLE elabel_handle = 0;
    gint32 ret = get_fru_elabel_handle(POSITION_HDD_DRAWER, &elabel_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Get backplane elabel object handle failed, err code %d", __func__, ret);
        *o_result_jso = NULL;
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    gchar elabel_str[MAX_PROPERTY_STR_VAL] = {0};
    ret = dal_get_property_value_string(elabel_handle,
        g_chassis_enclosure_hdd_drawer_provider[i_paras->index].pme_prop_name, elabel_str, sizeof(elabel_str));
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "Get hdd drawer elabel property[%s] failed, err code %d",
            g_chassis_enclosure_hdd_drawer_provider[i_paras->index].pme_prop_name, ret);
        *o_result_jso = NULL;
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(elabel_str);

    return HTTP_OK;
}