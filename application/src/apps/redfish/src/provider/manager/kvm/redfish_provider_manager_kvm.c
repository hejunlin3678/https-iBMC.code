
#include "redfish_provider.h"
#define KVM_JNLP_FILE_FORMAT \
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
         <jnlp spec=\"1.0+\" codebase=\"https://%s\">\n\
		 <information>\n\
		 <title>Remote Virtual Console   IP :%s   SN: %s</title>\n\
		      <vendor>iBMC</vendor>\n\
	  </information>\n\
		      <resources>\n\
			      <j2se version=\"1.7+\" />\n\
			      <jar href=\"/bmc/pages/jar/%s?authParam=%u\" main=\"true\"/>\n\
		      </resources>\n\
		      <applet-desc name=\"Remote Virtual Console   IP :%s   SN: %s\" main-class=\"com.kvm.KVMApplet\" width=\"950\" height=\"700\" >\n\
			      <param name=\"verifyValue\" value=\"%u\"/>\n\
			      <param name=\"mmVerifyValue\" value=\"%u\"/>\n\
			      <param name=\"decrykey\" value=\"%s\"/>\n\
			      <param name=\"local\" value=\"%s\"/>\n\
			      <param name=\"compress\" value=\"%d\"/>\n\
			      <param name=\"vmm_compress\" value=\"%d\"/>\n\
			      <param name=\"port\" value=\"%d\"/>\n\
			      <param name=\"vmmPort\" value=\"%d\"/>\n\
			      <param name=\"privilege\" value=\"%u\"/>\n\
			      <param name=\"bladesize\" value=\"1\"/>\n\
			      <param name=\"IPA\" value=\"%s\"/>\n\
			      <param name=\"IPB\" value=\"%s\"/>\n\
			      <param name=\"verifyValueExt\" value=\"%s\"/>\n\
			      <param name=\"title\" value=\" IP :%s   SN :%s\"/>\n\
			      <param name=\"capsSync\" value=\"%u\"/>\n\
		     </applet-desc>\n\
		     <security>\n\
			      <all-permissions/>\n\
		     </security>\n\
	  </jnlp>"


LOCAL gint32 get_kvmproperty_value_gint32(const gchar *class_name, const gchar *property_name, gint32 *out_val)
{
    gint32 ret;
    OBJ_HANDLE ntp_handle = 0;
    ret = dal_get_object_handle_nth(class_name, 0, &ntp_handle);
    
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));
    
    ret = dal_get_property_value_int32(ntp_handle, property_name, out_val);
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_property_value_byte fail. ", __FUNCTION__, __LINE__));
    return VOS_OK;
}

LOCAL gint32 get_numberofactivatedsessions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 kvm_num = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_kvmproperty_value_gint32(KVM_CLASS_NAME, KVM_PROPERTY_NUM, &kvm_num);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: get_kvmproperty_value_gint32 fail. ", __FUNCTION__, __LINE__));
    *o_result_jso = json_object_new_int(kvm_num);
    return HTTP_OK;
}


LOCAL gint32 get_activated_session_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *node = NULL;
    OBJ_HANDLE obj_handle = 0;
    GSList *kvm_link_list = NULL;
    gint32 link_mode = G_MAXUINT8;
    gchar prop_ip[SESSION_IP_LEN + 1] = {0};

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = dfl_get_object_list(KVMLINK_CLASS_NAME, &kvm_link_list);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get kvm link list fail(%d).", __FUNCTION__, ret));

    for (node = kvm_link_list; node; node = g_slist_next(node)) {
        
        obj_handle = (OBJ_HANDLE)node->data;
        (void)dal_get_property_value_string(obj_handle, KVMLINK_PROPERTY_IP, prop_ip, SESSION_IP_LEN + 1);
        continue_if_expr(0 == g_strcmp0(prop_ip, "0"));

        (void)dal_get_property_value_int32(obj_handle, KVMLINK_PROPERTY_MODE, &link_mode);
        if (USER_KVM_LINK_MODE_SHARED == link_mode) {
            *o_result_jso = json_object_new_string(USER_KVM_LINK_MODE_SHARED_STR);
            break;
        } else if (USER_KVM_LINK_MODE_PRIVATE == link_mode) {
            *o_result_jso = json_object_new_string(USER_KVM_LINK_MODE_PRIVATE_STR);
            break;
        }
    }

    g_slist_free(kvm_link_list);
    return HTTP_OK;
}


LOCAL gint32 get_visible_java_kvm(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 support_java_kvm = 0;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }
   
    ret = get_kvmproperty_value_gint32(KVM_CLASS_NAME, KVM_PROPERTY_VISIBLE_JAVAKVM, &support_java_kvm);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get_kvmproperty_value_gint32 fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (support_java_kvm == 1) {
        *o_result_jso = json_object_new_boolean(TRUE);
    } else {
        *o_result_jso = json_object_new_boolean(FALSE);
    }

    return HTTP_OK;
}


