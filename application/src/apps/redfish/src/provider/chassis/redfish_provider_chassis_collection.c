
#include "redfish_provider_chassisoverview.h"
#include "redfish_provider.h"
LOCAL gint32 get_chassis_collection_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);


LOCAL PROPERTY_PROVIDER_S collection_chassis_provider[] = {
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, RFPROP_ODATA_ID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_collection_members, NULL, NULL, ETAG_FLAG_ENABLE},
};


gint32 rf_gen_chassis_component_id(const gchar *rsc_id, OBJ_HANDLE component_handle, gchar *chassis_id_buf,
    guint32 buf_len)
{
    CHASSIS_COMPONENT_INFO chassis_component_info = {0};
    gint32 ret;
    guchar comp_device_type = 0;
    guchar component_slot_id = 0;

    if ((chassis_id_buf == NULL) || ((rsc_id == NULL) && (component_handle == 0))) {
        return RET_ERR;
    }

    
    if (component_handle == 0) {
        
        (void)dal_rf_get_board_slot(chassis_id_buf, buf_len);
        if (g_ascii_strcasecmp(chassis_id_buf, rsc_id) == 0) {
            return RET_OK;
        }

        (void)memset_s(chassis_id_buf, buf_len, 0, buf_len);

        ret = rf_get_chassis_component_info_by_id(rsc_id, &chassis_component_info, &component_slot_id);
        if ((ret != RET_OK) || (chassis_component_info.chassis_id_prefix == NULL)) {
            debug_log(DLOG_MASS, "cann't find chassis component info for %s", rsc_id);
            return RET_ERR;
        }
    } else if (!dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_MGNT)) { 
        ret = dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &comp_device_type);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s failed:cann't get property value for %s", __FUNCTION__,
                PROPERTY_COMPONENT_DEVICE_TYPE);
            return RET_ERR;
        }

        
        if (comp_device_type == COMPONENT_TYPE_MAINBOARD) {
            return dal_rf_get_board_slot(chassis_id_buf, buf_len);
        }

        ret = dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICENUM, &component_slot_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s failed, cann't get property value for %s", __FUNCTION__,
                PROPERTY_COMPONENT_DEVICENUM);
            return RET_ERR;
        }
        
        ret = rf_get_chassis_component_info_by_type(comp_device_type, &chassis_component_info);
        if (ret != RET_OK || chassis_component_info.chassis_id_prefix == NULL) {
            debug_log(DLOG_ERROR, "get chassis component info for type (%d) failed", comp_device_type);
            return RET_ERR;
        }
    } else {
        
        chassis_component_info.component_num_prop = NULL;
        chassis_component_info.chassis_id_prefix = RF_CHASSIS_TYPE_ENCL;
    }

    
    if (chassis_component_info.component_num_prop == NULL) {
        ret = strncpy_s(chassis_id_buf, buf_len, chassis_component_info.chassis_id_prefix,
            strlen(chassis_component_info.chassis_id_prefix));
        if (ret != EOK) {
            debug_log(DLOG_ERROR, "copy chassis id prefix failed");
            return RET_ERR;
        }
    } else { 
        ret = snprintf_s(chassis_id_buf, buf_len, buf_len - 1, "%s%u", chassis_component_info.chassis_id_prefix,
            component_slot_id);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "format chassis id failed, ret is %d", ret);
            return RET_ERR;
        }
    }
    return RET_OK;
}

