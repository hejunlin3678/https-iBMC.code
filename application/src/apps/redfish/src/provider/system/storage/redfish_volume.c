
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "pme_app/uip/uip.h"
#include "pme_app/smlib/sml_errcodes.h"
#include "pme_app/smlib/sml_base.h"

#include "redfish_http.h"
#include "redfish_message.h"
#include "redfish_class.h"
#include "redfish_provider_resource.h"
#include "redfish_provider.h"

#include <pthread.h>

typedef enum {
    PARAM_OK = 0,
    PARAM_UNNECESSARY,
    PARAM_MISSING,
} PARAM_CHECK_RESULTS;

typedef struct {
    const gchar *str;
    guint8 value;
} RAID_LEVEL_MAP;


LOCAL gint32 get_json_obj(json_object *parent_jso, const gchar *prop_name, const gchar *prop_path,
    json_object **obj_jso, json_object *o_message_array_jso)
{
    json_bool bool_jso;
    json_object *message_info_jso = NULL;

    

    bool_jso = json_object_object_get_ex(parent_jso, prop_name, obj_jso);
    return_val_do_info_if_fail(bool_jso, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s %d: get property %s error", __FUNCTION__, __LINE__, prop_name);
        (void)create_message_info(MSGID_PROP_MISSING, prop_path, &message_info_jso, prop_path);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));

    return VOS_OK;
}


LOCAL gint32 get_uint_value_from_json(json_object *obj_jso, const char *prop_name, void *val, gint32 int_size,
    json_object *o_message_array_jso)
{
    gint32 int32_val = 0;
    gint64 int64_val = 0;
    json_object *err_jso = NULL;
    gchar str_value[MAX_VOL_STR_LEN] = {0};

    

    return_val_do_info_if_expr(json_object_is_type(obj_jso, json_type_double), HTTP_BAD_REQUEST,
        (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%lf", json_object_get_double(obj_jso));
        (void)create_message_info(MSGID_PROP_TYPE_ERR, prop_name, &err_jso, str_value, prop_name);
        (void)json_object_array_add(o_message_array_jso, err_jso));

    return_val_do_info_if_expr(json_object_is_type(obj_jso, json_type_null), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_TYPE_ERR, prop_name, &err_jso, RF_VALUE_NULL, prop_name);
        (void)json_object_array_add(o_message_array_jso, err_jso));

    if (sizeof(guint8) == int_size) {
        int32_val = json_object_get_int(obj_jso);
        
        return_val_do_info_if_expr(int32_val < 0 || int32_val > 255, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_VALUE_OUT_OF_RANGE, NULL, &err_jso, prop_name);
            (void)json_object_array_add(o_message_array_jso, err_jso));

        *((guint8 *)val) = (guint8)int32_val;
    } else if (sizeof(guint32) == int_size) {
        int32_val = json_object_get_int(obj_jso);
        *((guint32 *)val) = (guint32)int32_val;
    } else if (sizeof(guint64) == int_size) {
        
        int64_val = json_object_get_int64(obj_jso);
        return_val_do_info_if_expr(int64_val < 0, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_VALUE_OUT_OF_RANGE, NULL, &err_jso, prop_name);
            (void)json_object_array_add(o_message_array_jso, err_jso));

        *((guint64 *)val) = (guint64)int64_val;
    }

    return VOS_OK;
}


LOCAL gint32 get_string_from_json(json_object *obj_jso, const char *prop_name, gchar *str_val, gint32 str_len,
    json_object *o_message_array_jso)
{
    int iRet = -1;
    gint32 ret = VOS_OK;
    const gchar *str = NULL;

    json_object *err_jso = NULL;

    

    if (TRUE == json_object_is_type(obj_jso, json_type_null)) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, prop_name, &err_jso, RF_VALUE_NULL, prop_name);
        json_object_array_add(o_message_array_jso, err_jso);

        ret = HTTP_BAD_REQUEST;
    } else {
        str = dal_json_object_get_str(obj_jso);
        if (NULL != str) {
            iRet = snprintf_s(str_val, str_len, str_len - 1, "%s", str);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        } else {
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &err_jso);
            ret = json_object_array_add(o_message_array_jso, err_jso);
            return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(err_jso);
                debug_log(DLOG_ERROR, "%s %d: json_object_array_add fail", __FUNCTION__, __LINE__));
            debug_log(DLOG_ERROR, "%s, %d: dal_json_object_get_str error.", __FUNCTION__, __LINE__);
            ret = HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    return ret;
}


LOCAL void format_disk_id_array(gchar *str_val, gint32 str_len, const guint8 *id_array, gint32 arr_len)
{
    int iRet;
    guint32 i;
    guint32 len;
    gchar tmp_buff[MAX_VOL_STR_LEN] = {0};
    

    len = (arr_len > MAX_PD_ID_NUM_IN_MSG ? MAX_PD_ID_NUM_IN_MSG : arr_len);

    for (i = 0; i < len; i++) {
        if (0 == i) {
            if (1 == len) {
                (void)snprintf_s(tmp_buff, MAX_VOL_STR_LEN, MAX_VOL_STR_LEN - 1, "%d", id_array[i]);
            } else {
                (void)snprintf_s(tmp_buff, MAX_VOL_STR_LEN, MAX_VOL_STR_LEN - 1, "%d, ", id_array[i]);
            }
        } else if (i == len - 1 && arr_len < MAX_PD_ID_NUM_IN_MSG) {
            iRet = snprintf_s(tmp_buff, MAX_VOL_STR_LEN, MAX_VOL_STR_LEN - 1, "%s%d", tmp_buff, id_array[i]);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        } else {
            iRet = snprintf_s(tmp_buff, MAX_VOL_STR_LEN, MAX_VOL_STR_LEN - 1, "%s%d, ", tmp_buff, id_array[i]);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        }
    }

    if (arr_len > MAX_PD_ID_NUM_IN_MSG) {
        iRet = snprintf_s(str_val, str_len, str_len - 1, "[%s...]", tmp_buff);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    } else {
        iRet = snprintf_s(str_val, str_len, str_len - 1, "[%s]", tmp_buff);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    }
}


LOCAL gint32 get_disk_slot_by_id(guint8 id, CREATE_INFO_S *create_info, guint8 *slot, guint16 *enclosure,
    json_object *o_message_array_jso)
{
    int iRet;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *err_jso = NULL;

    gchar str_value[MAX_VOL_STR_LEN] = {0};
    gchar str_value2[MAX_VOL_STR_LEN] = {0};
    guint8 presence = 0;

    iRet = snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_FWINV_OEM,
        RFPROP_FWINV_HUAWEI, RFPROP_VOLUME_DRIVES);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    format_disk_id_array(str_value2, sizeof(str_value2), create_info->disk_id_array, create_info->disk_num);

    
    ret = dal_get_specific_object_byte(CLASS_HDD_NAME, PROPERTY_HDD_ID, id, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PHY_DISK_ID_INVALID, str_value, &err_jso, str_value2, str_value);
        json_object_array_add(o_message_array_jso, err_jso);
        debug_log(DLOG_ERROR, "%s, %d: dal_get_specific_object_byte fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_HDD_PRESENCE, &presence);
    return_val_do_info_if_fail(VOS_OK == ret && 0 != presence, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PHY_DISK_ID_INVALID, str_value, &err_jso, str_value2, str_value);
        json_object_array_add(o_message_array_jso, err_jso);
        debug_log(DLOG_ERROR, "%s, %d: dal_get_property_value_byte fail.\n", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_HDD_SLOT_NUMBER, slot);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &err_jso);
        json_object_array_add(o_message_array_jso, err_jso);
        debug_log(DLOG_ERROR, "%s, %d: dal_get_property_value_byte fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_uint16(obj_handle, PROPERTY_HDD_ENCLOSURE_DEVICE_ID, enclosure);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &err_jso);
        json_object_array_add(o_message_array_jso, err_jso);
        debug_log(DLOG_ERROR, "%s, %d: dal_get_property_value_byte fail.\n", __FUNCTION__, __LINE__));
    
    return VOS_OK;
}


LOCAL gint32 is_create_volume_on_new_array(CREATE_INFO_S *create_info, json_object *o_message_array_jso)
{
    gint32 ret;
    guint32 i = 0;
    json_object *err_jso = NULL;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    GVariant *property_slot_value = NULL;
    guint8 slot_num = 0;
    guint8 disk_id;
    const guint8 *pds = NULL;
    guint32 pd_count = 0;
    guint16 array_id = 0;
    gsize parm_temp = 0;

    guint16 enclosure = 0xFFFF;
    GVariant *property_enclosure_value = NULL;
    const guint16 *pd_enclosures = NULL;
    const gchar *raid_obj_name = dfl_get_object_name(create_info->object_handle);
    gchar array_ref_raid_name[PROP_VAL_LENGTH] = {0};

    
    
    return_val_if_fail(1 == create_info->disk_num, VOS_OK);

    disk_id = create_info->disk_id_array[0];

    
    
    ret = get_disk_slot_by_id(disk_id, create_info, &slot_num, &enclosure, o_message_array_jso);
    
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = dfl_get_object_list(CLASS_DISK_ARRAY_NAME, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_OK,
        debug_log(DLOG_ERROR, "%s, %d: dfl_get_object_list fail.\n", __FUNCTION__, __LINE__));

    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        
        (void)memset_s(array_ref_raid_name, sizeof(array_ref_raid_name), 0, sizeof(array_ref_raid_name));
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_DISK_ARRAY_REF_RAID_CONTROLLER,
            array_ref_raid_name, sizeof(array_ref_raid_name));
        if (0 != g_strcmp0(raid_obj_name, array_ref_raid_name)) {
            continue;
        }
        

        ret = dal_get_property_value_uint16((OBJ_HANDLE)obj_node->data, PROPERTY_DISK_ARRAY_ID, &array_id);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &err_jso);
            json_object_array_add(o_message_array_jso, err_jso); g_slist_free(obj_list);
            debug_log(DLOG_ERROR, "%s, %d: get prop %s fail.\n", __FUNCTION__, __LINE__, PROPERTY_DISK_ARRAY_ID));

        ret = dfl_get_property_value((OBJ_HANDLE)obj_node->data, PROPERTY_DISK_ARRAY_REF_PD_ENCLOSURES,
            &property_enclosure_value);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &err_jso);
            json_object_array_add(o_message_array_jso, err_jso); g_slist_free(obj_list); debug_log(DLOG_ERROR,
            "%s, %d: get prop %s fail.\n", __FUNCTION__, __LINE__, PROPERTY_DISK_ARRAY_REF_PD_ENCLOSURES));

        
        pd_enclosures =
            (const guint16 *)g_variant_get_fixed_array(property_enclosure_value, &parm_temp, sizeof(guint16));
        pd_count = (guint32)parm_temp;
        check_fail_do_continue(NULL != pd_enclosures, g_variant_unref(property_enclosure_value);
            property_enclosure_value = NULL);

        ret =
            dfl_get_property_value((OBJ_HANDLE)obj_node->data, PROPERTY_DISK_ARRAY_REF_PD_SLOTS, &property_slot_value);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &err_jso);
            json_object_array_add(o_message_array_jso, err_jso); g_slist_free(obj_list);
            g_variant_unref(property_enclosure_value); debug_log(DLOG_ERROR, "%s, %d: get prop %s fail.\n",
            __FUNCTION__, __LINE__, PROPERTY_DISK_ARRAY_REF_PD_SLOTS));

        
        pds = (const guint8 *)g_variant_get_fixed_array(property_slot_value, &parm_temp, sizeof(guint8));
        pd_count = (guint32)parm_temp;
        check_fail_do_continue(NULL != pds, g_variant_unref(property_enclosure_value);
            g_variant_unref(property_slot_value); property_slot_value = NULL);

        for (i = 0; i < pd_count; i++) {
            
            return_val_do_info_if_expr((pds[i] == slot_num) && (pd_enclosures[i] == enclosure), VOS_OK,
                create_info->create_type = ADD_LD_ON_EXIST_ARRAY;
                create_info->array_id = array_id; 
                g_variant_unref(property_enclosure_value); g_variant_unref(property_slot_value);
                g_slist_free(obj_list));
        }

        g_variant_unref(property_slot_value);
        property_slot_value = NULL;
        g_variant_unref(property_enclosure_value);
        property_enclosure_value = NULL;
    }

    
    g_slist_free(obj_list);

    return VOS_OK;
}