LOCAL gint32 get_kvmencryption_configurable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    return_val_do_info_if_expr(
        (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_expr(0 == i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

#ifdef COMPATIBILITY_ENABLED
    *o_result_jso = json_object_new_boolean(TRUE);
#else
    *o_result_jso = json_object_new_boolean(FALSE);
#endif

    return HTTP_OK;
}


LOCAL gint32 get_kvmencryption(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
#ifdef COMPATIBILITY_ENABLED
    gint32 ret;
    gint32 kvmencryption = 0;
#endif

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

#ifdef COMPATIBILITY_ENABLED
    ret = get_kvmproperty_value_gint32(KVM_CLASS_NAME, KVM_PROPERTY_COMPRESS_STATE, &kvmencryption);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: get_kvmproperty_value_gint32 fail. ", __FUNCTION__, __LINE__));
    (1 == kvmencryption) ? (*o_result_jso = json_object_new_boolean(TRUE)) :
                           (*o_result_jso = json_object_new_boolean(FALSE));
#else
    *o_result_jso = json_object_new_boolean(TRUE);
#endif

    return HTTP_OK;
}


#ifdef COMPATIBILITY_ENABLED
LOCAL gint32 get_vmm_encryption(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso)
{
    gint32 ret;
    gint32 vmm_encryption = 0;
    
    ret = get_kvmproperty_value_gint32(PROXY_VMM_CLASS_NAME, VMM_PROPERTY_COMPRESS_STATE, &vmm_encryption);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));
    
    if (0 != vmm_encryption) {
        (void)create_message_info(MSG_KVM_SET_ENCRYPTION_FAIL, NULL, o_message_jso);
        return VOS_ERR;
    }

    return VOS_OK;
}
#endif


LOCAL gint32 set_kvmencryption(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
#ifdef COMPATIBILITY_ENABLED
    gint32 ret;
    json_bool kvm_bool;
    gint32 s_encryption;
    GSList *input_list = NULL;
    OBJ_HANDLE kvm_handle = 0;
#endif

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_KVMMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_MANAGER_KVMENCRYPTION, o_message_jso,
        PROPERTY_MANAGER_KVMENCRYPTION));
    

#ifdef COMPATIBILITY_ENABLED
    kvm_bool = json_object_get_boolean(i_paras->val_jso);
    (TRUE == kvm_bool) ? (s_encryption = 1) : (s_encryption = 0);

    ret = dal_get_object_handle_nth(PROXY_KVM_CLASS_NAME, 0, &kvm_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));

    if (0 == s_encryption) {
        
        ret = get_vmm_encryption(i_paras, o_message_jso);
        return_val_if_expr(VOS_OK != ret, ret);
    }

    input_list = g_slist_append(input_list, g_variant_new_int32(s_encryption));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, kvm_handle,
        PROXY_KVM_CLASS_NAME, KVM_METHOD_SET_ENCRYPT_STATE, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

        case MANAGER_KVM_IS_IN_USE:
            
            (void)create_message_info(MANAGER_KVM_MESSAGE_KVMUSE, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
#else
    (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, PROPERTY_MANAGER_KVMENCRYPTION, o_message_jso,
        PROPERTY_MANAGER_KVMENCRYPTION);
    return HTTP_NOT_IMPLEMENTED;
#endif
}


LOCAL gint32 get_keyboardandmouse(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 kvm_usb = 0;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    ret = get_kvmproperty_value_gint32(KVM_CLASS_NAME, KVM_PROPERTY_KEYBOARDMODE, &kvm_usb);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: get_kvmproperty_value_gint32 fail. ", __FUNCTION__, __LINE__));
    (1 == kvm_usb) ? (*o_result_jso = json_object_new_boolean(TRUE)) : (*o_result_jso = json_object_new_boolean(FALSE));
    return HTTP_OK;
}

LOCAL gint32 set_keyboardandmouse(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 set_kvm_usb;
    json_bool kvm_bool;
    GSList *input_list = NULL;
    OBJ_HANDLE kvm_handle = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_KVMMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_MANAGER_KVMMOUSE, o_message_jso,
        PROPERTY_MANAGER_KVMMOUSE));
    

    kvm_bool = json_object_get_boolean(i_paras->val_jso);
    (TRUE == kvm_bool) ? (set_kvm_usb = 1) : (set_kvm_usb = 0);
    ret = dal_get_object_handle_nth(KVM_CLASS_NAME, 0, &kvm_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_int32(set_kvm_usb));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, kvm_handle,
        KVM_CLASS_NAME, KVM_METHOD_SETKEYBOARDMODE, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 get_auto_os_lock_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 state = 0;

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    return_val_do_info_if_expr(!i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_kvmproperty_value_gint32(KVM_CLASS_NAME, KVM_PROPERTY_AUTO_OS_LOCK_STATE, &state);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: get_kvmproperty_value_gint32 fail. ", __FUNCTION__, __LINE__));
    (state == 1) ? (*o_result_jso = json_object_new_boolean(TRUE)) : (*o_result_jso = json_object_new_boolean(FALSE));
    return HTTP_OK;
}


LOCAL gint32 set_auto_os_lock_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 set_state;
    json_bool state_bool;
    GSList *input_list = NULL;
    OBJ_HANDLE kvm_handle = 0;

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    return_val_do_info_if_expr(!(i_paras->user_role_privilege & USERROLE_KVMMGNT), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_AUTO_OS_LOCK_ENABLED, o_message_jso,
        RFPROP_MANAGER_AUTO_OS_LOCK_ENABLED));

    state_bool = json_object_get_boolean(i_paras->val_jso);
    (state_bool == TRUE) ? (set_state = 1) : (set_state = 0);

    ret = dal_get_object_handle_nth(KVM_CLASS_NAME, 0, &kvm_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_byte(set_state));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, kvm_handle,
        KVM_CLASS_NAME, KVM_METHOD_SET_AUTO_OS_LOCK_STATE, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);

    return_val_if_expr(ret == VOS_OK, HTTP_OK);
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 get_auto_os_lock_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 type = 0;

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    return_val_do_info_if_expr(!i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_kvmproperty_value_gint32(KVM_CLASS_NAME, KVM_PROPERTY_AUTO_OS_LOCK_TYPE, &type);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: get_kvmproperty_value_gint32 fail. ", __FUNCTION__, __LINE__));
    (type == 1) ? (*o_result_jso = json_object_new_string(USER_KVM_AUTO_OS_LOCK_TYPE_CUSTOM_STR)) :
                  (*o_result_jso = json_object_new_string(USER_KVM_AUTO_OS_LOCK_TYPE_WINDOWS_STR));
    return HTTP_OK;
}


