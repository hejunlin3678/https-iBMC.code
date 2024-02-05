

#include "redfish_provider.h"


LOCAL void __get_endpoint_related_rsc_info(OBJ_HANDLE obj_handle, json_object *o_rsc_jso)
{
    gint32 ret;
    json_object *connected_entities_jso = NULL;
    json_object *connected_entity_item_jso = NULL;
    json_object *pci_id_jso = NULL;
    json_object *pci_id_val_jso = NULL;
    json_object *entity_link_jso = NULL;
    gchar               ref_entity_link[MAX_URI_LENGTH] = {0};
    OBJ_HANDLE ref_comp_handle = 0;
    const gchar *ep_handle_name = NULL;

    ep_handle_name = dfl_get_object_name(obj_handle);
    return_do_info_if_fail(NULL != ep_handle_name, debug_log(DLOG_ERROR,
        "%s failed: cann't get object name for handle [%" OBJ_HANDLE_FORMAT "]", __FUNCTION__, obj_handle));

    rf_add_property_jso_string(obj_handle, PROPERTY_COMPOSITION_ENDPOINT_PROTOCOL, RFPROP_ENDPOINT_PROTOCOL, o_rsc_jso);
    rf_add_property_jso_string(obj_handle, PROPERTY_COMPOSITION_ENDPOINT_DESCRIPTION, RFOBJ_DESCRIPTION, o_rsc_jso);

    connected_entities_jso = json_object_new_array();
    return_do_info_if_fail(NULL != connected_entities_jso,
        debug_log(DLOG_ERROR, "%s failed: alloc json array object failed", __FUNCTION__));

    json_object_object_add(o_rsc_jso, RFPROP_ENDPOINT_CONNECTED_ENTITIES, connected_entities_jso);

    connected_entity_item_jso = json_object_new_object();
    return_do_info_if_fail(NULL != connected_entity_item_jso,
        debug_log(DLOG_ERROR, "%s failed: alloc json object failed", __FUNCTION__));

    rf_add_property_jso_string(obj_handle, PROPERTY_COMPOSITION_ENDPOINT_ENTITY_TYPE, RFPROP_ENDPOINT_ENTITY_TYPE,
        connected_entity_item_jso);

    pci_id_jso = json_object_new_object();
    return_do_info_if_fail(NULL != pci_id_jso,
        debug_log(DLOG_ERROR, "%s failed: alloc json object failed", __FUNCTION__);
        json_object_put(connected_entity_item_jso));

    
    get_pci_id_property_uint16(obj_handle, PROPERTY_COMPOSITION_ENDPOINT_DID, &pci_id_val_jso);
    json_object_object_add(pci_id_jso, RFPROP_PCIEFUNCTION_DEVICEID, pci_id_val_jso);

    
    pci_id_val_jso = NULL;
    get_pci_id_property_uint16(obj_handle, PROPERTY_COMPOSITION_ENDPOINT_VID, &pci_id_val_jso);
    json_object_object_add(pci_id_jso, RFPROP_PCIEFUNCTION_VENDORID, pci_id_val_jso);

    
    pci_id_val_jso = NULL;
    get_pci_id_property_uint16(obj_handle, PROPERTY_COMPOSITION_ENDPOINT_SUB_DID, &pci_id_val_jso);
    json_object_object_add(pci_id_jso, RFPROP_PCIEFUNCTION_SUBSYSTEMID, pci_id_val_jso);

    
    pci_id_val_jso = NULL;
    get_pci_id_property_uint16(obj_handle, PROPERTY_COMPOSITION_ENDPOINT_SUB_VID, &pci_id_val_jso);
    json_object_object_add(pci_id_jso, RFPROP_PCIEFUNCTION_SUBSYSTEMVENDORID, pci_id_val_jso);

    json_object_object_add(connected_entity_item_jso, RFPROP_ENDPOINT_ENTITY_PCI_ID, pci_id_jso);

    
    ret = dfl_get_referenced_object(obj_handle, PROPERTY_COMPOSITION_ENDPOINT_REF_COMP, &ref_comp_handle);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s failed: cann't get reference componet handle for %s", __FUNCTION__, ep_handle_name);
        json_object_put(connected_entity_item_jso));

    ret = rf_gen_component_uri(ref_comp_handle, ref_entity_link, sizeof(ref_entity_link));
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR,
        "%s failed: gen component uri with object handle [%s] failed", __FUNCTION__, ep_handle_name);
        json_object_put(connected_entity_item_jso));

    entity_link_jso = json_object_new_object();
    return_do_info_if_fail(NULL != entity_link_jso,
        debug_log(DLOG_ERROR, "%s failed: alloc new json object failed", __FUNCTION__);
        json_object_put(connected_entity_item_jso));

    json_object_object_add(entity_link_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)ref_entity_link));
    json_object_object_add(connected_entity_item_jso, RFPROP_ENDPOINT_ENTITY_LINK, entity_link_jso);

    json_object_array_add(connected_entities_jso, connected_entity_item_jso);

    return;
}


