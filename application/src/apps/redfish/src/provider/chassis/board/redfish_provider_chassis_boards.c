

#include "redfish_provider.h"
#include "redfish_provider_chassis.h"

LOCAL gint32 get_chassis_boards_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_boards_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_boards_member(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_boards_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gboolean is_member_of_boards_rsc(OBJ_HANDLE comp_handle);

LOCAL PROPERTY_PROVIDER_S g_chassis_boards_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_boards_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS_COUNT, CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_boards_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_boards_member, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_boards_oem, NULL, NULL, ETAG_FLAG_ENABLE}
};


#define PROPERTY_BOARD_NO_MAX_NUM "0xFF"
LOCAL BOARD_PROPERTY g_boards_property[] = {
    {COMPONENT_TYPE_HDD_BACKPLANE,      RFPROP_TYPE_HDDBACKPLANE,       PROPERTY_COMPOENT_HDDBACK_CARD_NUM}, // 硬盘背板
    {COMPONENT_TYPE_PCIE_RISER,         RFPROP_TYPE_PCIERISER,          PROPERTY_COMPOENT_RISER_CARD_NUM},         // Riser卡
    {COMPONENT_TYPE_RAID_CARD,          RFPROP_TYPE_RAIDCARD,           PROPERTY_COMPOENT_RAID_CARD_NUM},            // Raid卡
    {COMPONENT_TYPE_EXPAND_BOARD,       RFPROP_TYPE_M2_TRANSFORM_CARD,  PROPERTY_COMPONENT_M2_NUM},      // M.2转接卡
    {COMPONENT_TYPE_PASS_THROUGH_CARD,  RFPROP_TYPE_PASS_THROUGH_CARD,  PROPERTY_BOARD_NO_MAX_NUM},                                                  // 直通卡，没有最大个数
    {COMPONENT_TYPE_MEZZ,               RFPROP_TYPE_MEZZCARD,           PROPERTY_COMPOENT_MEZZ_CARD_NUM}, // MEZZ卡
    {COMPONENT_TYPE_BOARD_PCIE_ADAPTER, RFPROP_TYPE_BOARD_PCIE_ADAPTER, PROPERTY_BOARD_NO_MAX_NUM}, // PCIE转接卡，没有最大个数
    {COMPONENT_TYPE_GPU_BOARD,          RFPROP_TYPE_GPU_BOARD,          PROPERTY_COMPOENT_GPU_BOARD_NUM}, // GPU板
    {COMPONENT_TYPE_FAN_BACKPLANE,      RFPROP_TYPE_FANBACKPLANE,       PROPERTY_BOARD_NO_MAX_NUM}, // 风扇背板，没有最大个数
    {COMPONENT_TYPE_HORIZ_CONN_BRD,     RFPROP_TYPE_HORIZ_CONN_BRD,     PROPERTY_COMPONENT_HORIZ_CONN_BRD_NUM}, // 水平转接板
    {COMPONENT_TYPE_BOARD_CPU,          RFPROP_TYPE_CPUBOARD,           PROPERTY_COMPOENT_CPU_BOARD_NUM},         // 处理器板
    {COMPONENT_TYPE_MEMORY_RISER,       RFPROP_TYPE_MEMORYBOARD,        PROPERTY_COMPOENT_MEMORY_CARD_NUM}, // 内存板
    {COMPONENT_TYPE_IO_BOARD,           RFPROP_TYPE_IOBOARD,            PROPERTY_COMPOENT_IO_CARD_NUM},             // IO板
    {COMPONENT_TYPE_CHASSIS_BACKPLANE,  RFPROP_TYPE_CHASSIS_BACKPLANE,  PROPERTY_BOARD_NO_MAX_NUM}, // 机框背板，没有最大个数
    {COMPONENT_TYPE_EXPANDER,           RFPROP_TYPE_EXPANDBOARD,        PROPERTY_COMPONENT_EXP_BOARD_NUM},
    {COMPONENT_TYPE_EXPBOARD,           RFPROP_TYPE_EXPANDBOARD,        PROPERTY_COMPONENT_EXP_BOARD_NUM},
    
    {COMPONENT_TYPE_LEAKDET_CARD,       RFPROP_TYPE_LEAKDETECTIONCARD,  PROPERTY_COMPONENT_LEAKDETCARD_NUM},   // 漏液检测卡
    {COMPONENT_TYPE_CIC,                RFPROP_TYPE_CICCARD,            PROPERTY_COMPONENT_CIC_CARD_NUM},       // CIC卡
    {COMPONENT_TYPE_EPM,                RFPROP_TYPE_EXP_MODULE,         PROPERTY_COMPONENT_EXP_MODULE_NUM}, // 北冰洋级联模块
    {COMPONENT_TYPE_FAN_MODULE,         RFPROP_TYPE_FAN_MODULE,         PROPERTY_COMPONENT_FAN_MODULE_NUM},  // 北冰洋风扇模块
    {COMPONENT_TYPE_AR_CARD,            RFPROP_TYPE_ARCARD,             PROPERTY_COMPONENT_ARCARD_NUM},  // 北冰洋AR卡
    {COMPONENT_TYPE_SOC_BOARD,          RFPROP_TYPE_SOCBOARD,           PROPERTY_COMPONENT_SOC_BOARD_NUM} // SoC主板
};



