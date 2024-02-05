
#include "redfish_provider.h"

LOCAL gint32 get_hmm_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_hmm_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_hmm_odata_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_hmm_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_hmm_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_hmm_chassis_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_hmm_manufacture(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_hmm_model(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_hmm_serial_number(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_hmm_part_number(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_hmm_asset_tag(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_hmm_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_hmm_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_hmm_thermal_link(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_hmm_power_link(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_hmm_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_hmm_indicator_led(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_hmm_indicator_led(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 obtain_the_led_status(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso,
    guint8 fru_id, guint8 led_id);
LOCAL gint32 turn_the_chassis_and_boards_led(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    const gchar* rf_property, guint8 fru_id, guint8 led_id);
LOCAL void choose_led_fru_with_property(const gchar* property_name, const gchar* member_id,
    guint8 *fru_id, guint8 *led_id);

#define VERSION_LEN 4

LOCAL PROPERTY_PROVIDER_S chassis_hmm_provider[] = {
    { RFPROP_ODATA_CONTEXT,     MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_hmm_odata_context, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_ODATA_ID,          MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_hmm_odata_id, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_ODATA_TYPE,        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_hmm_odata_type, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_ID,                MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_hmm_id, NULL, NULL, ETAG_FLAG_DISABLE },
    { RF_PROPERTY_NAME,         MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_hmm_name, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_CHASSIS_TYPE,      MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_hmm_chassis_type, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_MANUFACTURE,       MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_hmm_manufacture, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_MODEL,             MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_hmm_model, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_SERIAL_NUMBER,     MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_hmm_serial_number, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_PART_NUMBER,       MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_hmm_part_number, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_ASSET_TAG,         MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_hmm_asset_tag, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_CHASSIS_INDICATELED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, get_hmm_indicator_led, set_hmm_indicator_led, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_CHASSIS_HEALTH_LED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        get_hmm_indicator_led, set_hmm_indicator_led, NULL, ETAG_FLAG_ENABLE },
    { RFPROP_STATUS,            MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_hmm_status, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_OEM,               MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_hmm_oem, NULL, NULL, ETAG_FLAG_DISABLE },
    { RF_CHASSIS_THERMAL,       MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_hmm_thermal_link, NULL, NULL, ETAG_FLAG_DISABLE },
    { RF_CHASSIS_POWER,         MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_hmm_power_link, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_LINKS,             MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_hmm_links, NULL, NULL, ETAG_FLAG_DISABLE }
};

gint32 smm_chassis_hmm_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    if (dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_MGNT)) {
        gchar hmm_name[MAX_NAME_SIZE] = { 0 };
        guint8 hmm_index = 0;
        get_smm_name_from_redfish_uri(RF_CHASSIS_UI, i_paras->uri, hmm_name, sizeof(hmm_name));
        (void)get_smm_index(&hmm_index);
        if (hmm_index != hmm_name[3] - '0') { // 第3位为单板槽位，大西洋直通板只获取本板信息
            return HTTP_NOT_FOUND;
        }
    }
    *prop_provider = chassis_hmm_provider;
    *count = sizeof(chassis_hmm_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}

PROPERTY_PROVIDER_S *get_self_provider_service(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S* provider,
    gsize lens)
{
    if ((NULL != i_paras) && (i_paras->index >= 0) && (i_paras->index < lens)) {
        return &provider[i_paras->index];
    }
    return NULL;
}


LOCAL gint32 smm_read_fru_info(gchar *fru_property_name, json_object **o_result_jso)
{
    OBJ_HANDLE elabel_handle = 0;
    GVariant *value_var = NULL;
    const gchar *value_str = NULL;
    gsize value_len = 0;
    gint32 ret;
    
    ret = get_fru_elabel_handle(SMM_POSITION, &elabel_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get motherboard elabel object handle failed, err code %d", __func__, ret);
        *o_result_jso = NULL;
        return VOS_ERR;
    }
    
    ret = dfl_get_property_value(elabel_handle, fru_property_name, &value_var);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get motherboard elabel property[%s] failed, err code %d", __func__,
            PROPERTY_ELABEL_PRODUCT_MFG_NAME, ret);
        *o_result_jso = NULL;
        return VOS_ERR;
    }
    value_str = g_variant_get_string(value_var, &value_len);
    *o_result_jso = NULL;
    if (value_len != 0) {
        *o_result_jso = json_object_new_string(value_str);
    }
    g_variant_unref(value_var);
    return VOS_OK;
}


LOCAL gint32 get_hmm_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    *o_result_jso = json_object_new_string("/redfish/v1/$metadata#Chassis/Members/$entity");
    return HTTP_OK;
}


LOCAL gint32 get_hmm_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet;
    gchar odata_id[MAX_URI_LEN] = { 0 };
    gchar hmm_name[MAX_NAME_SIZE] = { 0 };
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    get_smm_name_from_redfish_uri("/redfish/v1/chassis/", i_paras->uri, hmm_name, sizeof(hmm_name));
    iRet = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, "/redfish/v1/Chassis/%s", hmm_name);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    *o_result_jso = json_object_new_string(odata_id);
    return HTTP_OK;
}

LOCAL gint32 get_hmm_odata_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    *o_result_jso = json_object_new_string("#Chassis.v1_4_0.Chassis");
    return HTTP_OK;
}

LOCAL gint32 get_hmm_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gchar hmm_name[MAX_NAME_SIZE] = { 0 };
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    get_smm_name_from_redfish_uri("/redfish/v1/chassis/", i_paras->uri, hmm_name, sizeof(hmm_name));
    *o_result_jso = json_object_new_string(hmm_name);
    return HTTP_OK;
}

LOCAL gint32 get_hmm_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    *o_result_jso = json_object_new_string("HMM");
    return HTTP_OK;
}

