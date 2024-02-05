

#include "elabel/base.h"
#include "pme_app/dalib/dal_obj_priv_data.h"

#define FRU_EXTENSON_LABEL_MAX_LEN 640

#define SET_EXTENSION_ELABEL_PROPERTY(handle, property_name, string_buf)                          \
    SET_ELABEL_PROPERTY_BY_LEN(handle, property_name, FRU_EXTENSON_LABEL_MAX_LEN + 1, string_buf, \
        FRU_EXTENSON_LABEL_MAX_LEN)

#define COPY_ELABEL_INTERNAL_TO_FRU(fru_label_exist, fru_internal_label, elabel_internal)                           \
    do {                                                                                                            \
        errno_t strncpy_fun_ret = EOK;                                                                              \
        if (0 != (elabel_internal).len) {                                                                           \
            if (NULL == (fru_internal_label).data) {                                                                \
                (fru_internal_label).data = (guint8 *)g_malloc0(FRU_INTERNAL_AREA_LEN);                             \
                if (NULL == (fru_internal_label).data) {                                                            \
                    break;                                                                                          \
                }                                                                                                   \
            }                                                                                                       \
            (void)memset_s((fru_internal_label).data, FRU_MAX_ELABEL_LEN, 0, FRU_MAX_ELABEL_LEN);                   \
            (fru_internal_label).len = (elabel_internal).len;                                                       \
            strncpy_fun_ret = memmove_s((fru_internal_label).data, FRU_INTERNAL_AREA_LEN, (elabel_internal).data,   \
                MIN(FRU_INTERNAL_AREA_LEN - 4, (elabel_internal).len));                                             \
            if (strncpy_fun_ret != EOK) {                                                                           \
                debug_log(DLOG_ERROR, "%s: memmove_s failed, strncpy_fun_ret = %d", __FUNCTION__, strncpy_fun_ret); \
            }                                                                                                       \
            (fru_label_exist) = TRUE;                                                                               \
        }                                                                                                           \
    } while (0)

#define SET_ELABEL_PROPERTY_BY_LEN(handle, property_name, property_buf_len, string_buf, len)               \
    do {                                                                                                   \
        errno_t strncpy_fun_ret = EOK;                                                                     \
        gint32 ret = 0;                                                                                    \
        gchar *property_buf = NULL;                                                                        \
        GVariant *property_val = NULL;                                                                     \
                                                 \
        property_buf = (gchar *)g_malloc0(property_buf_len);                                               \
        if (NULL == property_buf) {                                                                        \
            debug_log(DLOG_ERROR, "%s: call dfl_set_property_value(%s, %s) failed, error = %d!(line:%d).", \
                dfl_get_object_name(handle), (property_name), (string_buf), ret, __LINE__);                \
            break;                                                                                         \
        }                                                                                                  \
        strncpy_fun_ret = strncpy_s(property_buf, (property_buf_len), (const gchar *)(string_buf), (len)); \
        if (strncpy_fun_ret != EOK) {                                                                      \
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, strncpy_fun_ret);        \
        }                                                                                                  \
        property_val = g_variant_new_string(property_buf);                                                 \
        ret = dfl_set_property_value(handle, (property_name), property_val, DF_NONE);                      \
        g_free(property_buf);                                                                              \
        g_variant_unref(property_val);                                                                     \
        if (DFL_OK != ret) {                                                                               \
            debug_log(DLOG_ERROR, "%s: call dfl_set_property_value(%s, %s) failed, error = %d!(line:%d).", \
                dfl_get_object_name(handle), (property_name), (string_buf), ret, __LINE__);                \
        }                                                                                                  \
    } while (0)

LOCAL gint32 __get_fru_area_internaluse(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, guint32 *max_length);
LOCAL gint32 __get_fru_area_chassisinfo(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, guint32 *max_length);
LOCAL gint32 __get_fru_area_boardinfo(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, guint32 *max_length);
LOCAL gint32 __get_fru_area_productinfo(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, guint32 *max_length);
LOCAL gint32 __get_fru_area_extendelable(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, guint32 *max_length);
LOCAL gint32 __get_fru_area_systeminfo(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, guint32 *max_length);
LOCAL gint32 __get_fru_internal_extend_area(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, guint32 *max_length);
LOCAL gint32 __get_chassis_extend_area(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, guint32 *max_length);
LOCAL gint32 __get_fru_product_extend_area(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, guint32 *max_length);
LOCAL void __lookup_chassis_type(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);
LOCAL void __lookup_chassis_part_number(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);
LOCAL void __lookup_chassis_serial_number(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);
LOCAL void __lookup_chassis_extra(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);
LOCAL gint32 __lookup_board_mfgdata(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);
LOCAL gint32 __lookup_board_manufacturer(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);
LOCAL gint32 __lookup_board_product_name(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);
LOCAL gint32 __lookup_board_serial_number(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);
LOCAL gint32 __lookup_board_part_number(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);
LOCAL gint32 __lookup_board_file_id(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);
LOCAL gint32 __lookup_board_extra(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);
LOCAL gint32 __lookup_product_manufacturer(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);
LOCAL gint32 __lookup_product_name(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);
LOCAL gint32 __lookup_product_part_number(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);
LOCAL gint32 __lookup_product_version(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);
LOCAL gint32 __lookup_product_serial_number(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);
LOCAL gint32 __lookup_product_asset_tag(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);
LOCAL gint32 __lookup_product_file_id(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);
LOCAL gint32 __lookup_product_extra(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);
LOCAL void __lookup_system_manufacture_name(guint8 eeprom_format, ELABEL_INFO_S *elabel_info, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, SYSTEM_INFO_U *sys_info, guint32 *max_length);
LOCAL void __lookup_system_name(guint8 eeprom_format, ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, SYSTEM_INFO_U *sys_info, guint32 *max_length);
LOCAL void __lookup_system_version(guint8 eeprom_format, ELABEL_INFO_S *elabel_info, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, SYSTEM_INFO_U *sys_info, guint32 *max_length);
LOCAL void __lookup_system_serial_number(guint8 eeprom_format, ELABEL_INFO_S *elabel_info, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, SYSTEM_INFO_U *sys_info, guint32 *max_length);
LOCAL gint32 __copy_multi_label(GList **multi_label, guint8 *elabel_multi_info);
LOCAL gint32 __get_fru_area_multirecord(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, guint32 *max_length);

typedef gint32 (*LOOKUP_OPERATOR_FIELD_ADDR_FUN)(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, guint32 *max_length);

typedef void (*LOOKUP_CHASSIS_FIELD_ADDR_FUN)(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);

typedef gint32 (*LOOKUP_BOARD_FIELD_ADDR_FUN)(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);

typedef gint32 (*LOOKUP_PRODUCT_FIELD_ADDR_FUN)(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth);

typedef void (*LOOKUP_SYSTEM_FIELD_ADDR_FUN)(guint8 eeprom_format, ELABEL_INFO_S *elabel_info, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, SYSTEM_INFO_U *sys_info, guint32 *max_length);

typedef struct tag_fru_arer_info {
    guint8 fru_arer_info_type;
    LOOKUP_OPERATOR_FIELD_ADDR_FUN lookup_field_addr;
} FRU_AREA_INFO;

typedef struct tag_get_fru_chassis_data {
    guint8 field;
    LOOKUP_CHASSIS_FIELD_ADDR_FUN __lookup_chassis_filed_addr;
} GET_FRU_CHASSIS_DATA;

typedef struct tag_get_fru_board_data {
    guint8 field;
    LOOKUP_BOARD_FIELD_ADDR_FUN __lookup_board_filed_addr;
} GET_FRU_BOARD_DATA;

typedef struct tag_get_fru_product_data {
    guint8 field;
    LOOKUP_PRODUCT_FIELD_ADDR_FUN __lookup_product_filed_addr;
} GET_FRU_PRODUCT_DATA;

typedef struct tag_get_fru_system_data {
    guint8 field;
    LOOKUP_SYSTEM_FIELD_ADDR_FUN __lookup_system_filed_addr;
} GET_FRU_SYSTEM_DATA;

const gchar *g_chassis_type_desc[] = {
    "Unspecified",        "Other",               "Unknown",
    "Desktop",            "Low Profile Desktop", "Pizza Box",
    "Mini Tower",         "Tower",               "Portable",
    "LapTop",             "Notebook",            "Hand Held",
    "Docking Station",    "All in One",          "Sub Notebook",
    "Space-saving",       "Lunch Box",           "Main Server Chassis",
    "Expansion Chassis",  "SubChassis",          "Bus Expansion Chassis",
    "Peripheral Chassis", "RAID Chassis",        "Rack Mount Chassis"
};

LOCAL FRU_AREA_INFO fru_area_info[] = {
    {FRU_AREA_INTERNALUSE, __get_fru_area_internaluse},
    {FRU_AREA_CHASSISINFO, __get_fru_area_chassisinfo},
    {FRU_AREA_BOARDINFO, __get_fru_area_boardinfo},
    {FRU_AREA_PRODUCTINFO, __get_fru_area_productinfo},
    {FRU_AREA_EXTENDELABLE, __get_fru_area_extendelable},
    {FRU_AREA_SYSTEMINFO, __get_fru_area_systeminfo},
    {FRU_INTERNAL_EXTEND_AREA, __get_fru_internal_extend_area},
    {FRU_CHASSIS_EXTEND_AREA, __get_chassis_extend_area},
    {FRU_PRODUCT_EXTEND_AREA, __get_fru_product_extend_area},
    {FRU_AREA_MULTIRECORD, __get_fru_area_multirecord}
};

LOCAL GET_FRU_CHASSIS_DATA g_lookup_fru_chassis_func[] = {
    {FRU_CHASSIS_TYPE, __lookup_chassis_type},
    {FRU_CHASSIS_PART_NUMBER, __lookup_chassis_part_number},
    {FRU_CHASSIS_SERIAL_NUMBER, __lookup_chassis_serial_number},
    {FRU_CHASSIS_EXTRA, __lookup_chassis_extra},
};