LOCAL gint32 set_auto_os_lock_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    const gchar *type = NULL;
    gint32 set_type;
    GSList *input_list = NULL;
    OBJ_HANDLE kvm_handle = 0;

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    return_val_do_info_if_expr(!(i_paras->user_role_privilege & USERROLE_KVMMGNT), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_AUTO_OS_LOCK_TYPE, o_message_jso,
        RFPROP_MANAGER_AUTO_OS_LOCK_TYPE));

    type = dal_json_object_get_str(i_paras->val_jso);
    // type值检查
    if (g_strcmp0(type, USER_KVM_AUTO_OS_LOCK_TYPE_CUSTOM_STR) != 0 &&
        g_strcmp0(type, USER_KVM_AUTO_OS_LOCK_TYPE_WINDOWS_STR) != 0) {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_MANAGER_AUTO_OS_LOCK_TYPE, o_message_jso, type,
            RFPROP_MANAGER_AUTO_OS_LOCK_TYPE);
        return HTTP_BAD_REQUEST;
    }

    g_strcmp0(type, USER_KVM_AUTO_OS_LOCK_TYPE_CUSTOM_STR) == 0 ? (set_type = 1) : (set_type = 0);
    ret = dal_get_object_handle_nth(KVM_CLASS_NAME, 0, &kvm_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_byte(set_type));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, kvm_handle,
        KVM_CLASS_NAME, KVM_METHOD_SET_AUTO_OS_LOCK_TYPE, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    return_val_if_expr(ret == VOS_OK, HTTP_OK);

    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 get_auto_os_lock_key(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    GVariant *key_gv = NULL;
    const gchar **key_as = NULL;
    gsize free_count = 0;
    gsize idx = 0;
    gint32 ret_val;
    json_object *free_block_jso = 0;
    OBJ_HANDLE kvm_handle = 0;

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    return_val_do_info_if_expr(!i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret_val = dal_get_object_handle_nth(KVM_CLASS_NAME, 0, &kvm_handle);
    return_val_do_info_if_expr(ret_val != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));

    free_block_jso = json_object_new_array();
    if (free_block_jso == NULL) {
        debug_log(DLOG_ERROR, "%s failed: alloc new json array failed.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret_val = dfl_get_property_value(kvm_handle, KVM_PROPERTY_AUTO_OS_LOCK_KEY, &key_gv);
    if (ret_val == DFL_OK) {
        key_as = g_variant_get_strv(key_gv, &free_count);
        if (key_as != NULL) {
            for (idx = 0; idx < free_count; idx++) {
                json_object_array_add(free_block_jso, json_object_new_string(key_as[idx]));
            }
            g_free(key_as);
        }
        g_variant_unref(key_gv);
    } else {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        g_variant_unref(key_gv);
        (void)json_object_put(free_block_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = free_block_jso;
    return HTTP_OK;
}


LOCAL gint32 set_auto_os_lock_key(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    const gchar* key[AUTO_LOCK_KEY_ARRAY_LENGTH] = {NULL};
    GSList *input_list = NULL;
    OBJ_HANDLE kvm_handle = 0;
    json_object *temp_jso = NULL;
    guint32 len_array;
    guint32 i;
    guint32 empty_count = 0;

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    return_val_do_info_if_expr(!(i_paras->user_role_privilege & USERROLE_KVMMGNT), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_AUTO_OS_LOCK_KEY, o_message_jso,
        RFPROP_MANAGER_AUTO_OS_LOCK_KEY));

    ret = dal_get_object_handle_nth(KVM_CLASS_NAME, 0, &kvm_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth fail. ", __FUNCTION__, __LINE__));

    len_array = json_object_array_length(i_paras->val_jso);
    // 数组长度超出限制场景，需要提前判断，否则下面key数组越界
    return_val_do_info_if_expr(len_array > AUTO_LOCK_KEY_ARRAY_LENGTH, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, RFPROP_MANAGER_AUTO_OS_LOCK_KEY, o_message_jso,
        RFPROP_MANAGER_AUTO_OS_LOCK_KEY));

    // redfish不进行提前key合法性校验，KVM类方法有判断，防止重复判断以及提前判断导致操作日志无法记录
    for (i = 0; i < len_array; i++) {
        temp_jso = json_object_array_get_idx(i_paras->val_jso, i);
        key[i] = dal_json_object_get_str(temp_jso);
        debug_log(DLOG_DEBUG, "set key[%d]=%s.\n", i, key[i]);
    }

    input_list = g_slist_append(input_list, g_variant_new_strv((const gchar * const *)key, len_array));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, kvm_handle,
        KVM_CLASS_NAME, KVM_METHOD_SET_AUTO_OS_LOCK_KEY, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    temp_jso = NULL;
    return_val_if_expr(ret == VOS_OK, HTTP_OK);

    // 判断错误场景,填充错误消息
    for (i = 0; i < len_array; i++) {
        if (g_ascii_strcasecmp(key[i], "") == 0) {
            empty_count++;
        } else {
            ret = dal_check_valid_key(key[i]);
            // Key值不在支持列表
            return_val_do_info_if_expr(ret != VOS_OK, HTTP_BAD_REQUEST,
                (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_MANAGER_AUTO_OS_LOCK_KEY, o_message_jso,
                key[i], RFPROP_MANAGER_AUTO_OS_LOCK_KEY));
        }
    }
    // 数组的值全空
    return_val_do_info_if_expr(empty_count == len_array, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_MANAGER_AUTO_OS_LOCK_KEY, o_message_jso, "empty",
        RFPROP_MANAGER_AUTO_OS_LOCK_KEY));

    // 系统内部调用报错
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 get_timeoutperiod(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 kvm_timeout = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_kvmproperty_value_gint32(KVM_CLASS_NAME, KVM_PROPERTY_TIMEOUT, &kvm_timeout);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: get_kvmproperty_value_gint32 fail. ", __FUNCTION__, __LINE__));
    *o_result_jso = json_object_new_int(kvm_timeout);
    return HTTP_OK;
}

LOCAL gint32 set_timeoutperiod(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
#define MAX_KVM_TIMEOUT_SET_CNT 10
    gint32 ret;
    gint32 set_kvm_timeout;
    GSList *input_list = NULL;
    OBJ_HANDLE kvm_handle = 0;
    guint32 retry_time = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_KVMMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_MANAGER_KVMTIMOUT, o_message_jso,
        PROPERTY_MANAGER_KVMTIMOUT));
    

    
    if (TRUE == json_object_is_type(i_paras->val_jso, json_type_double)) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, PROPERTY_MANAGER_KVMTIMOUT, o_message_jso,
            dal_json_object_get_str(i_paras->val_jso), PROPERTY_MANAGER_KVMTIMOUT);

        return HTTP_BAD_REQUEST;
    }

    set_kvm_timeout = json_object_get_int(i_paras->val_jso);

    
    ret = dal_get_object_handle_nth(KVM_CLASS_NAME, 0, &kvm_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_int32(set_kvm_timeout));
    ret = RPC_UNKNOWN;

    while (retry_time < MAX_KVM_TIMEOUT_SET_CNT && ret == RPC_UNKNOWN) {
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, kvm_handle,
            KVM_CLASS_NAME, KVM_METHOD_SET_TIMEOUT, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
        retry_time++; // 如果设置过KVM端口等会触发kvm_vmm进程重启，此时调用方法返回-200±X，增加重试等进程恢复保证设置成功
        do_if_expr(dal_is_rpc_exception(ret) == TRUE, ret = RPC_UNKNOWN; vos_task_delay(1000));
    }

    uip_free_gvariant_list(input_list);
    

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 get_manager_kvm_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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
    ret = snprintf_s(slot_uri, sizeof(slot_uri), sizeof(slot_uri) - 1, MANAGER_KVMSERVICE_ODATAID, slot);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));
    *o_result_jso = json_object_new_string((const gchar *)slot_uri);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 get_manager_vmm_kvm_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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

    ret = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, MANAGER_KVMSERVICE_CONTEXT,
        slot_id);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string((const gchar *)odata_context);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 get_manager_kvmservice_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;

    json_object *obj_jso_setkey = NULL;
    json_object *obj_jso_export_kvmjnlp = NULL;

    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar target_uri[PSLOT_URI_LEN] = {0};
    gchar action_info[PSLOT_URI_LEN] = {0};
    gchar kvm_action_info[PSLOT_URI_LEN] = {0};
    gchar kvmjnlp_target_uri[PSLOT_URI_LEN] = {0};

    gint32 ret;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function return err of redfish_slot_id.", __FUNCTION__, __LINE__));
    ret = snprintf_s(target_uri, sizeof(target_uri), sizeof(target_uri) - 1, KVM_ACTION_TARGET_URI, slot);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));

    ret = snprintf_s(action_info, sizeof(action_info), sizeof(action_info) - 1, KVM_ACTION_INFO_URI, slot);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));

    obj_jso_setkey = json_object_new_object();
    return_val_do_info_if_fail(NULL != obj_jso_setkey, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_object fail.", __FUNCTION__, __LINE__));

    json_object_object_add(obj_jso_setkey, RFPROP_TARGET, json_object_new_string((const gchar *)target_uri));
    json_object_object_add(obj_jso_setkey, ACTION_INFO, json_object_new_string((const gchar *)action_info));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(obj_jso_setkey);
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_object fail.", __FUNCTION__, __LINE__));

    json_object_object_add(*o_result_jso, KVM_ACTION_PROP_NAME, obj_jso_setkey);

    
    obj_jso_export_kvmjnlp = json_object_new_object();
    return_val_do_info_if_fail(NULL != obj_jso_export_kvmjnlp, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_kvm_jnlp_object fail.", __FUNCTION__, __LINE__));

    iRet = snprintf_s(kvmjnlp_target_uri, sizeof(kvmjnlp_target_uri), sizeof(kvmjnlp_target_uri) - 1,
        KVM_ACTION_KVMJNLP_TARGET_RUI, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    iRet = snprintf_s(kvm_action_info, sizeof(kvm_action_info), sizeof(kvm_action_info) - 1,
        KVM_ACTION_KVMJNLP_INFO_URI, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    json_object_object_add(obj_jso_export_kvmjnlp, RFPROP_TARGET,
        json_object_new_string((const gchar *)kvmjnlp_target_uri));
    json_object_object_add(obj_jso_export_kvmjnlp, ACTION_INFO, json_object_new_string((const gchar *)kvm_action_info));

    json_object_object_add(*o_result_jso, KVM_ACTION_KVMJNLP_NAME, obj_jso_export_kvmjnlp);

    return HTTP_OK;
}


LOCAL gint32 get_key_prop_val(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, const gchar *prop_name,
    json_object **prop_jso)
{
    json_bool bool_jso;
    json_bool del_flag = FALSE;

    
    (void)check_delete_property_by_class(i_paras->val_jso, prop_name, &del_flag);
    return_val_if_expr(del_flag, VOS_ERR);

    
    bool_jso = json_object_object_get_ex(i_paras->val_jso, prop_name, prop_jso);
    return_val_do_info_if_expr(!del_flag && !bool_jso, VOS_ERR,
        (void)create_message_info(MSGID_PROP_MISSING, prop_name, o_message_jso, prop_name));

    return VOS_OK;
}


LOCAL gint32 act_set_kvm_key(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    GSList *input_list = NULL;
    OBJ_HANDLE kvm_handle = 0;

    gint32 id;
    gint32 mode = RFPROP_MANAGER_KVMKEY_MODE_SHARED;
    const gchar *str = NULL;
    const gchar *secret_key = NULL;
    const gchar *id_ext = NULL;

    json_object *obj_jso_id = NULL;
    json_object *obj_jso_idext = NULL;
    json_object *obj_jso_secretkey = NULL;
    json_object *obj_jso_mode = NULL;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_KVMMGNT, HTTP_FORBIDDEN,
        json_object_clear_string(i_paras->val_jso, RFPROP_MANAGER_KVMKEY_SECRETKEY);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = get_key_prop_val(i_paras, o_message_jso, RFPROP_COMMON_ID, &obj_jso_id);
    if (ret != VOS_OK) {
        json_object_clear_string(i_paras->val_jso, RFPROP_MANAGER_KVMKEY_SECRETKEY);
        return HTTP_BAD_REQUEST;
    }

    ret = get_key_prop_val(i_paras, o_message_jso, RFPROP_MANAGER_KVMKEY_IDEXT, &obj_jso_idext);
    if (ret != VOS_OK) {
        json_object_clear_string(i_paras->val_jso, RFPROP_MANAGER_KVMKEY_SECRETKEY);
        return HTTP_BAD_REQUEST;
    }

    ret = get_key_prop_val(i_paras, o_message_jso, RFPROP_MANAGER_KVMKEY_SECRETKEY, &obj_jso_secretkey);
    if (ret != VOS_OK) {
        json_object_clear_string(i_paras->val_jso, RFPROP_MANAGER_KVMKEY_SECRETKEY);
        return HTTP_BAD_REQUEST;
    }

    ret = get_key_prop_val(i_paras, o_message_jso, RFPROP_MANAGER_KVMKEY_MODE, &obj_jso_mode);
    if (ret != VOS_OK) {
        json_object_clear_string(i_paras->val_jso, RFPROP_MANAGER_KVMKEY_SECRETKEY);
        return HTTP_BAD_REQUEST;
    }

    
    
    return_val_do_info_if_fail(TRUE == json_object_is_type(obj_jso_id, json_type_int), HTTP_BAD_REQUEST,
        json_object_clear_string(i_paras->val_jso, RFPROP_MANAGER_KVMKEY_SECRETKEY);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_COMMON_ID, o_message_jso,
        dal_json_object_get_str(obj_jso_id), RFPROP_COMMON_ID));
    

    id = json_object_get_int(obj_jso_id);
    id_ext = dal_json_object_get_str(obj_jso_idext);
    secret_key = dal_json_object_get_str(obj_jso_secretkey);
    
    if (secret_key != NULL) {
        if (g_regex_match_simple("^[0-9a-fA-F]{64}$", secret_key, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0) == FALSE) {
            (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, RF_SENSITIVE_INFO,
                RFPROP_MANAGER_KVMKEY_SECRETKEY, SET_KVM_KEY_ACTION_NAME);
            json_object_clear_string(i_paras->val_jso, RFPROP_MANAGER_KVMKEY_SECRETKEY);
            return HTTP_BAD_REQUEST;
        }
    }

    str = dal_json_object_get_str(obj_jso_mode);
    do_val_if_expr(!g_strcmp0(str, PROPVAL_MODE_EXCLUSIVE), (mode = RFPROP_MANAGER_KVMKEY_MODE_PRIVATE));

    
    ret = dal_get_object_handle_nth(KVM_CLASS_NAME, 0, &kvm_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        json_object_clear_string(i_paras->val_jso, RFPROP_MANAGER_KVMKEY_SECRETKEY);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_int32(id));
    input_list = g_slist_append(input_list, g_variant_new_int32(mode));
    input_list = g_slist_append(input_list, g_variant_new_string(i_paras->user_name));
    input_list = g_slist_append(input_list, g_variant_new_string(secret_key));
    input_list = g_slist_append(input_list, g_variant_new_int32((gint32)i_paras->user_role_privilege));
    input_list = g_slist_append(input_list, g_variant_new_string(id_ext));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, kvm_handle,
        KVM_CLASS_NAME, KVM_METHOD_SET_KEY, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);

    json_object_clear_string(i_paras->val_jso, RFPROP_MANAGER_KVMKEY_SECRETKEY);
    uip_free_gvariant_list(input_list);

    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: KVM_METHOD_SET_KEY fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 kvm_jnlp_get_element(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, const gchar **hostname,
    const gchar **servername, const gchar **language, const gchar **mode)
{
    json_bool is_ok;

    return_val_do_info_if_fail((NULL != i_paras && NULL != o_message_jso && NULL != hostname && NULL != servername &&
        NULL != language && NULL != mode && VOS_OK == provider_paras_check(i_paras)),
        VOS_ERR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    // 获取hostname, servername, path,mode
    is_ok = get_element_str(i_paras->val_jso, PROPERTY_HOSTNAME, hostname);
    return_val_do_info_if_expr(!is_ok, VOS_ERR,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, PROPERTY_HOSTNAME, o_message_jso, EXPORT_KVM_JNLP_ACTION_NAME,
        PROPERTY_HOSTNAME);
        debug_log(DLOG_ERROR, "%s, %d: get host name error.", __FUNCTION__, __LINE__));

    is_ok = get_element_str(i_paras->val_jso, PROPERTY_SERVERNAME, servername);
    return_val_do_info_if_expr(!is_ok, VOS_ERR,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, PROPERTY_SERVERNAME, o_message_jso,
        EXPORT_KVM_JNLP_ACTION_NAME, PROPERTY_SERVERNAME);
        debug_log(DLOG_ERROR, "%s, %d: get server name error.", __FUNCTION__, __LINE__));

    is_ok = get_element_str(i_paras->val_jso, PROPERTY_LANGUAGE, language);
    return_val_do_info_if_expr(!is_ok, VOS_ERR,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, PROPERTY_LANGUAGE, o_message_jso, EXPORT_KVM_JNLP_ACTION_NAME,
        PROPERTY_LANGUAGE);
        debug_log(DLOG_ERROR, "%s, %d: get language error.", __FUNCTION__, __LINE__));

    is_ok = get_element_str(i_paras->val_jso, RFPROP_MANAGER_KVMKEY_MODE, mode);
    if (!is_ok) {
        (void)create_message_info(MSGID_ACT_PARA_MISSING, RFPROP_MANAGER_KVMKEY_MODE, o_message_jso,
            EXPORT_KVM_JNLP_ACTION_NAME, RFPROP_MANAGER_KVMKEY_MODE);
        debug_log(DLOG_ERROR, "%s, %d: get mode error.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 kvm_jnlp_get_bmc_element(PROVIDER_PARAS_S *i_paras, gint32 *kvm_compress, gint32 *kvmPort,
    guint32 *verifyId, gchar *secret_key, gchar *verifyIdExt, const gchar **mode)
{
    errno_t safe_fun_ret;
    gint32 ret;
    GSList *output_list = NULL;
    OBJ_HANDLE handle = 0;
    GSList *input_list = NULL;
    const gchar *tmp_str = NULL;

    return_val_do_info_if_fail((NULL != i_paras && NULL != kvm_compress && NULL != kvmPort && NULL != verifyId &&
        NULL != secret_key && NULL != verifyIdExt && NULL != mode && VOS_OK == provider_paras_check(i_paras)),
        VOS_ERR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    ret = dal_get_object_handle_nth(KVM_CLASS_NAME, 0, &handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_int32(handle, KVM_PROPERTY_COMPRESS_STATE, kvm_compress);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:get kvm_compress error,Failed to get the Kvm.jnlp", __FUNCTION__));

    
    ret = dal_get_property_value_int32(handle, KVM_PROPERTY_PORT, kvmPort);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:get kvmPort error,Failed to get the Kvm.jnlp", __FUNCTION__));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, handle,
        KVM_CLASS_NAME, KVM_METHOD_GET_LINK_KEY, AUTH_DISABLE, i_paras->user_role_privilege, NULL, &output_list);
    return_val_do_info_if_fail((VOS_OK == ret || NULL == output_list), VOS_ERR,
        debug_log(DLOG_ERROR, "%s:call kvm method getlinkey error,Failed to get the Kvm.jnlp", __FUNCTION__));

    *verifyId = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));

    tmp_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), NULL);
    safe_fun_ret = strncpy_s(secret_key, TMP_STR_LEN, tmp_str, TMP_STR_LEN - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    tmp_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 2), NULL);
    safe_fun_ret = strncpy_s(verifyIdExt, TMP_STR_LEN, tmp_str, TMP_STR_LEN - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    uip_free_gvariant_list(output_list);
    output_list = NULL;

    // call setcodekey methode
    input_list = g_slist_append(input_list, g_variant_new_int32(*verifyId));
    input_list = g_slist_append(input_list, g_variant_new_int32(strcmp(*mode, "Shared") ? 1 : 0));
    input_list = g_slist_append(input_list, g_variant_new_string(i_paras->user_name));
    input_list = g_slist_append(input_list, g_variant_new_string(secret_key));
    input_list = g_slist_append(input_list, g_variant_new_int32((gint32)i_paras->user_role_privilege));
    input_list = g_slist_append(input_list, g_variant_new_string(verifyIdExt));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, handle,
        KVM_CLASS_NAME, KVM_METHOD_SET_KEY, AUTH_DISABLE, i_paras->user_role_privilege, input_list, &output_list);

    uip_free_gvariant_list(input_list);
    return_val_do_info_if_fail((VOS_OK == ret), VOS_ERR,
        debug_log(DLOG_ERROR, "%s:call kvm method set_code_key error,Failed to get the Kvm.jnlp", __FUNCTION__));
    uip_free_gvariant_list(output_list);

    return VOS_OK;
}


