
#include "redfish_provider.h"

#define RECORD_ROOT "work_record"
#define RECORD_ID "recordId"
#define RECORD_COUNT "recordCount"
#define RECORD_SET "records"
#define RECORD_INFO "recordInfo"
#define RECORD_INFO_ID "id"
#define RECORD_INFO_USER "user"
#define RECORD_INFO_IP "ip"
#define RECORD_INFO_TIME "mod_time"
#define RECORD_INFO_DETAIL "detail_info"
#define STR_ADD "Add"
#define STR_MODIFY "Edit"
#define STR_DELETE "Delete"
#define STR_SUCCESS "successfully"
#define STR_FAILED "failed"


LOCAL void workrecord_log(PROVIDER_PARAS_S *i_paras, gchar *operate_str, gchar *result, gint32 record_id)
{
    GSList *caller_info = NULL;

    return_do_info_if_expr((NULL == i_paras) || (NULL == operate_str) || (NULL == result),
        debug_log(DLOG_ERROR, "input param error."));

    
    caller_info =
        g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(i_paras->is_from_webui)));
    
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));
    if (record_id >= 0) {
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "%s Work Record %d %s", operate_str, record_id,
            result);
    } else {
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "%s Work Record %s", operate_str, result);
    }
    uip_free_gvariant_list(caller_info);
}


LOCAL gint32 get_record_nodes(xmlDocPtr *doc, xmlNodePtr *root, xmlNodePtr *id_node, xmlNodePtr *num_node,
    xmlNodePtr *info_node)
{
    xmlDocPtr record_doc = NULL;
    xmlNodePtr root_node = NULL;
    xmlNodePtr child_node = NULL;
    xmlNodePtr child_content = NULL;
    glong file_size;

    return_val_if_expr(doc == NULL || root == NULL || id_node == NULL || num_node == NULL || info_node == NULL,
        VOS_ERR);
    
    file_size = vos_get_file_length(RECORD_FILE);
    if (file_size > 0) {
        record_doc = dal_open_xml_doc(RECORD_FILE);
    }

    if (record_doc == NULL) {
        debug_log(DLOG_ERROR, "%s,%d : Load xml failed, creat it.", __FUNCTION__, __LINE__);

        record_doc = xmlNewDoc(BAD_CAST "1.0");
        goto_label_do_info_if_expr(record_doc == NULL, __error,
            debug_log(DLOG_ERROR, "%s,%d : Create xml failed.", __FUNCTION__, __LINE__));

        root_node = xmlNewNode(NULL, BAD_CAST RECORD_ROOT);
        goto_label_do_info_if_expr(root_node == NULL, __error,
            debug_log(DLOG_ERROR, "%s,%d : Create root node failed.", __FUNCTION__, __LINE__));
        (void)xmlDocSetRootElement(record_doc, root_node);

        child_node = xmlNewNode(NULL, BAD_CAST RECORD_ID);
        child_content = xmlNewText(BAD_CAST "0");
        (void)xmlAddChild(child_node, child_content);
        (void)xmlAddChild(root_node, child_node);

        child_node = xmlNewNode(NULL, BAD_CAST RECORD_COUNT);
        child_content = xmlNewText(BAD_CAST "0");
        (void)xmlAddChild(child_node, child_content);
        (void)xmlAddChild(root_node, child_node);

        child_node = xmlNewNode(NULL, BAD_CAST RECORD_SET);
        (void)xmlAddChild(root_node, child_node);

        xmlKeepBlanksDefault(0);
        xmlIndentTreeOutput = 1;
        if ((vos_get_file_accessible(RECORD_FILE) == FALSE) && (errno == ENOENT)) {
            (void)proxy_create_file(RECORD_FILE, "w+", APACHE_UID, APPS_USER_GID, 0664);
        }
        (void)xmlSaveFormatFileEnc(RECORD_FILE, record_doc, "UTF-8", 1);

        xmlFreeDoc(record_doc);

        record_doc = NULL;
        record_doc = dal_open_xml_doc(RECORD_FILE);
    }

    if (record_doc) {
        
        *root = xmlDocGetRootElement(record_doc);
        goto_label_do_info_if_expr((*root == NULL) || (xmlStrcmp((*root)->name, (const xmlChar *)RECORD_ROOT)), __error,
            debug_log(DLOG_ERROR, "%s,%d : Can't find root node.", __FUNCTION__, __LINE__));

        
        *id_node = (*root)->children;
        goto_label_do_info_if_expr((*id_node == NULL) || (xmlStrcmp((*id_node)->name, (const xmlChar *)RECORD_ID)),
            __error, debug_log(DLOG_ERROR, "%s,%d : Can't find id node.", __FUNCTION__, __LINE__));

        
        *num_node = (*id_node)->next;
        goto_label_do_info_if_expr((*num_node == NULL) || (xmlStrcmp((*num_node)->name, (const xmlChar *)RECORD_COUNT)),
            __error, debug_log(DLOG_ERROR, "%s,%d : Can't find num node.", __FUNCTION__, __LINE__));

        
        *info_node = (*num_node)->next;
        goto_label_do_info_if_expr((*info_node == NULL) || (xmlStrcmp((*info_node)->name, (const xmlChar *)RECORD_SET)),
            __error, debug_log(DLOG_ERROR, "%s,%d : Can't find info node.", __FUNCTION__, __LINE__));

        *doc = record_doc;
        return VOS_OK;
    } else {
        debug_log(DLOG_ERROR, "%s,%d : Load xml failed.", __FUNCTION__, __LINE__);
    }

__error:
    if (record_doc) {
        xmlFreeDoc(record_doc);
    }
    debug_log(DLOG_ERROR, "%s,%d : Open xml failed.", __FUNCTION__, __LINE__);
    return VOS_ERR;
}

