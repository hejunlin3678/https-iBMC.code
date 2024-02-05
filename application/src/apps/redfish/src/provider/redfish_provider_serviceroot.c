

#include "redfish_provider.h"
#include "redfish_provider_chassisoverview.h"
#include "redfish_provider_resource.h"

LOCAL gint32 get_serviceroot_uuid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL void get_sms_oem(json_object *huawei);
LOCAL gint32 get_serviceroot_fabric_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL void get_data_acquisition_service_oem(json_object *huawei);
LOCAL void get_rack_mgnt_service_oem(json_object *huawei);
typedef gint32 (*double_prop_handle)(double value);
LOCAL gint32 chassis_height_prop_check(double value);
LOCAL void get_prop_string_oem(gchar *prop_name, gchar *pme_class_name, gchar *pme_prop_name, json_object *huawei,
    json_bool prop_check);
LOCAL void get_prop_array_oem(gchar *prop_name, gchar *pme_class_name, gchar *pme_prop_name, json_object *huawei);
LOCAL void get_prop_int_oem(gchar *prop_name, gchar *pme_class_name, gchar *pme_prop_name, json_object *huawei);
LOCAL void get_prop_double_oem(gchar *prop_name, gchar *pme_class_name, gchar *pme_prop_name, json_object *huawei,
    double_prop_handle pfn_prop_handle);
LOCAL void get_prop_boolean_oem(gchar *prop_name, gchar *pme_class_name, gchar *pme_prop_name, json_object *huawei);
LOCAL gint32 get_service_root_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL PROPERTY_PROVIDER_S g_serviceroot_provider[] = {
    
    {RFPROP_ServiceRoot_UUID, BMC_CLASEE_NAME, BMC_DEV_GUID_NAME, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_serviceroot_uuid, redfish_set_uuid, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_COMMON_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_service_root_oem, NULL, NULL, ETAG_FLAG_ENABLE},
    {RF_PROP_FABRIC_LINKS, "", "", USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_serviceroot_fabric_links, NULL, NULL, ETAG_FLAG_ENABLE},
};


LOCAL gint32 get_serviceroot_uuid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return redfish_get_uuid(i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 get_serviceroot_fabric_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE ep_identifier_handle = 0;
    json_object *fabric_jso = NULL;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    
    (void)dal_get_object_handle_nth(CLASS_NAME_ENDPOINT_IDENTIFIER, 0, &ep_identifier_handle);
    return_val_if_fail(0 != ep_identifier_handle, HTTP_INTERNAL_SERVER_ERROR);

    fabric_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != fabric_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s failed: alloc new json object failed", __FUNCTION__));

    json_object_object_add(fabric_jso, RFPROP_ODATA_ID, json_object_new_string(URI_FORMAT_FABRIC));

    *o_result_jso = fabric_jso;

    return HTTP_OK;
}


LOCAL void get_overview_oem(json_object *huawei, const char *uriStr, const char *objectName)
{
    json_object *odata_id = NULL;
    json_object *tmp_jso = NULL;

    return_do_info_if_expr((huawei == NULL || uriStr == NULL || objectName == NULL),
        debug_log(DLOG_ERROR, "%s:param is NULL ", __FUNCTION__));
    return_do_info_if_expr(json_object_get_type(huawei) != json_type_object,
        debug_log(DLOG_ERROR, "%s:param is not json type object", __FUNCTION__));

    odata_id = json_object_new_string(uriStr);
    return_do_info_if_expr(odata_id == NULL, debug_log(DLOG_ERROR, "%s:  json_object_new_string fail\n", __FUNCTION__);
        json_object_object_add(huawei, objectName, NULL));

    tmp_jso = json_object_new_object();
    return_do_info_if_expr(tmp_jso == NULL, json_object_put(odata_id);
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail.\n", __FUNCTION__, __LINE__);
        json_object_object_add(huawei, objectName, NULL));

    json_object_object_add(tmp_jso, RFPROP_ODATA_ID, odata_id);
    json_object_object_add(huawei, objectName, tmp_jso);

    return;
}