LOCAL guint8 *get_chassis_boards_type_array(gint32 *length)
{
    
    static guint8 rfboard_type_array[] = {
        COMPONENT_TYPE_HDD_BACKPLANE,
        COMPONENT_TYPE_RAID_CARD,
        COMPONENT_TYPE_PCIE_CARD,
        COMPONENT_TYPE_MEZZ,
        COMPONENT_TYPE_NIC_CARD,
        COMPONENT_TYPE_MEMORY_RISER,
        COMPONENT_TYPE_PCIE_RISER,
        COMPONENT_TYPE_MAINBOARD,
        COMPONENT_TYPE_FAN_BACKPLANE,
        COMPONENT_TYPE_SWITCH_MEZZ,
        COMPONENT_TYPE_IO_BOARD,
        COMPONENT_TYPE_BOARD_CPU,
        COMPONENT_TYPE_EXPAND_BOARD,
        COMPONENT_TYPE_LED_BOARD,
        COMPONENT_TYPE_PIC_CARD,
        COMPONENT_TYPE_TWIN_NODE_BACKPLANE,
        
        COMPONENT_TYPE_BOARD_PCIE_ADAPTER,
        
        COMPONENT_TYPE_GPU_BOARD,
        
        COMPONENT_TYPE_PASS_THROUGH_CARD,
        
        
        COMPONENT_TYPE_MM_MMC,
        COMPONENT_TYPE_CHASSIS_BACKPLANE,
        
        
        COMPONENT_TYPE_PSU_BACKPLANE,
        
        
        COMPONENT_TYPE_HORIZ_CONN_BRD,
        
        COMPONENT_TYPE_ASSET_LOCATE_BRD,
        COMPONENT_TYPE_EXPANDER,
        COMPONENT_TYPE_EXPBOARD,
        
        COMPONENT_TYPE_LEAKDET_CARD,
        COMPONENT_TYPE_CIC,
        COMPONENT_TYPE_OCP,
        COMPONENT_TYPE_EPM,
        COMPONENT_TYPE_FAN_MODULE,
        COMPONENT_TYPE_AR_CARD,
        COMPONENT_TYPE_SOC_BOARD
    };
    *length = G_N_ELEMENTS(rfboard_type_array);
    return rfboard_type_array;
}