LOCAL void generate_necessity_check_result_json(guint8 check_result, const gchar *str_value,
    json_object *o_message_array_jso)
{
    json_object *message_info_jso = NULL;

    if (check_result == PARAM_UNNECESSARY) {
        (void)create_message_info(MSGID_VOLUME_NUNECESSARY_PROP, NULL, &message_info_jso, str_value);
    } else if (check_result == PARAM_MISSING) {
        (void)create_message_info(MSGID_PROP_MISSING, str_value, &message_info_jso, str_value);
    } else {
        return;
    }

    (void)json_object_array_add(o_message_array_jso, message_info_jso);
    return;
}


LOCAL guint8 check_raid_level_necessity(CREATE_INFO_S *create_info, json_bool bool_ret)
{
    if (dal_test_controller_vendor(create_info->type_id, VENDER_PMC)) {  
        if (!bool_ret) {
            return PARAM_MISSING;
        }
    } else if (create_info->create_type == ADD_LD_ON_EXIST_ARRAY) {  
        if (bool_ret) {
            return PARAM_UNNECESSARY;
        }
    } else {
        if (!bool_ret) {      
            return PARAM_MISSING;
        }
    }

    return PARAM_OK;
}

LOCAL guint8 parse_raid_level(const gchar* raidlevel_str)
{
    RAID_LEVEL_MAP level_map[] = {
        {RAID_LEVEL_STR_0, RAID_LEVEL_0},
        {RAID_LEVEL_STR_1, RAID_LEVEL_1},
        {RAID_LEVEL_STR_5, RAID_LEVEL_5},
        {RAID_LEVEL_STR_6, RAID_LEVEL_6},
        {RAID_LEVEL_STR_10, RAID_LEVEL_10},
        {RAID_LEVEL_STR_50, RAID_LEVEL_50},
        {RAID_LEVEL_STR_60, RAID_LEVEL_60},
        {RAID_LEVEL_STR_1ADM, RAID_LEVEL_1ADM},
        {RAID_LEVEL_STR_10ADM, RAID_LEVEL_10ADM},
        {RAID_LEVEL_STR_1TRIPLE, RAID_LEVEL_1TRIPLE},
        {RAID_LEVEL_STR_10TRIPLE, RAID_LEVEL_10TRIPLE},
    };

    for (guint8 i = 0; i < G_N_ELEMENTS(level_map); i++) {
        if (g_strcmp0(level_map[i].str, raidlevel_str) == 0) {
            return level_map[i].value;
        }
    }

    return RAID_LEVEL_UNKNOWN;
}


LOCAL gint32 check_cachecade_raid_level(guint8 raidlevel, CREATE_INFO_S *create_info)
{
    if (create_info->create_type != CREATE_LD_AS_CACHECADE) {
        return RET_OK;
    }

    
    if (dal_test_controller_vendor(create_info->type_id, VENDER_PMC)) {
        if (raidlevel != RAID_LEVEL_0 && raidlevel != RAID_LEVEL_1 && raidlevel != RAID_LEVEL_5 &&
            raidlevel != RAID_LEVEL_10) {
            return RET_ERR;
        }
    } else {
        if (raidlevel != RAID_LEVEL_0 && raidlevel != RAID_LEVEL_1) {
            return RET_ERR;
        }
    }

    return RET_OK;
}


LOCAL gint32 get_raid_level(json_object *huawei_jso, CREATE_INFO_S *create_info, json_object *o_message_array_jso)
{
    gint32 ret = VOS_ERR;
    gchar raidlevel_str[MAX_VOL_STR_LEN] = {0};
    guint8 raidlevel = 0xFF;
    json_object *val_jso = NULL;
    json_object *message_info_jso = NULL;
    gchar str_value[MAX_VOL_STR_LEN] = {0};

    
    // sizeof(str_value)大小为128，大于格式化字符串长度，无需判断返回值
    (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_COMMON_OEM,
        RFPROP_COMMON_HUAWEI, RFPROP_VOLUME_RAID_LEVEL);
    json_bool bool_ret = json_object_object_get_ex(huawei_jso, RFPROP_VOLUME_RAID_LEVEL, &val_jso);
    guint8 check_result = check_raid_level_necessity(create_info, bool_ret);
    if (check_result != PARAM_OK) {
        generate_necessity_check_result_json(check_result, str_value, o_message_array_jso);
        return HTTP_BAD_REQUEST;
    }

    if (bool_ret) {
        ret = get_string_from_json(val_jso, str_value, raidlevel_str, MAX_VOL_STR_LEN, o_message_array_jso);
        return_val_if_fail(VOS_OK == ret, ret);
        int iRet = snprintf_s(create_info->str_raid_level, sizeof(create_info->str_raid_level),
            sizeof(create_info->str_raid_level) - 1, "%s", raidlevel_str);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

        
        raidlevel = parse_raid_level(raidlevel_str);
        if (raidlevel == RAID_LEVEL_UNKNOWN) {
            debug_log(DLOG_ERROR, "%s %d : %s: %s is fail", __FUNCTION__, __LINE__, RFPROP_VOLUME_RAID_LEVEL,
                raidlevel_str);
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, str_value, &message_info_jso, raidlevel_str, str_value);
            ret = json_object_array_add(o_message_array_jso, message_info_jso);
            return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
                (void)json_object_put(message_info_jso);
                debug_log(DLOG_ERROR, "%s %d: json_object_array_add fail", __FUNCTION__, __LINE__));

            return HTTP_BAD_REQUEST;
        }

        if (check_cachecade_raid_level(raidlevel, create_info) != RET_OK) {
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, str_value, &message_info_jso, raidlevel_str, str_value);
            json_object_array_add(o_message_array_jso, message_info_jso);
            return HTTP_BAD_REQUEST;
        }
        create_info->raid_level = raidlevel;
    }

    return VOS_OK;
}


LOCAL gint32 get_read_policy(json_object *huawei_jso, CREATE_INFO_S *create_info, json_object *o_message_array_jso)
{
    gint32 ret = VOS_ERR;
    json_bool bool_ret;
    guint8 o_readpolicy;
    gchar readpolicy_ptr[MAX_VOL_STR_LEN] = {0};
    gchar str_value[MAX_VOL_STR_LEN] = {0};
    json_object *val_jso = NULL;
    json_object *message_info_jso = NULL;

    
    bool_ret = json_object_object_get_ex(huawei_jso, RFPROP_VOLUME_DEFAULT_READ_POLICY, &val_jso);
    
    o_readpolicy = READ_POLICY_DEF;

    if (bool_ret) {
        // sizeof(str_value)大小为128，大于格式化字符串长度，无需判断返回值
        (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_COMMON_OEM,
            RFPROP_COMMON_HUAWEI, RFPROP_VOLUME_DEFAULT_READ_POLICY);

        
        if (create_info->create_type == CREATE_LD_AS_CACHECADE ||
            dal_test_controller_vendor(create_info->type_id, VENDER_PMC)) {
            generate_necessity_check_result_json(PARAM_UNNECESSARY, str_value, o_message_array_jso);
            return HTTP_BAD_REQUEST;
        }

        ret = get_string_from_json(val_jso, str_value, readpolicy_ptr, MAX_VOL_STR_LEN, o_message_array_jso);
        return_val_if_fail(VOS_OK == ret, ret);

        if (g_strcmp0(readpolicy_ptr, LD_READ_NO) == 0) {
            o_readpolicy = READ_POLICY_0;
        } else if (g_strcmp0(readpolicy_ptr, LD_READ_AHEAD) == 0) {
            o_readpolicy = READ_POLICY_1;
        } else {
            debug_log(DLOG_ERROR, "%s %d :%s: %s is fail", __FUNCTION__, __LINE__, RFPROP_VOLUME_DEFAULT_READ_POLICY,
                readpolicy_ptr);
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, str_value, &message_info_jso, readpolicy_ptr, str_value);
            ret = json_object_array_add(o_message_array_jso, message_info_jso);
            if (VOS_OK != ret) {
                debug_log(DLOG_ERROR, "%s %d: json_object_array_add fail", __FUNCTION__, __LINE__);
                (void)json_object_put(message_info_jso);
            }

            return HTTP_BAD_REQUEST;
        }
    }

    create_info->read_policy = o_readpolicy;
    return VOS_OK;
}


LOCAL guint8 check_write_policy_necessity(CREATE_INFO_S *create_info, json_bool bool_ret)
{
    if (dal_test_controller_vendor(create_info->type_id, VENDER_PMC)) {
        
        if (create_info->create_type == CREATE_LD_AS_CACHECADE) {
            if (!bool_ret) {
                return PARAM_MISSING;
            }
        } else {
            
            if (bool_ret) {
                return PARAM_UNNECESSARY;
            }
        }
    }

    return PARAM_OK;
}


LOCAL gint32 get_write_policy(json_object *huawei_jso, CREATE_INFO_S *create_info, json_object *o_message_array_jso)
{
    int iRet = -1;
    gint32 ret = VOS_ERR;
    json_bool bool_ret;
    guint8 o_writepolicy;
    gchar writepolicy_ptr[MAX_VOL_STR_LEN] = {0};
    gchar str_value[MAX_VOL_STR_LEN] = {0};
    json_object *val_jso = NULL;
    json_object *message_info_jso = NULL;

    
    bool_ret = json_object_object_get_ex(huawei_jso, RFPROP_VOLUME_DEFAULT_WRITE_POLICY, &val_jso);
    
    o_writepolicy = WRITE_POLICY_DEF;

    iRet = snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_COMMON_OEM,
        RFPROP_COMMON_HUAWEI, RFPROP_VOLUME_DEFAULT_WRITE_POLICY);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    guint8 check_result = check_write_policy_necessity(create_info, bool_ret);
    if (check_result != PARAM_OK) {
        generate_necessity_check_result_json(check_result, str_value, o_message_array_jso);
        return HTTP_BAD_REQUEST;
    }

    if (bool_ret) {
        ret = get_string_from_json(val_jso, str_value, writepolicy_ptr, MAX_VOL_STR_LEN, o_message_array_jso);
        return_val_if_fail(VOS_OK == ret, ret);

        if (0 == g_strcmp0(writepolicy_ptr, LD_WRITE_THROUGH)) {
            o_writepolicy = WRITE_POLICY_0;
        } else if (0 == g_strcmp0(writepolicy_ptr, LD_WRITE_BACKBBU)) {
            o_writepolicy = WRITE_POLICY_1;
        }
        
        else if (0 == g_strcmp0(writepolicy_ptr, LD_WRITE_BACK) &&
            (CREATE_LD_AS_CACHECADE != create_info->create_type)) {
            
            o_writepolicy = WRITE_POLICY_2;
        } else {
            debug_log(DLOG_ERROR, "%s %d : %s: %s is fail", __FUNCTION__, __LINE__, RFPROP_VOLUME_DEFAULT_WRITE_POLICY,
                writepolicy_ptr);
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, str_value, &message_info_jso, writepolicy_ptr, str_value);
            ret = json_object_array_add(o_message_array_jso, message_info_jso);
            if (VOS_OK != ret) {
                debug_log(DLOG_ERROR, "%s %d: json_object_array_add fail", __FUNCTION__, __LINE__);
                (void)json_object_put(message_info_jso);
            }

            return HTTP_BAD_REQUEST;
        }
    }

    create_info->write_policy = o_writepolicy;
    return VOS_OK;
}


LOCAL gint32 get_io_policy(json_object *huawei_jso, CREATE_INFO_S *create_info, json_object *o_message_array_jso)
{
    gint32 ret = VOS_ERR;
    json_bool bool_ret;
    guint8 o_iopolicy;
    gchar iopolicy_ptr[MAX_VOL_STR_LEN] = {0};
    gchar str_value[MAX_VOL_STR_LEN] = {0};
    json_object *val_jso = NULL;
    json_object *message_info_jso = NULL;

    
    bool_ret = json_object_object_get_ex(huawei_jso, RFPROP_VOLUME_DEFAULT_CACHE_POLICY, &val_jso);
    
    o_iopolicy = IO_POLICY_DEF;

    if (bool_ret) {
        (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_COMMON_OEM,
            RFPROP_COMMON_HUAWEI, RFPROP_VOLUME_DEFAULT_CACHE_POLICY);

        
        if (create_info->create_type == CREATE_LD_AS_CACHECADE ||
            dal_test_controller_vendor(create_info->type_id, VENDER_PMC)) {
            generate_necessity_check_result_json(PARAM_UNNECESSARY, str_value, o_message_array_jso);
            return HTTP_BAD_REQUEST;
        }

        ret = get_string_from_json(val_jso, str_value, iopolicy_ptr, MAX_VOL_STR_LEN, o_message_array_jso);
        return_val_if_fail(VOS_OK == ret, ret);

        if (0 == g_strcmp0(iopolicy_ptr, LD_IO_CACHE)) {
            o_iopolicy = IO_POLICY_0;
        } else if (0 == g_strcmp0(iopolicy_ptr, LC_IO_DIRECT)) {
            o_iopolicy = IO_POLICY_1;
        } else {
            debug_log(DLOG_ERROR, "%s %d :%s: %s is fail", __FUNCTION__, __LINE__, RFPROP_VOLUME_DEFAULT_CACHE_POLICY,
                iopolicy_ptr);
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, str_value, &message_info_jso, iopolicy_ptr, str_value);
            ret = json_object_array_add(o_message_array_jso, message_info_jso);
            if (VOS_OK != ret) {
                debug_log(DLOG_ERROR, "%s %d: json_object_array_add fail", __FUNCTION__, __LINE__);
                (void)json_object_put(message_info_jso);
            }

            return HTTP_BAD_REQUEST;
        }
    }

    create_info->io_policy = o_iopolicy;
    return VOS_OK;
}