LOCAL void get_huawei_prop_oem(json_object *huawei)
{
    
    get_prop_string_oem(RFPROP_SERVICEROOT_SECURITY_BANNER, CLASS_SECURITY_ENHANCE, PROPERTY_SECURITY_BANNERCONTENT,
        huawei, FALSE);

    
    get_prop_string_oem(RFPROP_SERVICEROOT_PRODUCT_NAME, BMC_CLASEE_NAME, PROPERTY_BMC_SYSTEMNAME, huawei, FALSE);

    
    get_prop_string_oem(RFPROP_SERVICEROOT_PRODUCT_SN, CLASS_ELABEL, PROPERTY_ELABEL_PRODUCT_SN, huawei, TRUE);

    
    get_prop_string_oem(RFPROP_SERVICEROOT_HOST_NAME, BMC_CLASEE_NAME, PROPERTY_BMC_HOSTNAME, huawei, FALSE);

    
    get_prop_string_oem(RFPROP_SERVICEROOT_LANGUAGE_SET, BMC_CLASEE_NAME, PROPERTY_BMC_LANGUAGESET, huawei, FALSE);

    
    get_prop_string_oem(RFPROP_SERVICEROOT_COPYRIGHT, BMC_CONTACT_NAME, BMC_COPYRIGHT, huawei, FALSE);

    
    get_prop_string_oem(RFPROP_SERVICEROOT_DOWNLOAD_KVM_LINK, BMC_CONTACT_NAME, BMC_DOWNLOADKVMLINK, huawei, FALSE);

    
    get_prop_boolean_oem(RFPROP_SERVICEROOT_DOC_SUPPORT_FLAG, BMC_CONTACT_NAME, BMC_DOC_SUPPORT_FLAG, huawei);

    
    get_prop_array_oem(RFPROP_SERVICEROOT_DOMAIN_NAME, LDAP_CLASS_NAME, LDAP_ATTRIBUTE_USER_DOMAIN, huawei);

    
    get_prop_string_oem(RFPROP_SERVICEROOT_PRODUCT_PIC, BMC_PRODUCT_NAME_APP, BMC_PRODUCT_PICTURE, huawei, FALSE);

    
    get_prop_int_oem(RFPROP_ACCOUNTSERVICE_LOCKOUTDURATION, CLASS_SECURITY_ENHANCE,
        PROPERTY_AUTH_FAIL_LOCK_TIME_EXTENSION, huawei);
    

    
    get_prop_string_oem(RFPROP_SERVICEROOT_SMSNAME, CLASS_SMS, PROPERTY_SMS_SMSNAME, huawei, FALSE);

    
    get_prop_string_oem(RFPROP_SERVICEROOT_SOFTWARE_NAME, BMC_CLASEE_NAME, BMC_SOFTWARE_NAME, huawei, FALSE);

    return;
}


LOCAL gint32 get_service_root_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    json_object *huawei = NULL;
    json_object *array_json = NULL;
    guint8 data_acquire_support = 0;
    OBJ_HANDLE rack_mgnt_handle = 0;

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    huawei = json_object_new_object();
    return_val_do_info_if_expr(huawei == NULL, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(array_json);
        debug_log(DLOG_ERROR, "%s : json_object_new_object fail", __FUNCTION__));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei);
        debug_log(DLOG_ERROR, "%s : json_object_new_object fail", __FUNCTION__));

    json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei);
    get_sms_oem(huawei);

    
    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROPERTY_PME_SERVICECONFIG_DATA_ACQUISITION_SUPPORT,
        &data_acquire_support);
    if (data_acquire_support == ENABLE) {
        get_data_acquisition_service_oem(huawei);
    }
    

    
    (void)dal_get_object_handle_nth(CLASS_RACK_ASSET_MGMT, 0, &rack_mgnt_handle);
    do_if_expr((rack_mgnt_handle != 0), get_rack_mgnt_service_oem(huawei));
    

    
    guint8 software_type = 0;
    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_EM || software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT ||
        rm_is_support_chassis_overview()) {
        get_overview_oem(huawei, RF_CHASSISOVERVIEW_URI, RF_CHASSISOVERVIEW);
    } else {
        get_overview_oem(huawei, RF_SYSTEMOVERVIEW_URI, RF_SYSTEMOVERVIEW);
    }

    get_huawei_prop_oem(huawei);
    
    
    get_prop_boolean_oem(RFPROP_SERVICEROOT_KRB_STATE, KRBCOMMON_CLASS_NAME, KRBCOMMON_ATTRIBUTE_ENABLE, huawei);
    

    
    get_prop_double_oem(RFPROP_SERVICEROOT_CHASSIS_HEIGHT_U, CLASS_NAME_PRODUCT, PROPERTY_PRODUCT_HEIGHT_U, huawei,
        chassis_height_prop_check);

    
    get_nic_fqdn(1, huawei);

    return HTTP_OK;
}