LOCAL GET_FRU_BOARD_DATA g_lookup_fru_board_func[] = {
    {FRU_BOARD_MFGDATE, __lookup_board_mfgdata},
    {FRU_BOARD_MANUFACTURER, __lookup_board_manufacturer},
    {FRU_BOARD_PRODUCTNAME, __lookup_board_product_name},
    {FRU_BOARD_SERIALNUMBER, __lookup_board_serial_number},
    {FRU_BOARD_PARTNUMBER, __lookup_board_part_number},
    {FRU_BOARD_FRUFILEID, __lookup_board_file_id},
    {FRU_BOARD_EXTRA, __lookup_board_extra},
};

LOCAL GET_FRU_PRODUCT_DATA g_lookup_fru_product_func[] = {
    {FRU_PRODUCT_MANUFACTURER, __lookup_product_manufacturer},
    {FRU_PRODUCT_NAME, __lookup_product_name},
    {FRU_PRODUCT_PARTNUMBER, __lookup_product_part_number},
    {FRU_PRODUCT_VERSION, __lookup_product_version},
    {FRU_PRODUCT_SERIALNUMBER, __lookup_product_serial_number},
    {FRU_PRODUCT_ASSETTAG, __lookup_product_asset_tag},
    {FRU_PRODUCT_FRUFILEID, __lookup_product_file_id},
    {FRU_PRODUCT_EXTRA, __lookup_product_extra},
};

LOCAL GET_FRU_SYSTEM_DATA g_lookup_fru_system_func[] = {
    {SYSTEM_MANUFACTURE_NAME, __lookup_system_manufacture_name},
    {SYSTEM_NAME, __lookup_system_name},
    {SYSTEM_VERSION, __lookup_system_version},
    {SYSTEM_SERIAL_NUMBER, __lookup_system_serial_number},
};

LOCAL GMutex g_version_change_mutex;


LOCAL gboolean g_fru_read_back_status = FALSE;

void init_elabel_base(void)
{
    g_mutex_init(&g_version_change_mutex);
}

LOCAL void __lookup_system_manufacture_name(guint8 eeprom_format, ELABEL_INFO_S *elabel_info, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, SYSTEM_INFO_U *sys_info, guint32 *max_length)
{
    *addr = &(elabel_info->system.manufacture);
    *field_size = sizeof(elabel_info->system.manufacture);
    *field_data_size = sizeof(elabel_info->system.manufacture.data);
    if (max_length != NULL) {
        *max_length = (eeprom_format == EEPROM_FORMAT_TIANCHI) ? sizeof(sys_info->tc_system_desc_st.sys_mfg_name) :
                                                                 sizeof(sys_info->system_area_st.sys_mfg_name.data);
    }
}

LOCAL void __lookup_system_name(guint8 eeprom_format, ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, SYSTEM_INFO_U *sys_info, guint32 *max_length)
{
    *addr = &(elabel_info->system.name);
    *field_size = sizeof(elabel_info->system.name);
    *field_data_size = sizeof(elabel_info->system.name.data);
    if (max_length != NULL) {
        *max_length = (eeprom_format == EEPROM_FORMAT_TIANCHI) ? sizeof(sys_info->tc_system_desc_st.sys_product_name) :
                                                                 sizeof(sys_info->system_area_st.sys_product_name.data);
    }
}

LOCAL void __lookup_system_version(guint8 eeprom_format, ELABEL_INFO_S *elabel_info, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, SYSTEM_INFO_U *sys_info, guint32 *max_length)
{
    *addr = &(elabel_info->system.version);
    *field_size = sizeof(elabel_info->system.version);
    *field_data_size = sizeof(elabel_info->system.version.data);
    if (max_length != NULL) {
        *max_length = (eeprom_format == EEPROM_FORMAT_TIANCHI) ? sizeof(sys_info->tc_system_desc_st.sys_ver) :
                                                                 sizeof(sys_info->system_area_st.sys_ver.data);
    }
}

LOCAL void __lookup_system_serial_number(guint8 eeprom_format, ELABEL_INFO_S *elabel_info, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, SYSTEM_INFO_U *sys_info, guint32 *max_length)
{
    *addr = &(elabel_info->system.serial_num);
    *field_size = sizeof(elabel_info->system.serial_num);
    *field_data_size = sizeof(elabel_info->system.serial_num.data);
    if (max_length != NULL) {
        *max_length = (eeprom_format == EEPROM_FORMAT_TIANCHI) ? sizeof(sys_info->tc_system_desc_st.sys_sn) :
                                                                 sizeof(sys_info->system_area_st.sys_sn.data);
    }
}

LOCAL gint32 __lookup_product_manufacturer(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    *addr = &(elabel_info->product.manufacture);
    *field_size = sizeof(elabel_info->product.manufacture);
    *field_data_size = sizeof(elabel_info->product.manufacture.data);
    if (max_lenth != NULL) {
        *max_lenth = sizeof(fru_info->product.product_manufacturer.content);
    }
    return RET_OK;
}

LOCAL gint32 __lookup_product_name(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    *addr = &(elabel_info->product.name);
    *field_size = sizeof(elabel_info->product.name);
    *field_data_size = sizeof(elabel_info->product.name.data);
    if (max_lenth != NULL) {
        *max_lenth = sizeof(fru_info->product.product_name.content);
    }
    return RET_OK;
}

LOCAL gint32 __lookup_product_part_number(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    *addr = &(elabel_info->product.part_num);
    *field_size = sizeof(elabel_info->product.part_num);
    *field_data_size = sizeof(elabel_info->product.part_num.data);
    if (max_lenth != NULL) {
        *max_lenth = sizeof(fru_info->product.product_part_num.content);
    }
    return RET_OK;
}

LOCAL gint32 __lookup_product_version(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    *addr = &(elabel_info->product.version);
    *field_size = sizeof(elabel_info->product.version);
    *field_data_size = sizeof(elabel_info->product.version.data);
    if (max_lenth != NULL) {
        *max_lenth = sizeof(fru_info->product.product_version.content);
    }
    return RET_OK;
}

LOCAL gint32 __lookup_product_serial_number(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    *addr = &(elabel_info->product.serial_num);
    *field_size = sizeof(elabel_info->product.serial_num);
    *field_data_size = sizeof(elabel_info->product.serial_num.data);
    if (max_lenth != NULL) {
        *max_lenth = sizeof(fru_info->product.product_serial_num.content);
    }
    return RET_OK;
}

LOCAL gint32 __lookup_product_asset_tag(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    *addr = &(elabel_info->product.asset_tag);
    *field_size = sizeof(elabel_info->product.asset_tag);
    *field_data_size = sizeof(elabel_info->product.asset_tag.data);
    if (max_lenth != NULL) {
        *max_lenth = sizeof(fru_info->product.product_asset_tag.content);
    }
    return RET_OK;
}

LOCAL gint32 __lookup_product_file_id(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    *addr = &(elabel_info->product.file_id);
    *field_size = sizeof(elabel_info->product.file_id);
    *field_data_size = sizeof(elabel_info->product.file_id.data);
    if (max_lenth != NULL) {
        *max_lenth = sizeof(fru_info->product.file_id.content);
    }
    return RET_OK;
}

LOCAL gint32 __lookup_product_extra(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    *addr = &(elabel_info->product_extend);
    *field_size = sizeof(elabel_info->product_extend);
    *field_data_size = sizeof(elabel_info->product_extend.data);
    if (max_lenth != NULL) {
        *max_lenth = sizeof(elabel_info->product_extend.data);
    }
    return RET_OK;
}

LOCAL gint32 __lookup_board_mfgdata(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    *addr = &(elabel_info->board.mfg_time);
    *field_size = sizeof(elabel_info->board.mfg_time);
    *field_data_size = sizeof(elabel_info->board.mfg_time.data);
    if (max_lenth != NULL) {
        OBJ_HANDLE obj_handle = 0;
        guint8 elabel_format = 0;
        (void)dfl_get_object_handle(OBJECT_BMC, &obj_handle);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_BMC_ELABELMANUFORMAT, &elabel_format);
        if (elabel_format == 1) {
            *max_lenth = sizeof(fru_info->board.mfg_date_time);
        } else {
            *max_lenth = sizeof(fru_info->board.mfg_date_time) - 1;
        }
    }
    return RET_OK;
}

LOCAL gint32 __lookup_board_manufacturer(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    *addr = &(elabel_info->board.manufacture);
    *field_size = sizeof(elabel_info->board.manufacture);
    *field_data_size = sizeof(elabel_info->board.manufacture.data);
    if (max_lenth != NULL) {
        *max_lenth = sizeof(fru_info->board.board_manufacturer.content);
    }
    return RET_OK;
}

LOCAL gint32 __lookup_board_product_name(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    *addr = &(elabel_info->board.product_name);
    *field_size = sizeof(elabel_info->board.product_name);
    *field_data_size = sizeof(elabel_info->board.product_name.data);
    if (max_lenth != NULL) {
        *max_lenth = sizeof(fru_info->board.board_product_name.content);
    }
    return RET_OK;
}

LOCAL gint32 __lookup_board_serial_number(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    *addr = &(elabel_info->board.serial_num);
    *field_size = sizeof(elabel_info->board.serial_num);
    *field_data_size = sizeof(elabel_info->board.serial_num.data);
    if (max_lenth != NULL) {
        *max_lenth = sizeof(fru_info->board.board_serial_num.content);
    }
    return RET_OK;
}

LOCAL gint32 __lookup_board_part_number(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    *addr = &(elabel_info->board.part_num);
    *field_size = sizeof(elabel_info->board.part_num);
    *field_data_size = sizeof(elabel_info->board.part_num.data);
    if (max_lenth != NULL) {
        *max_lenth = sizeof(fru_info->board.board_part_num.content);
    }
    return RET_OK;
}

