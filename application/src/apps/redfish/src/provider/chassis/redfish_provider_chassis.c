

#include "redfish_provider.h"
#include "redfish_provider_with_cucc_customized.h"
#include "redfish_provider_chassis.h"

LOCAL gint32 get_chassis_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_chassis_indicateled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_chassis_indicateled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_health_led_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_health_led_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_uid_led_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_uid_led_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_chassis_oem_property(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_chassis_oem_property(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL void get_chassis_oem_id(json_object **oem_num, guint8 *support_chassid_id);
LOCAL void get_chassis_oem_inlet_tem(guchar component_type, json_object **oem_tem);



LOCAL gint32 get_chassis_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_chassis_thermal_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_power_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_links_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkadapters_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);


LOCAL gint32 get_chassis_pciedevices_collection_odata_id(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);
LOCAL gint32 get_chassis_drives_collection_odata_id(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);
LOCAL gint32 get_chassis_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_chassis_location(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_chassis_rack_location(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_depth_mm(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_chassis_height_mm(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_chassis_width_mm(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 act_chassis_set_dimensions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 set_chassis_location_property(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *obj_json_mode, gchar *property_name, gchar *method_name);
LOCAL gint32 set_chassis_location_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object *o_result_jso);
LOCAL gint32 set_unit_direction(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object *o_result_jso);
LOCAL gint32 set_unit_rfid_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    json_object *trans_key_json);
LOCAL gint32 set_chassis_assettag(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 act_chassis_control_led(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gboolean redfish_check_chassis_unit_uri_valid(gchar *uri_string, OBJ_HANDLE *unit_handle);
LOCAL gint32 act_chassis_set_unit_rfid_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_chassis_set_device_installed_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_chassis_export_template_file(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_chassis_import_template_file(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

#define SCR_TAG_TEMP_FILE_PATH_NAME "/opt/pme/conf/asset_locator/RFIDImportTempFile.tar.gz"
#define DEST_TAG_TEMP_FILE_PATH_NAME "/tmp/web/RFIDImportTempFile.tar.gz"
#define DEST_TAG_FILE_PATH_NAME "/tmp/RFIDImportTempFile.csv"
#define UTAG_CONTENT_MAX_LEN 1024

#define RFID_CHARACTER_REGEX "^[0-9a-zA-Z_ `~!@#$%^&*()-=+{}|;,'.:\"(.*)\"?/<>\\-\\.\\\\]{0,48}$"
#define RFID_NUMBER_REGEX "^[1-9]\\d*|0$"
#define RFID_CHECK_IN_DATA_REGEX                                                                     \
    "^((((19|20)\\d{2})/(0?[13-9]|1[012])/(0?[1-9]|[12]\\d|30))|(((19|20)\\d{2})/(0?[13578]|1[02])/" \
    "31)|(((19|20)\\d{2})/0?2/(0?[1-9]|1\\d|2[0-8]))|((((19|20)([13579][26]|[2468][048]|0[48]))|(2000))/0?2/29))$"
#define MAX_LINE_LENGTH 256

#define TEMP_FILE_MAX_SIZE (36 * 1024)
#define IMPORT_FILE_PATH_PARA "FilePath"
#define MAX_NUM_BIT_COUNT 8
#define CONTENT_MAX_LEN 1024
#define MAX_CARD_TYPE_LEN 16

#define TAG_IMPORT_FILE_ITEM_MAX_NUM 16      
#define TAG_IMPORT_FILE_ITEM_DATA_MAX_LEN 64 
#define UTAG_NUM_POS 0                       

LOCAL CONFILE_ITEM_FORMAT_S g_config_file_item[] = {
    { "*Upos",            "0", TAG_TEMP_FILE_REQUIRED_ITEM, RFID_NUMBER_REGEX},
    { "*Model",           "1", TAG_TEMP_FILE_REQUIRED_ITEM, RFID_CHARACTER_REGEX},
    { "*DeviceType",      "2", TAG_TEMP_FILE_REQUIRED_ITEM, RFID_CHARACTER_REGEX},
    { "*Manufacturer",    "3", TAG_TEMP_FILE_REQUIRED_ITEM, RFID_CHARACTER_REGEX},
    { "*SerialNumber",    "4", TAG_TEMP_FILE_REQUIRED_ITEM, RFID_CHARACTER_REGEX},
    { "PartNumber",       "5", TAG_TEMP_FILE_NOT_REQUIRED_ITEM, RFID_CHARACTER_REGEX},
    { "*UHeight",         "6", TAG_TEMP_FILE_REQUIRED_ITEM, RFID_NUMBER_REGEX},
    { "LifeCycleYear",    "7", TAG_TEMP_FILE_NOT_REQUIRED_ITEM, RFID_NUMBER_REGEX},
    { "CheckInTime",      "8", TAG_TEMP_FILE_NOT_REQUIRED_ITEM, RFID_CHECK_IN_DATA_REGEX},
    { "WeightKg",         "9", TAG_TEMP_FILE_NOT_REQUIRED_ITEM, RFID_NUMBER_REGEX},
    { "RatedPowerWatts", "10", TAG_TEMP_FILE_NOT_REQUIRED_ITEM, RFID_NUMBER_REGEX},
    { "AssetOwner",      "11", TAG_TEMP_FILE_NOT_REQUIRED_ITEM, RFID_CHARACTER_REGEX}
};

LOCAL PROPERTY_PROVIDER_S g_chassis_provider[] = {
    {RFPROP_ODATA_ID,            RF_VALUE_NULL,          RF_VALUE_NULL,                 USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_odata_id,    NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_TYPE,        BMC_PRODUCT_NAME_APP,   MAP_PROPERTY_NULL,            USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_type,        NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_INDICATELED, LED_CLASS_NAME,         STATE,                        USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        get_uid_led_status, set_uid_led_status, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_HEALTH_LED, LED_CLASS_NAME, STATE, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW,
        get_health_led_status, set_health_led_status, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_ID,          MAP_PROPERTY_NULL,       MAP_PROPERTY_NULL,             USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RF_CHASSIS_THERMAL,            RF_VALUE_NULL,          RF_VALUE_NULL,                 USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_thermal_odata_id,    NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RF_CHASSIS_POWER,            RF_VALUE_NULL,          RF_VALUE_NULL,                 USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_power_odata_id,    NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RF_CHASSIS_ADAPTERS,            RF_VALUE_NULL,          RF_VALUE_NULL,                 USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkadapters_odata_id,    NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_PCIEDEVICES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciedevices_collection_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_DRIVES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_drives_collection_odata_id, NULL, NULL, ETAG_FLAG_DISABLE},
    {RF_CHASSIS_LINKS,            RF_VALUE_NULL,          RF_VALUE_NULL,                 USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_links_odata_id,    NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_OEM,            RF_VALUE_NULL,          RF_VALUE_NULL,                 USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_chassis_oem_property,    set_chassis_oem_property,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_STATUS,            RF_VALUE_NULL,          RF_VALUE_NULL,                 USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_status,    NULL,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LOCATION,            RF_VALUE_NULL,          RF_VALUE_NULL,                 USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_chassis_location,    set_chassis_rack_location,                    NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ASSET_TAG,            RF_VALUE_NULL,          RF_VALUE_NULL,                 USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL,    set_chassis_assettag,                    NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_CHASSIS_DEPTH_MM, RF_VALUE_NULL, RF_VALUE_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_depth_mm,    NULL,   NULL, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_HEIGHT_MM, RF_VALUE_NULL, RF_VALUE_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_height_mm,   NULL,   NULL, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_WIDTH_MM, RF_VALUE_NULL, RF_VALUE_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_width_mm,    NULL,   NULL, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_OEM_ACTION_SET_DIMENSIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, act_chassis_set_dimensions, ETAG_FLAG_ENABLE},
    
    {RFPROP_CHASSIS_OEM_ACTION_SET_UNIT_RFID_INFO, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, act_chassis_set_unit_rfid_info, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_OEM_ACTION_SET_DEVICE_INSTALLED_STATUS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, act_chassis_set_device_installed_status, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_OEM_ACTION_EXPORT_TEMPLATE_FILE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, NULL, NULL, act_chassis_export_template_file, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_OEM_ACTION_IMPORT_CONFIG_FILE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, NULL, NULL, act_chassis_import_template_file, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_OEM_ACTION_CONTROL_LED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_chassis_control_led, ETAG_FLAG_ENABLE}
};

LOCAL SET_UNIT_RFID_INFO_S g_set_unit_rfid_info[] = {
    {PROPERTY_UNIT_INFO_DEV_MODEL,      json_type_string,   MANDATORY_PROPERTY,     0, 0,           TRANSFER_UNIT_INFO_DEV_MODEL},
    {PROPERTY_UNIT_INFO_DEV_TYPE,       json_type_string,   MANDATORY_PROPERTY,     0, 0,           TRANSFER_UNIT_INFO_DEV_TYPE},
    {PROPERTY_UNIT_INFO_MANUFACTURER,   json_type_string,   MANDATORY_PROPERTY,     0, 0,           TRANSFER_UNIT_INFO_MANUFACTURER},
    {PROPERTY_UNIT_INFO_SER_NUM,        json_type_string,   MANDATORY_PROPERTY,     0, 0,           TRANSFER_UNIT_INFO_SER_NUM},
    {PROPERTY_UNIT_INFO_PART_NUM,       json_type_string,   OPTIONAL_PROPERTY,      0, 0,           TRANSFER_UNIT_INFO_PART_NUM},
    {PROPERTY_UNIT_INFO_U_HEIGHT,       json_type_int,      OPTIONAL_PROPERTY,      1, G_MAXINT64,  TRANSFER_UNIT_INFO_HIGHT},
    {PROPERTY_UNIT_INFO_LIFE_CYCLE,     json_type_string,   OPTIONAL_PROPERTY,      1, G_MAXUINT8,  TRANSFER_UNIT_INFO_LIFE_CYCLE},
    {PROPERTY_UNIT_INFO_CHK_IN_TIME,    json_type_string,   OPTIONAL_PROPERTY,      0, 0,           TRANSFER_UNIT_INFO_CHK_IN_TIME},
    {PROPERTY_UNIT_INFO_WEIGHT,         json_type_string,   OPTIONAL_PROPERTY,      1, G_MAXINT32,  TRANSFER_UNIT_INFO_WEIGHT},
    {PROPERTY_UNIT_INFO_RATED_POWER,    json_type_string,   OPTIONAL_PROPERTY,      1, G_MAXINT32,  TRANSFER_UNIT_INFO_RATED_POWER},
    {PROPERTY_UNIT_INFO_ASSET_OWNER,    json_type_string,   OPTIONAL_PROPERTY,      0, 0,           TRANSFER_UNIT_INFO_ASSET_OWNER},
    {PROPERTY_UNIT_INFO_EXTEND_FIELD,   json_type_string,   OPTIONAL_PROPERTY,      0, 0,           TRANSFER_UNIT_INFO_EXTENDFIELD}
};


static gint32 set_item_to_utag(PROVIDER_PARAS_S *i_paras, gchar utag_item[][TAG_IMPORT_FILE_ITEM_DATA_MAX_LEN],
    guint8 item_count)
{
    char key_item[MAX_NUM_BIT_COUNT] = {0};
    json_object *trans_key_json = NULL;
    gint32 ret;
    guint8 i;
    guint16 str_len;
    GSList *input_list = NULL;

    trans_key_json = json_object_new_object();

    for (i = 1; i <= item_count - 1; i++) {
        (void)sprintf_s(key_item, MAX_NUM_BIT_COUNT, "%u", i);
        json_object_object_add(trans_key_json, (const gchar *)key_item,
            json_object_new_string((const gchar *)utag_item[i]));
    }

    str_len = (guint16)json_object_get_string_len(trans_key_json);
    input_list = g_slist_append(input_list, g_variant_new_uint16(str_len));
    input_list = g_slist_append(input_list, g_variant_new_string(dal_json_object_get_str(trans_key_json)));
    ret =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->obj_handle,
        CLASS_UNIT_INFO, METHOD_SET_UNIT_INFO, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    (void)json_object_put(trans_key_json);
    uip_free_gvariant_list(input_list);

    if ((ret == VOS_OK) || (ret == RFERR_SUCCESS))
        return VOS_OK;
    else
        return VOS_ERR;
}

static gint32 check_import_config_file_item(const gchar *utag_item, guint8 column)
{
    
    if (strlen(utag_item) == 0) {
        if (g_config_file_item[column].check_null_flag == TAG_TEMP_FILE_REQUIRED_ITEM) {
            debug_log(DLOG_ERROR, "the item of column %d can not be null", column);
            return VOS_ERR;
        } else {
            return VOS_OK;
        }
    }

    
    if (FALSE == g_regex_match_simple(g_config_file_item[column].regex, (const gchar *)utag_item, G_REGEX_OPTIMIZE,
        (GRegexMatchFlags)0)) {
        debug_log(DLOG_ERROR, "the column %d item %s regex fail", column, utag_item);
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 get_import_config_file_item(const gchar *line, gchar utag_item[][TAG_IMPORT_FILE_ITEM_DATA_MAX_LEN],
    guint8 max_row, guint8 max_col)
{
    const gchar *line_ptr = NULL;
    gchar *item_ptr = NULL;
    guint8 item_row = 0;
    guint8 item_col = 0;
    guint32 double_quote_count = 0;

    line_ptr = line;

    item_ptr = utag_item[item_row];

    while (*line_ptr != '\0') {
        
        if (*line_ptr == ',') {
            if (double_quote_count % 2 == 0) {
                debug_log(DLOG_MASS, "the utag_item[%d] is %s", item_row, utag_item[item_row]);
                double_quote_count = 0;
                item_col = 0;
                line_ptr++;
                item_row++;
                item_ptr = utag_item[item_row];
                continue;
            }
        }

        
        if (*line_ptr == '"') {
            double_quote_count++;
        }

        if (item_row >= max_row || item_col >= max_col) {
            debug_log(DLOG_ERROR,
                "The current item_row is %d, item_col is %d, max_row is %d, max_col is %d , out of boundary .",
                item_row, item_col, max_row, max_col);
            return VOS_ERR;
        }

        *item_ptr = *line_ptr;
        line_ptr++;
        item_ptr++;
        item_col++;
    }

    return VOS_OK;
}

LOCAL void transform_config_file_item_format(gchar *utag_item, guint8 max_col)
{
    errno_t safe_fun_ret;
    gchar item_temp[TAG_IMPORT_FILE_ITEM_DATA_MAX_LEN] = {0};
    gint8 utag_item_pos;
    gint8 item_temp_pos = 0;

    // max_col 大于 item_temp 空间大小时，存在越界风险
    if (max_col > TAG_IMPORT_FILE_ITEM_DATA_MAX_LEN) {
        return;
    }

    
    if (*utag_item != '"') {
        debug_log(DLOG_MASS, "The utag_item is %s no need conversion format", utag_item);
        return;
    }

    

    // utag_item_pos 从 1 开始 ，到 strlen(utag_item[column]) - 1，即去掉首尾'"'
    for (utag_item_pos = 1; utag_item_pos < strlen(utag_item) - 1; utag_item_pos++) {
        item_temp[item_temp_pos] = utag_item[utag_item_pos];

        if (utag_item[utag_item_pos] == '"' && utag_item[utag_item_pos + 1] == '"') {
            utag_item_pos++;
        }

        item_temp_pos++;
    }

    debug_log(DLOG_MASS, "Before conversion the item is %s len is %zu ,after item is %s len is %zu", utag_item,
        strlen(utag_item), item_temp, strlen(item_temp));

    // 清空当前 utag_item[column] ，将转换后的新字段 复制到 utag_item[column]
    (void)memset_s(utag_item, max_col, 0, max_col);

    safe_fun_ret = memcpy_s(utag_item, max_col, item_temp, strlen(item_temp));
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    return;
}


static gint32 set_utag_info(PROVIDER_PARAS_S *i_paras, gchar *line, guint8 item_count, gchar **utag_num)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    guint8 column = 0;
    gchar utag_item[TAG_IMPORT_FILE_ITEM_MAX_NUM][TAG_IMPORT_FILE_ITEM_DATA_MAX_LEN] = {{0}, {0}};
    guint32 unum_id = 0;
    guint8 detected = TAG_NOT_DETECTED;
    gchar *item_tmp = NULL;

    do_val_if_expr(NULL != (item_tmp = strrchr(line, '\r')), *item_tmp = '\0'; item_tmp = NULL);

    ret = get_import_config_file_item(line, utag_item, TAG_IMPORT_FILE_ITEM_MAX_NUM, TAG_IMPORT_FILE_ITEM_DATA_MAX_LEN);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "Get import config file item failed .");
        return VOS_ERR;
    }

    while (column < item_count) {
        transform_config_file_item_format(utag_item[column], TAG_IMPORT_FILE_ITEM_DATA_MAX_LEN);

        debug_log(DLOG_MASS, "the utag_item[%d] is %s len is %zu", column, utag_item[column],
            strlen(utag_item[column]));

        ret = check_import_config_file_item(utag_item[column], column);
        if (ret != VOS_OK) {
            if (column == UTAG_NUM_POS) {
                // 若U位号校验失败，则说明U位号为空或不符合格式，一律返回Unkown
                safe_fun_ret = memcpy_s(*utag_num, CONTENT_MAX_LEN + 1, "Unknown", strlen("Unknown"));
                return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
                    debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            } else {
                safe_fun_ret =
                    memcpy_s(*utag_num, CONTENT_MAX_LEN + 1, utag_item[UTAG_NUM_POS], strlen(utag_item[UTAG_NUM_POS]));
                return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
                    debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            }

            return VOS_ERR;
        }

        if (column == UTAG_NUM_POS) {
            safe_fun_ret =
                memcpy_s(*utag_num, CONTENT_MAX_LEN + 1, utag_item[UTAG_NUM_POS], strlen(utag_item[UTAG_NUM_POS]));
            return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            ret = vos_str2int(utag_item[UTAG_NUM_POS], 10, &unum_id, NUM_TPYE_UINT32);
            if (ret != VOS_OK || unum_id > INVALID_UINT8) {
                debug_log(DLOG_ERROR, "%s:device type str %s to int failed", __FUNCTION__, utag_item[column]);
                break;
            }

            ret = dal_get_specific_object_byte(CLASS_UNIT_INFO, PROPERTY_UNIT_INFO_U_NUM, (guchar)unum_id,
                &i_paras->obj_handle);
            return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
                debug_log(DLOG_ERROR, "%s get PROPERTY_UNIT_INFO_U_NUM %d fail, result is %d\n", __FUNCTION__, unum_id,
                ret));

            ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_UNIT_INFO_DETECTED, &detected);
            return_val_do_info_if_fail((ret == VOS_OK) && (detected != TAG_NOT_DETECTED), VOS_ERR,
                debug_log(DLOG_ERROR, "the Tag %d is not found ,ret is %d !\r\n", unum_id, ret));
        }

        column++;
    }

    if (column != item_count) {
        debug_log(DLOG_ERROR, "the expect item count is %d cur item column is %d", item_count, column);
        return VOS_ERR;
    }

    ret = set_item_to_utag(i_paras, utag_item, item_count);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "set utag pos %s rfid info failed! \r\n ", *utag_num);
        return VOS_ERR;
    }

    return VOS_OK;
}

static gint32 check_utag_temp_format(gchar *line, guint8 *item_count)
{
    gchar *item, *saveptr_writedata = NULL;
    guint8 item_id = 0;
    item = strtok_s(line, ",", &saveptr_writedata);
    while (item) {
        do_val_if_expr(NULL != strrchr(item, '\r'), (item[strlen(item) - 1] = '\0'));
        if (0 != g_strcmp0(item, g_config_file_item[item_id].utag_item)) {
            debug_log(DLOG_ERROR, "%s: expect item is %s len is %zu,real item is %s len is %zu\r\n", __FUNCTION__,
                g_config_file_item[item_id].utag_item, strlen(g_config_file_item[item_id].utag_item), item,
                strlen(item));
            return VOS_ERR;
        }
        item_id++;
        item = strtok_s(NULL, ",", &saveptr_writedata);
    }

    *item_count = item_id;
    return VOS_OK;
}

static void import_config_file_monitor_info_free_func(void *monitor_data)
{
    json_object *jso = (json_object *)monitor_data;
    (void)json_object_put(jso);
    return;
}

LOCAL TASK_MONITOR_INFO_S *import_config_file_monitor_info_new(PROVIDER_PARAS_S *i_paras)
{
    TASK_MONITOR_INFO_S *monitor_info;
    json_object *user_data_jso = NULL;
    monitor_info = task_monitor_info_new((GDestroyNotify)import_config_file_monitor_info_free_func);
    if (NULL == monitor_info) {
        debug_log(DLOG_ERROR, "task_monitor_info_new failed");
        return NULL;
    }

    user_data_jso = json_object_new_object();
    if (NULL == user_data_jso) {
        debug_log(DLOG_ERROR, "alloc a new json object failed");
        task_monitor_info_free(monitor_info);
        return NULL;
    }

    json_object_object_add(user_data_jso, RF_USERDATA_FROM_WEBUI_FLAG,
        json_object_new_int(i_paras->is_from_webui)); // is_from_webui
    json_object_object_add(user_data_jso, RF_IMPORT_USER_NAME, json_object_new_string(i_paras->user_name)); // 用户名
    json_object_object_add(user_data_jso, RF_IMPORT_USER_CLIENT, json_object_new_string(i_paras->client));  // 用户IP
    json_object_object_add(user_data_jso, RF_IMPORT_USER_ROLE,
        json_object_new_int(i_paras->user_role_privilege)); // user_role_privilegeid
    json_object_object_add(user_data_jso, RF_IMPORT_OBJ_HANDLE, json_object_new_int(i_paras->obj_handle));

    monitor_info->user_data = user_data_jso;

    return monitor_info;
}

LOCAL gint32 import_progress_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gchar *filebuf = NULL;
    guint8 item_count = 0;
    guint8 check_flag = 0;
    gint16 succe_set_count = 0;
    gint32 file_size;
    gint32 ret;
    gint32 total_count = 0;
    gchar *line = NULL;
    gchar *saveptr_line = NULL;
    PROVIDER_PARAS_S *i_paras = NULL;
    const gchar *client = NULL;
    const gchar *user_name = NULL;
    gint32 cur_user_privilege = 0;
    gint32 obj_handle = 0;
    gchar *cur_fail_set_tag = NULL;
    gchar all_fail_set_tag[MAX_LINE_LENGTH] = {0};

    gint32 is_from_webui = 0;
    const gchar *import_tmp_path = NULL;

    (void)get_element_str((json_object *)monitor_fn_data->user_data, RF_IMPORT_USER_CLIENT, &client);
    (void)get_element_int((json_object *)monitor_fn_data->user_data, RF_IMPORT_USER_ROLE, &cur_user_privilege);
    (void)get_element_str((json_object *)monitor_fn_data->user_data, RF_IMPORT_USER_NAME, &user_name);
    (void)get_element_int((json_object *)monitor_fn_data->user_data, RF_IMPORT_OBJ_HANDLE, &obj_handle);
    (void)get_element_int((json_object *)monitor_fn_data->user_data, RF_USERDATA_FROM_WEBUI_FLAG, &is_from_webui);

    (void)get_element_str(body_jso, IMPORT_FILE_PATH_PARA, &import_tmp_path);

    if (0 != g_strcmp0(import_tmp_path, DEST_TAG_FILE_PATH_NAME)) {
        // redfish模块降权限后，调用代理方法拷贝文件
        ret = proxy_copy_file((guchar)is_from_webui, user_name, client, import_tmp_path, DEST_TAG_FILE_PATH_NAME,
            REDFISH_USER_UID, REDFISH_USER_GID, COPY_FILE_DELETE_FLAG);
        if (VOS_OK != ret) {
            debug_log(DLOG_ERROR, "%s: copy request file to %s failed", __FUNCTION__, DEST_TAG_FILE_PATH_NAME);
            (void)proxy_delete_file((guchar)is_from_webui, user_name, client, import_tmp_path);
            (void)proxy_delete_file((guchar)is_from_webui, user_name, client, DEST_TAG_FILE_PATH_NAME);
            (void)create_message_info(MSGID_CONFIG_FILE_IMPORT_FAIL, NULL, message_obj);
            return HTTP_BAD_REQUEST;
        }
    }

    
    (void)dal_set_file_owner(DEST_TAG_FILE_PATH_NAME, REDFISH_USER_UID, APPS_USER_GID);
    (void)dal_set_file_mode(DEST_TAG_FILE_PATH_NAME, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

    FILE *fp = dal_fopen_check_realpath(DEST_TAG_FILE_PATH_NAME, "r", DEST_TAG_FILE_PATH_NAME);
    if (NULL == fp) {
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MSGID_FILE_NOT_EXIST, NULL, message_obj);
        return HTTP_BAD_REQUEST;
    }
    
    ret = get_file_lines(DEST_TAG_FILE_PATH_NAME, &total_count);
    goto_label_do_info_if_fail(VOS_OK == ret && total_count > 1, err_exit, ret = HTTP_BAD_REQUEST;
        debug_log(DLOG_ERROR, "%s: Get operate log fail", __FUNCTION__));

    ret = fseek(fp, 0, SEEK_END);
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit, ret = HTTP_BAD_REQUEST;
        debug_log(DLOG_ERROR, "%s: fseek file failed", __FUNCTION__));

    file_size = ftell(fp);
    if (file_size > TEMP_FILE_MAX_SIZE || file_size <= 0) {
        debug_log(DLOG_ERROR, "%s : file size error, the size is %d", __FUNCTION__,
            file_size);
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MSGID_CONFIG_FILE_IMPORT_FAIL, NULL, message_obj);
        (void)fclose(fp);
        return HTTP_BAD_REQUEST;
    }

    filebuf = (gchar *)g_malloc0(file_size + 1);
    goto_label_do_info_if_fail(filebuf != NULL, err_exit, ret = HTTP_BAD_REQUEST;
        debug_log(DLOG_ERROR, "%s : filebuf malloc failed", __FUNCTION__));

    ret = fseek(fp, 0, SEEK_SET);
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit, ret = HTTP_BAD_REQUEST;
        debug_log(DLOG_ERROR, "%s: fseek file failed", __FUNCTION__));

    ret = fread(filebuf, sizeof(gchar), file_size, fp);
    filebuf[file_size] = '\0';
    goto_label_do_info_if_fail(0 != ret, err_exit, ret = HTTP_BAD_REQUEST;
        debug_log(DLOG_ERROR, "%s: fread file failed ret is %d", __FUNCTION__, ret));

    i_paras = (PROVIDER_PARAS_S *)g_malloc0(sizeof(PROVIDER_PARAS_S));
    goto_label_do_info_if_fail(i_paras != NULL, err_exit, ret = HTTP_BAD_REQUEST;
        debug_log(DLOG_ERROR, "%s : i_paras malloc failed!", __FUNCTION__));

    (void)strncpy_s(i_paras->user_name, SESSION_USER_NAME_MAX_LEN + 1, user_name, SESSION_USER_NAME_MAX_LEN);
    (void)strncpy_s(i_paras->client, SESSION_IP_LEN, client, SESSION_IP_LEN - 1);
    i_paras->user_role_privilege = (guint8)cur_user_privilege;
    i_paras->obj_handle = (guint8)obj_handle;

    cur_fail_set_tag = (gchar *)g_malloc0(CONTENT_MAX_LEN + 1);

    line = strtok_s(filebuf, "\n", &saveptr_line);
    while (line) {
        if (check_flag == 0) {
            check_flag = 1;
            ret = check_utag_temp_format(line, &item_count);
            goto_label_do_info_if_fail(ret == VOS_OK, err_exit, ret = HTTP_BAD_REQUEST;
                debug_log(DLOG_ERROR, "%s : check utag temp format failed!", __FUNCTION__));
        } else {
            ret = set_utag_info(i_paras, line, item_count, &cur_fail_set_tag);
            if (ret == VOS_OK) {
                succe_set_count++;
                monitor_fn_data->task_progress = succe_set_count * 100 / (total_count - 1);
            } else {
                if (VOS_OK != g_strcmp0(all_fail_set_tag, "")) {
                    ret = strncat_s(all_fail_set_tag, MAX_LINE_LENGTH, ",", strlen(","));
                    do_if_expr(ret != EOK, debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d", __FUNCTION__, ret));
                }
                ret = strncat_s(all_fail_set_tag, MAX_LINE_LENGTH, cur_fail_set_tag, strlen(cur_fail_set_tag));
                do_if_expr(ret != EOK, debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d", __FUNCTION__, ret));
                (void)memset_s(cur_fail_set_tag, CONTENT_MAX_LEN, 0, CONTENT_MAX_LEN);
                monitor_fn_data->task_progress = succe_set_count * 100 / (total_count - 1);
            }
        }
        line = strtok_s(NULL, "\n", &saveptr_line);
    }

    if (VOS_OK != g_strcmp0(all_fail_set_tag, "")) {
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MSGID_IMPORT_UTAG_CONFIG_FILE_ERR, NULL, message_obj,
            (const gchar *)all_fail_set_tag);
    } else {
        monitor_fn_data->task_state = RF_TASK_COMPLETED;
        monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
        (void)create_message_info(MSGID_SUCCESS, NULL, message_obj);
    }

    g_free(filebuf);
    filebuf = NULL;
    g_free(i_paras);
    i_paras = NULL;
    g_free(cur_fail_set_tag);
    cur_fail_set_tag = NULL;
    (void)fclose(fp);
    fp = NULL;
    (void)vos_rm_filepath((const gchar *)DEST_TAG_FILE_PATH_NAME); // 解析完成后，删除临时文件

    return RF_OK;

err_exit:
    do_val_if_expr((filebuf != NULL), g_free(filebuf));
    do_val_if_expr((i_paras != NULL), g_free(i_paras));
    do_val_if_expr((cur_fail_set_tag != NULL), g_free(cur_fail_set_tag));
    (void)fclose(fp);
    fp = NULL;
    (void)vos_rm_filepath((const gchar *)DEST_TAG_FILE_PATH_NAME); // 解析完成后，删除临时文件
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
    
    return RF_OK;
}