LOCAL gint32 get_hmm_chassis_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    *o_result_jso = json_object_new_string("Module");
    return HTTP_OK;
}

LOCAL gint32 get_hmm_manufacture(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guint8 smm_index = 0;
    gchar smm_name[MAX_NAME_SIZE] = { 0 };
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    get_smm_name_from_redfish_uri("/redfish/v1/chassis/", i_paras->uri, smm_name, sizeof(smm_name));
    (void)get_smm_index(&smm_index);
    if (smm_index != smm_name[3] - '0') { // 备板不在位信息为null
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = smm_read_fru_info(PROPERTY_ELABEL_PRODUCT_MFG_NAME, o_result_jso);
    if (ret != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}

LOCAL gint32 get_hmm_model(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE handle = 0;
    gint32 ret;
    GVariant *product_name_var = NULL;
    const gchar *product_name_str = NULL;
    gsize product_name_len = 0;
    guint8 smm_index = 0;
    gchar smm_name[MAX_NAME_SIZE] = { 0 };
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    get_smm_name_from_redfish_uri("/redfish/v1/chassis/", i_paras->uri, smm_name, sizeof(smm_name));
    (void)get_smm_index(&smm_index);
    if (smm_index != smm_name[3] - '0') { // 备板不在位信息为null
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    ret = dfl_get_object_handle(BMC_PRODUCT_NAME, &handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get object[%s] handle failed, err code %d", __func__, BMC_PRODUCT_NAME, ret);
        *o_result_jso = NULL;
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    ret = dfl_get_property_value(handle, PROPERTY_PRODUCT_NAME, &product_name_var);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "[%s] Get property[%s] of object[%s] failed, err code %d", __func__,
            PROPERTY_PRODUCT_NAME, BMC_PRODUCT_NAME, ret);
        *o_result_jso = NULL;
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    product_name_str = g_variant_get_string(product_name_var, &product_name_len);
    *o_result_jso = NULL;
    if (product_name_len != 0) {
        *o_result_jso = json_object_new_string(product_name_str);
    }
    g_variant_unref(product_name_var);
    return HTTP_OK;
}

LOCAL gint32 get_hmm_serial_number(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guint8 smm_index = 0;
    gchar smm_name[MAX_NAME_SIZE] = { 0 };
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    get_smm_name_from_redfish_uri("/redfish/v1/chassis/", i_paras->uri, smm_name, sizeof(smm_name));
    (void)get_smm_index(&smm_index);
    if (smm_index != smm_name[3] - '0') { // 备板不在位信息为null
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = smm_read_fru_info(PROPERTY_ELABEL_PRODUCT_SN, o_result_jso);
    if (ret != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}

LOCAL gint32 get_hmm_part_number(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar smm_name[MAX_NAME_SIZE] = { 0 };
    guint8 smm_index = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    get_smm_name_from_redfish_uri("/redfish/v1/chassis/", i_paras->uri, smm_name, sizeof(smm_name));
    (void)get_smm_index(&smm_index);
    if (smm_index != smm_name[3] - '0') { // 备板不在位信息为null
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = smm_read_fru_info(PROPERTY_ELABEL_PRODUCT_PN, o_result_jso);
    if (ret != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}

gint32 get_health_and_uid_status(PROPERTY_PROVIDER_S* provider, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    guint8 led_id = 0xFF;
    guint8 fru_id = MAIN_FRUID;

    choose_led_fru_with_property(provider->property_name, i_paras->member_id, &fru_id, &led_id);

    gint32 ret = obtain_the_led_status(i_paras, o_result_jso, fru_id, led_id);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}

gint32 set_health_and_uid_status(PROPERTY_PROVIDER_S* provider, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    
    if ((o_message_jso == NULL) || (i_paras == NULL) || (i_paras->val_jso == NULL) ||
        provider_paras_check(i_paras) != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    guint8 led_id = 0xFF;
    guint8 fru_id = MAIN_FRUID;

    choose_led_fru_with_property(provider->property_name, i_paras->member_id, &fru_id, &led_id);

    return turn_the_chassis_and_boards_led(i_paras, o_message_jso, provider->property_name, fru_id, led_id);
}

LOCAL gint32 get_hmm_indicator_led(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    
    if (dal_get_uid_location() == UID_LOCATION_MAINBOARD) {
        return get_indicator_led_status(i_paras, o_message_jso, o_result_jso);
    } else if (dal_get_uid_location() == UID_MAINBOARD_AND_CHASSIS) {
        return get_health_and_uid_status(get_self_provider_service(i_paras, chassis_hmm_provider,
            G_N_ELEMENTS(chassis_hmm_provider)), i_paras, o_message_jso, o_result_jso);
    }
    
    return HTTP_OK;
}


LOCAL gint32 set_hmm_indicator_led(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    
    if (dal_get_uid_location() == UID_LOCATION_MAINBOARD) {
        return set_indicator_led_status(i_paras, o_message_jso, o_result_jso);
    } else if (dal_get_uid_location() == UID_MAINBOARD_AND_CHASSIS) {
        return set_health_and_uid_status(get_self_provider_service(i_paras, chassis_hmm_provider,
            G_N_ELEMENTS(chassis_hmm_provider)), i_paras, o_message_jso, o_result_jso);
    }
    
    return HTTP_BAD_REQUEST;
}


gint32 get_indicator_led_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE handle = 0;
    gint32 ret;
    json_object *state_obj = NULL;
    guint8 on_duration = 0;
    guint8 off_duration = 0;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] NULL pointer.", __func__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    ret = dfl_get_object_handle(UID_LED, &handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get UID Led object failed, err code %d", __func__, ret);
        state_obj = json_object_new_string("Unknown");
        json_object_object_add(*o_result_jso, RFPROP_CHASSIS_INDICATELED, state_obj);
        return HTTP_OK;
    }
    
    (void)dal_get_property_value_byte(handle, ON_DURATION, &on_duration);
    (void)dal_get_property_value_byte(handle, OFF_DURATION, &off_duration);
    
    if (on_duration == 0 && off_duration == 0) { 
        state_obj = json_object_new_string(RF_LED_OFF);
    } else if (on_duration > 0 && off_duration == 0) { 
        state_obj = json_object_new_string(RF_LED_LIT);
    } else if (on_duration > 0 && off_duration > 0) { 
        state_obj = json_object_new_string(RF_LED_BLINKING);
    }
    
    *o_result_jso = state_obj;
    return HTTP_OK;
}


gint32 set_indicator_led_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    const gchar *uid_state = NULL;
    GSList *input_list = NULL;
    gint32 ret;
    OBJ_HANDLE handle = 0;
    
    if (o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_CHASSIS_INDICATELED, o_message_jso,
        RFPROP_CHASSIS_INDICATELED));
    ret = dfl_get_object_handle(UID_LED, &handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get object %s handle failed, err code %d", __func__, UID_LED, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    uid_state = dal_json_object_get_str(i_paras->val_jso);
    if (0 == g_strcmp0(RF_LED_LIT, uid_state)) {
        input_list = g_slist_append(input_list, g_variant_new_byte(TURN_ON_IDENTIFY_LED));
        input_list = g_slist_append(input_list, g_variant_new_byte(RF_CHASSIS_LED_LOCK_STATE));
    } else if (0 == g_strcmp0(RF_LED_OFF, uid_state)) {
        input_list = g_slist_append(input_list, g_variant_new_byte(TURN_OFF_IDENTIFY_LED));
        input_list = g_slist_append(input_list, g_variant_new_byte(RF_CHASSIS_LED_LOCK_STATE));
    } else if (0 == g_strcmp0(RF_LED_BLINKING, uid_state) &&
        (dal_get_uid_location() == UID_LOCATION_MAINBOARD)) {
        input_list = g_slist_append(input_list, g_variant_new_byte(TURN_ON_IDENTIFY_LED));
        input_list = g_slist_append(input_list, g_variant_new_byte(RF_CHASSIS_LED_BLINKING_TIME));
    } else {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_CHASSIS_INDICATELED, o_message_jso, uid_state,
            RFPROP_CHASSIS_INDICATELED);
        return HTTP_BAD_REQUEST;
    }
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, handle,
        LED_CLASS_NAME, METHOD_UIDLED_STATE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    return ret_value_judge(i_paras, o_message_jso, ret);
}

LOCAL gint32 obtain_the_led_status(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso,
    guint8 fru_id, guint8 led_id)
{
    OBJ_HANDLE obj_handle = 0;

    gint32 ret = dal_get_object_handle_nth(LED_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get led handle failed, fru_id[%d], led_id[%d], ret[%d]",
            fru_id, led_id, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    GSList *input_list = NULL;
    GSList* output_list = NULL;
    
    input_list = g_slist_append(input_list, g_variant_new_byte(fru_id)); 
    input_list = g_slist_append(input_list, g_variant_new_byte(led_id)); 

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        LED_CLASS_NAME, METHOD_PICMG_GET_LED, AUTH_ENABLE, i_paras->user_role_privilege, input_list, &output_list);
    uip_free_gvariant_list(input_list);

    guint8 out_val = g_variant_get_byte(g_slist_nth_data(output_list, 0));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    if (out_val == RF_CHASSIS_LED_OFF_VALUE) {
        *o_result_jso = json_object_new_string(RF_LED_OFF);
    } else if (out_val == RF_CHASSIS_LED_LIT_VALUE) {
        *o_result_jso = json_object_new_string(RF_LED_LIT);
    } else {
        *o_result_jso = json_object_new_string(RF_LED_BLINKING);
    }
    return RET_OK;
}

LOCAL gint32 turn_the_chassis_and_boards_led(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    const gchar* rf_property, guint8 fru_id, guint8 led_id)
{
    OBJ_HANDLE obj_handle = 0;

    gint32 ret = dal_get_object_handle_nth(LED_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get led handle failed, fru_id[%d], led_id[%d], ret[%d]",
            fru_id, led_id, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    GSList *input_list = NULL;
    
    input_list = g_slist_append(input_list, g_variant_new_byte(fru_id)); 
    input_list = g_slist_append(input_list, g_variant_new_byte(led_id)); 
    
    if (0 == g_strcmp0(RF_LED_LIT, dal_json_object_get_str(i_paras->val_jso))) {
        input_list = g_slist_append(input_list, g_variant_new_byte(TURN_ON_IDENTIFY_LED));
    } else if (0 == g_strcmp0(RF_LED_OFF, dal_json_object_get_str(i_paras->val_jso))) {
        input_list = g_slist_append(input_list, g_variant_new_byte(RELEASE_OVER_RIDE_STATE)); 
    } else {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, rf_property, o_message_jso,
            dal_json_object_get_str(i_paras->val_jso), rf_property);
        uip_free_gvariant_list(input_list);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(DEFAULT_DURATION_TIME)); 
    input_list = g_slist_append(input_list, g_variant_new_byte(DEFAULT_LED_COLOR)); 

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        LED_CLASS_NAME, METHOD_PICMG_SET_LED, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);
    return ret_value_judge(i_paras, o_message_jso, ret);
}

LOCAL void choose_led_fru_with_property(const gchar* property_name, const gchar* member_id,
    guint8 *fru_id, guint8 *led_id)
{
    if (property_name == NULL || member_id == NULL) {
        return;
    }

    if (!g_strcmp0(property_name, RFPROP_CHASSIS_INDICATELED)) {
        *led_id = LED_UID_NUM_ID;
    } else if (!g_strcmp0(property_name, RFPROP_CHASSIS_HEALTH_LED)) {
        *led_id = HEALTH_LED_ID;
    } else {
        *led_id = 0xff;
    }

    if (!g_strcmp0(member_id, CHASSIS_SHARED_RESOURCES_ENLOSURE)) {
        *fru_id = BASE_FRU_ID;
    } else {
        *fru_id = MAIN_FRUID;
    }
}

LOCAL gint32 get_hmm_asset_tag(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guint8 smm_index = 0;
    gchar smm_name[MAX_NAME_SIZE] = { 0 };
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    get_smm_name_from_redfish_uri("/redfish/v1/chassis/", i_paras->uri, smm_name, sizeof(smm_name));
    (void)get_smm_index(&smm_index);
    if (smm_index != smm_name[3] - '0') { // 备板不在位信息为null
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = smm_read_fru_info(PROPERTY_ELABEL_PRODUCT_ASSET_TAG, o_result_jso);
    if (ret != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}

LOCAL gint32 get_hmm_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guint8 as_status = 0;
    gchar *state_str = NULL;
    guint8 smm_health = 0;
    gchar smm_name[MAX_NAME_SIZE] = { 0 };
    guint8 smm_index = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    get_smm_name_from_redfish_uri("/redfish/v1/chassis/", i_paras->uri, smm_name, sizeof(smm_name));
    (void)get_smm_index(&smm_index);
    if (smm_index == smm_name[3] - '0') { // 查询为本板
        // 主备信息和健康信息
        (void)redfish_get_smm_health(&smm_health);
        // 生成状态信息
        (void)get_board_active_state(&as_status);
        if (ASM_ACTIVE == as_status) {
            state_str = ENABLED_STRING;
        } else {
            state_str = "StandbySpare";
        }

        get_resource_status_property(&smm_health, NULL, state_str, o_result_jso, FALSE);
    } else { // 当透传到备板失败时，会进入此处处理
        *o_result_jso = json_object_new_object();
        json_object_object_add(*o_result_jso, "State", json_object_new_string("Absent"));
        json_object_object_add(*o_result_jso, "Health", NULL);
    }
    return HTTP_OK;
}


LOCAL gint32 _get_smm_fru_value_obj(json_object **fru_obj)
{
    gint32 ret;
    const gchar *value_str = NULL;
    gsize value_len = 0;
    json_object *value_obj = NULL;
    json_object *board_fru = NULL;
    json_object *product_fru = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    OBJ_HANDLE elabel_handle = 0;

    *fru_obj = NULL;

    ret = get_fru_elabel_handle(SMM_POSITION, &elabel_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] get fru elabel handle failed. ret:%d.", __FUNCTION__, ret);
        return VOS_ERR;
    }
    input_list = g_slist_append(input_list, PROPERTY_ELABEL_BOARD_ID);
    input_list = g_slist_append(input_list, PROPERTY_ELABEL_BOARD_MFG_NAME);
    input_list = g_slist_append(input_list, PROPERTY_ELABEL_BOARD_SN);
    input_list = g_slist_append(input_list, PROPERTY_ELABEL_BOARD_PN);
    input_list = g_slist_append(input_list, PROPERTY_ELABEL_BOARD_MFG_TIME);
    input_list = g_slist_append(input_list, PROPERTY_ELABEL_PRODUCT_ID);
    input_list = g_slist_append(input_list, PROPERTY_ELABEL_PRODUCT_VERSION);
    ret = dfl_multiget_property_value(elabel_handle, input_list, &output_list);
    g_slist_free(input_list);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] dfl_multiget_property_value call failed. ret:%d.", __FUNCTION__, ret);
        return VOS_ERR;
    }
    *fru_obj = json_object_new_object();

    
    board_fru = json_object_new_object();
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), &value_len);
    value_obj = NULL;
    do_if_expr(value_len != 0, value_obj = json_object_new_string(value_str););

    json_object_object_add(board_fru, "FruFileId", value_obj);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), &value_len);
    value_obj = NULL;
    do_if_expr(value_len != 0, value_obj = json_object_new_string(value_str););

    json_object_object_add(board_fru, "Manufacturer", value_obj);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 2), &value_len);
    value_obj = NULL;
    do_if_expr(value_len != 0, value_obj = json_object_new_string(value_str););

    json_object_object_add(board_fru, "SerialNumber", value_obj);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 3), &value_len);
    value_obj = NULL;
    do_if_expr(value_len != 0, value_obj = json_object_new_string(value_str););

    json_object_object_add(board_fru, "PartNumber", value_obj);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 4), &value_len);
    value_obj = NULL;
    do_if_expr(value_len != 0, value_obj = json_object_new_string(value_str););

    json_object_object_add(board_fru, "ManufacturingDate", value_obj);
    
    json_object_object_add(*fru_obj, "Board", board_fru);
    
    product_fru = json_object_new_object();
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 5), &value_len);
    value_obj = NULL;
    do_if_expr(value_len != 0, value_obj = json_object_new_string(value_str););

    json_object_object_add(product_fru, "FruFileId", value_obj);
    
    value_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 6), &value_len);
    value_obj = NULL;
    do_if_expr(value_len != 0, value_obj = json_object_new_string(value_str););

    json_object_object_add(product_fru, "Version", value_obj);
    
    json_object_object_add(*fru_obj, "Product", product_fru);

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return VOS_OK;
}