LOCAL gint32 __lookup_board_file_id(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    *addr = &(elabel_info->board.file_id);
    *field_size = sizeof(elabel_info->board.file_id);
    *field_data_size = sizeof(elabel_info->board.file_id.data);
    if (max_lenth != NULL) {
        *max_lenth = sizeof(fru_info->board.file_id.content);
    }
    return RET_OK;
}

LOCAL gint32 __lookup_board_extra(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    *addr = &(elabel_info->extend);
    *field_size = sizeof(elabel_info->extend);
    *field_data_size = sizeof(elabel_info->extend.data);
    if (max_lenth != NULL) {
        *max_lenth = sizeof(elabel_info->extend.data);
    }

    return RET_OK;
}

LOCAL void __lookup_chassis_type(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    *addr = &(elabel_info->chassis.type);
    *field_size = sizeof(elabel_info->chassis.type);
    *field_data_size = sizeof(elabel_info->chassis.type.data);
    if (max_lenth != NULL) {
        *max_lenth = sizeof(fru_info->chassis.chassis_type);
    }
}

LOCAL void __lookup_chassis_part_number(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    *addr = &(elabel_info->chassis.part_num);
    *field_size = sizeof(elabel_info->chassis.part_num);
    *field_data_size = sizeof(elabel_info->chassis.part_num.data);
    if (max_lenth != NULL) {
        *max_lenth = sizeof(fru_info->chassis.chassis_part_num.content);
    }
}

LOCAL void __lookup_chassis_serial_number(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    *addr = &(elabel_info->chassis.serial_num);
    *field_size = sizeof(elabel_info->chassis.serial_num);
    *field_data_size = sizeof(elabel_info->chassis.serial_num.data);
    if (max_lenth != NULL) {
        *max_lenth = sizeof(fru_info->chassis.chassis_serial_num.content);
    }
}

LOCAL void __lookup_chassis_extra(ELABEL_INFO_S *elabel_info, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    *addr = &(elabel_info->chassis_extend);
    *field_size = sizeof(elabel_info->chassis_extend);
    *field_data_size = sizeof(elabel_info->chassis_extend.data);
    if (max_lenth != NULL) {
        *max_lenth = sizeof(elabel_info->chassis_extend.data);
    }
}