LOCAL gint32 act_chassis_import_template_file(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *body_jso = NULL;
    TASK_MONITOR_INFO_S *import_monitor_data = NULL;
    const gchar *config_file_path = NULL;

    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: has no USERROLE_BASICSETTING privilege.\n", __FUNCTION__));

    body_jso = i_paras->val_jso;
    (void)get_element_str(body_jso, IMPORT_FILE_PATH_PARA, &config_file_path);

    ret = dal_check_real_path2(config_file_path, TMP_PATH_WITH_SLASH);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s : file real_path is illegal! return %d\r\n", __FUNCTION__, ret);
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, NULL, o_message_jso, config_file_path, IMPORT_FILE_PATH_PARA);
        return HTTP_BAD_REQUEST;
    }

    if (TRUE != vos_get_file_accessible(config_file_path)) {
        debug_log(DLOG_ERROR, "%s : file real_path is not exist! \r\n", __FUNCTION__);
        (void)create_message_info(MSGID_FILE_NOT_EXIST, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    ret = check_redfish_running_task_exist(import_progress_monitor, i_paras->val_jso, i_paras->uri, NULL);
    if (FALSE != ret) {
        debug_log(DLOG_ERROR, "%s failed:running task %s exist", __FUNCTION__, RF_IMPORT_CCONFIG_FILE_TASK_DESC);
        (void)create_message_info(MSGID_TASK_CONFIG_IMPORT_ERR, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    import_monitor_data = import_config_file_monitor_info_new(i_paras);

    goto_label_do_info_if_fail(NULL != import_monitor_data, err_exit,
        debug_log(DLOG_ERROR, "alloc a import_monitor_data monitor info failed"));

    import_monitor_data->task_progress = TASK_NO_PROGRESS; 
    import_monitor_data->rsc_privilege = USERROLE_BASICSETTING;

    ret = create_new_task(RF_IMPORT_CCONFIG_FILE_TASK_DESC, import_progress_monitor, import_monitor_data, body_jso,
        i_paras->uri, o_result_jso);

    goto_label_do_info_if_fail(RF_OK == ret, err_exit, task_monitor_info_free(import_monitor_data);
        debug_log(DLOG_ERROR, "%s : creat Task %s failed! \r\n", __FUNCTION__, RF_IMPORT_CCONFIG_FILE_TASK_DESC));

    return HTTP_ACCEPTED;

err_exit:

    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 act_chassis_export_template_file(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: has no USERROLE_BASICSETTING privilege.\n", __FUNCTION__));

    if (TRUE != vos_get_file_accessible(SCR_TAG_TEMP_FILE_PATH_NAME)) {
        debug_log(DLOG_ERROR, "%s : file real_path:%s is illegal! \r\n", __FUNCTION__, SCR_TAG_TEMP_FILE_PATH_NAME);
        (void)create_message_info(MSGID_FILE_NOT_EXIST, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    (void)vos_file_copy(DEST_TAG_TEMP_FILE_PATH_NAME, SCR_TAG_TEMP_FILE_PATH_NAME);

    (void)dal_set_file_owner(DEST_TAG_TEMP_FILE_PATH_NAME, REDFISH_USER_UID, APPS_USER_GID);
    (void)dal_set_file_mode(DEST_TAG_TEMP_FILE_PATH_NAME, (S_IRUSR | S_IWUSR));

    (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso);

    return HTTP_OK;
}


LOCAL gint32 act_chassis_control_led(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    json_object *obj_led_state = NULL;
    json_object *obj_led_time = NULL;
    gchar class_name[MAX_NAME_SIZE] = {0};
    gboolean check_ret;

    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    check_ret = is_node_enc_rsc(i_paras->obj_handle, NULL, NULL, NULL);
    return_val_do_info_if_expr(check_ret, HTTP_NOT_FOUND,
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, RFPROP_CHASSIS_OEM_ACTION_CONTROL_LED));
    

    ret = json_object_object_get_ex(i_paras->val_jso, RF_CHASSIS_OEM_ACTION_PARAM_INDICATOR_LED, &obj_led_state);
    return_val_do_info_if_expr(FALSE == ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, RFPROP_CHASSIS_OEM_ACTION_CONTROL_LED,
        RF_CHASSIS_OEM_ACTION_PARAM_INDICATOR_LED));

    
    if (0 == g_strcmp0(RF_LED_BLINKING, dal_json_object_get_str(obj_led_state))) {
        ret = json_object_object_get_ex(i_paras->val_jso, RF_CHASSIS_OEM_ACTION_PARAM_DURATION, &obj_led_time);
        return_val_do_info_if_expr(FALSE == ret, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso,
            RFPROP_CHASSIS_OEM_ACTION_CONTROL_LED, RF_CHASSIS_OEM_ACTION_PARAM_DURATION));

        return_val_do_info_if_expr(json_type_int != json_object_get_type(obj_led_time), HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_ACT_PARA_TYPE_ERR, NULL, o_message_jso,
            dal_json_object_to_json_string(obj_led_time), RF_CHASSIS_OEM_ACTION_PARAM_DURATION,
            RFPROP_CHASSIS_OEM_ACTION_CONTROL_LED));
    } else {
        ret = json_object_object_get_ex(i_paras->val_jso, RF_CHASSIS_OEM_ACTION_PARAM_DURATION, &obj_led_time);
        return_val_do_info_if_expr(TRUE == ret, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_PROP_NOT_REQUIRED, RFPROP_CHASSIS_OEM_ACTION_CONTROL_LED, o_message_jso,
            RFPROP_CHASSIS_OEM_ACTION_CONTROL_LED));
    }
    

    if (0 == g_strcmp0(RF_LED_LIT, dal_json_object_get_str(obj_led_state))) {
        input_list = g_slist_append(input_list, g_variant_new_byte(TURN_ON_IDENTIFY_LED));
        input_list = g_slist_append(input_list, g_variant_new_byte(RF_CHASSIS_LED_LOCK_STATE));
    } else if (0 == g_strcmp0(RF_LED_OFF, dal_json_object_get_str(obj_led_state))) {
        input_list = g_slist_append(input_list, g_variant_new_byte(TURN_OFF_IDENTIFY_LED));
        input_list = g_slist_append(input_list, g_variant_new_byte(RF_CHASSIS_LED_LOCK_STATE));
    } else if (0 == g_strcmp0(RF_LED_BLINKING, dal_json_object_get_str(obj_led_state))) {
        input_list = g_slist_append(input_list, g_variant_new_byte(TURN_ON_IDENTIFY_LED));
        input_list = g_slist_append(input_list, g_variant_new_byte(json_object_get_int(obj_led_time)));
    }

    
    (void)dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);
    if (0 != g_strcmp0(class_name, CLASS_UNIT_INFO)) {
        
        ret = dfl_get_object_handle(UID_LED, &obj_handle);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_IMPLEMENTED, uip_free_gvariant_list(input_list);
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_CHASSIS_INDICATELED, o_message_jso,
            RFPROP_CHASSIS_INDICATELED));

        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
            LED_CLASS_NAME, METHOD_UIDLED_STATE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    } else {
        
        input_list = g_slist_append(input_list, g_variant_new_byte(U_LED_YELLOW));
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
            i_paras->obj_handle, CLASS_UNIT_INFO, METHOD_SET_LED_STATUS, AUTH_ENABLE, i_paras->user_role_privilege,
            input_list, NULL);
    }

    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

            
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_NOT_FOUND;

            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return RFERR_INSUFFICIENT_PRIVILEGE;

            
        default:
            debug_log(DLOG_ERROR, "%s, %d: ret = %d.\n", __FUNCTION__, __LINE__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 set_unit_rfid_info_add_property(PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object *trans_key_json)
{
    gint32 ret;
    json_object* val_json = NULL;
    json_bool ret_bool;

    gsize info_array_size = G_N_ELEMENTS(g_set_unit_rfid_info);
    for (gsize i = 0; i < info_array_size; i++) {
        ret_bool = json_object_object_get_ex(i_paras->val_jso, g_set_unit_rfid_info[i].property_name, &val_json);
        if (!ret_bool || val_json == NULL) {
            if (g_set_unit_rfid_info[i].operate_type == MANDATORY_PROPERTY) {
                (void)json_object_put(trans_key_json);
                (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso,
                    RFPROP_CHASSIS_OEM_ACTION_SET_UNIT_RFID_INFO, g_set_unit_rfid_info[i].property_name);
                return HTTP_BAD_REQUEST;
            }
            continue;
        }

        if (g_set_unit_rfid_info[i].lower_limit != 0 || g_set_unit_rfid_info[i].upper_limit != 0) {
            ret = is_valid_integer_jso_or_null(val_json, g_set_unit_rfid_info[i].property_name,
                g_set_unit_rfid_info[i].lower_limit, g_set_unit_rfid_info[i].upper_limit, o_message_jso);
            if (ret != RET_OK) {
                (void)json_object_put(trans_key_json);
                return ret;
            }
        }

        if (g_set_unit_rfid_info[i].property_type == json_type_int) {
            json_object_object_add(trans_key_json, g_set_unit_rfid_info[i].transfer_key,
                json_object_new_int(json_object_get_int(val_json)));
        } else if (g_set_unit_rfid_info[i].property_type == json_type_string) {
            if (dal_check_string_is_valid_ascii(dal_json_object_get_str(val_json)) != RET_OK) {
                (void)json_object_put(trans_key_json);
                (void)create_message_info(MSGID_PROP_FORMAT_ERR, NULL, o_message_jso,
                    RFPROP_CHASSIS_OEM_ACTION_SET_UNIT_RFID_INFO, g_set_unit_rfid_info[i].property_name);
                return HTTP_BAD_REQUEST;
            }

            json_object_object_add(trans_key_json, g_set_unit_rfid_info[i].transfer_key,
                json_object_new_string(dal_json_object_get_str(val_json)));
        }
    }
    return RET_OK;
}


LOCAL gint32 act_chassis_set_unit_rfid_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *trans_key_json = NULL;

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    return_val_do_info_if_fail(NULL != o_message_jso, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_CHASSIS_INDICATELED, o_message_jso,
        RFPROP_CHASSIS_INDICATELED));

    
    trans_key_json = json_object_new_object();
    ret = set_unit_rfid_info_add_property(i_paras, o_message_jso, trans_key_json);
    if (ret != RET_OK) {
        return ret;
    }

    ret = set_unit_rfid_info(i_paras, o_message_jso, o_result_jso, trans_key_json);
    (void)json_object_put(trans_key_json);

    return ret;
}

LOCAL gint32 set_device_installed_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, guint8 installed_state)
{
    gint32 ret;
    GSList *input_list = NULL;

    input_list = g_slist_append(input_list, g_variant_new_byte(installed_state));

    ret =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->obj_handle,
        CLASS_UNIT_INFO, METHOD_SET_UNIT_INSTALLED_STATUS, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

            
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_NOT_FOUND;

            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return RFERR_INSUFFICIENT_PRIVILEGE;

            
        case SET_RFID_HEIGHT_ERR:
            (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, RFPROP_CHASSIS_OEM_ACTION_SET_DEVICE_INSTALLED_STATUS,
                o_message_jso, RFPROP_CHASSIS_OEM_ACTION_SET_DEVICE_INSTALLED_STATUS);
            debug_log(DLOG_ERROR, "%s: unit occupied. ret=%d", __FUNCTION__, ret);
            return HTTP_BAD_REQUEST;

            
        case SET_RFID_EEP_SPACE_LACK_ERR:
            (void)create_message_info(MSGID_RECORDS_EXC_MAXNUM, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s: storage space not enough, ret=%d", __FUNCTION__, ret);
            return HTTP_BAD_REQUEST;

        case SET_RFID_REMOVED_WHEN_ONLINE_ERR:
            (void)create_message_info(MSGID_OPERATION_FAILED, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s: can not remove device when device is online, ret=%d", __FUNCTION__, ret);
            return HTTP_BAD_REQUEST;

            
        default:
            debug_log(DLOG_ERROR, "%s : ret = %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_BAD_REQUEST;
}

LOCAL gint32 act_chassis_set_device_installed_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *val_json = NULL;
    json_bool ret_bool;
    guint8 installed_state = TAG_NOT_DETECTED;

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    return_val_do_info_if_fail(NULL != o_message_jso, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_CHASSIS_INDICATELED, o_message_jso,
        RFPROP_CHASSIS_INDICATELED));

    ret_bool = json_object_object_get_ex(i_paras->val_jso, RF_CHASSIS_OEM_ACTION_PARAM_INSTALLED_STATE, &val_json);
    return_val_do_info_if_expr(!ret_bool || (val_json == NULL), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso,
        RFPROP_CHASSIS_OEM_ACTION_SET_DEVICE_INSTALLED_STATUS, RF_CHASSIS_OEM_ACTION_PARAM_INSTALLED_STATE));

    if (g_strcmp0(RF_CHASSIS_DEVICE_INSTALLED_STATE_INSTALLED, dal_json_object_get_str(val_json)) == 0) {
        installed_state = TAG_DETECTED;
    } else if (g_strcmp0(RF_CHASSIS_DEVICE_INSTALLED_STATE_REMOVED, dal_json_object_get_str(val_json)) == 0) {
        installed_state = TAG_NOT_DETECTED;
    } else {
        (void)create_message_info(MSGID_ACT_PARA_TYPE_ERR, NULL, o_message_jso, dal_json_object_get_str(val_json),
            RF_CHASSIS_OEM_ACTION_PARAM_INSTALLED_STATE, RFPROP_CHASSIS_OEM_ACTION_SET_DEVICE_INSTALLED_STATUS);
        return HTTP_BAD_REQUEST;
    }

    return set_device_installed_status(i_paras, o_message_jso, installed_state);
}


LOCAL gint32 set_chassis_assettag(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guchar fru_id = 0;
    gint32 ret;
    OBJ_HANDLE component;
    gchar   class_name[MAX_NAME_SIZE] = {0};

    component = i_paras->obj_handle;
    ret = dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);
    return_val_do_info_if_fail((VOS_OK == ret), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    if (0 == g_strcmp0(class_name, CLASS_RACK_ASSET_MGMT)) {
        (void)dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE, COMPONENT_TYPE_MAINBOARD,
            &component);
    }
    ret = dal_get_property_value_byte(component, PROPERTY_COMPONENT_FRUID, &fru_id);
    return_val_do_info_if_fail((VOS_OK == ret && INVALID_FRUID != fru_id), HTTP_NOT_IMPLEMENTED,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_ASSET_TAG, o_message_jso, RFPROP_ASSET_TAG));

    debug_log(DLOG_INFO, "get chassis component fru id successfully, fru id is %d", fru_id);

    return set_fru_product_assettag(i_paras, o_message_jso, fru_id, o_result_jso);
}


LOCAL void get_chassis_oem_id(json_object **oem_id, guint8 *support_chassis_id)
{
    gint32 ret;
    guint8 chassis_type = 0;
    guint32 chassis_id = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar chassis_id_str[RF_CHASSIS_ID_MAX_LEN_VALUE] = {0};

    *support_chassis_id = 1;

    
    ret = dal_get_object_handle_nth(BMC_PRODUCT_NAME_APP, 0, &obj_handle);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s:dal_get_object_handle_nth fail.", __FUNCTION__));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SERVER_TYPE, &chassis_type);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s:dal_get_property_value_byte fail.", __FUNCTION__));

    
    if (chassis_type != RF_CHASSIS_TYPE_BLADE_VALUE) {
        *support_chassis_id = 0;
        return;
    }

    
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_CHASSIS_NUM, &chassis_id);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s:dal_get_property_value_uint32 fail.", __FUNCTION__));
    
    return_do_info_if_fail(0xFFFFFFFF != chassis_id,
        debug_log(DLOG_DEBUG, "%s:dal_get_property_value_uint32 fail, chassis_id is not valid.", __FUNCTION__));
    

    
    ret = snprintf_s(chassis_id_str, RF_CHASSIS_ID_MAX_LEN_VALUE, RF_CHASSIS_ID_MAX_LEN_VALUE - 1, "%u", chassis_id);
    return_do_info_if_fail(RF_ERROR != ret, debug_log(DLOG_ERROR, "%s:snprintf_s fail.", __FUNCTION__));

    *oem_id = json_object_new_string((const gchar *)chassis_id_str);
    do_val_if_fail(NULL != *oem_id, debug_log(DLOG_ERROR, "%s:json_object_new_string fail.", __FUNCTION__));

    return;
}

LOCAL void get_chassis_oem_location(json_object **oem_id)
{
    if (oem_id == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar prop_val[PROP_VAL_MAX_LENGTH] = {0};

    ret = dal_get_object_handle_nth(CLASS_NAME_CHASSIS, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get object handle of %s failed. ret = %d.", CLASS_NAME_CHASSIS, ret);
        return;
    }

    ret = dal_get_property_value_string(obj_handle, PROPERTY_CHASSIS_LOCATION, prop_val, PROP_VAL_MAX_LENGTH);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "cann't get property value of %s failed. ret = %d.", PROPERTY_CHASSIS_LOCATION, ret);
        return;
    }

    *oem_id = json_object_new_string((const gchar *)prop_val);
    if (*oem_id == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail.", __FUNCTION__);
    }

    return;
}


LOCAL void get_chassis_oem_inlet_tem(guchar component_type, json_object **oem_tem)
{
    gint32 ret;
    gdouble out_val = 0.0;
    OBJ_HANDLE obj_handle = 0;
    EVENT_WORK_STATE_S *sensor_reading_status = NULL;
    guchar read_status = 0;
    guchar board_type = 0;

    
    
    (void)dal_rf_get_board_type(&board_type);
    return_if_fail(((COMPONENT_TYPE_MAINBOARD == component_type) && (BOARD_MM != board_type)) ||
        (COMPONENT_TYPE_BACKPLANE_BOARD == component_type && BOARD_BLADE != board_type));
    

    ret = dal_get_specific_object_string(CLASS_THRESHOLD_SENSOR, PROPERTY_THR_SENSOR_INNER_NAME,
        SENSER_INLET_TEMP_INNER_NAME, &obj_handle);
    return_do_info_if_fail(ret == DFL_OK,
        debug_log(DLOG_DEBUG, "%s:get inlet temp object handle failed , ret: %d", __FUNCTION__, ret));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_THS_SENSOR_READING_STATUS, &read_status);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s:dal_get_property_value_byte fail.", __FUNCTION__));

    
    sensor_reading_status = (EVENT_WORK_STATE_S *)&read_status;

    if (!sensor_reading_status->disable_scanning || !sensor_reading_status->disable_scanning_local ||
        !sensor_reading_status->disable_all ||
        !sensor_reading_status->initial_update_progress 
        || !sensor_reading_status->disable_access_error) {
        debug_log(DLOG_DEBUG, "%s: read status is not support.", __FUNCTION__);
        return;
    }

    
    ret = dal_get_property_value_double(obj_handle, PROPERTY_THR_SENSOR_READING_CONVERT, &out_val);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s:dal_get_property_value_byte fail.", __FUNCTION__));

    *oem_tem = ex_json_object_new_double(out_val, "%.2f");
    do_val_if_fail(NULL != *oem_tem, debug_log(DLOG_ERROR, "%s:ex_json_object_new_double fail.", __FUNCTION__));

    return;
}


void get_chassis_oem_drive_summary(json_object **oem_tem)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guchar presence_val = 0;
    guchar hdd_health = 0;
    guchar pcie_hdd_health = 0;
    guchar health = 0;

    json_object *status_obj_jso = NULL;

    guchar component_type = 0;
    guchar component_presence = 0;
    guchar component_num = 0;
    guchar sd_health = 0;
    OBJ_HANDLE o_obj_handle = 0;

    gint32 count = 0;

    if (NULL == oem_tem) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    
    ret = dfl_get_object_list(CLASS_HDD_NAME, &obj_list);
    if (VOS_OK == ret && NULL != obj_list) {
        for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
            
            (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_PRESENCE, &presence_val);

            do_if_expr(RF_HDD_PRESENCE != presence_val, continue);

            (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_HEALTH, &hdd_health);
            health = hdd_health > health ? hdd_health : health;
            count++;
        }
    }

    do_if_expr(NULL != obj_list, g_slist_free(obj_list));

    obj_list = NULL;

    
    ret = dfl_get_object_list(CLASS_PCIEHDD_NAME, &obj_list);
    if (VOS_OK == ret && NULL != obj_list) {
        for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
            
            (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_PRESENCE, &presence_val);
            do_if_expr(RF_HDD_PRESENCE != presence_val, continue);

            (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_HEALTH, &pcie_hdd_health);
            health = pcie_hdd_health > health ? pcie_hdd_health : health;
            count++;
        }
    }

    do_if_expr(NULL != obj_list, g_slist_free(obj_list));

    obj_list = NULL;

    
    ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
    if (VOS_OK == ret && NULL != obj_list) {
        for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
            ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICE_TYPE,
                &component_type);
            do_if_expr(((VOS_OK != ret) || (COMPONENT_TYPE_SD_CARD != component_type)), continue);

            ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_PRESENCE,
                &component_presence);
            do_if_expr(((VOS_OK != ret) || (0 == component_presence)), continue);

            
            ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICENUM, &component_num);
            do_if_expr(((VOS_OK != ret) || (0 == component_num)), continue);

            ret = dal_get_specific_object_byte(CLASS_MASS_STORAGE_NAME, PROPETRY_ID, component_num, &o_obj_handle);
            do_if_expr((VOS_OK != ret), continue);

            ret = dal_get_property_value_byte(o_obj_handle, PROPETRY_HEALTH, &sd_health);
            do_if_expr(((VOS_OK != ret) || (0xff == sd_health)), continue);

            health = sd_health > health ? sd_health : health;
            count++;
        }
    }

    do_if_expr(NULL != obj_list, g_slist_free(obj_list));

    
    *oem_tem = json_object_new_object();
    json_object_object_add(*oem_tem, RFPROP_COMMON_COUNT, json_object_new_int(count));
    do_if_expr(count > 0, get_resource_status_property(NULL, &health, NULL, &status_obj_jso, TRUE));
    json_object_object_add(*oem_tem, RFPROP_SYSTEM_SUMMARY_STATUS, status_obj_jso);

    return;
}



LOCAL gint32 get_power_management_max_supported(guint8 *ps_num)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    ret = dal_get_object_handle_nth(OBJ_PRODUCT_COMPONENT, 0, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get OBJ_PRODUCT_COMPONENT handle fail", __FUNCTION__));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPOENT_PS_NUM, ps_num);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get PROPERTY_COMPOENT_PS_NUM fail", __FUNCTION__));

    return VOS_OK;
}


LOCAL void get_chassis_oem_networkadapters_summary(OBJ_HANDLE obj_handle, json_object **oem_tem)
{
    gint32 ret;
    guchar netcard_num = 0;
    guint8 net_health = 0;
    guint8 health = 0;
    guint8 virtual_flag = NOT_AVAILABLE;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *status_obj_jso = NULL;
    OBJ_HANDLE ref_com_handle = 0;

    if (oem_tem == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    
    ret = dfl_get_object_list(CLASS_NETCARD_NAME, &obj_list);
    
    return_do_info_if_expr(ret != VOS_OK || obj_list == NULL,
        debug_log(DLOG_DEBUG, "%s, %d: dfl_get_object_list fail.\n", __FUNCTION__, __LINE__));
    

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_NETCARD_VIRTUAL_FLAG, &virtual_flag);
        if (virtual_flag) {
            debug_log(DLOG_MASS, "%s, %d: %s is virtural card, ignore it.", __FUNCTION__, __LINE__,
                dfl_get_object_name((OBJ_HANDLE)obj_node->data));
            continue;
        }
        ret = dfl_get_referenced_object((OBJ_HANDLE)obj_node->data, PROPERTY_NETCARD_REF_COMPONENT, &ref_com_handle);
        continue_if_expr(ret != DFL_OK);

        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, BUSY_NETCARD_HEALTH, &health);

        net_health = health > net_health ? health : net_health;
        netcard_num++;
    }

    g_slist_free(obj_list);

    
    *oem_tem = json_object_new_object();

    get_resource_status_property(NULL, &net_health, NULL, &status_obj_jso, TRUE);
    json_object_object_add(*oem_tem, RFPROP_COMMON_COUNT, json_object_new_int(netcard_num));
    json_object_object_add(*oem_tem, RFPROP_SYSTEM_SUMMARY_STATUS, status_obj_jso);

    return;
}