gint32 get_endpoint_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gchar               endpoint_rsc_id[MAX_RSC_ID_LEN] = {0};
    gint32 ret;
    OBJ_HANDLE composition_endpoint_handle = 0;
    const gchar *ep_identifier_name = NULL;
    gchar               endpoint_rsc_uri[MAX_URI_LENGTH] = {0};
    guchar container_presence = 0;
    guchar ep_identifier_id = 0;
    guchar ep_presence = 0;
    guchar ep_health;
    json_object *status_jso = NULL;

    if (i_paras->obj_handle == 0) {
        debug_log(DLOG_ERROR, "%s obj handle is NULL", __FUNCTION__);
        return RF_RSC_NOT_FOUND;
    }

    ep_identifier_name = dfl_get_object_name(i_paras->obj_handle);
    if (ep_identifier_name == NULL) {
        debug_log(DLOG_ERROR, "%s dfl_get_object_name faield", __FUNCTION__);
        return RF_RSC_NOT_FOUND;
    }
   
    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_NAME_EP_CONTAINER_PRESENCE, &container_presence);
    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_NAME_EP_IDENTIFIER_ID, &ep_identifier_id);
    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_NAME_EP_IDENTIFIER_PRESENCE, &ep_presence);

    
    if (0 == container_presence || (INVALID_EP_IDENTIFIER_ID == ep_identifier_id)) {
        return RF_RSC_NOT_FOUND;
    }

    ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_NAME_EP_CONTAINER, endpoint_rsc_id,
        sizeof(endpoint_rsc_id));
    return_val_do_info_if_fail(VOS_OK == ret, RF_RSC_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s failed: cann't get property value for [%s:%s]", __FUNCTION__, ep_identifier_name,
        PROPERTY_NAME_EP_CONTAINER));

    
    
    return_val_do_info_if_fail((guint32)strlen(endpoint_rsc_id) < sizeof(endpoint_rsc_id), RF_RSC_NOT_FOUND,
        debug_log(DLOG_DEBUG, "%s failed: no enough space for rsc id for object [%s]", __FUNCTION__,
        ep_identifier_name));
    
    ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_NAME_EP_IDENTIFIER_SHORT_NAME,
        endpoint_rsc_id + strlen(endpoint_rsc_id), sizeof(endpoint_rsc_id) - strlen(endpoint_rsc_id));
    return_val_do_info_if_fail(VOS_OK == ret, RF_RSC_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s failed: cann't get property value for [%s:%s]", __FUNCTION__, ep_identifier_name,
        PROPERTY_NAME_EP_IDENTIFIER_SHORT_NAME));

    do_val_if_expr(strlen(endpoint_rsc_id) == (sizeof(endpoint_rsc_id) - 1),
        debug_log(DLOG_INFO, "%s: endpoint rsc name [%s] for object handle [%s] reach or exceed limit", __FUNCTION__,
        endpoint_rsc_id, ep_identifier_name));

    ret = snprintf_s(endpoint_rsc_uri, sizeof(endpoint_rsc_uri), sizeof(endpoint_rsc_uri) - 1, URI_FORMAT_ENDPOINT,
        RF_PCIE_FABRIC, endpoint_rsc_id);
    return_val_do_info_if_fail(ret > 0, RF_RSC_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s failed: cann't format rsc uri for [%s]", __FUNCTION__, ep_identifier_name));

    json_object_object_add(o_rsc_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)endpoint_rsc_uri));
    json_object_object_add(o_rsc_jso, RFPROP_COMMON_ID, json_object_new_string((const gchar *)endpoint_rsc_id));
    json_object_object_add(o_rsc_jso, RFPROP_COMMON_NAME, json_object_new_string((const gchar *)endpoint_rsc_id));

    
    if (0 == ep_presence) {
        goto quit;
    }

    ret = dal_get_identifier_related_endpoint(i_paras->obj_handle, &composition_endpoint_handle);
    if (VOS_OK != ret) {
        debug_log(DLOG_DEBUG, "%s: cann't get related endpoint for %s", __FUNCTION__,
            dfl_get_object_name(i_paras->obj_handle));

        goto quit;
    }

    
    (void)dal_get_property_value_byte(composition_endpoint_handle, PROPERTY_COMPOSITION_ENDPOINT_HEALTH, &ep_health);
    (void)get_resource_status_property(&ep_health, NULL, RF_STATE_ENABLED, &status_jso, FALSE);
    json_object_object_add(o_rsc_jso, RFPROP_STATUS, status_jso);

    
    __get_endpoint_related_rsc_info(composition_endpoint_handle, o_rsc_jso);

    return RF_OK;

quit:
    ep_health = HEALTH_NORMAL;
    (void)get_resource_status_property(&ep_health, NULL, RF_STATE_ABSENT, &status_jso, FALSE);
    json_object_object_add(o_rsc_jso, RFPROP_STATUS, status_jso);

    return RF_OK;
}


LOCAL PROPERTY_PROVIDER_S g_endpoint_provider[] = {
};