LOCAL gint32 add_odata_link(json_object **huawei, json_object **o_result_jso, const gchar* smm_name)
{
    json_object *value_obj = NULL;
    gchar odata_id[MAX_URI_LEN] = {0};
    gint32 ret;

    
    value_obj = json_object_new_object();
    if (NULL != value_obj) {
        json_object_object_add(*huawei, RFPROP_CHASSIS_THRESHOLD_SENSORS, value_obj);
        ret = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, URI_FORMAT_CHASSIS_SENSORLIST_THRESHOLD,
            smm_name);
        if (ret <= 0) {
            (void)json_object_put(*o_result_jso);
            *o_result_jso = NULL;
            return RF_FAILED;
        }
        json_object_object_add(value_obj, RFPROP_ODATA_ID, json_object_new_string(odata_id));
        value_obj = NULL;
    }

    
    value_obj = json_object_new_object();
    if (NULL != value_obj) {
        json_object_object_add(*huawei, RFPROP_CHASSIS_DISCRETE_SENSORS, value_obj);
        ret = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, URI_FORMAT_CHASSIS_SENSORLIST_DISCRETE,
            smm_name);
        if (ret <= 0) {
            (void)json_object_put(*o_result_jso);
            *o_result_jso = NULL;
            return RF_FAILED;
        }
        json_object_object_add(value_obj, RFPROP_ODATA_ID, json_object_new_string(odata_id));
        value_obj = NULL;
    }

    
    value_obj = json_object_new_object();
    if (value_obj != NULL) {
        json_object_object_add(*huawei, RFRESC_OPTICAL_MODULE, value_obj);
        ret = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, URI_FORMAT_CHASSIS_DIRECT_OPTICAL, smm_name);
        if (ret <= 0) {
            (void)json_object_put(*o_result_jso);
            *o_result_jso = NULL;
            return RF_FAILED;
        }
        json_object_object_add(value_obj, RFPROP_ODATA_ID, json_object_new_string(odata_id));
        value_obj = NULL;
    }

    return RET_OK;
}