void get_chassis_oem_powersupply_summary(json_object **oem_tem)
{
    gint32 ret;
    guchar power_num = 0;
    OBJ_HANDLE obj_handle = 0;
    guchar ps_health = 0;
    guchar health = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guchar component_type = 0;
    json_object *status_obj_jso = NULL;

    if (NULL == oem_tem) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    
    ret = get_power_management_max_supported(&power_num);
    return_if_expr(VOS_OK != ret || 0 == power_num);

    

    
    ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
    
    do_if_expr(0 != ret, debug_log(DLOG_ERROR, "%s, %d: dfl_get_object_list fail.\n", __FUNCTION__, __LINE__));
    
    do_if_expr(NULL == obj_list, debug_log(DLOG_MASS, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
        do_if_expr((COMPONENT_TYPE_PS == component_type), (obj_handle = (OBJ_HANDLE)obj_node->data));

        if (0 != obj_handle) {
            (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_HEALTH, &ps_health);
        }

        health = ps_health > health ? ps_health : health;
    }

    do_if_expr(NULL != obj_list, g_slist_free(obj_list));

    

    
    *oem_tem = json_object_new_object();

    get_resource_status_property(NULL, &health, NULL, &status_obj_jso, TRUE);
    json_object_object_add(*oem_tem, RFPROP_COMMON_COUNT, json_object_new_int(power_num));
    json_object_object_add(*oem_tem, RFPROP_SYSTEM_SUMMARY_STATUS, status_obj_jso);

    return;
}

LOCAL gint32 get_bat_management_max_supported_count(guint8 *bat_num)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    if (bat_num == NULL) {
        debug_log(DLOG_ERROR, "%s: Input pram can not be null ", __FUNCTION__);
        return VOS_ERR;
    }

    ret = dal_get_object_handle_nth(OBJ_PRODUCT_COMPONENT, 0, &obj_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get OBJ_PRODUCT_COMPONENT handle fail", __FUNCTION__);
        return VOS_ERR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_BBU_MODULE_NUM, bat_num);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get PROPERTY_COMPONENT_BBU_MODULE_NUM fail", __FUNCTION__);
        return VOS_ERR;
    }

    return VOS_OK;
}

LOCAL void get_chassis_oem_battery_summary(json_object **oem_tem)
{
    gint32 ret;
    guchar battery_num = 0;
    OBJ_HANDLE obj_handle = 0;
    guchar bat_health = 0;
    guchar health = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guchar component_type = 0;
    json_object *status_obj_jso = NULL;

    if (oem_tem == NULL) {
        debug_log(DLOG_ERROR, "%s: Input pram can not be null ", __FUNCTION__);
        return;
    }

    
    ret = get_bat_management_max_supported_count(&battery_num);
    if ((ret != VOS_OK) || (battery_num == 0)) {
        return;
    }

    ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
    if ((ret != VOS_OK) || (obj_list == NULL)) {
        debug_log(DLOG_ERROR, "%s: Get component list fail", __FUNCTION__);
        goto exit;
    }

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        obj_handle = (OBJ_HANDLE)obj_node->data;

        (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
        if (component_type != COMPONENT_TYPE_BATTERY) {
            continue;
        }

        (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_HEALTH, &bat_health);

        health = bat_health > health ? bat_health : health;
    }

    if (obj_list != NULL) {
        g_slist_free(obj_list);
    }

exit:
    *oem_tem = json_object_new_object();

    get_resource_status_property(NULL, &health, NULL, &status_obj_jso, TRUE);
    json_object_object_add(*oem_tem, RFPROP_COMMON_COUNT, json_object_new_int(battery_num));
    json_object_object_add(*oem_tem, RFPROP_SYSTEM_SUMMARY_STATUS, status_obj_jso);

    return;
}


LOCAL void get_chassis_oem_package_code(json_object **package_code)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar chassis_partnumber[FRU_DATA_STRING_LEN] = {0};

    return_do_info_if_fail(package_code != NULL, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    ret = dal_get_object_handle_nth(CLASS_ELABEL, 0, &obj_handle);
    
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "%s:dal_get_object_handle_nth fail.", __FUNCTION__));
    

    
    ret =
        dal_get_property_value_string(obj_handle, PROPERTY_ELABEL_CHASSIS_PN, chassis_partnumber, FRU_DATA_STRING_LEN);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s:dal_get_property_value_string fail.", __FUNCTION__));
    
    ret = check_string_val_effective((const gchar *)chassis_partnumber);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "%s:check_string_val_effective fail.", __FUNCTION__));

    *package_code = json_object_new_string((const gchar *)chassis_partnumber);
    do_val_if_fail(NULL != *package_code, debug_log(DLOG_ERROR, "%s:json_object_new_string fail.", __FUNCTION__));
}

LOCAL void get_chassis_oem_switches_odata_id(OBJ_HANDLE obj_handle, json_object** board_odata_id)
{
    gchar               slot_id[MAX_RSC_NAME_LEN] = {0};
    gchar               chassis_switches_uri[MAX_URI_LENGTH] = {0};
    json_object*        chassis_switches_jso = NULL;
    json_object*        odata_id_jso = NULL;
    gint32 ret, i;
    guint32 switch_cnt = 0;
    guint32 tmp_cnt;
    const gchar *plane_class_name[MAX_LSW_PLANE_NUM] = { CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, CLASS_LSW_NODE_MANAGE_DP };

    for (i = 0; i < MAX_LSW_PLANE_NUM; i++) {
        tmp_cnt = 0;
        ret = dfl_get_object_count(plane_class_name[i], &tmp_cnt);
        if ((ret != DFL_OK) && (ret != ERRCODE_OBJECT_NOT_EXIST)) {
            debug_log(DLOG_ERROR, "[%s] dfl_get_object_count failed, i=%d ret=%d", __FUNCTION__, i, ret);
        }
        switch_cnt += tmp_cnt;
    }
    if (switch_cnt == 0) {
        return;
    }

    ret = rf_gen_chassis_component_id(NULL, obj_handle, slot_id, MAX_RSC_NAME_LEN);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s:generate chassis component id failed, ret is %d", __FUNCTION__, ret);
        return;
    }

    ret = snprintf_s(chassis_switches_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT_CHASSIS_SWITCHES, slot_id);
    if (ret <= EOK) {
        debug_log(DLOG_ERROR, "%s:format chassis board uri failed, ret is %d", __FUNCTION__, ret);
        return;
    }

    chassis_switches_jso = json_object_new_string(chassis_switches_uri);
    if (chassis_switches_jso == NULL) {
        debug_log(DLOG_ERROR, "alloc new json string failed");
        return;
    }

    odata_id_jso = json_object_new_object();
    if (odata_id_jso == NULL) {
        debug_log(DLOG_ERROR, "alloc new json object failed");
        json_object_put(chassis_switches_jso);
        return;
    }

    json_object_object_add(odata_id_jso, RFPROP_ODATA_ID, chassis_switches_jso);
    *board_odata_id = odata_id_jso;
    return;
}


LOCAL void get_chassis_oem_odata_id(OBJ_HANDLE obj_handle, json_object **propertylist_odata_id, const gchar *string_uri)
{
    gint32 ret;
    gchar  slot_id[MAX_RSC_NAME_LEN] = {0};
    gchar  chassis_propertylist_uri[MAX_URI_LENGTH] = {0};
    json_object *chassis_propertylist_jso = NULL;
    json_object *odata_id_jso = NULL;

    
    
    if (check_enclosure_component_type(obj_handle, TRUE) != RET_OK) {
        return;
    }

    ret = rf_gen_chassis_component_id(NULL, obj_handle, slot_id, MAX_RSC_NAME_LEN);
    return_do_info_if_fail(ret == VOS_OK,
        debug_log(DLOG_ERROR, "%s:generate chassis component id failed, ret is %d", __FUNCTION__, ret));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(chassis_propertylist_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, string_uri, slot_id);
#pragma GCC diagnostic pop
    return_do_info_if_fail(ret > 0,
        debug_log(DLOG_ERROR, "%s:format chassis board uri failed, ret is %d", __FUNCTION__, ret));

    chassis_propertylist_jso = json_object_new_string((const gchar *)chassis_propertylist_uri);
    return_do_info_if_fail(chassis_propertylist_jso != NULL, debug_log(DLOG_ERROR, "alloc new json string failed"));

    odata_id_jso = json_object_new_object();
    return_do_info_if_fail(odata_id_jso != NULL, debug_log(DLOG_ERROR, "alloc new json object failed");
        json_object_put(chassis_propertylist_jso));

    json_object_object_add(odata_id_jso, RFPROP_ODATA_ID, chassis_propertylist_jso);

    *propertylist_odata_id = odata_id_jso;

    return;
}

LOCAL void get_chassis_oem_board_odata_id(OBJ_HANDLE obj_handle, json_object **board_odata_id)
{
    get_chassis_oem_odata_id(obj_handle, board_odata_id, URI_FORMAT_CHASSIS_BOARDS);
}



LOCAL void get_chassis_device_max_num(OBJ_HANDLE handle, json_object **max_num_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar* property_compoent_list[][2] = {
        {RFPROP_MEM_NUM, PROPERTY_COMPOENT_MEMORY_NUM}, 
        {RFPROP_PCIE_NUM, PROPERTY_COMPOENT_PCIE_CARD_NUM}, 
        {RFPROP_CPU_NUM, PROPERTY_COMPOENT_CPU_NUM},  
        {RFPROP_DISK_NUM, PROPERTY_COMPOENT_DISK_NUM}, 
        {RFPROP_POWERSUPPLY_NUM, PROPERTY_COMPOENT_PS_NUM}, 
        {RFPROP_FAN_NUM, PROPERTY_COMPOENT_FAN_NUM}, 
        {RFPROP_MEZZCARD_NUM, PROPERTY_COMPOENT_MEZZ_CARD_NUM}, 
        {RFPROP_SD_CARD_NUM, PROPERTY_COMPOENT_SD_CARD_NUM}, 
        {RFPROP_SD_CONTROLLER_NUM, PROPERTY_COMPOENT_SDCARD_CONTROLLER_NUM}, 
        {RFPROP_SECURITY_MODULE_NUM, PROPERTY_COMPONENT_SECURITY_MODULE_NUM}, 
        {RFPROP_BBU_MODULE_NUM, PROPERTY_COMPONENT_BBU_MODULE_NUM}, 
        {PROPERTY_COMPONENT_OCP_CARD_NUM, PROPERTY_COMPONENT_OCP_CARD_NUM} 
        };

    
    return_do_info_if_expr(max_num_jso == NULL, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    ret = dal_get_object_handle_nth(CLASS_PRODUCT_COMPONENT, 0, &obj_handle);
    return_do_info_if_fail(ret == VOS_OK, debug_log(DLOG_ERROR, "%s, get o_obj_handle fail.\n", __FUNCTION__));

    *max_num_jso = json_object_new_object();
    if (*max_num_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: alloc new json object failed", __FUNCTION__);
        return;
    }

    for (guint32 cnt = 0; cnt < G_N_ELEMENTS(property_compoent_list); cnt++) {
        guint8 value;
        ret = dal_get_property_value_byte(obj_handle, property_compoent_list[cnt][1], &value);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "get propertys [%s] fail, ret = %d", property_compoent_list[cnt][1], ret);
            continue;
        }
        json_object_object_add(*max_num_jso, property_compoent_list[cnt][0], json_object_new_int(value));
    }
}


LOCAL void get_chassis_oem_board_manufacturer(OBJ_HANDLE handle, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar prop_val[PROP_VAL_MAX_LENGTH] = {0};

    ret = dal_get_object_handle_nth(CLASS_ELABEL, 0, &obj_handle);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s:dal_get_object_handle_nth fail.", __FUNCTION__));

    ret = dal_get_property_value_string(obj_handle, PROPERTY_ELABEL_BOARD_MFG_NAME, prop_val, PROP_VAL_MAX_LENGTH);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s:dal_get_property_value_string fail.", __FUNCTION__));

    *o_result_jso = json_object_new_string((const gchar *)prop_val);
    do_val_if_fail(NULL != *o_result_jso, debug_log(DLOG_ERROR, "%s:json_object_new_string fail.", __FUNCTION__));

    return;
}



LOCAL void get_chassis_oem_energy_efficiency(OBJ_HANDLE handle, json_object **oem_eneffic)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    guint32 es_percent;
    gdouble energy_save;
    gdouble carfp_reduction;

    
    return_do_info_if_expr(NULL == oem_eneffic, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    ret = dal_get_object_handle_nth(CLASS_NAME_AMMETER, 0, &obj_handle);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s, %d: get obj_handle fail.\n", __FUNCTION__, __LINE__));

    property_name_list = g_slist_append(property_name_list, PSM_ESPERCENT_PROPERTY_NAME);
    property_name_list = g_slist_append(property_name_list, PSM_ENERGYSAVE_PROPERTY_NAME);
    property_name_list = g_slist_append(property_name_list, PSM_CARBONSAVE_PROPERTY_NAME);

    
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value_list);
    g_slist_free(property_name_list);
    
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_DEBUG, "%s, %d: get propertys failed.\n", __FUNCTION__, __LINE__));
    
    es_percent = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value_list, 0));
    energy_save = g_variant_get_double((GVariant *)g_slist_nth_data(property_value_list, 1));
    energy_save = ((gdouble)((gint32)(energy_save * 1000 + 0.5))) / 1000;
    carfp_reduction = g_variant_get_double((GVariant *)g_slist_nth_data(property_value_list, 2));
    carfp_reduction = ((gdouble)((gint32)(carfp_reduction * 1000 + 0.5))) / 1000;

    g_slist_free_full(property_value_list, (GDestroyNotify)g_variant_unref);
    
    *oem_eneffic = json_object_new_object();
    return_do_info_if_fail(NULL != *oem_eneffic,
        debug_log(DLOG_ERROR, "%s: alloc new json object failed.\n", __FUNCTION__));
    json_object_object_add(*oem_eneffic, RFPROP_CHASSIS_ENERGYSAVINGS_PERCENT, json_object_new_int(es_percent));
    
    json_object_object_add(*oem_eneffic, RFPROP_CHASSIS_POWER_SAVINGS, ex_json_object_new_double(energy_save, "%.1f"));
    json_object_object_add(*oem_eneffic, RFPROP_CHASSIS_CARBONFOOTPRINT_REDUCTION,
        ex_json_object_new_double(carfp_reduction, "%.1f"));
    

    return;
}


LOCAL void get_chassis_oem_mainboard(OBJ_HANDLE handle, json_object **oem_mianboard)
{
    gint32 ret;
    OBJ_HANDLE elabel_handle = 0;
    OBJ_HANDLE fru_handle = 0;

    
    ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, 0, &fru_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dal_get_specific_object_byte(%s) failed: ret %d.", CLASS_FRU, ret);
        return;
    }

    
    ret = dfl_get_referenced_object(fru_handle, PROPERTY_FRU_ELABEL_RO, &elabel_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_referenced_object(%s) handle failed: ret %d.", PROPERTY_FRU_ELABEL_RO, ret);
        return;
    }

    
    *oem_mianboard = json_object_new_object();
    if (*oem_mianboard == NULL) {
        debug_log(DLOG_ERROR, "%s: alloc new json object failed.\n", __FUNCTION__);
        return;
    }

    
    rf_add_property_jso_string(elabel_handle, PROPERTY_ELABEL_BOARD_PRO_NAME, RFPROP_BOARD_BOARDNAME, *oem_mianboard);
    rf_add_property_jso_string(elabel_handle, PROPERTY_ELABEL_BOARD_SN, RFPROP_SERIAL_NUMBER, *oem_mianboard);

    
    if (dal_is_customized_by_cmcc()) {
        rf_add_property_jso_string(elabel_handle, PROPERTY_ELABEL_PRODUCT_MFG_NAME, RFPROP_MANUFACTURE, *oem_mianboard);
    }
}


LOCAL void get_chassis_rack_oem_actions(json_object *obj_actions, const gchar *slot_str, const char *rfprop_action,
    const char *rf_action, const char *rf_actioninfo)
{
    gint32 ret;
    gchar string_value[POWER_STR_MAX_LEN * 2] = {0};

    json_object *obj_uid_ac = NULL;
    json_object *obj_target = NULL;
    json_object *obj_action_info = NULL;

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "#%s", rfprop_action);
    return_if_expr(ret < 0);

    obj_uid_ac = json_object_new_object();
    json_object_object_add(obj_actions, (const gchar *)string_value, obj_uid_ac);
    return_if_expr(NULL == obj_uid_ac);

    
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, rf_action, slot_str);
#pragma GCC diagnostic pop
    return_if_expr(ret < 0);

    obj_target = json_object_new_string((const gchar *)string_value);
    json_object_object_add(obj_uid_ac, RFPROP_TARGET, obj_target);
    return_if_expr(NULL == obj_target);

    
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, rf_actioninfo, slot_str);
#pragma GCC diagnostic pop
    return_if_expr(ret < 0);

    obj_action_info = json_object_new_string((const gchar *)string_value);
    json_object_object_add(obj_uid_ac, RFPROP_ACTION_INFO, obj_action_info);

    return;
}


LOCAL void get_chassis_oem_actions(PROVIDER_PARAS_S *i_paras, json_object *obj_huawei)
{
    gint32 ret;
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gchar class_name[MAX_NAME_SIZE] = {0};
    guint8 unum = 0;
    OBJ_HANDLE component_handle = OBJ_HANDLE_COMMON;
    guint8 detected = TAG_NOT_DETECTED;
    OBJ_HANDLE obj_rack_asset_mgmt = OBJ_HANDLE_COMMON;
    guint8 rack_asset_mgmt_type = RACK_ASSET_MGMT_TYPE_MODBUS;
    json_object *obj_actions = NULL;

    return_if_expr(NULL == obj_huawei);

    obj_actions = json_object_new_object();
    json_object_object_add(obj_huawei, RFPROP_ACTIONS, obj_actions);
    return_if_expr(NULL == obj_actions);

    ret = dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);
    if (0 == g_strcmp0(class_name, CLASS_RACK_ASSET_MGMT)) {
        (void)dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE, MAINBOARD_DEVICETYPE,
            &component_handle);

        ret = rf_gen_chassis_component_id(NULL, component_handle, slot_str, sizeof(slot_str));
        return_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

        
        get_chassis_rack_oem_actions(obj_actions, slot_str, RFPROP_CHASSIS_OEM_ACTION_SET_DIMENSIONS,
            RF_CHASSIS_OEM_ACTION_SET_DIMENSIONS, RF_CHASSIS_OEM_ACTION_SET_DIMENSIONS_ACTIONINFO);

        
        get_chassis_rack_oem_actions(obj_actions, slot_str, RFPROP_CHASSIS_OEM_ACTION_EXPORT_TEMPLATE_FILE,
            RF_CHASSIS_OEM_ACTION_EXPORT_TEMPLATE_FILE, RF_CHASSIS_OEM_ACTION_EXPORT_TEMPLATE_FILE_ACTIONINFO);

        get_chassis_rack_oem_actions(obj_actions, slot_str, RFPROP_CHASSIS_OEM_ACTION_IMPORT_CONFIG_FILE,
            RF_CHASSIS_OEM_ACTION_IMPORT_CONFIG_FILE, RF_CHASSIS_OEM_ACTION_IMPORT_CONFIG_FILE_ACTIONINFO);

        return;
    } else if (0 == g_strcmp0(class_name, CLASS_UNIT_INFO)) {
        ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_UNIT_INFO_U_NUM, &unum);
        return_if_expr(VOS_OK != ret);

        ret = snprintf_s(slot_str, sizeof(slot_str), sizeof(slot_str) - 1, "U%u", unum);
        return_if_expr(ret < 0);

        ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_UNIT_INFO_DETECTED, &detected);
        return_if_expr(VOS_OK != ret);

        if (detected != TAG_NOT_DETECTED) {
            
            get_chassis_rack_oem_actions(obj_actions, slot_str, RFPROP_CHASSIS_OEM_ACTION_SET_UNIT_RFID_INFO,
                RF_CHASSIS_OEM_ACTION_SET_UNIT_RFID_INFO, RF_CHASSIS_OEM_ACTION_SET_UNIT_RFID_INFO_ACTIONINFO);
        }

        (void)dal_get_object_handle_nth(CLASS_RACK_ASSET_MGMT, 0, &obj_rack_asset_mgmt);
        (void)dal_get_property_value_byte(obj_rack_asset_mgmt, PROPERTY_RACK_ASSET_MGMT_TYPE, &rack_asset_mgmt_type);
        if (rack_asset_mgmt_type == RACK_ASSET_MGMT_TYPE_NETWORK) {
            
            get_chassis_rack_oem_actions(obj_actions, slot_str, RFPROP_CHASSIS_OEM_ACTION_SET_DEVICE_INSTALLED_STATUS,
                RF_CHASSIS_OEM_ACTION_SET_DEVICE_INSTALLED_STATUS,
                RF_CHASSIS_OEM_ACTION_SET_DEVICE_INSTALLED_STATUS_ACTIONINFO);
        }
        
    } else {
        ret = rf_gen_chassis_component_id(i_paras->member_id, 0, slot_str, sizeof(slot_str));
        return_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s, %d: rf_gen_chassis_component_id failed.", __FUNCTION__, __LINE__));
    }

    
    get_chassis_rack_oem_actions(obj_actions, slot_str, RFPROP_CHASSIS_OEM_ACTION_CONTROL_LED,
        RF_CHASSIS_OEM_ACTION_CONTROL_LED, RF_CHASSIS_OEM_ACTION_CONTROL_LED_ACTIONINFO);

    return;
}


LOCAL void get_chassis_weight(OBJ_HANDLE obj_handle, json_object *rsc_jso)
{
    gint32 ret;
    guint32 weight = 0;
    json_object *property_jso = NULL; // 属性 值的 json格式

    return_do_info_if_expr(NULL == rsc_jso, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    if (0 == obj_handle) {
        json_object_object_add(rsc_jso, RFPROP_UNITDEVICES_WEIGHT, NULL);
        return;
    }

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_UNIT_INFO_WEIGHT, &weight);
    return_do_info_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s: get property value fail", __FUNCTION__));

    return_do_info_if_expr(0 == weight, debug_log(DLOG_DEBUG, "%s: device weight is not valid", __FUNCTION__));

    
    property_jso = json_object_new_int64((gint64)weight);
    do_if_expr(NULL == property_jso, debug_log(DLOG_ERROR, "%s:json_object_new_int fail", __FUNCTION__));

    json_object_object_add(rsc_jso, RFPROP_UNITDEVICES_WEIGHT, property_jso);

    return;
}


LOCAL void get_chassis_oem_topuid(PROVIDER_PARAS_S *i_paras, json_object **oem_uid)
{
    guint8 uid = 0;
    guint32 ret;

    
    if (NULL == oem_uid || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_UNIT_INFO_U_NUM, &uid);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s: get unitdevice height failed", __FUNCTION__));

    return_do_info_if_expr(0 == uid, debug_log(DLOG_DEBUG, "%s: device uid is not valid", __FUNCTION__));

    *oem_uid = json_object_new_int((gint32)uid);
    do_if_expr(*oem_uid == NULL,
        debug_log(DLOG_ERROR, "%s: call json_object_new_int failed, unit height is %u", __FUNCTION__, uid));
    return;
}


LOCAL void get_chassis_oem_height(PROVIDER_PARAS_S *i_paras, json_object **oem_height)
{
    guint8 height = 0;
    guint32 ret;

    
    if (NULL == oem_height || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_UNIT_INFO_U_HEIGHT, &height);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s: get unitdevice height failed", __FUNCTION__));

    if (0 == height) {
        debug_log(DLOG_MASS, "%s: unitdevice height is invalid", __FUNCTION__);
        return;
    }

    *oem_height = json_object_new_int((gint32)height);
    do_if_expr(*oem_height == NULL,
        debug_log(DLOG_ERROR, "%s: call json_object_new_int failed, unit height is %u", __FUNCTION__, height));
    return;
}


LOCAL void get_chassis_oem_available_rack_space(PROVIDER_PARAS_S *i_paras, json_object **oem_available_rack_space)
{
    gint32 ret;
    guint8 available_rack_space = 0;

    
    if (NULL == oem_available_rack_space || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return;
    }

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_UNIT_INFO_AVAILABLE_RACK_SPACE,
        &available_rack_space);
    return_do_info_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s: get unit device available rack space failed", __FUNCTION__));

    if (0 == available_rack_space) {
        debug_log(DLOG_MASS, "%s: unit device available rack space is invalid", __FUNCTION__);
        return;
    }

    *oem_available_rack_space = json_object_new_int((gint32)available_rack_space);
    do_if_expr(NULL == *oem_available_rack_space, debug_log(DLOG_ERROR,
        "%s failed, unit device available rack space in U is %d", __FUNCTION__, available_rack_space));
    return;
}


