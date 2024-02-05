
#include <stdlib.h>
#include "pme_app/pme_app.h"
#include "card_upgrade.h"
#include "pcie_card.h"
#include "card_manage_module.h"
#include "pcie_card_parse_bdf_thread.h"

LOCAL gint32 __upgrade_pcie_card_verifier(const gchar *filename);
LOCAL gint32 __upgrade_pcie_card_handler(UPGRADE_PARAM_S *up_param);

LOCAL UPGRADE_TABLE_ELEMENT_S g_card_xml_upgrade_table[] = {
    { .bom_id = "14140130",
      .card_type = UP_PCIE_CARD_TYPE,
      .handler = __upgrade_pcie_card_handler,
      .verifier = __upgrade_pcie_card_verifier }
};


LOCAL void __init_upgrade_param(UPGRADE_PARAM_S *up_param, const gchar *filename, UPGRADE_FUNC_S *up_func)
{
    up_param->filename = filename;
    up_param->need_verify = FALSE;
    up_param->card_type = UP_UNKNOWN_TYPE;
    for (guint32 i = 0; i < (sizeof(g_card_xml_upgrade_table) / sizeof(UPGRADE_TABLE_ELEMENT_S)); i++) {
        const gchar *tmp_bom_id = g_card_xml_upgrade_table[i].bom_id;
        if (strncmp(filename, tmp_bom_id, strlen(tmp_bom_id)) == 0) {
            up_func->handler = g_card_xml_upgrade_table[i].handler;
            up_func->verifier = g_card_xml_upgrade_table[i].verifier;
            up_param->card_type = g_card_xml_upgrade_table[i].card_type;
            return;
        }
    }
    return;
}


LOCAL gint32 __parse_id_aux_id(const gchar *filename, guint32 *id, guint32 *aux_id)
{
    gint32 ret;
    gchar *sub_str = NULL;
    gchar *bom_id = NULL;
    guint32 tmp_id = PCIE_INVALID_BOARD_ID;
    guint32 tmp_aux_id = PCIE_INVALID_VID_DID;
    gchar tmp_filename[MAX_FILEPATH_LENGTH + 1] = {0};
    ret = strncpy_s(tmp_filename, sizeof(tmp_filename), filename, sizeof(tmp_filename) - 1);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s failed.", __FUNCTION__);
        return RET_ERR;
    }
    bom_id = strtok_s(tmp_filename, "_", &sub_str);
    if (bom_id == NULL) {
        debug_log(DLOG_ERROR, "%s: strtok_s failed.", __FUNCTION__);
        return RET_ERR;
    }
    ret = sscanf_s(sub_str, "%08x_%08x.xml", &tmp_id, &tmp_aux_id);
    if (ret != 2) { // 2: 读取id, aux_id共两个个值
        debug_log(DLOG_ERROR, "%s: sscanf_s id, aux_id failed.", __FUNCTION__);
        return RET_ERR;
    }
    (*id) = tmp_id;
    (*aux_id) = tmp_aux_id;
    return RET_OK;
}