LOCAL gint32 kvm_jnlp_get_vmm_element(gint32 *vmm_compress, gint32 *vmmPort)
{
    gint32 ret;
    OBJ_HANDLE handle = 0;

    return_val_do_info_if_fail((NULL != vmm_compress && NULL != vmmPort), VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    ret = dal_get_object_handle_nth(VMM_CLASS_NAME, 0, &handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:get vmm handle error,Failed to get the Kvm.jnlp", __FUNCTION__));

    
    ret = dal_get_property_value_int32(handle, VMM_PROPERTY_COMPRESS_STATE, vmm_compress);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:get vmm_compress error,Failed to get the Kvm.jnlp", __FUNCTION__));

    
    ret = dal_get_property_value_int32(handle, VMM_PROPERTY_PORT, vmmPort);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:get vmmPort error,Failed to get the Kvm.jnlp", __FUNCTION__));
    return VOS_OK;
}


LOCAL gint32 get_kvm_info(json_object **o_message_jso, guint8 *caps_enable, gchar *device_sn, guint32 device_len)
{
    OBJ_HANDLE handle = 0;
    gint32 ret;
    // 获取设备序列号
    ret = dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &handle);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s:get bmc handle error,Failed to get the Kvm.jnlp", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_string(handle, PROPERTY_BMC_DEV_SERIAL, device_sn, device_len);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s:get DeviceSerialNumber error,Failed to get the Kvm.jnlp", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 是否支持大小写同步
    if (dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &handle) != RET_OK ||
        dal_get_property_value_byte(handle, PROPERTY_PRODUCT_KVM_CAPS_SYNC_ENABLE, caps_enable) != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s:get %s error,Failed to get the Kvm.jnlp", __FUNCTION__,
            PROPERTY_PRODUCT_KVM_CAPS_SYNC_ENABLE);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return RET_OK;
}