LOCAL void get_chassis_oem_ratepower(PROVIDER_PARAS_S *i_paras, json_object **oem_ratepower)
{
    guint32 ratepower = 0;
    guint32 ret;

    
    if (NULL == oem_ratepower || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    ret = dal_get_property_value_uint32(i_paras->obj_handle, PROPERTY_UNIT_INFO_RATED_POWER, &ratepower);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s: get unitdevice ratepower failed", __FUNCTION__));

    return_do_info_if_expr(0 == ratepower, debug_log(DLOG_DEBUG, "%s: device ratepower is not valid", __FUNCTION__));

    *oem_ratepower = json_object_new_int64((gint64)ratepower);
    do_if_expr(*oem_ratepower == NULL,
        debug_log(DLOG_ERROR, "%s: call json_object_new_int64 failed, unit ratepower is %u", __FUNCTION__, ratepower));
    return;
}


LOCAL void get_chassis_oem_checkintime(PROVIDER_PARAS_S *i_paras, json_object **oem_checkintime)
{
    guint32 ret;
    gchar str_checkintime[MAX_STRBUF_LEN] = {0};

    
    if (NULL == oem_checkintime || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_UNIT_INFO_CHK_IN_TIME, str_checkintime,
        MAX_STRBUF_LEN);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s: get unitdevice lifecycle fail", __FUNCTION__));

    if (0 == strlen(str_checkintime) || (0 == g_strcmp0(str_checkintime, "N/A"))) {
        debug_log(DLOG_MASS, "%s: unitdevice checkintime is invalid", __FUNCTION__);
        return;
    }

    *oem_checkintime = json_object_new_string((const gchar *)str_checkintime);
    do_if_expr(*oem_checkintime == NULL, debug_log(DLOG_DEBUG, "%s: call json_object_new_string failed", __FUNCTION__));
    return;
}


LOCAL void get_chassis_oem_assetowner(PROVIDER_PARAS_S *i_paras, json_object **oem_assetowner)
{
    gint32 ret;
    gchar str_assetowner[MAX_STRBUF_LEN] = {0};

    
    if (NULL == oem_assetowner || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_UNIT_INFO_ASSET_OWNER, str_assetowner,
        sizeof(str_assetowner));
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s: get unitdevice manufacture fail", __FUNCTION__));

    if (0 == strlen(str_assetowner) || (0 == g_strcmp0(str_assetowner, "N/A"))) {
        debug_log(DLOG_MASS, "%s: unitdevice assetowner is invalid", __FUNCTION__);
        return;
    }

    *oem_assetowner = json_object_new_string((const gchar *)str_assetowner);
    do_if_expr(*oem_assetowner == NULL, debug_log(DLOG_DEBUG, "%s: call json_object_new_string failed", __FUNCTION__));
    return;
}


LOCAL void get_chassis_oem_discoveredtime(PROVIDER_PARAS_S *i_paras, json_object **oem_discoveredtime)
{
    gchar str_discoveredtime[MAX_STRBUF_LEN] = {0};
    gint32 ret;
    guint32 timestamp = 0;

    
    if (NULL == oem_discoveredtime || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    (void)dal_get_property_value_uint32(i_paras->obj_handle, PROPERTY_UNIT_INFO_DISCOVERED_TIME, &timestamp);
    return_do_info_if_expr(0 == timestamp, debug_log(DLOG_DEBUG, "%s: device discovered time is null", __FUNCTION__));

    
    ret = dal_get_redfish_datetime_stamp(timestamp, str_discoveredtime, sizeof(str_discoveredtime));
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s dal_get_redfish_datetime_stamp fail.", __FUNCTION__));

    *oem_discoveredtime = json_object_new_string((const gchar *)str_discoveredtime);
    do_if_expr(*oem_discoveredtime == NULL,
        debug_log(DLOG_DEBUG, "%s: call json_object_new_string failed", __FUNCTION__));
    return;
}


LOCAL void get_chassis_oem_devicetype(PROVIDER_PARAS_S *i_paras, json_object **oem_devicetype)
{
    guint32 ret;
    gchar str_devicetype[MAX_STRBUF_LEN] = {0};

    
    if (NULL == oem_devicetype || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    ret =
        dal_get_property_value_string(i_paras->obj_handle, PROPERTY_UNIT_INFO_DEV_TYPE, str_devicetype, MAX_STRBUF_LEN);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s: get unitdevice devicetype fail", __FUNCTION__));

    if (0 == strlen(str_devicetype) || (0 == g_strcmp0(str_devicetype, "N/A"))) {
        debug_log(DLOG_MASS, "%s: unitdevice devicetype is invalid", __FUNCTION__);
        return;
    }

    *oem_devicetype = json_object_new_string((const gchar *)str_devicetype);
    do_if_expr(*oem_devicetype == NULL, debug_log(DLOG_DEBUG, "%s: call json_object_new_string failed", __FUNCTION__));
    return;
}


LOCAL void get_chassis_oem_lifecycle(PROVIDER_PARAS_S *i_paras, json_object **oem_lifecycle)
{
    gint32 ret;
    guint8 lifecycle = 0;

    
    if (NULL == oem_lifecycle || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_UNIT_INFO_LIFE_CYCLE, &lifecycle);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s: get unitdevice lifecycle fail", __FUNCTION__));

    return_do_info_if_expr(0 == lifecycle,
        debug_log(DLOG_DEBUG, "%s: device lifecycle is not valid, lifecycle is %d", __FUNCTION__, lifecycle));

    *oem_lifecycle = json_object_new_int(lifecycle);
    do_if_expr(*oem_lifecycle == NULL, debug_log(DLOG_DEBUG, "%s: call json_object_new_string failed", __FUNCTION__));
    return;
}


LOCAL void get_chassis_oem_indicator_color(PROVIDER_PARAS_S *i_paras, json_object **oem_indicator_color)
{
    gint32 ret;
    gchar str_indicator_color[MAX_STRBUF_LEN] = {0};

    
    if (NULL == oem_indicator_color || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_UNIT_INFO_INDICATOR_COLOR, str_indicator_color,
        sizeof(str_indicator_color));
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s: get unitdevice led color fail", __FUNCTION__));

    if (0 == strlen(str_indicator_color) || (0 == g_strcmp0(str_indicator_color, "N/A"))) {
        debug_log(DLOG_MASS, "%s: unitdevice led color is invalid", __FUNCTION__);
        return;
    }

    *oem_indicator_color = json_object_new_string((const gchar *)str_indicator_color);
    do_if_expr(*oem_indicator_color == NULL,
        debug_log(DLOG_DEBUG, "%s: call json_object_new_string failed", __FUNCTION__));
    return;
}


LOCAL void get_chassis_oem_rfid_uid(PROVIDER_PARAS_S *i_paras, json_object **oem_rfid_uid)
{
    gint32 ret;
    gchar str_rfid_uid[MAX_EXTEND_FIELD_LEN] = {0};

    
    if (NULL == oem_rfid_uid || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    ret = dal_get_byte_array_with_separator(i_paras->obj_handle, PROPERTY_UNIT_INFO_UID, ":", str_rfid_uid,
        sizeof(str_rfid_uid));
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s: get unitdevice RFID UID fail", __FUNCTION__));

    if (0 == strlen(str_rfid_uid) || (0 == g_strcmp0(str_rfid_uid, "N/A"))) {
        debug_log(DLOG_MASS, "%s: unitdevice RFID UID is invalid", __FUNCTION__);
        return;
    }

    *oem_rfid_uid = json_object_new_string((const gchar *)str_rfid_uid);
    do_if_expr(*oem_rfid_uid == NULL, debug_log(DLOG_DEBUG, "%s: call json_object_new_string failed", __FUNCTION__));
    return;
}

LOCAL void get_chassis_oem_rw_capability(PROVIDER_PARAS_S *i_paras, json_object **oem_rw_capability)
{
    gint32 ret;
    guint8 rw_capability = UNIT_INFO_RW;

    
    if (oem_rw_capability == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_UNIT_INFO_RW_CAPABILITY, &rw_capability);
    return_do_info_if_expr(ret != VOS_OK, debug_log(DLOG_MASS, "%s: get unitdevice RWCapability fail", __FUNCTION__));

    if (rw_capability == UNIT_INFO_RW) {
        *oem_rw_capability = json_object_new_string(UNIT_INFO_RW_STR);
    } else if (rw_capability == UNIT_INFO_READ_ONLY) {
        *oem_rw_capability = json_object_new_string(UNIT_INFO_READ_ONLY_STR);
    }

    do_if_expr(*oem_rw_capability == NULL,
        debug_log(DLOG_DEBUG, "%s: call json_object_new_string failed", __FUNCTION__));
    return;
}


LOCAL void get_chassis_oem_extend_field(PROVIDER_PARAS_S *i_paras, json_object **oem_extend_field)
{
    gint32 ret;
    gchar str_extend_field[MAX_EXTEND_FIELD_LEN] = {0};

    
    if (NULL == oem_extend_field || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_UNIT_INFO_EXTEND_FIELD, str_extend_field,
        sizeof(str_extend_field));
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s: get unitdevice extend filed fail", __FUNCTION__));

    if (0 == strlen(str_extend_field) || (0 == g_strcmp0(str_extend_field, "N/A"))) {
        debug_log(DLOG_MASS, "%s: unitdevice extend filed is invalid", __FUNCTION__);
        return;
    }

    *oem_extend_field = json_tokener_parse((const gchar *)str_extend_field);
    do_if_expr(*oem_extend_field == NULL, debug_log(DLOG_DEBUG, "%s: call json_tokener_parse failed", __FUNCTION__));
    return;
}


LOCAL gint32 get_chassis_unit_oem_property(PROVIDER_PARAS_S *i_paras, json_object *huawei_jso)
{
    json_object *oem_property = NULL;

    
    if (huawei_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    get_chassis_oem_topuid(i_paras, &oem_property);
    json_object_object_add(huawei_jso, RFPROP_UNITDEVICES_UNUM, oem_property);

    oem_property = NULL;
    get_chassis_oem_indicator_color(i_paras, &oem_property);
    json_object_object_add(huawei_jso, RFPROP_UNITDEVICES_INDICATOR_COLOR, oem_property);

    oem_property = NULL;
    get_chassis_oem_discoveredtime(i_paras, &oem_property);
    json_object_object_add(huawei_jso, RFPROP_UNITDEVICES_DISCOVEREDTIME, oem_property);

    oem_property = NULL;
    get_chassis_oem_height(i_paras, &oem_property);
    json_object_object_add(huawei_jso, RFPROP_UNITDEVICES_HEIGHT, oem_property);

    oem_property = NULL;
    get_chassis_oem_available_rack_space(i_paras, &oem_property);
    json_object_object_add(huawei_jso, RFPROP_UNITDEVICES_AVAILABLE_RACK_SPACE, oem_property);

    oem_property = NULL;
    get_chassis_oem_devicetype(i_paras, &oem_property);
    json_object_object_add(huawei_jso, RFPROP_UNITDEVICES_DEVICETYPE, oem_property);

    oem_property = NULL;
    get_chassis_oem_ratepower(i_paras, &oem_property);
    json_object_object_add(huawei_jso, RFPROP_UNITDEVICES_RATEPOWER, oem_property);

    oem_property = NULL;
    get_chassis_oem_checkintime(i_paras, &oem_property);
    json_object_object_add(huawei_jso, RFPROP_UNITDEVICES_CHECKINTIME, oem_property);

    oem_property = NULL;
    get_chassis_oem_assetowner(i_paras, &oem_property);
    json_object_object_add(huawei_jso, RFPROP_UNITDEVICES_ASSETOWNER, oem_property);

    oem_property = NULL;
    get_chassis_oem_lifecycle(i_paras, &oem_property);
    json_object_object_add(huawei_jso, RFPROP_UNITDEVICES_LIFECYCLE, oem_property);

    oem_property = NULL;
    get_chassis_oem_rfid_uid(i_paras, &oem_property);
    json_object_object_add(huawei_jso, RFPROP_UNITDEVICES_RFID_TAG_UID, oem_property);

    oem_property = NULL;
    get_chassis_oem_rw_capability(i_paras, &oem_property);
    json_object_object_add(huawei_jso, RFPROP_UNITDEVICES_RW_CAPABILITY, oem_property);

    oem_property = NULL;
    get_chassis_oem_extend_field(i_paras, &oem_property);
    json_object_object_add(huawei_jso, RFPROP_UNITDEVICES_EXTEND_FIELD, oem_property);

    get_chassis_oem_actions(i_paras, huawei_jso);

    return HTTP_OK;
}

LOCAL void get_oem_rack_info(PROVIDER_PARAS_S *i_paras, json_object *huawei_jso)
{
    rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_RACK_MODEL, RF_LOCATION_RACK_MODEL,
        huawei_jso);
    rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_TYPICAL_CONFIGURATION_SN,
        RF_LOCATION_TYPICAL_CONF_RACK_SN, huawei_jso);
    rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_EMPTY_RACK_SN, RF_LOCATION_EMPTY_RACK_SN,
        huawei_jso);
    rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_BASIC_RACK_SN,
        RF_LOCATION_BASIC_RACK_SN, huawei_jso);
    rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_RACK_FUNCTION, RF_LOCATION_RACK_FUNCTION,
        huawei_jso);
}


LOCAL gint32 get_chassis_rack_oem_property(PROVIDER_PARAS_S *i_paras, json_object *huawei_jso)
{
    guchar property_value = 0;
    json_object *property_jso = NULL;
    guint32 uint32_property_value;
    OBJ_HANDLE component_handle = 0;
    json_object *oem_property = NULL;
    OBJ_HANDLE obj_product_component = OBJ_HANDLE_COMMON;

    
    rf_add_property_jso_byte(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_U_COUNT, RF_LOCATION_UCOUNT, huawei_jso);
    rf_add_property_jso_byte(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_U_COUNT_USED, RF_LOCATION_UCOUNT_USED,
        huawei_jso);

    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_DIRECTION, &property_value);
    if (property_value == RF_FROM_BOTTOM_UP) {
        property_jso = json_object_new_string(RF_DIRECTION_FROM_BOTTOM_UP);
    } else if (property_value == RF_FROM_TOP_DOWN) {
        property_jso = json_object_new_string(RF_DIRECTION_FROM_TOP_DOWN);
    }
    json_object_object_add(huawei_jso, RF_LOCATION_DIRECTION, property_jso);

    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_DIRECTION_TYPE, &property_value);
    if (property_value == RACK_ASSET_MGMT_DIRECTION_CHANGEABLE) {
        property_jso = json_object_new_string(RACK_ASSET_MGMT_DIRECTION_CHANGEABLE_STR);
    } else if (property_value == RACK_ASSET_MGMT_DIRECTION_FIXED) {
        property_jso = json_object_new_string(RACK_ASSET_MGMT_DIRECTION_FIXED_STR);
    }
    json_object_object_add(huawei_jso, RF_LOCATION_DIRECTION_TYPE, property_jso);

    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_UNIT_OCCUPY_DIRECTION,
        &property_value);
    if (property_value == RACK_ASSET_MGMT_UNIT_OCCPUY_DOWNWARD) {
        property_jso = json_object_new_string(RF_UNIT_OCCUPY_DIRECTION_DOWNWARD);
    } else if (property_value == RACK_ASSET_MGMT_UNIT_OCCPUY_UPWARD) {
        property_jso = json_object_new_string(RF_UNIT_OCCUPY_DIRECTION_UPWARD);
    }
    json_object_object_add(huawei_jso, RF_UNIT_OCCUPY_DIRECTION, property_jso);

    
    (void)dal_get_property_value_uint32(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_LOAD_CAPACITY,
        &uint32_property_value);
    if (uint32_property_value == 0) {
        (void)json_object_object_add(huawei_jso, RF_LOCATION_LOAD_CAPACITY, NULL);
    } else {
        (void)json_object_object_add(huawei_jso, RF_LOCATION_LOAD_CAPACITY, json_object_new_int(uint32_property_value));
    }

    (void)dal_get_property_value_uint32(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_RATED_POWER,
        &uint32_property_value);
    if (uint32_property_value == 0) {
        (void)json_object_object_add(huawei_jso, RF_LOCATION_RATED_POWER, NULL);
    } else {
        (void)json_object_object_add(huawei_jso, RF_LOCATION_RATED_POWER, json_object_new_int(uint32_property_value));
    }

    get_oem_rack_info(i_paras, huawei_jso);

    (void)dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE, MAINBOARD_DEVICETYPE,
        &component_handle);
    oem_property = NULL;
    get_chassis_oem_board_odata_id(component_handle, &oem_property);
    do_val_if_expr(oem_property != NULL, json_object_object_add(huawei_jso, RFPROP_CHASSIS_BOARDS, oem_property));

    
    property_value = 0;
    (void)dal_get_object_handle_nth(CLASS_PRODUCT_COMPONENT, 0, &obj_product_component);
    (void)dal_get_property_value_byte(obj_product_component, PROPERTY_COMPONENT_BBU_MODULE_NUM, &property_value);
    if (property_value != 0) {
        oem_property = NULL;
        get_chassis_oem_odata_id(component_handle, &oem_property, URI_FORMAT_CHASSIS_BBUS);
        if (oem_property != NULL) {
            json_object_object_add(huawei_jso, RFPROP_CHASSIS_BBU, oem_property);
        }
    }

    get_chassis_oem_actions(i_paras, huawei_jso);
    return HTTP_OK;
    
}


LOCAL gint32 _get_node_enclosure_chassis_oem(PROVIDER_PARAS_S *i_paras, json_object *o_result_jso)
{
    json_object *prop_jso = NULL;
    OBJ_HANDLE product_handle = 0;
    guint32 product_id = 0;
    guint8 support_chassis_id = 0;

    return_val_do_info_if_fail(o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : input param is NULL", __FUNCTION__));

    (void)get_chassis_oem_id(&prop_jso, &support_chassis_id);
    
    if (support_chassis_id == 1) {
        json_object_object_add(o_result_jso, RFPROP_CHASSIS_ID, prop_jso);
        prop_jso = NULL;
    }

    (void)get_chassis_oem_powersupply_summary(&prop_jso);
    json_object_object_add(o_result_jso, RFPROP_CHASSIS_POWERSUPPLY_SUMMARY, prop_jso);
    prop_jso = NULL;

    (void)dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &product_handle);
    (void)dal_get_property_value_uint32(product_handle, PROPERTY_PRODUCT_ID, &product_id);
    if (check_pangea_node_to_enc(i_paras->obj_handle) &&
        check_pangea_uri_in_blade(URI_SPECIAL_BLADE_PREFIX, i_paras->uri) != TRUE) {
        
        (void)get_chassis_oem_odata_id(i_paras->obj_handle, &prop_jso, URI_FORMAT_CHASSIS_LED_GROUP);
        json_object_object_add(o_result_jso, RFPROP_CHASSIS_LED_GROUP, prop_jso);
        prop_jso = NULL;
    } else {
        (void)get_chassis_oem_board_odata_id(i_paras->obj_handle, &prop_jso);
        json_object_object_add(o_result_jso, RFPROP_CHASSIS_BOARDS, prop_jso);
        prop_jso = NULL;
    }

    (void)get_chassis_oem_board_odata_id(i_paras->obj_handle, &prop_jso);
    json_object_object_add(o_result_jso, RFPROP_CHASSIS_BOARDS, prop_jso);

    return HTTP_OK;
}

LOCAL void add_chassis_id_and_location(json_object *o_result_jso)
{
    guint8 support_chassis_id = 0;
    json_object *oem_property = NULL;

    get_chassis_oem_id(&oem_property, &support_chassis_id);
    if (support_chassis_id == 1) {
        json_object_object_add(o_result_jso, RFPROP_CHASSIS_ID, json_object_get(oem_property));
        oem_property = NULL;
    }

    get_chassis_oem_location(&oem_property);
    if (dal_check_if_vsmm_supported()) {
        json_object_object_add(o_result_jso, RFPROP_CHASSIS_LOCATION, json_object_get(oem_property));
        oem_property = NULL;
    }
    json_object_put(oem_property);
    return;
}

LOCAL void add_chassis_oem_odata_id(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    json_object *oem_property = NULL;
    guint8 bbu_module_num = 0;
    OBJ_HANDLE obj_product_component = OBJ_HANDLE_COMMON;

    get_chassis_oem_odata_id(obj_handle, &oem_property, URI_FORMAT_CHASSIS_SENSORLIST_THRESHOLD);
    if (oem_property != NULL) {
        json_object_object_add(o_result_jso, RFPROP_CHASSIS_THRESHOLD_SENSORS, oem_property);
    }

    oem_property = NULL;
    get_chassis_oem_odata_id(obj_handle, &oem_property, URI_FORMAT_CHASSIS_SENSORLIST_DISCRETE);
    if (oem_property != NULL) {
        json_object_object_add(o_result_jso, RFPROP_CHASSIS_DISCRETE_SENSORS, oem_property);
    }

    (void)dal_get_object_handle_nth(CLASS_PRODUCT_COMPONENT, 0, &obj_product_component);
    (void)dal_get_property_value_byte(obj_product_component, PROPERTY_COMPONENT_BBU_MODULE_NUM, &bbu_module_num);
    if (bbu_module_num != 0) {
        oem_property = NULL;
        get_chassis_oem_odata_id(obj_handle, &oem_property, URI_FORMAT_CHASSIS_BBUS);
        if (oem_property != NULL) {
            json_object_object_add(o_result_jso, RFPROP_CHASSIS_BBU, oem_property);
        }
    }

    return;
}

LOCAL void add_chassis_oem_fru(json_object *o_result_jso, guchar component_type)
{
    if (component_type != FRU_TYPE_BACKPLANE) {
        return;
    }

    (void)add_chassis_fru(o_result_jso);
    return;
}


LOCAL gint32 vsmm_get_one_component_info(OBJ_HANDLE handle, gpointer user_data)
{
    errno_t safe_fun_ret;
    json_object *components_array = (json_object *)user_data;
    json_object *component_obj = NULL;
    guint8 presence = 0;
    const gchar *blade_name = NULL;
    gchar blade_name_array[MAX_NAME_SIZE] = { 0 };

    
    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_PRESENCE, &presence);

    blade_name = dfl_get_object_name(handle);
    safe_fun_ret = strncpy_s(blade_name_array, sizeof(blade_name_array), blade_name, sizeof(blade_name_array) - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    component_obj = json_object_new_object();
    if (component_obj == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail.", __FUNCTION__);
        return RET_OK; // 当前单板信息添加失败，返回ok继续循环添加其他单板信息
    }

    json_object_object_add(component_obj, RFPROP_OVERVIEW_ID, json_object_new_string((const gchar *)blade_name_array));
    json_object_object_add(component_obj, PROTERY_IPMBETH_BLADE_PRESENCE,
        (presence == BLADE_PRESENCE) ? json_object_new_string("Present") : json_object_new_string("Absent"));
    json_object_array_add(components_array, component_obj);
    return RET_OK;
}

LOCAL void add_chassis_oem_components(json_object *o_result_jso)
{
    json_object *components_array = NULL;

    if (!dal_check_if_vsmm_supported()) {
        return;
    }
    components_array = json_object_new_array();
    if (components_array == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_array failed.", __FUNCTION__);
        return;
    }
    (void)dfl_foreach_object(CLASS_NAME_IPMBETH_BLADE, vsmm_get_one_component_info, components_array, NULL);
    json_object_object_add(o_result_jso, RFPROP_OVERVIEW_COMPONENTS, components_array);
}


LOCAL gint32 _get_common_chassis_oem(PROVIDER_PARAS_S *i_paras, json_object *o_result_jso, guchar component_type)
{
    typedef struct _CHASSIS_GET_ODATA_ {
        const gchar *odata_name;
        void (*get_odata_func)(OBJ_HANDLE obj_handle, json_object** board_odata_id);
    } CHASSIS_GET_ODATA;

    json_object *oem_property = NULL;
    const CHASSIS_GET_ODATA chassis_get_odata[] = {
        {RFPROP_CHASSIS_LANSWITCH_SUMMARY, get_chassis_oem_switches_odata_id}, // 获取交换链接
        {RFPROP_CHASSIS_BOARDS, get_chassis_oem_board_odata_id},
        {RFPROP_CHASSIS_NETWORK_ADAPTERS_SUMMARY, get_chassis_oem_networkadapters_summary},
        {RFPROP_CHASSIS_MAX_NUM, get_chassis_device_max_num},
        {RFPROP_SYSTEM_BOARD_MANUFACTURER, get_chassis_oem_board_manufacturer},
        {RFPROP_CHASSIS_ENERGY_EFFICIENCY, get_chassis_oem_energy_efficiency},
        {RFPROP_CHASSIS_MAINBOARD, get_chassis_oem_mainboard}
    };

    return_val_do_info_if_fail(o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : input param is NULL", __FUNCTION__));

    
    add_chassis_id_and_location(o_result_jso);

    get_chassis_oem_inlet_tem(component_type, &oem_property);
    do_val_if_expr(oem_property != NULL, json_object_object_add(o_result_jso, RFPROP_CHASSIS_INLET_TEM, oem_property));
    
    oem_property = NULL;
	
    
    get_chassis_oem_drive_summary(&oem_property);
    json_object_object_add(o_result_jso, RFPROP_CHASSIS_DRIVE_SUMMARY, oem_property);

    oem_property = NULL;
    get_chassis_oem_powersupply_summary(&oem_property);
    json_object_object_add(o_result_jso, RFPROP_CHASSIS_POWERSUPPLY_SUMMARY, oem_property);
    
    
    oem_property = NULL;
    (void)get_chassis_oem_package_code(&oem_property);
    json_object_object_add(o_result_jso, RFPROP_CHASSIS_PACKAGE_CODE, oem_property);
    

    for (gint32 i = 0; i < G_N_ELEMENTS(chassis_get_odata); i++) {
        oem_property = NULL;
        chassis_get_odata[i].get_odata_func(i_paras->obj_handle, &oem_property);
        if (oem_property != NULL) {
            json_object_object_add(o_result_jso, chassis_get_odata[i].odata_name, oem_property);
        }
    }

    add_chassis_oem_odata_id(i_paras->obj_handle, o_result_jso);
    add_chassis_oem_fru(o_result_jso, component_type);
    add_chassis_oem_components(o_result_jso);

    
    get_chassis_oem_actions(i_paras, o_result_jso);
    

    return HTTP_OK;
}


LOCAL gint32 get_chassis_oem_property_by_device_type(PROVIDER_PARAS_S *i_paras, json_object *huawei_jso)
{
    gint32 ret;
    guchar component_type = 0;

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
    return_val_do_info_if_fail((ret == VOS_OK), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "get property value for %s failed, ret is %d", PROPERTY_COMPONENT_DEVICE_TYPE, ret));

    
    
    if (is_node_enc_rsc(i_paras->obj_handle, NULL, NULL, &component_type)) {
        ret = _get_node_enclosure_chassis_oem(i_paras, huawei_jso);
    } else if (check_pangea_node_to_enc(i_paras->obj_handle)) {
        ret = _get_node_enclosure_chassis_oem(i_paras, huawei_jso);
    } else {
        ret = _get_common_chassis_oem(i_paras, huawei_jso, component_type);
    }
    if (check_pangea_node_to_enc(i_paras->obj_handle) &&
        check_pangea_uri_in_blade(URI_SPECIAL_BLADE_PREFIX, i_paras->uri)) {
        ret = _get_common_chassis_oem(i_paras, huawei_jso, component_type);
    }

    return ret;
}


LOCAL gint32 get_chassis_oem_property(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *huawei = NULL;
    
    gint32 ret;
    gchar class_name[MAX_NAME_SIZE] = {0};
    

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__));
    huawei = json_object_new_object();
    return_val_do_info_if_fail(NULL != huawei, HTTP_INTERNAL_SERVER_ERROR, json_object_put(*o_result_jso);
        *o_result_jso = NULL; debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__));
    json_object_object_add(*o_result_jso, RFPROP_COMMON_MANUFACTURER, huawei);

    // 获取class_name
    ret = dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR, json_object_put(*o_result_jso);
        *o_result_jso = NULL; debug_log(DLOG_ERROR, "%s: failed to call dfl_get_class_name", __FUNCTION__));

    if (0 == g_strcmp0(class_name, CLASS_UNIT_INFO)) {
        return get_chassis_unit_oem_property(i_paras, huawei);
    } else if (0 == g_strcmp0(class_name, CLASS_RACK_ASSET_MGMT)) {
        return get_chassis_rack_oem_property(i_paras, huawei);
    } else {
        return get_chassis_oem_property_by_device_type(i_paras, huawei);
    }
}