LOCAL gint32 __parse_pcie_ids(const gchar *filename, guint16 *vid, guint16 *did, guint16 *svid, guint16 *sdid)
{
    gint32 ret;
    gchar *sub_str = NULL;
    gchar *bom_id = NULL;
    gchar tmp_filename[MAX_FILEPATH_LENGTH + 1] = {0};
    ret = strncpy_s(tmp_filename, sizeof(tmp_filename), filename, sizeof(tmp_filename) - 1);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s failed.", __FUNCTION__);
        return RET_ERR;
    }
    bom_id = strtok_s(tmp_filename, "_", &sub_str);
    if (bom_id == NULL) {
        debug_log(DLOG_ERROR, "%s: strtok_s failed.", __FUNCTION__);
        return RET_ERR;
    }
    ret = sscanf_s(sub_str, "%04hx%04hx_%04hx%04hx.xml", vid, did, svid, sdid);
    if (ret != 4) { // 4: 读取四元组信息共四个值
        debug_log(DLOG_ERROR, "%s: sscanf_s vid, did, svid, sdid failed.", __FUNCTION__);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 __is_object_matched(OBJ_HANDLE obj_handle, VALIDATE_KV_PAIRS_S *validate_param)
{
    gint32 ret;
    GSList *keys = validate_param->keys;
    GSList *values = validate_param->expected_vals;
    guint32 keys_count = validate_param->count;
    GVariant *property_data = NULL;
    const char *property_name = NULL;

    debug_log(DLOG_DEBUG, "%s: Start matching obj, keys_count = %d.", __FUNCTION__, keys_count);
    for (guint32 i = 0; i < keys_count; i++) {
        property_name = g_variant_get_string((GVariant *)g_slist_nth_data(keys, i), NULL);
        if (dfl_get_property_value(obj_handle, property_name, &property_data) != RET_OK) {
            return RET_ERR;
        }
        ret = g_variant_equal(property_data, (GVariant *)g_slist_nth_data(values, i));
        g_variant_unref(property_data);
        property_data = NULL;
        if (ret == 0) { // 属性值不相等
            return RET_ERR;
        }
    }

    return RET_OK;
}

LOCAL gint32 __get_pcie_card_connector(OBJ_HANDLE pcie_card_handle, OBJ_HANDLE *connector_handle)
{
    gint32 ret;
    guint32 position = 0xffff;
    const gchar *obj_name = NULL;
    const gchar *pos_str = NULL;
    GVariant *property_value = NULL;

    obj_name = dfl_get_object_name(pcie_card_handle);
    if (obj_name == NULL) {
        debug_log(DLOG_ERROR, "%s: Get obj_name failed.", __FUNCTION__);
        return RET_ERR;
    }
    
    pos_str = g_strrstr(obj_name, "-");
    if (pos_str == NULL) {
        debug_log(DLOG_ERROR, "%s: Get position string failed.", __FUNCTION__);
        return RET_ERR;
    }
    
    ret = sscanf_s(pos_str, "-%x", &position);
    if (ret != 1) {
        debug_log(DLOG_ERROR, "%s: Get position failed.", __FUNCTION__);
        return RET_ERR;
    }
    property_value = g_variant_new_uint32(position);
    ret = dfl_get_specific_object(CLASS_CONNECTOR_NAME, PROPERTY_CONNECTOR_POSITION, property_value, connector_handle);
    g_variant_unref(property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get specific object with pos(%d) failed, ret = %d.", __FUNCTION__, position, ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 __check_connector_id_auxid(OBJ_HANDLE pcie_card_handle)
{
    gint32 ret;
    OBJ_HANDLE connector_handle = 0;
    guint32 id = PCIE_INVALID_BOARD_ID;
    guint32 aux_id = PCIE_INVALID_VID_DID;
    ret = __get_pcie_card_connector(pcie_card_handle, &connector_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get connector handle failed, ret = %d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    (void)dal_get_property_value_uint32(connector_handle, PROPERTY_CONNECTOR_AUXID, &aux_id);
    if (aux_id == PCIE_INVALID_VID_DID) {
        debug_log(DLOG_ERROR, "%s: Aux Id of connector is default value, ret = %d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    (void)dal_get_property_value_uint32(connector_handle, PROPERTY_CONNECTOR_ID, &id);
    if (id == PCIE_INVALID_BOARD_ID) {
        debug_log(DLOG_ERROR, "%s: Id of connector is default value, ret = %d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}
LOCAL gint32 __check_validate_result(const gchar *class_name, VALIDATE_KV_PAIRS_S *validate_param)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *node = NULL;
    OBJ_HANDLE obj_handle;

    ret = dfl_get_object_list(class_name, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get object list failed, ret = %d.", __FUNCTION__, ret);
        return RET_ERR;
    }
    for (node = obj_list; node != NULL; node = node->next) {
        obj_handle = (OBJ_HANDLE)node->data;
        if (__is_object_matched(obj_handle, validate_param) != RET_OK) {
            continue;
        }
        debug_log(DLOG_DEBUG, "%s: All property values matched.", __FUNCTION__);

        
        ret = __check_connector_id_auxid(obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Check connector id aux_id failed.", __FUNCTION__);
            break;
        }
        g_slist_free(obj_list);
        return RET_OK;
    }
    g_slist_free(obj_list);
    return RET_ERR;
}


LOCAL gint32 __upgrade_pcie_card_verifier(const gchar *filename)
{
    gint32 ret;
    GSList *keys = NULL;
    GSList *expected_values = NULL;
    guint16 vid = PCIE_INVALID_VID;
    guint16 did = PCIE_INVALID_DID;
    guint16 svid = PCIE_INVALID_SUBVID;
    guint16 sdid = PCIE_INVALID_SUBDID;
    VALIDATE_KV_PAIRS_S validate_param = {
        .keys = NULL,
        .expected_vals = NULL,
        .count = 0
    };

    ret = __parse_pcie_ids(filename, &vid, &did, &svid, &sdid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Parse pcie info failed.", __FUNCTION__);
        return RET_ERR;
    }
    validate_param.count = PCIE_CARD_KEY_LEN;
    keys = g_slist_append(keys, g_variant_new_string(PROPERTY_PCIE_CARD_VID));
    keys = g_slist_append(keys, g_variant_new_string(PROPERTY_PCIE_CARD_DID));
    keys = g_slist_append(keys, g_variant_new_string(PROPERTY_PCIE_CARD_SVID));
    keys = g_slist_append(keys, g_variant_new_string(PROPERTY_PCIE_CARD_SDID));
    expected_values = g_slist_append(expected_values, g_variant_new_uint16(vid));
    expected_values = g_slist_append(expected_values, g_variant_new_uint16(did));
    expected_values = g_slist_append(expected_values, g_variant_new_uint16(svid));
    expected_values = g_slist_append(expected_values, g_variant_new_uint16(sdid));

    validate_param.keys = keys;
    validate_param.expected_vals = expected_values;
    ret = __check_validate_result(CLASS_PCIE_CARD, &validate_param);
    g_slist_free_full(keys, (GDestroyNotify)g_variant_unref);
    g_slist_free_full(expected_values, (GDestroyNotify)g_variant_unref);

    return ret;
}


LOCAL gboolean __match_pcie_info_with_filename(const gchar *filename, PCIE_CARD_INFO_S did_vid,
    PCIE_CARD_SUB_INFO_S sdid_svid)
{
    gint32 ret;
    guint16 vid = PCIE_INVALID_VID;
    guint16 did = PCIE_INVALID_DID;
    guint16 svid = PCIE_INVALID_SUBVID;
    guint16 sdid = PCIE_INVALID_SUBDID;

    ret = __parse_pcie_ids(filename, &vid, &did, &svid, &sdid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Parse pcie info failed.", __FUNCTION__);
        return FALSE;
    }
    if ((vid != (did_vid.vender_id)) || (did != (did_vid.device_id)) || (svid != (sdid_svid.sub_vender_id)) ||
        (sdid != (sdid_svid.sub_device_id))) {
        return FALSE;
    }
    return TRUE;
}


LOCAL gint32 __init_pcie_validation_param(const gchar *filename, PCIE_XML_UP_PARAM_S *pcie_param, guint8 card_type)
{
    gint32 ret;
    guint8 slot;
    PCIE_CARD_GROUP_INFO_S *group_info = NULL;
    PCIE_CARD_GROUP_SUB_INFO_S *group_sub_info = NULL;

    get_group_info_by_type(card_type, &group_info, &group_sub_info);
    if (group_info == NULL || group_sub_info == NULL) {
        debug_log(DLOG_ERROR, "%s: Get group info by type failed.", __FUNCTION__);
        return RET_ERR;
    }

    pcie_param->pcie_count = 0;
    pcie_param->pcie_type = card_type;
    (void)memset_s(&(pcie_param->pcie_index), sizeof(pcie_param->pcie_index), 0, sizeof(pcie_param->pcie_index));
    (void)memset_s(&(pcie_param->slot_id), sizeof(pcie_param->slot_id), 0, sizeof(pcie_param->slot_id));

    for (guint32 i = 0; i < group_info->count; i++) {
        if (__match_pcie_info_with_filename(filename, group_info->info[i], group_sub_info->sub_info[i]) == FALSE) {
            continue;
        }
        slot = PCIE_INVALID_SLOT_ID;
        ret = pcie_get_slot_by_index(pcie_param->pcie_type, i, &slot);
        if (ret != RET_OK || slot == PCIE_INVALID_SLOT_ID) {
            debug_log(DLOG_ERROR, "%s: Get slot id failed, ret = %d.", __FUNCTION__, ret);
            return RET_ERR;
        }
        pcie_param->slot_id[pcie_param->pcie_count] = slot;
        pcie_param->pcie_index[pcie_param->pcie_count] = i;
        (pcie_param->pcie_count)++;
    }

    return RET_OK;
}


LOCAL void __backup_pcie_info(SAVED_PCIE_INFO_S *saved_pcie_info, guint32 idx, guint8 card_type)
{
    PCIE_CARD_GROUP_INFO_S *group_info = NULL;
    PCIE_CARD_GROUP_SUB_INFO_S *group_sub_info = NULL;
    PCIE_CARD_STATUS_GROUP_S *pcie_status_group = NULL;
    PCIE_CARD_STATUS_GROUP_S *pcie_status_by_bios_group = NULL;

    get_group_info_by_type(card_type, &group_info, &group_sub_info);
    get_pcie_status_info(&pcie_status_group, &pcie_status_by_bios_group);

    if (group_info != NULL) {
        saved_pcie_info->vid = group_info->info[idx].vender_id;
        saved_pcie_info->did = group_info->info[idx].device_id;
    }

    if (group_sub_info != NULL) {
        saved_pcie_info->svid = group_sub_info->sub_info[idx].sub_vender_id;
        saved_pcie_info->sdid = group_sub_info->sub_info[idx].sub_device_id;
    }

    if (pcie_status_group != NULL) {
        saved_pcie_info->status = pcie_status_group->status[idx];
    }

    if (pcie_status_by_bios_group != NULL) {
        saved_pcie_info->status_by_bios = pcie_status_by_bios_group->status[idx];
    }

    return;
}


LOCAL void __restore_pcie_info(SAVED_PCIE_INFO_S *saved_pcie_info, guint32 idx, guint8 card_type)
{
    PCIE_CARD_GROUP_INFO_S *group_info = NULL;
    PCIE_CARD_GROUP_SUB_INFO_S *group_sub_info = NULL;
    PCIE_CARD_STATUS_GROUP_S *pcie_status_group = NULL;
    PCIE_CARD_STATUS_GROUP_S *pcie_status_by_bios_group = NULL;

    if ((saved_pcie_info) == NULL) {
        return;
    }

    get_group_info_by_type(card_type, &group_info, &group_sub_info);
    get_pcie_status_info(&pcie_status_group, &pcie_status_by_bios_group);

    if (group_info != NULL) {
        group_info->info[idx].vender_id = saved_pcie_info->vid;
        group_info->info[idx].device_id = saved_pcie_info->did;
        per_save((guint8 *)group_info);
    }

    if (group_sub_info != NULL) {
        group_sub_info->sub_info[idx].sub_vender_id = saved_pcie_info->svid;
        group_sub_info->sub_info[idx].sub_device_id = saved_pcie_info->sdid;
        per_save((guint8 *)group_sub_info);
    }

    if (pcie_status_group != NULL) {
        pcie_status_group->status[idx] = saved_pcie_info->status;
        per_save((guint8 *)pcie_status_group);
    }

    if (pcie_status_by_bios_group != NULL) {
        pcie_status_by_bios_group->status[idx] = saved_pcie_info->status_by_bios;
        per_save((guint8 *)pcie_status_by_bios_group);
    }

    return;
}

LOCAL gint32 __trigger_reload_xml(OBJ_HANDLE obj_handle, guint32 id, guint32 aux_id)
{
    gint32 ret;
    ret = dfl_notify_presence_v2(obj_handle, id, aux_id, 0);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Set connector presence to 0 failed, ret = %d.", __FUNCTION__, ret);
        return RET_ERR;
    }
    
    vos_task_delay(5000);

    ret = dfl_notify_presence_v2(obj_handle, id, aux_id, 1);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Set connector presence to 1 failed, ret = %d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 __upgrade_pcie_card_handler(UPGRADE_PARAM_S *up_param)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    PCIE_XML_UP_PARAM_S pcie_up_param = { 0 };
    guint32 id = PCIE_INVALID_BOARD_ID;
    guint32 aux_id = PCIE_INVALID_VID_DID;
    if (up_param == NULL) {
        debug_log(DLOG_ERROR, "%s: Invalid upgrade parameters.", __FUNCTION__);
        return RET_ERR;
    }
    ret = __parse_id_aux_id(up_param->filename, &id, &aux_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Parse filename failed.", __FUNCTION__);
        return RET_ERR;
    }

    ret = __init_pcie_validation_param(up_param->filename, &pcie_up_param, up_param->card_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Initialize Pcie card validation parameters failed, ret = %d.", __FUNCTION__, ret);
        return RET_ERR;
    }
    if (pcie_up_param.pcie_count == 0) {
        debug_log(DLOG_ERROR, "%s: No matched connector.", __FUNCTION__);
        return RET_OK;
    }
    debug_log(DLOG_DEBUG, "%s: Found %d matched PCIe connector(s).", __FUNCTION__, pcie_up_param.pcie_count);
    up_param->need_verify = TRUE;
    for (guint32 i = 0; i < pcie_up_param.pcie_count; i++) {
        obj_handle = 0;
        SAVED_PCIE_INFO_S saved_pcie_info = { 0xffff, 0xffff, 0xffff, 0xffff, 0xff, 0xff };
        ret = pcie_card_get_connector_handle(pcie_up_param.slot_id[i], &obj_handle, up_param->card_type);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Get pcie card object handle failed, ret = %d.", __FUNCTION__, ret);
            return RET_ERR;
        }
        __backup_pcie_info(&saved_pcie_info, pcie_up_param.pcie_index[i], up_param->card_type);
        ret = __trigger_reload_xml(obj_handle, id, aux_id);
        __restore_pcie_info(&saved_pcie_info, pcie_up_param.pcie_index[i], up_param->card_type);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: PCIe card reload xml failed, ret = %d.", __FUNCTION__, ret);
            return RET_ERR;
        }
    }
    
    vos_task_delay(6000);

    return RET_OK;
}


gint32 upgrade_card_xml(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    const gchar *filename = NULL;
    UPGRADE_PARAM_S up_param = { 0 };
    UPGRADE_FUNC_S up_func = { 0 };
    if (input_list == NULL || output_list == NULL) {
        debug_log(DLOG_ERROR, "%s: Invalid input parameters.", __FUNCTION__);
        return RET_ERR;
    }
    filename = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    if (filename == NULL) {
        debug_log(DLOG_ERROR, "%s: Get filename failed.", __FUNCTION__);
        return RET_ERR;
    }
    debug_log(DLOG_DEBUG, "%s: Upgrading xml file %s.", __FUNCTION__, filename);

    __init_upgrade_param(&up_param, filename, &up_func);
    if (up_param.card_type == UP_UNKNOWN_TYPE) {
        debug_log(DLOG_ERROR, "%s: Unsupported upgrade type, filename = %s.", __FUNCTION__, filename);
        *output_list = g_slist_append(*output_list, g_variant_new_int32(RET_SKIP_FILE));
        return RET_OK;
    }

    ret = up_func.handler(&up_param);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Validate stage failed.", __FUNCTION__);
        return RET_ERR;
    }

    if (up_param.need_verify == TRUE) {
        ret = up_func.verifier(filename);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Verify stage failed, filename = %s.", __FUNCTION__, filename);
            *output_list = g_slist_append(*output_list, g_variant_new_int32(RET_VERIFY_FAILED));
        } else {
            debug_log(DLOG_ERROR, "%s: %s upgrade successfully.", __FUNCTION__, filename);
            *output_list = g_slist_append(*output_list, g_variant_new_int32(RET_VERIFY_SUCCESS));
        }
        return RET_OK;
    }

    debug_log(DLOG_DEBUG, "%s: Skip card xml %s verification.", __FUNCTION__, filename);
    *output_list = g_slist_append(*output_list, g_variant_new_int32(RET_SKIP_FILE));

    return RET_OK;
}