LOCAL gint32 __check_endpoint_uri_valid(const gchar *rsc_id, OBJ_HANDLE *ep_identifier_handle)
{
    GSList *property_list = NULL;
    gint32 ret;

    return_val_do_info_if_fail((NULL != rsc_id) && (NULL != ep_identifier_handle), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    property_list = g_slist_append(property_list, g_variant_new_string(PROPERTY_NAME_EP_CONTAINER));
    property_list = g_slist_append(property_list, g_variant_new_string(PROPERTY_NAME_EP_IDENTIFIER_SHORT_NAME));

    ret = rf_check_rsc_uri_valid_with_property_list(rsc_id, CLASS_NAME_ENDPOINT_IDENTIFIER, property_list,
        ep_identifier_handle);
    g_slist_free_full(property_list, (GDestroyNotify)g_variant_unref);

    return ret;
}


gint32 endpoint_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    ret = __check_endpoint_uri_valid(i_paras->member_id, &i_paras->obj_handle);
    if (VOS_ERR == ret) {
        debug_log(DLOG_ERROR, "%s failed: check uri valid failed for [%s]", __FUNCTION__, i_paras->member_id);

        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_endpoint_provider;
    
    *count = 0;

    return VOS_OK;
}


LOCAL void __add_zone_related_endpoints(OBJ_HANDLE composition_zone_handle, const gchar *prop_name,
    json_object *result_jso)
{
    GVariant *endpoints_var = NULL;
    gint32 ret;
    const gchar *obj_name = NULL;
    const gchar **str_v;
    guint32 str_array_size;
    guint32 i;
    gchar               endpoint_id[MAX_RSC_ID_LEN] = {0};
    gchar               endpoint_uri[MAX_URI_LENGTH] = {0};
    errno_t str_ret = EOK;
    gsize parm_temp = 0;

    return_do_info_if_fail((0 != composition_zone_handle) && (NULL != prop_name) && (NULL != result_jso),
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    obj_name = dfl_get_object_name(composition_zone_handle);
    return_do_info_if_fail(NULL != obj_name, debug_log(DLOG_ERROR,
        "%s failed: cann't get object name for %" OBJ_HANDLE_FORMAT, __FUNCTION__, composition_zone_handle));

    ret = dfl_get_property_value(composition_zone_handle, prop_name, &endpoints_var);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s failed: cann't get property value for [%s:%s]", __FUNCTION__, obj_name, prop_name));

    str_v = g_variant_get_strv(endpoints_var, &parm_temp);
    str_array_size = (guint32)parm_temp;
    return_do_info_if_fail(NULL != str_v,
        debug_log(DLOG_ERROR, "%s failed: cann't get string array from [%s:%s]", __FUNCTION__, obj_name, prop_name);
        g_variant_unref(endpoints_var));

    for (i = 0; i < str_array_size; i++) {
        (void)memset_s(endpoint_id, sizeof(endpoint_id), 0, sizeof(endpoint_id));

        str_ret = strncpy_s(endpoint_id, sizeof(endpoint_id), str_v[i], strlen(str_v[i]));
        continue_do_info_if_fail(EOK == str_ret,
            debug_log(DLOG_ERROR, "%s failed: copy string %s failed", __FUNCTION__, str_v[i]));

        (void)str_delete_char(endpoint_id, '/');

        (void)memset_s(endpoint_uri, sizeof(endpoint_uri), 0, sizeof(endpoint_uri));

        ret = snprintf_s(endpoint_uri, sizeof(endpoint_uri), sizeof(endpoint_uri) - 1, URI_FORMAT_ENDPOINT,
            RF_PCIE_FABRIC, endpoint_id);
        continue_do_info_if_fail(ret > 0,
            debug_log(DLOG_ERROR, "%s failed: format endpoint uri failed, ret is %d", __FUNCTION__, ret));

        RF_ADD_ODATA_ID_MEMBER(result_jso, endpoint_uri);
    }

    g_variant_unref(endpoints_var);
    
    g_free(str_v);
    

    return;
}


gint32 get_zone_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    int iRet;
    json_object *endpoints_array_jso = NULL;
    json_object *links_jso = NULL;
    gchar               formated_zone_uri[MAX_URI_LENGTH] = {0};
    guchar zone_id = 0;
    gint32 ret;
    gchar               zone_name_buf[MAX_RSC_NAME_LEN] = {0};

    if (i_paras == NULL || o_rsc_jso == NULL || o_err_array_jso == NULL) {
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__);
        return RF_FAILED;
    }

    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_COMPOSITION_ZONE_ID, &zone_id);

    
    ret = snprintf_s(formated_zone_uri, sizeof(formated_zone_uri), sizeof(formated_zone_uri) - 1, URI_FORMAT_ZONE,
        RF_PCIE_FABRIC, zone_id);
    return_val_do_info_if_fail(ret > 0, RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed: format zone uri failed, ret is %d", __FUNCTION__, ret));

    json_object_object_add(o_rsc_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)formated_zone_uri));

    
    iRet = snprintf_s(zone_name_buf, sizeof(zone_name_buf), sizeof(zone_name_buf) - 1, RF_ZONE_NAME_FORMAT, zone_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    json_object_object_add(o_rsc_jso, RFPROP_COMMON_NAME, json_object_new_string((const gchar *)zone_name_buf));

    endpoints_array_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != endpoints_array_jso, RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed: alloc new json array failed", __FUNCTION__));

    __add_zone_related_endpoints(i_paras->obj_handle, PROPERTY_UPSIDE_ENDPOINTS, endpoints_array_jso);
    __add_zone_related_endpoints(i_paras->obj_handle, PROPERTY_DOWNSIDE_ENDPOINTS, endpoints_array_jso);

    links_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != links_jso, RF_FAILED,
        debug_log(DLOG_ERROR, "%s faiiled: alloc new json object failed", __FUNCTION__);
        json_object_put(endpoints_array_jso));

    json_object_object_add(links_jso, RFPROP_ZONE_ENDPOINTS, endpoints_array_jso);
    json_object_object_add(o_rsc_jso, RFPROP_LINKS, links_jso);

    return RF_OK;
}


