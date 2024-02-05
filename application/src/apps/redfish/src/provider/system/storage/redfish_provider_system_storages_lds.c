
#include "redfish_provider.h"

LOCAL gint32 get_system_storage_ld_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage_ld_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_systems_storages_lds_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_systems_storages_lds_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_system_storage_lds_provider[] = {
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_systems_storages_lds_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_systems_storages_lds_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS_COUNT, CLASS_LOGICAL_DRIVE_NAME, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_ld_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, CLASS_LOGICAL_DRIVE_NAME, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage_ld_members, NULL, NULL, ETAG_FLAG_ENABLE}
};

LOCAL gint32 get_systems_storages_lds_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar slot[MAX_RSC_ID_LEN] = {0};
    gchar lds_context[MAX_URI_LENGTH] = {0};
    guint8 controller_id = 0;

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get slot fail.", __FUNCTION__));
    
    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_RAID_CONTROLLER_ID, &controller_id);
    
    iRet = snprintf_s(lds_context, sizeof(lds_context), sizeof(lds_context) - 1, RFPROP_VOLUMES_ODATA_CONTEXT, slot,
        controller_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    
    
    *o_result_jso = json_object_new_string((const char *)lds_context);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}

LOCAL gint32 get_systems_storages_lds_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar slot[MAX_RSC_ID_LEN] = {0};
    gchar lds_uri[MAX_URI_LENGTH] = {0};
    guint8 controller_id = 0;

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get slot fail.", __FUNCTION__));
    
    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_RAID_CONTROLLER_ID, &controller_id);
    
    iRet = snprintf_s(lds_uri, sizeof(lds_uri), sizeof(lds_uri) - 1, RFPROP_SYSTEM_REVOLUMES, slot, controller_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    
    
    *o_result_jso = json_object_new_string((const char *)lds_uri);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}

LOCAL gint32 get_system_storage_ld_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = 0;
    guint32 count = 0;
    GSList *ld_obj_list = NULL;
    GSList *obj_node = NULL;
    gchar raid_obj_name[MAX_NAME_SIZE] = {0};
    guint8 ld_raid_level = 0;

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dfl_get_object_list(g_system_storage_lds_provider[i_paras->index].pme_class_name, &ld_obj_list);
    if ((VOS_OK == ret) && (NULL != ld_obj_list)) {
        
        for (obj_node = ld_obj_list; obj_node; obj_node = obj_node->next) {
            ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER,
                raid_obj_name, sizeof(raid_obj_name));
            continue_do_info_if_fail(VOS_OK == ret,
                debug_log(DLOG_ERROR, "%s:dal_get_property_value_string fail", __FUNCTION__));
            if (0 == g_strcmp0(dfl_get_object_name(i_paras->obj_handle), raid_obj_name)) {
                
                (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_RAID_LEVEL,
                    &ld_raid_level);
                continue_if_expr(RAID_LEVEL_DELETED == ld_raid_level);
                
                count++;
            }
        }
        
        g_slist_free(ld_obj_list);
    } else {
        debug_log(DLOG_MASS, "%s: dfl_get_object_list fail.\n", __FUNCTION__);
        count = 0;
    }

    *o_result_jso = json_object_new_int((gint32)count);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __FUNCTION__));

    return HTTP_OK;
}

LOCAL gint32 get_system_storage_ld_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet = -1;
    gint32 ret;
    gchar raid_obj_name[MAX_NAME_SIZE] = {0};
    guint16 ld_id = 0;
    gchar ld_uri[MAX_URI_LENGTH] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *obj_jso = NULL;
    json_object *o_data = NULL;
    OBJ_HANDLE raid_obj_handle = 0;
    guchar raid_id = 0;
    gchar slot[MAX_RSC_ID_LEN] = {0};
    guint8 ld_raid_level = 0;

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get slot fail.", __FUNCTION__));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __FUNCTION__));

    ret = dfl_get_object_list(g_system_storage_lds_provider[i_paras->index].pme_class_name, &obj_list);
    return_val_do_info_if_fail((VOS_OK == ret) || (NULL == obj_list), HTTP_OK,
        debug_log(DLOG_MASS, "%s:get obj_list fail.\n", __FUNCTION__));

    return_val_do_info_if_expr(0 == g_slist_length(obj_list), HTTP_OK, g_slist_free(obj_list);
        debug_log(DLOG_MASS, "%s: no ld object", __FUNCTION__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER,
            raid_obj_name, sizeof(raid_obj_name));
        ret = dfl_get_object_handle(raid_obj_name, &raid_obj_handle);
        continue_do_info_if_fail(DFL_OK == ret,
            debug_log(DLOG_ERROR, "%s:dfl_get_object_handle raidcontroller fail", __FUNCTION__));
        
        do_if_fail(0 == g_strcmp0(dfl_get_object_name(i_paras->obj_handle), raid_obj_name), continue);
        

        
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_RAID_LEVEL,
            &ld_raid_level);
        continue_if_expr(RAID_LEVEL_DELETED == ld_raid_level);
        

        (void)dal_get_property_value_byte(raid_obj_handle, PROPERTY_RAID_CONTROLLER_ID, &raid_id);
        (void)dal_get_property_value_uint16((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_TARGET_ID, &ld_id);
        
        
        
        iRet = snprintf_s(ld_uri, sizeof(ld_uri), sizeof(ld_uri) - 1, RFPROP_SYSTEM_REVOLUME, slot, raid_id, ld_id);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        
        
        obj_jso = json_object_new_object();
        continue_do_info_if_fail(NULL != obj_jso,
            debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__));

        o_data = json_object_new_string((const gchar *)ld_uri);
        do_info_if_true(NULL == o_data, debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__));

        json_object_object_add(obj_jso, RFPROP_ODATA_ID, o_data);
        o_data = NULL;
        
        ret = json_object_array_add(*o_result_jso, obj_jso);
        do_info_if_true(ret, json_object_put(obj_jso);
            debug_log(DLOG_ERROR, "%s:json_object_array_add fail", __FUNCTION__));

        obj_jso = NULL;
    }

    g_slist_free(obj_list);
    return HTTP_OK;
}

gint32 system_storage_lds_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    return_val_do_info_if_expr((VOS_OK != provider_paras_check(i_paras)) || (NULL == count), VOS_ERR,
        debug_log(DLOG_ERROR, "input param error"));

    
    gint32 result;
    guint8 board_type = 0;
    guint8 controller_id = 0;

    result = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_expr((VOS_OK != result) || (DISABLE == board_type), HTTP_NOT_FOUND,
        debug_log(DLOG_DEBUG, "%s:The board is switch", __FUNCTION__));
    

    
    if (redfish_check_volume_uri_effective(i_paras->uri, &controller_id, &(i_paras->obj_handle))) {
        *prop_provider = g_system_storage_lds_provider;
        *count = G_N_ELEMENTS(g_system_storage_lds_provider);
        return VOS_OK;
    }
    
    return HTTP_NOT_FOUND;
}