LOCAL gint32 gen_kvm_jnlp_content_str(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, gchar *buffer,
    guint32 buf_len)
{
    const gchar *hostname = NULL;
    const gchar *servername = NULL;
    const gchar *language = NULL;
    guint8 user_privilege = 0;
    const gchar *mode = NULL;
    gint32 ret;
    guint32 verify_id = 0;
    guint32 mm_verify_id;
    gchar secret_key[TMP_STR_LEN] = {0};
    gchar verify_id_ext[TMP_STR_LEN] = {0};
    gchar device_sn[TMP_STR_LEN] = {0};
    gchar format_sn[MAX_LINE_SIZE] = {0};
    gint32 kvm_compress = 0;
    gint32 vmm_compress = 0;
    gint32 kvm_port = 0;
    gint32 vmm_port = 0;
    gchar jar_name[MAX_FILE_NAME_LEN] = {0};
    guint8 caps_enable = 1;
    // 获取privilege
    ret = dal_get_user_role_from_provider(i_paras->user_roleid, &user_privilege);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s:get user privilege fail, ret:%d", __FUNCTION__, ret));

    // 获取请求体中的参数hostname,language,path,mode
    ret = kvm_jnlp_get_element(i_paras, o_message_jso, &hostname, &servername, &language, &mode);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s:get element error,Failed to get the Kvm.jnlp", __FUNCTION__));

    // 获取jarname
    ret = find_fullfilename_from_directory(KVM_FIND_JARNAME_PATH, "vconsole", jar_name, sizeof(jar_name));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: get jar_name fail", __FUNCTION__));

    // 获取kvm 侧的参数
    ret = kvm_jnlp_get_bmc_element(i_paras, &kvm_compress, &kvm_port, &verify_id, secret_key, verify_id_ext, &mode);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s:get bmc element error,Failed to get the Kvm.jnlp", __FUNCTION__));
    
    ret = get_kvm_info(o_message_jso, &caps_enable, device_sn, sizeof(device_sn));
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    mm_verify_id = verify_id;

    // 获取vmm侧的参数
    ret = kvm_jnlp_get_vmm_element(&vmm_compress, &vmm_port);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s:get DeviceSerialNumber error,Failed to get the Kvm.jnlp", __FUNCTION__));

    
    dal_format_device_sn(device_sn, format_sn, sizeof(format_sn));

    ret = snprintf_truncated_s(buffer, buf_len, KVM_JNLP_FILE_FORMAT, hostname, servername, format_sn, jar_name,
        verify_id, servername, format_sn, verify_id, mm_verify_id, secret_key, language, kvm_compress, vmm_compress,
        kvm_port, vmm_port, user_privilege, servername, servername, verify_id_ext, servername, format_sn, caps_enable);
    return_val_do_info_if_expr(ret <= 0, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));

    return VOS_OK;
}