LOCAL gint32 __check_field_addr_input_param(FRU_PRIV_PROPERTY_S *fru_priv, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size)
{
    if (fru_priv == NULL || addr == NULL || field_size == NULL || field_data_size == NULL ||
        fru_priv->elabel_info == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 __check_area_internal_chassis_board_product(FRU_PRIV_PROPERTY_S *fru_priv, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size)
{
    gint32 ret = __check_field_addr_input_param(fru_priv, addr, field_size, field_data_size);
    if (ret != RET_OK) {
        return ret;
    }

    if (fru_priv->fru_info == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 __lookup_internal_field_addr_size(ELABEL_INFO_S *elabel_info, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    switch (field) {
        case FRU_INTERNAL_DEFAULT:
            *addr = &(elabel_info->internal);
            *field_size = sizeof(elabel_info->internal);
            *field_data_size = sizeof(elabel_info->internal.data);
            if (max_lenth != NULL) {
                *max_lenth = sizeof(elabel_info->internal.data);
            }
            return RET_OK;
        case FRU_INTERNAL_EXTRA:
            *addr = &(elabel_info->internal_extend);
            *field_size = sizeof(elabel_info->internal_extend);
            *field_data_size = sizeof(elabel_info->internal_extend.data);
            if (max_lenth != NULL) {
                *max_lenth = sizeof(elabel_info->internal_extend.data);
            }
            return RET_OK;
        default:
            break;
    }
    return RET_ERR;
}


LOCAL gint32 __lookup_chassis_field_addr_size(ELABEL_INFO_S *elabel_info, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    size_t count = sizeof(g_lookup_fru_chassis_func) / sizeof(GET_FRU_CHASSIS_DATA);
    for (size_t i = 0; i < count; i++) {
        if (g_lookup_fru_chassis_func[i].field == field) {
            g_lookup_fru_chassis_func[i].__lookup_chassis_filed_addr(elabel_info, addr, field_size, field_data_size,
                fru_info, max_lenth);
            return RET_OK;
        }
    }

    return RET_ERR;
}


LOCAL gint32 __lookup_board_field_addr_size(ELABEL_INFO_S *elabel_info, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    size_t count = sizeof(g_lookup_fru_board_func) / sizeof(GET_FRU_BOARD_DATA);
    for (size_t i = 0; i < count; i++) {
        if (g_lookup_fru_board_func[i].field == field) {
            return g_lookup_fru_board_func[i].__lookup_board_filed_addr(elabel_info, addr, field_size, field_data_size,
                fru_info, max_lenth);
        }
    }

    return RET_ERR;
}


LOCAL gint32 __lookup_product_field_addr_size(ELABEL_INFO_S *elabel_info, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, FRU_INFO_S *fru_info, guint32 *max_lenth)
{
    size_t count = sizeof(g_lookup_fru_product_func) / sizeof(GET_FRU_PRODUCT_DATA);
    for (size_t i = 0; i < count; i++) {
        if (g_lookup_fru_product_func[i].field == field) {
            return g_lookup_fru_product_func[i].__lookup_product_filed_addr(elabel_info, addr, field_size,
                field_data_size, fru_info, max_lenth);
        }
    }

    return RET_ERR;
}


LOCAL gint32 __lookup_system_field_addr_size(guint8 eeprom_format, ELABEL_INFO_S *elabel_info, guint8 field,
    gpointer *addr, guint32 *field_size, guint32 *field_data_size, SYSTEM_INFO_U *sys_info, guint32 *max_length)
{
    size_t count = sizeof(g_lookup_fru_system_func) / sizeof(GET_FRU_SYSTEM_DATA);
    for (size_t i = 0; i < count; i++) {
        if (g_lookup_fru_system_func[i].field == field) {
            g_lookup_fru_system_func[i].__lookup_system_filed_addr(eeprom_format, elabel_info, addr, field_size,
                field_data_size, sys_info, max_length);
            return RET_OK;
        }
    }

    return RET_ERR;
}


LOCAL gint32 __get_fru_area_internaluse(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, guint32 *max_length)
{
    gint32 ret = __check_area_internal_chassis_board_product(fru_priv, addr, field_size, field_data_size);
    if (ret != RET_OK) {
        return ret;
    }

    return __lookup_internal_field_addr_size(fru_priv->elabel_info, field, addr, field_size, field_data_size,
        fru_priv->fru_info, max_length);
}


LOCAL gint32 __get_fru_area_chassisinfo(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, guint32 *max_length)
{
    gint32 ret = __check_area_internal_chassis_board_product(fru_priv, addr, field_size, field_data_size);
    if (ret != RET_OK) {
        return ret;
    }

    return __lookup_chassis_field_addr_size(fru_priv->elabel_info, field, addr, field_size, field_data_size,
        fru_priv->fru_info, max_length);
}


LOCAL gint32 __get_fru_area_boardinfo(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, guint32 *max_length)
{
    gint32 ret = __check_area_internal_chassis_board_product(fru_priv, addr, field_size, field_data_size);
    if (ret != RET_OK) {
        return ret;
    }

    return __lookup_board_field_addr_size(fru_priv->elabel_info, field, addr, field_size, field_data_size,
        fru_priv->fru_info, max_length);
}


LOCAL gint32 __get_fru_area_productinfo(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, guint32 *max_length)
{
    gint32 ret = __check_area_internal_chassis_board_product(fru_priv, addr, field_size, field_data_size);
    if (ret != RET_OK) {
        return ret;
    }

    return __lookup_product_field_addr_size(fru_priv->elabel_info, field, addr, field_size, field_data_size,
        fru_priv->fru_info, max_length);
}


LOCAL gint32 __get_fru_area_extendelable(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, guint32 *max_length)
{
    gint32 ret = __check_field_addr_input_param(fru_priv, addr, field_size, field_data_size);
    if (ret != RET_OK) {
        return ret;
    }

    ELABEL_INFO_S *elabel_info = fru_priv->elabel_info;
    *addr = &(elabel_info->extend);
    *field_size = sizeof(elabel_info->extend);
    *field_data_size = sizeof(elabel_info->extend.data);

    if (max_length != NULL) {
        *max_length = sizeof(elabel_info->extend.data);
    }

    return RET_OK;
}


LOCAL gint32 __get_fru_area_systeminfo(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, guint32 *max_length)
{
    gint32 ret = __check_field_addr_input_param(fru_priv, addr, field_size, field_data_size);
    if (ret != RET_OK) {
        return ret;
    }

    if (fru_priv->system_info == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }

    return __lookup_system_field_addr_size(fru_priv->eeprom_format, fru_priv->elabel_info, field, addr, field_size,
        field_data_size, fru_priv->system_info, max_length);
}


LOCAL gint32 __get_fru_internal_extend_area(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, guint32 *max_length)
{
    gint32 ret = __check_field_addr_input_param(fru_priv, addr, field_size, field_data_size);
    if (ret != RET_OK) {
        return ret;
    }

    ELABEL_INFO_S *elabel_info = fru_priv->elabel_info;

    *addr = &(elabel_info->internal_extend);
    *field_size = sizeof(elabel_info->internal_extend);
    *field_data_size = sizeof(elabel_info->internal_extend.data);

    if (max_length != NULL) {
        *max_length = sizeof(elabel_info->internal_extend.data);
    }

    return RET_OK;
}


LOCAL gint32 __get_chassis_extend_area(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr, guint32 *field_size,
    guint32 *field_data_size, guint32 *max_length)
{
    gint32 ret = __check_field_addr_input_param(fru_priv, addr, field_size, field_data_size);
    if (ret != RET_OK) {
        return ret;
    }

    ELABEL_INFO_S *elabel_info = fru_priv->elabel_info;

    *addr = &(elabel_info->chassis_extend);
    *field_size = sizeof(elabel_info->chassis_extend);
    *field_data_size = sizeof(elabel_info->chassis_extend.data);

    if (max_length != NULL) {
        *max_length = sizeof(elabel_info->chassis_extend.data);
    }

    return RET_OK;
}


LOCAL gint32 __get_fru_product_extend_area(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, guint32 *max_length)
{
    gint32 ret = __check_field_addr_input_param(fru_priv, addr, field_size, field_data_size);
    if (ret != RET_OK) {
        return ret;
    }

    ELABEL_INFO_S *elabel_info = fru_priv->elabel_info;
    *addr = &(elabel_info->product_extend);
    *field_size = sizeof(elabel_info->product_extend);
    *field_data_size = sizeof(elabel_info->product_extend.data);

    if (max_length != NULL) {
        *max_length = sizeof(elabel_info->product_extend.data);
    }

    return RET_OK;
}


LOCAL gint32 __lookup_hmm_operator_field_addr_size(FRU_PRIV_PROPERTY_S *fru_priv, guint8 area, guint8 field,
    gpointer *addr, guint32 *field_size, guint32 *field_data_size, guint32 *max_length)
{
    if (fru_priv->fru_property.fru_type != FRU_TYPE_BACKPLANE) {
        return RET_OK;
    }

    if (area != FRU_AREA_CHASSISINFO) {
        return RET_ERR;
    }

    if (max_length == NULL) {
        return RET_OK;
    }

    if (field == FRU_CHASSIS_PART_NUMBER) {
        *max_length = CHASSIS_PART_NUMBER_LEN;
    } else if (field == FRU_CHASSIS_SERIAL_NUMBER) {
        *max_length = CHASSIS_SERIAL_NUMBER_LEN;
    }

    return RET_OK;
}


LOCAL void __calc_date_time(const guint8 *input, gchar *output, guint32 out_size)
{
    if (out_size == 0) {
        debug_log(DLOG_ERROR, "%s failed:input param error, out_size is 0.", __FUNCTION__);
        return;
    }
    if (input == NULL || output == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return;
    }

    OBJ_HANDLE obj_handle = 0;
    guint8 elabel_format = 0;
    guint32 minutes = 0;
    (void)dfl_get_object_handle(OBJECT_BMC, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BMC_ELABELMANUFORMAT, &elabel_format);
    if (elabel_format == 1) {
        minutes = ((guint32)input[0]) |
                  ((guint32)input[1] << 8) |   // input[1]需要移位8
                  ((guint32)input[2] << 16) |  // input[2]需要移位16
                  ((guint32)input[3] << 24);   // input[3]需要移位24
    } else {
        // 输入数据分别移位 8 和 16
        minutes = ((guint32)input[0]) |
                  ((guint32)input[1] << 8) |   // input[1]需要移位8
                  ((guint32)input[2] << 16);   // input[2]需要移位16
    }
    guint32 seconds = minutes * 60; // 乘以60转换为秒

    
    RTC_TIME_S date = vos_gettime_bysecond(1996, seconds);

    const gchar *wday[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    gint32 ret = snprintf_s(output, out_size, out_size - 1, "%04d/%02d/%02d %s %02d:%02d:%02d", date.wYear,
        date.ucMonth, date.ucDate, wday[date.ucDay], date.ucHours, date.ucMinutes, date.ucSecond);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    return;
}

LOCAL void __set_elabel_board_manufacture(FRU_PRIV_PROPERTY_S *fru_priv_property, gboolean *update_flag)
{
    OBJ_HANDLE ref_object_handle = 0;
    gchar ref_object_name[MAX_NAME_SIZE] = {0};
    gchar ref_property_name[MAX_NAME_SIZE] = {0};
    FRU_PRIV_PROPERTY_S *ref_fru_priv_property = NULL;

    gint32 ret = dfl_get_referenced_property(fru_priv_property->fru_property.elabel_handle,
        PROPERTY_ELABEL_BOARD_MFG_NAME, ref_object_name, ref_property_name, MAX_NAME_SIZE, MAX_NAME_SIZE);
    if (ret != DFL_OK) {
        return;
    }

    
    GVariant *property_value = g_variant_new_string((gchar *)ref_object_name);
    ret = dfl_get_specific_object(CLASS_FRU, PROPERTY_FRU_ELABEL_RO, property_value, &ref_object_handle);
    g_variant_unref(property_value);
    if (ret != DFL_OK) {
        return;
    }

    
    ret = get_obj_priv_data(ref_object_handle, (gpointer *)&(ref_fru_priv_property));
    if (ret != DFL_OK) {
        return;
    }

    if (strcmp((gchar *)fru_priv_property->elabel_info->board.manufacture.data,
        (gchar *)ref_fru_priv_property->elabel_info->board.manufacture.data) == 0) {
        put_obj_priv_data(ref_object_handle, (gpointer)ref_fru_priv_property);
        return;
    }

    
    (void)memset_s(&fru_priv_property->elabel_info->board.manufacture,
        sizeof(fru_priv_property->elabel_info->board.manufacture), 0,
        sizeof(fru_priv_property->elabel_info->board.manufacture));
    (void)memcpy_s((gchar *)&fru_priv_property->elabel_info->board.manufacture,
        sizeof(fru_priv_property->elabel_info->board.manufacture),
        (gchar *)&ref_fru_priv_property->elabel_info->board.manufacture,
        sizeof(ref_fru_priv_property->elabel_info->board.manufacture));

    
    set_one_elabel_property(fru_priv_property->fru_property.elabel_handle, PROPERTY_ELABEL_BOARD_MFG_NAME,
        (const gchar *)fru_priv_property->fru_info->board.board_manufacturer.content);

    *update_flag = TRUE;
    put_obj_priv_data(ref_object_handle, (gpointer)ref_fru_priv_property);
}

LOCAL void __set_elabel_product_manufacture(FRU_PRIV_PROPERTY_S *fru_priv_property, gboolean *update_flag)
{
    OBJ_HANDLE ref_object_handle = 0;
    gchar ref_object_name[MAX_NAME_SIZE] = {0};
    gchar ref_property_name[MAX_NAME_SIZE] = {0};
    FRU_PRIV_PROPERTY_S *ref_fru_priv_property = NULL;

    
    gint32 ret = dfl_get_referenced_property(fru_priv_property->fru_property.elabel_handle,
        PROPERTY_ELABEL_PRODUCT_MFG_NAME, ref_object_name, ref_property_name, MAX_NAME_SIZE, MAX_NAME_SIZE);
    if (ret != DFL_OK) {
        return;
    }

    
    GVariant *property_value = g_variant_new_string((gchar *)ref_object_name);
    ret = dfl_get_specific_object(CLASS_FRU, PROPERTY_FRU_ELABEL_RO, property_value, &ref_object_handle);
    g_variant_unref(property_value);
    if (ret != DFL_OK) {
        return;
    }

    
    ret = get_obj_priv_data(ref_object_handle, (gpointer *)&(ref_fru_priv_property));
    if (ret != DFL_OK) {
        return;
    }
    if (strcmp((gchar *)fru_priv_property->elabel_info->product.manufacture.data,
        (gchar *)ref_fru_priv_property->elabel_info->product.manufacture.data) == 0) {
        put_obj_priv_data(ref_object_handle, (gpointer)ref_fru_priv_property);
        return;
    }

    
    (void)memset_s(&fru_priv_property->elabel_info->product.manufacture,
        sizeof(fru_priv_property->elabel_info->product.manufacture), 0,
        sizeof(fru_priv_property->elabel_info->product.manufacture));
    (void)memcpy_s((gchar *)&fru_priv_property->elabel_info->product.manufacture,
        sizeof(fru_priv_property->elabel_info->product.manufacture),
        (gchar *)&ref_fru_priv_property->elabel_info->product.manufacture,
        sizeof(ref_fru_priv_property->elabel_info->product.manufacture));

    
    set_one_elabel_property(fru_priv_property->fru_property.elabel_handle, PROPERTY_ELABEL_PRODUCT_MFG_NAME,
        (const gchar *)fru_priv_property->fru_info->board.board_manufacturer.content);

    *update_flag = TRUE;
    put_obj_priv_data(ref_object_handle, (gpointer)ref_fru_priv_property);
}


LOCAL gint32 __elabel_syn_manufacture(OBJ_HANDLE object_handle)
{
    (void)prctl(PR_SET_NAME, (gulong) "SynElabelManu");

    FRU_PRIV_PROPERTY_S *fru_priv_property = NULL;

    
    gint32 ret = get_obj_priv_data(object_handle, (gpointer *)&(fru_priv_property));
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: call dfl_get_binded_object failed, error = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    gboolean update_flag = FALSE;

    __set_elabel_board_manufacture(fru_priv_property, &update_flag);

    __set_elabel_product_manufacture(fru_priv_property, &update_flag);

    
    if (!update_flag) {
        put_obj_priv_data(object_handle, (gpointer)fru_priv_property);
        return RET_OK;
    }

    (void)vos_thread_sem4_v(fru_priv_property->elabel_syn_semid);
    sync_elabel_to_fru(fru_priv_property->fru_info, fru_priv_property->elabel_info);
    (void)vos_thread_sem4_v(fru_priv_property->update_syn_semid); 
    (void)dal_set_property_value_byte(fru_priv_property->fru_property.frudev_handle, PROPERTY_FRUDEV_HEALTH,
        FRU_HEALTH_OK, DF_NONE);

    put_obj_priv_data(object_handle, (gpointer)fru_priv_property);

    return RET_OK;
}


LOCAL gint32 __get_extend_label_by_index(ELABEL_EXTEND_AREA_S *extend, guint8 field, guint8 **label, guint32 *len)
{
    if (extend == NULL || label == NULL || len == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }

    gchar **split_str = g_strsplit((const gchar *)extend->data, ";", MAX_EXTERN_ELABEL_NUM);
    if (split_str == NULL) {
        debug_log(DLOG_ERROR, "%s: g_strsplit error.", __FUNCTION__);
        return COMP_CODE_UNKNOWN;
    }

    gchar *tmp = NULL;
    for (guint32 i = 0; i < MAX_EXTERN_ELABEL_NUM; i++) {
        if ((split_str[i] == NULL) || ((i + 1) == field)) {
            tmp = g_strdup(split_str[i]);
            break;
        }
    }
    g_strfreev(split_str);

    if (tmp == NULL) {
        *label = (guint8 *)g_malloc0(sizeof(guint8));
        if (*label == NULL) {
            return RET_ERR;
        }
        *len = 0;
    } else {
        *label = (guint8 *)tmp;
        *len = strlen(tmp);
    }

    return RET_OK;
}


LOCAL gint32 __read_multi_record(FRU_PRIV_PROPERTY_S *fru_priv, guint8 *multi_record, guint32 field_size)
{
    gint32 ret;
    gint32 i;
    if (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) {
        errno_t safe_fun_ret = memcpy_s(multi_record, MAX_MULTI_LEN, &fru_priv->elabel_info->multi_record, field_size);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
        return FRUDATA_OK;
    }
    for (i = 0; i < EEP_READ_RETRY; i++) {
        ret = dfl_block_read(fru_priv->fru_property.frudev_handle, PROPERTY_FRUDEV_STORAGE_LOC,
            ELABEL_MULTI_AREA_OFFSET, field_size, multi_record);
        if (ret == DFL_OK) {
            return FRUDATA_OK;
        }
        vos_task_delay(EEP_READ_INTERVAL);
    }

    debug_log(DLOG_ERROR, "[%s]read multi record from eeprom fail, ret=%d.", __FUNCTION__, ret);
    return ret;
}

LOCAL gint32 __get_multi_label_buf_and_len(FRU_PRIV_PROPERTY_S *fru_priv, guint8 **label, guint32 *len,
    guint32 field_size)
{
    guint16 muti_len = 0;
    gint32 ret;
    guint8  tmp_buf[MAX_MULTI_LEN] = {0};
    ret = __read_multi_record(fru_priv, tmp_buf, field_size);
    if (ret != RET_OK) {
        return ret;
    }
    ELABEL_MULTIRECORD_SINGLE_S *single_record = (ELABEL_MULTIRECORD_SINGLE_S *)tmp_buf;
    while (!(single_record->end_flag & MASK_BIT7) && muti_len < field_size) {
        muti_len += SINGLE_MULTI_HEAD_LEN + single_record->len;
        single_record = (ELABEL_MULTIRECORD_SINGLE_S *)((guint8 *)tmp_buf + muti_len);
    }
    if (single_record->end_flag & MASK_BIT7) {
        muti_len += SINGLE_MULTI_HEAD_LEN + single_record->len;
    }
    if (muti_len == 0) {
        return COMP_CODE_SUCCESS;
    }
    *label = (guint8 *)g_malloc0(muti_len);
    if (*label == NULL) {
        debug_log(DLOG_ERROR, "%s:alloc memory failed.", __FUNCTION__);
        return COMP_CODE_OUTOF_RANGE;
    }
    *len = muti_len;
    ret = memcpy_s(*label, field_size, tmp_buf, *len);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: copy mem failed, ret = %d", __FUNCTION__, ret);
        g_free(*label);
        *label = NULL;
        return COMP_CODE_OUTOF_RANGE;
    }
    return COMP_CODE_SUCCESS;
}


LOCAL void __gen_one_extend_label(GList **extend_label, gchar *buf, guint8 len, guint8 pos, gboolean is_last_one)
{
    if (buf == NULL || extend_label == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return;
    }

    FRU_ELABEL_FORMAT_S *fru_label = (FRU_ELABEL_FORMAT_S *)g_malloc0(sizeof(FRU_ELABEL_FORMAT_S));
    if (fru_label == NULL) {
        return;
    }
    fru_label->type = CHAR_TYPE; 

    
    fru_label->content[0] = (is_last_one ? pos : (0x80 + pos));
    errno_t safe_fun_ret = memmove_s(&fru_label->content[1], FRU_MAX_ELABEL_LEN - 1, buf, len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    fru_label->len = len + 1;
    *extend_label = g_list_append(*extend_label, fru_label);

    return;
}

LOCAL void __copy_elabel_to_fru(gboolean *fru_label_exist, FRU_ELABEL_FORMAT_S *fru_label,
    ELABEL_FIELD_72_BYTES_S elabel)
{
    (void)memset_s(fru_label->content, FRU_MAX_ELABEL_LEN, 0, FRU_MAX_ELABEL_LEN);
    fru_label->len = MIN(FRU_MAX_ELABEL_LEN, elabel.len);
    fru_label->type = CHAR_TYPE;
    gint32 ret = memmove_s(fru_label->content, FRU_MAX_ELABEL_LEN, elabel.data, MIN(FRU_MAX_ELABEL_LEN, elabel.len));
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, ret);
    }
    if (elabel.len != 0) {
        *fru_label_exist = TRUE;
    }
}

LOCAL void __sync_internal_to_fru(FRU_INFO_S *fru_info, ELABEL_INFO_S *elabel_info)
{
    COPY_ELABEL_INTERNAL_TO_FRU(fru_info->has_internal, fru_info->internal, elabel_info->internal);
}

LOCAL void __sync_chassis_to_fru(FRU_INFO_S *fru_info, ELABEL_INFO_S *elabel_info)
{
    fru_info->chassis.area_ver = DEFAULT_AREA_VER;

    if (elabel_info->chassis.type.len != 0) { 
        fru_info->chassis.chassis_type = elabel_info->chassis.type.data[0];
        fru_info->has_chassis = TRUE;
    } else {
        fru_info->chassis.chassis_type = 0; 
    }

    __copy_elabel_to_fru(&(fru_info->has_chassis), &(fru_info->chassis.chassis_part_num),
        elabel_info->chassis.part_num);
    __copy_elabel_to_fru(&(fru_info->has_chassis), &(fru_info->chassis.chassis_serial_num),
        elabel_info->chassis.serial_num);
    
    if (copy_extend_label(&fru_info->chassis.custom_info, &elabel_info->chassis_extend) > 0) {
        fru_info->has_chassis = TRUE;
    }
}

LOCAL void __sync_board_to_fru(FRU_INFO_S *fru_info, ELABEL_INFO_S *elabel_info)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 elabel_format = 0;
    (void)dfl_get_object_handle(OBJECT_BMC, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BMC_ELABELMANUFORMAT, &elabel_format);
    if (elabel_format == 1) {
        errno_t safe_fun_ret = memmove_s(fru_info->board.mfg_date_time, sizeof(fru_info->board.mfg_date_time),
            elabel_info->board.mfg_time.data, sizeof(fru_info->board.mfg_date_time));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
    } else {
        errno_t safe_fun_ret = memmove_s(fru_info->board.mfg_date_time,
            sizeof(fru_info->board.mfg_date_time) - 1, elabel_info->board.mfg_time.data,
            sizeof(fru_info->board.mfg_date_time) - 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
    }
    __copy_elabel_to_fru(&(fru_info->has_board), &(fru_info->board.board_manufacturer), elabel_info->board.manufacture);
    __copy_elabel_to_fru(&(fru_info->has_board), &(fru_info->board.board_product_name),
        elabel_info->board.product_name);
    __copy_elabel_to_fru(&(fru_info->has_board), &(fru_info->board.board_serial_num), elabel_info->board.serial_num);
    __copy_elabel_to_fru(&(fru_info->has_board), &(fru_info->board.board_part_num), elabel_info->board.part_num);
    __copy_elabel_to_fru(&(fru_info->has_board), &(fru_info->board.file_id), elabel_info->board.file_id);
    
    if (copy_extend_label(&fru_info->board.extension_label, &elabel_info->extend) > 0) {
        fru_info->has_board = TRUE;
    }
}

LOCAL void __sync_product_to_fru(FRU_INFO_S *fru_info, ELABEL_INFO_S *elabel_info)
{
    __copy_elabel_to_fru(&(fru_info->has_product), &(fru_info->product.product_manufacturer),
        elabel_info->product.manufacture);
    __copy_elabel_to_fru(&(fru_info->has_product), &(fru_info->product.product_name), elabel_info->product.name);
    __copy_elabel_to_fru(&(fru_info->has_product), &(fru_info->product.product_part_num),
        elabel_info->product.part_num);
    __copy_elabel_to_fru(&(fru_info->has_product), &(fru_info->product.product_version), elabel_info->product.version);
    __copy_elabel_to_fru(&(fru_info->has_product), &(fru_info->product.product_serial_num),
        elabel_info->product.serial_num);
    __copy_elabel_to_fru(&(fru_info->has_product), &(fru_info->product.product_asset_tag),
        elabel_info->product.asset_tag);
    __copy_elabel_to_fru(&(fru_info->has_product), &(fru_info->product.file_id), elabel_info->product.file_id);
    
    if (copy_extend_label(&fru_info->product.custom_info, &elabel_info->product_extend) > 0) {
        fru_info->has_product = TRUE;
    }
    if (__copy_multi_label(&fru_info->multi_record, elabel_info->multi_record) == RET_OK) {
        fru_info->has_multi = TRUE;
    }
}


void split_extend_with_semicolon(ELABEL_EXTEND_AREA_S *elabel_extend)
{
    if (elabel_extend->num <= 0 || elabel_extend->len > FRU_EXTEND_DATA_MAX_LEN || elabel_extend->len == 0) {
        debug_log(DLOG_ERROR, "[%s] invlaid extend elabel, num = %u, len = %u", __FUNCTION__, elabel_extend->num,
            elabel_extend->len);
        return;
    }

    elabel_extend->len = elabel_extend->len - 1; // V2 的长度不包含最后一个为0的字节
    for (guint32 i = 0; i < elabel_extend->len; i++) {
        if (elabel_extend->data[i] == '\0') {
            elabel_extend->data[i] = ';';
        }
    }

    elabel_extend->crc = arith_make_crc_checksum(0, (const guchar *)&elabel_extend->len,
        MIN(elabel_extend->len + FRU_EXTERN_LABEL_PREFIX_LEN,
        sizeof(elabel_extend->data) + FRU_EXTERN_LABEL_PREFIX_LEN));
}

void sync_elabel_from_fru(ELABEL_FIELD_72_BYTES_S *elabel_field, const FRU_ELABEL_FORMAT_S fruinfo_label)
{
    elabel_field->len = fruinfo_label.len;
    errno_t memcoy_s_ret =
        memcpy_s(elabel_field->data, sizeof(elabel_field->data), fruinfo_label.content, elabel_field->len);
    if (memcoy_s_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, memcoy_s_ret);
    }
    elabel_field->crc = arith_make_crc_checksum(0, (const guchar *)&elabel_field->len,
        MIN(elabel_field->len + sizeof(guint16), sizeof(elabel_field->data) + sizeof(guint16)));
}


void combine_extension_label(GList *input, gchar *output, gint32 out_size, guint8 *num)
{
    if (input == NULL || output == NULL) {
        debug_log(DLOG_ERROR, "[%s][%d] Invalid parameter!", __FUNCTION__, __LINE__);
        return;
    }

    GList *list = input;
    gchar *p = output;
    guint8 extend_label_num = 0;
    gint32 str_len = 0;
    while (list != NULL) {
        GList *next = list->next;
        FRU_ELABEL_FORMAT_S *elabel_tmp = (FRU_ELABEL_FORMAT_S *)list->data;

        guint8 *content = elabel_tmp->content;
        if ((elabel_tmp->content[0] <= 0x0f) ||
            ((elabel_tmp->content[0] >= 0x80) && (elabel_tmp->content[0] <= 0x8f))) {
            content = content + 1;
        }

        if (out_size - str_len <= 0) {
            list = next;
            continue;
        }

        gint32 len;
        if (((strstr((const gchar *)content, "=") != NULL) || (strstr((const gchar *)content, ":") != NULL)) &&
            (strlen(output) != 0)) {
            len = snprintf_s(p + str_len, out_size - str_len, out_size - str_len - 1, ";%s", content);
            extend_label_num++;
        } else {
            len = snprintf_s(p + str_len, out_size - str_len, out_size - str_len - 1, "%s", content);
        }

        if (len <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, len);
        }
        str_len += len;

        list = next;
    }

    if (num != NULL) {
        *num = extend_label_num + 1;
    }

    return;
}


void send_ver_change_evt(guint8 evt_code)
{
    g_mutex_lock(&g_version_change_mutex);

    OBJ_HANDLE object_handle;
    (void)dfl_get_object_handle(OBJ_FRU_STATICS, &object_handle);

    (void)dal_set_property_value_uint32(object_handle, PROPERTY_VERSION_CHANGE, MAKE_DWORD(0x01, 0xff, evt_code, 0x01),
        DF_NONE);

    
    vos_task_delay(5000);

    (void)dal_set_property_value_uint32(object_handle, PROPERTY_VERSION_CHANGE, MAKE_DWORD(0x00, 0xff, evt_code, 0x01),
        DF_NONE);

    g_mutex_unlock(&g_version_change_mutex);

    return;
}

void set_one_elabel_property(OBJ_HANDLE handle, const gchar *property_name, const gchar *desc)
{
    gchar *property_buf = (gchar *)g_malloc0(FRU_MAX_ELABEL_LEN + 1);
    if (property_buf == NULL) {
        debug_log(DLOG_ERROR, "%s g_malloc0 error", __FUNCTION__);
        return;
    }

    errno_t strncpy_fun_ret = strncpy_s(property_buf, FRU_MAX_ELABEL_LEN + 1, desc, FRU_MAX_ELABEL_LEN);
    if (strncpy_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, strncpy_fun_ret);
    }
    GVariant *property_val = g_variant_new_string(property_buf);
    gint32 ret = dfl_set_property_value(handle, property_name, property_val, DF_NONE);
    g_free(property_buf);
    g_variant_unref(property_val);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call dfl_set_property_value(%s, %s) failed, error = %d.",
            dfl_get_object_name(handle), property_name, desc, ret);
    }
}