LOCAL CHASSIS_COMPONENT_INFO g_board_chassis_info[] = {
    {COMPONENT_TYPE_IO_BOARD,   PROPERTY_COMPOENT_IO_CARD_NUM,              RF_IOBOARD_CHASSIS_PREFIX,    RF_CHASSIS_TYPE_MODULE},
    {COMPONENT_TYPE_GPU_BOARD,  PROPERTY_COMPOENT_GPU_BOARD_NUM,           RF_GPUBOARD_CHASSIS_PREFIX,    RF_CHASSIS_TYPE_DRAWER},
    {COMPONENT_TYPE_CHASSIS,    PROPERTY_COMPONENT_NODE_NUM,                RF_NODE_CHASSIS_PREFIX,       RF_CHASSIS_TYPE_BLADE},
    {COMPONENT_TYPE_BACKPLANE_BOARD, NULL,                                  RF_ENCLOSURE_CHASSIS_PREFIX,  RF_CHASSIS_TYPE_ENCL},
    {COMPONENT_TYPE_HMM,        PROPERTY_COMPONENT_MM_NUM,                  RF_SMM_CHASSIS_PREFIX,        RF_CHASSIS_TYPE_MODULE},
    {COMPONENT_TYPE_UNIT, PROPERTY_COMPONENT_UNIT_NUM, RF_UNIT_CHASSIS_PREFIX, RF_CHASSIS_TYPE_MODULE},
    {COMPONENT_TYPE_AR_CARD, NULL, RF_NODE_CHASSIS_PREFIX, RF_CHASSIS_TYPE_BLADE}
};


gint32 rf_get_chassis_component_info_by_type(guchar component_type, CHASSIS_COMPONENT_INFO *chassis_component_info)
{
    guint32 i;
    guint32 len;
    errno_t ptr_ret = EOK;

    return_val_do_info_if_fail((NULL != chassis_component_info), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    len = G_N_ELEMENTS(g_board_chassis_info);
    for (i = 0; i < len; i++) {
        if (component_type == g_board_chassis_info[i].component_type) {
            ptr_ret = memmove_s((void *)chassis_component_info, sizeof(CHASSIS_COMPONENT_INFO),
                (void *)&g_board_chassis_info[i], sizeof(CHASSIS_COMPONENT_INFO));
            return_val_do_info_if_fail((EOK == ptr_ret), VOS_ERR,
                debug_log(DLOG_ERROR, "copy chassis component info failed"));

            return VOS_OK;
        }
    }

    debug_log(DLOG_ERROR, "Unknown chassis component type (%d)", component_type);

    return VOS_ERR;
}


gint32 rf_get_chassis_component_info_by_id(const gchar *chassis_id, CHASSIS_COMPONENT_INFO *chassis_component_info,
    guchar *component_id)
{
    guint32 i;
    guint32 len;
    errno_t ptr_ret;
    guchar max_component_cnt = 0;
    const gchar *pos = NULL;
    gint32 ret;
    guchar index_out = 0;
    OBJ_HANDLE obj_handle = 0;

    return_val_do_info_if_fail((NULL != chassis_id), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed, input param error", __FUNCTION__));

    ret = dal_get_object_handle_nth(CLASS_PRODUCT_COMPONENT, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "cann't get object handle for class [%s], ret is %d", CLASS_PRODUCT_COMPONENT, ret));

    len = G_N_ELEMENTS(g_board_chassis_info);
    for (i = 0; i < len; i++) {
        pos = g_str_case_rstr(chassis_id, g_board_chassis_info[i].chassis_id_prefix);
        if (NULL != pos) {
            if (NULL == g_board_chassis_info[i].component_num_prop) {
                
                return_val_if_fail(0 == g_ascii_strcasecmp(chassis_id, g_board_chassis_info[i].chassis_id_prefix),
                    VOS_ERR);
            } else {
                
                pos += strlen(g_board_chassis_info[i].chassis_id_prefix);
                ret = vos_str2int(pos, 10, (void *)&index_out, NUM_TPYE_UCHAR);
                return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
                    debug_log(DLOG_ERROR, "cann't get component index with %s", pos));

                ret = dal_get_property_value_byte(obj_handle, g_board_chassis_info[i].component_num_prop,
                    &max_component_cnt);
                return_val_do_info_if_fail((VOS_OK == ret), VOS_ERR,
                    debug_log(DLOG_ERROR, "get property (%s) value for object handle (%s) failed, ",
                    g_board_chassis_info[i].component_num_prop, dfl_get_object_name(obj_handle)));

                if (index_out > max_component_cnt) {
                    debug_log(DLOG_DEBUG, "current component id exceed, max index limit is %d, current index id is %d",
                        max_component_cnt, index_out);

                    return VOS_ERR;
                }
            }

            if (NULL != chassis_component_info) {
                ptr_ret = memmove_s((void *)chassis_component_info, sizeof(CHASSIS_COMPONENT_INFO),
                    (const void *)&g_board_chassis_info[i], sizeof(CHASSIS_COMPONENT_INFO));
                return_val_do_info_if_fail((EOK == ptr_ret), VOS_ERR,
                    debug_log(DLOG_ERROR, "copy chassis component info failed"));
            }

            
            do_val_if_expr(NULL != component_id, (*component_id = index_out));

            return VOS_OK;
        }
    }

    debug_log(DLOG_DEBUG, "cann't find component info for %s", chassis_id);

    return VOS_ERR;
}


