

#include "redfish_provider.h"
#define MANAGER_VMMCOLLECTION_URI "/redfish/v1/Managers/%s/VirtualMedia/%s"
#define MANAGR_VMMCOLLECTION_CONTEXT "/redfish/v1/$metadata#Managers/Members/%s/VirtualMedia/$entity"
#define MANAGER_VMMCOLLECTION_ODATAID "/redfish/v1/Managers/%s/VirtualMedia"
LOCAL gint32 get_manager_vmmcollection_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_vmmcollection_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_vmmcollection_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_vmmcollection_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

typedef enum {
    VIRTUALMEDIA_TYPE_CD = 0,   
    VIRTUALMEDIA_TYPE_SP = 1,   
    VIRTUALMEDIA_TYPE_IBMA = 2, 
    VIRTUALMEDIA_TYPE_MAX,
} ENUM_VIRTUALMEDIA_TYPE;


LOCAL PROPERTY_PROVIDER_S g_vmmcollection_provider[] = {
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, RFPROP_ODATA_ID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_vmmcollection_members, NULL, NULL, ETAG_FLAG_ENABLE },
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_vmmcollection_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_vmmcollection_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_vmmcollection_count, NULL, NULL, ETAG_FLAG_ENABLE},
};

LOCAL gint32 get_manager_vmmcollection_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar slot_uri[PSLOT_URI_LEN] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gint32 ret;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: redfish_get_board_slot fail.", __FUNCTION__, __LINE__));
    ret = snprintf_s(slot_uri, sizeof(slot_uri), sizeof(slot_uri) - 1, MANAGER_VMMCOLLECTION_ODATAID, slot);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string(slot_uri);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 get_manager_vmmcollection_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
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

    ret = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, MANAGR_VMMCOLLECTION_CONTEXT,
        slot_id);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string(odata_context);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 get_manager_vmmcollection_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    // 打开数据库，打开数据库表，获取数据，填充响应，解锁数据库文件，关闭数据库

    gint32 ret;
    gchar uri[PSLOT_URI_LEN] = {0};
    json_object *obj_jso = NULL;
    gchar pslot[MEMORY_SN_LEN] = {0};
    gchar i;
    guint32 product_num = 0;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(pslot, MEMORY_SN_LEN);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:   memory alloc fail. ", __FUNCTION__, __LINE__));
    
    ret = dal_get_product_version_num(&product_num);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get product_num fail.\n", __FUNCTION__, __LINE__));

    for (i = 0; i < VIRTUALMEDIA_TYPE_MAX; i++) {
        if (VIRTUALMEDIA_TYPE_CD == i) {
            ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, MANAGER_VMMCOLLECTION_URI, pslot, "CD");
        }
        
        else if ((VIRTUALMEDIA_TYPE_SP == i) && (PRODUCT_VERSION_V5 <= product_num) &&
            (TRUE == rf_support_sp_service())) {
            ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, MANAGER_VMMCOLLECTION_URI, pslot, "USBStick");
        }
        
        else if ((VIRTUALMEDIA_TYPE_IBMA == i) && (PRODUCT_VERSION_V5 <= product_num) &&
            (VOS_OK == get_usbstick_ibma_service_support_state())) {
            ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, MANAGER_VMMCOLLECTION_URI, pslot, "iBMAUSBStick");
        } else {
            continue;
        }

        return_val_do_info_if_fail(VOS_OK < ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));

        
        obj_jso = json_object_new_object();
        return_val_do_info_if_expr(NULL == obj_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(*o_result_jso);
            debug_log(DLOG_ERROR, "%s,%d:   memory alloc fail. ", __FUNCTION__, __LINE__));
        json_object_object_add(obj_jso, RFPROP_ODATA_ID, json_object_new_string(uri));
        ret = json_object_array_add(*o_result_jso, obj_jso);

        
        do_val_if_expr(VOS_OK != ret, json_object_put(obj_jso);
            debug_log(DLOG_ERROR, "%s %d:json_object_array_add fail", __FUNCTION__, __LINE__));
    }

    return HTTP_OK;
}

LOCAL gint32 get_manager_vmmcollection_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 count;
    guint32 product_num = 0;
    gint32 spFlag;
    gint32 ibmaFlag;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    ret = dal_get_product_version_num(&product_num);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get product_num fail.\n", __FUNCTION__, __LINE__));

    spFlag = rf_support_sp_service();
    ibmaFlag = get_usbstick_ibma_service_support_state();
    
    if (PRODUCT_VERSION_V5 <= product_num) {
        if ((spFlag == TRUE) && (ibmaFlag == VOS_OK)) {
            count = 3; 
        } else if (((spFlag == TRUE) && (ibmaFlag != VOS_OK)) || ((spFlag != TRUE) && (ibmaFlag == VOS_OK))) {
            count = 2; 
        } else {
            count = 1; 
        }
    }
    
    else {
        count = 1; 
    }

    
    *o_result_jso = json_object_new_int(count);
    return HTTP_OK;
}


gint32 manager_provider_vmmcollection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);

    guint8 kvm_enabled = DISABLE;
    gint32 ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_KVM, &kvm_enabled);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_func_ability fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (kvm_enabled == DISABLE) {
        return HTTP_NOT_FOUND;
    }

    ret = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_do_info_if_expr(TRUE != ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s,%d:URI check failed. ", __FUNCTION__, __LINE__));

    *prop_provider = g_vmmcollection_provider;
    *count = sizeof(g_vmmcollection_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