LOCAL gint32 get_hmm_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    json_object *huawei = NULL;

    gint32 ret;
    json_object *value_obj = NULL;
    json_object *fru_obj = NULL;
    json_object *ver_obj = NULL;
    guint8 bom_ver = 0;
    gchar bom_version_str[VERSION_LEN] = { 0 };
    gchar pcb_version_str[VERSION_LEN] = { 0 };
    OBJ_HANDLE obj_handle = 0;
    guint8 smm_index = 0;
    gchar smm_name[MAX_NAME_SIZE] = { 0 };
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail.\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    huawei = json_object_new_object();
    if (huawei == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail.\n", __FUNCTION__, __LINE__);
        (void)json_object_put(*o_result_jso);
        *o_result_jso = NULL;
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(*o_result_jso, RFPROP_OEM_HUAWEI, huawei);

    get_smm_name_from_redfish_uri("/redfish/v1/chassis/", i_paras->uri, smm_name, sizeof(smm_name));

    ret = add_odata_link(&huawei, o_result_jso, smm_name);
    if (ret != RET_OK) {
        return ret;
    }

    
    (void)get_smm_index(&smm_index);
    if (smm_index != smm_name[3] - '0') { // 备板不在位,fru信息为null
        json_object_object_add(huawei, "Fru", NULL);
        return HTTP_OK;
    }

    ret = _get_smm_fru_value_obj(&fru_obj);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_OK,
        debug_log(DLOG_ERROR, "[%s]Get smm fru info failed.ret:%d\r\n", __func__, ret));

    json_object_object_add(huawei, "Fru", fru_obj);

    
    ver_obj = json_object_new_object();
    if (ver_obj == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail.\n", __FUNCTION__, __LINE__);
        return HTTP_OK;
    }
    
    ret = uip_get_object_property_guchar(CLASS_MAINBOARD_NAME, PROPERTY_MAINBOARD_BOMID, &bom_ver);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s]Get smm bom version failed.ret:%d\r\n", __func__, ret);
        (void)json_object_put(ver_obj);
        return HTTP_OK;
    }

    (void)snprintf_s(bom_version_str, sizeof(bom_version_str), sizeof(bom_version_str) - 1, "%03u", bom_ver);
    value_obj = NULL;
    if (strlen(bom_version_str) != 0) {
        value_obj = json_object_new_string(bom_version_str);
    }
    json_object_object_add(ver_obj, "BomVersion", value_obj);

    
    ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, 0, &obj_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s]Get fru0 object handle failed.ret:%d\r\n", __func__, ret);
        (void)json_object_put(ver_obj);
        return HTTP_OK;
    }
    (void)dal_get_property_value_string(obj_handle, PROPERTY_FRU_PCB_VERSION, pcb_version_str, sizeof(pcb_version_str));
    value_obj = NULL;
    if (strlen(pcb_version_str) != 0) {
        value_obj = json_object_new_string(pcb_version_str);
    }
    json_object_object_add(ver_obj, "PCBVersion", value_obj);
    
    json_object_object_add(huawei, "Version", ver_obj);

    return HTTP_OK;
}


