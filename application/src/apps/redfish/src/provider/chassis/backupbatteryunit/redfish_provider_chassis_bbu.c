

#include <math.h>
#include "redfish_provider.h"

#define M2_PRESENCE_INVALID 0x04
#define M2_1_PRESENCE_MASK 0x01
#define M2_2_PRESENCE_MASK 0x02

LOCAL gint32 get_chassis_bbu_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_bbu_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_chassis_bbu_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_chassis_bbu_boardname(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_bbu_battery(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_bbu_fw(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_chassis_bbu_working_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_bbu_working_hours(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_bbu_m2_1_presece(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_bbu_m2_2_presece(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_bbu_power_type(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);

typedef struct {
    guint8 remain_cap; // 剩余电量百分比
    guint8 work_status; // 电池工作状态
    guint32 soh; // 剩余寿命百分比
    guint32 rate_cap; // 额定容量（Wh）
    gdouble voltage; // 电压（V）
    gdouble current; // 电流（I）
    gdouble ablity_coef; // 电池能力系数（%）
    gdouble remain_discharge_time; // 电池剩余放电时间（H）
    gdouble remain_charge_time; // 电池剩余充电时间（H）
} BAT_REMAIN_TIME_FORMULA_PARAM_S;

LOCAL PROPERTY_PROVIDER_S g_chassis_bbu_provider[] = {
    {RFPROP_ODATA_ID, CLASS_BBU_MODULE_NAME, PROPERTY_BBU_MODULE_ID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_bbu_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_ID, CLASS_BBU_MODULE_NAME, PROPERTY_BBU_MODULE_ID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_bbu_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_STATUS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_bbu_status, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BBU_MODULE_BOARDNAME, CLASS_BBU_MODULE_NAME, PROPERTY_BBU_MODULE_BOARDNAME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_bbu_boardname, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BBU_BATTERY_POWER_TYPE, CLASS_BBU_MODULE_NAME, PROPERTY_BBU_MODULE_POWER_TYPE, USERROLE_READONLY,
        SYS_LOCKDOWN_NULL, get_chassis_bbu_power_type, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BBU_MODUlE_FW, CLASS_BBU_MODULE_NAME, PROPERTY_BBU_MODULE_FIRMWARE_VER, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_bbu_fw, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BBU_MODUlE_WORKING_STATE, CLASS_BBU_MODULE_NAME, PROPERTY_BBU_MODULE_WORK_TIME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_bbu_working_state, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BBU_MODUlE_WORKING_HOURS, CLASS_BBU_MODULE_NAME, PROPERTY_BBU_MODULE_WORK_TIME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_bbu_working_hours, NULL, NULL, ETAG_FLAG_DISABLE},
    {RFPROP_BBU_MODUlE_M2_1_PRESENCE, CLASS_BBU_MODULE_NAME, PROPERTY_BBU_MODULE_M2_STATUS, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_bbu_m2_1_presece, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BBU_MODUlE_M2_2_PRESENCE, CLASS_BBU_MODULE_NAME, PROPERTY_BBU_MODULE_M2_STATUS, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_bbu_m2_2_presece, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BBU_MODUlE_BATTERY, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_bbu_battery, NULL, NULL, ETAG_FLAG_DISABLE},
};

LOCAL gint32 redfish_bbu_string_check(gchar *data, guint32 len)
{
    return_val_if_expr(((data == NULL) || (len == 0)), VOS_ERR);
    data[len - 1] = '\0';
    return_val_if_expr((strlen(data) == 0 || g_strcmp0("N/A", data) == 0 || g_strcmp0("Unknown", data) == 0), VOS_ERR);
    return VOS_OK;
}

LOCAL gint32 redfish_bbu_get_property_string(OBJ_HANDLE obj_handle, const gchar *property_name,
    json_object **o_result_jso)
{
    gint32 ret;
    char str_temp[MAX_STRBUF_LEN] = {0};

    return_val_if_expr(o_result_jso == NULL || property_name == NULL, HTTP_INTERNAL_SERVER_ERROR);

    ret = dal_get_property_value_string(obj_handle, property_name, str_temp, sizeof(str_temp));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get %s value fail.", __FUNCTION__, __LINE__, property_name));

    ret = redfish_bbu_string_check(str_temp, sizeof(str_temp));
    return_val_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string((const char *)str_temp);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_chassis_bbu_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guint8 healthe;
    gchar *bbu_status_str = "Enabled"; // 在位即enabled，状态有另外的属性

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_expr(!i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_BBU_HEALTH, &healthe);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get healthe value fail.", __FUNCTION__, __LINE__));

    
    (void)get_resource_status_property(&healthe, NULL, bbu_status_str, o_result_jso, TRUE);
    return HTTP_OK;
}

LOCAL gint32 get_chassis_bbu_working_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 work_status;
    gchar* bbu_status_str[] = {
        "Offline",
        "Init",
        "Idle",
        "Charging",
        "FullCharged",
        "Discharging",
        "OnlineTest",
        "Protect",
        "Shutdown",
        "Sleep",
        "Undefined",
    };

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_expr(!i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_BBU_MODULE_WORK_STATUS, &work_status);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get work_status value fail.", __FUNCTION__, __LINE__));

    if (work_status >= (sizeof(bbu_status_str) / sizeof(bbu_status_str[0]))) {
        work_status = (sizeof(bbu_status_str) / sizeof(bbu_status_str[0])) - 1;
    }

    *o_result_jso = json_object_new_string((const char *)bbu_status_str[work_status]);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_chassis_bbu_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guint8 bbu_id;

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_expr(!i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_BBU_MODULE_ID, &bbu_id);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get property value fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_int(bbu_id);
    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_chassis_bbu_boardname(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_expr(!i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return redfish_bbu_get_property_string(i_paras->obj_handle, PROPERTY_BBU_MODULE_BOARDNAME, o_result_jso);
}

LOCAL gint32 get_chassis_bbu_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int result;
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gint32 bbu_id;

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_expr(!i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = vos_str2int(i_paras->member_id, BASE_10, &bbu_id, NUM_TPYE_INT32);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get processor id from %s failed, ret is %d", i_paras->member_id, ret));

    result = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_CHASSIS_BBU, slot, bbu_id);
    do_val_if_expr(result <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, result));

    *o_result_jso = json_object_new_string((const char *)uri);

    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL gint32 get_chassis_bbu_fw(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_expr(!i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return redfish_bbu_get_property_string(i_paras->obj_handle, PROPERTY_BBU_MODULE_FIRMWARE_VER, o_result_jso);
}

LOCAL gint32 get_chassis_bbu_working_hours(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint32 work_time;

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_expr(!i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_property_value_uint32(i_paras->obj_handle, PROPERTY_BBU_MODULE_WORK_TIME, &work_time);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get work time value fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_int64(work_time);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_chassis_bbu_m2_1_presece(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 m2_status;
    gboolean m2_device1_presence;

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_expr(!i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_BBU_MODULE_M2_STATUS, &m2_status);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get property value fail.", __FUNCTION__, __LINE__));
    
    if (m2_status == M2_PRESENCE_INVALID) {
        *o_result_jso = NULL;
        return HTTP_INTERNAL_SERVER_ERROR;
    } else if (m2_status & M2_1_PRESENCE_MASK) {
        m2_device1_presence = TRUE;
    } else {
        m2_device1_presence = FALSE;
    }

    *o_result_jso = json_object_new_boolean(m2_device1_presence);
    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_chassis_bbu_m2_2_presece(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 m2_status;
    gboolean m2_device2_presence;

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_expr(!i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_BBU_MODULE_M2_STATUS, &m2_status);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get property value fail.", __FUNCTION__, __LINE__));
    
    if (m2_status == M2_PRESENCE_INVALID) {
        *o_result_jso = NULL;
        return HTTP_INTERNAL_SERVER_ERROR;
    } else if (m2_status & M2_2_PRESENCE_MASK) {
        m2_device2_presence = TRUE;
    } else {
        m2_device2_presence = FALSE;
    }

    *o_result_jso = json_object_new_boolean(m2_device2_presence);
    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_bbu_power_type(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso)
{
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "NULL pointer.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    return redfish_bbu_get_property_string(i_paras->obj_handle, PROPERTY_BBU_MODULE_POWER_TYPE, o_result_jso);
}

LOCAL gboolean is_valid_double_for_canbus_battery(gdouble value)
{
    if (fabs(value - BBU_CANBUS_INVALID_FLOAT) < 1) {
        return FALSE;
    }

    return TRUE;
}

LOCAL gboolean is_valid_int16_for_canbus_battery(gint16 value)
{
    return (value != BBU_CANBUS_INVALID_INT16);
}

typedef struct {
    const gchar* prop_name;
    const gchar* rfprop_name;
} PROP_AND_RFPROP_PAIR_S;

LOCAL void get_battery_cell_voltage(OBJ_HANDLE obj_bbu_module, json_object* battery_obj)
{
    static PROP_AND_RFPROP_PAIR_S cell_voltage_rf_pair[] = {
        {PROPERTY_BBU_MODULE_CELL_VOLTAGE1, RFPROP_BBU_CELL_VOLTAGE1},
        {PROPERTY_BBU_MODULE_CELL_VOLTAGE2, RFPROP_BBU_CELL_VOLTAGE2},
        {PROPERTY_BBU_MODULE_CELL_VOLTAGE3, RFPROP_BBU_CELL_VOLTAGE3},
        {PROPERTY_BBU_MODULE_CELL_VOLTAGE4, RFPROP_BBU_CELL_VOLTAGE4},
        {PROPERTY_BBU_MODULE_CELL_VOLTAGE5, RFPROP_BBU_CELL_VOLTAGE5},
        {PROPERTY_BBU_MODULE_CELL_VOLTAGE6, RFPROP_BBU_CELL_VOLTAGE6},
        {PROPERTY_BBU_MODULE_CELL_VOLTAGE7, RFPROP_BBU_CELL_VOLTAGE7},
        {PROPERTY_BBU_MODULE_CELL_VOLTAGE8, RFPROP_BBU_CELL_VOLTAGE8},
        {PROPERTY_BBU_MODULE_CELL_VOLTAGE9, RFPROP_BBU_CELL_VOLTAGE9},
        {PROPERTY_BBU_MODULE_CELL_VOLTAGE10, RFPROP_BBU_CELL_VOLTAGE10},
        {PROPERTY_BBU_MODULE_CELL_VOLTAGE11, RFPROP_BBU_CELL_VOLTAGE11},
        {PROPERTY_BBU_MODULE_CELL_VOLTAGE12, RFPROP_BBU_CELL_VOLTAGE12},
        {PROPERTY_BBU_MODULE_CELL_VOLTAGE13, RFPROP_BBU_CELL_VOLTAGE13},
        {PROPERTY_BBU_MODULE_CELL_VOLTAGE14, RFPROP_BBU_CELL_VOLTAGE14},
        {PROPERTY_BBU_MODULE_CELL_VOLTAGE15, RFPROP_BBU_CELL_VOLTAGE15},
        {PROPERTY_BBU_MODULE_CELL_VOLTAGE16, RFPROP_BBU_CELL_VOLTAGE16},
        {PROPERTY_BBU_MODULE_CELL_VOLTAGE17, RFPROP_BBU_CELL_VOLTAGE17}
    };
    json_object* cell_vol_obj = NULL;
    gdouble double_value;

    cell_vol_obj = json_object_new_object();
    if (cell_vol_obj == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return;
    }

    for (int i = 0; i < G_N_ELEMENTS(cell_voltage_rf_pair); i++) {
        double_value = 0;
        (void)dal_get_property_value_double(obj_bbu_module, cell_voltage_rf_pair[i].prop_name, &double_value);
        if (is_valid_double_for_canbus_battery(double_value) == TRUE) {
            rf_add_prop_json_double(obj_bbu_module, cell_voltage_rf_pair[i].prop_name,
                cell_voltage_rf_pair[i].rfprop_name, cell_vol_obj, ACCURACY_FORMAT_3F);
        } else {
            json_object_object_add(cell_vol_obj, cell_voltage_rf_pair[i].rfprop_name, NULL);
        }
    }
    // Canbus电池没有第18节电芯
    json_object_object_add(cell_vol_obj, RFPROP_BBU_CELL_VOLTAGE18, NULL);

    json_object_object_add(battery_obj, RFPROP_BBU_CELL_VOLTAGE, cell_vol_obj);
}

LOCAL void get_battery_cell_temperature(OBJ_HANDLE obj_bbu_module, json_object* battery_obj)
{
    static PROP_AND_RFPROP_PAIR_S cell_temperature_rf_pair[] = {
        {PROPERTY_BBU_MODULE_CELL_TEMPERATURE1, RFPROP_BBU_CELL_TEMPERATURE1},
        {PROPERTY_BBU_MODULE_CELL_TEMPERATURE2, RFPROP_BBU_CELL_TEMPERATURE2},
        {PROPERTY_BBU_MODULE_CELL_TEMPERATURE3, RFPROP_BBU_CELL_TEMPERATURE3},
        {PROPERTY_BBU_MODULE_CELL_TEMPERATURE4, RFPROP_BBU_CELL_TEMPERATURE4},
        {PROPERTY_BBU_MODULE_CELL_TEMPERATURE5, RFPROP_BBU_CELL_TEMPERATURE5},
        {PROPERTY_BBU_MODULE_CELL_TEMPERATURE6, RFPROP_BBU_CELL_TEMPERATURE6},
        {PROPERTY_BBU_MODULE_CELL_TEMPERATURE7, RFPROP_BBU_CELL_TEMPERATURE7},
        {PROPERTY_BBU_MODULE_CELL_TEMPERATURE8, RFPROP_BBU_CELL_TEMPERATURE8},
        {PROPERTY_BBU_MODULE_CELL_TEMPERATURE9, RFPROP_BBU_CELL_TEMPERATURE9},
        {PROPERTY_BBU_MODULE_CELL_TEMPERATURE10, RFPROP_BBU_CELL_TEMPERATURE10},
        {PROPERTY_BBU_MODULE_CELL_TEMPERATURE11, RFPROP_BBU_CELL_TEMPERATURE11},
        {PROPERTY_BBU_MODULE_CELL_TEMPERATURE12, RFPROP_BBU_CELL_TEMPERATURE12},
        {PROPERTY_BBU_MODULE_CELL_TEMPERATURE13, RFPROP_BBU_CELL_TEMPERATURE13},
        {PROPERTY_BBU_MODULE_CELL_TEMPERATURE14, RFPROP_BBU_CELL_TEMPERATURE14},
        {PROPERTY_BBU_MODULE_CELL_TEMPERATURE15, RFPROP_BBU_CELL_TEMPERATURE15},
        {PROPERTY_BBU_MODULE_CELL_TEMPERATURE16, RFPROP_BBU_CELL_TEMPERATURE16},
        {PROPERTY_BBU_MODULE_CELL_TEMPERATURE17, RFPROP_BBU_CELL_TEMPERATURE17}
    };
    json_object* cell_temp_obj = NULL;
    guint16 uint16_value;

    cell_temp_obj = json_object_new_object();
    if (cell_temp_obj == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return;
    }

    for (int i = 0; i < G_N_ELEMENTS(cell_temperature_rf_pair); i++) {
        uint16_value = 0;
        (void)dal_get_property_value_uint16(obj_bbu_module, cell_temperature_rf_pair[i].prop_name, &uint16_value);
        if (is_valid_int16_for_canbus_battery((gint16)uint16_value) == TRUE) {
            rf_add_property_jso_uint16(obj_bbu_module, cell_temperature_rf_pair[i].prop_name,
                cell_temperature_rf_pair[i].rfprop_name, cell_temp_obj);
        } else {
            json_object_object_add(cell_temp_obj, cell_temperature_rf_pair[i].rfprop_name, NULL);
        }
    }
    // Canbus电池没有第18节电芯
    json_object_object_add(cell_temp_obj, RFPROP_BBU_CELL_TEMPERATURE18, NULL);

    json_object_object_add(battery_obj, RFPROP_BBU_CELL_TEMPERATURE, cell_temp_obj);
}

LOCAL gchar* get_discharging_status_for_canbus(guint8 discharging_status)
{
    if (discharging_status == BBU_DISCHARGE_STATUS_CHARGING) {
        return BBU_DISCHARGE_STATUS_CHARGING_STR;
    } else if (discharging_status == BBU_DISCHARGE_STATUS_DISCHARGING) {
        return BBU_DISCHARGE_STATUS_DISCHARGING_STR;
    } else if (discharging_status == BBU_DISCHARGE_STATUS_FULLY_CHARGED) {
        return BBU_DISCHARGE_STATUS_FULLY_CHARGED_STR;
    } else if (discharging_status == BBU_DISCHARGE_STATUS_IDLE) {
        return BBU_DISCHARGE_STATUS_IDLE_STR;
    } else if (discharging_status == BBU_DISCHARGE_STATUS_UNKNOWN) {
        return BBU_DISCHARGE_STATUS_UNKNOWN_STR;
    }
    return NULL;
}

LOCAL void get_chassis_bbu_battery_canbus(OBJ_HANDLE obj_bbu_module, json_object* battery_obj)
{
    gchar* str_value = NULL;
    guint8 uint8_value;
    guint32 uint32_value;
    gdouble double_value;

    uint8_value = 0;
    (void)dal_get_property_value_byte(obj_bbu_module, PROPERTY_BBU_MODULE_DISCHARGING_STATUS, &uint8_value);
    str_value = get_discharging_status_for_canbus(uint8_value);
    json_object_object_add(battery_obj, RFPROP_BBU_DISCHARGE_STATUS, json_object_new_string(str_value));

    double_value = 0;
    dal_get_property_value_double(obj_bbu_module, PROPERTY_BBU_MODULE_VOLTAGE4, &double_value);
    if (is_valid_double_for_canbus_battery(double_value) == TRUE) {
        rf_add_prop_json_double(obj_bbu_module, PROPERTY_BBU_MODULE_VOLTAGE4,
            RFPROP_BBU_BAT_STR_VOLTAGE, battery_obj, ACCURACY_FORMAT_3F);
    } else {
        json_object_object_add(battery_obj, RFPROP_BBU_BAT_STR_VOLTAGE, NULL);
    }

    double_value = 0;
    dal_get_property_value_double(obj_bbu_module, PROPERTY_BBU_MODULE_BATTERY_STRING_CURRENT, &double_value);
    if (is_valid_double_for_canbus_battery(double_value) == TRUE) {
        rf_add_prop_json_double(obj_bbu_module, PROPERTY_BBU_MODULE_BATTERY_STRING_CURRENT,
            RFPROP_BBU_BAT_STR_CURRENT, battery_obj, ACCURACY_FORMAT_3F);
    } else {
        json_object_object_add(battery_obj, RFPROP_BBU_BAT_STR_CURRENT, NULL);
    }

    double_value = 0;
    dal_get_property_value_double(obj_bbu_module, PROPERTY_BBU_MODULE_VOLTAGE1, &double_value);
    if (is_valid_double_for_canbus_battery(double_value) == TRUE) {
        rf_add_prop_json_double(obj_bbu_module, PROPERTY_BBU_MODULE_VOLTAGE1,
            RFPROP_BBU_BUSBAR_VOLTAGE, battery_obj, ACCURACY_FORMAT_3F);
    } else {
        json_object_object_add(battery_obj, RFPROP_BBU_BUSBAR_VOLTAGE, NULL);
    }

    rf_add_property_jso_uint32(obj_bbu_module, PROPERTY_BBU_MODULE_CELL_TEMP_IN_RANGE_1_TIME,
        RFPROP_BBU_CELL_MAX_TEMP_HIGHER_THAN_60_TIME, battery_obj);

    rf_add_prop_json_double(obj_bbu_module, PROPERTY_BBU_MODULE_DIRE_SAMPL_VOLTAGE,
        RFPROP_BBU_DIRE_SAMPL_VOLTAGE, battery_obj, ACCURACY_FORMAT_3F);

    uint32_value = 0;
    (void)dal_get_property_value_uint32(obj_bbu_module, PROPERTY_BBU_MODULE_REAL_CHARGE_CURRENT, &uint32_value);
    json_object_object_add(battery_obj, RFPROP_BBU_BUSBAR_CURR_SAMPL,
        ex_json_object_new_double(1.0 * uint32_value / BBU_CHARGE_MODIFY, ACCURACY_FORMAT_3F));

    rf_add_prop_json_double(obj_bbu_module, PROPERTY_BBU_MODULE_BATTERY_CURR_SAMPL,
        RFPROP_BBU_BATTERY_CURR_SAMPL, battery_obj, ACCURACY_FORMAT_3F);

    rf_add_prop_json_double(obj_bbu_module, PROPERTY_BBU_MODULE_OUTPUT_POWER,
        RFPROP_BBU_POWER_OUTPUT_WATTS, battery_obj, ACCURACY_FORMAT_3F);

    get_battery_cell_voltage(obj_bbu_module, battery_obj);
    get_battery_cell_temperature(obj_bbu_module, battery_obj);
}

LOCAL void add_basic_prop_for_battery_obj(OBJ_HANDLE obj_handle, json_object *battery_obj)
{
    (void)rf_add_property_jso_uint32(obj_handle, PROPERTY_BBU_MODULE_FULL_CAP, RFPROP_BBU_BATTERY_FULL_CHARGED,
        battery_obj);
    (void)rf_add_property_jso_uint32(obj_handle, PROPERTY_BBU_MODULE_REMAIN_CAP,
        RFPROP_BBU_BATTERY_REMAIN_POWER, battery_obj);
    (void)rf_add_property_jso_byte(obj_handle, PROPERTY_BBU_MODULE_PER_REMAIN_CAP,
        RFPROP_BBU_BATTERY_REMAIN_POWER_PER, battery_obj);

    (void)rf_add_property_jso_string(obj_handle, PROPERTY_BBU_MODULE_BATT_MANU,
        RFPROP_BBU_BATTERY_MANUFACTURER, battery_obj);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_BBU_MODULE_BATT_SN, RFPROP_BBU_BATTERY_SN,
        battery_obj);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_BBU_MODULE_BATT_MANU_DATE,
        RFPROP_BBU_BATTERY_PRODUCTION_DATE, battery_obj);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_BBU_MODULE_BATT_MODEL, RFPROP_BBU_BATTERY_MODEL,
        battery_obj);

    (void)rf_add_property_jso_string(obj_handle, PROPERTY_BBU_MODULE_BATT_PART_NUM,
        RFPROP_BBU_BATTERY_PART_NUMBER, battery_obj);
    (void)rf_add_property_jso_uint32(obj_handle, PROPERTY_BBU_MODULE_STATE_OF_HEALTH,
        RFPROP_BBU_BATTERY_REMAIN_HEALTH_PER, battery_obj);
    (void)rf_add_property_jso_uint32(obj_handle, PROPERTY_BBU_MODULE_RATE_CAP,
        RFPROP_BBU_BATTERY_RATED_CAP_WATT_H, battery_obj);
    (void)rf_add_property_jso_uint16(obj_handle, PROPERTY_BBU_MODULE_BATT_DISCHARGE_TIMES,
        RFPROP_BBU_BATTERY_DISCHARGE_TIMES, battery_obj);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_BBU_MODULE_BATT_SILK_TEXT,
        RFPROP_BBU_BATTERY_DEVICE_LOCATOR, battery_obj);
}

LOCAL void add_cell_temperature_for_battery_obj(OBJ_HANDLE obj_handle, json_object *battery_obj)
{
    gint16 int16_value = 0;

    dal_get_property_value_int16(obj_handle, PROPERTY_BBU_MODULE_CELL_MAX_TEMP, &int16_value);
    if (is_valid_int16_for_canbus_battery(int16_value) == TRUE) {
        rf_add_property_jso_int16(obj_handle, PROPERTY_BBU_MODULE_CELL_MAX_TEMP, RFPROP_BBU_BATTERY_CELL_MAX_TEMP,
            battery_obj);
    } else {
        json_object_object_add(battery_obj, RFPROP_BBU_BATTERY_CELL_MAX_TEMP, NULL);
    }

    int16_value = 0;
    dal_get_property_value_int16(obj_handle, PROPERTY_BBU_MODULE_CELL_MIN_TEMP, &int16_value);
    if (is_valid_int16_for_canbus_battery(int16_value) == TRUE) {
        rf_add_property_jso_int16(obj_handle, PROPERTY_BBU_MODULE_CELL_MIN_TEMP, RFPROP_BBU_BATTERY_CELL_MIN_TEMP,
            battery_obj);
    } else {
        json_object_object_add(battery_obj, RFPROP_BBU_BATTERY_CELL_MIN_TEMP, NULL);
    }
}

LOCAL gint32 check_bat_remain_time_param_value(OBJ_HANDLE obj_handle, BAT_REMAIN_TIME_FORMULA_PARAM_S formula_param)
{
    if ((fabs(formula_param.ablity_coef) < RF_MIN_FLOAT_NUMBER)) {
        debug_log(DLOG_ERROR, "%s: The value of %s is 0.0", __FUNCTION__, PROPERTY_BBU_MODULE_BAT_ABILITY_COEF);
        return RET_ERR;
    }

    if ((fabs(formula_param.current) < RF_MIN_FLOAT_NUMBER)) {
        debug_log(DLOG_ERROR, "%s: The value of %s is 0.0", __FUNCTION__, PROPERTY_BBU_MODULE_BATTERY_CURR_SAMPL);
        return RET_ERR;
    }

    if ((fabs(formula_param.voltage) < RF_MIN_FLOAT_NUMBER)) {
        debug_log(DLOG_ERROR, "%s: The value of %s is 0.0", __FUNCTION__, PROPERTY_BBU_MODULE_VOLTAGE1);
        return RET_ERR;
    }

    if ((formula_param.work_status != BBU_STATE_ON_CHARGE) && (formula_param.work_status != BBU_STATE_ON_DISCHARGE)) {
        debug_log(DLOG_ERROR, "%s: The battery workstatus is %d", __FUNCTION__, formula_param.work_status);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 get_bat_remain_time_prop_value(OBJ_HANDLE obj_handle, BAT_REMAIN_TIME_FORMULA_PARAM_S *formula_param)
{
    gint32 ret;

    if (formula_param == NULL) {
        debug_log(DLOG_ERROR, "%s: Input can not be null", __FUNCTION__);
        return RET_ERR;
    }

    ret =
        dal_get_extern_value_byte(obj_handle, PROPERTY_BBU_MODULE_PER_REMAIN_CAP, &formula_param->remain_cap, DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get value of %s failed, ret=%d", __FUNCTION__, PROPERTY_BBU_MODULE_PER_REMAIN_CAP,
            ret);
        return RET_ERR;
    }

    ret = dal_get_extern_value_byte(obj_handle, PROPERTY_BBU_MODULE_WORK_STATUS, &formula_param->work_status, DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get value of %s failed, ret=%d", __FUNCTION__, PROPERTY_BBU_MODULE_WORK_STATUS, ret);
        return RET_ERR;
    }

    ret = dal_get_extern_value_uint32(obj_handle, PROPERTY_BBU_MODULE_STATE_OF_HEALTH, &formula_param->soh, DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get value of %s failed, ret=%d", __FUNCTION__, PROPERTY_BBU_MODULE_STATE_OF_HEALTH,
            ret);
        return RET_ERR;
    }

    ret = dal_get_extern_value_uint32(obj_handle, PROPERTY_BBU_MODULE_RATE_CAP, &formula_param->rate_cap, DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get value of %s failed, ret=%d", __FUNCTION__, PROPERTY_BBU_MODULE_RATE_CAP, ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_double(obj_handle, PROPERTY_BBU_MODULE_VOLTAGE1, &formula_param->voltage);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get value of %s failed, ret=%d", __FUNCTION__, PROPERTY_BBU_MODULE_VOLTAGE1, ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_double(obj_handle, PROPERTY_BBU_MODULE_BATTERY_CURR_SAMPL, &formula_param->current);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get value of %s failed, ret=%d", __FUNCTION__,
            PROPERTY_BBU_MODULE_BATTERY_CURR_SAMPL, ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_double(obj_handle, PROPERTY_BBU_MODULE_BAT_ABILITY_COEF, &formula_param->ablity_coef);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get value of %s failed, ret=%d", __FUNCTION__, PROPERTY_BBU_MODULE_BAT_ABILITY_COEF,
            ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 calculate_bat_remain_time(BAT_REMAIN_TIME_FORMULA_PARAM_S *formula_param)
{
    if (formula_param == NULL) {
        debug_log(DLOG_ERROR, "%s: Input can not be null", __FUNCTION__);
        return RET_ERR;
    }

    
    if (formula_param->work_status == BBU_STATE_ON_CHARGE) {
        formula_param->remain_charge_time =
            (((100.0 - formula_param->remain_cap) / 100.0) * (formula_param->soh / 100.0) * // 100.0 转化为百分比
            (1.0 * formula_param->rate_cap / formula_param->voltage) * formula_param->ablity_coef) /
            fabs(formula_param->current);
    } else if (formula_param->work_status == BBU_STATE_ON_DISCHARGE) {
        formula_param->remain_discharge_time =
            ((formula_param->remain_cap / 100.0) * (formula_param->soh / 100.0) * // 100.0 转化为百分比
            (1.0 * formula_param->rate_cap / formula_param->voltage) * formula_param->ablity_coef) /
            fabs(formula_param->current);
    }

    debug_log(DLOG_MASS,
        "%s: remain_charge_time=%lf, remain_discharge_time=%lf, remain_cap=%d,soh=%d, rate_cap=%d, voltage=%f, "
        "ablity_coef=%f, current=%f",
        __FUNCTION__, formula_param->remain_charge_time, formula_param->remain_discharge_time,
        formula_param->remain_cap, formula_param->soh, formula_param->rate_cap, formula_param->voltage,
        formula_param->ablity_coef, formula_param->current);

    return RET_OK;
}

LOCAL void add_battery_remain_time_prop(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    gint32 ret;
    BAT_REMAIN_TIME_FORMULA_PARAM_S formula_param = {0};

    if (o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: Input can not be null", __FUNCTION__);
        return;
    }

    ret = get_bat_remain_time_prop_value(obj_handle, &formula_param);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get value of formula param of battery remain time failed", __FUNCTION__);
        goto exit;
    }

    ret = check_bat_remain_time_param_value(obj_handle, formula_param);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Check formula param of battery remain time failed", __FUNCTION__);
        goto exit;
    }

    ret = calculate_bat_remain_time(&formula_param);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Calculate battery remain time failed", __FUNCTION__);
        goto exit;
    }

    json_object_object_add(o_result_jso, RFPROP_BBU_BATTERY_REMAIN_DISCHARGE_TIME,
        ex_json_object_new_double(formula_param.remain_discharge_time, "%.3f"));
    json_object_object_add(o_result_jso, RFPROP_BBU_BATTERY_REMAIN_CHARGE_TIME,
        ex_json_object_new_double(formula_param.remain_charge_time, "%.3f"));

    return;

exit:
    json_object_object_add(o_result_jso, RFPROP_BBU_BATTERY_REMAIN_DISCHARGE_TIME, NULL);
    json_object_object_add(o_result_jso, RFPROP_BBU_BATTERY_REMAIN_CHARGE_TIME, NULL);

    return;
}

LOCAL void add_battery_property_jso(OBJ_HANDLE obj_handle, json_object *battery_obj)
{
    guint8 bbu_protocol = BAT_PROTOCOL_INVALID;

    add_basic_prop_for_battery_obj(obj_handle, battery_obj);

    add_cell_temperature_for_battery_obj(obj_handle, battery_obj);

    (void)add_battery_remain_time_prop(obj_handle, battery_obj);

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BBU_MODULE_PROTOCOL, &bbu_protocol);
    if (bbu_protocol == BAT_PROTOCOL_CANBUS) {
        get_chassis_bbu_battery_canbus(obj_handle, battery_obj);
    }

    return;
}

LOCAL gint32 get_chassis_bbu_battery(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *battery_array = NULL;
    json_object *battery_obj = NULL;
    gint32 ret;

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_expr(!i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    battery_array = json_object_new_array();
    return_val_if_expr(battery_array == NULL, HTTP_INTERNAL_SERVER_ERROR);

    
    battery_obj = json_object_new_object();
    return_val_do_info_if_expr(battery_obj == NULL, HTTP_INTERNAL_SERVER_ERROR, json_object_put(battery_array));

    add_battery_property_jso(i_paras->obj_handle, battery_obj);

    ret = json_object_array_add(battery_array, battery_obj);
    do_val_if_expr(ret != 0, json_object_put(battery_obj));

    *o_result_jso = battery_array;
    return HTTP_OK;
}

LOCAL gint32 redfish_chassis_bbu_uri_check(const gchar *i_member_id, OBJ_HANDLE *o_obj_handle)
{
    gint32 ret;
    gint32 match_flag = RF_ERROR;
    guchar bbu_id = 0;
    gchar bbu_id_string[MAX_STRBUF_LEN] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    
    if (i_member_id == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dfl_get_object_list(CLASS_BBU_MODULE_NAME, &obj_list);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_NOT_FOUND,
        debug_log(DLOG_DEBUG, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        bbu_id = 0;
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_BBU_MODULE_ID, &bbu_id);
        continue_if_expr(ret != VOS_OK);

        (void)snprintf_s(bbu_id_string, sizeof(bbu_id_string), sizeof(bbu_id_string) - 1, "%u", bbu_id);
        match_flag = g_strcmp0(bbu_id_string, i_member_id);
        break_do_info_if_expr((match_flag == RF_MATCH_OK), (*o_obj_handle = (OBJ_HANDLE)obj_node->data));
    }
    g_slist_free(obj_list);

    
    return_val_if_expr(match_flag != RF_MATCH_OK, HTTP_NOT_FOUND);

    return VOS_OK;
}

gint32 chassis_bbu_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    OBJ_HANDLE component_handle = 0;
    gboolean bool_ret;

    bool_ret = redfish_check_chassis_uri_valid(i_paras->uri, &component_handle);
    do_if_expr(!bool_ret, return HTTP_NOT_FOUND);

    ret = check_enclosure_component_type(component_handle, FALSE);
    return_val_if_expr(ret != VOS_OK, HTTP_NOT_FOUND);

    ret = redfish_chassis_bbu_uri_check(i_paras->member_id, &i_paras->obj_handle);
    do_if_expr(ret != VOS_OK, return ret);

    *prop_provider = g_chassis_bbu_provider;
    *count = sizeof(g_chassis_bbu_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