LOCAL gint32 __get_fru_area_multirecord(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, guint32 *max_length)
{
    gint32 ret = __check_field_addr_input_param(fru_priv, addr, field_size, field_data_size);
    if (ret != RET_OK) {
        return ret;
    }
    if (fru_priv->fru_info == NULL) {
        debug_log(DLOG_ERROR, "[%s] Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }
    *addr = &(fru_priv->elabel_info->multi_record);
    *field_size = sizeof(fru_priv->elabel_info->multi_record);
    *field_data_size = sizeof(fru_priv->elabel_info->multi_record);
    if (max_length != NULL) {
        *max_length = sizeof(fru_priv->elabel_info->multi_record);
    }
    return RET_OK;
}


gint32 lookup_operator_field_addr_size(FRU_PRIV_PROPERTY_S *fru_priv, guint8 area, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, guint32 *max_length)
{
    gint32 ret = __check_field_addr_input_param(fru_priv, addr, field_size, field_data_size);
    if (ret != RET_OK) {
        return ret;
    }

    size_t fru_area_info_count = sizeof(fru_area_info) / sizeof(FRU_AREA_INFO);
    LOOKUP_OPERATOR_FIELD_ADDR_FUN lookup_temp_field_addr = NULL;
    for (size_t i = 0; i < fru_area_info_count; i++) {
        if (fru_area_info[i].fru_arer_info_type == area) {
            lookup_temp_field_addr = fru_area_info[i].lookup_field_addr;
            break;
        }
    }

    ret = RET_ERR;
    if (lookup_temp_field_addr != NULL) {
        ret = lookup_temp_field_addr(fru_priv, field, addr, field_size, field_data_size, max_length);
    }
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] lookup field addr fail.", __FUNCTION__);
        return ret;
    }

    guchar soft_ware_type = MGMT_SOFTWARE_TYPE_UNKNOW;
    ret = dal_get_software_type(&soft_ware_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]dal_get_software_type fail.", __FUNCTION__);
        return ret;
    }

    if (soft_ware_type == MGMT_SOFTWARE_TYPE_EM || dal_check_if_vsmm_supported()) {
        __lookup_hmm_operator_field_addr_size(fru_priv, area, field, addr, field_size, field_data_size, max_length);
    }

    return ret;
}


