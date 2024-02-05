
#include "redfish_provider.h"
#include "redfish_provider_chassis_pciedevices.h"

#define MSGID_PCIEDEVICE_SET_NMI_FAILED "SetNMIFailed"

LOCAL gint32 get_chassis_pciedevices_nmi_actioninfo(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 is_valid_sdi_or_dpu_boardid(OBJ_HANDLE object_handle);

LOCAL PROPERTY_PROVIDER_S g_chassis_pciedevices_nmi_provider[] = {
    {
        RFPROP_ODATA_ID,
        MAP_PROPERTY_NULL,
        MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_chassis_pciedevices_nmi_actioninfo,
        NULL, NULL,
        ETAG_FLAG_ENABLE
    }
};


gint32 set_pciedevices_nmi_entry(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE sdi_handle = 0;
    guchar pcie_card_slot = INVALID_VAL;
    GSList *input_list = NULL;

    if ((i_paras == NULL) || (o_message_jso == NULL) || (o_result_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_BASICSETTING) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    (void)dal_get_specific_object_position(i_paras->obj_handle, CLASS_PCIE_SDI_CARD, &sdi_handle);
    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_PCIE_CARD_SLOT_ID, &pcie_card_slot);

    input_list = g_slist_append(input_list, g_variant_new_byte(pcie_card_slot));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, sdi_handle,
        CLASS_PCIE_SDI_CARD, METHOD_SET_SDI_CARD_NMI, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set sdi card NMI failed. ret:%d", __FUNCTION__, ret);
        (void)create_message_info(MSGID_PCIEDEVICE_SET_NMI_FAILED, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL void get_chassis_pciedevices_nmi_uri(OBJ_HANDLE chassis_handle, OBJ_HANDLE i_pcie_handle,
    gchar *pciedevice_uri, gint32 uri_len)
{
    const gchar *card_prefix_ptr = NULL;
    guchar pcie_card_slot = INVALID_VAL;
    gchar pciecardx[MAX_STRBUF_LEN + 1] = {0};
    gchar slot[MAX_RSC_ID_LEN + 1] = {0};

    gint32 ret = rf_gen_chassis_component_id(NULL, chassis_handle, slot, MAX_RSC_ID_LEN + 1);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s: get slot id error.", __FUNCTION__);
    }

    card_prefix_ptr = get_chassis_pciedevices_cardtype(i_pcie_handle);
    (void)dal_get_property_value_byte(i_pcie_handle, PROPERTY_PCIE_CARD_LOGIC_SLOT, &pcie_card_slot);
    if (pcie_card_slot == INVALID_VAL) {
        (void)dal_get_property_value_byte(i_pcie_handle, PROPERTY_PCIE_CARD_SLOT_ID, &pcie_card_slot);
    }

    ret = snprintf_s(pciecardx, MAX_STRBUF_LEN + 1, MAX_STRBUF_LEN, "%s%u", card_prefix_ptr, pcie_card_slot);
    if (ret <= 0) {
        debug_log(DLOG_MASS, "%s: snprintf_s fail .", __FUNCTION__);
    }

    ret = snprintf_s(pciedevice_uri, uri_len, uri_len - 1, URI_FORMAT_PCIEDEVICES_NMI_ACTION, slot, pciecardx);
    if (ret <= 0) {
        debug_log(DLOG_MASS, "%s: snprintf_s fail .", __FUNCTION__);
    }
}


LOCAL gint32 get_chassis_pciedevices_nmi_actioninfo(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE chassis_handle = 0;
    gchar pciedevice_uri[MAX_PCIEPATH_LEN] = {0};
    gchar nmi_action[MAX_URI_LENGTH] = {0};

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_MASS, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    if (is_valid_sdi_or_dpu_boardid(i_paras->obj_handle) == RET_OK) {
        chassis_handle = (OBJ_HANDLE)g_slist_nth_data(i_paras->parent_obj_handle_list, 0);
        
        get_chassis_pciedevices_nmi_uri(chassis_handle, i_paras->obj_handle, pciedevice_uri, sizeof(pciedevice_uri));
        ret = snprintf_s(nmi_action, sizeof(nmi_action), sizeof(nmi_action) - 1, "%s%s", pciedevice_uri,
            PCIEDEVICES_NMI_ACTION);
        if (ret <= 0) {
            debug_log(DLOG_MASS, "%s: location devicename fail .", __FUNCTION__);
        }

        *o_result_jso = json_object_new_string((const gchar*)nmi_action);
        if (*o_result_jso == NULL) {
            debug_log(DLOG_ERROR, "%s: json new object failed.", __FUNCTION__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    return HTTP_OK;
}


gint32 get_pciedevices_srv_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *actions_json = NULL;
    OBJ_HANDLE chassis_handle = 0;
    gchar pciedevice_uri[MAX_URI_LENGTH] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_MASS, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    if (is_valid_sdi_or_dpu_boardid(i_paras->obj_handle) == RET_OK) {
        chassis_handle = (OBJ_HANDLE)g_slist_nth_data(i_paras->parent_obj_handle_list, 0);
        
        actions_json = json_object_new_object();
        if (actions_json == NULL) {
            debug_log(DLOG_ERROR, "%s: alloc new object failed", __FUNCTION__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        get_chassis_pciedevices_nmi_uri(chassis_handle, i_paras->obj_handle, pciedevice_uri, sizeof(pciedevice_uri));
        rf_add_action_prop(actions_json, (const gchar*)pciedevice_uri, RFACTION_SET_PCIEDEVICES_NMI_ACTION);
        *o_result_jso = actions_json;
    } else {
        *o_result_jso = NULL;
    }

    return HTTP_OK;
}


gint32 chassis_pciedevices_nmi_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gboolean b_ret;

    if (i_paras == NULL || prop_provider == NULL || count == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return RET_ERR;
    }

    if (is_valid_sdi_or_dpu_boardid(i_paras->obj_handle) == RET_OK) {
        b_ret = redfish_check_manager_uri_valid(i_paras->uri);
        if (b_ret != TRUE) {
            return HTTP_NOT_FOUND;
        }

        *prop_provider = g_chassis_pciedevices_nmi_provider;
        *count = G_N_ELEMENTS(g_chassis_pciedevices_nmi_provider);
    } else {
        return HTTP_NOT_FOUND;
    }

    return RET_OK;
}


LOCAL gint32 is_valid_sdi_or_dpu_boardid(OBJ_HANDLE object_handle)
{
    guint32 boardid = 0;
    guint32 i;
    gint32 ret;
    guint32 valid_boardid[] = {
        SDI_CARD_50_BOARDID_A5,
        SDI_CARD_51_BOARDID_A9,
        DPU_CARD_12_BOARDID_ED,
    };
    ret = dal_get_property_value_uint32(object_handle, PROPERTY_SDI_CARD_BOARDID, &boardid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get %s.%s failed, ret = %d", __FUNCTION__, dfl_get_object_name(object_handle),
            PROPERTY_SDI_CARD_BOARDID, ret);
        return RET_ERR;
    }

    for (i = 0; i < sizeof(valid_boardid) / sizeof(valid_boardid[0]); i++) {
        if (boardid == valid_boardid[i]) {
            return RET_OK;
        }
    }
    
    return RET_ERR;
}