LOCAL gint32 get_cache_policy(json_object *huawei_jso, CREATE_INFO_S *create_info, json_object *o_message_array_jso)
{
    gint32 ret = VOS_ERR;
    json_bool bool_ret;
    guint8 o_cachepolicy;
    gchar cachepolicy_ptr[MAX_VOL_STR_LEN] = {0};
    gchar str_value[MAX_VOL_STR_LEN] = {0};
    json_object *val_jso = NULL;
    json_object *message_info_jso = NULL;

    
    bool_ret = json_object_object_get_ex(huawei_jso, RFPROP_VOLUME_DRIVE_CACHE_POLICY, &val_jso);
    
    o_cachepolicy = CACHE_POLICY_DEF;

    if (bool_ret) {
        (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_COMMON_OEM,
            RFPROP_COMMON_HUAWEI, RFPROP_VOLUME_DRIVE_CACHE_POLICY);

        
        if (create_info->create_type == CREATE_LD_AS_CACHECADE ||
            dal_test_controller_vendor(create_info->type_id, VENDER_PMC)) {
            generate_necessity_check_result_json(PARAM_UNNECESSARY, str_value, o_message_array_jso);
            return HTTP_BAD_REQUEST;
        }

        ret = get_string_from_json(val_jso, str_value, cachepolicy_ptr, MAX_VOL_STR_LEN, o_message_array_jso);
        return_val_if_fail(VOS_OK == ret, ret);

        if (0 == g_strcmp0(cachepolicy_ptr, LD_DCACHE_DEF)) {
            o_cachepolicy = CACHE_POLICY_0;
        } else if (0 == g_strcmp0(cachepolicy_ptr, LD_DCACHE_ENABLE)) {
            o_cachepolicy = CACHE_POLICY_1;
        } else if (0 == g_strcmp0(cachepolicy_ptr, LD_DCACHE_DISABLE)) {
            o_cachepolicy = CACHE_POLICY_2;
        } else {
            debug_log(DLOG_ERROR, "%s %d :%s: %s is fail", __FUNCTION__, __LINE__, RFPROP_VOLUME_DRIVE_CACHE_POLICY,
                cachepolicy_ptr);
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, str_value, &message_info_jso, cachepolicy_ptr, str_value);
            ret = json_object_array_add(o_message_array_jso, message_info_jso);
            if (VOS_OK != ret) {
                debug_log(DLOG_ERROR, "%s %d: json_object_array_add fail", __FUNCTION__, __LINE__);
                (void)json_object_put(message_info_jso);
            }

            return HTTP_BAD_REQUEST;
        }
    }

    create_info->disk_cache_policy = o_cachepolicy;
    return VOS_OK;
}


LOCAL gint32 get_access_policy(json_object *huawei_jso, CREATE_INFO_S *create_info, json_object *o_message_array_jso)
{
    gint32 ret = VOS_ERR;
    json_bool bool_ret;
    guint8 o_accpolicy;
    gchar accpolicy_ptr[MAX_VOL_STR_LEN] = {0};
    gchar str_value[MAX_VOL_STR_LEN] = {0};
    json_object *val_jso = NULL;
    json_object *message_info_jso = NULL;

    
    bool_ret = json_object_object_get_ex(huawei_jso, RFPROP_VOLUME_ACCESS_POLICY, &val_jso);
    
    o_accpolicy = ACCESS_POLICY_DEF;

    if (bool_ret) {
        (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_COMMON_OEM,
            RFPROP_COMMON_HUAWEI, RFPROP_VOLUME_ACCESS_POLICY);

        
        if (create_info->create_type == CREATE_LD_AS_CACHECADE ||
            dal_test_controller_vendor(create_info->type_id, VENDER_PMC)) {
            generate_necessity_check_result_json(PARAM_UNNECESSARY, str_value, o_message_array_jso);
            return HTTP_BAD_REQUEST;
        }

        ret = get_string_from_json(val_jso, str_value, accpolicy_ptr, MAX_VOL_STR_LEN, o_message_array_jso);
        return_val_if_fail(VOS_OK == ret, ret);

        if (0 == g_strcmp0(accpolicy_ptr, LD_ACC_RW)) {
            o_accpolicy = ACCESS_POLICY_0;
        } else if (0 == g_strcmp0(accpolicy_ptr, LD_ACC_READ)) {
            o_accpolicy = ACCESS_POLICY_1;
        } else if (0 == g_strcmp0(accpolicy_ptr, LD_ACC_BLOCK)) {
            o_accpolicy = ACCESS_POLICY_2;
        } else {
            debug_log(DLOG_ERROR, "%s %d :%s: %s is fail", __FUNCTION__, __LINE__, RFPROP_VOLUME_ACCESS_POLICY,
                accpolicy_ptr);
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, str_value, &message_info_jso, accpolicy_ptr, str_value);
            ret = json_object_array_add(o_message_array_jso, message_info_jso);
            if (VOS_OK != ret) {
                debug_log(DLOG_ERROR, "%s %d: json_object_array_add fail", __FUNCTION__, __LINE__);
                (void)json_object_put(message_info_jso);
            }

            return HTTP_BAD_REQUEST;
        }
    }

    create_info->access_policy = o_accpolicy;
    return VOS_OK;
}


LOCAL gint32 get_volume_name(json_object *huawei_jso, CREATE_INFO_S *create_info,
    json_object *o_message_array_jso)
{
    int iRet = -1;
    gint32 ret = VOS_ERR;
    json_bool bool_ret;
    json_object *val_jso = NULL;
    json_object *message_info_jso = NULL;
    gchar volumename_ptr[SML_LD_NAME_LENGTH] = {0};
    gchar str_value[MAX_VOL_STR_LEN] = {0};

    bool_ret = json_object_object_get_ex(huawei_jso, RFPROP_VOLUME_NAME, &val_jso);
    
    if (bool_ret) {
        (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_COMMON_OEM,
            RFPROP_COMMON_HUAWEI, RFPROP_VOLUME_NAME);

        
        if (create_info->create_type == CREATE_LD_AS_CACHECADE &&
            dal_test_controller_vendor(create_info->type_id, VENDER_PMC)) {
            generate_necessity_check_result_json(PARAM_UNNECESSARY, str_value, o_message_array_jso);
            return HTTP_BAD_REQUEST;
        }

        ret = get_string_from_json(val_jso, str_value, volumename_ptr, SML_LD_NAME_LENGTH, o_message_array_jso);
        return_val_if_fail(VOS_OK == ret, ret);

        size_t len = strlen(volumename_ptr);

        
        for (size_t i = 0; i < len; i++) {
            
            return_val_do_info_if_expr(volumename_ptr[i] < 0x20 || volumename_ptr[i] > 0x7E, HTTP_BAD_REQUEST,
                debug_log(DLOG_ERROR, "%s %d: %s not a valid character", __FUNCTION__, __LINE__, RFPROP_VOLUME_NAME);
                
                (void)create_message_info(MSGID_PROP_FORMAT_ERR, str_value, &message_info_jso, volumename_ptr,
                str_value);
                (void)json_object_array_add(o_message_array_jso, message_info_jso));
            
        }

        iRet = snprintf_s(create_info->volume_name, sizeof(create_info->volume_name),
            sizeof(create_info->volume_name) - 1, "%s", volumename_ptr);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    }

    // 默认值为空字符串
    return VOS_OK;
}


LOCAL gint32 get_disk_id_array(json_object *huawei_jso, CREATE_INFO_S *create_info, json_object *o_message_array_jso)
{
    gint32 ret;
    guint32 i;
    gchar str_value[MAX_VOL_STR_LEN] = {0};
    json_object *val_jso = NULL;
    json_object *item_json = NULL;
    json_object *message_info_jso = NULL;

    (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_COMMON_OEM,
        RFPROP_COMMON_HUAWEI, RFPROP_VOLUME_DRIVES);

    ret = get_json_obj(huawei_jso, RFPROP_VOLUME_DRIVES, str_value, &val_jso, o_message_array_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    create_info->disk_num = json_object_array_length(val_jso);
    if (create_info->disk_num == 0 || create_info->disk_num > MAX_PD_ID_NUM) {
        (void)create_message_info(MSGID_PHY_DISK_COUNT_ERR, str_value, &message_info_jso, "[]", str_value);
        json_object_array_add(o_message_array_jso, message_info_jso);
        return HTTP_BAD_REQUEST;
    }

    for (i = 0; i < create_info->disk_num; i++) {
        item_json = json_object_array_get_idx(val_jso, i);
        ret = get_uint_value_from_json(item_json, str_value, &(create_info->disk_id_array[i]), sizeof(guint8),
            o_message_array_jso);
        return_val_if_fail(VOS_OK == ret, ret);
    }

    return VOS_OK;
}


LOCAL guint8 check_span_num_necessity(CREATE_INFO_S *create_info, json_bool bool_ret)
{
    if (dal_test_controller_vendor(create_info->type_id, VENDER_PMC)) {
        
        if (create_info->create_type == CREATE_LD_AS_CACHECADE && bool_ret) {
            return PARAM_UNNECESSARY;
        }
    } else {
        
        if (create_info->create_type != CREATE_LD_ON_NEW_ARRAY && bool_ret) {
            return PARAM_UNNECESSARY;
        }
    }

    return PARAM_OK;
}


LOCAL void get_default_span_num(CREATE_INFO_S *create_info)
{
    guint8 min_disk_num;
    guint8 min_disk_num_per_span;

    switch (create_info->raid_level) {
        case RAID_LEVEL_0:
        case RAID_LEVEL_1:
        case RAID_LEVEL_5:
        case RAID_LEVEL_6:
        case RAID_LEVEL_1ADM:
        case RAID_LEVEL_1TRIPLE:
            create_info->span_num = SPAN_NUM_1;
            break;

        case RAID_LEVEL_10:
            
            if ((create_info->disk_num >= 4) && (create_info->disk_num % 2 == 0)) {
                
                create_info->span_num = create_info->disk_num / 2;
                break;
            }

            create_info->span_num = SPAN_NUM_2;
            break;

        case RAID_LEVEL_50:
        case RAID_LEVEL_10ADM:
        case RAID_LEVEL_10TRIPLE:
            
            if ((create_info->disk_num >= 6) && (create_info->disk_num % 3 == 0)) {
                
                create_info->span_num = create_info->disk_num / 3;
                break;
            }

            create_info->span_num = SPAN_NUM_2;
            break;

        case RAID_LEVEL_60:
            if (dal_test_controller_vendor(create_info->type_id, VENDER_PMC) ||
                create_info->type_id == LSI_3908_WITH_MR || create_info->type_id == LSI_3916_WITH_MR) {
                min_disk_num = 8;  // PMC卡和3908、3916的RAID 6至少4盘
                min_disk_num_per_span = 4;
            } else {
                min_disk_num = 6; // 博通卡的RAID 6至少3盘
                min_disk_num_per_span = 3;
            }

            if ((create_info->disk_num >= min_disk_num) && (create_info->disk_num % min_disk_num_per_span == 0)) {
                create_info->span_num = create_info->disk_num / min_disk_num_per_span;
                break;
            }

            create_info->span_num = SPAN_NUM_2;
            break;

        default:
            create_info->span_num = SPAN_NUM_1;
            break;
    }
    return;
}


LOCAL gint32 get_span_num(json_object *huawei_jso, CREATE_INFO_S *create_info, json_object *o_message_array_jso)
{
    gint32 ret;
    gchar str_value[MAX_VOL_STR_LEN] = {0};
    json_object *span_num_jso = NULL;
    json_bool bool_jso;

    (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_COMMON_OEM,
        RFPROP_COMMON_HUAWEI, RFPROP_VOLUME_SPAN_NUMBER);

    bool_jso = json_object_object_get_ex(huawei_jso, RFPROP_VOLUME_SPAN_NUMBER, &span_num_jso);

    guint8 check_result = check_span_num_necessity(create_info, bool_jso);
    if (check_result != PARAM_OK) {
        generate_necessity_check_result_json(check_result, str_value, o_message_array_jso);
        return HTTP_BAD_REQUEST;
    }

    
    if (!bool_jso) {
        get_default_span_num(create_info);
        return VOS_OK;
    }

    ret = get_uint_value_from_json(span_num_jso, str_value, &(create_info->span_num), sizeof(guint8),
        o_message_array_jso);

    return ret;
}

LOCAL gint32 __parse_strip_size(CREATE_INFO_S *create_info)
{
    switch (create_info->strip_size) {
        case STRIPE_SIZE_16K_SCHE:
            create_info->strip_size_as_para = STRIPE_SIZE_16K_BMC;
            return RET_OK;

        case STRIPE_SIZE_32K_SCHE:
            create_info->strip_size_as_para = STRIPE_SIZE_32K_BMC;
            return RET_OK;

        case STRIPE_SIZE_64K_SCHE:
            create_info->strip_size_as_para = STRIPE_SIZE_64K_BMC;
            return RET_OK;

        case STRIPE_SIZE_128K_SCHE:
            create_info->strip_size_as_para = STRIPE_SIZE_128K_BMC;
            return RET_OK;

        case STRIPE_SIZE_256K_SCHE:
            create_info->strip_size_as_para = STRIPE_SIZE_256K_BMC;
            return RET_OK;

        case STRIPE_SIZE_512K_SCHE:
            create_info->strip_size_as_para = STRIPE_SIZE_512K_BMC;
            return RET_OK;

        case STRIPE_SIZE_1M_SCHE:
            create_info->strip_size_as_para = STRIPE_SIZE_1M_BMC;
            return RET_OK;

        default:
            return RET_ERR;
    }
}


LOCAL gint32 get_stripe_size(json_object *body_jso_checked, CREATE_INFO_S *create_info,
    json_object *o_message_array_jso)
{
    gint32 ret = VOS_OK;
    json_bool bool_ret;
    gchar str_value[MAX_VOL_STR_LEN] = {0};
    gchar str_value2[MAX_VOL_STR_LEN] = {0};
    json_object *stripe_size_jso = NULL;
    json_object *err_jso = NULL;

    (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s", RFPROP_VOLUME_OPTIMUMIOSIZE);
    bool_ret = json_object_object_get_ex(body_jso_checked, RFPROP_VOLUME_OPTIMUMIOSIZE, &stripe_size_jso);

    create_info->strip_size_as_para = STRIPE_SIZE_DEFAULT_BMC;

    if (bool_ret) {
        
        
        return_val_do_info_if_expr(CREATE_LD_AS_CACHECADE == create_info->create_type, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_VOLUME_NUNECESSARY_PROP, NULL, &err_jso, str_value);
            (void)json_object_array_add(o_message_array_jso, err_jso));
        

        ret = get_uint_value_from_json(stripe_size_jso, str_value, &(create_info->strip_size), sizeof(guint32),
            o_message_array_jso);
        return_val_if_fail(VOS_OK == ret, ret);

        if (__parse_strip_size(create_info) != RET_OK) {
            (void)snprintf_s(str_value2, sizeof(str_value2), sizeof(str_value2) - 1, "%d", create_info->strip_size);
            (void)create_message_info(MSGID_STRIPE_SIZE_ERR, str_value, &err_jso, str_value2, str_value);
            ret = json_object_array_add(o_message_array_jso, err_jso);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: json_object_array_add fail", __FUNCTION__);
                (void)json_object_put(err_jso);
            }

            ret = HTTP_BAD_REQUEST;
        }
    }

    return ret;
}