LOCAL PROPERTY_PROVIDER_S g_zone_provider[] = {
};


LOCAL gint32 __check_zone_uri_valid(const gchar *rsc_id, OBJ_HANDLE *composition_zone_handle)
{
    guint32 zone_id;
    gint32 ret;

    return_val_do_info_if_fail((NULL != rsc_id) && (NULL != composition_zone_handle) &&
        (strlen(rsc_id) <= MAX_ZONE_RSC_ID_LEN),
        VOS_ERR, debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    ret = vos_str2int(rsc_id, 10, (void *)&zone_id, NUM_TPYE_UINT32);
    return_val_do_info_if_fail((VOS_OK == ret) && (zone_id < INVALID_VAL), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: invalid zone id:[%s]", __FUNCTION__, rsc_id));

    ret = dal_get_specific_object_byte(CLASS_NAME_COMPOSITION_ZONE, PROPERTY_COMPOSITION_ZONE_ID, (guchar)zone_id,
        composition_zone_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: cann't find object handle for [%s] with [%s:%d]", __FUNCTION__,
        CLASS_NAME_COMPOSITION_ZONE, PROPERTY_COMPOSITION_ZONE_ID, zone_id));

    return VOS_OK;
}


gint32 zone_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    ret = __check_zone_uri_valid(i_paras->member_id, &i_paras->obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s: check rsc id failed, ret is %d", __FUNCTION__, ret));

    *prop_provider = g_zone_provider;
    
    *count = 0;

    return VOS_OK;
}


LOCAL gint32 __get_endpoint_uri_with_identifier_handle(OBJ_HANDLE obj_handle, gchar *ep_uri, guint32 ep_uri_len)
{
    gint32 ret;
    gchar container[MAX_NAME_SIZE] = {0};
    gchar shortname[MAX_NAME_SIZE] = {0};
    gchar ep_id[MAX_RSC_ID_LEN] = {0};
    const gchar *obj_name = NULL;
    guchar ep_identifier_id = 0;
    guchar container_presence = 0;

    return_val_do_info_if_fail((0 != obj_handle) && (NULL != ep_uri) && (0 != ep_uri_len), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    obj_name = dfl_get_object_name(obj_handle);
    return_val_do_info_if_fail(NULL != obj_name, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: cann't get object name for handle [%" OBJ_HANDLE_FORMAT "]", __FUNCTION__,
        obj_handle));

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_NAME_EP_CONTAINER_PRESENCE, &container_presence);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_NAME_EP_IDENTIFIER_ID, &ep_identifier_id);

    if (0 == container_presence || (INVALID_EP_IDENTIFIER_ID == ep_identifier_id)) {
        return VOS_ERR;
    }

    ret = dal_get_property_value_string(obj_handle, PROPERTY_NAME_EP_CONTAINER, container, sizeof(container));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: cann't get property value for [%s:%s]", __FUNCTION__, obj_name,
        PROPERTY_NAME_EP_CONTAINER));

    ret =
        dal_get_property_value_string(obj_handle, PROPERTY_NAME_EP_IDENTIFIER_SHORT_NAME, shortname, sizeof(shortname));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: cann't get property value for [%s:%s]", __FUNCTION__, obj_name,
        PROPERTY_NAME_EP_IDENTIFIER_SHORT_NAME));

    ret = snprintf_s(ep_id, sizeof(ep_id), sizeof(ep_id) - 1, "%s%s", container, shortname);
    return_val_do_info_if_fail(ret > 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: format endpoint id failed, ret is %d", __FUNCTION__, ret));

    ret = snprintf_s(ep_uri, ep_uri_len, ep_uri_len - 1, URI_FORMAT_ENDPOINT, RF_PCIE_FABRIC, ep_id);
    return_val_do_info_if_fail(ret > 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: format endpoint uri failed, ret is %d", __FUNCTION__, ret));

    return VOS_OK;
}


gint32 get_endpoint_collection_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gint32 ret;
    GSList *ep_identifier_list = NULL;
    GSList *ep_list_iter = NULL;
    json_object *ep_members_jso = NULL;
    OBJ_HANDLE ep_identifier_handle;
    gchar               ep_uri[MAX_URI_LENGTH] = {0};
    gint32 ep_members_cnt;

    if (i_paras == NULL || o_rsc_jso == NULL || o_err_array_jso == NULL) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return RF_FAILED;
    }

    ep_members_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != ep_members_jso, RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed: alloc new json array failed.", __FUNCTION__));

    
    json_object_object_add(o_rsc_jso, RFPROP_MEMBERS, ep_members_jso);
    json_object_object_add(o_rsc_jso, RFPROP_MEMBERS_COUNT, 0);

    ret = dfl_get_object_list(CLASS_NAME_ENDPOINT_IDENTIFIER, &ep_identifier_list);
    return_val_do_info_if_fail(VOS_OK == ret, RF_OK,
        debug_log(DLOG_MASS, "%s: cann't get any object handle for [%s]", __FUNCTION__,
        CLASS_NAME_ENDPOINT_IDENTIFIER));

    for (ep_list_iter = ep_identifier_list; NULL != ep_list_iter; ep_list_iter = ep_list_iter->next) {
        (void)memset_s((void *)ep_uri, sizeof(ep_uri), 0, sizeof(ep_uri));

        ep_identifier_handle = (OBJ_HANDLE)ep_list_iter->data;
        ret = __get_endpoint_uri_with_identifier_handle(ep_identifier_handle, ep_uri, sizeof(ep_uri));
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "%s: get endpoint uri for handle [%s] failed",
            __FUNCTION__, dfl_get_object_name(ep_identifier_handle)));

        RF_ADD_ODATA_ID_MEMBER(ep_members_jso, ep_uri);
    }

    ep_members_cnt = json_object_array_length(ep_members_jso);
    json_object_object_add(o_rsc_jso, RFPROP_MEMBERS_COUNT, json_object_new_int(ep_members_cnt));
    g_slist_free(ep_identifier_list);

    return RF_OK;
}