gint32 check_elabel_length(guint32 field_data_size, guint8 offset, guint32 cur_len, guint32 last_len, guint8 req_len,
    guint32 max_length)
{
    
    if ((offset > last_len) || (req_len != cur_len + sizeof(DFT_WRITE_ELABEL_REQ_S) - 1)) {
        return RET_ERR;
    }

    
    if ((cur_len > max_length) || (offset + cur_len > max_length)) {
        return RET_ERR;
    }

    return RET_OK;
}


void check_and_fill_default_manufacturer(ELABEL_INFO_S *elabel_info)
{
    if (elabel_info == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return;
    }

    if (elabel_info->board.manufacture.len == 0) {
        elabel_info->board.manufacture.len = (guint16)strlen(DEFAULT_MFG_HUAWEI);
        (void)strncpy_s((gchar *)(elabel_info->board.manufacture.data), sizeof((elabel_info->board.manufacture.data)),
            DEFAULT_MFG_HUAWEI, strlen(DEFAULT_MFG_HUAWEI));
    }

    if (elabel_info->product.manufacture.len == 0) {
        elabel_info->product.manufacture.len = (guint16)strlen(DEFAULT_MFG_HUAWEI);
        (void)strncpy_s((gchar *)(elabel_info->product.manufacture.data),
            sizeof((elabel_info->product.manufacture.data)), DEFAULT_MFG_HUAWEI, strlen(DEFAULT_MFG_HUAWEI));
    }

    return;
}


LOCAL gboolean __judge_para(FRU_PRIV_PROPERTY_S *fru_priv, guint8 **label, guint32 *len)
{
    if (fru_priv == NULL || label == NULL || len == NULL) {
        return FALSE;
    }

    return TRUE;
}


LOCAL gboolean __is_extend_area(guint8 area)
{
    return ((area == FRU_AREA_EXTENDELABLE) || (area == FRU_INTERNAL_EXTEND_AREA) ||
        (area == FRU_CHASSIS_EXTEND_AREA) || (area == FRU_PRODUCT_EXTEND_AREA));
}


LOCAL gint32 fill_label_with_property(OBJ_HANDLE elabel_handle, guint8 **label, guint32 *len)
{
    gchar name_value[NAME_LEN + 1] = {0};
    guint32 name_len;
    gint32 ret =
        dal_get_property_value_string(elabel_handle, PROPERTY_ELABEL_BOARD_PRO_NAME, name_value, sizeof(name_value));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "failed to get BoardProductName, ret=%d", ret);
        return RET_ERR;
    }
    name_len = strlen(name_value);
    if (name_len == 0 || name_len > NAME_LEN) {
        debug_log(DLOG_ERROR, "get length of BoardProductName error");
        return RET_ERR;
    }
    *label = (guint8 *)g_malloc0(name_len + 1);
    if (*label == NULL) {
        debug_log(DLOG_ERROR, "failed to g_malloc0");
        return RET_ERR;
    }
    *len = name_len;
    ret = memcpy_s(*label, name_len + 1, name_value, name_len);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "failed to memcpy_s, ret=%d", ret);
        g_free(*label);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gboolean is_speical_tianchi_productname_property(FRU_PRIV_PROPERTY_S *fru_priv, guint8 area, guint8 field)
{
    guint8 comp_type = 0xff;
    (void)dal_get_component_type_by_fruid(fru_priv->fru_property.fru_id, &comp_type);
    if (dal_match_product_id(PRODUCT_ID_TIANCHI) && comp_type == COMPONENT_TYPE_BOARD_CPU &&
        area == FRU_AREA_BOARDINFO && field == FRU_BOARD_PRODUCTNAME) {
        return TRUE;
    } else {
        return FALSE;
    }
}