LOCAL gint32 act_export_kvm_jnlp(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar buffer[MAX_BUFF_SIZE] = {0};
    json_object *file_jso = NULL;
    json_object *buffer_jso = NULL;

    
    goto_label_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK), err_exit,
        ret = HTTP_INTERNAL_SERVER_ERROR;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    goto_label_do_info_if_fail(i_paras->user_role_privilege & USERROLE_KVMMGNT, err_exit, ret = HTTP_FORBIDDEN;
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = gen_kvm_jnlp_content_str(i_paras, o_message_jso, buffer, sizeof(buffer));
    goto_label_do_info_if_expr(ret != VOS_OK, err_exit,
        debug_log(DLOG_ERROR, "%s: generate kvm jnlp file fail", __FUNCTION__));

    
    buffer_jso = json_object_new_string((const gchar *)buffer);
    if (buffer_jso == NULL) {
        debug_log(DLOG_ERROR, "new json object string buffer failed");
        ret = HTTP_INTERNAL_SERVER_ERROR;
        goto err_exit;
    }

    file_jso = json_object_new_string("kvm.jnlp");
    if (file_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: new json object string failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        ret = HTTP_INTERNAL_SERVER_ERROR;
        goto err_exit;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: new json object string o_result_json failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        ret = HTTP_INTERNAL_SERVER_ERROR;
        goto err_exit;
    }
    json_object_object_add(*o_result_jso, RFPROP_JNLP_FILE_NAME, file_jso);
    json_object_object_add(*o_result_jso, RFPROP_JNLP_FILE_CONTENT, buffer_jso);

    (void)record_method_operation_log(i_paras->is_from_webui, TRUE, i_paras->user_name, i_paras->client,
        KVM_STARTUP_FILE);
    
    i_paras->custom_header = json_object_new_object();
    json_object_object_add(i_paras->custom_header, REQ_HEADER_CONTENT_TYPE,
        json_object_new_string(RF_CONTENT_TYPE_OCTET_STREAM));

    return HTTP_OK;

err_exit:
    if (buffer_jso != NULL) {
        json_object_put(buffer_jso);
    }
    if (file_jso != NULL) {
        json_object_put(file_jso);
    }
    (void)record_method_operation_log(i_paras->is_from_webui, FALSE, i_paras->user_name, i_paras->client,
        KVM_STARTUP_FILE);
    return ret;
}