LOCAL gint32 set_chassis_rack_load_capacity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object *property)
{
    json_object *o_message_jso_temp = NULL; // 临时消息
    guint32 load_capacity;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_RACK_ASSET_MGMT_LOAD_CAPACITY,
        &o_message_jso_temp, PROPERTY_RACK_ASSET_MGMT_LOAD_CAPACITY);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

    ret = is_valid_integer_jso_or_null(property, RF_LOCATION_LOAD_CAPACITY, 0, G_MAXINT32, &o_message_jso_temp);
    if (ret != RET_OK) {
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        return ret;
    }
    load_capacity = (guint32)json_object_get_int64(property);

    ret = dfl_get_object_handle(CLASS_RACK_ASSET_MGMT, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_IMPLEMENTED,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, PROPERTY_RACK_ASSET_MGMT_LOAD_CAPACITY,
        &o_message_jso_temp, PROPERTY_RACK_ASSET_MGMT_LOAD_CAPACITY);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s:get CLASS_RACK_ASSET_MGMT obj_handle failed.", __FUNCTION__));

    input_list = g_slist_append(input_list, g_variant_new_uint32(load_capacity));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_RACK_ASSET_MGMT, METHOD_RACK_ASSET_MGMT_SET_LOAD_CAPACITY, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    switch (ret) {
        case VOS_OK:
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_RACK_ASSET_MGMT_LOAD_CAPACITY,
                &o_message_jso_temp, PROPERTY_RACK_ASSET_MGMT_LOAD_CAPACITY);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            return VOS_ERR;

        default:
            debug_log(DLOG_ERROR, "%s : call %s return %d", __FUNCTION__, PROPERTY_RACK_ASSET_MGMT_LOAD_CAPACITY, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            return VOS_ERR;
    }

    return VOS_OK;
}

LOCAL gint32 set_chassis_rack_rated_power(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object *property)
{
    json_object *o_message_jso_temp = NULL; // 临时消息
    guint32 rated_power;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_RACK_ASSET_MGMT_RATED_POWER,
        &o_message_jso_temp, PROPERTY_RACK_ASSET_MGMT_RATED_POWER);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

    ret = is_valid_integer_jso_or_null(property, RF_LOCATION_RATED_POWER, 0, G_MAXINT32, &o_message_jso_temp);
    if (ret != RET_OK) {
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        return ret;
    }
    rated_power = (guint32)json_object_get_int64(property);

    ret = dfl_get_object_handle(CLASS_RACK_ASSET_MGMT, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_IMPLEMENTED,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, PROPERTY_RACK_ASSET_MGMT_RATED_POWER,
        &o_message_jso_temp, PROPERTY_RACK_ASSET_MGMT_RATED_POWER);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s:get CLASS_RACK_ASSET_MGMT obj_handle failed.", __FUNCTION__));

    input_list = g_slist_append(input_list, g_variant_new_uint32(rated_power));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_RACK_ASSET_MGMT, METHOD_RACK_ASSET_MGMT_SET_RATED_POWER, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    switch (ret) {
        case VOS_OK:
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_RACK_ASSET_MGMT_RATED_POWER,
                &o_message_jso_temp, PROPERTY_RACK_ASSET_MGMT_RATED_POWER);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            return VOS_ERR;

        default:
            debug_log(DLOG_ERROR, "%s : call %s return %d", __FUNCTION__, PROPERTY_RACK_ASSET_MGMT_RATED_POWER, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            return VOS_ERR;
    }

    return VOS_OK;
}


guint32 set_huawei_chassis_id(PROVIDER_PARAS_S* i_paras, json_object* o_message_jso, json_object* huawei_obj)
{
    gint32 chassis_id;
    OBJ_HANDLE handle;
    json_object *chassi_id_jso = NULL;
    json_object *msg_jso = NULL;
    const gchar *str_id = NULL;
    GSList *input_list = NULL;

    if (json_object_object_get_ex(huawei_obj, RFPROP_CHASSIS_ID, &chassi_id_jso) == FALSE) {
        return HTTP_BAD_REQUEST;
    }

    str_id = dal_json_object_get_str(chassi_id_jso);
    chassis_id = arith_str_to_int(str_id);
    if (chassi_id_jso == NULL || str_id == NULL || strlen(str_id) == 0 || chassis_id < 0) { // 参数类型错误
        create_message_info(MSGID_PROP_TYPE_ERR, "Oem/Huawei/ChassisID", &msg_jso, str_id, "Oem/Huawei/ChassisID");
        json_object_array_add(o_message_jso, msg_jso);
        return HTTP_BAD_REQUEST;
    }

    if (chassis_id > 0xffe) { 
        create_message_info(MSGID_PROP_VALUE_OUTOFRANGE, "Oem/Huawei/ChassisID", &msg_jso,
            str_id, "Oem/Huawei/ChassisID", "0", "4094");
        json_object_array_add(o_message_jso, msg_jso);
        return HTTP_BAD_REQUEST;
    }

    gint32 ret = dfl_get_object_handle(OBJECT_NAME_CHASSIS, &handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] Get object %s handle failed, err code %d", __func__, OBJECT_NAME_CHASSIS, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32((guint32)chassis_id));
    input_list = g_slist_append(input_list, g_variant_new_byte(0));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, handle,
                                        OBJECT_NAME_CHASSIS, METHOD_CHASSIS_SET_CHASSIS_NUMBER, AUTH_DISABLE,
                                        i_paras->user_role_privilege, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Call class(%s) method(%s) failed, err code %d",
            __func__, OBJECT_NAME_CHASSIS, METHOD_CHASSIS_SET_CHASSIS_NUMBER, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL gint32 send_ipmi_set_chassis_id(guint8 target_instance, guint32 chassis_id)
{
    IPMI_REQ_MSG_HEAD_S request = { 0 };
    guint8 cmd[7] = {0};
    gpointer response = NULL;
    const guint8 *response_data = NULL;
    gint32 ret;

    request.target_type = IPMI_BMC;
    request.target_instance = target_instance;
    request.lun = 0;
    request.netfn = NETFN_OEM_REQ;
    request.cmd = IPMI_OEM_DFT_COMMAND;
    request.src_len = sizeof(cmd);
    cmd[0x0] = 0x61; // 发送chassisid给bmc的IPMI命令，子命令字为0x61
    cmd[0x1] = 0x01;
    cmd[0x2] = 0x04;
    cmd[0x3] = LONGB3(chassis_id);
    cmd[0x4] = LONGB2(chassis_id);
    cmd[0x5] = LONGB1(chassis_id);
    cmd[0x6] = LONGB0(chassis_id);

    ret = ipmi_send_request(&request, (gconstpointer)cmd, MAX_POSSIBLE_IPMI_FRAME_LEN, &response, TRUE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Send ipmi to [0x%x] failed, err code %d", target_instance, ret);
        return RET_ERR;
    }
    response_data = get_ipmi_src_data((gconstpointer)response);
    if (response_data == NULL) {
        debug_log(DLOG_ERROR, "Ipmi response data from [0x%x] is null", target_instance);
        g_free(response);
        return RET_ERR;
    }
    ret = response_data[0];
    if (ret != COMP_CODE_SUCCESS) {
        debug_log(DLOG_ERROR, "Ipmi response from [0x%x] is not ok, ret is 0x%x", target_instance, response_data[0]);
    }

    g_free(response);
    return ret;
}

LOCAL gint32 set_chassis_id_primary(PROVIDER_PARAS_S *i_paras, guint32 chassis_num)
{
    GSList *input_list = NULL;
    OBJ_HANDLE handle = 0;
    guint32 ret;
    if (provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dfl_get_object_handle(OBJECT_NAME_CHASSIS, &handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32(chassis_num));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, handle,
        CLASS_NAME_CHASSIS, METHOD_CHASSIS_SET_CHASSIS_NUMBER, AUTH_DISABLE, i_paras->user_role_privilege,
        input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Call class[%s] method[%s] failed, ret = %d", CLASS_NAME_CHASSIS,
            METHOD_CHASSIS_SET_CHASSIS_NUMBER, ret);
        return RET_ERR;
    }
    input_list = NULL;

    return RET_OK;
}

LOCAL gint32 set_chassis_id_secondary(PROVIDER_PARAS_S *i_paras, guint32 chassis_num)
{
    gint32 ret;
    guint8 master_slot_id;
    GSList *caller_info = NULL;
    if (provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    master_slot_id = dal_get_master_vsmm_slot_id();
    ret = send_ipmi_set_chassis_id(master_slot_id, chassis_num);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "send_ipmi_set_chassis_id failed. ret = %d.", ret);
        return RET_ERR;
    }

    caller_info = g_slist_append(caller_info, g_variant_new_string(USER_LOGIN_INTERFACE_REDFISH_STRING));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "send_ipmi_set_chassis_id failed. ret = %d.", ret);
        (void)proxy_method_operation_log(caller_info, "redfish", "Set chassis number failed");
        uip_free_gvariant_list(caller_info);
        return RET_ERR;
    }
    (void)proxy_method_operation_log(caller_info, "redfish", "Set chassis number to %d successfully", chassis_num);

    uip_free_gvariant_list(caller_info);
    return RET_OK;
}

LOCAL gint32 set_chassis_id(PROVIDER_PARAS_S *i_paras, json_object *chassis_id_jso, json_object **o_message_jso,
    json_object *msg_jso)
{
    gint32 ret;
    OBJ_HANDLE handle = 0;
    guint32 chassis_num;
    const gchar *chassis_id = "Oem/Huawei/ChassisID";

    if (!dal_check_if_vsmm_supported()) {
        create_message_info(MSGID_PROP_NOT_WRITABLE, chassis_id, &msg_jso, chassis_id);
        json_object_array_add(*o_message_jso, msg_jso);
        return RET_ERR;
    }

    chassis_num = arith_str_to_int(dal_json_object_get_str(chassis_id_jso));
    if (chassis_num > RF_MAX_CHASSIS_ID) {
        create_message_info(MSGID_PROP_VALUE_OUTOFRANGE, chassis_id, &msg_jso,
            dal_json_object_get_str(chassis_id_jso), RFPROP_CHASSIS_ID, "0", "999999");
        json_object_array_add(*o_message_jso, msg_jso);
        return RET_ERR;
    }

    ret = dfl_get_object_handle(OBJECT_NAME_CHASSIS, &handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get object %s handle failed, err code %d", OBJECT_NAME_CHASSIS, ret);
        return RET_ERR;
    }
    if (dal_get_vsmm_ms_status() == MSM_PRIMARY_STATE) {
        ret = set_chassis_id_primary(i_paras, chassis_num);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: set_chassis_id_primary failed. ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    } else {
        ret = set_chassis_id_secondary(i_paras, chassis_num);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: set_chassis_id_secondary failed. ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    }
    return RET_OK;
}

LOCAL gint32 send_ipmi_set_chassis_location(const gchar *location)
{
    IPMI_REQ_MSG_HEAD_S request = { 0 };
    guint8 cmd[RF_CHASSIS_LOCATION_MAX_LEN + 3] = {0};
    gpointer response = NULL;
    const guint8 *response_data = NULL;
    guint8 instance;
    gint32 ret;

    if (location == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return RET_ERR;
    }
    instance = dal_get_master_vsmm_slot_id();
    request.target_type = IPMI_BMC;
    request.target_instance = instance;
    request.lun = 0;
    request.netfn = NETFN_OEM_REQ;
    request.cmd = IPMI_OEM_DFT_COMMAND;
    request.src_len = strlen(location) + 3; // 3代表网络功能码、命令字和子命令字的字节长度和
    cmd[0x0] = 0x61; // 发送chassisid给bmc的IPMI命令，子命令字为0x61
    cmd[0x1] = 0x02;
    cmd[0x2] = strlen(location);
    errno_t safe_fun_ret = memcpy_s(&cmd[0x3], RF_CHASSIS_LOCATION_MAX_LEN, location, strlen(location));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    ret = ipmi_send_request(&request, (gconstpointer)cmd, MAX_POSSIBLE_IPMI_FRAME_LEN, &response, TRUE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Send ipmi to [0x%x] failed, err code %d", instance, ret);
        return RET_ERR;
    }
    response_data = get_ipmi_src_data(response);
    if (response_data == NULL) {
        debug_log(DLOG_ERROR, "Ipmi response data from [0x%x] is null", instance);
        g_free(response);
        return RET_ERR;
    }
    ret = response_data[0];
    if (ret != COMP_CODE_SUCCESS) {
        debug_log(DLOG_ERROR, "Ipmi response from [0x%x] is not ok, ret = 0x%x", instance, response_data[0]);
    }

    g_free(response);
    return ret;
}

LOCAL gint32 set_chassis_location_primary(PROVIDER_PARAS_S *i_paras, const gchar *chassis_location)
{
    GSList *input_list = NULL;
    OBJ_HANDLE handle = 0;
    guint32 ret;
    guint8 write_eeprom_flag = 1;
    guint8 broadcast_flag = 1;

    if (provider_paras_check(i_paras) != RET_OK || chassis_location == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dfl_get_object_handle(OBJECT_NAME_CHASSIS, &handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(chassis_location));
    input_list = g_slist_append(input_list, g_variant_new_byte(write_eeprom_flag));
    input_list = g_slist_append(input_list, g_variant_new_byte(broadcast_flag));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, handle,
        CLASS_NAME_CHASSIS, METHOD_CHASSIS_SET_CHASSIS_LOCATION, AUTH_DISABLE, i_paras->user_role_privilege,
        input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Call class[%s] method[%s] failed, ret = %d", CLASS_NAME_CHASSIS,
            METHOD_CHASSIS_SET_CHASSIS_LOCATION, ret);
        return RET_ERR;
    }
    input_list = NULL;

    return RET_OK;
}

LOCAL gint32 set_chassis_location_secondary(PROVIDER_PARAS_S *i_paras, const gchar *chassis_location)
{
    gint32 ret;
    GSList *caller_info = NULL;
    if (provider_paras_check(i_paras) != RET_OK || chassis_location == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = send_ipmi_set_chassis_location(chassis_location);

    caller_info = g_slist_append(caller_info, g_variant_new_string(USER_LOGIN_INTERFACE_REDFISH_STRING));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "send_ipmi_set_chassis_location failed. ret = %d.", ret);
        (void)proxy_method_operation_log(caller_info, "redfish", "Set chassis location failed");
        uip_free_gvariant_list(caller_info);
        return RET_ERR;
    }
    (void)proxy_method_operation_log(caller_info, "redfish", "Set chassis location to %s successfully",
        chassis_location);

    uip_free_gvariant_list(caller_info);
    return RET_OK;
}

LOCAL gint32 set_huawei_chassis_location(PROVIDER_PARAS_S *i_paras, json_object *property, json_object **o_message_jso,
    json_object *msg_jso)
{
    gint32 ret;
    const gchar *chassis_location = NULL;
    gchar location_str[RF_CHASSIS_LOCATION_MAX_LEN] = { 0 };
    errno_t safe_fun_ret = EOK;
    const gchar *location = "Oem/Huawei/ChassisLocation";

    if (!dal_check_if_vsmm_supported()) {
        return RET_ERR;
    }

    chassis_location = dal_json_object_get_str(property);
    if (strlen(chassis_location) > RF_CHASSIS_LOCATION_MAX_LEN) {
        create_message_info(MSGID_VALUE_OUT_OF_RANGE, location, &msg_jso, location);
        json_object_array_add(*o_message_jso, msg_jso);
        return RET_ERR;
    }

    safe_fun_ret = strncpy_s(location_str, sizeof(location_str), chassis_location, sizeof(location_str) - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    if (dal_check_string_is_valid_ascii(location_str) != VOS_OK) {
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, location, &msg_jso, chassis_location, location);
        json_object_array_add(*o_message_jso, msg_jso);
        return RET_ERR;
    }

    if (dal_get_vsmm_ms_status() == MSM_PRIMARY_STATE) {
        ret = set_chassis_location_primary(i_paras, chassis_location);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: set_chassis_location_primary failed. ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    } else {
        ret = set_chassis_location_secondary(i_paras, chassis_location);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: set_chassis_location_secondary failed. ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    }

    return RET_OK;
}


LOCAL gint32 set_chassis_huawei(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object *huawei_obj)
{
    gint32 ret;
    gint32 ret_set = 0;
    json_object *property = NULL;
    gboolean flag = FALSE;
    json_object *chassi_id_jso = NULL;
    json_object *msg_jso = NULL;

    
    if (json_object_object_get_ex(huawei_obj, RFPROP_CHASSIS_ID, &chassi_id_jso)) {
        ret = set_chassis_id(i_paras, chassi_id_jso, o_message_jso, msg_jso);
        if (ret == RET_OK) {
            flag = TRUE;
        }
    }
    if (json_object_object_get_ex(huawei_obj, RFPROP_CHASSIS_LOCATION, &property)) {
        ret = set_huawei_chassis_location(i_paras, property, o_message_jso, msg_jso);
        if (ret == RET_OK) {
            flag = TRUE;
        }
    }

    ret = json_object_object_get_ex(huawei_obj, PROPERTY_RACK_ASSET_MGMT_LOAD_CAPACITY, &property);
    if (ret) { // 用户在body中有该属性的设置
        ret_set = set_chassis_rack_load_capacity(i_paras, o_message_jso, property);
        do_val_if_expr(ret_set == VOS_OK, (flag = TRUE));
    }

    ret = json_object_object_get_ex(huawei_obj, PROPERTY_RACK_ASSET_MGMT_RATED_POWER, &property);
    if (ret) { // 用户在body中有该属性的设置
        ret_set = set_chassis_rack_rated_power(i_paras, o_message_jso, property);
        do_val_if_expr(ret_set == VOS_OK, (flag = TRUE));
    }

    ret = json_object_object_get_ex(huawei_obj, PROPERTY_RACK_ASSET_MGMT_DIRECTION, &property);
    if (ret && property) {
        ret_set = set_unit_direction(i_paras, o_message_jso, property);
        do_val_if_expr(ret_set == VOS_OK, (flag = TRUE));
    }

    if (flag == TRUE) {
        return VOS_OK; // 成功设置所有的属性
    } else {
        return VOS_ERR; // 成功设置部分的属性
    }
}

LOCAL gint32 set_chassis_oem_property(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    json_object *huawei_jso = NULL;
    json_object *msg_jso = NULL;

    guint32 ret = 0;
    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    *o_message_jso = json_object_new_array();
    if (!json_object_object_get_ex(i_paras->val_jso, RFPROP_OEM_HUAWEI, &huawei_jso)) {
        
        create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_OEM, &msg_jso, RFPROP_OEM);
        json_object_array_add(*o_message_jso, msg_jso);
        return HTTP_BAD_REQUEST;
    }

    
    if (dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
        if ((i_paras->user_role_privilege & USERROLE_BASICSETTING) != USERROLE_BASICSETTING) {
            create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_OEM, o_message_jso, RFPROP_OEM);
            return HTTP_FORBIDDEN;
        }
        ret = set_huawei_chassis_id(i_paras, *o_message_jso, huawei_jso);
        if (ret == HTTP_INTERNAL_SERVER_ERROR) {
            create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_jso);
            json_object_array_add(*o_message_jso, msg_jso);
        }
        return ret;
    }

    ret = set_chassis_huawei(i_paras, o_message_jso, huawei_jso);
    if (ret != RET_OK) {
        return HTTP_BAD_REQUEST;
    }

    return HTTP_OK;
    
}


LOCAL gint32 set_unit_direction(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object *o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    json_object *message_temp_jso = NULL;
    guchar property_value = 0;

    
    return_val_do_info_if_fail(NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    
    gint32 ret = dal_get_object_handle_nth(CLASS_RACK_ASSET_MGMT, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s: get %s obj handle fail(%d).", __FUNCTION__, CLASS_RACK_ASSET_MGMT, ret);
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, PROPERTY_RACK_ASSET_MGMT_DIRECTION, &message_temp_jso,
            PROPERTY_RACK_ASSET_MGMT_DIRECTION);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    
    if (0 == g_strcmp0(dal_json_object_get_str(o_result_jso), RF_DIRECTION_FROM_BOTTOM_UP)) {
        property_value = RF_FROM_BOTTOM_UP;
    } else if (0 == g_strcmp0(dal_json_object_get_str(o_result_jso), RF_DIRECTION_FROM_TOP_DOWN)) {
        property_value = RF_FROM_TOP_DOWN;
    }
    input_list = g_slist_append(input_list, g_variant_new_byte(property_value));
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_RACK_ASSET_MGMT, METHOD_RACK_ASSET_MGMT_SETDIRECTION, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);
    uip_free_gvariant_list(input_list);
    switch (ret) {
        case VOS_OK:
            break;
        case RFERR_SUCCESS:
            ret = RET_OK;
            break;

            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret = RFERR_INSUFFICIENT_PRIVILEGE;
            break;

            
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_temp_jso, i_paras->uri);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret = HTTP_NOT_FOUND;
            break;

            
        case SET_UNIT_DIRECTION_NOT_SUPPORTED:
            (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, &message_temp_jso,
                PROPERTY_RACK_ASSET_MGMT_DIRECTION);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret = HTTP_BAD_REQUEST;
            break;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret = HTTP_INTERNAL_SERVER_ERROR;
            break;
    }

    return ret;
}


LOCAL gint32 set_unit_rfid_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    json_object *trans_key_json)
{
#define MGMT_TYPE_NETWORK_UNIT_INFO_MAX_LEN "376"
    gint32 ret;
    GSList *input_list = NULL;
    guint16 str_len;
    json_object *height_jso = NULL;

    
    return_val_do_info_if_fail(NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    

    str_len = (guint16)json_object_get_string_len(trans_key_json);
    input_list = g_slist_append(input_list, g_variant_new_uint16(str_len));
    input_list = g_slist_append(input_list, g_variant_new_string(dal_json_object_get_str(trans_key_json)));
    ret =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->obj_handle,
        CLASS_UNIT_INFO, METHOD_SET_UNIT_INFO, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);
    switch (ret) {
        case VOS_OK:
            ret = HTTP_OK;
            break;
        case RFERR_SUCCESS:
            ret = HTTP_OK;
            break;

            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            ret = RFERR_INSUFFICIENT_PRIVILEGE;
            break;

            
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            ret = HTTP_NOT_FOUND;
            break;

            
        case SET_RFID_STATUS_ERR:
            (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, RFPROP_CHASSIS_OEM_ACTION_SET_UNIT_RFID_INFO,
                o_message_jso, RFPROP_CHASSIS_OEM_ACTION_SET_UNIT_RFID_INFO);
            ret = HTTP_BAD_REQUEST;
            debug_log(DLOG_ERROR, "the Tag is not found ,ret is %d !\r\n", ret);
            break;

            
        case SET_RFID_HEIGHT_ERR:
            if (TRUE == json_object_object_get_ex(trans_key_json, TRANSFER_UNIT_INFO_HIGHT, &height_jso)) {
                (void)create_message_info(MSGID_INVALID_ACTION_PARAM_VALUE, NULL, o_message_jso,
                    dal_json_object_get_str(height_jso), RFPROP_UNITDEVICES_HEIGHT);
                debug_log(DLOG_ERROR, "%s: %s param invalid. ret=%d", __FUNCTION__, RFPROP_UNITDEVICES_HEIGHT, ret);
                ret = HTTP_BAD_REQUEST;
                break;
            }
            
        case SET_RFID_EEP_INFO_TOO_LONG_ERR:
            (void)create_message_info(MSGID_BODY_EXC_MAXLEN, NULL, o_message_jso, MGMT_TYPE_NETWORK_UNIT_INFO_MAX_LEN);
            debug_log(DLOG_ERROR, "%s: info too long for one eeprom block, ret=%d", __FUNCTION__, ret);
            ret = HTTP_BAD_REQUEST;
            break;
            
        case SET_RFID_SWITCH_COUNT_LIMIT_ERR:
            (void)create_message_info(MSGID_INVALID_ACTION_PARAM_VALUE, NULL, o_message_jso, DEVICE_TYPE_NETWORK_STR,
                RFPROP_UNITDEVICES_DEVICETYPE);
            debug_log(DLOG_ERROR, "%s: switch count exceed limit, ret=%d", __FUNCTION__, ret);
            ret = HTTP_BAD_REQUEST;
            break;
            

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            ret = HTTP_INTERNAL_SERVER_ERROR;
            break;
    }

    return ret;
}


LOCAL gint32 chassis_get_odata_id(gchar *common_uri, gchar *chassis_common_uri, gint32 len_chassis_common_uri)
{
    int iRet;
    gint32 ret;
    gchar slot[MAX_RSC_ID_LEN + 1] = {0};

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: get slot id error.\n", __FUNCTION__, __LINE__));

    iRet = snprintf_s(chassis_common_uri, len_chassis_common_uri, len_chassis_common_uri - 1, "%s%s", common_uri, slot);
    return_val_do_info_if_expr(iRet <= 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet));
    return VOS_OK;
}

LOCAL gint32 get_chassis_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar str_odata_id[MAX_STRBUF_LEN + 1] = {0};
    gchar               chassis_id[MAX_MEM_ID_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    

    
    ret = rf_gen_chassis_component_id(i_paras->member_id, 0, chassis_id, MAX_MEM_ID_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "gen chassis component id failed, ret is %d", ret));

    ret = snprintf_s(str_odata_id, MAX_STRBUF_LEN + 1, MAX_STRBUF_LEN, URI_FORMAT_CHASSIS, chassis_id);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "format sting failed, ret is %d", ret));

    *o_result_jso = json_object_new_string((const gchar *)str_odata_id);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}


LOCAL gint32 get_chassis_componet_type(PROVIDER_PARAS_S *i_paras, guchar *component_type)
{
    gint32 ret;
    gchar class_name[MAX_NAME_SIZE] = {0};

    ret = dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: failed to call dfl_get_class_name", __FUNCTION__));

    if (0 == g_strcmp0(class_name, CLASS_RACK_ASSET_MGMT)) {
        *component_type = COMPONENT_TYPE_MAINBOARD;
    } else if (0 != g_strcmp0(class_name, CLASS_COMPONENT)) {
        return VOS_ERR;
    } else {
        ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, component_type);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_ERROR, "get property value for %s failed, ret is %d", __FUNCTION__, ret));
    }

    return VOS_OK;
}


LOCAL gint32 get_chassis_thermal_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar chassis_id[MAX_MEM_ID_LEN] = {0};
    gchar thermal_odata_id[MAX_STRBUF_LEN + 1] = {0};
    
    guchar component_type = 0;
    guchar board_type = 0;
    

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    gint32 ret = get_chassis_componet_type(i_paras, &component_type);
    return_val_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR);

    
    ret = dal_rf_get_board_type(&board_type);
    return_val_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR);

    
    
    gboolean check_ret = is_node_enc_rsc(i_paras->obj_handle, &board_type, NULL, &component_type);
    
    if (check_ret != TRUE && check_pangea_node_to_enc(i_paras->obj_handle) != TRUE) {
        
        if (((BOARD_MM != board_type) && (COMPONENT_TYPE_MAINBOARD != component_type)) ||
            ((BOARD_MM == board_type) && (COMPONENT_TYPE_CHASSIS_BACKPLANE != component_type))) {
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }
    

    ret = rf_gen_chassis_component_id(i_paras->member_id, 0, chassis_id, MAX_MEM_ID_LEN);
    do_if_expr(VOS_OK != ret, return HTTP_INTERNAL_SERVER_ERROR);

    ret = snprintf_s(thermal_odata_id, sizeof(thermal_odata_id), sizeof(thermal_odata_id) - 1,
        URI_FORMAT_CHASSIS_THERMAL, chassis_id);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "format string failed, ret is %d", ret));

    *o_result_jso = json_object_new_object();
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    json_object_object_add(*o_result_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)thermal_odata_id));
    return HTTP_OK;
}