LOCAL gint32 get_volume_capacity(json_object *body_jso_checked, CREATE_INFO_S *create_info,
    json_object *o_message_array_jso)
{
    gint32 ret = VOS_OK;
    json_bool bool_ret;
    guint64 o_volume_capacity;
    gchar str_value[MAX_VOL_STR_LEN] = {0};
    json_object *val_jso = NULL;

    (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s", RFPROP_VOLUME_CAPACITY);

    bool_ret = json_object_object_get_ex(body_jso_checked, RFPROP_VOLUME_CAPACITY, &val_jso);
    
    o_volume_capacity = CAPACITY_DEFAULT_VAL;

    if (bool_ret) {
        
        if (create_info->create_type == CREATE_LD_AS_CACHECADE &&
            dal_test_controller_vendor(create_info->type_id, VENDER_LSI)) {
            generate_necessity_check_result_json(PARAM_UNNECESSARY, str_value, o_message_array_jso);
            return HTTP_BAD_REQUEST;
        }

        ret = get_uint_value_from_json(val_jso, str_value, &o_volume_capacity, sizeof(guint64), o_message_array_jso);
        return_val_if_fail(VOS_OK == ret, ret);

        o_volume_capacity = o_volume_capacity / (1024 * 1024);
    }

    create_info->volume_capacity = (guint32)o_volume_capacity;
    return ret;
}

LOCAL guint8 parse_init_mode(const gchar* initmode_ptr)
{
    if (g_strcmp0(initmode_ptr, LD_INITMODE_UN) == 0) {
        return LD_INIT_STATE_NO_INIT;
    } else if (g_strcmp0(initmode_ptr, LD_INITMODE_QUICK) == 0) {
        return LD_INIT_STATE_QUICK_INIT;
    } else if (g_strcmp0(initmode_ptr, LD_INITMODE_FULL) == 0) {
        return LD_INIT_STATE_FULL_INIT;
    } else if (g_strcmp0(initmode_ptr, LD_INITMODE_RPI) == 0) {
        return LD_INIT_STATE_RPI;
    } else if (g_strcmp0(initmode_ptr, LD_INITMODE_OPO) == 0) {
        return LD_INIT_STATE_OPO;
    } else if (g_strcmp0(initmode_ptr, LD_INITMODE_FRONT) == 0) {
        return LD_INIT_STATE_FRONT;
    } else if (g_strcmp0(initmode_ptr, LD_INITMODE_BACKGROUND) == 0) {
        return LD_INIT_STATE_BACKGROUND;
    }

    return LD_INIT_STATE_BUTT;
}


LOCAL gint32 get_init_mode(json_object *huawei_jso, CREATE_INFO_S *create_info, json_object *o_message_array_jso)
{
    gint32 ret = VOS_ERR;
    json_bool bool_ret;
    guint8 o_volume_intimode;
    gchar initmode_ptr[MAX_VOL_STR_LEN] = {0};
    gchar str_value[MAX_VOL_STR_LEN] = {0};
    json_object *val_jso = NULL;
    json_object *message_info_jso = NULL;

    
    bool_ret = json_object_object_get_ex(huawei_jso, RFPROP_VOLUME_INITMODE, &val_jso);
    
    o_volume_intimode = INIT_MODE_DEF;

    if (bool_ret) {
        (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_COMMON_OEM,
            RFPROP_COMMON_HUAWEI, RFPROP_VOLUME_INITMODE);
        
        
        return_val_do_info_if_expr(CREATE_LD_AS_CACHECADE == create_info->create_type, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_VOLUME_NUNECESSARY_PROP, NULL, &message_info_jso, str_value);
            (void)json_object_array_add(o_message_array_jso, message_info_jso));
        
        ret = get_string_from_json(val_jso, str_value, initmode_ptr, MAX_VOL_STR_LEN, o_message_array_jso);
        return_val_if_fail(VOS_OK == ret, ret);

        o_volume_intimode = parse_init_mode(initmode_ptr);
        if (o_volume_intimode == LD_INIT_STATE_BUTT) {
            debug_log(DLOG_ERROR, "%s %d :%s: %s is fail", __FUNCTION__, __LINE__, RFPROP_VOLUME_INITMODE,
                initmode_ptr);
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, str_value, &message_info_jso, initmode_ptr, str_value);
            ret = json_object_array_add(o_message_array_jso, message_info_jso);
            if (VOS_OK != ret) {
                debug_log(DLOG_ERROR, "%s %d: json_object_array_add fail", __FUNCTION__, __LINE__);
                (void)json_object_put(message_info_jso);
            }

            return HTTP_BAD_REQUEST;
        }
    }

    create_info->init_mode = o_volume_intimode;
    return VOS_OK;
}


LOCAL gint32 get_block_index(json_object *huawei_jso, CREATE_INFO_S *create_info, json_object *o_message_array_jso)
{
    gint32 ret_val;
    json_object *block_index_jso = NULL;
    gint32 block_index_int32 = 0;
    gint32 return_val = HTTP_BAD_REQUEST;
    json_object *message_jso = NULL;
    gchar *prop_pointer = "Oem/Huawei/" RFPROP_VOLUME_BLOCK_INDEX;

    ret_val = json_object_object_get_ex(huawei_jso, RFPROP_VOLUME_BLOCK_INDEX, &block_index_jso);
    if ((ret_val == TRUE) && (block_index_jso != NULL)) {
        if (dal_test_controller_vendor(create_info->type_id, VENDER_PMC)) {
            generate_necessity_check_result_json(PARAM_UNNECESSARY, prop_pointer, o_message_array_jso);
            return HTTP_BAD_REQUEST;
        }

        if (create_info->create_type == ADD_LD_ON_EXIST_ARRAY) {
            block_index_int32 = json_object_get_int(block_index_jso);
            if (block_index_int32 >= 0 && block_index_int32 <= SML_MAX_HOLES_IN_ARRAY - 1) {
                create_info->block_index = (guint8)block_index_int32;
                return_val = VOS_OK;
            } else {
                // 格式错误
                (void)create_message_info(MSGID_PROP_FORMAT_ERR, prop_pointer, &message_jso,
                    dal_json_object_get_str(block_index_jso), prop_pointer);
            }
        } else {
            // 不是在已有array上添加逻辑盘，返回不支持的参数
            (void)create_message_info(MSGID_VOLUME_NUNECESSARY_PROP, prop_pointer, &message_jso, prop_pointer);
        }
    } else if ((ret_val == TRUE) && (block_index_jso == NULL)) {
        // 类型错误
        (void)create_message_info(MSGID_PROP_TYPE_ERR, prop_pointer, &message_jso, RF_VALUE_NULL, prop_pointer);
    } else {
        create_info->block_index = STORAGE_INFO_INVALID_BYTE;
        return_val = VOS_OK;
    }

    if (message_jso) {
        (void)json_object_array_add(o_message_array_jso, message_jso);
    }

    return return_val;
}


LOCAL guint8 check_associated_ld_necessity(CREATE_INFO_S *create_info, json_bool bool_ret)
{
    if (dal_test_controller_vendor(create_info->type_id, VENDER_PMC)) {
        
        if (create_info->create_type == CREATE_LD_AS_CACHECADE && !bool_ret) {
            return PARAM_MISSING;
        }
    } else {
        
        if (bool_ret) {
            return PARAM_UNNECESSARY;
        }
    }

    return PARAM_OK;
}