gboolean redfish_check_board_memberid(OBJ_HANDLE obj_handle, guint8 i_type_id, const gchar *i_memberid)
{
    
    gint32 length = 0;
    
    guint8 *board_type_pointor = get_chassis_boards_type_array(&length);
    if (NULL == board_type_pointor) {
        debug_log(DLOG_MASS, "%s, %d: get_chassis_boards_type_array fail.\n", __FUNCTION__, __LINE__);
        return FALSE;
    }
    for (gint32 i = 0; i < length; i++) {
        if (i_type_id == board_type_pointor[i]) {
            gchar device_name[MAX_STRBUF_LEN] = {0};
            gchar board_location[MAX_STRBUF_LEN] = {0};
            gchar uri_str[MAX_STRBUF_LEN] = {0};
            (void)dal_get_property_value_string(obj_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name,
                sizeof(device_name));
            (void)dal_clear_string_blank(device_name, sizeof(device_name));
            (void)dal_get_property_value_string(obj_handle, PROPERTY_COMPONENT_LOCATION, board_location,
                sizeof(board_location));
            (void)dal_clear_string_blank(board_location, sizeof(board_location));
            continue_if_expr(0 == strlen(device_name) || 0 == strlen(board_location));
            gint32 ret = snprintf_s(uri_str, sizeof(uri_str), sizeof(uri_str) - 1, "%s%s", board_location, device_name);
            do_val_if_expr(0 >= ret, debug_log(DLOG_ERROR, "%s %d : snprintf_s fail.", __FUNCTION__, __LINE__));
            if (0 == g_ascii_strcasecmp(uri_str, i_memberid)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

gboolean redfish_check_board_uri_effective(gchar *i_memberid, OBJ_HANDLE *o_obj_handle, OBJ_HANDLE chassis_handle)
{
    gint32 ret;
    gboolean match_flag = FALSE;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 component_type = 0;

    if (!is_node_enc_rsc(chassis_handle, NULL, NULL, NULL)) {
        
        ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
        do_val_if_expr(VOS_OK != ret, debug_log(DLOG_MASS, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

        for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
            dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);

            if (is_member_of_boards_rsc((OBJ_HANDLE)obj_node->data) != TRUE) {
                continue;
            }

            if (redfish_check_board_memberid((OBJ_HANDLE)obj_node->data, component_type, i_memberid)) {
                match_flag = TRUE;
                break;
            }
        }

        return_val_do_info_if_expr((NULL != obj_node) && match_flag, match_flag,
            *o_obj_handle = (OBJ_HANDLE)obj_node->data;
            g_slist_free(obj_list));

        do_if_expr(NULL != obj_list, g_slist_free(obj_list));
    }
    
    // 检查是否是外围卡
    obj_list = NULL;
    ret = dfl_get_object_list(CLASS_PERIPHERAL_CARD_NAME, &obj_list);
    if (VOS_OK == ret && NULL != obj_list) {
        for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
            gchar name_buf[MAX_STRBUF_LEN] = {0};
            gchar member_id[MAX_STRBUF_LEN] = {0};
            OBJ_HANDLE perip_handle = (OBJ_HANDLE)obj_node->data;
            ret = dal_get_property_value_string(perip_handle, PROPERTY_CARD_NAME, name_buf, sizeof(name_buf));
            continue_do_info_if_expr(VOS_OK != ret, debug_log(DLOG_MASS, "%s: get property value fail.", __FUNCTION__));
            (void)dal_clear_string_blank(name_buf, sizeof(name_buf));

            int iRet =
                snprintf_s(member_id, sizeof(member_id), sizeof(member_id) - 1, "%s%s", RF_PREFIX_PERIPHERAL, name_buf);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

            if (0 == g_ascii_strcasecmp(member_id, i_memberid)) {
                match_flag = TRUE;
                *o_obj_handle = perip_handle;
                break;
            }
        }
        do_if_expr(NULL != obj_list, g_slist_free(obj_list));
        
    }
    return match_flag;
}

LOCAL gint32 get_chassis_boards_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_id[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    
    // 获取 @odata.id
    ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot_id, MAX_RSC_ID_LEN);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);
    ret = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, "%s%s/%s", RF_CHASSIS_UI, slot_id,
        RFPROP_CHASSIS_BOARDS);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));
    
    *o_result_jso = json_object_new_string((const gchar *)odata_id);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}