LOCAL gint32 get_chassis_networkadapters_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar chassis_id[MAX_MEM_ID_LEN] = {0};
    gchar adapters_odata_id[MAX_URI_LENGTH] = {0};
    
    guchar x86_enable = 0;
    guchar component_type = 0;
    

    gchar class_name[MAX_NAME_SIZE] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_X86, &x86_enable);
    return_val_if_fail(ENABLE == x86_enable, HTTP_INTERNAL_SERVER_ERROR);

    ret = dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: failed to call dfl_get_class_name", __FUNCTION__));

    if (0 != g_strcmp0(class_name, CLASS_COMPONENT)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
    return_val_if_fail((COMPONENT_TYPE_MAINBOARD == component_type), HTTP_INTERNAL_SERVER_ERROR);
    

    ret = rf_gen_chassis_component_id(i_paras->member_id, 0, chassis_id, MAX_MEM_ID_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "cann't get chassis id for component (%s)", dfl_get_object_name(i_paras->obj_handle)));

    do_val_if_expr(0 >= snprintf_s(adapters_odata_id, sizeof(adapters_odata_id), sizeof(adapters_odata_id) - 1,
        CHASSIS_NETWORK_ADAPTERS, chassis_id),
        debug_log(DLOG_ERROR, "%s %d: snprintf_s fail!", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_object();
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    json_object_object_add(*o_result_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)adapters_odata_id));

    return HTTP_OK;
}


LOCAL gint32 get_chassis_pciedevices_collection_odata_id(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
                                                                              json_object** o_result_jso)
{
    gint32 ret;
    errno_t secure_rv;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar pciedevices_odata_id[MAX_URI_LENGTH] = {0};

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return_val_if_fail(check_enclosure_component_type(i_paras->obj_handle, FALSE) == VOS_OK,
        HTTP_INTERNAL_SERVER_ERROR);

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    secure_rv = snprintf_s(pciedevices_odata_id, sizeof(pciedevices_odata_id), sizeof(pciedevices_odata_id) - 1,
        URI_FORMAT_CHASSIS_PCIEDEVICE_COLLECTION, slot);
    return_val_do_info_if_expr(secure_rv <= 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : snprintf_s uri fail", __FUNCTION__));

    *o_result_jso = json_object_new_object();
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    json_object_object_add(*o_result_jso, RFPROP_ODATA_ID, json_object_new_string((const char *)pciedevices_odata_id));
    return HTTP_OK;
}


LOCAL gint32 get_chassis_drives_collection_odata_id(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
                                                                       json_object** o_result_jso)
{
    gint32 ret;
    errno_t secure_rv;
    guchar x86_enable = 0;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar drives_odata_id[MAX_URI_LENGTH] = {0};

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_X86, &x86_enable);
    return_val_if_expr(x86_enable == DISABLE, HTTP_INTERNAL_SERVER_ERROR);

    return_val_if_fail(check_enclosure_component_type(i_paras->obj_handle, FALSE) == VOS_OK,
        HTTP_INTERNAL_SERVER_ERROR);

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    secure_rv = snprintf_s(drives_odata_id, sizeof(drives_odata_id), sizeof(drives_odata_id) - 1,
        URI_FORMAT_CHASSIS_DRIVE_COLLECTION, slot);
    return_val_do_info_if_expr(secure_rv <= 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : snprintf_s uri fail", __FUNCTION__));

    *o_result_jso = json_object_new_object();
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    json_object_object_add(*o_result_jso, RFPROP_ODATA_ID, json_object_new_string((const char *)drives_odata_id));
    return HTTP_OK;
}


LOCAL gint32 get_chassis_power_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar chassis_id[MAX_MEM_ID_LEN] = {0};
    gchar power_odata_id[MAX_URI_LENGTH] = {0};
    
    guchar component_type = 0;
    guchar board_type = 0;
    

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    if (check_pangea_uri_in_blade(URI_SPECIAL_BLADE_PREFIX, i_paras->uri)) {
        return HTTP_OK;
    }

    
    gint32 ret = get_chassis_componet_type(i_paras, &component_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d: get_chassis_componet_type failed.", __FUNCTION__, __LINE__));

    (void)dal_rf_get_board_type(&board_type);

    
    
    gboolean check_ret = is_node_enc_rsc(i_paras->obj_handle, &board_type, NULL, &component_type);
    if (dal_check_if_vsmm_supported()) {
        check_ret = TRUE;
    }
    if (!check_ret && !check_pangea_node_to_enc(i_paras->obj_handle)) {
        
        if (((BOARD_MM != board_type) && (COMPONENT_TYPE_MAINBOARD != component_type)) ||
            ((BOARD_MM == board_type) && (COMPONENT_TYPE_CHASSIS_BACKPLANE != component_type))) {
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    

    ret = rf_gen_chassis_component_id(i_paras->member_id, 0, chassis_id, MAX_MEM_ID_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "cann't get chassis id with component (%s)", dfl_get_object_name(i_paras->obj_handle)));

    int iRet = snprintf_s(power_odata_id, sizeof(power_odata_id), sizeof(power_odata_id) - 1, URI_FORMAT_CHASSIS_POWER,
        chassis_id);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }

    *o_result_jso = json_object_new_object();
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    json_object_object_add(*o_result_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)power_odata_id));
    return HTTP_OK;
}

LOCAL void generate_chassis_pciedevice_jso(json_object *o_result_jso, const gchar *pciedevice_uri)
{
    gint32 ret;
    json_object *jso_obj = NULL;
    json_object *jso_prop = NULL;

    if (o_result_jso == NULL || pciedevice_uri == NULL) {
        return;
    }

    jso_obj = json_object_new_object();
    if (jso_obj == NULL) {
        return;
    }

    jso_prop = json_object_new_string(pciedevice_uri);
    if (jso_prop == NULL) {
        json_object_put(jso_obj);
        return;
    }

    (void)json_object_object_add(jso_obj, RFPROP_ODATA_ID, jso_prop);
    ret = json_object_array_add(o_result_jso, jso_obj);
    if (ret != 0) {
        json_object_put(jso_obj);
    }
    jso_obj = NULL;
    jso_prop = NULL;

    return;
}

LOCAL void get_pcie_slot(OBJ_HANDLE obj_handle, guchar *pcie_card_slot)
{
    if (check_npu_handle_with_cucc_customized(obj_handle)) {
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_NPU_ID, pcie_card_slot);
        return;
    }
    
    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_LOGIC_SLOT, pcie_card_slot);
    if (ret != RET_OK || *pcie_card_slot == INVALID_VAL) {
        (void)dal_get_property_value_byte(obj_handle, PROPETRY_PCIECARD_SLOT, pcie_card_slot);
    }
}

LOCAL void generate_chassis_pciedevice_odata_id(OBJ_HANDLE obj_handle, json_object *o_result_jso, const char *card_type)
{
    gint32 ret;
    guint8 pcie_presence = 0;
    guchar pcie_card_slot = INVALID_VAL;
    OBJ_HANDLE chassis_comp_handle = 0;
    gchar pcie_memberid[MAX_RSC_ID_LEN] = {0};
    gchar slot[MAX_RSC_ID_LEN + 1] = {0};
    gchar pciedevice_uri[MAX_STRBUF_LEN] = {0};

    if (card_type == NULL || o_result_jso == NULL) {
        return;
    }

    
    ret = dal_get_property_value_byte(obj_handle, PROPETRY_PCIECARD_PRESENCE, &pcie_presence);
    if (ret != VOS_OK || pcie_presence == 0) {
        return;
    }

    
    ret = rf_get_object_location_handle(0, obj_handle, &chassis_comp_handle);
    if (ret != VOS_OK) {
        return;
    }

    get_pcie_slot(obj_handle, &pcie_card_slot);
    ret = snprintf_s(pcie_memberid, MAX_RSC_ID_LEN, MAX_RSC_ID_LEN - 1, "%s%u", card_type, pcie_card_slot);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "format pcie member id failed, ret is %d", ret);
        return;
    }

    
    ret = rf_gen_chassis_component_id(slot, chassis_comp_handle, slot, MAX_RSC_ID_LEN);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "gen chassis component id with location handle [%s] failed",
            dfl_get_object_name(chassis_comp_handle));
        return;
    }

    ret = snprintf_s(pciedevice_uri, sizeof(pciedevice_uri), sizeof(pciedevice_uri) - 1,
        URI_FORMAT_CHASSIS_PCIEDEVICES_URI, slot, pcie_memberid);
    if (ret <= 0) {
        debug_log(DLOG_MASS, "%s %d: snprintf_s fail.", __FUNCTION__, __LINE__);
        return;
    }

    generate_chassis_pciedevice_jso(o_result_jso, (const gchar *)pciedevice_uri);

    return;
}

void redfish_get_chassis_pciedevice_odata_id(json_object *o_result_jso, const char *class_type)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    gchar card_type[MAX_CARD_TYPE_LEN] = {0};

    if (g_ascii_strcasecmp(class_type, CLASS_OCP_CARD) == 0) {
        ret = strcpy_s(card_type, sizeof(card_type), RF_OCPCARD_PREFIX);
    } else if (g_ascii_strcasecmp(class_type, CLASS_NPU) == 0) {
        ret = strcpy_s(card_type, sizeof(card_type), CLASS_NPU);
    } else {
        ret = strcpy_s(card_type, sizeof(card_type), RF_PCIECARD_PREFIX);
    }
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d\n", __FUNCTION__, ret);
        return;
    }

    
    ret = dfl_get_object_list(class_type, &obj_list);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s, %d: get obj lsit fail.\n", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        generate_chassis_pciedevice_odata_id((OBJ_HANDLE)obj_node->data, o_result_jso, card_type);
    }

    g_slist_free(obj_list);
    return;
}


LOCAL void chassis_get_drives_odata_id(const gchar *class_name, json_object *o_drives_odata_id)
{
    gint32 ret;
    guchar hdd_presence = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    json_object *obj_json = NULL;
    json_object *prop_json = NULL;
    gchar  driver_member_id[STORAGE_CTRL_INFO_LENGTH] = {0};
    gchar  driver_info[STORAGE_CTRL_NAME_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_do_info_if_expr(VOS_ERR == ret, debug_log(DLOG_MASS, "%s:%d fail", __FUNCTION__, __LINE__));

    
    ret = dfl_get_object_list(class_name, &obj_list);
    do_val_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s, %d: get storage controller fail, ret = %d\n", __FUNCTION__, __LINE__, ret));

    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_PRESENCE, &hdd_presence);
        do_if_expr(((VOS_OK != ret) || (0 == hdd_presence)), continue);

        
        (void)memset_s(driver_member_id, sizeof(driver_member_id), 0, sizeof(driver_member_id));

        ret = redfish_get_drives_location_devicename((OBJ_HANDLE)obj_node->data, driver_member_id,
            sizeof(driver_member_id));
        do_if_expr(VOS_OK != ret, continue);

        obj_json = NULL;
        prop_json = NULL;
        
        obj_json = json_object_new_object();
        do_if_expr(NULL == obj_json, continue);
        (void)memset_s(driver_info, sizeof(driver_info), 0, sizeof(driver_info));

        ret = snprintf_s(driver_info, sizeof(driver_info), sizeof(driver_info) - 1, URI_FORMAT_CHASSIS_STORAGE_DRIVES,
            slot, driver_member_id);
        do_if_expr(0 >= ret, debug_log(DLOG_ERROR, "%s %d: snprintf_s fail.", __FUNCTION__, __LINE__);
            json_object_put(obj_json); continue);
        
        prop_json = json_object_new_string((const gchar *)driver_info);
        do_if_expr(NULL == prop_json, json_object_put(obj_json); continue);
        (void)json_object_object_add(obj_json, RFPROP_ODATA_ID, prop_json);

        
        ret = json_object_array_add(o_drives_odata_id, obj_json);
        do_if_expr(ret, json_object_put(obj_json);
            debug_log(DLOG_ERROR, "%s, %d: json object array add fail.", __FUNCTION__, __LINE__));
    }

    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));
    return;
}


void chassis_get_sdcard_odata_id(json_object *o_drives_odata_id)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *obj_json = NULL;
    json_object *prop_json = NULL;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    guchar component_presence = 0;
    guchar component_type = 0;
    guchar component_num = 0;
    gchar sdcard_info[STORAGE_CTRL_NAME_LENGTH] = {0};
    gchar deivef_loc[STORAGE_CTRL_INFO_LENGTH] = {0};
    gchar deivef_dev[STORAGE_CTRL_INFO_LENGTH] = {0};

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_do_info_if_expr(VOS_ERR == ret, debug_log(DLOG_MASS, "%s:%d get board slot fail", __FUNCTION__, __LINE__));

    ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
    do_val_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s: get component list fail %d", __FUNCTION__, ret));

    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
        
        if ((VOS_OK == ret) && (COMPONENT_TYPE_SD_CARD == component_type)) {
            
            ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_PRESENCE,
                &component_presence);
            do_if_expr(((VOS_OK != ret) || (0 == component_presence)), continue);

            
            ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICENUM, &component_num);
            do_if_expr(((VOS_OK != ret) || (0 == component_num)), continue);

            
            (void)memset_s(deivef_loc, sizeof(deivef_loc), 0, sizeof(deivef_loc));
            (void)memset_s(deivef_dev, sizeof(deivef_dev), 0, sizeof(deivef_dev));

            
            (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_LOCATION, deivef_loc,
                sizeof(deivef_loc));
            (void)dal_clear_string_blank(deivef_loc, sizeof(deivef_loc));

            (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICE_NAME, deivef_dev,
                sizeof(deivef_dev));
            (void)dal_clear_string_blank(deivef_dev, sizeof(deivef_dev));
            do_if_expr(0 == strlen(deivef_loc) || 0 == strlen(deivef_dev), continue);

            
            obj_json = NULL;
            prop_json = NULL;
            obj_json = json_object_new_object();
            do_if_expr(NULL == obj_json, continue);
            (void)memset_s(sdcard_info, sizeof(sdcard_info), 0, sizeof(sdcard_info));

            ret = snprintf_s(sdcard_info, sizeof(sdcard_info), sizeof(sdcard_info) - 1, SYSTEMS_STORAGE_DRIVES_SDCARDS,
                slot, deivef_loc, deivef_dev);
            
            do_if_expr(0 >= ret, debug_log(DLOG_MASS, "%s %d:snprintf_s fail.", __FUNCTION__, __LINE__);
                json_object_put(obj_json); continue);
            
            prop_json = json_object_new_string((const gchar *)sdcard_info);
            do_if_expr(NULL == prop_json, json_object_put(obj_json); continue);

            (void)json_object_object_add(obj_json, RFPROP_ODATA_ID, prop_json);
            ret = json_object_array_add(o_drives_odata_id, obj_json);
            do_if_expr(VOS_OK != ret, json_object_put(obj_json));
        }
    }

    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));
    return;
}


LOCAL void _get_chassis_related_computer_systems(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    guchar component_type;
    gint32 ret;
    gchar               str_odata_id[MAX_STRBUF_LEN + 1] = {0};
    json_object *jso_computersystems = NULL;
    json_object *jso_system_odata_id = NULL;

    gchar class_name[MAX_NAME_SIZE] = {0};

    return_do_info_if_fail(NULL != o_result_jso, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    ret = dfl_get_class_name(obj_handle, class_name, MAX_NAME_SIZE);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: failed to call dfl_get_class_name", __FUNCTION__));

    if (0 != g_strcmp0(class_name, CLASS_COMPONENT)) {
        return;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s failed:cann't get property value for %s",
        __FUNCTION__, PROPERTY_COMPONENT_DEVICE_TYPE));

    
    if (COMPONENT_TYPE_MAINBOARD != component_type) {
        return;
    }

    ret = chassis_get_odata_id(RF_SYSTEMS_URI, str_odata_id, sizeof(str_odata_id));
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s,%d:   create uri fail. ", __FUNCTION__, __LINE__));

    jso_computersystems = json_object_new_array();
    return_do_info_if_fail(NULL != jso_computersystems,
        debug_log(DLOG_ERROR, "%s,%d:    memory alloc fail. ", __FUNCTION__, __LINE__));

    jso_system_odata_id = json_object_new_object();
    return_do_info_if_fail(NULL != jso_system_odata_id, json_object_put(jso_computersystems);
        debug_log(DLOG_ERROR, "%s,%d:    memory alloc fail. ", __FUNCTION__, __LINE__));

    json_object_object_add(jso_system_odata_id, RFPROP_ODATA_ID, json_object_new_string((const gchar *)str_odata_id));
    ret = json_object_array_add(jso_computersystems, jso_system_odata_id);
    return_do_info_if_expr(0 != ret, debug_log(DLOG_ERROR, "%s:json_object_array_add", __FUNCTION__);
        (void)json_object_put(jso_system_odata_id); (void)json_object_put(jso_computersystems));

    json_object_object_add(o_result_jso, REDFISH_CHASSIS_COMPUTERSYSTEMS, jso_computersystems);

    return;
}


LOCAL void _get_chassis_related_managers(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    guchar component_type;
    gint32 ret;
    gchar               str_odata_id[MAX_STRBUF_LEN + 1] = {0};
    json_object *jso_managedby = NULL;
    json_object *jso_managedy_odata_id = NULL;
    guchar board_type = 0;

    gchar class_name[MAX_NAME_SIZE] = {0};

    return_do_info_if_fail((NULL != o_result_jso), debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    ret = dfl_get_class_name(obj_handle, class_name, MAX_NAME_SIZE);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: failed to call dfl_get_class_name", __FUNCTION__));

    if (0 != g_strcmp0(class_name, CLASS_COMPONENT)) {
        return;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
    return_do_info_if_fail((NULL != o_result_jso), debug_log(DLOG_ERROR,
        "%s failed:cann't get property value for %s, ret is %d", __FUNCTION__, PROPERTY_COMPONENT_DEVICE_TYPE, ret));

    (void)dal_rf_get_board_type(&board_type);

    
    if (((COMPONENT_TYPE_MAINBOARD != component_type) && (COMPONENT_TYPE_IO_BOARD != component_type) &&
        (COMPONENT_TYPE_GPU_BOARD != component_type) && (COMPONENT_TYPE_BACKPLANE_BOARD != component_type)) ||
        ((COMPONENT_TYPE_BACKPLANE_BOARD == component_type) && (BOARD_BLADE == board_type))) {
        return;
    }

    ret = chassis_get_odata_id(RF_MANAGERS_URI, str_odata_id, sizeof(str_odata_id));
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "get manager odata id failed, ret is %d", ret));

    jso_managedy_odata_id = json_object_new_array();
    return_do_info_if_fail(NULL != jso_managedy_odata_id, debug_log(DLOG_ERROR, "alloc new json array failed"));

    jso_managedby = json_object_new_object();
    return_do_info_if_fail(NULL != jso_managedby, debug_log(DLOG_ERROR, "allco new json object failed");
        json_object_put(jso_managedy_odata_id));

    json_object_object_add(jso_managedby, RFPROP_ODATA_ID, json_object_new_string((const gchar *)str_odata_id));
    ret = json_object_array_add(jso_managedy_odata_id, jso_managedby);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "add item to json array failed, ret is %d", ret);
        (void)json_object_put(jso_managedby); (void)json_object_put(jso_managedy_odata_id));

    json_object_object_add(o_result_jso, REDFISH_CHASSIS_MANAGEDBY, jso_managedy_odata_id);

    return;
}


LOCAL void _get_chassis_related_drives(OBJ_HANDLE obj_handle, json_object* o_result_jso)
{
    gint32 ret;
    guchar x86_enable = 0;

    json_object* jso_drives = NULL;

    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_X86, &x86_enable);
    return_do_info_if_expr(ret != VOS_OK,
        debug_log(DLOG_ERROR, "%s: cann't get property value for property [%s] of class [%s], ret is %d",
            __FUNCTION__, PROTERY_PME_SERVICECONFIG_X86, CLASS_NAME_PME_SERVICECONFIG, ret));

    if (x86_enable == DISABLE || check_enclosure_component_type(obj_handle, FALSE) != VOS_OK) {
        return;
    }

    jso_drives = json_object_new_array();
    return_do_info_if_expr(jso_drives == NULL,
        (void)json_object_object_add(o_result_jso, RFPROP_CHASSIS_DRIVES, NULL);
        debug_log(DLOG_ERROR, "%s: alloc new json array failed", __FUNCTION__));

    get_chassis_related_drives_member(jso_drives);
    (void)json_object_object_add(o_result_jso, RFPROP_CHASSIS_DRIVES, jso_drives);
    return;
}


void get_chassis_related_drives_member(json_object* jso_drives)
{
    return_do_info_if_expr(jso_drives == NULL,
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    
    
    
    
    (void)chassis_get_drives_odata_id(CLASS_HDD_NAME, jso_drives);
    (void)chassis_get_drives_odata_id(CLASS_PCIEHDD_NAME, jso_drives);
    
    (void)chassis_get_sdcard_odata_id(jso_drives);

    
    

    return;
}

LOCAL gboolean check_pciedevices_device_type(OBJ_HANDLE obj_handle, guchar device_type, guchar board_type)
{
    if (device_type == COMPONENT_TYPE_MAINBOARD) {
        return TRUE;
    }
    if ((board_type == BOARD_MM)) {
        if ((device_type == COMPONENT_TYPE_IO_BOARD) || (device_type == COMPONENT_TYPE_GPU_BOARD)) {
            return TRUE;
        }
    }
    return FALSE;
}

/*****************************************************************************
 函 数 名  : rf_get_object_location_handle
 功能描述  :获取对象句柄的归属句柄，主板或载板
 输入参数  : OBJ_HANDLE obj_handle             部件句柄
 输出参数  : OBJ_HANDLE* location_handle       归属资源句柄
 返 回 值  : VOS_OK，获取成功
             VOS_ERR，获取失败

 修改历史      :
  1.日    期   : 2017年6月27日
    作    者   : zhongqiu 00356691
    修改内容   : 新生成函数

**************************************************************************** */
gint32 rf_get_object_location_handle(guint32 recursive_depth, OBJ_HANDLE obj_handle, OBJ_HANDLE *location_handle)
{
    const gchar *obj_name = NULL;
    GSList *obj_list = NULL;
    GSList *obj_list_iter = NULL;
    gint32 ret;
    OBJ_HANDLE mainboard_handle = 0;
    OBJ_HANDLE component_handle = 0;
    OBJ_HANDLE connector_handle = 0;
    OBJ_HANDLE anchor_handle = 0;
    guchar comp_device_type = 0;
    guchar board_type = 0;

    obj_name = dfl_get_object_name(obj_handle);

    return_val_do_info_if_fail((NULL != obj_name) && (NULL != location_handle), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    recursive_depth++;

    if (recursive_depth > MAX_RECURSIVE_DEPTH) {
        debug_log(DLOG_ERROR, "%s failed:max recursive depth exceeded, ret is %d", __FUNCTION__, recursive_depth);

        return VOS_ERR;
    }

    

    
    ret = dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE, COMPONENT_TYPE_MAINBOARD,
        &mainboard_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "get mainboard component failed, ret is %d", ret));

    ret = dal_get_object_list_position(obj_handle, CLASS_COMPONENT, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_OK, (*location_handle = mainboard_handle));

    
    ret = dal_rf_get_board_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, g_slist_free(obj_list); obj_list = NULL;
        debug_log(DLOG_ERROR, "%s:cann't get board type", __FUNCTION__));
    

    for (obj_list_iter = obj_list; NULL != obj_list_iter; obj_list_iter = obj_list_iter->next) {
        component_handle = (OBJ_HANDLE)obj_list_iter->data;

        ret = dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &comp_device_type);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "cann't get property value for %s , object name is %s",
            PROPERTY_COMPONENT_DEVICE_TYPE, dfl_get_object_name(component_handle)));

        
        if (check_pciedevices_device_type(obj_handle, comp_device_type, board_type)) {
            debug_log(DLOG_DEBUG, "successfully found location obj [%s]", dfl_get_object_name(component_handle));
            break;
        }
    }

    g_slist_free(obj_list);

    
    return_val_do_info_if_expr((NULL != obj_list_iter), VOS_OK, (*location_handle = component_handle));

    ret = dal_get_specific_object_position(obj_handle, CLASS_ANCHOR, &anchor_handle);
    goto_if_fail(VOS_OK == ret, quit);

    ret = dal_get_anchor_related_connector(anchor_handle, &connector_handle);
    goto_if_fail(VOS_OK == ret, quit);

    
    ret = rf_get_object_location_handle(recursive_depth, connector_handle, location_handle);
    return_val_if_expr(VOS_OK == ret, VOS_OK);

quit:

    
    *location_handle = mainboard_handle;

    return VOS_OK;
}


gint pcie_connector_compare_func(gconstpointer a, gconstpointer b)
{
    OBJ_HANDLE connector_handle;
    OBJ_HANDLE location_handle;
    OBJ_HANDLE location_handle_compare = 0;
    gint32 ret;
    gchar connector_type_buf[MAX_CONNECTOR_TYPE_LEN] = {0};

    return_val_do_info_if_fail((a != NULL) && (b != NULL), -1,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    connector_handle = *(const OBJ_HANDLE *)a;
    location_handle = *(const OBJ_HANDLE *)b;

    ret = dal_get_property_value_string(connector_handle, PROPERTY_CONNECTOR_TYPE, connector_type_buf,
        MAX_CONNECTOR_TYPE_LEN);
    return_val_if_fail((ret == VOS_OK) && ((g_ascii_strcasecmp(connector_type_buf, CONNECTOR_TYPE_PCIE) == 0) ||
        (g_ascii_strcasecmp(connector_type_buf, CONNECTOR_TYPE_PCIECONNECTOR) == 0) ||
        (g_ascii_strcasecmp(connector_type_buf, CONNECTOR_TYPE_OCPCARDCONNECTOR) == 0) ||
        (g_ascii_strcasecmp(connector_type_buf, CONNECTOR_TYPE_PANGEA_CARD) == 0) ||
        (g_ascii_strcasecmp(connector_type_buf, CONNECTOR_TYPE_PCIE_SDI) == 0)),

        VOS_ERR);

    ret = rf_get_object_location_handle(0, connector_handle, &location_handle_compare);
    return_val_do_info_if_fail(ret == VOS_OK, -1,
        debug_log(DLOG_ERROR, "cann't get location handle for %s", dfl_get_object_name(connector_handle)));

    return (location_handle_compare == location_handle) ? 0 : -1;
}


LOCAL void _get_chassis_related_pciedevices(json_object *o_result_jso)
{
    json_object *jso_pciedevice_odata_id = NULL;
    jso_pciedevice_odata_id = json_object_new_array();
    redfish_get_chassis_pciedevice_odata_id(jso_pciedevice_odata_id, CLASS_PCIECARD_NAME);
    redfish_get_chassis_pciedevice_odata_id(jso_pciedevice_odata_id, CLASS_OCP_CARD);
    json_object_object_add(o_result_jso, RFPROP_CHASSIS_PCIEDEVICES, jso_pciedevice_odata_id);

    return;
}


LOCAL void _get_chassis_contains_info(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    gint32 ret;
    guchar comp_device_type = 0;
    json_object *chassis_member_array_jso = NULL;
    guchar board_type = 0;
    gchar               chassis_uri[MAX_URI_LENGTH] = {0};
    gchar               chassis_id[MAX_MEM_ID_LEN] = {0};

    gchar class_name[MAX_NAME_SIZE] = {0};

    // 非Component对象直接返回
    ret = dfl_get_class_name(obj_handle, class_name, MAX_NAME_SIZE);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: failed to call dfl_get_class_name", __FUNCTION__));

    if (0 != g_strcmp0(class_name, CLASS_COMPONENT)) {
        return;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &comp_device_type);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "cann't get property value for [%s] of object [%s]",
        PROPERTY_COMPONENT_DEVICE_TYPE, dfl_get_object_name(obj_handle)));

    (void)dal_rf_get_board_type(&board_type);

    
    if (COMPONENT_TYPE_CHASSIS_BACKPLANE != comp_device_type) {
        return;
    }

    chassis_member_array_jso = json_object_new_array();
    return_if_fail(NULL != chassis_member_array_jso);

    json_object_object_add(o_result_jso, RFPROP_CONTAINS, chassis_member_array_jso);

    if (BOARD_BLADE == board_type) {
        (void)dal_rf_get_board_slot(chassis_id, MAX_MEM_ID_LEN);
        ret = snprintf_s(chassis_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT_CHASSIS, chassis_id);
        return_do_info_if_fail(ret > 0, debug_log(DLOG_ERROR, "format sttring failed, ret is %d", ret));

        RF_ADD_ODATA_ID_MEMBER(chassis_member_array_jso, chassis_uri);

        return;
    }

    gen_enclosure_contains_chassis_array(chassis_member_array_jso);

    return;
}