LOCAL gint32 _gen_chassis_component_uri_array(CHASSIS_COMPONENT_INFO component_info, json_object *uri_array_jso)
{
    int iRet = -1;
    gint32 ret;
    gchar               chassis_uri[MAX_URI_LENGTH] = {0};
    gchar               chassis_id[MAX_MEM_ID_LEN] = {0};
    errno_t ptr_ret = EOK;
    OBJ_HANDLE product_component_handle = 0;
    guchar max_component_cnt = 0;
    gint32 j;

    return_val_do_info_if_fail((NULL != uri_array_jso), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    ret = dal_get_object_handle_nth(CLASS_PRODUCT_COMPONENT, 0, &product_component_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "get object handle for class [%s] failed, ret is %d", CLASS_PRODUCT_COMPONENT, ret));

    
    if (NULL == component_info.component_num_prop) {
        (void)memset_s(chassis_id, MAX_MEM_ID_LEN, 0, MAX_MEM_ID_LEN);
        (void)memset_s(chassis_uri, MAX_URI_LENGTH, 0, MAX_URI_LENGTH);

        ptr_ret = strncat_s(chassis_id, MAX_MEM_ID_LEN, component_info.chassis_id_prefix,
            strlen(component_info.chassis_id_prefix));
        return_val_do_info_if_fail(EOK == ptr_ret, VOS_ERR, debug_log(DLOG_ERROR, "connect chassis id failed"));

        ret = snprintf_s(chassis_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT_CHASSIS, chassis_id);
        return_val_do_info_if_fail(ret > 0, VOS_ERR,
            debug_log(DLOG_ERROR, "format chassis uri failed, ret is %d", ret));

        RF_ADD_ODATA_ID_MEMBER(uri_array_jso, chassis_uri);

        return VOS_OK;
    }

    
    ret = dal_get_property_value_byte(product_component_handle, component_info.component_num_prop, &max_component_cnt);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "cann't get property value for (%s) of object (%s)", component_info.component_num_prop,
        dfl_get_object_name(product_component_handle)));

    for (j = 1; j < max_component_cnt + 1; j++) {
        (void)memset_s(chassis_id, MAX_MEM_ID_LEN, 0, MAX_MEM_ID_LEN);
        (void)memset_s(chassis_uri, MAX_URI_LENGTH, 0, MAX_URI_LENGTH);

        iRet = snprintf_s(chassis_id, MAX_MEM_ID_LEN, MAX_MEM_ID_LEN - 1, "%s%d", component_info.chassis_id_prefix, j);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        iRet = snprintf_s(chassis_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT_CHASSIS, chassis_id);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

        RF_ADD_ODATA_ID_MEMBER(uri_array_jso, chassis_uri);
    }

    return VOS_OK;
}


void gen_enclosure_contains_chassis_array(json_object *uri_array_jso)
{
    guint32 i;
    guint32 array_size;
    gint32 ret;

    if (uri_array_jso == NULL) {
        return;
    }

    array_size = G_N_ELEMENTS(g_board_chassis_info);
    for (i = 0; i < array_size; i++) {
        if (COMPONENT_TYPE_CHASSIS_BACKPLANE != g_board_chassis_info[i].component_type) {
            ret = _gen_chassis_component_uri_array(g_board_chassis_info[i], uri_array_jso);
            continue_do_info_if_fail(ret == VOS_OK, debug_log(DLOG_ERROR,
                "gen chassis component uri array for %s failed", g_board_chassis_info[i].chassis_id_prefix));
        }
    }

    return;
}