LOCAL PROPERTY_PROVIDER_S g_endpoint_collection_provider[] = {
};


gint32 endpoint_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    OBJ_HANDLE ep_identifier_handle = 0;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    (void)dal_get_object_handle_nth(CLASS_NAME_ENDPOINT_IDENTIFIER, 0, &ep_identifier_handle);
    return_val_if_fail(0 != ep_identifier_handle, HTTP_NOT_FOUND);

    *prop_provider = g_endpoint_collection_provider;
    
    *count = 0;

    return VOS_OK;
}


gint32 get_zone_collection_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gint32 ret;
    GSList *composition_zone_list = NULL;
    GSList *list_iter = NULL;
    OBJ_HANDLE obj_handle_iter;
    const gchar *obj_iter_name = NULL;
    json_object *zone_members_array_jso = NULL;
    guchar composition_zone_id;
    gchar               composition_zone_member_uri[MAX_URI_LENGTH] = {0};

    if (i_paras == NULL || o_rsc_jso == NULL || o_err_array_jso == NULL) {
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__);
        return RF_FAILED;
    }

    zone_members_array_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != zone_members_array_jso, RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed: alloc json array failed", __FUNCTION__));

    
    json_object_object_add(o_rsc_jso, RFPROP_MEMBERS, zone_members_array_jso);
    json_object_object_add(o_rsc_jso, RFPROP_MEMBERS_COUNT, 0);

    ret = dfl_get_object_list(CLASS_NAME_COMPOSITION_ZONE, &composition_zone_list);
    return_val_do_info_if_expr(VOS_OK != ret, RF_OK,
        debug_log(DLOG_DEBUG, "%s: no object found for [%s], ret is %d", __FUNCTION__, CLASS_NAME_COMPOSITION_ZONE,
        ret));

    for (list_iter = composition_zone_list; NULL != list_iter; list_iter = list_iter->next) {
        obj_handle_iter = (OBJ_HANDLE)list_iter->data;

        obj_iter_name = dfl_get_object_name(obj_handle_iter);
        continue_do_info_if_fail(NULL != obj_iter_name,
            debug_log(DLOG_ERROR, "%s: cann't get object name for %" OBJ_HANDLE_FORMAT, __FUNCTION__, obj_handle_iter));

        composition_zone_id = 0;
        ret = dal_get_property_value_byte(obj_handle_iter, PROPERTY_COMPOSITION_ZONE_ID, &composition_zone_id);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s failed: cann't get property value for [%s:%s], ret is %d", __FUNCTION__,
            obj_iter_name, PROPERTY_COMPOSITION_ZONE_ID, ret));

        
        (void)memset_s(composition_zone_member_uri, sizeof(composition_zone_member_uri), 0,
            sizeof(composition_zone_member_uri));
        ret = snprintf_s(composition_zone_member_uri, sizeof(composition_zone_member_uri),
            sizeof(composition_zone_member_uri) - 1, URI_FORMAT_ZONE, RF_PCIE_FABRIC, composition_zone_id);
        continue_do_info_if_fail(ret > 0, debug_log(DLOG_ERROR,
            "%s failed: format composition zone uri for [%s] failed, ret is %d", __FUNCTION__, obj_iter_name, ret));

        RF_ADD_ODATA_ID_MEMBER(zone_members_array_jso, composition_zone_member_uri);
    }

    g_slist_free(composition_zone_list);

    
    json_object_object_add(o_rsc_jso, RFPROP_MEMBERS_COUNT,
        json_object_new_int(json_object_array_length(zone_members_array_jso)));

    return RF_OK;
}


LOCAL PROPERTY_PROVIDER_S g_zone_collection_provider[] = {
};


gint32 zone_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    OBJ_HANDLE ep_identifier_handle = 0;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    (void)dal_get_object_handle_nth(CLASS_NAME_ENDPOINT_IDENTIFIER, 0, &ep_identifier_handle);
    return_val_if_fail(0 != ep_identifier_handle, HTTP_NOT_FOUND);

    *prop_provider = g_zone_collection_provider;
    
    *count = 0;

    return VOS_OK;
}


LOCAL PROPERTY_PROVIDER_S g_config_composition_action_info_provider[] = {
};


