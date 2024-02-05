
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "redfish_provider_chassisoverview.h"

#define RFPROP_OVERVIEW_SLOT "Slot"
#define RFPROP_OVERVIEW_PRODUCT_SN "ProductSerialNumber"


LOCAL void padding_product_sn_ipmi_request(IPMIMSG_BLADE_T *request_head,
    DFT_READ_ELABEL_REQ_S *data, guint8 slave_addr)
{
    request_head->lun = 0;
    request_head->netfun = NETFN_OEM_REQ;
    request_head->slave_addr = slave_addr;
    request_head->cmd = IPMI_OEM_DFT_COMMAND;

    request_head->src_len = sizeof(DFT_READ_ELABEL_REQ_S);
    data->subcmd = 0x05;
    data->fru_id = 0x00;
    data->area = 0x03; 
    data->field = 0x04; 
    data->flag_offset = 0x00;
    data->len = 0xff;
    request_head->data = (guint8 *)data;
}


LOCAL gint32 get_remote_node_product_sn(guint8 slave_addr, gchar *product_sn, gsize len)
{
    gint32 ret;
    IPMIMSG_BLADE_T request_head = {0};
    DFT_READ_ELABEL_REQ_S data = {0};
    gpointer response = NULL;
    const guint8 *resp_data = NULL;
    gsize resp_data_len = 0;

    padding_product_sn_ipmi_request(&request_head, &data, slave_addr);
    ret = redfish_ipmi_send_msg_to_blade(&request_head, MAX_POSSIBLE_IPMI_FRAME_LEN, &response, TRUE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s redfish_ipmi_send_msg_to_blade(%d) failed, ret = %d", __FUNCTION__, slave_addr, ret);
        return RET_ERR;
    }

    resp_data = get_ipmi_src_data(response);
    resp_data_len = get_ipmi_src_data_len(response);
    if (resp_data_len >= 1 && resp_data[0] != COMP_CODE_SUCCESS) {
        debug_log(DLOG_ERROR, "%s error code(%d)", __FUNCTION__, resp_data[0]);
        g_free(response);
        return RET_ERR;
    }

    if (resp_data_len <= 2) { 
        debug_log(DLOG_ERROR, "%s len(%"G_GSIZE_FORMAT") error", __FUNCTION__, resp_data_len);
        g_free(response);
        return RET_ERR;
    }

    ret = memcpy_s(product_sn, len, resp_data + 2, resp_data_len - 2); 
    g_free(response);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s memcpy_s failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 get_one_component_info(OBJ_HANDLE handle, gpointer user_data)
{
    json_object *components_obj = (json_object *)user_data;
    json_object *component_obj = NULL;
    guint8 presence = 0;
    guint8 blade_type = 0;
    guint8 slave_addr = 0;
    gint32 ret;
    const gchar *blade_name = NULL;
    gchar blade_name_f[MAX_NAME_SIZE] = { 0 };
    gchar product_sn[128] = {0};

    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_PRESENCE, &presence);
    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_BLADETYPE, &blade_type);
    
    if (presence != BLADE_PRESENCE || blade_type == 0x03) {
        return RET_OK;
    }

    blade_name = dfl_get_object_name(handle);
    if (blade_name == NULL) {
        debug_log(DLOG_ERROR, "%s: get object name failed.", __FUNCTION__);
        return RET_OK;
    }
    ret = strcpy_s(blade_name_f, sizeof(blade_name_f), blade_name);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s failed, ret = %d", __FUNCTION__, ret);
        return RET_OK;
    }
    format_string_as_upper_head(blade_name_f, strlen(blade_name_f));

    component_obj = json_object_new_object();
    if (component_obj == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed.", __FUNCTION__);
        return RET_OK;
    }

    json_object_object_add(component_obj, RFPROP_OVERVIEW_ID, json_object_new_string(blade_name_f));
    rf_add_property_jso_byte(handle, PROPERTY_IPMBETH_BLADE_ID, RFPROP_OVERVIEW_SLOT, component_obj);

    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_SLAVEADDR, &slave_addr);
    ret = get_remote_node_product_sn(slave_addr, product_sn, sizeof(product_sn));

    json_object_object_add(component_obj, RFPROP_OVERVIEW_PRODUCT_SN,
        (ret == RET_OK) ? json_object_new_string(product_sn) : NULL);

    json_object_array_add(components_obj, component_obj);
    return RET_OK;
}


gint32 get_chassis_overview(PROVIDER_PARAS_S *i_paras,
    json_object *o_rsc_jso, json_object *o_err_array_jso, gchar **o_rsp_body_str)
{
    json_object *msg_obj = NULL;
    json_object *components_obj = NULL;

    
    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_obj);
        json_object_array_add(o_err_array_jso, msg_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if ((i_paras->user_role_privilege & USERROLE_READONLY) == 0) {
        debug_log(DLOG_ERROR, "%s: privilege not usermgmt", __FUNCTION__);
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &msg_obj);
        json_object_array_add(o_err_array_jso, msg_obj);
        return HTTP_FORBIDDEN;
    }

    components_obj = json_object_new_array();
    (void)dfl_foreach_object(CLASS_NAME_IPMBETH_BLADE, get_one_component_info, components_obj, NULL);
    json_object_object_add(o_rsc_jso, RFPROP_OVERVIEW_COMPONENTS, components_obj);

    return RET_OK;
}


gboolean rm_is_support_chassis_overview(void)
{
    guint8 software_type = 0;
    gboolean is_enc = FALSE;
    OBJ_HANDLE obj_rack_asset = 0;
    
    gint32 ret = dal_get_object_handle_nth(CLASS_RACK_ASSET_MGMT, 0, &obj_rack_asset);
    if (ret != RET_OK) {
        is_enc = TRUE;
    }
    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_RM && is_enc) {
        return TRUE;
    }
    return FALSE;
}