gint32 get_label_buf_and_len(FRU_PRIV_PROPERTY_S *fru_priv, guint8 area, guint8 field, guint8 **label, guint32 *len)
{
    if (!__judge_para(fru_priv, label, len)) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return COMP_CODE_UNKNOWN;
    }
    
    gpointer operator_addr = NULL;
    guint32 field_size = 0;
    guint32 field_data_size = 0;
    gint32 ret;
    
    if (is_speical_tianchi_productname_property(fru_priv, area, field)) {
        ret = fill_label_with_property(fru_priv->fru_property.elabel_handle, label, len);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "fill label failed, area=%u, field=%u, ret=%d", area, field, ret);
            ret = COMP_CODE_UNKNOWN;
        }
        return ret;
    }
    ret = lookup_operator_field_addr_size(fru_priv, area, field, &operator_addr, &field_size, &field_data_size, NULL);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: parametter error: area = %d, field = %d, ret = %d", __FUNCTION__, area, field, ret);
        return COMP_CODE_OUTOF_RANGE;
    }
    if (area == FRU_AREA_MULTIRECORD) {
        ret = __get_multi_label_buf_and_len(fru_priv, label, len, field_size);
        return ret;
    }
    if (__is_extend_area(area)) {
        if (__get_extend_label_by_index((ELABEL_EXTEND_AREA_S *)operator_addr, field, label, len) != RET_OK) {
            return COMP_CODE_OUTOF_RANGE;
        }
        return RET_OK;
    }

    ELABEL_FIELD_S *field_addr = (ELABEL_FIELD_S *)operator_addr;
    *len = field_addr->len;

    if (field_addr->len == 0) {
        return COMP_CODE_SUCCESS;
    }
    if (field_data_size == 0) {
        return COMP_CODE_OUTOF_RANGE;
    }

    *label = (guint8 *)g_malloc0(field_data_size);
    if (*label == NULL) {
        return COMP_CODE_OUTOF_RANGE;
    }

    (void)memcpy_s(*label, field_data_size, field_addr->data, field_addr->len);

    return RET_OK;
}

void set_label_text(FRU_ELABEL_FORMAT_S *label, const guint8 *buf, guint16 *offset, gsize buf_size)
{
    if (*offset >= buf_size) {
        debug_log(DLOG_ERROR, "%s: read out of bounds, %u>=%" G_GSIZE_FORMAT, __FUNCTION__, *offset, buf_size);
        return;
    }

    gint32 ret = memmove_s(label, sizeof(FRU_ELABEL_FORMAT_S), &buf[(*offset)++], 1);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, ret);
    }

    guint8 len_tmp = (label->len > FRU_MAX_ELABEL_LEN) ? FRU_MAX_ELABEL_LEN : label->len;
    if (*offset + len_tmp > buf_size) {
        debug_log(DLOG_ERROR, "%s: read out of bounds, %u>%" G_GSIZE_FORMAT, __FUNCTION__, *offset + len_tmp, buf_size);
        return;
    }

    ret = memmove_s(label->content, FRU_MAX_ELABEL_LEN, &buf[*offset], len_tmp);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, ret);
    }
    *offset += label->len;
    label->len = len_tmp;
}

void set_elabel_chassis_property(FRU_PROPERTY_S *fru_prop, const FRU_INFO_S *fru_info)
{
    if (fru_info->has_chassis) {
        if (fru_info->chassis.chassis_type < sizeof(g_chassis_type_desc) / sizeof(gchar *)) {
            set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_CHASSIS_TYPE,
                g_chassis_type_desc[fru_info->chassis.chassis_type]);
        } else {
            debug_log(DLOG_ERROR, "set_elabel_property:chassis_type:%d out of buff!", fru_info->chassis.chassis_type);
        }
    } else {
        gchar chassis_type_desc_default[FRU_MAX_ELABEL_LEN] = {0};
        set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_CHASSIS_TYPE, chassis_type_desc_default);
    }

    set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_CHASSIS_PN,
        (const gchar *)fru_info->chassis.chassis_part_num.content);
    set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_CHASSIS_SN,
        (const gchar *)fru_info->chassis.chassis_serial_num.content);

    
    gchar extension[FRU_EXTENSON_LABEL_MAX_LEN + 1] = {0};
    if (fru_info->chassis.custom_info != NULL) {
        combine_extension_label(fru_info->chassis.custom_info, extension, sizeof(extension), NULL);
    }
    set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_CHASSIS_EL, extension);
}

void set_elabel_board_property(FRU_PROPERTY_S *fru_prop, const FRU_INFO_S *fru_info)
{
    gchar date_time[FRU_MAX_ELABEL_LEN] = {0};
    guint8 comp_type = 0xff;
    __calc_date_time(fru_info->board.mfg_date_time, date_time, sizeof(date_time));
    set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_BOARD_MFG_TIME, date_time);
    set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_BOARD_MFG_NAME,
        (const gchar *)fru_info->board.board_manufacturer.content);
    (void)dal_get_component_type_by_fruid(fru_prop->fru_id, &comp_type);
    
    if (!(dal_match_product_id(PRODUCT_ID_TIANCHI) && comp_type == COMPONENT_TYPE_BOARD_CPU &&
        fru_info->board.board_product_name.len == 0)) {
        set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_BOARD_PRO_NAME,
            (const gchar *)fru_info->board.board_product_name.content);
    }
    set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_BOARD_SN,
        (const gchar *)fru_info->board.board_serial_num.content);
    set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_BOARD_PN,
        (const gchar *)fru_info->board.board_part_num.content);
    set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_BOARD_ID,
        (const gchar *)fru_info->board.file_id.content);

    
    gchar extension[FRU_EXTENSON_LABEL_MAX_LEN + 1] = {0};
    if (fru_info->board.extension_label != NULL) {
        combine_extension_label(fru_info->board.extension_label, extension, sizeof(extension), NULL);
    }

    SET_EXTENSION_ELABEL_PROPERTY(fru_prop->elabel_handle, PROPERTY_ELABEL_BOARD_EL, extension);
}

void set_elabel_product_property(FRU_PROPERTY_S *fru_prop, const FRU_INFO_S *fru_info)
{
    set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_PRODUCT_SN,
        (const gchar *)fru_info->product.product_serial_num.content);
    set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_PRODUCT_NAME,
        (const gchar *)fru_info->product.product_name.content);
    set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_PRODUCT_MFG_NAME,
        (const gchar *)fru_info->product.product_manufacturer.content);
    set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_PRODUCT_PN,
        (const gchar *)fru_info->product.product_part_num.content);
    set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_PRODUCT_VERSION,
        (const gchar *)fru_info->product.product_version.content);
    set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_PRODUCT_ASSET_TAG,
        (const gchar *)fru_info->product.product_asset_tag.content);
    set_one_elabel_property(fru_prop->elabel_handle, PROPERTY_ELABEL_PRODUCT_ID,
        (const gchar *)fru_info->product.file_id.content);

    
    gchar extension[FRU_EXTENSON_LABEL_MAX_LEN + 1] = {0};
    if (fru_info->product.custom_info != NULL) {
        combine_extension_label(fru_info->product.custom_info, extension, sizeof(extension), NULL);
    }

    SET_EXTENSION_ELABEL_PROPERTY(fru_prop->elabel_handle, PROPERTY_ELABEL_PRODUCT_EL, extension);
}


gint32 add_one_extend_label(ELABEL_EXTEND_AREA_S *extend_label, guint8 *buf, guint16 length)
{
    if (extend_label == NULL || buf == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return COMP_CODE_UNKNOWN;
    }

    // According to the definition of ELABEL_EXTEND_AREA_S, all fields except data occupy 8 bytes.
    if (strlen((const gchar *)buf) + 1 + extend_label->len + 1 > FRU_EXTENSON_LABEL_MAX_LEN - 8) {
        debug_log(DLOG_ERROR, "%s: extend buf is too long, len = %zu.", __FUNCTION__,
            strlen((const gchar *)buf) + extend_label->len + 2);
        return COMP_CODE_OUTOF_RANGE;
    }

    if (extend_label->num >= MAX_EXTERN_ELABEL_NUM) {
        debug_log(DLOG_ERROR, "%s: extend num is larger than 10, num = %d.", __FUNCTION__, extend_label->num);
        return COMP_CODE_OUTOF_RANGE;
    }

    if (extend_label->len == 0) {
        // According to the definition of ELABEL_EXTEND_AREA_S, all fields except data occupy 8 bytes.
        if (extend_label->len + length > FRU_EXTERN_LABEL_AREA_LEN - 8) {
            debug_log(DLOG_ERROR, "the extend elable len is wrong. oringal len: %d, add len: %d", extend_label->len,
                length);
            return COMP_CODE_OUTOF_RANGE;
        }

        extend_label->len += snprintf_truncated_s((gchar *)(extend_label->data + extend_label->len),
            sizeof(extend_label->data) - extend_label->len, "%s", buf);
    } else {
        // According to the definition of ELABEL_EXTEND_AREA_S, all fields except data occupy 8 bytes.
        if (extend_label->len + length + 1 > FRU_EXTERN_LABEL_AREA_LEN - 8) {
            return COMP_CODE_OUTOF_RANGE;
        }

        extend_label->len += snprintf_truncated_s((gchar *)(extend_label->data + extend_label->len),
            sizeof(extend_label->data) - extend_label->len, ";%s", buf);
    }

    extend_label->num++;
    extend_label->valid_flag = TRUE;
    extend_label->crc =
        arith_make_crc_checksum(0, (const guchar *)&extend_label->len, extend_label->len + FRU_EXTERN_LABEL_PREFIX_LEN);

    return COMP_CODE_SUCCESS;
}