LOCAL void _get_chassis_containedby_info(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    gint32 ret;
    guchar comp_device_type = 0;
    gchar               enc_chassis_uri[MAX_URI_LENGTH] = {0};
    json_object *odata_id_jso = NULL;
    json_object *chassis_uri_jso = NULL;
    guchar board_type = 0;
    guint32 platform_id = 0;
    OBJ_HANDLE product_obj_handle = 0;
    gchar class_name[MAX_NAME_SIZE] = {0};

    (void)dal_rf_get_board_type(&board_type);

    (void)dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &product_obj_handle);
    (void)dal_get_property_value_uint32(product_obj_handle, PROPERTY_PRODUCT_ID, &platform_id);

    
    
    return_if_fail((BOARD_MM == board_type) || (BOARD_BLADE == board_type) || (BOARD_RM == board_type));

    if (BOARD_RM == board_type) {
        ret = dfl_get_class_name(obj_handle, class_name, MAX_NAME_SIZE);
        return_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: failed to call dfl_get_class_name", __FUNCTION__));
        if (0 == g_strcmp0(class_name, CLASS_UNIT_INFO)) {
            ret = snprintf_s(enc_chassis_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT_CHASSIS,
                RF_RACK_CHASSIS_PREFIX);
        } else {
            return;
        }
    } else {
        
        if (BOARD_BLADE == board_type) {
            return_if_fail(check_is_support_node_enclosure(platform_id, board_type));
        }
        

        
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &comp_device_type);
        return_if_fail(COMPONENT_TYPE_CHASSIS_BACKPLANE != comp_device_type);

        ret = snprintf_s(enc_chassis_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT_CHASSIS,
            RF_ENCLOSURE_CHASSIS_PREFIX);
    }

    return_do_info_if_fail(ret > 0,
        debug_log(DLOG_ERROR, "%s:format enclosure chassis uri failed, ret is %d", __FUNCTION__, ret));

    odata_id_jso = json_object_new_object();
    return_do_info_if_fail(NULL != odata_id_jso,
        debug_log(DLOG_ERROR, "%s:alloc new json object failed", __FUNCTION__));

    chassis_uri_jso = json_object_new_string((const gchar *)enc_chassis_uri);
    json_object_object_add(odata_id_jso, RFPROP_ODATA_ID, chassis_uri_jso);

    json_object_object_add(o_result_jso, RFPROP_CONTAINED_BY, odata_id_jso);

    return;
}


LOCAL gint32 get_chassis_links_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    *o_result_jso = (NULL == *o_result_jso) ? json_object_new_object() : json_object_get(*o_result_jso);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    _get_chassis_contains_info(i_paras->obj_handle, *o_result_jso);
    _get_chassis_containedby_info(i_paras->obj_handle, *o_result_jso);
    _get_chassis_related_computer_systems(i_paras->obj_handle, *o_result_jso);
    _get_chassis_related_managers(i_paras->obj_handle, *o_result_jso);
    _get_chassis_related_drives(i_paras->obj_handle, *o_result_jso);
    _get_chassis_related_pciedevices(*o_result_jso);

    return HTTP_OK;
}


LOCAL gint32 _get_mainboard_chassis_type(const gchar **chassis_type_str)
{
    gint32 ret;
    guchar out_val;

    return_val_do_info_if_fail(NULL != chassis_type_str, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    ret = dal_rf_get_board_type(&out_val);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "cann't get board type, ret is %d", ret));

    if (BOARD_RACK == out_val) {
        *chassis_type_str = RF_CHASSIS_TYPE_RACK;
    } else if (BOARD_BLADE == out_val) {
        *chassis_type_str = RF_CHASSIS_TYPE_BLADE;
    } else if (BOARD_MM == out_val) {
        *chassis_type_str = RF_CHASSIS_TYPE_MODULE;
    } else {
        *chassis_type_str = RF_CHASSIS_TYPE_OTHER;
    }

    return VOS_OK;
}



LOCAL gint32 get_chassis_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    const gchar *chassis_type_str = NULL;
    CHASSIS_COMPONENT_INFO chassis_component_info = { 0 };
    guchar component_id;

    guchar board_type = 0;
    gchar class_name[MAX_NAME_SIZE] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // U位不支持显示chassistype
    (void)dal_rf_get_board_type(&board_type);
    if (BOARD_RM == board_type) {
        ret = dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s: failed to call dfl_get_class_name", __FUNCTION__));

        if (0 == g_strcmp0(class_name, CLASS_UNIT_INFO)) {
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    
    
    ret = rf_get_chassis_component_info_by_id(i_paras->member_id, &chassis_component_info, &component_id);
    if (VOS_OK == ret) {
        chassis_type_str = chassis_component_info.chassis_type;
    } else {
        
        (void)_get_mainboard_chassis_type(&chassis_type_str);
    }

    
    do_val_if_expr((NULL != chassis_type_str), (*o_result_jso = json_object_new_string(chassis_type_str)));

    return (NULL != chassis_type_str) ? HTTP_OK : HTTP_INTERNAL_SERVER_ERROR;
    
}



LOCAL gint32 get_chassis_indicateled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guint8 out_val = 0;
    OBJ_HANDLE obj_handle = 0;
    guchar component_type = 0;

    gchar class_name[MAX_NAME_SIZE] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: failed to call dfl_get_class_name, ret is %d", __FUNCTION__, ret));

    if (0 != g_strcmp0(class_name, CLASS_COMPONENT)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
    return_val_if_fail((VOS_OK == ret && COMPONENT_TYPE_MAINBOARD == component_type), HTTP_INTERNAL_SERVER_ERROR);

    ret = dfl_get_object_handle(UID_LED, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "cann't get object handle for %s", UID_LED));

    
    ret = dal_get_property_value_byte(obj_handle, STATE, &out_val);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));

    if (out_val == RF_CHASSIS_LED_OFF_VALUE) {
        *o_result_jso = json_object_new_string(RF_LED_OFF);
    } else if (out_val == RF_CHASSIS_LED_LIT_VALUE) {
        *o_result_jso = json_object_new_string(RF_LED_LIT);
    } else {
        *o_result_jso = json_object_new_string(RF_LED_BLINKING);
    }

    return HTTP_OK;
}

LOCAL gint32 get_uid_led_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    if (dal_get_uid_location() == UID_LOCATION_MAINBOARD) {
        return get_chassis_indicateled(i_paras, o_message_jso, o_result_jso);
    } else if (dal_get_uid_location() == UID_MAINBOARD_AND_CHASSIS) {
        return get_health_and_uid_status(get_self_provider_service(i_paras, g_chassis_provider,
            G_N_ELEMENTS(g_chassis_provider)), i_paras, o_message_jso, o_result_jso);
    }
    return HTTP_OK;
}

LOCAL gint32 set_uid_led_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    if (dal_get_uid_location() == UID_LOCATION_MAINBOARD) {
        return set_chassis_indicateled(i_paras, o_message_jso, o_result_jso);
    } else if (dal_get_uid_location() == UID_MAINBOARD_AND_CHASSIS) {
        return set_health_and_uid_status(get_self_provider_service(i_paras, g_chassis_provider,
            G_N_ELEMENTS(g_chassis_provider)), i_paras, o_message_jso, o_result_jso);
    }
    
    (void)create_message_info(MSGID_PROP_UNKNOWN, RFPROP_CHASSIS_INDICATELED, o_message_jso,
        dal_json_object_get_str(i_paras->val_jso), RFPROP_CHASSIS_INDICATELED);
    return HTTP_BAD_REQUEST;
}

LOCAL gint32 get_health_led_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return get_health_and_uid_status(get_self_provider_service(i_paras, g_chassis_provider,
        G_N_ELEMENTS(g_chassis_provider)), i_paras, o_message_jso, o_result_jso);
}

LOCAL gint32 set_health_led_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return set_health_and_uid_status(get_self_provider_service(i_paras, g_chassis_provider,
        G_N_ELEMENTS(g_chassis_provider)), i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 get_chassis_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar slot_str[MAX_RSC_ID_LEN] = {0};

    
    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso) && (NULL != i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = rf_gen_chassis_component_id(i_paras->member_id, 0, slot_str, MAX_RSC_ID_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "gen chassis component id for %s failed, ret is %d", i_paras->member_id, ret));
    

    *o_result_jso = json_object_new_string((const gchar *)slot_str);
    
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);
    

    return HTTP_OK;
}

LOCAL gint32 get_chassis_status_enc(json_object **o_result_jso)
{
    gint32 ret;
    guchar health = 0;
    OBJ_HANDLE obj_handle = 0;

    if (dal_check_if_vsmm_supported()) {
        ret = dal_get_object_handle_nth(CLASS_WARNING, 0, &obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "get o_obj_handle Warning fail. ret = %d", ret);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        ret = dal_get_property_value_byte(obj_handle, PROPERTY_WARNING_HEALTH_STATUS, &health);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "get property_data of HealthStatus fail. ret = %d", ret);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        get_resource_status_property(&health, NULL, ENABLED_STRING, o_result_jso, TRUE);
        return HTTP_OK;
    }

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 get_chassis_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guchar component_status = 0;
    guchar rsc_health = 0;
    guchar rsc_presence;
    const gchar *resource_state = NULL;
    guchar power_state = 0;

    guchar board_type = 0;
    gchar class_name[MAX_NAME_SIZE] = {0};
    gint32 ret = 0;
    OBJ_HANDLE component_handle = 0;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    if (0 == g_ascii_strcasecmp(i_paras->member_id, RF_ENCLOSURE_CHASSIS_PREFIX)) {
        return get_chassis_status_enc(o_result_jso);
    }

    // 资产管理板显示Status属性,U位设备展示Status属性
    (void)dal_rf_get_board_type(&board_type);
    if (BOARD_RM == board_type) {
        ret = dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s: failed to call dfl_get_class_name", __FUNCTION__));

        // U位设备不支持显示status信息
        if (0 == g_strcmp0(class_name, CLASS_UNIT_INFO)) {
            return HTTP_INTERNAL_SERVER_ERROR;
        } else if (0 == g_strcmp0(class_name, CLASS_RACK_ASSET_MGMT)) {
            (void)dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE,
                COMPONENT_TYPE_MAINBOARD, &component_handle);
            (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_PRESENCE, &component_status);
            (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_POWERSTATE, &power_state);
            (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_HEALTH, &rsc_health);
        }
    } else {
        (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_COMPONENT_PRESENCE, &component_status);
        (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_COMPONENT_POWERSTATE, &power_state);
        (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_COMPONENT_HEALTH, &rsc_health);
    }

    rsc_presence = ((0 == i_paras->obj_handle) || (0 == component_status)) ? 0 : 1;
    rsc_health = (0 == rsc_presence) ? HEALTH_NORMAL : rsc_health;

    if (0 == rsc_presence) {
        resource_state = RF_STATE_ABSENT;
    } else if (0 == power_state) {
        
        resource_state = RF_STATE_DISABLED;
    } else {
        resource_state = RF_STATE_ENABLED;
    }

    (void)get_resource_status_property(&rsc_health, NULL, resource_state, o_result_jso, FALSE);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_location(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar component_type = 0;
    gchar               chassis_location[MAX_SERVER_LOCATION_LEN] = {0};
    json_object *location_jso = NULL;
    json_object *location_info_jso = NULL; 

    guchar board_type = 0;
    gchar  class_name[MAX_NAME_SIZE] = {0};

    

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    location_jso = (NULL == *o_result_jso) ? json_object_new_object() : json_object_get(*o_result_jso);

    (void)dal_rf_get_board_type(&board_type);
    ret = dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);
    if (BOARD_RM == board_type && VOS_OK == ret && 0 == g_strcmp0(class_name, CLASS_RACK_ASSET_MGMT)) {
        json_object *postal_adress_jso;
        json_object *placement_jso;
        postal_adress_jso = json_object_new_object();
        placement_jso = json_object_new_object();

        // 获取Country属性值
        rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_COUNTRY, RF_LOCATION_COUNTRY,
            postal_adress_jso);

        // 获取Province属性值
        rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_TERRITORY, RF_LOCATION_PROVINCE,
            postal_adress_jso);

        // 获取City属性值
        rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_CITY, RF_LOCATION_CITY,
            postal_adress_jso);

        // 获取Street属性值
        rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_STREET, RF_LOCATION_STREET,
            postal_adress_jso);

        // 获取HouseNumber属性值
        rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_HOUSE_NUM, RF_LOCATION_HOUSE_NUMBER,
            postal_adress_jso);

        // 获取Name属性值
        rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_NAME, RF_LOCATION_NAME,
            postal_adress_jso);

        // 获取PostalCode属性值
        rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_POST_CODE, RF_LOCATION_POSTAL_CODE,
            postal_adress_jso);

        // 获取Buliding属性值
        rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_BUILDING, RF_LOCATION_BUILDING,
            postal_adress_jso);

        // 获取Floor属性值
        rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_FLOOR, RF_LOCATION_FLOOR,
            postal_adress_jso);

        // 获取Room属性值
        rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_ROOM, RF_LOCATION_ROOM,
            postal_adress_jso);

        // 获取Location属性值
        rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_LOCATION_INFO, RF_LOCATION_LOCATION,
            postal_adress_jso);

        json_object_object_add(location_jso, RF_LOCATION_POSTAL_ADDRESS, postal_adress_jso);

        // 获取Row属性值
        rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_ROW, RF_LOCATION_ROW, placement_jso);

        // 获取Rack属性值
        rf_add_property_jso_string(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_RACK, RF_LOCATION_RACK, placement_jso);

        json_object_object_add(location_jso, RF_LOCATION_PLACEMENT, placement_jso);
    }
    
    else if (VOS_OK == ret && 0 == g_strcmp0(class_name, CLASS_COMPONENT)) {
        (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
        return_val_do_info_if_fail(COMPONENT_TYPE_CHASSIS_BACKPLANE == component_type, HTTP_INTERNAL_SERVER_ERROR,
            json_object_put(location_jso));

        ret = dal_get_server_location(chassis_location, MAX_SERVER_LOCATION_LEN);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, json_object_put(location_jso);
            debug_log(DLOG_ERROR, "%s: get server location failed, ret is %d", __FUNCTION__, ret));
        ret = rf_string_check((const gchar *)chassis_location);
        location_info_jso = (VOS_OK == ret) ? json_object_new_string((const gchar *)chassis_location) : NULL;

        json_object_object_add(location_jso, RF_LOCATION_INFO, location_info_jso);
    }
    
    else if (VOS_OK == ret && 0 == g_strcmp0(class_name, CLASS_UNIT_INFO)) {
        debug_log(DLOG_DEBUG, "%s: unsupported class name : %s", __FUNCTION__, class_name);
        do_info_if_true(NULL != location_jso, json_object_put(location_jso));
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    else {
        debug_log(DLOG_ERROR, "%s: unsupported class name : %s", __FUNCTION__, class_name);
    }

    *o_result_jso = location_jso;

    return HTTP_OK;
}


LOCAL gint32 set_chassis_rack_location(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret_code = HTTP_BAD_REQUEST;
    gint32 ret = 0;
    json_object *val_json = NULL;
    json_object *obj_json_mode = NULL;

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    
    return_val_do_info_if_fail(0 == g_strcmp0(CLASS_RACK_ASSET_MGMT, dfl_get_object_name(i_paras->obj_handle)),
        HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s: %s obj handle name is not support.", __FUNCTION__,
        dfl_get_object_name(i_paras->obj_handle));
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_LOCATION, o_message_jso, RFPROP_LOCATION));
    

    
    *o_message_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_message_jso, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: new array failed.", __FUNCTION__, __LINE__));

    
    if (json_object_object_get_ex(i_paras->val_jso, RF_LOCATION_POSTAL_ADDRESS, &val_json) == TRUE) {
        
        if (json_object_object_get_ex(val_json, PROPERTY_RACK_ASSET_MGMT_COUNTRY, &obj_json_mode) == TRUE) {
            ret = set_chassis_location_property(i_paras, o_message_jso, o_result_jso, obj_json_mode,
                PROPERTY_RACK_ASSET_MGMT_COUNTRY, METHOD_RACK_ASSET_MGMT_SETCOUNTRY);
            do_info_if_true(HTTP_OK == ret, (ret_code = HTTP_OK));
        }

        
        if (json_object_object_get_ex(val_json, PROPERTY_RACK_ASSET_MGMT_TERRITORY, &obj_json_mode) == TRUE) {
            ret = set_chassis_location_property(i_paras, o_message_jso, o_result_jso, obj_json_mode,
                PROPERTY_RACK_ASSET_MGMT_TERRITORY, METHOD_RACK_ASSET_MGMT_SETTERRITORY);
            do_info_if_true(HTTP_OK == ret, (ret_code = HTTP_OK));
        }

        
        if (json_object_object_get_ex(val_json, PROPERTY_RACK_ASSET_MGMT_CITY, &obj_json_mode) == TRUE) {
            ret = set_chassis_location_property(i_paras, o_message_jso, o_result_jso, obj_json_mode,
                PROPERTY_RACK_ASSET_MGMT_CITY, METHOD_RACK_ASSET_MGMT_SETCITY);
            do_info_if_true(HTTP_OK == ret, (ret_code = HTTP_OK));
        }

        
        if (json_object_object_get_ex(val_json, PROPERTY_RACK_ASSET_MGMT_STREET, &obj_json_mode) == TRUE) {
            ret = set_chassis_location_property(i_paras, o_message_jso, o_result_jso, obj_json_mode,
                PROPERTY_RACK_ASSET_MGMT_STREET, METHOD_RACK_ASSET_MGMT_SETSTREET);
            do_info_if_true(HTTP_OK == ret, (ret_code = HTTP_OK));
        }

        
        if (json_object_object_get_ex(val_json, PROPERTY_RACK_ASSET_MGMT_HOUSE_NUM, &obj_json_mode) == TRUE) {
            ret = set_chassis_location_property(i_paras, o_message_jso, o_result_jso, obj_json_mode,
                PROPERTY_RACK_ASSET_MGMT_HOUSE_NUM, METHOD_RACK_ASSET_MGMT_SETHOUSENUM);
            do_info_if_true(HTTP_OK == ret, (ret_code = HTTP_OK));
        }

        
        if (json_object_object_get_ex(val_json, PROPERTY_RACK_ASSET_MGMT_NAME, &obj_json_mode) == TRUE) {
            ret = set_chassis_location_property(i_paras, o_message_jso, o_result_jso, obj_json_mode,
                PROPERTY_RACK_ASSET_MGMT_NAME, METHOD_RACK_ASSET_MGMT_SETNAME);
            do_info_if_true(HTTP_OK == ret, (ret_code = HTTP_OK));
        }

        
        if (json_object_object_get_ex(val_json, PROPERTY_RACK_ASSET_MGMT_POST_CODE, &obj_json_mode) == TRUE) {
            ret = set_chassis_location_property(i_paras, o_message_jso, o_result_jso, obj_json_mode,
                PROPERTY_RACK_ASSET_MGMT_POST_CODE, METHOD_RACK_ASSET_MGMT_SETPOSTCODE);
            do_info_if_true(HTTP_OK == ret, (ret_code = HTTP_OK));
        }

        
        if (json_object_object_get_ex(val_json, PROPERTY_RACK_ASSET_MGMT_BUILDING, &obj_json_mode) == TRUE) {
            ret = set_chassis_location_property(i_paras, o_message_jso, o_result_jso, obj_json_mode,
                PROPERTY_RACK_ASSET_MGMT_BUILDING, METHOD_RACK_ASSET_MGMT_SETBUILDING);
            do_info_if_true(HTTP_OK == ret, (ret_code = HTTP_OK));
        }

        
        if (json_object_object_get_ex(val_json, PROPERTY_RACK_ASSET_MGMT_FLOOR, &obj_json_mode) == TRUE) {
            ret = set_chassis_location_property(i_paras, o_message_jso, o_result_jso, obj_json_mode,
                PROPERTY_RACK_ASSET_MGMT_FLOOR, METHOD_RACK_ASSET_MGMT_SETFLOOR);
            do_info_if_true(HTTP_OK == ret, (ret_code = HTTP_OK));
        }

        
        if (json_object_object_get_ex(val_json, PROPERTY_RACK_ASSET_MGMT_ROOM, &obj_json_mode) == TRUE) {
            ret = set_chassis_location_property(i_paras, o_message_jso, o_result_jso, obj_json_mode,
                PROPERTY_RACK_ASSET_MGMT_ROOM, METHOD_RACK_ASSET_MGMT_SETROOM);
            do_info_if_true(HTTP_OK == ret, (ret_code = HTTP_OK));
        }

        
        if (json_object_object_get_ex(val_json, RF_LOCATION_LOCATION, &obj_json_mode) == TRUE) {
            ret = set_chassis_location_info(i_paras, o_message_jso, obj_json_mode);
            do_info_if_true(HTTP_OK == ret, (ret_code = HTTP_OK));
        }
    }

    
    if (json_object_object_get_ex(i_paras->val_jso, RF_LOCATION_PLACEMENT, &val_json) == TRUE) {
        
        if (json_object_object_get_ex(val_json, PROPERTY_RACK_ASSET_MGMT_ROW, &obj_json_mode) == TRUE) {
            ret = set_chassis_location_property(i_paras, o_message_jso, o_result_jso, obj_json_mode,
                PROPERTY_RACK_ASSET_MGMT_ROW, METHOD_RACK_ASSET_MGMT_SETROW);
            do_info_if_true(HTTP_OK == ret, (ret_code = HTTP_OK));
        }

        
        if (json_object_object_get_ex(val_json, PROPERTY_RACK_ASSET_MGMT_RACK, &obj_json_mode) == TRUE) {
            ret = set_chassis_location_property(i_paras, o_message_jso, o_result_jso, obj_json_mode,
                PROPERTY_RACK_ASSET_MGMT_RACK, METHOD_RACK_ASSET_MGMT_SETRACK);
            do_info_if_true(HTTP_OK == ret, (ret_code = HTTP_OK));
        }
    }
    return ret_code;
}


LOCAL gint32 set_chassis_location_property(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *obj_json_mode, gchar *property_name, gchar *method_name)
{
#define MAX_RACK_PROPERTY_STRING_LEN 48
    gint32 ret;
    GSList *input_list = NULL;
    json_object *message_temp_jso = NULL;

    
    return_val_do_info_if_fail(NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    // 有效性检查
    if (json_object_get_string_len(obj_json_mode) > MAX_RACK_PROPERTY_STRING_LEN) {
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, property_name, &message_temp_jso,
            dal_json_object_get_str(obj_json_mode), property_name);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_BAD_REQUEST;
    }

    
    
    if (NULL == obj_json_mode) {
        input_list = g_slist_append(input_list, g_variant_new_string(""));
    } else {
        input_list = g_slist_append(input_list, g_variant_new_string(dal_json_object_get_str(obj_json_mode)));
    }
    

    
    ret =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->obj_handle,
        CLASS_RACK_ASSET_MGMT, method_name, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            ret = HTTP_OK;
            break;
        case RFERR_SUCCESS:
            ret = HTTP_OK;
            break;

            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret = RFERR_INSUFFICIENT_PRIVILEGE;
            break;

            
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_temp_jso, i_paras->uri);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret = HTTP_NOT_FOUND;
            break;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret = HTTP_INTERNAL_SERVER_ERROR;
            break;
    }

    return ret;
}


LOCAL gint32 set_chassis_location_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object *o_result_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    json_object *message_temp_jso = NULL;

    
    return_val_do_info_if_fail(NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    
    
    if (NULL == o_result_jso) {
        input_list = g_slist_append(input_list, g_variant_new_string(""));
    } else {
        input_list = g_slist_append(input_list, g_variant_new_string(dal_json_object_get_str(o_result_jso)));
    }
    

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_RACK_ASSET_MGMT, METHOD_RACK_ASSET_MGMT_SET_LOCATION_INFO, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret = RFERR_INSUFFICIENT_PRIVILEGE;
            break;

        case VOS_OK:
            ret = HTTP_OK;
            break;

        case RFERR_SUCCESS:
            ret = HTTP_OK;
            break;

            
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_temp_jso, i_paras->uri);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret = HTTP_NOT_FOUND;
            break;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret = HTTP_INTERNAL_SERVER_ERROR;
            break;
    }

    return ret;
}