LOCAL gint32 get_associated_ld(json_object* huawei_jso, CREATE_INFO_S* create_info, json_object* o_message_array_jso)
{
    json_object *associated_ld_jso = NULL;
    gchar *prop_pointer = "Oem/Huawei/"REPROP_VOLUME_ASSOCIATED_LD;
    
    json_bool bool_ret = json_object_object_get_ex(huawei_jso, REPROP_VOLUME_ASSOCIATED_LD, &associated_ld_jso);
    guint8 check_result = check_associated_ld_necessity(create_info, bool_ret);
    if (check_result != PARAM_OK) {
        generate_necessity_check_result_json(check_result, prop_pointer, o_message_array_jso);
        return HTTP_BAD_REQUEST;
    }

    if (!bool_ret || associated_ld_jso == NULL) {
        return RET_OK;
    }

    gsize array_len = json_object_array_length(associated_ld_jso);
    json_object *message_info_jso = NULL;
    if (array_len != 1) {  // 只能为1
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, prop_pointer, &message_info_jso,
            dal_json_object_to_json_string(associated_ld_jso), prop_pointer);
        (void)json_object_array_add(o_message_array_jso, message_info_jso);
        return HTTP_BAD_REQUEST;
    }

    json_object *jso_value = json_object_array_get_idx(associated_ld_jso, 0);
    if (jso_value == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_array_get_idx failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 获取属性值
    json_object *jso_odata = NULL;
    json_object_object_get_ex(jso_value, RFPROP_FWINV_ODATA_ID, &jso_odata);
    if (jso_odata == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    const gchar *associated_ld_str = dal_json_object_get_str(jso_odata);
    if (get_ld_id_from_uri(create_info->ctrl_id, associated_ld_str, &create_info->associated_ld) != RET_OK) {
        (void)create_message_info(MSGID_PROPERTY_VALUE_ERR, prop_pointer, &message_info_jso, prop_pointer);
        (void)json_object_array_add(o_message_array_jso, message_info_jso);
        return HTTP_BAD_REQUEST;
    }

    return RET_OK;
}


LOCAL guint8 check_accelerator_necessity(CREATE_INFO_S *create_info, json_bool bool_ret)
{
    if (dal_test_controller_vendor(create_info->type_id, VENDER_PMC)) {
        
        if (create_info->create_type == CREATE_LD_AS_CACHECADE && bool_ret) {
            return PARAM_UNNECESSARY;
        }
    } else {
        
        if (bool_ret) {
            return PARAM_UNNECESSARY;
        }
    }

    return PARAM_OK;
}


LOCAL gint32 get_accelerator(json_object* huawei_jso, CREATE_INFO_S* create_info, json_object* o_message_array_jso)
{
    json_object *accelerator_jso = NULL;
    gchar *prop_pointer = "Oem/Huawei/"REPROP_VOLUME_ACCELERATION_METHOD;
    
    json_bool bool_ret = json_object_object_get_ex(huawei_jso, REPROP_VOLUME_ACCELERATION_METHOD, &accelerator_jso);
    guint8 check_result = check_accelerator_necessity(create_info, bool_ret);
    if (check_result != PARAM_OK) {
        generate_necessity_check_result_json(check_result, prop_pointer, o_message_array_jso);
        return HTTP_BAD_REQUEST;
    }

    if (!bool_ret || accelerator_jso == NULL) {
        return RET_OK;
    }

    const gchar *accelerator_str = dal_json_object_get_str(accelerator_jso);
    if (g_strcmp0(accelerator_str, "None") == 0) {
        create_info->accelerator = LD_ACCELERATOR_NONE;
    } else if (g_strcmp0(accelerator_str, "ControllerCache") == 0) {
        create_info->accelerator = LD_ACCELERATOR_CACHE;
    } else if (g_strcmp0(accelerator_str, "IOBypass") == 0) {
        create_info->accelerator = LD_ACCELERATOR_IOBYPASS;
    } else {
        json_object *message_info_jso = NULL;
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, prop_pointer, &message_info_jso, accelerator_str,
            prop_pointer);
        (void)json_object_array_add(o_message_array_jso, message_info_jso);
        return HTTP_BAD_REQUEST;
    }

    return RET_OK;
}


LOCAL guint8 check_cache_line_size_necessity(CREATE_INFO_S *create_info, json_bool bool_ret)
{
    if (dal_test_controller_vendor(create_info->type_id, VENDER_PMC)) {
        
        if (create_info->create_type != CREATE_LD_AS_CACHECADE && bool_ret) {
            return PARAM_UNNECESSARY;
        }
    } else {
        
        if (bool_ret) {
            return PARAM_UNNECESSARY;
        }
    }

    return PARAM_OK;
}


LOCAL gint32 get_cache_line_size(json_object *huawei_jso, CREATE_INFO_S *create_info,
    json_object *o_message_array_jso)
{
    json_object *cache_line_size_jso = NULL;
    gchar *prop_pointer = "Oem/Huawei/"REPROP_VOLUME_CACHE_LINE_SIZE;
    
    json_bool bool_ret = json_object_object_get_ex(huawei_jso, REPROP_VOLUME_CACHE_LINE_SIZE, &cache_line_size_jso);
    guint8 check_result = check_cache_line_size_necessity(create_info, bool_ret);
    if (check_result != PARAM_OK) {
        generate_necessity_check_result_json(check_result, prop_pointer, o_message_array_jso);
        return HTTP_BAD_REQUEST;
    }

    if (!bool_ret || cache_line_size_jso == NULL) {
        return RET_OK;
    }

    gint32 cache_line_size = json_object_get_int(cache_line_size_jso);
    if (cache_line_size != 64 && cache_line_size != 256) { // 只有64/256才是有效值
        json_object *message_info_jso = NULL;
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, prop_pointer, &message_info_jso,
            dal_json_object_to_json_string(cache_line_size_jso), prop_pointer);
        (void)json_object_array_add(o_message_array_jso, message_info_jso);
        return HTTP_BAD_REQUEST;
    }

    create_info->cache_line_size = cache_line_size / 64;  // 以64为基数转换
    return RET_OK;
}