LOCAL gint32 get_workrecord_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return get_universal_provider_odata_info(i_paras, o_message_jso, o_result_jso, DIAGNOSTIC_WORKRECORD_ODATAID);
}

LOCAL gint32 get_workrecord_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return get_universal_provider_odata_info(i_paras, o_message_jso, o_result_jso, DIAGNOSTIC_WORKRECORD_CONTEXT);
}


LOCAL gint32 get_records(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    xmlDocPtr record_doc = NULL;
    xmlNodePtr root_node = NULL;
    xmlNodePtr id_node = NULL;
    xmlNodePtr num_node = NULL;
    xmlNodePtr info_node = NULL;
    xmlNodePtr childptr;
    xmlNodePtr leafptr = NULL;
    xmlChar *value = NULL;
    PROVIDER_PARAS_S *tmp_paras = NULL;
    json_object *tmp_obj = NULL;
    tmp_paras = (PROVIDER_PARAS_S *)i_paras;

    if (NULL == o_result_jso || o_message_jso == NULL || VOS_OK != provider_paras_check(tmp_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(tmp_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_record_nodes(&record_doc, &root_node, &id_node, &num_node, &info_node);
    return_val_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_array();

    
    childptr = info_node->children;
    while (childptr != NULL) {
        
        leafptr = childptr->children;
        tmp_obj = json_object_new_object();
        while (leafptr != NULL) {
            value = xmlNodeGetContent(leafptr);
            if (value != NULL) {
                if (!xmlStrcmp(leafptr->name, (const xmlChar *)RECORD_INFO_ID)) {
                    json_object_object_add(tmp_obj, PROPERTY_LOG_ID, json_object_new_string((const gchar *)value));
                } else if (!xmlStrcmp(leafptr->name, (const xmlChar *)RECORD_INFO_USER)) {
                    json_object_object_add(tmp_obj, PROPERTY_LOG_USER, json_object_new_string((const gchar *)value));
                } else if (!xmlStrcmp(leafptr->name, (const xmlChar *)RECORD_INFO_IP)) {
                    json_object_object_add(tmp_obj, PROPERTY_LOG_ADDR, json_object_new_string((const gchar *)value));
                } else if (!xmlStrcmp(leafptr->name, (const xmlChar *)RECORD_INFO_TIME)) {
                    json_object_object_add(tmp_obj, PROPERTY_LOG_TIME, json_object_new_string((const gchar *)value));
                } else if (!xmlStrcmp(leafptr->name, (const xmlChar *)RECORD_INFO_DETAIL)) {
                    json_object_object_add(tmp_obj, PROPERTY_LOG_DETAILS, json_object_new_string((const gchar *)value));
                }

                xmlFree(value);
                value = NULL;
            }

            leafptr = leafptr->next;
        }
        childptr = childptr->next;
        if (json_object_object_length(tmp_obj) > 0) {
            json_object_array_add(*o_result_jso, tmp_obj);
        } else {
            json_object_put(tmp_obj);
        }
    }

    xmlFreeDoc(record_doc);
    return HTTP_OK;
}


LOCAL gint32 get_appropriate_id(xmlNodePtr info_node, guint32 *next_id)
{
    gint32 ret = 0;
    guint32 i;
    guint32 array_index = 0;
    guint32 record_id[MAX_WORKRECORD_NUM + 1] = {0};
    xmlNodePtr childptr;
    xmlNodePtr leafptr = NULL;
    xmlChar *value = NULL;

    return_val_do_info_if_fail(NULL != info_node && NULL != next_id, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    childptr = info_node->children;
    
    while (childptr) {
        
        for (leafptr = childptr->children; leafptr; leafptr = leafptr->next) {
            if (!xmlStrcmp(leafptr->name, (const xmlChar *)RECORD_INFO_ID)) {
                
                value = xmlNodeGetContent(leafptr);
                ret = vos_str2int((const gchar *)value, 10, &array_index, NUM_TPYE_UINT32);
                do_val_if_expr(NULL != value, xmlFree(value); value = NULL);
                return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
                    debug_log(DLOG_ERROR, "%s, %d: get record id error.", __FUNCTION__, __LINE__));
                if (array_index <= MAX_WORKRECORD_NUM) {
                    record_id[array_index] = 1;
                }
                break;
            }
        }
        childptr = childptr->next;
    }

    
    for (i = 1; i <= MAX_WORKRECORD_NUM; i++) {
        if (0 == record_id[i]) {
            *next_id = i;
            return VOS_OK;
        }
    }
    return VOS_ERR;
}


LOCAL gint32 get_next_available_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    xmlDocPtr record_doc = NULL;
    xmlNodePtr root_node = NULL;
    xmlNodePtr id_node = NULL;
    xmlNodePtr num_node = NULL;
    xmlNodePtr info_node = NULL;
    xmlChar *value = NULL;
    guint32 record_num = 0;
    guint32 next_available_id = 0;
    gchar next_id_string[64] = {0};

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_record_nodes(&record_doc, &root_node, &id_node, &num_node, &info_node);
    return_val_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR);
    
    value = xmlNodeGetContent(num_node);
    ret = vos_str2int((const gchar *)value, 10, &record_num, NUM_TPYE_UINT32);
    do_val_if_expr(value != NULL, xmlFree(value); value = NULL);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: get current work number fail.", __FUNCTION__, __LINE__);
        xmlFreeDoc(record_doc);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return_val_do_info_if_fail(record_num < MAX_WORKRECORD_NUM, HTTP_OK, xmlFreeDoc(record_doc); *o_result_jso = NULL);

    ret = get_appropriate_id(info_node, &next_available_id);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, xmlFreeDoc(record_doc);
        debug_log(DLOG_ERROR, "%s,%d:get next valid id fail.", __FUNCTION__, __LINE__));

    (void)snprintf_s(next_id_string, sizeof(next_id_string), sizeof(next_id_string) - 1, "%u", next_available_id);
    *o_result_jso = json_object_new_string(next_id_string);

    xmlFreeDoc(record_doc);
    return HTTP_OK;
}