gint32 config_composition_action_info_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    OBJ_HANDLE ep_identifier_handle = 0;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    
    (void)dal_get_object_handle_nth(CLASS_NAME_ENDPOINT_IDENTIFIER, 0, &ep_identifier_handle);
    return_val_if_fail(0 != ep_identifier_handle, HTTP_NOT_FOUND);

    *prop_provider = g_config_composition_action_info_provider;
    
    *count = 0;

    return VOS_OK;
}


LOCAL void __get_composition_mode_related_zone_info(OBJ_HANDLE composition_setting_handle,
    json_object **related_zone_jso)
{
    const guchar *related_zone_array = NULL;
    guint32 zone_array_len;
    GVariant *related_zone_var = NULL;
    gint32 ret;
    const gchar *obj_name = NULL;
    gchar               zone_uri[MAX_URI_LENGTH] = {0};
    guint32 i;
    json_object *zone_uri_jso = NULL;
    gsize parm_temp = 0;

    return_do_info_if_fail(NULL != related_zone_jso,
        debug_log(DLOG_ERROR, "%s failed: invalid input param", __FUNCTION__));

    *related_zone_jso = json_object_new_array();
    return_do_info_if_fail(NULL != *related_zone_jso,
        debug_log(DLOG_ERROR, "%s failed: alloc related zone jso failed", __FUNCTION__));

    obj_name = dfl_get_object_name(composition_setting_handle);
    return_do_info_if_fail(NULL != obj_name, debug_log(DLOG_ERROR,
        "%s failed: cann't get object name for %" OBJ_HANDLE_FORMAT, __FUNCTION__, composition_setting_handle));

    ret = dfl_get_property_value(composition_setting_handle, PROPERTY_COMPOSITION_SETTING_ZONES, &related_zone_var);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s failed: cann't get property value for [%s:%s], ret is %d", __FUNCTION__, obj_name,
        PROPERTY_COMPOSITION_SETTING_ZONES, ret));

    related_zone_array = (const guchar *)g_variant_get_fixed_array(related_zone_var, &parm_temp, sizeof(guchar));
    zone_array_len = (guint32)parm_temp;
    return_do_info_if_fail((NULL != related_zone_array),
        debug_log(DLOG_ERROR, "%s failed: cann't get array variant for %s", __FUNCTION__, obj_name);
        g_variant_unref(related_zone_var));

    
    for (i = 0; i < zone_array_len; i++) {
        ret = snprintf_s(zone_uri, sizeof(zone_uri), sizeof(zone_uri) - 1, URI_FORMAT_ZONE, RF_PCIE_FABRIC,
            related_zone_array[i]);
        continue_do_info_if_fail(ret > 0, debug_log(DLOG_ERROR, "%s failed: format zone uri with %d failed, ret is %d",
            __FUNCTION__, related_zone_array[i], ret));

        zone_uri_jso = json_object_new_object();
        continue_do_info_if_fail(NULL != zone_uri_jso,
            debug_log(DLOG_ERROR, "%s failed: cann't alloc new json object", __FUNCTION__));

        json_object_object_add(zone_uri_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)zone_uri));
        ret = json_object_array_add(*related_zone_jso, zone_uri_jso);
        continue_do_info_if_fail(0 == ret,
            debug_log(DLOG_ERROR, "%s: add item jso to zone info array failed, ret is %d", __FUNCTION__, ret));
    }

    g_variant_unref(related_zone_var);

    return;
}


LOCAL void __get_available_composition_mode_info(json_object **available_mode_jso)
{
    gint32 ret;
    OBJ_HANDLE composition_obj_handle = 0;
    GSList *composition_setting_obj_list = NULL;
    GSList *composition_setting_list_iter = NULL;
    OBJ_HANDLE composition_setting_iter_handle;
    guchar setting_available = 0;
    guchar setting_id = 0;
    
    json_object *available_mode_item_jso = NULL;
    
    json_object *zone_jso = NULL;

    return_do_info_if_fail((NULL != available_mode_jso),
        debug_log(DLOG_ERROR, "%s failed: invalid input param", __FUNCTION__));

    *available_mode_jso = json_object_new_array();
    return_do_info_if_fail(NULL != *available_mode_jso,
        debug_log(DLOG_ERROR, "%s failed: alloc new available mode jso failed", __FUNCTION__));

    ret = dal_get_object_handle_nth(CLASS_NAME_COMPOSITION, 0, &composition_obj_handle);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_MASS, "%s: cann't get composition handle, ret is %d", __FUNCTION__, ret));

    ret = dal_get_object_list_position(composition_obj_handle, CLASS_NAME_COMPOSITION_SETTING,
        &composition_setting_obj_list);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_MASS, "%s: cann't get composition setting list with the same position as [%s]", __FUNCTION__,
        dfl_get_object_name(composition_obj_handle)));

    for (composition_setting_list_iter = composition_setting_obj_list; NULL != composition_setting_list_iter;
        composition_setting_list_iter = composition_setting_list_iter->next) {
        composition_setting_iter_handle = (OBJ_HANDLE)composition_setting_list_iter->data;

        setting_available = 0;
        (void)dal_get_property_value_byte(composition_setting_iter_handle, PROPERTY_COMPOSITION_SETTING_AVAILABLE,
            &setting_available);
        continue_if_fail(ENABLE == setting_available);

        available_mode_item_jso = json_object_new_object();
        continue_do_info_if_fail(NULL != available_mode_item_jso,
            debug_log(DLOG_ERROR, "%s: alloc available mode item jso failed", __FUNCTION__));

        
        setting_id = 0;
        (void)dal_get_property_value_byte(composition_setting_iter_handle, PROPERTY_COMPOSITION_SETTING_ID,
            &setting_id);
        json_object_object_add(available_mode_item_jso, RFPROP_COMPOSITION_MODE_ID, json_object_new_int(setting_id));

        
        zone_jso = NULL;
        __get_composition_mode_related_zone_info(composition_setting_iter_handle, &zone_jso);
        json_object_object_add(available_mode_item_jso, RFPROP_COMPOSITION_RELATED_ZONES, zone_jso);

        json_object_array_add(*available_mode_jso, available_mode_item_jso);
    }

    g_slist_free(composition_setting_obj_list);

    return;
}