LOCAL gint32 get_ld_policy(json_object *huawei_jso, CREATE_INFO_S *create_info, json_object *o_message_array_jso)
{
    
    gint32 ret = get_read_policy(huawei_jso, create_info, o_message_array_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    ret = get_io_policy(huawei_jso, create_info, o_message_array_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    ret = get_access_policy(huawei_jso, create_info, o_message_array_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    ret = get_cache_policy(huawei_jso, create_info, o_message_array_jso);
    return ret;
}


LOCAL gint32 get_ld_create_info(json_object *body_jso_checked, json_object *huawei_jso, CREATE_INFO_S *create_info,
    json_object *o_message_array_jso)
{
    
    gint32 ret = get_raid_level(huawei_jso, create_info, o_message_array_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    ret = get_volume_name(huawei_jso, create_info, o_message_array_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    ret = get_write_policy(huawei_jso, create_info, o_message_array_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    ret = get_span_num(huawei_jso, create_info, o_message_array_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    ret = get_volume_capacity(body_jso_checked, create_info, o_message_array_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    

    
    ret = get_stripe_size(body_jso_checked, create_info, o_message_array_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    ret = get_ld_policy(huawei_jso, create_info, o_message_array_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    ret = get_init_mode(huawei_jso, create_info, o_message_array_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret = get_block_index(huawei_jso, create_info, o_message_array_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret = get_associated_ld(huawei_jso, create_info, o_message_array_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret = get_accelerator(huawei_jso, create_info, o_message_array_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret = get_cache_line_size(huawei_jso, create_info, o_message_array_jso);
    return ret;
}


LOCAL gint32 check_create_type(json_object *huawei_jso, CREATE_INFO_S *create_info, json_object *o_message_array_jso)
{
    
    create_info->create_type = CREATE_LD_ON_NEW_ARRAY;
    gint32 ret = is_create_volume_on_new_array(create_info, o_message_array_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    json_object *val_jso = NULL;
    json_bool bool_jso = json_object_object_get_ex(huawei_jso, RFPROP_VOLUME_CACHECADE_FLAG, &val_jso);
    if (!bool_jso || val_jso == NULL) {
        return RET_OK;
    }

    
    if (json_object_get_boolean(val_jso) == FALSE) {
        return RET_OK;
    }

    
    if (dal_test_controller_vendor(create_info->type_id, VENDER_PMC)) {
        create_info->create_type = CREATE_LD_AS_CACHECADE;
    } else if (create_info->create_type == ADD_LD_ON_EXIST_ARRAY) {
        
        const gchar* str_value = "Oem/Huawei/"RFPROP_VOLUME_CACHECADE_FLAG;
        json_object *message_info_jso = NULL;
        (void)create_message_info(MSGID_VOLUME_NUNECESSARY_PROP, str_value, &message_info_jso, str_value);
        (void)json_object_array_add(o_message_array_jso, message_info_jso);
        return HTTP_BAD_REQUEST;
    } else {
        
        create_info->create_type = CREATE_LD_AS_CACHECADE;
    }

    return RET_OK;
}


LOCAL gint32 fill_create_info(json_object *body_jso_checked, CREATE_INFO_S *create_info,
    json_object *o_message_array_jso)
{
    gint32 ret;
    gchar str_value[MAX_VOL_STR_LEN] = {0};
    json_object *oem_jso = NULL;
    json_object *huawei_jso = NULL;

    
    ret = get_json_obj(body_jso_checked, RFPROP_COMMON_OEM, RFPROP_COMMON_OEM, &oem_jso, o_message_array_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s", RFPROP_COMMON_OEM,
        RFPROP_COMMON_HUAWEI);
    ret = get_json_obj(oem_jso, RFPROP_COMMON_HUAWEI, str_value, &huawei_jso, o_message_array_jso);
    return_val_if_fail(VOS_OK == ret, ret);
    
    ret = get_disk_id_array(huawei_jso, create_info, o_message_array_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    (void)dal_get_property_value_byte(create_info->object_handle, PROPERTY_RAID_CONTROLLER_TYPE_ID,
        &create_info->type_id);

    ret = check_create_type(huawei_jso, create_info, o_message_array_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret = get_ld_create_info(body_jso_checked, huawei_jso, create_info, o_message_array_jso);
    if (ret != RET_OK) {
        return ret;
    }

    return VOS_OK;
}


LOCAL gint32 process_disk_raid_err(gint32 err_code, CREATE_INFO_S *create_info, json_object **message_obj)
{
    gint32 ret_val = HTTP_BAD_REQUEST;
    gchar str_value[MAX_VOL_STR_LEN] = {0};
    gchar str_value2[MAX_VOL_STR_LEN] = {0};

#define PD_STATUS_ERR_MESSAGE()                                                                                  \
    case SML_ERR_CONFIG_INVALID_PD_BOOT_SECTOR_FOUND:                                                            \
    case SML_ERR_CONFIG_INVALID_PD_NON_SUPPORTED:                                                                \
    case SML_ERR_CONFIG_INVALID_PD_SCSI_DEV_TYPE:                                                                \
    case SML_ERR_CONFIG_INVALID_PD_IN_USE:                                                                       \
    case SML_ERR_CONFIG_INVALID_PD_WITH_FOREIGN_CONFIG:                                                          \
    case SML_ERR_PD_STATE_UNSUPPORTED_TO_SET:                                                                    \
        (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_FWINV_OEM,      \
            RFPROP_FWINV_HUAWEI, RFPROP_VOLUME_DRIVES);                                                          \
        format_disk_id_array(str_value2, sizeof(str_value2), create_info->disk_id_array, create_info->disk_num); \
        (void)create_message_info(MSGID_PHY_STATUS_ERR, str_value, message_obj, str_value2, str_value);     \
        break

#define RAID_LELVEL_ERR_MESSAGE()                                                                                 \
    case SML_ERR_CONFIG_INVALID_PARAM_RAID_LEVEL:                                                                 \
    case SM_CODE_INVALID_RAID_LEVEL:                                                                              \
        (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_FWINV_OEM,       \
            RFPROP_FWINV_HUAWEI, RFPROP_VOLUME_RAID_LEVEL);                                                       \
        (void)create_message_info(MSGID_RAID_LEVEL_ERR, str_value, message_obj, create_info->str_raid_level, \
            str_value);                                                                                           \
        break

    switch (err_code) {
            
        case SML_ERR_CONFIG_INVALID_PD_SDD_HDD_MIXED:
            (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_FWINV_OEM,
                RFPROP_FWINV_HUAWEI, RFPROP_VOLUME_DRIVES);
            format_disk_id_array(str_value2, sizeof(str_value2), create_info->disk_id_array, create_info->disk_num);
            (void)create_message_info(MSGID_STORAGE_MEDI_ERR, str_value, message_obj, str_value2, str_value);
            break;

            // 所选择的物理盘的扇区大小不一致
        case SML_ERR_CONFIG_BLOCK_SIZE_NOT_SAME: 
            (void)create_message_info(MSGID_DIFF_DRIVES_SECTOR_SIZE,
                RFPROP_FWINV_OEM "/" RFPROP_FWINV_HUAWEI "/" RFPROP_VOLUME_DRIVES, message_obj);
            break;

            
        case SML_ERR_CONFIG_INVALID_PD_SDD_SAS_SATA_MIXED:
            (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_FWINV_OEM,
                RFPROP_FWINV_HUAWEI, RFPROP_VOLUME_DRIVES);
            format_disk_id_array(str_value2, sizeof(str_value2), create_info->disk_id_array, create_info->disk_num);
            (void)create_message_info(MSGID_PHY_DISK_INTERFACES_ERR, str_value, message_obj, str_value2,
                str_value);
            break;

            
            PD_STATUS_ERR_MESSAGE();

            
        case SML_ERR_CONFIG_INVALID_PARAM_REPEATED_PD_ID:
            (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_FWINV_OEM,
                RFPROP_FWINV_HUAWEI, RFPROP_VOLUME_DRIVES);
            format_disk_id_array(str_value2, sizeof(str_value2), create_info->disk_id_array, create_info->disk_num);
            (void)create_message_info(MSGID_PHY_LIST_DOUBLE_ERR, str_value, message_obj, str_value2, str_value);
            break;

            
            RAID_LELVEL_ERR_MESSAGE();

            
        case SML_ERR_CONFIG_OPERATION_NOT_SUPPORT:
            (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_FWINV_OEM,
                RFPROP_FWINV_HUAWEI, RFPROP_VOLUME_REF_RAIDCONTROLLER_ID);
            (void)snprintf_s(str_value2, sizeof(str_value2), sizeof(str_value2) - 1, "%d", create_info->ctrl_id);
            (void)create_message_info(MSGID_RAID_NOT_SUPPORTED, str_value, message_obj, str_value2, str_value);
            break;

            
        case SML_ERR_CONFIG_TARGET_LD_ID_EXHAUSTED:
            (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_FWINV_OEM,
                RFPROP_FWINV_HUAWEI, RFPROP_VOLUME_REF_RAIDCONTROLLER_ID);
            (void)snprintf_s(str_value2, sizeof(str_value2), sizeof(str_value2) - 1, "%d", create_info->ctrl_id);
            (void)create_message_info(MSGID_RAID_VOLUMES_MAX, str_value, message_obj, str_value2, str_value);
            break;

            
        case SML_ERR_CONFIG_INVALID_PD_NON_SDD_FOR_CACHECADE:
            (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_FWINV_OEM,
                RFPROP_FWINV_HUAWEI, RFPROP_VOLUME_DRIVES);
            format_disk_id_array(str_value2, sizeof(str_value2), create_info->disk_id_array, create_info->disk_num);
            (void)create_message_info(MSGID_PHY_NOT_SSD, str_value, message_obj, str_value2, str_value);
            break;

            
            
        case SM_CODE_INVALID_PD_COUNT:
            
            (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_FWINV_OEM,
                RFPROP_FWINV_HUAWEI, RFPROP_VOLUME_DRIVES);
            format_disk_id_array(str_value2, sizeof(str_value2), create_info->disk_id_array, create_info->disk_num);
            (void)create_message_info(MSGID_PHY_DISK_COUNT_ERR, str_value, message_obj, str_value2, str_value);
            break;

            
            
        case SM_CODE_INVALID_PD_ID:
            (void)snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_FWINV_OEM,
                RFPROP_FWINV_HUAWEI, RFPROP_VOLUME_DRIVES);
            format_disk_id_array(str_value2, sizeof(str_value2), create_info->disk_id_array, create_info->disk_num);
            (void)create_message_info(MSGID_PHY_DISK_ID_INVALID, str_value, message_obj, str_value2, str_value);
            break;
            

        default:
            debug_log(DLOG_ERROR, "%s: result code = 0x%x", __FUNCTION__, err_code);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
            ret_val = HTTP_INTERNAL_SERVER_ERROR;
            break;
    }

    return ret_val;
}

LOCAL void create_message_ld_name_exceed(CREATE_INFO_S *create_info, json_object **message_obj)
{
#define MAX_NAME_LEN_NUM    8
    const gchar *extra = NULL;
    gchar max_len_str[MAX_NAME_LEN_NUM] = {0};

    gsize max_len = dal_get_ld_name_max_len(create_info->ctrl_id);
    (void)snprintf_s(max_len_str, MAX_NAME_LEN_NUM, MAX_NAME_LEN_NUM - 1, "%zu", max_len);
    extra = max_len_str;

    (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, RFPROP_VOLUME_NAME, message_obj, create_info->volume_name,
        RFPROP_VOLUME_NAME, extra);
    return;
}

LOCAL void create_message_no_available_space(CREATE_INFO_S *create_info, json_object **message_obj)
{
    const gchar *prop_name = RFPROP_FWINV_OEM "/" RFPROP_FWINV_HUAWEI "/" RFPROP_VOLUME_DRIVES;
    gchar prop_val[MAX_VOL_STR_LEN] = {0};

    format_disk_id_array(prop_val, sizeof(prop_val), create_info->disk_id_array, create_info->disk_num);
    (void)create_message_info(MSGID_NO_ARRAY_SPACE, prop_name, message_obj, prop_val, prop_name);
    return;
}

LOCAL void create_err_message_info_inner(CREATE_INFO_S *create_info, const gchar *message_id, json_object **message_obj)
{
    gchar str_value[MAX_VOL_STR_LEN] = {0};
    gchar str_value2[MAX_VOL_STR_LEN] = {0};

    errno_t securec_rv = snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_FWINV_OEM,
        RFPROP_FWINV_HUAWEI, RFPROP_VOLUME_DRIVES);
    if (securec_rv <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, securec_rv);
    }
    format_disk_id_array(str_value2, sizeof(str_value2), create_info->disk_id_array, create_info->disk_num);
    (void)create_message_info(message_id, str_value, message_obj, str_value2, str_value);
}

LOCAL void create_span_err_message_info(CREATE_INFO_S *create_info, json_object **message_obj)
{
    gchar str_value[MAX_VOL_STR_LEN] = {0};
    gchar str_value2[MAX_VOL_STR_LEN] = {0};

    gint32 ret = snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%s/%s/%s", RFPROP_FWINV_OEM,
        RFPROP_FWINV_HUAWEI, RFPROP_VOLUME_SPAN_NUMBER);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    ret = snprintf_s(str_value2, sizeof(str_value2), sizeof(str_value2) - 1, "%u", create_info->span_num);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    (void)create_message_info(MSGID_SPAN_NUMBER_ERR, str_value, message_obj, str_value2, str_value);
    return;
}

LOCAL void create_stripe_size_err_message_info(CREATE_INFO_S *create_info, json_object **message_obj)
{
    gchar str_value[MAX_VOL_STR_LEN] = { 0 };
    gint32 ret = snprintf_s(str_value, sizeof(str_value), sizeof(str_value) - 1, "%u", create_info->strip_size);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret = %d", __FUNCTION__, ret);
    }
    (void)create_message_info(MSGID_STRIPE_SIZE_ERR, RFPROP_VOLUME_OPTIMUMIOSIZE, message_obj, str_value,
        RFPROP_VOLUME_OPTIMUMIOSIZE);
    return;
}


LOCAL gint32 process_err(CREAT_OR_DEL_VOL_PARAS_S *paras, json_object **message_obj)
{
    CREATE_INFO_S *create_info = &(paras->creat_or_del_info.create_info);
    CREAT_OR_DEL_VOL_RES_S *creat_or_del_result = &(paras->result);

    switch (creat_or_del_result->err_code) {
            
        case ERR_CODE_THREAD_NOT_READY:
            break;

            
        case ERR_CODE_VOL_CREATE_SUCCESS:
            (void)create_message_info(MSGID_VOLUME_CREATE_SUCCESS, NULL, message_obj, creat_or_del_result->res_id);
            break;

            
        case ERR_CODE_VOL_DEL_SUCCESS:
            (void)create_message_info(MSGID_VOLUME_DELETE_SUCCESS, NULL, message_obj, creat_or_del_result->res_id);
            break;

            
        case SML_ERR_CONFIG_INVALID_PARAM_SPAN_DEPTH:

            
        case SM_CODE_INVALID_SPAN_DEPTH:
            
            create_span_err_message_info(create_info, message_obj);
            break;

            
        case SML_ERR_CONFIG_INVALID_PARAM_NUM_DRIVE_PER_SPAN:
            create_err_message_info_inner(create_info, MSGID_SPAN_DISK_ERR, message_obj);
            break;

            
        case SML_ERR_CONFIG_ARRAY_SIZE_TOO_SMALL:
            create_err_message_info_inner(create_info, MSGID_ARRAY_SPACE_ERR, message_obj);
            break;

            
        case SML_ERR_CONFIG_ARRAY_NO_AVAILABLE_SPACE:
            create_message_no_available_space(create_info, message_obj);
            break;

            
        case SML_ERR_CONFIG_INVALID_PARAM_CAPACITY_TOO_SMALL:
        case SML_ERR_CONFIG_INVALID_PARAM_CAPACITY_TOO_LARGE:
        case SM_CODE_CAPACITY_OUT_OF_RANGE:
            (void)create_message_info(MSGID_VOLUME_CAPACITY_ERR, NULL, message_obj, RFPROP_VOLUME_CAPACITY);
            break;

        case SML_ERR_CONFIG_ASSOCIATED_LD_SIZE_OUT_OF_RANGE:
            (void)create_message_info(MSGID_ASSOCIATED_VOLUME_CAPACITY_ERR, NULL, message_obj);
            break;

            
        case SM_CODE_STRIP_SIZE_OUT_OF_RANGE:
            create_stripe_size_err_message_info(create_info, message_obj);
            break;

            
        case SM_CODE_PD_NOT_IDENTIFIED:
        case SM_CODE_NOT_SUPPORT_IN_PRESENT_STATE:
        case SML_ERR_CTRL_STATUS_INVALID:
        case MFI_STAT_DEVICE_NOT_FOUND:
        case SM_CODE_GET_PD_LIST_FAILED:
        case SML_ERR_LD_OPERATION_NOT_SUPPORT:
        case MFI_STAT_LD_INIT_IN_PROGRESS: // 逻辑盘正在初始化中
        case SML_ERR_DATA_INVALID:
            debug_log(DLOG_ERROR, "%s: result code = 0x%x", __FUNCTION__, creat_or_del_result->err_code);
            (void)create_message_info(MSGID_STATUS_NOT_SUPPORT, NULL, message_obj);
            break;

            
        case SML_ERR_INVALID_CMD:
        case SM_CODE_INVALID_OPERTATION:
        case SML_ERR_INVALID_PARAMETER:
            (void)create_message_info(MSGID_OPERATION_NOT_SUPPORTED, NULL, message_obj);
            break;

        case SM_CODE_READ_POLCIY_OUT_OF_RANGE:
            (void)create_message_info(MSGID_PROP_INVALID_VALUE, RFPROP_VOLUME_DEFAULT_READ_POLICY, message_obj,
                "", RFPROP_VOLUME_DEFAULT_READ_POLICY);
            break;
        case SM_CODE_WRITE_POLICY_OUT_OF_RANGE:
            (void)create_message_info(MSGID_PROP_INVALID_VALUE, RFPROP_VOLUME_DEFAULT_WRITE_POLICY, message_obj,
                "", RFPROP_VOLUME_DEFAULT_WRITE_POLICY);
            break;
        case SM_CODE_IO_POLICY_OUT_OF_RANGE:
            (void)create_message_info(MSGID_PROP_INVALID_VALUE, RFPROP_VOLUME_DEFAULT_CACHE_POLICY, message_obj,
                "", RFPROP_VOLUME_DEFAULT_CACHE_POLICY);
            break;

        case SM_CODE_LD_NAME_EXCEED_MAX_LEN:
            create_message_ld_name_exceed(create_info, message_obj);
            break;

        case SML_ERR_CTRL_BBU_STATUS_ABNORMAL:
            (void)create_message_info(MSGID_PROPERTY_BBU_STATE_NOT_SUPPORT, NULL, message_obj);
            break;

        case SM_CODE_OPERATION_IN_PROGRESS:
            (void)create_message_info(VMM_MESSAGE_OPERATIONINPROCESS, NULL, message_obj);
            break;

        default:
           return process_disk_raid_err(creat_or_del_result->err_code, create_info, message_obj);
    }

    return HTTP_BAD_REQUEST;
}


LOCAL gint32 creat_vol_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    CREAT_OR_DEL_VOL_PARAS_S *paras = (CREAT_OR_DEL_VOL_PARAS_S *)(monitor_fn_data->user_data);
    CREAT_OR_DEL_VOL_RES_S *result = &(paras->result);
    errno_t str_ret = EOK;

    switch (result->status) {
        case STAT_IN_PROGRESS:
            monitor_fn_data->task_state = RF_TASK_RUNNING;
            break;

        case STAT_OK:
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
            monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
            
            str_ret = strncpy_s(monitor_fn_data->create_resourse, sizeof(monitor_fn_data->create_resourse) - 1,
                result->res_id, strlen(result->res_id));
            do_val_if_fail(str_ret == EOK, debug_log(DLOG_ERROR, "%s strncpy_s fail.", __FUNCTION__));
            
            (void)process_err(paras, message_obj);
            break;

        case STAT_TASK_EXPCEPTION:
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
            break;

        case STAT_FAILED:
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            (void)process_err(paras, message_obj);
            break;

        default:
            break;
    }

    return VOS_OK; 
}


LOCAL gint32 del_vol_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    CREAT_OR_DEL_VOL_PARAS_S *paras = (CREAT_OR_DEL_VOL_PARAS_S *)(monitor_fn_data->user_data);
    CREAT_OR_DEL_VOL_RES_S *result = &(paras->result);

    switch (result->status) {
        case STAT_IN_PROGRESS:
            monitor_fn_data->task_state = RF_TASK_RUNNING;
            break;

        case STAT_OK:
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
            monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
            (void)process_err(paras, message_obj);
            break;

        case STAT_FAILED:
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            (void)process_err(paras, message_obj);
            break;

        case STAT_TASK_EXPCEPTION:
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
            break;

        default:
            break;
    }

    return VOS_OK; 
}


LOCAL void monitor_new_created_ld(void *data)
{
    CREAT_OR_DEL_VOL_PARAS_S *paras = NULL;
    CREAT_OR_DEL_VOL_RES_S *result = NULL;
    gint32 times;
    json_object *body_object = NULL;
    json_object *link_object = NULL;
    json_object *drive_object = NULL;

    if (NULL == data) {
        debug_log(DLOG_ERROR, "%s %d: param is NULL", __FUNCTION__, __LINE__);
        return;
    }

    (void)prctl(PR_SET_NAME, (uintptr_t) "rfCreateldMonitor");

    paras = (CREAT_OR_DEL_VOL_PARAS_S *)data;
    result = &(paras->result);

    
    
    
    // 最大等待次数
    times = 60 * 5 * 2;

    while (VOS_OK != rf_validate_rsc_exist(result->res_id, &body_object) ||
        TRUE != json_object_object_get_ex(body_object, RFPROP_VOLUME_LINK, &link_object) ||
        TRUE != json_object_object_get_ex(link_object, RFPROP_VOLUME_LINK_DRIVES, &drive_object) ||
        0 == json_object_array_length(drive_object)) {
        json_object_put(body_object);
        body_object = NULL;
        (void)vos_task_delay(500);

        break_if_expr(times < 0);
        times--;
    }

    // 函数里面有判空
    json_object_put(body_object);

    if (times > 0) {
        
        result->status = STAT_OK;
        result->err_code = ERR_CODE_VOL_CREATE_SUCCESS;
    } else {
        result->status = STAT_FAILED;
        result->err_code = VOS_ERR;
        debug_log(DLOG_ERROR, "%s: wait 5 minutes, logical drive %s not created", __FUNCTION__, result->res_id);
    }
    

    return;
}


gboolean redfish_check_volume_uri_effective(const gchar *i_uri, guint8 *o_controller_id, OBJ_HANDLE *o_obj_handle)
{
    gboolean result = FALSE;
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 controller_id = 0;
    gchar str_uri[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, result,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = dfl_get_object_list(CLASS_RAID_CONTROLLER_NAME, &obj_list);
    return_val_do_info_if_expr(VOS_OK != ret, result,
        debug_log(DLOG_MASS, "%s: get storage controller fail", __FUNCTION__));

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_RAID_CONTROLLER_ID, &controller_id);
        do_if_expr(((VOS_OK != ret) || (0xFF == controller_id)), continue);

        (void)memset_s(str_uri, sizeof(str_uri), 0, sizeof(str_uri));

        ret = snprintf_s(str_uri, sizeof(str_uri), sizeof(str_uri) - 1, SYSTEMS_STORAGE_VOLUMES_URI, slot_id,
            controller_id);
        do_if_expr(0 >= ret, debug_log(DLOG_ERROR, "%s %d: snprintf_s fail.", __FUNCTION__, __LINE__); continue);

        if (0 == g_ascii_strcasecmp(i_uri, str_uri)) {
            
            *o_controller_id = controller_id;
            *o_obj_handle = (OBJ_HANDLE)obj_node->data;
            g_slist_free(obj_list);
            return TRUE;
        }
    }

    g_slist_free(obj_list);
    
    return result;
}


LOCAL gint32 call_create_volume_method(CREAT_OR_DEL_VOL_PARAS_S *ld_create_func_paras)
{
    const gchar *method_name = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    guint8 raid_id = G_MAXUINT8;

    return_val_if_expr(ld_create_func_paras == NULL, VOS_ERR);

    CREATE_INFO_S *create_info = &(ld_create_func_paras->creat_or_del_info.create_info);
    PROVIDER_PARAS_S *provider_param = &(ld_create_func_paras->provider_paras);
    CREAT_OR_DEL_VOL_RES_S *result = &(ld_create_func_paras->result);

    if (CREATE_LD_ON_NEW_ARRAY == create_info->create_type || ADD_LD_ON_EXIST_ARRAY == create_info->create_type) {
        if (CREATE_LD_ON_NEW_ARRAY == create_info->create_type) { 
            method_name = METHOD_RAID_CONTROLLER_CREATE_COMMON_LD;
            input_list = g_slist_append(input_list, (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE,
                create_info->disk_id_array, create_info->disk_num, sizeof(guint8)));
        } else { 
            method_name = METHOD_RAID_CONTROLLER_ADD_LD;
            input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint16(create_info->array_id));
            input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(create_info->block_index));
        }

        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(create_info->raid_level));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(create_info->span_num));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(create_info->volume_name));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint32(create_info->volume_capacity));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(CAPACITY_UNIT_MB));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(create_info->strip_size_as_para));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(create_info->read_policy));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(create_info->write_policy));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(create_info->io_policy));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(create_info->access_policy));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(create_info->disk_cache_policy));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(create_info->init_mode));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(create_info->accelerator));
    } else { 
        method_name = METHOD_RAID_CONTROLLER_CREATE_CACHECADE_LD;
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE,
            create_info->disk_id_array, create_info->disk_num, sizeof(guint8)));
        input_list = g_slist_append(input_list, g_variant_new_byte(create_info->raid_level));
        input_list = g_slist_append(input_list, g_variant_new_string(create_info->volume_name));
        input_list = g_slist_append(input_list, g_variant_new_byte(create_info->write_policy));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint16(create_info->array_id));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint32(create_info->volume_capacity));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(CAPACITY_UNIT_MB));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint16(create_info->associated_ld));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(create_info->cache_line_size));
    }

    gint32 ret_val = uip_call_class_method_redfish(provider_param->is_from_webui, provider_param->user_name,
        provider_param->client, create_info->object_handle, CLASS_RAID_CONTROLLER_NAME, method_name, AUTH_ENABLE,
        provider_param->user_role_privilege, input_list, &output_list);

    gint32 result_code = ret_val;
    if (ret_val == VOS_OK) {
        guint16 ld_id = g_variant_get_uint16((GVariant *)(output_list->data));
        (void)redfish_get_board_slot(slot_id, sizeof(slot_id));
        (void)dal_get_property_value_byte(create_info->object_handle, PROPERTY_RAID_CONTROLLER_ID, &raid_id);
        ret_val = snprintf_s(result->res_id, sizeof(result->res_id), sizeof(result->res_id) - 1, RFPROP_SYSTEM_REVOLUME,
            slot_id, raid_id, ld_id);
        do_if_expr(ret_val <= 0,
            debug_log(DLOG_ERROR, "%s: snprintf_s new created logical drive url failed", __FUNCTION__));
    } else {
        result->err_code = ret_val;
        debug_log(DLOG_ERROR, "%s: call %s.%s failed, ret_val = 0x%x", __FUNCTION__,
            dfl_get_object_name(create_info->object_handle), method_name, ret_val);
    }
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    return result_code;
}