LOCAL gint32 get_workrecord_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    json_object *action_jso = NULL;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar rsc_uri[MAX_URI_LENGTH] = {0};
    gint32 ret;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = snprintf_s(rsc_uri, sizeof(rsc_uri), sizeof(rsc_uri) - 1, DIAGNOSTIC_WORKRECORD_ODATAID, slot);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));

    action_jso = json_object_new_object();
    if (NULL == action_jso) {
        debug_log(DLOG_ERROR, "alloc new object failed");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    rf_add_action_prop(action_jso, rsc_uri, RFACTION_ADD_RECORD);
    rf_add_action_prop(action_jso, rsc_uri, RFACTION_DELETE_RECORD);
    rf_add_action_prop(action_jso, rsc_uri, RFACTION_MODIFY_RECORD);

    *o_result_jso = action_jso;
    return HTTP_OK;
}


LOCAL gint32 act_add_record(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    json_bool is_ok;
    const gchar *details = NULL;
    gchar max_len_str[16] = {0};
    _cleanup_xml_doc_ xmlDocPtr record_doc = NULL;
    xmlNodePtr root_node = NULL;
    xmlNodePtr id_node = NULL;
    xmlNodePtr num_node = NULL;
    xmlNodePtr info_node = NULL;
    xmlNodePtr nodeptr = NULL;
    xmlNodePtr childptr = NULL;
    xmlNodePtr valueptr = NULL;
    gchar buffer[64] = {0};
    guint32 record_id = 0;
    guint32 next_id = 0;
    guint32 record_num = 0;
    GDateTime *dt = NULL;
    xmlChar *value = NULL;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso) &&
        (NULL != i_paras->val_jso),
        HTTP_BAD_REQUEST, debug_log(DLOG_ERROR, "input param error."));

    
    if (0 == (i_paras->user_role_privilege & USERROLE_READONLY)) {
        workrecord_log(i_paras, STR_ADD, STR_FAILED, -1);
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFACTION_ADD_RECORD, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    is_ok = get_element_str(i_paras->val_jso, PROPERTY_LOG_DETAILS, &details);
    return_val_do_info_if_expr(!is_ok, HTTP_BAD_REQUEST, workrecord_log(i_paras, STR_ADD, STR_FAILED, -1);
        (void)create_message_info(MSGID_ACT_PARA_MISSING, PROPERTY_LOG_DETAILS, o_message_jso, RFACTION_ADD_RECORD,
        PROPERTY_LOG_DETAILS));

    
    (void)snprintf_s(max_len_str, sizeof(max_len_str), sizeof(max_len_str) - 1, "%d", MAX_CONFIG_LEN - 1);
    return_val_do_info_if_expr(strlen(details) >= MAX_CONFIG_LEN, HTTP_BAD_REQUEST,
        workrecord_log(i_paras, STR_ADD, STR_FAILED, -1);
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, NULL, o_message_jso, "", PROPERTY_LOG_DETAILS,
        max_len_str));
    return_val_do_info_if_expr(strlen(details) == 0, HTTP_BAD_REQUEST, workrecord_log(i_paras, STR_ADD, STR_FAILED, -1);
        (void)create_message_info(MSGID_PROP_VAL_NOT_EMPTY, NULL, o_message_jso));

    
    ret = get_record_nodes(&record_doc, &root_node, &id_node, &num_node, &info_node);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        workrecord_log(i_paras, STR_ADD, STR_FAILED, -1));

    value = xmlNodeGetContent(num_node);
    ret = vos_str2int((const gchar *)value, 10, &record_num, NUM_TPYE_UINT32);
    do_val_if_expr(value != NULL, xmlFree(value); value = NULL);
    if (ret != VOS_OK) {
        workrecord_log(i_paras, STR_ADD, STR_FAILED, -1);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (record_num >= MAX_WORKRECORD_NUM) {
        
        (void)create_message_info(MSGID_RECORDS_EXC_MAXNUM, NULL, o_message_jso);
        workrecord_log(i_paras, STR_ADD, STR_FAILED, -1);
        return HTTP_BAD_REQUEST;
    }

    
    value = xmlNodeGetContent(id_node);
    ret = vos_str2int((const gchar *)value, 10, &record_id, NUM_TPYE_UINT32);
    do_val_if_expr(value != NULL, xmlFree(value); value = NULL);
    if (ret != VOS_OK) {
        workrecord_log(i_paras, STR_ADD, STR_FAILED, -1);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    ret = get_appropriate_id(info_node, &next_id);
    if (VOS_OK != ret) {
        workrecord_log(i_paras, STR_ADD, STR_FAILED, -1);
        debug_log(DLOG_ERROR, "%s,%d:get next valid id fail.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    (void)snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "%u", next_id);
    nodeptr = xmlNewNode(NULL, BAD_CAST RECORD_INFO);
    childptr = xmlNewNode(NULL, BAD_CAST RECORD_INFO_ID);
    valueptr = xmlNewText(BAD_CAST buffer);
    xmlAddChild(childptr, valueptr);
    xmlAddChild(nodeptr, childptr);
    xmlNodeSetContent(id_node, (const xmlChar *)buffer);

    
    childptr = xmlNewNode(NULL, BAD_CAST RECORD_INFO_USER);
    valueptr = xmlNewText(BAD_CAST i_paras->user_name);
    xmlAddChild(childptr, valueptr);
    xmlAddChild(nodeptr, childptr);

    
    childptr = xmlNewNode(NULL, BAD_CAST RECORD_INFO_IP);
    valueptr = xmlNewText(BAD_CAST i_paras->client);
    xmlAddChild(childptr, valueptr);
    xmlAddChild(nodeptr, childptr);

    
    dt = g_date_time_new_from_unix_local((gint64)vos_get_cur_time_stamp());
    (void)snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "%04d/%02d/%02d %02d:%02d:%02d",
        g_date_time_get_year(dt), g_date_time_get_month(dt), g_date_time_get_day_of_month(dt), g_date_time_get_hour(dt),
        g_date_time_get_minute(dt), g_date_time_get_second(dt));
    g_date_time_unref(dt);
    childptr = xmlNewNode(NULL, BAD_CAST RECORD_INFO_TIME);
    valueptr = xmlNewText(BAD_CAST buffer);
    xmlAddChild(childptr, valueptr);
    xmlAddChild(nodeptr, childptr);

    
    childptr = xmlNewNode(NULL, BAD_CAST RECORD_INFO_DETAIL);
    valueptr = xmlNewText((const xmlChar *)details);
    xmlAddChild(childptr, valueptr);
    xmlAddChild(nodeptr, childptr);

    
    snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "%d", record_id + 1);
    xmlNodeSetContent(id_node, (const xmlChar *)buffer);

    
    xmlAddChild(info_node, nodeptr);
    (void)snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "%d", record_num + 1);
    xmlNodeSetContent(num_node, (const xmlChar *)buffer);

    xmlKeepBlanksDefault(0);
    xmlIndentTreeOutput = 1;
    (void)xmlSaveFormatFileEnc(RECORD_FILE, record_doc, "UTF-8", 1);

    workrecord_log(i_paras, STR_ADD, STR_SUCCESS, (gint32)next_id);
    return HTTP_OK;
}