gboolean check_is_support_node_enclosure(guint32 product_id, guchar board_type)
{
    GSList *obj_list = NULL;
    GSList *obj_iter = NULL;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar comp_dev_type = 0;
    gboolean is_support = FALSE;

    if (dal_check_if_vsmm_supported()) {
        return TRUE;
    }

    
    if (product_id != PRODUCT_ID_ATLAS &&
        product_id != PRODUCT_ID_XSERIAL_SERVER &&
        product_id != PRODUCT_ID_PANGEA_V6) {
        return FALSE;
    }
    
    return_val_if_expr(BOARD_BLADE != board_type, FALSE);

    
    ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_ERROR, "%s, %d: get object handle for %s fail", __FUNCTION__, __LINE__, CLASS_COMPONENT));

    for (obj_iter = obj_list; NULL != obj_iter; obj_iter = obj_iter->next) {
        obj_handle = (OBJ_HANDLE)obj_iter->data;
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &comp_dev_type);
        if (check_pangea_node_to_enc(obj_handle)) {
            is_support = TRUE;
            break;
        }
        
        do_if_expr(COMPONENT_TYPE_BACKPLANE_BOARD == comp_dev_type, is_support = TRUE; break);
    }

    g_slist_free(obj_list);
    return is_support;
}


LOCAL void _add_node_enclousre_chassis(json_object *member_array_jso)
{
    gint32 ret;
    guchar board_type = 0;
    OBJ_HANDLE product_handle = 0;
    guint32 product_id = 0;
    gchar               chassis_uri[MAX_URI_LENGTH] = {0};

    ret = dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &product_handle);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "get object handle for class [%s] failed, ret is %d", CLASS_NAME_PRODUCT, ret));

    (void)dal_get_property_value_uint32(product_handle, PROPERTY_PRODUCT_ID, &product_id);
    (void)dal_rf_get_board_type(&board_type);

    debug_log(DLOG_DEBUG, "product id is [%d], board type is %d", product_id, board_type);

    
    
    return_if_fail(check_is_support_node_enclosure(product_id, board_type));
    

    ret = snprintf_s(chassis_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT_CHASSIS, RF_ENCLOSURE_CHASSIS_PREFIX);
    return_do_info_if_fail(ret > 0, debug_log(DLOG_ERROR, "%s:format string failed, ret is %d", __FUNCTION__, ret));

    RF_ADD_ODATA_ID_MEMBER(member_array_jso, chassis_uri);

    return;
}