LOCAL void init_create_info(CREATE_INFO_S *create_info, guint8 controller_id, OBJ_HANDLE obj_handle)
{
    
    create_info->type_id = INVALID_CONTROLLER_TYPE;
    create_info->raid_level = RAID_LEVEL_UNKNOWN;
    create_info->disk_num = STORAGE_INFO_INVALID_DWORD;
    create_info->span_num = STORAGE_INFO_INVALID_BYTE;
    create_info->volume_capacity = STORAGE_INFO_INVALID_DWORD;
    create_info->strip_size = STORAGE_INFO_INVALID_DWORD;
    create_info->strip_size_as_para = STORAGE_INFO_INVALID_BYTE;
    create_info->read_policy = STORAGE_INFO_INVALID_BYTE;
    create_info->write_policy = STORAGE_INFO_INVALID_BYTE;
    create_info->io_policy = STORAGE_INFO_INVALID_BYTE;
    create_info->access_policy = STORAGE_INFO_INVALID_BYTE;
    create_info->disk_cache_policy = STORAGE_INFO_INVALID_BYTE;
    create_info->init_mode = STORAGE_INFO_INVALID_BYTE;
    create_info->array_id = STORAGE_INFO_INVALID_WORD;
    create_info->block_index = STORAGE_INFO_INVALID_BYTE;
    create_info->accelerator = STORAGE_INFO_INVALID_BYTE;
    create_info->cache_line_size = STORAGE_INFO_INVALID_BYTE;
    create_info->associated_ld = STORAGE_INFO_INVALID_WORD;
    create_info->ctrl_id = controller_id;
    create_info->object_handle = obj_handle;
    return;
}