LOCAL gint32 act_delete_record(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guint8 find = 0;
    json_bool is_ok;
    const gchar *id = NULL;
    xmlDocPtr record_doc = NULL;
    xmlNodePtr root_node = NULL;
    xmlNodePtr id_node = NULL;
    xmlNodePtr num_node = NULL;
    xmlNodePtr info_node = NULL;
    xmlNodePtr childptr;
    gchar buffer[64] = {0};
    guint32 record_id = 0;
    gint32 cur_record_id = -1;
    xmlChar *value = NULL;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso) &&
        (NULL != i_paras->val_jso),
        HTTP_BAD_REQUEST, debug_log(DLOG_ERROR, "input param error."));

    
    if (0 == (i_paras->user_role_privilege & USERROLE_READONLY)) {
        workrecord_log(i_paras, STR_DELETE, STR_FAILED, -1);
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFACTION_DELETE_RECORD, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    is_ok = get_element_str(i_paras->val_jso, PROPERTY_LOG_ID, &id);
    return_val_do_info_if_expr(!is_ok, HTTP_BAD_REQUEST, workrecord_log(i_paras, STR_DELETE, STR_FAILED, -1);
        (void)create_message_info(MSGID_ACT_PARA_MISSING, PROPERTY_LOG_ID, o_message_jso, RFACTION_DELETE_RECORD,
        PROPERTY_LOG_ID));
    (void)vos_str2int(id, 10, &cur_record_id, NUM_TPYE_INT32);

    
    ret = get_record_nodes(&record_doc, &root_node, &id_node, &num_node, &info_node);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        workrecord_log(i_paras, STR_DELETE, STR_FAILED, cur_record_id));

    childptr = info_node->children;
    while (childptr) {
        if (childptr->children && !xmlStrcmp(childptr->children->name, (const xmlChar *)RECORD_INFO_ID)) {
            value = xmlNodeGetContent(childptr->children);
            ret = xmlStrcmp(value, (const xmlChar *)id);
            do_val_if_expr(value != NULL, xmlFree(value); value = NULL);
            if (ret == 0) {
                
                xmlUnlinkNode(childptr);
                xmlFreeNode(childptr);

                
                value = xmlNodeGetContent(num_node);
                ret = vos_str2int((const gchar *)value, 10, &record_id, NUM_TPYE_UINT32);
                do_val_if_expr(value != NULL, xmlFree(value); value = NULL);
                if (ret != VOS_OK) {
                    workrecord_log(i_paras, STR_DELETE, STR_FAILED, cur_record_id);
                    xmlFreeDoc(record_doc);
                    debug_log(DLOG_ERROR, "%s : vos_str2int faild(ret = %d).", __FUNCTION__, ret);
                    return HTTP_INTERNAL_SERVER_ERROR;
                }
                (void)snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "%d", record_id - 1);
                xmlNodeSetContent(num_node, (const xmlChar *)buffer);
                find = 1;
                break;
            }
        }
        childptr = childptr->next;
    }

    xmlKeepBlanksDefault(0);
    xmlIndentTreeOutput = 1;
    (void)xmlSaveFormatFileEnc(RECORD_FILE, record_doc, "UTF-8", 1);

    xmlFreeDoc(record_doc);

    return_val_do_info_if_expr(find == 0, HTTP_BAD_REQUEST,
        workrecord_log(i_paras, STR_DELETE, STR_FAILED, cur_record_id);
        (void)create_message_info(MSGID_INVALID_RECORD_ID, NULL, o_message_jso));

    workrecord_log(i_paras, STR_DELETE, STR_SUCCESS, cur_record_id);
    return HTTP_OK;
}