LOCAL void get_chassis_boards_perip_cnt(gint32 *cnt)
{
    gint32 ret;
    guint32 perip_cnt = 0;

    return_do_info_if_fail(NULL != cnt, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    ret = dfl_get_object_count(CLASS_PERIPHERAL_CARD_NAME, &perip_cnt);
    if (VOS_OK == ret) {
        *cnt += perip_cnt;
    }
}


LOCAL void get_chassis_boards_peripheral_member(const gchar *slot, json_object **o_result_jso)
{
    gint32 ret;
    GSList *perip_list = NULL;
    GSList *perip_node = NULL;
    gchar name_buf[MAX_STRBUF_LEN] = {0};
    gchar member_id[MAX_STRBUF_LEN] = {0};
    gchar uri_str[MAX_STRBUF_LEN] = {0};
    OBJ_HANDLE perip_handle;
    json_object *jso_obj = NULL;

    return_do_info_if_fail(slot != NULL && o_result_jso != NULL,
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    ret = dfl_get_object_list(CLASS_PERIPHERAL_CARD_NAME, &perip_list);
    if (ret == VOS_OK && perip_list != NULL) {
        for (perip_node = perip_list; perip_node; perip_node = g_slist_next(perip_node)) {
            perip_handle = (OBJ_HANDLE)perip_node->data;
            (void)memset_s(name_buf, sizeof(name_buf), 0, sizeof(name_buf));
            ret = dal_get_property_value_string(perip_handle, PROPERTY_CARD_NAME, name_buf, sizeof(name_buf));
            continue_do_info_if_expr(ret != VOS_OK,
                debug_log(DLOG_MASS, "%s, %d: get name property value fail.", __FUNCTION__, __LINE__));
            (void)dal_clear_string_blank(name_buf, sizeof(name_buf));

            (void)memset_s(member_id, sizeof(member_id), 0, sizeof(member_id));
            ret =
                snprintf_s(member_id, sizeof(member_id), sizeof(member_id) - 1, "%s%s", RF_PREFIX_PERIPHERAL, name_buf);
            do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));

            (void)memset_s(uri_str, sizeof(uri_str), 0, sizeof(uri_str));
            ret = snprintf_s(uri_str, sizeof(uri_str), sizeof(uri_str) - 1, URI_FORMAT_CHASSIS_BOARD, slot, member_id);
            do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));

            jso_obj = json_object_new_object();
            continue_if_expr(jso_obj == NULL);

            json_object_object_add(jso_obj, RFPROP_ODATA_ID, json_object_new_string((const gchar *)uri_str));
            json_object_array_add(*o_result_jso, jso_obj);
        }

        g_slist_free(perip_list);
    }
}



LOCAL gboolean is_member_of_boards_rsc(OBJ_HANDLE comp_handle)
{
    guchar presence = 0;
    guchar fru_id = 0;
    gchar comp_uid[STRING_LEN_MAX] = {0};

    (void)dal_get_property_value_byte(comp_handle, PROPERTY_COMPONENT_PRESENCE, &presence);
    if (presence == 0) {
        return FALSE;
    }

    (void)dal_get_property_value_byte(comp_handle, PROPERTY_COMPONENT_FRUID, &fru_id);
    if (fru_id == INVALID_DATA_BYTE) {
        return FALSE;
    }

    if (dal_match_product_id(PRODUCT_ID_TIANCHI) == TRUE) {
        return TRUE;
    }

    // 合法的UID信息表示为天池组件，不在Board下显示
    (void)dal_get_property_value_string(comp_handle, PROPERTY_COMPONENT_UNIQUE_ID,
        comp_uid, sizeof(comp_uid));
    if (dal_check_component_uid_valid(comp_uid) == TRUE) {
        return FALSE;
    }

    return TRUE;
}


LOCAL gint32 get_chassis_boards_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guchar component_type = 0;
    gint32 i = 0;
    
    gint32 count = 0;
    gint32 length = 0;
    guint8 *board_type_pointor = NULL;
    
    
    gboolean b_ret;
    

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    
    board_type_pointor = get_chassis_boards_type_array(&length);
    if (NULL == board_type_pointor) {
        debug_log(DLOG_MASS, "%s, %d: get_chassis_boards_type_array fail.\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    

    
    
    b_ret = is_node_enc_rsc(i_paras->obj_handle, NULL, NULL, NULL);
    
    if (!b_ret) {
        
        ret = dfl_get_object_list(g_chassis_boards_provider[i_paras->index].pme_class_name, &obj_list);
        do_val_if_expr(VOS_OK != ret, debug_log(DLOG_MASS, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

        for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
            ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data,
                g_chassis_boards_provider[i_paras->index].pme_prop_name, &component_type);
            continue_if_expr(VOS_OK != ret);

            if (is_member_of_boards_rsc((OBJ_HANDLE)obj_node->data) != TRUE) {
                continue;
            }

            for (i = 0; i < length; i++) {
                break_do_info_if_expr((component_type == board_type_pointor[i]), count++);
            }
        }

        do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));
    }
    

    
    get_chassis_boards_perip_cnt(&count);
    

    *o_result_jso = json_object_new_int(count);
    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}