gint32 search_equal_symbol_1st_postion(gchar **p, gchar **p1, guint8 *input)
{
    *p = g_strstr_len((const gchar *)input, FRU_MAX_ELABEL_LEN, "=");
    *p1 = g_strstr_len((const gchar *)input, FRU_MAX_ELABEL_LEN, ":");
    if (((*p == NULL) || (g_strstr_len(*p + 1, FRU_EXTENSON_LABEL_MAX_LEN, "=") != NULL) ||
        (g_strstr_len(*p + 1, FRU_EXTENSON_LABEL_MAX_LEN, ":") != NULL)) &&
        ((*p1 == NULL) || (g_strstr_len(*p1 + 1, FRU_EXTENSON_LABEL_MAX_LEN, ":") != NULL) ||
        (g_strstr_len(*p1 + 1, FRU_EXTENSON_LABEL_MAX_LEN, "=") != NULL))) {
        debug_log(DLOG_ERROR, "%s: set elabel is %s.", __FUNCTION__, input);
        return COMP_CODE_UNKNOWN;
    }

    return RET_OK;
}

gint32 elabel_area_parse(const guint8 *databuf, guint16 *offset, GList **extension_label, gsize buf_size)
{
    FRU_ELABEL_FORMAT_S *fru_label = (FRU_ELABEL_FORMAT_S *)g_malloc0(sizeof(FRU_ELABEL_FORMAT_S));
    if (fru_label == NULL) {
        return RET_ERR;
    }

    
    errno_t safe_fun_ret = memcpy_s(fru_label, sizeof(FRU_ELABEL_FORMAT_S), &databuf[(*offset)++], 1);
    if (safe_fun_ret != EOK) {
        g_free(fru_label);
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    guint8 len = fru_label->len;
    guint8 type = fru_label->type;
    if ((gsize)(*offset + len - 1) >= buf_size) {
        debug_log(DLOG_ERROR, "%s: read out of bounds, %u>=%" G_GSIZE_FORMAT, __FUNCTION__, *offset + len - 1,
            buf_size);
        g_free(fru_label);
        return RET_ERR;
    }

    guint8 j = 0;
    for (guint8 i = 0; i < len; i++) {
        
        if ((databuf[*offset + i] <= 0x0f) || ((databuf[*offset + i] >= 0x80) && (databuf[*offset + i] <= 0x8f))) {
            continue;
        }
        fru_label->content[j++] = databuf[*offset + i];

        
        
        if (j < FRU_MAX_ELABEL_LEN) {
            continue;
        }

        
        if (len <= i + 1) {
            continue;
        }
        j = 0;

        fru_label->type = type;
        fru_label->len = FRU_MAX_ELABEL_LEN;
        *extension_label = g_list_append(*extension_label, fru_label);
        fru_label = (FRU_ELABEL_FORMAT_S *)g_malloc0(sizeof(FRU_ELABEL_FORMAT_S));
        if (fru_label == NULL) {
            return RET_ERR;
        }
    }

    fru_label->type = type;
    fru_label->len = j;
    *extension_label = g_list_append(*extension_label, fru_label);
    *offset += len;

    return RET_OK;
}

gboolean check_support_elabel(guint8 fru_type)
{
    guchar soft_ware_type = MGMT_SOFTWARE_TYPE_UNKNOW;
    gint32 ret = dal_get_software_type(&soft_ware_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s dal_get_software_type fail.", __FUNCTION__);
        return FALSE;
    }

    if (soft_ware_type != MGMT_SOFTWARE_TYPE_EM) {
        return TRUE;
    }

    guint8 as_status = ACTIVE_STATE;
    ret = get_board_active_state(&as_status);
    if (as_status == ACTIVE_STATE && ret == RET_OK) { 
        return TRUE;
    }

    if (fru_type == FRU_TYPE_BACKPLANE) { 
        debug_log(DLOG_INFO, "As status is standby or known, backplane can not process dft.");
        return FALSE;
    }

    return TRUE;
}


gint32 fru_elabel_manufacture_syn_hook(OBJ_HANDLE object_handle, gpointer user_data)
{
    gulong task_id;

    (void)vos_task_create(&task_id, "manufacture_syn", (TASK_ENTRY)__elabel_syn_manufacture, (void *)object_handle,
        DEFAULT_PRIORITY);

    return RET_OK;
}


void sync_elabel_to_fru(FRU_INFO_S *fru_info, ELABEL_INFO_S *elabel_info)
{
    g_fru_read_back_status = TRUE;

    if (fru_info == NULL || elabel_info == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return;
    }

    
    fru_info->has_internal = FALSE;
    fru_info->has_chassis = FALSE;
    fru_info->has_board = FALSE;
    fru_info->has_product = FALSE;
    fru_info->has_multi = FALSE;

    
    __sync_internal_to_fru(fru_info, elabel_info);

    
    __sync_chassis_to_fru(fru_info, elabel_info);

    
    __sync_board_to_fru(fru_info, elabel_info);

    
    __sync_product_to_fru(fru_info, elabel_info);

    return;
}


guint32 copy_extend_label(GList **extend_label, ELABEL_EXTEND_AREA_S *elabel_extend_info)
{
    if (extend_label == NULL || elabel_extend_info == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return 0;
    }
    
    if (*extend_label != NULL) {
        g_list_free_full(*extend_label, g_free);
        *extend_label = NULL;
    }

    
    guint8 block_len = FRU_MAX_ELABEL_LEN - 1;
    guint8 max_block_num = (FRU_EXTERN_LABEL_AREA_LEN - FRU_EXTERN_LABEL_AREA_HEAD_LEN) / block_len;
    gchar **split_str = g_strsplit((const gchar *)elabel_extend_info->data, ";", 0);
    if (split_str == NULL) {
        return 0;
    }
    guint32 i = 0;
    for (; i < MAX_EXTERN_ELABEL_NUM; i++) {
        if (split_str[i] == NULL) {
            break;
        }

        gchar *p = split_str[i];
        guint32 single_elabel_len = strlen(split_str[i]);
        guint32 block_num = single_elabel_len / block_len;
        guint32 rest_num = single_elabel_len % block_len;

        guint8 total_block_num = (rest_num != 0) ? (block_num + 1) : block_num;
        debug_log(DLOG_DEBUG,
            "[%s] i %d, str [%s], single_elabel_len %d, block_num %d, rest_num %d, total_block_num %d", __FUNCTION__, i,
            p, single_elabel_len, block_num, rest_num, total_block_num);

        
        guint8 j = 0;
        for (; j < block_num && j <= max_block_num; j++) {
            __gen_one_extend_label(extend_label, p, block_len, j, total_block_num == j + 1);
            p += block_len;
        }

        if (rest_num != 0) {
            __gen_one_extend_label(extend_label, p, rest_num, j, TRUE);
        }
    }

    g_strfreev(split_str);

    return i;
}


LOCAL gint32 __copy_single_multi_label(GList **multi_label, guint8 *single_multi_elabel)
{
    if (!multi_label || !single_multi_elabel) {
        debug_log(DLOG_ERROR, "[%s] Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }

    ELABEL_MULTIRECORD_SINGLE_S *single_record = (ELABEL_MULTIRECORD_SINGLE_S *)single_multi_elabel;
    errno_t safe_fun_ret;
    FRU_MULTI_AREA_S *fru_record = NULL;
    fru_record = (FRU_MULTI_AREA_S *)g_malloc0(sizeof(FRU_MULTI_AREA_S));
    if (fru_record == NULL) {
        debug_log(DLOG_ERROR, "%s: alloc memory failed.", __FUNCTION__);
        return RET_ERR;
    }
    safe_fun_ret =
        memmove_s(&fru_record->header, sizeof(fru_record->header), (guint8 *)single_record, SINGLE_MULTI_HEAD_LEN);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        g_free(fru_record);
        return RET_ERR;
    }
    safe_fun_ret = memmove_s(&fru_record->data, sizeof(fru_record->data),
        (guint8 *)single_record + SINGLE_MULTI_HEAD_LEN, single_record->len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        g_free(fru_record);
        return RET_ERR;
    }
    *multi_label = g_list_append(*multi_label, fru_record);
    return RET_OK;
}


LOCAL gint32 __copy_multi_label(GList **multi_label, guint8 *elabel_multi_info)
{
    if (!multi_label || !elabel_multi_info) {
        debug_log(DLOG_ERROR, "[%s] Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }
    if (*multi_label != NULL) {
        g_list_free_full(*multi_label, g_free);
        *multi_label = NULL;
    }
    ELABEL_MULTIRECORD_SINGLE_S *single_record = (ELABEL_MULTIRECORD_SINGLE_S *)elabel_multi_info;
    size_t len = 0;
    gint32 ret;
    size_t elabel_len = strlen((const gchar *)elabel_multi_info);
    while (!(single_record->end_flag & MASK_BIT7) && len < elabel_len) {
        ret = __copy_single_multi_label(multi_label, (guint8 *)single_record);
        if (ret != RET_OK) {
            return RET_ERR;
        }
        len += SINGLE_MULTI_HEAD_LEN + single_record->len;
        single_record = (ELABEL_MULTIRECORD_SINGLE_S *)(elabel_multi_info + len);
    }
    if (single_record->end_flag & MASK_BIT7) {
        ret = __copy_single_multi_label(multi_label, (guint8 *)single_record);
        debug_log(DLOG_DEBUG, "%s: copy last part,ret=%d.", __FUNCTION__, ret);
        GList *tmp_ptr = NULL;
        for (tmp_ptr = *multi_label; tmp_ptr; tmp_ptr = g_list_next(tmp_ptr)) {
            single_record = (ELABEL_MULTIRECORD_SINGLE_S *)tmp_ptr->data;
            debug_log(DLOG_DEBUG, "%s: multi data=%02x %02x:%02x:%02x:%02x-%s.", __FUNCTION__, single_record->rec_type,
                single_record->end_flag, single_record->len, single_record->rec_crc, single_record->head_crc,
                single_record->data);
        }
        return ret;
    }
    debug_log(DLOG_ERROR, "%s: msg format error.", __FUNCTION__);
    return RET_ERR;
}

gboolean is_open_fru_read_back(void)
{
    return g_fru_read_back_status;
}

void open_fru_read_back(void)
{
    g_fru_read_back_status = TRUE;
}