LOCAL PROPERTY_PROVIDER_S g_kvm_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_kvm_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_vmm_kvm_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_MANAGER_KVMTIMOUT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_timeoutperiod, set_timeoutperiod, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_MANAGER_KVMMOUSE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_keyboardandmouse, set_keyboardandmouse, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_MANAGER_KVMENCRYPTIONCONFIGURABLE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_kvmencryption_configurable, NULL, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_MANAGER_KVMENCRYPTION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_kvmencryption, set_kvmencryption, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_MANAGER_KVMNUM_SESSION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_numberofactivatedsessions, NULL, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_MANAGER_ACTIVED_SESSION_TYPE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_activated_session_type, NULL, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_MANAGER_VISIBLE_JAVAKVM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, 
     get_visible_java_kvm, NULL, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_MANAGER_AUTO_OS_LOCK_STATE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_auto_os_lock_state, set_auto_os_lock_state, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_MANAGER_AUTO_OS_LOCK_TYPE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_auto_os_lock_type, set_auto_os_lock_type, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_MANAGER_AUTO_OS_LOCK_KEY, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_auto_os_lock_key, set_auto_os_lock_key, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ACTION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_kvmservice_actions, NULL, NULL, ETAG_FLAG_ENABLE},
    {SET_KVM_KEY_ACTION_NAME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_KVMMGNT, SYS_LOCKDOWN_FORBID, NULL, NULL, act_set_kvm_key, ETAG_FLAG_ENABLE},
    {EXPORT_KVM_JNLP_ACTION_NAME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_KVMMGNT, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_export_kvm_jnlp, ETAG_FLAG_ENABLE}
};