LOCAL gint32 get_chassis_boards_member(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guchar component_type = 0;
    gint32 i;
    gchar member_id[MAX_STRBUF_LEN] = {0};
    gchar uri_str[MAX_STRBUF_LEN] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    json_object *jso_obj = NULL;
    
    gint32 length = 0;
    guint8 *board_type_pointor = NULL;
    
    
    gboolean b_ret;
    

    
    return_val_do_info_if_fail(
        (o_result_jso != NULL && o_message_jso != NULL && provider_paras_check(i_paras) == VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    
    board_type_pointor = get_chassis_boards_type_array(&length);
    if (board_type_pointor == NULL) {
        debug_log(DLOG_MASS, "%s, %d: get_chassis_boards_type_array fail.\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    

    ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot, MAX_RSC_NAME_LEN);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get slot id error.\n", __FUNCTION__, __LINE__));

    
    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get slot id error.\n", __FUNCTION__, __LINE__));

    
    
    b_ret = is_node_enc_rsc(i_paras->obj_handle, NULL, NULL, NULL);
    
    if (!b_ret) {
        
        ret = dfl_get_object_list(g_chassis_boards_provider[i_paras->index].pme_class_name, &obj_list);
        do_val_if_expr(ret != VOS_OK, debug_log(DLOG_MASS, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

        for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
            (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data,
                g_chassis_boards_provider[i_paras->index].pme_prop_name, &component_type);

            if (is_member_of_boards_rsc((OBJ_HANDLE)obj_node->data) != TRUE) {
                continue;
            }
    
            for (i = 0; i < length; i++) {
                if (component_type == board_type_pointor[i]) {
                    
                    (void)memset_s(member_id, sizeof(member_id), 0, sizeof(member_id));

                    ret = get_object_obj_location_devicename((OBJ_HANDLE)obj_node->data, member_id, sizeof(member_id));
                    do_if_expr(VOS_OK != ret, continue);

                    ret = snprintf_s(uri_str, sizeof(uri_str), sizeof(uri_str) - 1, URI_FORMAT_CHASSIS_BOARD, slot,
                        member_id);
                    do_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s %d:snprintf_s fail.", __FUNCTION__, __LINE__));
                    
                    jso_obj = json_object_new_object();
                    do_if_expr(NULL == jso_obj, continue);
                    json_object_object_add(jso_obj, RFPROP_ODATA_ID, json_object_new_string((const gchar *)uri_str));
                    json_object_array_add(*o_result_jso, jso_obj);
                    jso_obj = NULL;
                    break;
                }
            }
        }

        do_val_if_expr(obj_list != NULL, g_slist_free(obj_list));
    }

    

    
    get_chassis_boards_peripheral_member((const gchar *)slot, o_result_jso);
    
    return HTTP_OK;
}

LOCAL gint32 get_ioboard_num_by_peripheral_foreach(OBJ_HANDLE object_handle, gpointer user_data)
{
    gint32 ret;
    guint8 type = 0;
    guchar *ioboard_num = (guchar *)user_data;

    
    return_val_do_info_if_fail(ioboard_num != NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: pointer ioboard_num is NULL.", __func__));

    ret = dal_get_property_value_byte(object_handle, PROPERTY_CARD_TYPE, &type);
    
    return_val_do_info_if_fail(ret == VOS_OK, VOS_OK,
        debug_log(DLOG_ERROR, "%s: get property type fail(%d).", __func__, ret));

    
    if (type == COMPONENT_TYPE_IO_BOARD) {
        ++(*ioboard_num);
    }

    return VOS_OK;
}


LOCAL gint32 get_chassis_boards_oem_board_max_num(guint32 arr_index, guchar *max_num)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    
    return_val_do_info_if_fail(arr_index < G_N_ELEMENTS(g_boards_property) && NULL != max_num, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: Input param error, arr_index is %d.", __func__, arr_index));

    
    if (0 == strcmp(g_boards_property[arr_index].prop_max_num, PROPERTY_BOARD_NO_MAX_NUM)) {
        *max_num = INVALID_DATA_BYTE;
        return VOS_OK;
    }

    
    ret = dal_get_object_handle_nth(CLASS_PRODUCT_COMPONENT, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get obj handle of class ProductComponent fail(%d).", __func__, ret));

    ret = dal_get_property_value_byte(obj_handle, g_boards_property[arr_index].prop_max_num, max_num);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get value of property %s fail(%d).", __func__,
        g_boards_property[arr_index].prop_max_num, ret));

    
    
    if (0 == strcmp(g_boards_property[arr_index].prop_max_num, PROPERTY_COMPOENT_IO_CARD_NUM)) {
        (void)dfl_foreach_object(CLASS_PERIPHERAL_CARD_NAME, get_ioboard_num_by_peripheral_foreach, max_num, NULL);
    }
    

    return VOS_OK;
}


LOCAL gint32 get_chassis_boards_oem_board_link(OBJ_HANDLE component_obj_handle, const gchar *slot,
    json_object **board_links_jso_arr)
{
    int iRet;
    gint32 ret;
    gchar odata_id[MAX_STRBUF_LEN + 1] = {0};
    gchar uri_str[MAX_STRBUF_LEN + 1] = {0};
    json_object *uri_jso = NULL;
    json_object *board_link_jso = NULL;

    
    return_val_do_info_if_fail(NULL != slot && NULL != board_links_jso_arr, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: Input param error.", __func__));

    
    ret = get_object_obj_location_devicename(component_obj_handle, odata_id, sizeof(odata_id));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s: get board link fail(%d).", __func__, ret));
    iRet = snprintf_s(uri_str, sizeof(uri_str), sizeof(uri_str) - 1, URI_FORMAT_CHASSIS_BOARD, slot, odata_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    uri_jso = json_object_new_string((const gchar *)uri_str);
    return_val_do_info_if_fail(NULL != uri_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: new json object as uri_jso fail.", __func__));

    board_link_jso = json_object_new_object();
    json_object_object_add(board_link_jso, RFPROP_ODATA_ID, uri_jso);

    
    if (NULL == *board_links_jso_arr) {
        *board_links_jso_arr = json_object_new_array();
    }

    json_object_array_add(*board_links_jso_arr, board_link_jso);
    return VOS_OK;
}


LOCAL void get_chassis_boards_oem_boards(json_object *huawei_jso, const gchar *slot)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_handle;
    gsize arr_index;
    guchar component_type = 0;
    guchar max_num = 0;
    json_object* links_jso_obj_arr[G_N_ELEMENTS(g_boards_property)] = {};
    json_object *max_num_jso = NULL;
    json_object *board_info_jso = NULL;
    json_object *boards_jso = NULL;
    
    return_do_info_if_fail(huawei_jso != NULL && slot != NULL,
        debug_log(DLOG_ERROR, "%s: Input param error.", __func__));

    
    gint32 ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
    return_do_info_if_fail(ret == VOS_OK,
        debug_log(DLOG_ERROR, "%s: get object list of class Component fail(%d).", __func__, ret));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        obj_handle = (OBJ_HANDLE)obj_node->data;

        if (is_member_of_boards_rsc(obj_handle) != TRUE) {
            continue;
        }

        ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
        continue_do_info_if_fail(ret == VOS_OK,
            debug_log(DLOG_ERROR, "%s: get prop DeviceType fail(%d).", __func__, ret));
        
        do_if_true(component_type == COMPONENT_TYPE_PSU_BACKPLANE,
            (component_type = COMPONENT_TYPE_IO_BOARD)) for (arr_index = 0; arr_index < G_N_ELEMENTS(g_boards_property);
            arr_index++) {
            if (component_type == g_boards_property[arr_index].board_type) {
                get_chassis_boards_oem_board_link(obj_handle, slot, &(links_jso_obj_arr[arr_index]));
                break;
            }
        }
    }
    g_slist_free(obj_list);

    boards_jso = json_object_new_object();

    
    for (arr_index = 0; arr_index < G_N_ELEMENTS(g_boards_property); arr_index++) {
        
        ret = get_chassis_boards_oem_board_max_num(arr_index, &max_num);
        if (ret != VOS_OK) {
            do_if_expr(links_jso_obj_arr[arr_index] != NULL, json_object_put(links_jso_obj_arr[arr_index]));
            continue;
        }
        if (max_num == 0) {
            
            do_if_expr(links_jso_obj_arr[arr_index] != NULL, json_object_put(links_jso_obj_arr[arr_index]));
            continue;
        } else if (max_num == INVALID_DATA_BYTE) {
            
            if (links_jso_obj_arr[arr_index] != NULL) {
                board_info_jso = json_object_new_object();
                json_object_object_add(board_info_jso, RFPROP_BOARD_MAX_NUM_SUPPORTED, NULL);
                json_object_object_add(board_info_jso, RF_CHASSIS_LINKS, links_jso_obj_arr[arr_index]);
                json_object_object_add(boards_jso, g_boards_property[arr_index].board_name, board_info_jso);
            }
        } else {
            
            max_num_jso = json_object_new_int(max_num);
            if (max_num_jso == NULL) {
                debug_log(DLOG_ERROR, "%s: new json object as max_num_jso fail.", __func__);
                do_if_expr(links_jso_obj_arr[arr_index] != NULL, json_object_put(links_jso_obj_arr[arr_index]));
                continue;
            }
            board_info_jso = json_object_new_object();
            json_object_object_add(board_info_jso, RFPROP_BOARD_MAX_NUM_SUPPORTED, max_num_jso);
            if (links_jso_obj_arr[arr_index] != NULL) {
                json_object_object_add(board_info_jso, RF_CHASSIS_LINKS, links_jso_obj_arr[arr_index]);
            } else {
                json_object_object_add(board_info_jso, RF_CHASSIS_LINKS, json_object_new_array());
            }
            json_object_object_add(boards_jso, g_boards_property[arr_index].board_name, board_info_jso);
        }
    }
    json_object_object_add(huawei_jso, RFPROP_CHASSIS_BOARDS, boards_jso);
}


LOCAL gint32 get_chassis_boards_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    json_object *oem_jso = NULL;
    json_object *huawei_jso = NULL;
    gchar slot[MAX_RSC_NAME_LEN + 1] = {0};
    gboolean b_ret;

    
    return_val_do_info_if_fail(NULL != i_paras && NULL != o_message_jso && NULL != o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: Input param error.", __func__));
    
    
    b_ret = is_node_enc_rsc(i_paras->obj_handle, NULL, NULL, NULL);
    return_val_if_expr(b_ret, HTTP_INTERNAL_SERVER_ERROR);
    

    
    ret = rf_gen_chassis_component_id(NULL, i_paras->obj_handle, slot, MAX_RSC_NAME_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get slot id fail(%d).", __func__, ret));

    
    oem_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != oem_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new json object as oem_jso fail.", __func__));

    huawei_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != huawei_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(oem_jso);
        debug_log(DLOG_ERROR, "%s: new json object fail.", __func__));
    json_object_object_add(oem_jso, RFPROP_COMMON_HUAWEI, huawei_jso);

    
    get_chassis_boards_oem_boards(huawei_jso, slot);

    *o_result_jso = oem_jso;

    return HTTP_OK;
}


gint32 chassis_boards_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean bool_ret;
    gint32 ret;

    bool_ret = rf_check_chassis_uri_valid_allow_node_enc(i_paras->uri, &i_paras->obj_handle);
    if (!bool_ret) {
        return HTTP_NOT_FOUND;
    }

    ret = check_enclosure_component_type(i_paras->obj_handle, TRUE);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);

    *prop_provider = g_chassis_boards_provider;
    *count = sizeof(g_chassis_boards_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