LOCAL void _gen_chassis_asset_locate_uri_array(json_object *asset_locate_array_jso)
{
    int iRet = -1;
    OBJ_HANDLE obj_rack_asset = 0;
    guint8 asset_mgmt_status = ASSET_MGMT_SVC_OFF;
    gint32 ret;
    GSList *unitinfo_list = NULL;
    GSList *unitinfo_iter = NULL;
    OBJ_HANDLE unitinfo_handle = 0;
    guint8 unit_number = 0;
    gchar chassis_uri[MAX_URI_LENGTH] = {0};
    gchar chassis_id[MAX_MEM_ID_LEN] = {0};

    // 资产管理配置对象获取
    ret = dal_get_object_handle_nth(CLASS_RACK_ASSET_MGMT, 0, &obj_rack_asset);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_DEBUG, "%s: get rackasset mgmt handle failed, ret is %d", __FUNCTION__, ret));

    ret = dal_get_property_value_byte(obj_rack_asset, PROPERTY_RACK_ASSET_MGMT_SVC_STATUS, &asset_mgmt_status);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s: Failed to query asset management service status. ret=%d", __FUNCTION__, ret));

    
    return_do_info_if_fail(ASSET_MGMT_SVC_ON == asset_mgmt_status,
        debug_log(DLOG_DEBUG, "%s:Asset management service not ready.", __FUNCTION__));

    // 找到相应slot的U位对象列表
    ret = dfl_get_object_list(CLASS_UNIT_INFO, &unitinfo_list);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_DEBUG, "%s: get unitinfo list failed, ret is %d", __FUNCTION__, ret));

    debug_log(DLOG_DEBUG, "%s: unitinfo obj num is %d", __FUNCTION__, g_slist_length(unitinfo_list));

    // 遍历U位信息，更新Members属性
    for (unitinfo_iter = unitinfo_list; NULL != unitinfo_iter; unitinfo_iter = unitinfo_iter->next) {
        unitinfo_handle = (OBJ_HANDLE)unitinfo_iter->data;

        (void)dal_get_property_value_byte(unitinfo_handle, PROPERTY_UNIT_INFO_U_NUM, &unit_number);
        iRet = snprintf_s(chassis_id, MAX_MEM_ID_LEN, MAX_MEM_ID_LEN - 1, "%s%u", RF_UNIT_CHASSIS_PREFIX, unit_number);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        iRet = snprintf_s(chassis_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT_CHASSIS, chassis_id);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        RF_ADD_ODATA_ID_MEMBER(asset_locate_array_jso, chassis_uri);
    }

    g_slist_free(unitinfo_list);
}


LOCAL gint32 get_chassis_collection_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet = -1;
    gint32 ret = 0;
    guint32 i;
    guint32 max_component_info_cnt;
    guchar board_type = 0;
    gchar               chassis_uri[MAX_URI_LENGTH] = {0};
    gchar               chassis_id[MAX_MEM_ID_LEN] = {0};

    
    
    return_val_do_info_if_expr(VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso || NULL == o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s %d", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:   memory alloc fail. ", __FUNCTION__, __LINE__));

    
    (void)dal_rf_get_board_type(&board_type);
    

    _add_node_enclousre_chassis(*o_result_jso);

    if ((BOARD_MM != board_type)) {
        (void)dal_rf_get_board_slot(chassis_id, MAX_MEM_ID_LEN);
        iRet = snprintf_s(chassis_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT_CHASSIS, chassis_id);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        RF_ADD_ODATA_ID_MEMBER(*o_result_jso, chassis_uri);

        debug_log(DLOG_MASS, "%s: board type is %d", __FUNCTION__, board_type);

        if (BOARD_RM == board_type) {
            _gen_chassis_asset_locate_uri_array(*o_result_jso);
        }
        return HTTP_OK;
    }

    
    max_component_info_cnt = G_N_ELEMENTS(g_board_chassis_info);
    for (i = 0; i < max_component_info_cnt; i++) {
        ret = _gen_chassis_component_uri_array(g_board_chassis_info[i], *o_result_jso);
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "gen chassis uri info for type [%s] failed",
            g_board_chassis_info[i].chassis_id_prefix));
    }

    return HTTP_OK;
}


gint32 chassis_provider_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    *prop_provider = collection_chassis_provider;
    *count = sizeof(collection_chassis_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gint32 get_chassis_collection_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    json_object *members_jso = NULL;
    gint32 array_len;

    (void)provider_get_prop_values(i_paras, &o_rsc_jso, o_err_array_jso, collection_chassis_provider,
        G_N_ELEMENTS(collection_chassis_provider));

    (void)json_object_object_get_ex(o_rsc_jso, RFPROP_MEMBERS, &members_jso);

    array_len = (NULL == members_jso) ? 0 : json_object_array_length(members_jso);

    json_object_object_add(o_rsc_jso, RFPROP_MEMBERS_COUNT, json_object_new_int(array_len));

    return RF_OK;
}


gint32 chassis_overview_provider_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    if (rm_is_support_chassis_overview() != TRUE) {
        return HTTP_NOT_FOUND;
    }
    return VOS_OK;
}