LOCAL void get_data_acquisition_service_oem(json_object *huawei)
{
    json_object *odata_id = NULL;
    json_object *tmp_jso = NULL;
    return_do_info_if_fail(NULL != huawei, debug_log(DLOG_ERROR, "%s:param is NULL ", __FUNCTION__));
    return_do_info_if_fail(json_type_object == json_object_get_type(huawei),
        debug_log(DLOG_ERROR, "%s:param is not json type object", __FUNCTION__));
    odata_id = json_object_new_string(URI_FORMAT_DA_SVC);
    goto_label_do_info_if_fail(NULL != odata_id, exit,
        debug_log(DLOG_ERROR, "%s:  json_object_new_string fail\n", __FUNCTION__));
    tmp_jso = json_object_new_object();
    goto_label_do_info_if_fail(NULL != tmp_jso, exit,
        debug_log(DLOG_ERROR, "%s, %d: get redfish_get_slot fail.\n", __FUNCTION__, __LINE__);
        json_object_put(odata_id));
    json_object_object_add(tmp_jso, RFPROP_ODATA_ID, odata_id);

exit:
    json_object_object_add(huawei, RF_DASVC, tmp_jso);

    return;
}


LOCAL void get_rack_mgnt_service_oem(json_object *huawei)
{
    json_object *odata_id = NULL;
    json_object *rack_mgnt_svc_jso = NULL;

    return_do_info_if_fail(NULL != huawei, debug_log(DLOG_ERROR, "%s:param is NULL ", __FUNCTION__));

    return_do_info_if_fail(json_type_object == json_object_get_type(huawei),
        debug_log(DLOG_ERROR, "%s:param is not json type object", __FUNCTION__));

    odata_id = json_object_new_string(URI_FORMAT_RACK_MGNT_SERVICE);
    goto_label_do_info_if_fail(NULL != odata_id, exit,
        debug_log(DLOG_ERROR, "%s: json_object_new_string odata_id fail\n", __FUNCTION__));

    rack_mgnt_svc_jso = json_object_new_object();
    goto_label_do_info_if_fail(NULL != rack_mgnt_svc_jso, exit,
        debug_log(DLOG_ERROR, "%s, %d: get redfish_get_slot fail.\n", __FUNCTION__, __LINE__);
        json_object_put(odata_id));

    json_object_object_add(rack_mgnt_svc_jso, RFPROP_ODATA_ID, odata_id);

exit:

    json_object_object_add(huawei, RFPROP_RACK_MGNT_SERVICE, rack_mgnt_svc_jso);

    return;
}