LOCAL void get_pcie_fabric_oem_info(PROVIDER_PARAS_S *i_paras, json_object *o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE composition_handle = 0;
    json_object *oem_jso = NULL;
    json_object *hw_jso = NULL;
    guchar cur_setting_id = 0;
    guchar pendding_setting_id = 0;
    json_object *composition_setting_jso = NULL;
    json_object *pendding_setting_jso = NULL;
    json_object *available_mode_jso = NULL;

    (void)json_object_object_get_ex(o_result_jso, RFPROP_COMMON_OEM, &oem_jso);
    (void)json_object_object_get_ex(oem_jso, RFPROP_COMMON_HUAWEI, &hw_jso);
    return_do_info_if_fail(json_type_object == json_object_get_type(hw_jso),
        debug_log(DLOG_ERROR, "%s failed: cann't get property", __FUNCTION__));

    ret = dal_get_object_handle_nth(CLASS_NAME_COMPOSITION, 0, &composition_handle);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_MASS, "%s failed: cann't find compositon handle", __FUNCTION__);
        json_object_object_add(hw_jso, RFPROP_COMPOSITION_MODE_INFO, NULL));

    
    (void)dal_get_property_value_byte(composition_handle, PROPERTY_COMPOSITION_CURRENT_SETTING, &cur_setting_id);
    (void)dal_get_property_value_byte(composition_handle, PROPERTY_COMPOSITION_PENDDING_SETTING, &pendding_setting_id);

    composition_setting_jso = json_object_new_object();
    return_do_info_if_fail(NULL != composition_setting_jso,
        debug_log(DLOG_ERROR, "%s failed: alloc new composition setting jso failed", __FUNCTION__));

    json_object_object_add(hw_jso, RFPROP_COMPOSITION_MODE_INFO, composition_setting_jso);
    json_object_object_add(composition_setting_jso, RFPROP_COMPOSITION_CURRENT_SETTING,
        json_object_new_int(cur_setting_id));

    
    if (cur_setting_id == pendding_setting_id) {
        pendding_setting_jso = NULL;
    } else {
        pendding_setting_jso = json_object_new_int(pendding_setting_id);
    }
    json_object_object_add(composition_setting_jso, RFPROP_COMPOSITION_PENDDING_SETTING, pendding_setting_jso);

    
    __get_available_composition_mode_info(&available_mode_jso);
    json_object_object_add(composition_setting_jso, RFPROP_COMPOSITION_AVALIBLE_MODE, available_mode_jso);

    return;
}


gint32 get_pcie_fabric_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    (void)get_pcie_fabric_oem_info(i_paras, o_rsc_jso);

    return RF_OK;
}


LOCAL gint32 _pcie_fabric_config_composition_action_pre_check(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 composition_mode_id = 0;
    gint32 ret;
    gchar               mode_id_str[MAX_STRBUF_LEN] = {0};
    OBJ_HANDLE composition_setting_handle = 0;
    guchar mode_available_status = 0;
    gint32 available_task_id;

    
    if (0 == (USERROLE_BASICSETTING & (i_paras->user_role_privilege))) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);

        return HTTP_FORBIDDEN;
    }

    
    (void)get_element_int(i_paras->val_jso, RFACTION_PARAM_MODE_ID, &composition_mode_id);

    (void)snprintf_s(mode_id_str, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "%d", composition_mode_id);

    return_val_do_info_if_fail((composition_mode_id > 0) && (composition_mode_id < MAX_COMPOSITION_SETTING_CNT),
        HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s failed: composition id [%d] exceed limt [%d]", __FUNCTION__, composition_mode_id,
        MAX_COMPOSITION_SETTING_CNT);
        (void)create_message_info(MSGID_INVALID_ACTION_PARAM_VALUE, NULL, o_message_jso, mode_id_str,
        RFACTION_PARAM_MODE_ID));

    ret = dal_get_specific_object_byte(CLASS_NAME_COMPOSITION_SETTING, PROPERTY_COMPOSITION_SETTING_ID,
        (guchar)composition_mode_id, &composition_setting_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s failed: get composition handle with [%s:%d] failed", __FUNCTION__,
        PROPERTY_COMPOSITION_SETTING_ID, composition_mode_id);
        (void)create_message_info(MSGID_INVALID_ACTION_PARAM_VALUE, NULL, o_message_jso, mode_id_str,
        RFACTION_PARAM_MODE_ID));

    (void)dal_get_property_value_byte(composition_setting_handle, PROPERTY_COMPOSITION_SETTING_AVAILABLE,
        &mode_available_status);
    return_val_do_info_if_fail(ENABLE == mode_available_status, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s failed: composition setting [%d] is not available", __FUNCTION__,
        composition_mode_id);
        (void)create_message_info(MSGID_INVALID_ACTION_PARAM_VALUE, NULL, o_message_jso, mode_id_str,
        RFACTION_PARAM_MODE_ID));

    
    available_task_id = find_available_task_id();
    return_val_do_info_if_fail(RF_INVALID_TASK_ID != available_task_id, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_TASK_LIMIT_EXCEED, NULL, o_message_jso));

    return VOS_OK;
}