gint32 managers_provider_kvmservice_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
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

    *prop_provider = g_kvm_provider;
    *count = sizeof(g_kvm_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}


LOCAL gint32 get_kvm_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar slot_uri[PSLOT_URI_LEN] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar *lower_url = NULL;
    gint32 ret;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: redfish_get_board_slot fail.", __FUNCTION__, __LINE__));

    lower_url = g_ascii_strdown(i_paras->uri, strlen(i_paras->uri));
    return_val_do_info_if_fail(NULL != lower_url, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "[%s]: g_ascii_strdown fail ", __FUNCTION__));

    // 增加对URI的判断
    if (NULL != strstr(lower_url, KVM_ACTION_SETKVMKEY_INFO_URI_SUFFIX)) {
        ret = snprintf_s(slot_uri, sizeof(slot_uri), sizeof(slot_uri) - 1, KVM_ACTION_INFO_URI, slot);
    } else if (NULL != strstr(lower_url, KVM_ACTION_KVMJNLP_INFO_URI_SUFFIX)) {
        ret = snprintf_s(slot_uri, sizeof(slot_uri), sizeof(slot_uri) - 1, KVM_ACTION_KVMJNLP_INFO_URI, slot);
    }
    g_free(lower_url);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string((const gchar *)slot_uri);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_manager_kvm_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_kvm_actioninfo_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};


gint32 manager_provider_kvm_actioninfo_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    guchar board_type = 0;
    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);

    ret = redfish_get_board_type(&board_type);
    return_val_if_expr(VOS_OK != ret, HTTP_NOT_FOUND);
    return_val_if_expr(BOARD_SWITCH == board_type, HTTP_NOT_FOUND);

    guint8 kvm_enabled = DISABLE;
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_KVM, &kvm_enabled);
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

    *prop_provider = g_manager_kvm_actioninfo_provider;
    *count = sizeof(g_manager_kvm_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