LOCAL void get_sms_oem(json_object *huawei)
{
    int iRet = -1;
    gint32 ret;
    guint8 sms_supported = 0;
    json_object *tmp_jso = NULL;
    json_object *array_json = NULL;
    json_object *odata_id = NULL;
    gchar uri[MAX_URI_LENGTH] = {0};
    OBJ_HANDLE sms_handle = 0;
    guchar sms_id = 0;
    
    guchar board_type = 0;
    

    return_do_info_if_fail(NULL != huawei, debug_log(DLOG_ERROR, "%s:param is NULL ", __FUNCTION__));
    return_do_info_if_fail(json_type_object == json_object_get_type(huawei),
        debug_log(DLOG_ERROR, "%s:param is not json type object", __FUNCTION__));

    // 判断bma是否连接
    
    ret = redfish_get_board_type(&board_type);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s:get board type fail.", __FUNCTION__));
    // 交换板不支持查询sms信息,直接返回null
    return_do_info_if_expr(BOARD_SWITCH == board_type, json_object_object_add(huawei, RF_SMSUPDATESVC, array_json));
    
    ret = dal_get_bma_connect_state(&sms_supported);

    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "%s: dal_get_bma_connect_state fail", __FUNCTION__);
        json_object_object_add(huawei, RF_SMSUPDATESVC, array_json));

    if (VOS_OK == sms_supported) {
        // 查询Sms导航信息
        ret = dal_get_specific_object_string(CLASS_SMS, PROPERTY_SMS_TYPE, PROPERTY_VALUE_SMS_TYPE_INBAND, &sms_handle);
        goto_if_expr(ret != VOS_OK, exit);
        ret = dal_get_property_value_byte(sms_handle, PROPERTY_SMS_ID, &sms_id);
        goto_label_do_info_if_fail(VOS_OK == ret, exit,
            debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte fail\n", __FUNCTION__));

        iRet = snprintf_s(uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s%u/%s", RF_SMS, sms_id, RF_UPDATESVC);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

        odata_id = json_object_new_string(uri);
        goto_label_do_info_if_fail(NULL != odata_id, exit,
            debug_log(DLOG_ERROR, "%s:  json_object_new_string fail\n", __FUNCTION__));

        tmp_jso = json_object_new_object();
        goto_label_do_info_if_fail(NULL != tmp_jso, exit,
            debug_log(DLOG_ERROR, "%s, %d: get redfish_get_slot fail.\n", __FUNCTION__, __LINE__);
            json_object_put(odata_id));

        
        json_object_object_add(tmp_jso, RFPROP_ODATA_ID, odata_id);
        array_json = json_object_new_array();
        goto_label_do_info_if_fail(NULL != tmp_jso, exit, (void)json_object_put(tmp_jso);
            debug_log(DLOG_ERROR, "%s: json_object_new_array fail\n", __FUNCTION__));

        ret = json_object_array_add(array_json, tmp_jso);
        goto_label_do_info_if_fail(NULL != tmp_jso, exit, (void)json_object_put(tmp_jso);
            (void)json_object_put(array_json); debug_log(DLOG_ERROR, "%s:json_object_array_add", __FUNCTION__);
            array_json = NULL);
    } else {
        json_object_object_add(huawei, RF_SMSUPDATESVC, array_json);
        return;
    }

exit:
    json_object_object_add(huawei, RF_SMSUPDATESVC, array_json);

    return;
}


LOCAL void get_prop_boolean_oem(gchar *prop_name, gchar *pme_class_name, gchar *pme_prop_name, json_object *huawei)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar prop_value = 0;
    json_object *prop_json = NULL;

    return_do_info_if_fail(NULL != huawei, debug_log(DLOG_ERROR, "%s: param is NULL.\n", __FUNCTION__));
    return_do_info_if_fail(json_type_object == json_object_get_type(huawei),
        debug_log(DLOG_ERROR, "%s: param is not json type object.\n", __FUNCTION__));

    ret = dal_get_object_handle_nth(pme_class_name, 0, &obj_handle);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "%s: get %s object fail, ret is %d.\n", __FUNCTION__, pme_class_name, ret));

    ret = dal_get_property_value_byte(obj_handle, pme_prop_name, &prop_value);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "%s: get %s prop_value fail, ret is %d.\n", __FUNCTION__, pme_prop_name, ret));

    prop_json = json_object_new_boolean(prop_value ? TRUE : FALSE);

exit:
    json_object_object_add(huawei, prop_name, prop_json);
    return;
}


LOCAL gint32 chassis_height_prop_check(double value)
{
    
    return_val_if_expr((value <= 0), VOS_ERR);

    return VOS_OK;
}


LOCAL void get_prop_int_oem(gchar *prop_name, gchar *pme_class_name, gchar *pme_prop_name, json_object *huawei)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gint32 prop_value = 0;
    json_object *prop_json = NULL;

    
    return_do_info_if_fail(NULL != huawei, debug_log(DLOG_ERROR, "%s: param is NULL.\n", __FUNCTION__));
    return_do_info_if_fail(json_type_object == json_object_get_type(huawei),
        debug_log(DLOG_ERROR, "%s: param is not json type object.\n", __FUNCTION__));

    
    (void)dal_get_object_handle_nth(pme_class_name, 0, &obj_handle);

    
    ret = dal_get_property_value_int32(obj_handle, pme_prop_name, &prop_value);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "%s: get %s prop_value fail, ret is %d.\n", __FUNCTION__, prop_name, ret));

    
    prop_json = json_object_new_int(prop_value);

exit:
    json_object_object_add(huawei, prop_name, prop_json);
}


