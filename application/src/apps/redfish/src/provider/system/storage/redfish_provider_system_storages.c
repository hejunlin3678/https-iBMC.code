
#include "redfish_provider.h"
#include "redfish_provider_system.h"

LOCAL gint32 get_systems_storages_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storages_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storages_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_system_storages_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);


LOCAL PROPERTY_PROVIDER_S members_storage_provider[] = {
    {  RFPROP_MEMBERS,  MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,  SYS_LOCKDOWN_NULL, get_systems_storages_members,  NULL,  NULL,  ETAG_FLAG_ENABLE},
    {  RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storages_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {  RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storages_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {  RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storages_count, NULL, NULL, ETAG_FLAG_ENABLE }
    
};


LOCAL gint32 get_system_storages_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;

    gint32 ret;
    gchar odata_context[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    iRet =
        snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, SYSTEMS_STROGES_METADATA, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string(odata_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_system_storages_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gchar slot_uri[PSLOT_URI_LEN] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:%d fail", __FUNCTION__, __LINE__));
    iRet = snprintf_s(slot_uri, sizeof(slot_uri), sizeof(slot_uri) - 1, SYSTEMS_STORAGES, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    
    *o_result_jso = json_object_new_string(slot_uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL void redfish_get_raidcontroller_count(guint8 *count)
{
    GSList* obj_list = NULL;
    GSList* obj_node = NULL;
    guint8 controller_id = 0;

    gint32 ret = dfl_get_object_list(CLASS_RAID_CONTROLLER_NAME, &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s: get storage controller list fail, ret = %d.", __FUNCTION__, ret);
        return;
    }

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_RAID_CONTROLLER_ID, &controller_id);
        if (ret != RET_OK || controller_id == 0xFF) {
            continue;
        }
        (*count)++;
    }
    g_slist_free(obj_list);
    return;
}

LOCAL void redfish_get_sdcontroller_count(guint8 *count)
{
    gint32 ret;
    GSList *obj_list = NULL;
    OBJ_HANDLE sd_handle;
    guint8 presence = 0;

    
    ret = dfl_get_object_list(CLASS_RAID_CHIP_NAME, &obj_list);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_MASS, "%s, %d: get storage controller list fail, ret = %d.\n", __FUNCTION__, __LINE__, ret));

    sd_handle = (OBJ_HANDLE)g_slist_nth_data(obj_list, 0);
    g_slist_free(obj_list);

    
    ret = dal_get_property_value_byte(sd_handle, PROPETRY_PRESENT, &presence);
    return_do_info_if_expr((VOS_OK != ret) || (0 == presence),
        debug_log(DLOG_MASS, "%s, %d:sd controller is not presence.\n", __FUNCTION__, __LINE__));

    (*count)++;
    return;
}

LOCAL gint32 get_system_storages_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 count = 0;
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    (void)redfish_get_raidcontroller_count(&count);
    (void)redfish_get_sdcontroller_count(&count);

    *o_result_jso = json_object_new_int(count);
    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}


LOCAL void storages_get_raidcontroller_memberid(const gchar *slot, json_object **o_result_jso)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 controller_id = 0;
    json_object *jso_obj = NULL;
    json_object *jso_uri = NULL;
    gchar controller_uri[STORAGE_CTRL_INFO_LENGTH] = {0};

    ret = dfl_get_object_list(CLASS_RAID_CONTROLLER_NAME, &obj_list);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_MASS, "%s, %d: get storage controller list fail, ret = %d.\n", __FUNCTION__, __LINE__, ret));

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_RAID_CONTROLLER_ID, &controller_id);
        do_if_expr(((VOS_OK != ret) || (0xFF == controller_id)), continue);

        jso_obj = json_object_new_object();
        do_if_expr(NULL == jso_obj, continue);

        (void)memset_s(controller_uri, STORAGE_CTRL_INFO_LENGTH, 0, STORAGE_CTRL_INFO_LENGTH);

        ret = snprintf_s(controller_uri, STORAGE_CTRL_INFO_LENGTH, STORAGE_CTRL_INFO_LENGTH - 1,
            SYSTEMS_STORAGE_CONTROLLER_URI, slot, controller_id);
        do_if_expr(0 >= ret, debug_log(DLOG_ERROR, "%s %d: snprintf_s fail.", __FUNCTION__, __LINE__);
            json_object_put(jso_obj); continue);

        jso_uri = json_object_new_string(controller_uri);
        do_if_expr(NULL == jso_uri, json_object_put(jso_obj); continue);
        (void)json_object_object_add(jso_obj, RFPROP_ODATA_ID, jso_uri);
        ret = json_object_array_add(*o_result_jso, jso_obj);
        do_if_expr(ret, json_object_put(jso_obj));
        jso_uri = NULL;
        jso_obj = NULL;
    }

    do_if_expr(NULL != obj_list, g_slist_free(obj_list));
    return;
}