LOCAL gint32 set_rack_dimensions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    CHASSIS_DIMENSIONS_S *rack_dimensions)
{
    gint32 ret;
    GSList *input_list = NULL;

    
    return_val_do_info_if_expr((NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras)) ||
        (NULL == rack_dimensions),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    
    input_list = g_slist_append(input_list, g_variant_new_uint32(rack_dimensions->depthMm));
    input_list = g_slist_append(input_list, g_variant_new_uint32(rack_dimensions->widthMm));
    input_list = g_slist_append(input_list, g_variant_new_uint32(rack_dimensions->heightMm));

    ret =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->obj_handle,
        CLASS_RACK_ASSET_MGMT, METHOD_SET_RACK_DIMENSIONS, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    do_info_if_true(VOS_OK != ret,
        debug_log(DLOG_ERROR, "%s: call %s with(%d,%d,%d) failed. ret=%d", __FUNCTION__, METHOD_SET_RACK_DIMENSIONS,
        rack_dimensions->depthMm, rack_dimensions->widthMm, rack_dimensions->heightMm, ret));

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

        case RFERR_SUCCESS:
            return HTTP_OK;

            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return RFERR_INSUFFICIENT_PRIVILEGE;

            
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_NOT_FOUND;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 get_chassis_depth_mm(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guint32 chassis_depth = 0;
    guchar board_type = 0;
    gchar           class_name[MAX_NAME_SIZE] = {0};

    return_val_do_info_if_expr((NULL == i_paras) || (NULL == o_message_jso) || (NULL == o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    (void)dal_rf_get_board_type(&board_type);
    (void)dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);

    if (BOARD_RM == board_type && 0 == g_strcmp0(class_name, CLASS_RACK_ASSET_MGMT)) {
        
        (void)dal_get_property_value_uint32(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_DEPTH_MM, &chassis_depth);
        *o_result_jso = json_object_new_int(chassis_depth);
        return HTTP_OK;
    } else {
        debug_log(DLOG_DEBUG, "%s: unsupported class name : %s", __FUNCTION__, class_name);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 get_chassis_height_mm(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guint32 chassis_height = 0;
    guchar board_type = 0;
    gchar           class_name[MAX_NAME_SIZE] = {0};

    return_val_do_info_if_expr((NULL == i_paras) || (NULL == o_message_jso) || (NULL == o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    (void)dal_rf_get_board_type(&board_type);
    (void)dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);

    if (BOARD_RM == board_type && 0 == g_strcmp0(class_name, CLASS_RACK_ASSET_MGMT)) {
        
        (void)dal_get_property_value_uint32(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_HEIGHT_MM, &chassis_height);
        *o_result_jso = json_object_new_int(chassis_height);
        return HTTP_OK;
    } else {
        debug_log(DLOG_DEBUG, "%s: unsupported class name : %s", __FUNCTION__, class_name);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 get_chassis_width_mm(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guint32 chassis_width = 0;
    guchar board_type = 0;
    gchar           class_name[MAX_NAME_SIZE] = {0};

    return_val_do_info_if_expr((NULL == i_paras) || (NULL == o_message_jso) || (NULL == o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    (void)dal_rf_get_board_type(&board_type);
    (void)dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);

    if (BOARD_RM == board_type && 0 == g_strcmp0(class_name, CLASS_RACK_ASSET_MGMT)) {
        
        (void)dal_get_property_value_uint32(i_paras->obj_handle, PROPERTY_RACK_ASSET_MGMT_WIDTH_MM, &chassis_width);
        *o_result_jso = json_object_new_int(chassis_width);
        return HTTP_OK;
    } else {
        debug_log(DLOG_DEBUG, "%s: unsupported class name : %s", __FUNCTION__, class_name);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 act_chassis_set_dimensions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar board_type = 0;
    gchar           class_name[MAX_NAME_SIZE] = {0};
    json_object *val_json = NULL;
    CHASSIS_DIMENSIONS_S chassis_dimensions;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_expr(0 == (i_paras->user_role_privilege & USERROLE_BASICSETTING), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_rf_get_board_type(&board_type);
    ret = dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);
    if (BOARD_RM == board_type && VOS_OK == ret && 0 == g_strcmp0(class_name, CLASS_RACK_ASSET_MGMT)) {
        
        (void)json_object_object_get_ex(i_paras->val_jso, PROPERTY_RACK_ASSET_MGMT_DEPTH_MM, &val_json);
        chassis_dimensions.depthMm = json_object_get_int(val_json);

        val_json = NULL;
        (void)json_object_object_get_ex(i_paras->val_jso, PROPERTY_RACK_ASSET_MGMT_WIDTH_MM, &val_json);
        chassis_dimensions.widthMm = json_object_get_int(val_json);

        val_json = NULL;
        (void)json_object_object_get_ex(i_paras->val_jso, PROPERTY_RACK_ASSET_MGMT_HEIGHT_MM, &val_json);
        chassis_dimensions.heightMm = json_object_get_int(val_json);

        
        return set_rack_dimensions(i_paras, o_message_jso, &chassis_dimensions);
    } else {
        debug_log(DLOG_ERROR, "%s: unsupported class name : %s", __FUNCTION__, class_name);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 set_chassis_indicateled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    const char *set_led_state = NULL;
    guchar component_type = 0;

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_CHASSIS_INDICATELED, o_message_jso,
        RFPROP_CHASSIS_INDICATELED));

    
    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
    return_val_do_info_if_fail((VOS_OK == ret && COMPONENT_TYPE_MAINBOARD == component_type), HTTP_NOT_IMPLEMENTED,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_CHASSIS_INDICATELED, o_message_jso,
        RFPROP_CHASSIS_INDICATELED));

    
    ret = dfl_get_object_handle(UID_LED, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_IMPLEMENTED,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_CHASSIS_INDICATELED, o_message_jso,
        RFPROP_CHASSIS_INDICATELED));

    
    set_led_state = dal_json_object_get_str(i_paras->val_jso);
    if (0 == g_strcmp0(RF_LED_LIT, set_led_state)) {
        input_list = g_slist_append(input_list, g_variant_new_byte(TURN_ON_IDENTIFY_LED));
        input_list = g_slist_append(input_list, g_variant_new_byte(RF_CHASSIS_LED_LOCK_STATE));
    } else if (0 == g_strcmp0(RF_LED_OFF, set_led_state)) {
        input_list = g_slist_append(input_list, g_variant_new_byte(TURN_OFF_IDENTIFY_LED));
        input_list = g_slist_append(input_list, g_variant_new_byte(RF_CHASSIS_LED_LOCK_STATE));
    } else if (0 == g_strcmp0(RF_LED_BLINKING, set_led_state)) {
        input_list = g_slist_append(input_list, g_variant_new_byte(TURN_ON_IDENTIFY_LED));
        input_list = g_slist_append(input_list, g_variant_new_byte(RF_CHASSIS_LED_BLINKING_TIME));
    } else {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_CHASSIS_INDICATELED, o_message_jso, set_led_state,
            RFPROP_CHASSIS_INDICATELED);
        return HTTP_BAD_REQUEST;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        LED_CLASS_NAME, METHOD_UIDLED_STATE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

            
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_NOT_FOUND;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


gint32 chassis_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean ret_result;

    
    ret_result = rf_check_chassis_uri_valid_allow_node_enc(i_paras->uri, &i_paras->obj_handle);
    
    if (!ret_result || i_paras->obj_handle == 0) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_chassis_provider;
    *count = sizeof(g_chassis_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gint32 chassis_etag_del_property(json_object *object)
{
    SPECIAL_PROP_S value[] = {
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_CHASSIS_INLET_TEM, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_CHASSIS_ENERGY_EFFICIENCY, NULL, NULL}},
        {0, {NULL, NULL, NULL, NULL, NULL}}
    };
    return rsc_del_none_etag_affected_property(object, value, G_N_ELEMENTS(value));
}


LOCAL void chassis_rsc_filter_unavailable_prop(json_object* o_rsc_jso)
{
    gint32 ret;
    guchar board_type = BOARD_OTHER;
    json_object* json_links = NULL;

    if (o_rsc_jso == NULL) {
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__);
        return;
    }
    ret = dal_rf_get_board_type(&board_type);
    if (ret != VOS_OK || board_type != BOARD_SWITCH || dal_check_if_vsmm_supported()) {
        return;
    }

    // 交换板不支持PCIeDevices和Drives
    json_object_object_del(o_rsc_jso, RFPROP_CHASSIS_PCIEDEVICES);
    json_object_object_del(o_rsc_jso, RFPROP_CHASSIS_DRIVES);
    if (json_object_object_get_ex(o_rsc_jso, RF_CHASSIS_LINKS, &json_links) == TRUE) {
        json_object_object_del(json_links, RFPROP_CHASSIS_PCIEDEVICES);
        json_object_object_del(json_links, RFPROP_CHASSIS_DRIVES);
    }

    return;
}

/*****************************************************************************
 函 数 名  : get_chassis_rsc
 功能描述  : 获取Chassis整个资源信息
 输入参数  : PROVIDER_PARAS_S*     i_paras
             json_object*          o_rsc_jso
             json_object*          o_err_array_jso
 输出参数  : 无

 修改历史      :
  1.日    期   : 2017年6月29日
    作    者   : zhongqiu 00356691
    修改内容   : 新生成函数

**************************************************************************** */
gint32 get_chassis_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_rsc_jso) && (NULL != o_err_array_jso), RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    (void)provider_get_prop_values(i_paras, &o_rsc_jso, o_err_array_jso, g_chassis_provider,
        G_N_ELEMENTS(g_chassis_provider));

    (void)rf_update_rsc_component_info(i_paras->obj_handle, o_rsc_jso);

    chassis_rsc_filter_unavailable_prop(o_rsc_jso);

    return VOS_OK;
}


gint32 get_rack_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    OBJ_HANDLE component = 0;
    gint32 ret;
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_rsc_jso) && (NULL != o_err_array_jso), RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    (void)provider_get_prop_values(i_paras, &o_rsc_jso, o_err_array_jso, g_chassis_provider,
        G_N_ELEMENTS(g_chassis_provider));
    ret = dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE, COMPONENT_TYPE_MAINBOARD,
        &component);
    if (ret != VOS_OK) {
        return VOS_ERR;
    }
    (void)rf_update_rsc_component_info(component, o_rsc_jso);

    return VOS_OK;
}


LOCAL gint32 get_chassis_set_dimensions_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar       string_value[ARRAY_LENTH] = {0};
    gchar       slot_str[MAX_RSC_ID_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_expr(0 == i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = dal_rf_get_board_slot(slot_str, sizeof(slot_str));
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: failed to get slot str", __FUNCTION__));

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
        RF_CHASSIS_OEM_ACTION_SET_DIMENSIONS_ACTIONINFO, slot_str);
    return_val_do_info_if_expr(ret <= 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: failed to format action info", __FUNCTION__));

    *o_result_jso = json_object_new_string((const gchar *)string_value);
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: failed to generate action info resource", __FUNCTION__));

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_chassis_set_dimensions_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_set_dimensions_actioninfo_odataid, NULL, NULL, ETAG_FLAG_ENABLE}
};

gint32 chassis_set_dimensions_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gboolean ret;

    
    return_val_if_expr((NULL == i_paras) || (NULL == prop_provider) || (NULL == count), HTTP_INTERNAL_SERVER_ERROR);

    ret = redfish_check_chassis_rack_uri_valid(i_paras->uri, &i_paras->obj_handle);
    if (TRUE != ret) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_chassis_set_dimensions_actioninfo_provider;
    *count = sizeof(g_chassis_set_dimensions_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}


LOCAL gint32 get_chassis_import_config_file_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar string_value[ARRAY_LENTH] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gint32 ret;
    OBJ_HANDLE component_handle;
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 获取 @odata.id
    (void)dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE, MAINBOARD_DEVICETYPE,
        &component_handle);

    ret = rf_gen_chassis_component_id(NULL, component_handle, slot_str, MAX_RSC_ID_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
        RF_CHASSIS_OEM_ACTION_IMPORT_CONFIG_FILE_ACTIONINFO, slot_str);
    return_val_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string((const gchar *)string_value);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_chassis_import_config_file_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, get_chassis_import_config_file_actioninfo_odataid, NULL, NULL, ETAG_FLAG_ENABLE}
};

gint32 chassis_import_config_file_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean ret;

    ret = redfish_check_chassis_rack_uri_valid(i_paras->uri, &i_paras->obj_handle);
    if (!ret) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_chassis_import_config_file_actioninfo_provider;
    *count = sizeof(g_chassis_import_config_file_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


LOCAL gint32 get_chassis_export_template_file_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar string_value[ARRAY_LENTH] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gint32 ret;
    OBJ_HANDLE component_handle;
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 获取 @odata.id
    (void)dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE, MAINBOARD_DEVICETYPE,
        &component_handle);

    ret = rf_gen_chassis_component_id(NULL, component_handle, slot_str, MAX_RSC_ID_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
        RF_CHASSIS_OEM_ACTION_EXPORT_TEMPLATE_FILE_ACTIONINFO, slot_str);
    return_val_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string((const gchar *)string_value);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_chassis_export_template_file_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, get_chassis_export_template_file_actioninfo_odataid, NULL, NULL, ETAG_FLAG_ENABLE}
};

gint32 chassis_export_template_file_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean ret;

    ret = redfish_check_chassis_rack_uri_valid(i_paras->uri, &i_paras->obj_handle);
    if (!ret) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_chassis_export_template_file_actioninfo_provider;
    *count = sizeof(g_chassis_export_template_file_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


LOCAL gint32 get_chassis_control_led_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar string_value[ARRAY_LENTH] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gint32 ret;
    gchar class_name[MAX_NAME_SIZE] = {0};
    guint8 unum = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 获取 @odata.id
    ret = dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);
    if (0 == g_strcmp0(class_name, CLASS_UNIT_INFO)) {
        ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_UNIT_INFO_U_NUM, &unum);
        return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

        ret = snprintf_s(slot_str, sizeof(slot_str), sizeof(slot_str) - 1, "%s%u", RF_UNIT_CHASSIS_PREFIX, unum);
        return_val_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR);
    } else {
        
        ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot_str, MAX_RSC_ID_LEN);
        return_val_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR);
        
    }

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
        RF_CHASSIS_OEM_ACTION_CONTROL_LED_ACTIONINFO, slot_str);
    return_val_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string((const gchar *)string_value);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_chassis_control_led_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, get_chassis_control_led_actioninfo_odataid, NULL, NULL, ETAG_FLAG_ENABLE}
};


gint32 chassis_control_led_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gboolean ret;

    ret = redfish_check_chassis_uri_valid(i_paras->uri, &i_paras->obj_handle);
    if (ret == TRUE && i_paras->obj_handle != OBJ_HANDLE_COMMON) {
        
        if (check_enclosure_component_type(i_paras->obj_handle, FALSE) != RET_OK) {
            return HTTP_NOT_FOUND;
        }
    } else {
        (void)redfish_check_chassis_unit_uri_valid(i_paras->uri, &i_paras->obj_handle);
        if (i_paras->obj_handle == OBJ_HANDLE_COMMON) {
            return HTTP_NOT_FOUND;
        }
    }

    *prop_provider = g_chassis_control_led_actioninfo_provider;
    *count = sizeof(g_chassis_control_led_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


LOCAL gint32 get_chassis_set_unit_rfid_info_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar string_value[ARRAY_LENTH] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gint32 ret;
    gchar class_name[MAX_NAME_SIZE] = {0};
    guint8 unum = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 获取 @odata.id
    ret = dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);
    if (ret != VOS_OK || g_strcmp0(class_name, CLASS_UNIT_INFO) != 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_UNIT_INFO_U_NUM, &unum);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    ret = snprintf_s(slot_str, sizeof(slot_str), sizeof(slot_str) - 1, "%s%u", RF_UNIT_CHASSIS_PREFIX, unum);
    return_val_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR);

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
        RF_CHASSIS_OEM_ACTION_SET_UNIT_RFID_INFO_ACTIONINFO, slot_str);
    return_val_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string((const gchar *)string_value);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_chassis_set_unit_rfid_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, get_chassis_set_unit_rfid_info_actioninfo_odataid, NULL, NULL, ETAG_FLAG_ENABLE}
};


gint32 chassis_set_unit_rfid_info_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean ret;
    guint8 detected = TAG_NOT_DETECTED;

    ret = redfish_check_chassis_unit_uri_valid(i_paras->uri, &i_paras->obj_handle);
    if (!ret || 0 == i_paras->obj_handle) {
        return HTTP_NOT_FOUND;
    }

    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_UNIT_INFO_DETECTED, &detected);
    if (detected == TAG_NOT_DETECTED) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_chassis_set_unit_rfid_actioninfo_provider;
    *count = sizeof(g_chassis_set_unit_rfid_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}

LOCAL gint32 get_chassis_set_device_installed_status_actioninfo_odataid(PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    gchar string_value[ARRAY_LENTH] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gint32 ret;
    gchar class_name[MAX_NAME_SIZE] = {0};
    guint8 unum = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 获取 @odata.id
    ret = dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);
    if (0 != g_strcmp0(class_name, CLASS_UNIT_INFO)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_UNIT_INFO_U_NUM, &unum);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    ret = snprintf_s(slot_str, sizeof(slot_str), sizeof(slot_str) - 1, "%s%u", RF_UNIT_CHASSIS_PREFIX, unum);
    return_val_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR);

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
        RF_CHASSIS_OEM_ACTION_SET_DEVICE_INSTALLED_STATUS_ACTIONINFO, slot_str);
    return_val_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string((const gchar *)string_value);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_chassis_set_device_installed_status_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_ALLOW, get_chassis_set_device_installed_status_actioninfo_odataid, NULL, NULL, ETAG_FLAG_ENABLE}
};

gint32 chassis_set_device_installed_status_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gboolean ret;

    ret = redfish_check_chassis_unit_uri_valid(i_paras->uri, &i_paras->obj_handle);
    if (!ret || 0 == i_paras->obj_handle) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_chassis_set_device_installed_status_actioninfo_provider;
    *count = sizeof(g_chassis_set_device_installed_status_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


LOCAL gint32 _find_chassis_unit_handle_with_id(const gchar *chassis_id, OBJ_HANDLE *unit_handle)
{
    gint32 ret;
    const gchar *pos;
    guchar unit_num = 0;

    pos = g_str_case_rstr(chassis_id, RF_UNIT_CHASSIS_PREFIX);
    return_val_do_info_if_expr(NULL == pos, VOS_ERR, debug_log(DLOG_DEBUG, "%s: pos is NULL", __FUNCTION__));

    pos += strlen(RF_UNIT_CHASSIS_PREFIX);
    ret = vos_str2int(pos, 10, (void *)&unit_num, NUM_TPYE_UCHAR);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s: cann't get component index with %s, ret is %d", __FUNCTION__, pos, ret));

    ret = dal_get_specific_object_byte(CLASS_UNIT_INFO, PROPERTY_UNIT_INFO_U_NUM, unit_num, unit_handle);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s: cann't get  unit obj with unit num(%d).ret=%d", __FUNCTION__, unit_num, ret));

    return VOS_OK;
}


LOCAL gboolean redfish_check_chassis_unit_uri_valid(gchar *uri_string, OBJ_HANDLE *unit_handle)
{
    gint32 ret;
    gchar *chassis_id = NULL;

    return_val_do_info_if_fail((NULL != uri_string) && (NULL != unit_handle), FALSE,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    ret = rf_get_regex_match_result_nth(URI_PATTERN_CHASSIS_RELATED_RSC, uri_string, 1, &chassis_id);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_ERROR, "match %s with %s failed", uri_string, URI_PATTERN_CHASSIS_RELATED_RSC));

    ret = _find_chassis_unit_handle_with_id(chassis_id, unit_handle);
    g_free(chassis_id);

    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_ERROR, "%s: cann't get component handle for input chassis uri", __FUNCTION__));

    return TRUE;
}


LOCAL gint32 add_unit_info_status(OBJ_HANDLE obj_handle, json_object *rsc_jso)
{
    gint32 ret;
    guint8 detected = TAG_NOT_FOUND;
    guint32 warn_status;
    json_object *status_jso = NULL;

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_UNIT_INFO_DETECTED, &detected);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "Get unit info Detected failed. ret=%d", ret));

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_UNIT_INFO_WARN_STATUS, &warn_status);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "Get unit info WarnStatus failed. ret=%d", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    status_jso = json_object_new_object();
    if (detected == TAG_NOT_DETECTED) {
        json_object_object_add(status_jso, RFPROP_CHASSIS_STATUS_STATE, json_object_new_string(ABSENT_STRING));
    } else {
        json_object_object_add(status_jso, RFPROP_CHASSIS_STATUS_STATE, json_object_new_string(ENABLED_STRING));
    }

    if (warn_status == 0) {
        json_object_object_add(status_jso, RFPROP_CHASSIS_STATUS_HEALTH, json_object_new_string(OK_STRING));
    } else {
        json_object_object_add(status_jso, RFPROP_CHASSIS_STATUS_HEALTH, json_object_new_string(WARNING_STRING));
    }

    json_object_object_add(rsc_jso, RFPROP_CHASSIS_STATUS, status_jso);

    return VOS_OK;
}


LOCAL void rf_update_rsc_unit_info(OBJ_HANDLE obj_handle, json_object *rsc_jso)
{
    get_chassis_weight(obj_handle, rsc_jso);
    rf_add_property_jso_string(obj_handle, PROPERTY_UNIT_INFO_MANUFACTURER, RFPROP_MANUFACTURE, rsc_jso);
    rf_add_property_jso_string(obj_handle, PROPERTY_UNIT_INFO_SER_NUM, RFPROP_SERIAL_NUMBER, rsc_jso);
    rf_add_property_jso_string(obj_handle, PROPERTY_UNIT_INFO_PART_NUM, RFPROP_PART_NUMBER, rsc_jso);
    rf_add_property_jso_string(obj_handle, PROPERTY_UNIT_INFO_DEV_MODEL, RFPROP_MODEL, rsc_jso);
    rf_add_property_jso_string(obj_handle, PROPERTY_UNIT_INFO_INDICATOR_LED, RFPROP_UNITDEVICES_INDICATOR_LED, rsc_jso);

    
    add_unit_info_status(obj_handle, rsc_jso);
}


gint32 chassis_provider_unit_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean ret_result;

    ret_result = redfish_check_chassis_unit_uri_valid(i_paras->uri, &i_paras->obj_handle);
    if (!ret_result) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_chassis_provider;
    *count = sizeof(g_chassis_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gint32 get_chassis_unit_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_rsc_jso) && (NULL != o_err_array_jso), RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    (void)provider_get_prop_values(i_paras, &o_rsc_jso, o_err_array_jso, g_chassis_provider,
        G_N_ELEMENTS(g_chassis_provider));

    (void)rf_update_rsc_unit_info(i_paras->obj_handle, o_rsc_jso);

    return VOS_OK;
}


gint32 chassis_location_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean ret_result;

    ret_result = redfish_check_chassis_rack_uri_valid(i_paras->uri, &i_paras->obj_handle);
    if (!ret_result) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_chassis_provider;
    *count = sizeof(g_chassis_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gboolean is_node_enc_rsc(OBJ_HANDLE component_handle, guchar *board_type, guint32 *product_id, guchar *component_type)
{
    gint32 ret = 0;
    OBJ_HANDLE product_handle = 0;
    guchar tmp_board_type = 0;
    guint32 tmp_pdt_id = 0;
    guchar tmp_comp_type = 0;

    
    if (product_id == NULL) {
        product_id = &tmp_pdt_id;
        ret = dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &product_handle);
        return_val_do_info_if_fail(VOS_OK == ret, FALSE,
            debug_log(DLOG_ERROR, "%s,%d: get object handle for class [%s] failed, ret is %d", __FUNCTION__, __LINE__,
            CLASS_NAME_PRODUCT, ret));

        (void)dal_get_property_value_uint32(product_handle, PROPERTY_PRODUCT_ID, product_id);
    }

    
    if (board_type == NULL) {
        board_type = &tmp_board_type;
        (void)dal_rf_get_board_type(board_type);
    }

    
    if (component_type == NULL) {
        component_type = &tmp_comp_type;
        (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, component_type);
    }

    if ((*product_id == PRODUCT_ID_ATLAS || *product_id == PRODUCT_ID_XSERIAL_SERVER) && (*board_type == BOARD_BLADE) &&
        (*component_type == COMPONENT_TYPE_CHASSIS_BACKPLANE)) {
        return TRUE;
    }

    return FALSE;
}

const gchar *get_chassis_pciedevices_cardtype(OBJ_HANDLE pcie_handle)
{
    gchar class_name[MAX_NAME_SIZE] = {0};
    gint32 ret;

    ret = dfl_get_class_name(pcie_handle, class_name, MAX_NAME_SIZE);
    if ((ret == VOS_OK) && (g_ascii_strcasecmp(class_name, CLASS_OCP_CARD) == 0)) {
        return RF_OCPCARD_PREFIX;
    }
    return RF_PCIECARD_PREFIX;
}


gint32 get_system_overview_chassis(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    json_object *obj_jso = NULL;
    json_object *obj_property = NULL;
    gchar   pslot[PSLOT_MAX_LEN] = {0};

    
    return_val_do_info_if_expr(provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:   json_object_new_array fail. ", __FUNCTION__, __LINE__));

    
    obj_jso = json_object_new_object();
    return_val_do_info_if_fail(obj_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:   json_object_new_object fail. ", __FUNCTION__, __LINE__));

    
    ret = get_common_id(i_paras, o_message_jso, &obj_jso);
    do_if_expr(ret != HTTP_OK,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of get_common_id.", __FUNCTION__, __LINE__));

    
    ret = redfish_get_board_slot(pslot, sizeof(pslot));
    if (ret == VOS_OK) {
        ret = find_chassis_component_handle_with_id((const gchar *)pslot, &(i_paras->obj_handle));
        do_if_expr(ret == VOS_OK, get_chassis_indicateled(i_paras, o_message_jso, &obj_property);
            json_object_object_add(obj_jso, RFPROP_CHASSIS_INDICATELED, obj_property));
    }

    
    get_system_overview_inlet_temp_info(obj_jso);

    
    obj_property = NULL;
    get_system_overview_present_fan_num(&obj_property);
    json_object_object_add(obj_jso, RFPROP_POWER_PRESENT_FAN_NUM, obj_property);

    
    obj_property = NULL;
    get_system_overview_power(i_paras, o_message_jso, &obj_property);
    json_object_object_add(obj_jso, RFPROP_POWER, obj_property);

    
    obj_property = NULL;
    get_system_overview_battery(&obj_property);
    json_object_object_add(obj_jso, RFPROP_BATTERY, obj_property);

    
    obj_property = NULL;
    get_chassis_oem_drive_summary(&obj_property);
    json_object_object_add(obj_jso, RFPROP_CHASSIS_DRIVE_SUMMARY, obj_property);

    
    obj_property = NULL;
    get_chassis_oem_networkadapters_summary(0, &obj_property);
    json_object_object_add(obj_jso, RFPROP_CHASSIS_NETWORK_ADAPTERS_SUMMARY, obj_property);

    
    obj_property = NULL;
    get_chassis_oem_powersupply_summary(&obj_property);
    json_object_object_add(obj_jso, RFPROP_CHASSIS_POWERSUPPLY_SUMMARY, obj_property);

    
    obj_property = NULL;
    get_chassis_oem_battery_summary(&obj_property);
    json_object_object_add(obj_jso, RFPROP_CHASSIS_BATTERY_SUMMARY, obj_property);

    
    get_system_overview_fansummary(&obj_jso);

    
    obj_property = NULL;
    get_chassis_device_max_num(0, &obj_property);
    json_object_object_add(obj_jso, RFPROP_CHASSIS_MAX_NUM, obj_property);

    (void)json_object_array_add(*o_result_jso, obj_jso);

    return HTTP_OK;
}