LOCAL void get_prop_double_oem(gchar *prop_name, gchar *pme_class_name, gchar *pme_prop_name, json_object *huawei,
    double_prop_handle pfn_prop_handle)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gdouble prop_value = 0;
    json_object *prop_json = NULL;

    
    return_do_info_if_expr((json_object_get_type(huawei) != json_type_object),
        debug_log(DLOG_ERROR, "%s: param is not json type object.", __FUNCTION__));

    
    ret = dal_get_object_handle_nth(pme_class_name, 0, &obj_handle);
    goto_label_do_info_if_expr((ret != VOS_OK), exit,
        debug_log(DLOG_ERROR, "%s: get %s object fail, ret is %d.", __FUNCTION__, pme_class_name, ret));

    
    ret = dal_get_property_value_double(obj_handle, pme_prop_name, &prop_value);
    goto_label_do_info_if_expr((ret != VOS_OK), exit,
        debug_log(DLOG_ERROR, "%s: get %s prop_value fail, ret is %d.", __FUNCTION__, prop_name, ret));

    
    if (pfn_prop_handle != NULL) {
        ret = pfn_prop_handle(prop_value);
        return_if_expr((ret != VOS_OK));
    }

    
    prop_json = ex_json_object_new_double(prop_value, "%.2f");

exit:
    json_object_object_add(huawei, prop_name, prop_json);

    return;
}


LOCAL void get_prop_string_oem(gchar *prop_name, gchar *pme_class_name, gchar *pme_prop_name, json_object *huawei,
    json_bool prop_check)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar state = 0;
    gchar prop_value[MAX_OEM_PROP_VAL_LEN + 1] = {0};
    json_object *prop_json = NULL;

    
    return_do_info_if_fail(NULL != huawei, debug_log(DLOG_ERROR, "%s: param is NULL.\n", __FUNCTION__));
    return_do_info_if_fail(json_type_object == json_object_get_type(huawei),
        debug_log(DLOG_ERROR, "%s: param is not json type object.\n", __FUNCTION__));

    
    ret = dal_get_object_handle_nth(pme_class_name, 0, &obj_handle);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_DEBUG, "%s: get %s obj_handle fail, ret is %d.\n", __FUNCTION__, pme_class_name, ret));

    
    if (0 == strcmp(RFPROP_SERVICEROOT_SECURITY_BANNER, prop_name)) {
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_SECURITY_BANNERSTATE, &state);
        goto_label_do_info_if_fail(VOS_OK == ret, exit,
            debug_log(DLOG_ERROR, "%s: get banner states fail, ret is %d.\n", __FUNCTION__, ret));

        
        if (STATE_ENABLE != state) {
            goto exit;
        }
    }

    
    if (strcmp(PROPERTY_BMC_LANGUAGESET, prop_name) == 0) {
        ret = dal_get_language_set(obj_handle, TRUE, prop_value, sizeof(prop_value));
    } else {
        ret = dal_get_property_value_string(obj_handle, pme_prop_name, prop_value, sizeof(prop_value));
    }
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "%s: get %s prop_value fail, ret is %d.\n", __FUNCTION__, prop_name, ret));

    
    if (prop_check) {
        return_do_info_if_expr((rf_string_check(prop_value) != VOS_OK),
            json_object_object_add(huawei, prop_name, NULL));
    }

    
    prop_json = json_object_new_string(prop_value);
    goto_label_do_info_if_fail(NULL != prop_json, exit,
        debug_log(DLOG_ERROR, "%s: new %s prop_json fail.\n", __FUNCTION__, prop_name));

exit:
    json_object_object_add(huawei, prop_name, prop_json);

    return;
}