LOCAL void storages_get_sdcardcontrolelr_memberid(gchar *slot, json_object **o_result_jso, guint8 slot_len)
{
    gint32 ret;
    json_object *jso_obj = NULL;
    json_object *jso_uri = NULL;
    gchar controller_uri[STORAGE_CTRL_INFO_LENGTH] = {0};

    OBJ_HANDLE sd_handle = 0;
    guint8 presence = 0;
    return_if_expr(NULL == slot || NULL == o_result_jso || 0 == slot_len);

    
    ret = dal_get_object_handle_nth(CLASS_RAID_CHIP_NAME, 0, &sd_handle);
    return_do_info_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s, %d: get storage controller list ret= %d fail.\n", __FUNCTION__, __LINE__, ret));
    

    
    ret = dal_get_property_value_byte(sd_handle, PROPETRY_PRESENT, &presence);
    return_do_info_if_expr((VOS_OK != ret) || (0 == presence),
        debug_log(DLOG_MASS, "%s, %d:sd controller is not presence.\n", __FUNCTION__, __LINE__));

    ret = snprintf_s(controller_uri, STORAGE_CTRL_INFO_LENGTH, STORAGE_CTRL_INFO_LENGTH - 1,
        SYSTEMS_STORAGE_SDCARDCONTROLLER_URI, slot);
    do_if_expr(0 >= ret, debug_log(DLOG_ERROR, "%s %d: snprintf_s fail.", __FUNCTION__, __LINE__));

    jso_obj = json_object_new_object();
    return_do_info_if_expr(NULL == jso_obj,
        debug_log(DLOG_MASS, "%s %d: json new object fail.", __FUNCTION__, __LINE__));

    jso_uri = json_object_new_string(controller_uri);
    return_do_info_if_expr(NULL == jso_uri, json_object_put(jso_obj);
        debug_log(DLOG_MASS, "%s %d: json new string fail.", __FUNCTION__, __LINE__));

    (void)json_object_object_add(jso_obj, RFPROP_ODATA_ID, jso_uri);
    ret = json_object_array_add(*o_result_jso, jso_obj);
    do_if_expr(ret, json_object_put(jso_obj));
    return;
}


LOCAL gint32 get_systems_storages_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar slot[MAX_RSC_ID_LEN] = {0};

    
    if (VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso || NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s %d", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:%d fail", __FUNCTION__, __LINE__));

    
    *o_result_jso = json_object_new_array();
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);
    
    (void)storages_get_raidcontroller_memberid(slot, o_result_jso);
    (void)storages_get_sdcardcontrolelr_memberid(slot, o_result_jso, sizeof(slot));

    return HTTP_OK;
}

gint32 collection_storage_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    guchar board_type = 0;

    
    ret = redfish_get_x86_enable_type(&board_type);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    return_val_if_fail(DISABLE != board_type, HTTP_NOT_FOUND);
    

    if (redfish_check_system_uri_valid(i_paras->uri)) {
        *prop_provider = members_storage_provider;
        *count = sizeof(members_storage_provider) / sizeof(PROPERTY_PROVIDER_S);

        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}