gint32 create_volume(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *o_id, guint32 id_len,
    json_object *o_message_array_jso)
{
    gint32 ret;
    gulong thread_id;
    gboolean bool_ret;
    guint8 controller_id = 0;
    OBJ_HANDLE obj_handle = 0;
    json_object *message_info_jso = NULL;
    TASK_MONITOR_INFO_S *task_monitor_info = NULL;
    CREAT_OR_DEL_VOL_PARAS_S *ld_create_func_paras = NULL;
    CREATE_INFO_S *create_info = NULL;
    CREAT_OR_DEL_VOL_RES_S *result = NULL;

    
    return_val_do_info_if_fail(
        ((NULL != o_id) && (NULL != o_message_array_jso) && (NULL != i_param) && (NULL != body_jso_checked)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s %d: input param error", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));
    
    gint32 board_result;
    guint8 board_type = 0;

    board_result = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_expr((VOS_OK != board_result) || (DISABLE == board_type), HTTP_NOT_FOUND,
        debug_log(DLOG_DEBUG, "%s:The board is switch", __FUNCTION__);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, i_param->uri);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));
    

    
    return_val_do_info_if_fail(0 != (i_param->user_role_privilege & USERROLE_BASICSETTING), HTTP_FORBIDDEN,
        debug_log(DLOG_ERROR, "%s %d: user has no privilege, user is %s", __FUNCTION__, __LINE__, i_param->user_name);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &message_info_jso);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));
    
    
    bool_ret = redfish_check_volume_uri_effective(i_param->uri, &controller_id, &obj_handle);
    
    return_val_do_info_if_fail(TRUE == bool_ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s: user input uri is bad.\n", __FUNCTION__);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, i_param->uri);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));

    ld_create_func_paras = (CREAT_OR_DEL_VOL_PARAS_S *)g_malloc0(sizeof(CREAT_OR_DEL_VOL_PARAS_S));
    return_val_do_info_if_fail(NULL != ld_create_func_paras, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s %d: malloc a new memory for CREAT_OR_DEL_VOL_PARAS_S failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));

    
    create_info = &(ld_create_func_paras->creat_or_del_info.create_info);
    init_create_info(create_info, controller_id, obj_handle);
    ret = fill_create_info(body_jso_checked, create_info, o_message_array_jso);
    return_val_do_info_if_fail(VOS_OK == ret, ret, g_free(ld_create_func_paras));

    
    result = &(ld_create_func_paras->result);

    
    (void)memcpy_s(&(ld_create_func_paras->provider_paras), sizeof(PROVIDER_PARAS_S), i_param,
        sizeof(PROVIDER_PARAS_S));

    ret = call_create_volume_method(ld_create_func_paras);
    if (ret != VOS_OK) {
        (void)process_err(ld_create_func_paras, &message_info_jso);
        ret = json_object_array_add(o_message_array_jso, message_info_jso);
        do_if_expr(ret != VOS_OK, json_object_put(message_info_jso));
        g_free(ld_create_func_paras);
        return HTTP_BAD_REQUEST;
    }

    result->err_code = ERR_CODE_THREAD_NOT_READY;
    result->status = STAT_IN_PROGRESS;
    task_monitor_info = task_monitor_info_new(g_free);
    return_val_do_info_if_fail(NULL != task_monitor_info, HTTP_INTERNAL_SERVER_ERROR, g_free(ld_create_func_paras);
        debug_log(DLOG_ERROR, "%s %d: task_monitor_info_new failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));

    task_monitor_info->task_progress = TASK_NO_PROGRESS; 
    task_monitor_info->user_data = ld_create_func_paras;
    task_monitor_info->rsc_privilege = USERROLE_BASICSETTING;

    
    ret = create_new_task("volume creation task", creat_vol_status_monitor, task_monitor_info, body_jso_checked,
        i_param->uri, &(i_param->val_jso)); 
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, task_monitor_info_free(task_monitor_info);
        debug_log(DLOG_ERROR, "%s %d: create_new_task failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));

    
    ret = vos_task_create(&thread_id, "MonitorNewCreatedLD", (TASK_ENTRY)monitor_new_created_ld, ld_create_func_paras,
        DEFAULT_PRIORITY);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, result->status = STAT_TASK_EXPCEPTION;
        debug_log(DLOG_ERROR, "%s %d: vos_task_create failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));

    return VOS_OK;
}


LOCAL gint32 get_controller_id_from_uri(gchar *uri, guchar *raid_id)
{
    gint32 ret;
    gchar *pStr = NULL;
    gint32 controllerId = INVALID_DATA_BYTE;
    gint32 logicalId = INVALID_DATA_BYTE;
    gchar *lower_uri = NULL;

    
    lower_uri = g_ascii_strdown(uri, strlen(uri));
    return_val_do_info_if_expr(NULL == lower_uri, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: g_ascii_strdown failed.", __FUNCTION__));

    pStr = strstr(lower_uri, "raidstorage");
    return_val_do_info_if_fail(NULL != pStr, VOS_ERR, g_free(lower_uri);
        debug_log(DLOG_ERROR, "%s: get RAIDStorage from uri failed.", __FUNCTION__));
    ret = sscanf_s(pStr, "raidstorage%d/volumes/logicaldrive%d", &controllerId, &logicalId);
    return_val_do_info_if_fail(ret > 0, VOS_ERR, g_free(lower_uri);
        debug_log(DLOG_ERROR, "%s: call sscanf_s failed, ret is %d", __FUNCTION__, ret));
    *raid_id = (guchar)controllerId;
    g_free(lower_uri);
    
    return VOS_OK;
}


LOCAL gint32 get_controller_handle_and_ld_id(PROVIDER_PARAS_S *i_param, guint16 *ld_id, OBJ_HANDLE *raid_obj_handle)
{
    gint32 ret;
    guint16 ld_id_value = 0;
    guint8 raid_id_value = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE raid_obj_handle_value = 0;
    gchar ld_uri[MAX_VOL_STR_LEN + 1] = {0};
    gchar raid_obj_name[MAX_VOL_STR_LEN] = {0};
    guint8 uri_raid_id = 0;

    
    ret = get_controller_id_from_uri(i_param->uri, &uri_raid_id);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get_controller_id_from_uri fail.", __FUNCTION__));
    

    
    ret = dfl_get_object_list(CLASS_LOGICAL_DRIVE_NAME, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER,
            raid_obj_name, sizeof(raid_obj_name));
        continue_do_info_if_fail(DFL_OK == ret,
            debug_log(DLOG_ERROR, "%s:dal_get_property_value_string fail", __FUNCTION__));

        
        ret = dfl_get_object_handle(raid_obj_name, &raid_obj_handle_value);
        continue_do_info_if_fail(DFL_OK == ret,
            debug_log(DLOG_ERROR, "%s:dfl_get_object_handle raidcontroller fail", __FUNCTION__));

        ret = dal_get_property_value_byte(raid_obj_handle_value, PROPERTY_RAID_CONTROLLER_ID, &raid_id_value);
        continue_do_info_if_fail(DFL_OK == ret,
            debug_log(DLOG_ERROR, "%s:dal_get_property_value_byte raidcontroller id fail", __FUNCTION__));

        
        continue_if_expr(raid_id_value != uri_raid_id);
        

        ret = dal_get_property_value_uint16((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_TARGET_ID, &ld_id_value);
        continue_do_info_if_fail(DFL_OK == ret,
            debug_log(DLOG_ERROR, "%s:dal_get_property_value_uint16 fail", __FUNCTION__));

        (void)memset_s(ld_uri, sizeof(ld_uri), 0x00, sizeof(ld_uri));
        
        (void)snprintf_s(ld_uri, MAX_VOL_STR_LEN, MAX_VOL_STR_LEN - 1, "LogicalDrive%u", ld_id_value);

        
        if (0 == g_ascii_strcasecmp(ld_uri, i_param->member_id)) {
            *raid_obj_handle = raid_obj_handle_value;
            *ld_id = ld_id_value;

            g_slist_free(obj_list);
            return VOS_OK;
        }
    }

    g_slist_free(obj_list);
    return VOS_ERR;
}


LOCAL void *delete_volume_thread_func(void *data)
{
    CREAT_OR_DEL_VOL_PARAS_S *paras = NULL;
    DEL_INFO_S *del_info = NULL;
    PROVIDER_PARAS_S *provider_param = NULL;
    CREAT_OR_DEL_VOL_RES_S *result = NULL;
    gint32 ret;
    GSList *input_list = NULL;

    if (NULL == data) {
        debug_log(DLOG_ERROR, "%s %d: param is NULL", __FUNCTION__, __LINE__);
        return NULL;
    }

    (void)prctl(PR_SET_NAME, (uintptr_t) "rfDeleteVolume");

    paras = (CREAT_OR_DEL_VOL_PARAS_S *)data;
    del_info = &(paras->creat_or_del_info.del_info);
    provider_param = &(paras->provider_paras);
    result = &(paras->result);

    
    input_list = g_slist_append(input_list, g_variant_new_uint16(del_info->volume_id));

    
    
    ret = uip_call_class_method_redfish(provider_param->is_from_webui, provider_param->user_name,
        provider_param->client, del_info->object_handle, CLASS_RAID_CONTROLLER_NAME, METHOD_RAID_CONTROLLER_DELETE_LD,
        AUTH_DISABLE, provider_param->user_role_privilege, input_list, NULL);
    

    uip_free_gvariant_list(input_list);

    if (VOS_OK == ret) {
        result->status = STAT_OK;
        result->err_code = ERR_CODE_VOL_DEL_SUCCESS;
    } else {
        result->status = STAT_FAILED;
        result->err_code = ret;
    }

    return NULL;
}


gint32 delete_volume(PROVIDER_PARAS_S *i_param, json_object *o_message_array_jso)
{
    int iRet;
    gint32 ret;
    guint16 ld_id = 0;
    OBJ_HANDLE raid_obj_handle = 0;
    json_object *message_info_jso = NULL;

    gulong thread_id;

    TASK_MONITOR_INFO_S *task_monitor_info = NULL;
    CREAT_OR_DEL_VOL_PARAS_S *ld_del_func_paras = NULL;
    DEL_INFO_S *del_info = NULL;
    CREAT_OR_DEL_VOL_RES_S *result = NULL;

    
    return_val_do_info_if_fail(((NULL != i_param) && (NULL != o_message_array_jso)), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s input param error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso));

    
    gint32 board_result;
    guint8 board_type = 0;

    board_result = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_expr((VOS_OK != board_result) || (DISABLE == board_type), HTTP_NOT_FOUND,
        debug_log(DLOG_DEBUG, "%s:The board is switch", __FUNCTION__);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, i_param->uri);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));
    

    
    return_val_do_info_if_fail(VOS_OK != (i_param->user_role_privilege & USERROLE_BASICSETTING), HTTP_FORBIDDEN,
        debug_log(DLOG_ERROR, "%s %d: user has no privilege, user is %s", __FUNCTION__, __LINE__, i_param->user_name);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &message_info_jso);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));
    
    ret = redfish_check_storages_ld_uri_effective(i_param->uri, NULL, FALSE);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s user input uri is bad.\n", __FUNCTION__);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, i_param->uri);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));

    
    ret = get_controller_handle_and_ld_id(i_param, &ld_id, &raid_obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));

    ld_del_func_paras = (CREAT_OR_DEL_VOL_PARAS_S *)g_malloc0(sizeof(CREAT_OR_DEL_VOL_PARAS_S));
    return_val_do_info_if_fail(NULL != ld_del_func_paras, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s %d: malloc a new memory for CREAT_OR_DEL_VOL_PARAS_S failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));

    
    del_info = &(ld_del_func_paras->creat_or_del_info.del_info);
    del_info->volume_id = ld_id;
    del_info->object_handle = raid_obj_handle;

    
    result = &(ld_del_func_paras->result);
    iRet = snprintf_s(result->res_id, MAX_VOL_STR_LEN, MAX_VOL_STR_LEN - 1, "%s", i_param->uri);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    result->err_code = ERR_CODE_THREAD_NOT_READY;
    result->status = STAT_IN_PROGRESS;

    
    (void)memcpy_s(&(ld_del_func_paras->provider_paras), sizeof(PROVIDER_PARAS_S), i_param, sizeof(PROVIDER_PARAS_S));

    task_monitor_info = task_monitor_info_new(g_free);
    return_val_do_info_if_fail(NULL != task_monitor_info, HTTP_INTERNAL_SERVER_ERROR, g_free(ld_del_func_paras);
        debug_log(DLOG_ERROR, "%s %d: task_monitor_info_new failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));

    task_monitor_info->task_progress = TASK_NO_PROGRESS; 
    task_monitor_info->user_data = ld_del_func_paras;
    task_monitor_info->rsc_privilege = USERROLE_BASICSETTING;

    
    ret = create_new_task("volume deletion task", del_vol_status_monitor, task_monitor_info, NULL, i_param->uri,
        &(i_param->val_jso)); 

    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, task_monitor_info_free(task_monitor_info);
        debug_log(DLOG_ERROR, "%s %d: create_new_task failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));

    ret = vos_task_create(&thread_id, "volume deletion thread", (TASK_ENTRY)delete_volume_thread_func,
        ld_del_func_paras, DEFAULT_PRIORITY);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, result->status = STAT_TASK_EXPCEPTION;
        debug_log(DLOG_ERROR, "%s %d: vos_task_create failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        (void)json_object_array_add(o_message_array_jso, message_info_jso));

    return VOS_OK;
}


LOCAL gint32 add_etag_header(json_object *rsc_jso, json_object *header_array_jso)
{
    int iRet;
    errno_t safe_fun_ret;
    gint32 ret;
    const gchar *redfish_resource = NULL;
    gchar etag[ETAG_LEN] = {0};
    gchar *check_sum = NULL;
    gchar etag_tmp[ETAG_LEN - 4] = {0};
    json_object *etag_jso = NULL;

    
    return_val_do_info_if_expr((NULL == rsc_jso) || (NULL == header_array_jso), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));
    

    redfish_resource = dal_json_object_get_str(rsc_jso);

    // 以资源对象字符串计算校验和sha256加密值
    check_sum = g_compute_checksum_for_string(G_CHECKSUM_SHA256, redfish_resource, strlen(redfish_resource));
    return_val_do_info_if_fail(NULL != check_sum, VOS_ERR, debug_log(DLOG_ERROR, "g_compute_checksum_for_string fail"));
    // 取check_sum前8个字节
    safe_fun_ret = strncpy_s(etag_tmp, ETAG_LEN - 4, check_sum, ETAG_LEN - 5);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    g_free(check_sum);

    // 生成etag字符串
    iRet = snprintf_s(etag, ETAG_LEN, ETAG_LEN - 1, "W/\"%s\"", etag_tmp);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    etag_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != etag_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d: json_object_new_object failed", __FUNCTION__, __LINE__));

    json_object_object_add(etag_jso, RESP_HEADER_ETAG, json_object_new_string(etag));

    ret = json_object_array_add(header_array_jso, etag_jso);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, (void)json_object_put(etag_jso);
        debug_log(DLOG_ERROR, "%s %d: json_object_array_add failed", __FUNCTION__, __LINE__));
    return VOS_OK;
}


gint32 generate_create_rsc_response_headers_task(PROVIDER_PARAS_S *i_param, gchar *id, json_object **header_array_jso,
    json_object *message_array_jso)
{
    gint32 ret;
    json_object *message_info_jso = NULL;

    
    return_val_do_info_if_expr((NULL == i_param) || (NULL == header_array_jso) || (NULL == id) ||
        (NULL == message_array_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));
    

    // 生成公共响应头
    ret = generate_common_response_headers(header_array_jso, i_param->uri, message_array_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: generate_common_response_headers error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso));

    // 生成Location头
    add_monitor_location_header(i_param->val_jso, *header_array_jso);

    // 生成ETag头
    (void)add_etag_header(i_param->val_jso, *header_array_jso);

    return HTTP_ACCEPTED;
}


gint32 generate_del_response_headers_task(PROVIDER_PARAS_S *i_param, json_object **header_array_jso, const gchar *uri,
    json_object *message_array_jso)
{
    gint32 ret;
    json_object *message_info_jso = NULL;

    
    return_val_do_info_if_expr((NULL == i_param) || (NULL == header_array_jso) || (NULL == uri) ||
        (NULL == message_array_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));
    

    // 生成公共响应头
    ret = generate_common_response_headers(header_array_jso, uri, message_array_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: generate_common_response_headers error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso));

    // 生成Location头
    add_monitor_location_header(i_param->val_jso, *header_array_jso);

    // 生成ETag头
    (void)add_etag_header(i_param->val_jso, *header_array_jso);

    return HTTP_ACCEPTED;
}