LOCAL gint32 act_modify_record(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 find = 0;
    json_bool is_ok;
    const gchar *id = NULL;
    const gchar *content = NULL;
    xmlDocPtr record_doc = NULL;
    xmlNodePtr root_node = NULL;
    xmlNodePtr id_node = NULL;
    xmlNodePtr num_node = NULL;
    xmlNodePtr info_node = NULL;
    xmlNodePtr childptr;
    xmlNodePtr leafptr = NULL;
    xmlNodePtr newnodeptr = NULL;
    xmlNodePtr valueptr = NULL;
    gchar max_len_str[16] = {0};
    xmlChar *value = NULL;
    gint32 cur_record_id = -1;
    GDateTime *dt = NULL;
    gchar buffer[64] = {0};

    if (NULL == o_result_jso || o_message_jso == NULL || NULL == i_paras) {
        workrecord_log(i_paras, STR_MODIFY, STR_FAILED, -1);
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (0 == (i_paras->user_role_privilege & USERROLE_READONLY)) {
        workrecord_log(i_paras, STR_MODIFY, STR_FAILED, -1);
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFACTION_MODIFY_RECORD, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    is_ok = get_element_str(i_paras->val_jso, PROPERTY_LOG_ID, &id);
    return_val_do_info_if_expr(!is_ok, HTTP_BAD_REQUEST, workrecord_log(i_paras, STR_MODIFY, STR_FAILED, -1);
        (void)create_message_info(MSGID_ACT_PARA_MISSING, PROPERTY_LOG_ID, o_message_jso, RFACTION_MODIFY_RECORD,
        PROPERTY_LOG_ID));
    (void)vos_str2int(id, 10, &cur_record_id, NUM_TPYE_INT32);

    is_ok = get_element_str(i_paras->val_jso, PROPERTY_LOG_DETAILS, &content);
    return_val_do_info_if_expr(!is_ok, HTTP_BAD_REQUEST, workrecord_log(i_paras, STR_MODIFY, STR_FAILED, cur_record_id);
        (void)create_message_info(MSGID_ACT_PARA_MISSING, PROPERTY_LOG_DETAILS, o_message_jso, RFACTION_MODIFY_RECORD,
        PROPERTY_LOG_DETAILS));

    (void)snprintf_s(max_len_str, sizeof(max_len_str), sizeof(max_len_str) - 1, "%d", MAX_CONFIG_LEN - 1);
    return_val_do_info_if_expr(strlen(content) >= MAX_CONFIG_LEN, HTTP_BAD_REQUEST,
        workrecord_log(i_paras, STR_MODIFY, STR_FAILED, cur_record_id);
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, NULL, o_message_jso, "", PROPERTY_LOG_DETAILS,
        max_len_str));
    return_val_do_info_if_expr(strlen(content) == 0, HTTP_BAD_REQUEST,
        workrecord_log(i_paras, STR_MODIFY, STR_FAILED, cur_record_id);
        (void)create_message_info(MSGID_PROP_VAL_NOT_EMPTY, NULL, o_message_jso));

    
    ret = get_record_nodes(&record_doc, &root_node, &id_node, &num_node, &info_node);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        workrecord_log(i_paras, STR_MODIFY, STR_FAILED, cur_record_id););

    
    childptr = info_node->children;
    while (childptr != NULL) {
        
        leafptr = childptr->children;
        if (leafptr && !xmlStrcmp(leafptr->name, (const xmlChar *)RECORD_INFO_ID)) {
            value = xmlNodeGetContent(leafptr);
            ret = xmlStrcmp(value, (const xmlChar *)id);
            do_val_if_expr(value != NULL, xmlFree(value); value = NULL);
            if (ret == 0) {
                
                
                newnodeptr = xmlNewNode(NULL, BAD_CAST RECORD_INFO_DETAIL);
                valueptr = xmlNewText((const xmlChar *)content);
                xmlAddChild(newnodeptr, valueptr);
                xmlAddChild(childptr, newnodeptr);
                

                
                
                dt = g_date_time_new_from_unix_local((gint64)vos_get_cur_time_stamp());
                (void)snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "%04d/%02d/%02d %02d:%02d:%02d",
                    g_date_time_get_year(dt), g_date_time_get_month(dt), g_date_time_get_day_of_month(dt),
                    g_date_time_get_hour(dt), g_date_time_get_minute(dt), g_date_time_get_second(dt));

                g_date_time_unref(dt);
                newnodeptr = xmlNewNode(NULL, BAD_CAST RECORD_INFO_TIME);
                valueptr = xmlNewText(BAD_CAST buffer);
                xmlAddChild(newnodeptr, valueptr);
                xmlAddChild(childptr, newnodeptr);

                
                newnodeptr = xmlNewNode(NULL, BAD_CAST RECORD_INFO_USER);
                valueptr = xmlNewText(BAD_CAST i_paras->user_name);
                xmlAddChild(newnodeptr, valueptr);
                xmlAddChild(childptr, newnodeptr);

                
                newnodeptr = xmlNewNode(NULL, BAD_CAST RECORD_INFO_IP);
                valueptr = xmlNewText(BAD_CAST i_paras->client);
                xmlAddChild(newnodeptr, valueptr);
                xmlAddChild(childptr, newnodeptr);
                

                find = 1;
                break;
            }
        }
        childptr = childptr->next;
    }

    xmlKeepBlanksDefault(0);
    xmlIndentTreeOutput = 1;
    (void)xmlSaveFormatFileEnc(RECORD_FILE, record_doc, "UTF-8", 1);

    xmlFreeDoc(record_doc);

    return_val_do_info_if_expr(find == 0, HTTP_BAD_REQUEST,
        workrecord_log(i_paras, STR_MODIFY, STR_FAILED, cur_record_id);
        (void)create_message_info(MSGID_INVALID_RECORD_ID, NULL, o_message_jso));

    workrecord_log(i_paras, STR_MODIFY, STR_SUCCESS, cur_record_id);
    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_workrecord_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_workrecord_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_workrecord_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {DIAGNOSTIC_NEXT_AVAILABLE_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_next_available_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {DIAGNOSTIC_RECORDS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_records, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ACTION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_workrecord_actions, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFACTION_ADD_RECORD, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, act_add_record, ETAG_FLAG_ENABLE},
    {RFACTION_DELETE_RECORD, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, act_delete_record, ETAG_FLAG_ENABLE},
    {RFACTION_MODIFY_RECORD, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, act_modify_record, ETAG_FLAG_ENABLE},
};


gint32 workrecord_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean ret;
    guchar board_type = 0;
    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);

    ret = redfish_get_board_type(&board_type);
    return_val_if_expr((VOS_OK != ret) || (BOARD_SWITCH == board_type), HTTP_NOT_FOUND);

    ret = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_do_info_if_expr(TRUE != ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s,%d:URI check failed. ", __FUNCTION__, __LINE__));

    *prop_provider = g_workrecord_provider;
    *count = sizeof(g_workrecord_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