LOCAL gint32 get_hmm_thermal_link(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    *o_result_jso = json_object_new_object();
    json_object_object_add(*o_result_jso, RFPROP_ODATA_ID,
        json_object_new_string("/redfish/v1/Chassis/Enclosure/Thermal"));
    return HTTP_OK;
}


LOCAL gint32 get_hmm_power_link(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    *o_result_jso = json_object_new_object();
    json_object_object_add(*o_result_jso, RFPROP_ODATA_ID,
        json_object_new_string("/redfish/v1/Chassis/Enclosure/Power"));
    return HTTP_OK;
}


LOCAL gint32 get_hmm_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    json_object *obj = NULL;
    json_object *obj2 = NULL;
    guint8 as_status = 0;
    guint8 smm_index = 0;
    gchar odata_id[MAX_URI_LEN] = { 0 };
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    *o_result_jso = json_object_new_object();
    
    obj = json_object_new_object();
    (void)get_smm_index(&smm_index);
    (void)get_board_active_state(&as_status);
    if (as_status != ASM_ACTIVE) { // 本板非主
        smm_index = smm_index == 1 ? 2 : 1;
    }
    (void)snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, "/redfish/v1/Managers/HMM%u", smm_index);
    json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string(odata_id));
    obj2 = json_object_new_array();
    json_object_array_add(obj2, obj);
    json_object_object_add(*o_result_jso, "ManagedBy", obj2);
    
    obj = json_object_new_object();
    json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string("/redfish/v1/Chassis/Enclosure"));
    json_object_object_add(*o_result_jso, "ContainedBy", obj);
    return HTTP_OK;
}