LOCAL void get_prop_array_oem(gchar *prop_name, gchar *pme_class_name, gchar *pme_prop_name, json_object *huawei)
{
    gint32 ret = 0;
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE iter_handle = 0;
    guchar ldap_state = 0;
    guchar krb_state = 0;
    GSList *obj_list = NULL;
    GSList *obj_list_iter = NULL;
    gchar prop_value[MAX_OEM_PROP_VAL_LEN + 1] = {0};
    json_object *prop_json = NULL;
    json_object *prop_json_array = NULL;
    guint8 ldap_id = 0;
    gchar krb_domain_str[MAX_OEM_PROP_VAL_LEN + 1] = {0};

    
    return_do_info_if_fail(NULL != huawei, debug_log(DLOG_ERROR, "%s: param is NULL.\n", __FUNCTION__));
    return_do_info_if_fail(json_type_object == json_object_get_type(huawei),
        debug_log(DLOG_ERROR, "%s: param is not json type object.\n", __FUNCTION__));

    
    if (0 == strcmp(RFPROP_SERVICEROOT_DOMAIN_NAME, prop_name)) {
        
        
        ret = dal_get_object_handle_nth(LDAPCOMMON_CLASS_NAME, 0, &obj_handle);
        if (ret == VOS_OK) {
            (void)dal_get_property_value_byte(obj_handle, LDAPCOMMON_ATTRIBUTE_ENABLE, &ldap_state);
        }

        
        ret = dal_get_object_handle_nth(KRBCOMMON_CLASS_NAME, 0, &obj_handle);
        if (ret == VOS_OK) {
            (void)dal_get_property_value_byte(obj_handle, KRBCOMMON_ATTRIBUTE_ENABLE, &krb_state);
        }
        
        if ((STATE_ENABLE != ldap_state) && (STATE_ENABLE != krb_state)) {
            goto exit;
        }
        
    }

    
    prop_json_array = json_object_new_array();
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "%s: new %s prop_json_array fail.\n", __FUNCTION__, prop_name));

    
    ret = dfl_get_object_list(pme_class_name, &obj_list);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "%s: get %s obj_list fail, ret is %d.\n", __FUNCTION__, pme_class_name, ret));

    
    for (obj_list_iter = obj_list; NULL != obj_list_iter; obj_list_iter = obj_list_iter->next) {
        
        iter_handle = (OBJ_HANDLE)obj_list_iter->data;

        ret = dal_get_property_value_byte(iter_handle, LDAP_ATTRIBUTE_ID, &ldap_id);
        goto_label_do_info_if_fail(VOS_OK == ret, exit,
            debug_log(DLOG_ERROR, "%s: get %s prop_value fail, ret is %d.\n", __FUNCTION__, LDAP_ATTRIBUTE_ID, ret));

        if ((ldap_id < LOG_TYPE_LDAP_SERVER6) && (ldap_state == STATE_ENABLE)) {
            
            ret = dal_get_property_value_string(iter_handle, pme_prop_name, prop_value, sizeof(prop_value));
            goto_label_do_info_if_fail(VOS_OK == ret, exit,
                debug_log(DLOG_ERROR, "%s: get %s prop_value fail, ret is %d.\n", __FUNCTION__, prop_name, ret));

            
            prop_json = json_object_new_string(prop_value);
            goto_label_do_info_if_fail(NULL != prop_json, exit,
                debug_log(DLOG_ERROR, "%s: new %s prop_json fail.\n", __FUNCTION__, prop_name));

            
            ret = json_object_array_add(prop_json_array, prop_json);
            goto_label_do_info_if_fail(VOS_OK == ret, exit,
                json_object_put(prop_json); //  如果添加失败，需释放prop_json
                debug_log(DLOG_ERROR, "%s: array add %s prop_json fail, ret is %d.\n", __FUNCTION__, prop_name, ret));
        } else if ((ldap_id >= LOG_TYPE_LDAP_SERVER6) && (krb_state == STATE_ENABLE)) {
            
            ret = dal_get_property_value_string(iter_handle, pme_prop_name, prop_value, sizeof(prop_value) - 1);
            goto_label_do_info_if_fail(VOS_OK == ret, exit,
                debug_log(DLOG_ERROR, "%s: get %s prop_value fail, ret is %d.\n", __FUNCTION__, prop_name, ret));
            
            if (strlen(prop_value)) {
                ret = snprintf_s(krb_domain_str, sizeof(krb_domain_str), sizeof(krb_domain_str) - 1, "%s(KRB)",
                    prop_value);
                do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret));
            }

            
            prop_json = json_object_new_string(krb_domain_str);
            goto_label_do_info_if_fail(NULL != prop_json, exit,
                debug_log(DLOG_ERROR, "%s: new %s prop_json fail.\n", __FUNCTION__, prop_name));

            
            ret = json_object_array_add(prop_json_array, prop_json);
            goto_label_do_info_if_fail(VOS_OK == ret, exit,
                json_object_put(prop_json); //  如果添加失败，需释放prop_json
                debug_log(DLOG_ERROR, "%s: array add %s prop_json fail, ret is %d.\n", __FUNCTION__, prop_name, ret));
        }
    }

exit:
    
    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));

    json_object_object_add(huawei, prop_name, prop_json_array);
}


gint32 serviceroot_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    *prop_provider = g_serviceroot_provider;
    *count = sizeof(g_serviceroot_provider) / sizeof(PROPERTY_PROVIDER_S);

    update_irm_inner_access_node_timestamp(i_paras->client, RF_SERVICE_ROOT);

    return VOS_OK;
}