LOCAL gint32 __composition_config_progress_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar topo_config_status = 0;

    ret = dal_get_object_handle_nth(CLASS_NAME_COMPOSITION, 0, &obj_handle);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "%s failed: cann't get composition handle, ret is %d", __FUNCTION__, ret));

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPOSITION_TOPO_CFG_STATUS, &topo_config_status);

    
    switch (topo_config_status) {
        case PCIeTopo_CONFIG_FINISHED:
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
            monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
            (void)create_message_info(MSGID_PCIE_TOPO_CONFIG_OK, NULL, message_obj);
            break;

        case PCIeTopo_CONFIG_TASK_RUNNING:
            monitor_fn_data->task_state = RF_TASK_RUNNING;
            break;

        default:
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            (void)create_message_info(MSGID_PCIE_TOPO_CONFIG_FAILED, NULL, message_obj,
                dal_get_pcie_topo_code_string((PCIeTopoCode)topo_config_status));
            break;
    }

    return RF_OK;

exit:
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;

    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);

    return RF_OK; 
}


LOCAL gint32 act_pcie_fabric_config_composition(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 mode_id = 0;
    GSList *input_list = NULL;
    OBJ_HANDLE composition_handle = 0;
    TASK_MONITOR_INFO_S *task_monitor_info = NULL;

    
    ret = _pcie_fabric_config_composition_action_pre_check(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s failed: precheck composition config action failed", __FUNCTION__));

    (void)get_element_int(i_paras->val_jso, RFACTION_PARAM_MODE_ID, &mode_id);

    
    (void)dal_get_object_handle_nth(CLASS_NAME_COMPOSITION, 0, &composition_handle);

    input_list = g_slist_append(input_list, g_variant_new_byte((guchar)mode_id));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, composition_handle,
        CLASS_NAME_COMPOSITION, METHOD_COMPOSITION_CONFIG_COMPOSITION, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    

    if (VOS_OK != ret) {
        (void)create_message_info(MSGID_PCIE_TOPO_CONFIG_FAILED, NULL, o_message_jso,
            dal_get_pcie_topo_code_string((PCIeTopoCode)ret));

        return HTTP_BAD_REQUEST;
    }

    
    task_monitor_info = task_monitor_info_new(NULL);
    goto_label_do_info_if_fail(NULL != task_monitor_info, err_exit,
        debug_log(DLOG_ERROR, "%s failed: alloc new task monitor info failed", __FUNCTION__));
    

    task_monitor_info->task_progress = TASK_NO_PROGRESS; 
    task_monitor_info->rsc_privilege = USERROLE_BASICSETTING;
    ret =
        create_new_task(RF_TASK_NAME_CONFIG_COMPOSITION, (task_status_monitor_fn)__composition_config_progress_monitor,
        task_monitor_info, i_paras->val_jso, i_paras->uri, o_result_jso);

    goto_label_do_info_if_fail(VOS_OK == ret, err_exit, task_monitor_info_free(task_monitor_info);
        debug_log(DLOG_ERROR, "%s: create task for %s failed", __FUNCTION__, RF_TASK_NAME_CONFIG_COMPOSITION));

    return HTTP_ACCEPTED;

err_exit:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL PROPERTY_PROVIDER_S g_fabric_provider[] = {
    {
        RFACTION_CONFIG_PCIE_TOPO,
        "", "",
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_pcie_fabric_config_composition,
        ETAG_FLAG_ENABLE
    }
};


gint32 pcie_fabric_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    OBJ_HANDLE ep_identifier_handle = 0;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    
    (void)dal_get_object_handle_nth(CLASS_NAME_ENDPOINT_IDENTIFIER, 0, &ep_identifier_handle);
    return_val_if_fail(0 != ep_identifier_handle, HTTP_NOT_FOUND);

    *prop_provider = g_fabric_provider;
    *count = G_N_ELEMENTS(g_fabric_provider);

    return VOS_OK;
}


LOCAL PROPERTY_PROVIDER_S g_fabric_collection_provider[] = {
};


gint32 fabric_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    OBJ_HANDLE ep_identifier_handle = 0;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    (void)dal_get_object_handle_nth(CLASS_NAME_ENDPOINT_IDENTIFIER, 0, &ep_identifier_handle);
    return_val_if_fail(0 != ep_identifier_handle, HTTP_NOT_FOUND);

    *prop_provider = g_fabric_collection_provider;
    
    *count = 0;

    return VOS_OK;
}
