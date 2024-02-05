
#include <arpa/inet.h>
#include "redfish_provider.h"
#include "redfish_util.h"
#include "redfish_http.h"
#include "pme_app/smm/smm_public.h"
#include "pme_app/kmc/kmc.h"
#include "redfish_provider_system.h"
#include "redfish_route.h"

#define RACK_NODE_INNER_NET_MAX_NUM 5

typedef struct _tag_rsc_pattern_node_data {
    
    gchar node_name[MAX_MEM_ID_LEN];

    
    guint32 provider_index;

    PROVIDER_URI_PATTERN_FLAG pattern_flag;
} RSC_URI_PATTERN_NODE_DATA_S;

#define RF_URI_PATTERN_PREFIX "^/"
#define RF_URI_PATTERN_SUFFIX_CHAR '$'




LOCAL REDFISH_OPR_INFO_S g_opr_fn_info[] = {
    {URI_SESSION_COLLECTION, SYS_LOCKDOWN_ALLOW, create_session, delete_session, generate_session_response_headers, generate_del_session_response_headers},
    {URI_ACCOUNT_COLLECTION, SYS_LOCKDOWN_FORBID, create_account, delete_account, generate_account_response_headers, generate_del_account_response_headers},
    {URI_VOLUME_COLLECTION, SYS_LOCKDOWN_FORBID, create_volume, delete_volume, generate_create_rsc_response_headers_task, generate_del_response_headers_task},
    {URI_EVT_SUBSCRIPTION_COLLECTION, SYS_LOCKDOWN_FORBID, create_subscription_entry, delete_subscription, generate_rsc_create_response_headers, generate_del_common_response_headers},

    {URI_SP_RAID_COLLECTION, SYS_LOCKDOWN_FORBID, create_sp_raid_id, NULL, generate_rsc_create_response_headers, NULL},
    {URI_SP_OSINSTALL_COLLECTION, SYS_LOCKDOWN_FORBID, create_sp_osinstall_id, NULL, generate_rsc_create_response_headers, NULL},
    {URI_SP_DIAGNOSE_COLLECTION, SYS_LOCKDOWN_FORBID, create_sp_diagnose_conf, NULL, generate_rsc_create_response_headers, NULL},
    {URI_SP_DRIVEERASE_COLLECTION, SYS_LOCKDOWN_FORBID, create_sp_drive_erase_conf, NULL, generate_rsc_create_response_headers, NULL},
    {URI_NETWORK_BONDING_COLLECTION, SYS_LOCKDOWN_FORBID, create_network_bonding, delete_network_bonding, generate_create_rsc_response_headers_task, generate_del_response_headers_task},
    {URI_VLAN_COLLECTION, SYS_LOCKDOWN_FORBID, create_vlan, delete_vlan, generate_create_rsc_response_headers_task, generate_del_response_headers_task},

    {NULL, SYS_LOCKDOWN_NULL, NULL, NULL, NULL, NULL}
};


LOCAL GNode *g_resource_provider_node_tree = NULL;
LOCAL const gchar *CHASSIS_PATTERN = "/redfish/v1/Chassis/${id}";
LOCAL const gchar *URI_CHASSIS = "/redfish/v1/Chassis";
LOCAL const gchar *URI_MANAGERS = "/redfish/v1/Managers";


LOCAL GNode *g_hmm_resource_provider_node_tree = NULL;

LOCAL GNode *__find_child_rsc_provider_node_by_name(GNode *parent, const gchar *node_name);

LOCAL GNode *__match_child_rsc_provider_node(GNode *parent, const gchar *segment_path, guint32 remain_segments);


gint32 proxy_copy_file(guchar from_webui_flag, const gchar *user_name, const gchar *client, const gchar *src_file,
    const gchar *dest_file, guint32 user_id, guint32 group_id, guint32 delete_flag)
{
    OBJ_HANDLE privilege_agent_handle = 0;
    GSList *input_list = NULL;

    return_val_do_info_if_expr(NULL == src_file || NULL == dest_file || NULL == user_name || NULL == client, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));

    (void)dfl_get_object_handle(OBJ_NAME_PRIVILEGEAGENT, &privilege_agent_handle);
    input_list = g_slist_append(input_list, g_variant_new_string(src_file));
    input_list = g_slist_append(input_list, g_variant_new_string(dest_file));
    input_list = g_slist_append(input_list, g_variant_new_uint32(user_id));
    input_list = g_slist_append(input_list, g_variant_new_uint32(group_id));
    input_list = g_slist_append(input_list, g_variant_new_uint32(delete_flag));

    gint32 ret_val = uip_call_class_method_redfish(from_webui_flag, user_name, client, privilege_agent_handle,
        CLASS_PRIVILEGEAGENT, METHOD_PRIVILEGEAGENT_COPYFILE, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);

    return ret_val;
}


gint32 proxy_modify_file_mode(guchar from_webui_flag, const gchar *user_name, const gchar *client,
    const gchar *dest_file, guint32 user_id, guint32 group_id)
{
    OBJ_HANDLE privilege_agent_handle = 0;
    GSList *input_list = NULL;

    return_val_do_info_if_expr(NULL == dest_file || NULL == user_name || NULL == client, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));

    if (FALSE == vos_get_file_accessible(dest_file)) {
        debug_log(DLOG_ERROR, "%s: file %s not exist", __FUNCTION__, dest_file);
        return VOS_ERR;
    }

    (void)dfl_get_object_handle(OBJ_NAME_PRIVILEGEAGENT, &privilege_agent_handle);
    input_list = g_slist_append(input_list, g_variant_new_string(dest_file));
    input_list = g_slist_append(input_list, g_variant_new_uint32(user_id));
    input_list = g_slist_append(input_list, g_variant_new_uint32(group_id));

    gint32 ret_val = uip_call_class_method_redfish(from_webui_flag, user_name, client, privilege_agent_handle,
        CLASS_PRIVILEGEAGENT, METHDO_PRIVILEGEAGENT_MODIFY_FILE_OWN, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);

    return ret_val;
}


void proxy_revert_file_mode_on_exit(const gchar *dest_file, guint32 user_id, guint32 group_id)
{
    GSList *input_list = NULL;

    if (dest_file == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__);
        return;
    }

    if (vos_get_file_accessible(dest_file) == FALSE) {
        debug_log(DLOG_ERROR, "%s: file %s not exist", __FUNCTION__, dest_file);
        return;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(dest_file));
    input_list = g_slist_append(input_list, g_variant_new_uint32(user_id));
    input_list = g_slist_append(input_list, g_variant_new_uint32(group_id));
    gint32 ret_val = uip_call_class_method_redfish(0, "N/A", "N/A", 0, CLASS_PRIVILEGEAGENT,
        METHDO_PRIVILEGEAGENT_REVERT_FILE_OWN, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret_val != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: %s change owner fail", __FUNCTION__, dest_file);
    }

    return;
}


gint32 proxy_delete_file(guchar from_webui_flag, const gchar *user_name, const gchar *client, const gchar *dest_file)
{
    gint32 ret_val;
    OBJ_HANDLE privilege_agent_handle = 0;
    GSList *input_list = NULL;

    return_val_do_info_if_expr(dest_file == NULL, VOS_ERR, debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));

    if (vos_get_file_accessible(dest_file) == FALSE) {
        return VOS_OK;
    }

    (void)dfl_get_object_handle(OBJ_NAME_PRIVILEGEAGENT, &privilege_agent_handle);
    input_list = g_slist_append(input_list, g_variant_new_string(dest_file));
    if (user_name == NULL || client == NULL) {
        ret_val =
            dfl_call_class_method(privilege_agent_handle, METHOD_PRIVILEGEAGENT_DELETE_FILE, NULL, input_list, NULL);
    } else {
        ret_val = uip_call_class_method_redfish(from_webui_flag, user_name, client, privilege_agent_handle,
            CLASS_PRIVILEGEAGENT, METHOD_PRIVILEGEAGENT_DELETE_FILE, AUTH_DISABLE, 0, input_list, NULL);
    }

    uip_free_gvariant_list(input_list);

    return ret_val;
}


gint32 proxy_create_file(const gchar *dest_file, const gchar *create_flag, guint32 user_id, guint32 group_id,
    guint32 file_mode)
{
    OBJ_HANDLE privilege_agent_handle = 0;
    GSList *input_list = NULL;

    return_val_do_info_if_expr(NULL == dest_file || NULL == create_flag, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));

    // 文件已存在，直接返回
    if (TRUE == vos_get_file_accessible(dest_file)) {
        debug_log(DLOG_DEBUG, "%s: file %s exist", __FUNCTION__, dest_file);
        return VOS_OK;
    }

    (void)dfl_get_object_handle(OBJ_NAME_PRIVILEGEAGENT, &privilege_agent_handle);
    input_list = g_slist_append(input_list, g_variant_new_string(dest_file));
    input_list = g_slist_append(input_list, g_variant_new_string(create_flag));
    input_list = g_slist_append(input_list, g_variant_new_uint32(file_mode));
    input_list = g_slist_append(input_list, g_variant_new_uint32(user_id));
    input_list = g_slist_append(input_list, g_variant_new_uint32(group_id));

    gint32 ret_val = dfl_call_class_method(privilege_agent_handle, METHOD_PRIVILEGEAGENT_CREATE_FILE, NULL,
        input_list, NULL);
    uip_free_gvariant_list(input_list);

    return ret_val;
}


LOCAL gint32 __get_rsc_uri_pattern_segments(gchar *rsc_uri_pattern, gchar **pattern_segments, guint32 *segment_length)
{
    guint32 pattern_length;
    gchar *pattern_prefix_str = NULL;
    gchar *pattern_uri_char_iter = NULL;
    guint32 segment_tmp_length = 0;

    return_val_do_info_if_expr((NULL == rsc_uri_pattern) || (NULL == pattern_segments) || (NULL == segment_length),
        VOS_ERR, debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    pattern_length = strlen(rsc_uri_pattern);
    pattern_uri_char_iter = rsc_uri_pattern;

    return_val_do_info_if_expr((0 == pattern_length), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: invalid rsc uri pattern", __FUNCTION__));

    do_val_if_expr(RF_URI_PATTERN_SUFFIX_CHAR == rsc_uri_pattern[pattern_length - 1],
        (rsc_uri_pattern[pattern_length - 1] = '\0'));

    // 样例： ^/redfish/v1/systems/[a-zA-Z0-9]+/ethernetinterfaces/[^/]+$

    pattern_prefix_str = g_strstr_len(pattern_uri_char_iter, pattern_length, RF_URI_PATTERN_PREFIX);
    do_val_if_expr(NULL != pattern_prefix_str,
        (pattern_uri_char_iter = pattern_prefix_str + strlen(RF_URI_PATTERN_PREFIX)));

    pattern_segments[segment_tmp_length++] = pattern_uri_char_iter;

    for (; '\0' != *pattern_uri_char_iter; pattern_uri_char_iter++) {
        
        if ((SLASH_CHAR == *pattern_uri_char_iter) && ('^' != *(pattern_uri_char_iter - 1))) {
            *pattern_uri_char_iter = '\0';
            pattern_uri_char_iter++;

            pattern_segments[segment_tmp_length++] = pattern_uri_char_iter;
        }
    }

    *segment_length = segment_tmp_length;

    return VOS_OK;
}


LOCAL RSC_URI_PATTERN_NODE_DATA_S *_new_empty_rsc_pattern_node_data(const gchar *node_name)
{
    RSC_URI_PATTERN_NODE_DATA_S *new_data = NULL;

    return_val_do_info_if_fail((NULL != node_name), NULL,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    new_data = (RSC_URI_PATTERN_NODE_DATA_S *)g_malloc0(sizeof(RSC_URI_PATTERN_NODE_DATA_S));
    return_val_do_info_if_fail(NULL != new_data, NULL, debug_log(DLOG_ERROR, "alloc new resource monitor data failed"));

    new_data->provider_index = G_MAXUINT32;

    errno_t str_ret = strncpy_s(new_data->node_name, MAX_MEM_ID_LEN, node_name, strlen(node_name));
    if (str_ret != EOK) {
        debug_log(DLOG_ERROR, "copy string %s failed", node_name);
        g_free(new_data);
    }

    return new_data;
}


LOCAL void _compare_with_node_name(GNode *node, void *data)
{
    RSC_URI_PATTERN_NODE_DATA_S *node_data = NULL;
    rdf_node_info_s *data_found = (rdf_node_info_s *)data;

    return_do_info_if_fail((NULL != node) && (NULL != data),
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    node_data = (RSC_URI_PATTERN_NODE_DATA_S *)node->data;

    if (0 == g_ascii_strcasecmp(data_found->data, node_data->node_name)) {
        data_found->found_node = node;
    }

    return;
}


LOCAL GNode *__find_child_rsc_provider_node_by_name(GNode *parent, const gchar *node_name)
{
    GNode *node_found = NULL;
    rdf_node_info_s node_info;

    return_val_do_info_if_expr((NULL == parent) || (NULL == node_name), NULL,
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    (void)memset_s(&node_info, sizeof(rdf_node_info_s), 0, sizeof(rdf_node_info_s));

    node_info.found_node = NULL;
    node_info.data = node_name;

    g_node_children_foreach(parent, G_TRAVERSE_ALL, (GNodeForeachFunc)_compare_with_node_name, (gpointer)&node_info);
    node_found = node_info.found_node;

    return node_found;
}


LOCAL gboolean _regex_match_with_data(GNode *node, void *data)
{
    RSC_URI_PATTERN_NODE_DATA_S *node_data = NULL;
    rdf_node_info_s *data_found = (rdf_node_info_s *)data;
    gchar               segment_regex[MAX_URI_LENGTH] = {0};
    gint32 ret;

    return_val_do_info_if_fail((NULL != node) && (NULL != data), FALSE,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    node_data = (RSC_URI_PATTERN_NODE_DATA_S *)node->data;

    ret = snprintf_s(segment_regex, sizeof(segment_regex), sizeof(segment_regex) - 1, "(?i)^%s$", node_data->node_name);
    return_val_do_info_if_expr(ret <= 0, FALSE,
        debug_log(DLOG_ERROR, "%s failed: format string with [%s] failed, ret is %d", __FUNCTION__,
        node_data->node_name, ret));

    if (TRUE == g_regex_match_simple(segment_regex, data_found->data, (GRegexCompileFlags)0, (GRegexMatchFlags)0)) {
        data_found->found_node = node;

        return TRUE;
    }

    return FALSE;
}


LOCAL GNode *__match_child_rsc_provider_node(GNode *parent, const gchar *segment_path, guint32 remain_segments)
{
    rdf_node_info_s node_info;
    GNode *node_found = NULL;
    GNode *iter_node = NULL;
    gboolean iter_result;
    guint32 iter_times = 0;

    return_val_do_info_if_expr((NULL == parent) || (NULL == segment_path), NULL,
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    (void)memset_s(&node_info, sizeof(rdf_node_info_s), 0, sizeof(rdf_node_info_s));

    node_info.data = segment_path;
    node_info.found_node = NULL;

    iter_node = parent->children;
    while (iter_node) {
        GNode *current;

        current = iter_node;
        iter_node = current->next;

        
        if ((NULL == current->children) && (0 != remain_segments)) {
            continue;
        }

        iter_times++;

        iter_result = _regex_match_with_data(current, (void *)&node_info);
        break_do_info_if_expr(TRUE == iter_result,
            debug_log(DLOG_DEBUG, "%s: regex match successfully, iter times is %d", __FUNCTION__, iter_times));
    }

    node_found = node_info.found_node;

    return node_found;
}


LOCAL gint32 __add_rsc_provider_node(GNode **root_node, const RESOURCE_PROVIDER_S *rsc_provider,
    guint32 rsc_provider_index)
{
#define REDFISH_SERVICE_PATTERN "redfish"
#define REDFISH_SERVICE_PATTERN_SEGMENT_NUM 1

    gchar               rsc_uri_tmp[MAX_URI_LENGTH] = {0};
    errno_t sec_ret;
    gchar*              pattern_segments[MAX_URI_SEGMENT_NUM] = {0};
    guint32 segment_num = 0;
    guint32 i;
    gint32 ret;
    GNode *parent = NULL;
    GNode *leaf = NULL;
    RSC_URI_PATTERN_NODE_DATA_S *node_data = NULL;

    return_val_do_info_if_expr((NULL == root_node) || (NULL == rsc_provider), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    sec_ret = memcpy_s(rsc_uri_tmp, sizeof(rsc_uri_tmp), rsc_provider->uri_pattern, strlen(rsc_provider->uri_pattern));
    return_val_do_info_if_expr(EOK != sec_ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: copy rsc uri [%s] failed, ret is %d", __FUNCTION__, rsc_provider->uri_pattern,
        sec_ret));

    ret = __get_rsc_uri_pattern_segments(rsc_uri_tmp, pattern_segments, &segment_num);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: cann't get uri pattern segments for %s, ret is %d", __FUNCTION__,
        rsc_provider->uri_pattern, ret));

    
    if (NULL == *root_node) {
        node_data = _new_empty_rsc_pattern_node_data(REDFISH_SERVICE_PATTERN);
        return_val_do_info_if_fail(NULL != node_data, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: alloc new resource monitor data failed", __FUNCTION__));

        *root_node = g_node_new(node_data);
        return_val_do_info_if_expr(NULL == *root_node, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: alloc new GNode failed", __FUNCTION__);
            g_free(node_data));

        debug_log(DLOG_DEBUG, "%s: new root node successfully", __FUNCTION__);

        return_val_do_info_if_expr(REDFISH_SERVICE_PATTERN_SEGMENT_NUM == segment_num, VOS_OK,
            debug_log(DLOG_DEBUG, "%s: add pattern [%s] with index [%d] ", __FUNCTION__, rsc_provider->uri_pattern,
            rsc_provider_index);
            node_data->provider_index = rsc_provider_index);
    }

    parent = *root_node;
    leaf = *root_node;

    
    for (i = REDFISH_SERVICE_PATTERN_SEGMENT_NUM; i < segment_num; i++) {
        debug_log(DLOG_DEBUG, "%s: segment index is %d, segment path is %s", __FUNCTION__, i, pattern_segments[i]);
        leaf = __find_child_rsc_provider_node_by_name(parent, pattern_segments[i]);
        if (NULL == leaf) {
            node_data = _new_empty_rsc_pattern_node_data(pattern_segments[i]);
            return_val_do_info_if_fail(NULL != node_data, VOS_ERR,
                debug_log(DLOG_ERROR, "%s: alloc new resource monitor data failed", __FUNCTION__));

            leaf = g_node_append_data(parent, node_data);
            return_val_do_info_if_expr(NULL == leaf, VOS_ERR,
                debug_log(DLOG_ERROR, "%s: alloc new GNode failed", __FUNCTION__);
                g_free(node_data));
        }

        parent = leaf;
    }

    return_val_do_info_if_expr(NULL == leaf, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: cann't alloc leaf node for pattern [%s]", __FUNCTION__, rsc_provider->uri_pattern));

    debug_log(DLOG_DEBUG, "%s: add pattern [%s] with index [%d] ", __FUNCTION__, rsc_provider->uri_pattern,
        rsc_provider_index);
    ((RSC_URI_PATTERN_NODE_DATA_S *)(leaf->data))->provider_index = rsc_provider_index;
    ((RSC_URI_PATTERN_NODE_DATA_S *)(leaf->data))->pattern_flag = rsc_provider->provider_uri_pattern_flag;

    return VOS_OK;
}


LOCAL void _copy_child_provider_node_to_sibling_node(GNode *parent_node, GNode *target_data_node)
{
    GNode *child_node = NULL;
    GNode *current = NULL;
    RSC_URI_PATTERN_NODE_DATA_S *node_data = NULL;
    GNode *copied_child_node = NULL;

    return_if_expr((NULL == parent_node));

    
    child_node = parent_node->children;

    while (child_node) {
        current = child_node;
        child_node = current->next;

        node_data = (RSC_URI_PATTERN_NODE_DATA_S *)(current->data);
        if ((NULL == node_data) || (PROVIDER_URI_SPECICAL_PATTERN_MODE != node_data->pattern_flag)) {
            continue;
        }

        
        for (copied_child_node = g_node_last_child(target_data_node); copied_child_node;
            copied_child_node = copied_child_node->prev) {
            g_node_prepend(current, g_node_copy(copied_child_node));
        }
    }

    return;
}


LOCAL gboolean __is_wildcard_provider_node(GNode *cur_child_node)
{
    RSC_URI_PATTERN_NODE_DATA_S *node_data = NULL;

    return_val_if_expr((NULL == cur_child_node) || (NULL == cur_child_node->data), FALSE);

    node_data = (RSC_URI_PATTERN_NODE_DATA_S *)(cur_child_node->data);

    return (PROVIDER_URI_WILDCARD_PATTERN_MODE == node_data->pattern_flag) ? TRUE : FALSE;
}


LOCAL gboolean __provider_child_node_copy_process_func(GNode *parent_node, gpointer data)
{
    GNode *child_node = NULL;
    GNode *current = NULL;
    gboolean b_ret;

    
    return_val_if_expr((NULL == parent_node), FALSE);

    
    child_node = parent_node->children;

    while (child_node) {
        current = child_node;
        child_node = current->next;

        
        if (G_NODE_IS_LEAF(current)) {
            continue;
        } else {
            b_ret = __is_wildcard_provider_node(current);
            continue_if_expr(FALSE == b_ret);

            _copy_child_provider_node_to_sibling_node(parent_node, current);
        }
    }

    return FALSE;
}


LOCAL void __check_and_copy_children_of_wildcard_pattern_node(GNode *provider_root_node)
{
    return_if_expr(NULL == provider_root_node);

    
    g_node_traverse(provider_root_node, G_PRE_ORDER, G_TRAVERSE_NON_LEAVES, -1, __provider_child_node_copy_process_func,
        NULL);
}


LOCAL gint32 __rf_init_rsc_provider_node_tree(RESOURCE_PROVIDER_S *rsc_provider_array, guint32 array_len,
    GNode **provider_root_node)
{
    gint32 ret;

    return_val_do_info_if_expr((NULL == rsc_provider_array) || (0 == array_len) || (NULL == provider_root_node),
        VOS_ERR, debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    for (guint32 i = 0; i < array_len; i++) {
        ret = __add_rsc_provider_node(provider_root_node, &rsc_provider_array[i], i);
        continue_do_info_if_expr(VOS_OK != ret,
            debug_log(DLOG_ERROR, "%s: add rsc provider node for %s failed, ret is %d", __FUNCTION__,
            rsc_provider_array[i].uri_pattern, ret));
    }

    __check_and_copy_children_of_wildcard_pattern_node(*provider_root_node);

    return VOS_OK;
}


void rf_init_rsc_provider_tree_info(void)
{
    gint32 ret;
    guint8 software_type = 0;
    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_EM || software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        ret = __rf_init_rsc_provider_node_tree(get_hmm_resource_provider_info(), get_hmm_resource_provider_info_size(),
            &g_hmm_resource_provider_node_tree);
        do_val_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR,
            "%s failed: cann't init provider node for g_resource_provider_info, ret is %d", __FUNCTION__, ret));

        debug_log(DLOG_INFO, "%s:g_hmm_resource_provider_node_tree size is %d", __FUNCTION__,
            g_node_n_nodes(g_hmm_resource_provider_node_tree, G_TRAVERSE_ALL));
    } else {
        ret = __rf_init_rsc_provider_node_tree(get_resource_provider_info(), get_resource_provider_info_size(),
            &g_resource_provider_node_tree);
        do_val_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR,
            "%s failed: cann't init provider node for g_resource_provider_info, ret is %d", __FUNCTION__, ret));

        debug_log(DLOG_INFO, "%s: g_resource_provider_node_tree size is %d", __FUNCTION__,
            g_node_n_nodes(g_resource_provider_node_tree, G_TRAVERSE_ALL));
    }

    return;
}


gint32 redfish_strip_to_int(gchar *str, guint32 str_size)
{
    gchar *str_p = NULL;
    gint32 end_flag = 0;
    guint32 count = 0;

    if (NULL == str) {
        return VOS_ERR;
    }

    while (*str != 0 && count < str_size) {
        
        if (*str < '0' || *str > '9') {
            
            if (*str != ' ') {
                end_flag = 1;
            }

            str_p = str;

            while (*str_p) {
                *str_p = *(str_p + 1);
                str_p++;
            }

            continue;
        } else {
            
            if (end_flag == 1) {
                return VOS_ERR;
            }
        }
        count++;
        str++;
    }

    return VOS_OK;
}


gint32 redfish_get_uuid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gchar uuid[RF_UUID_LEN + 1] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret = dal_get_system_guid_string(uuid, sizeof(uuid));
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));
    

    
    *o_result_jso = json_object_new_string((const gchar *)uuid);

    return HTTP_OK;
}


gint32 redfish_set_uuid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    const gchar *uuid_str = NULL;
    GSList *input_list = NULL;

    gint32 ret = dal_get_object_handle_nth(CLASS_BMC, 0, &obj_handle);
    goto_label_do_info_if_fail(DFL_OK == ret, internal_error_exit,
        debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth %s failed", __FUNCTION__, CLASS_BMC));

    uuid_str = dal_json_object_get_str(i_paras->val_jso);
    goto_label_do_info_if_expr(NULL == uuid_str, internal_error_exit,
        debug_log(DLOG_ERROR, "%s: dal_json_object_get_str value is null", __FUNCTION__));

    input_list = g_slist_append(input_list, g_variant_new_string(uuid_str));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_BMC, METHOD_BMC_SET_UUID, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_UUID, o_message_jso, RFPROP_UUID);
            return HTTP_FORBIDDEN;

        case VOS_ERR_NOTSUPPORT:
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_UUID, o_message_jso, uuid_str, RFPROP_UUID);
            return HTTP_BAD_REQUEST;

        default:
            debug_log(DLOG_ERROR, "%s: set uuid return value is %d", __FUNCTION__, ret);
    }

internal_error_exit:

    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


gint32 get_opr_fn_info(const gchar *identifier, REDFISH_OPR_INFO_S **opr_fn_info)
{
    gint32 ret = VOS_ERR;
    gint32 array_index;

    return_val_do_info_if_fail(((NULL != identifier) && (NULL != opr_fn_info)), ret,
        debug_log(DLOG_ERROR, "%s %d: input error", __FUNCTION__, __LINE__));

    for (array_index = 0; NULL != g_opr_fn_info[array_index].uri; array_index++) {
        if (TRUE == g_regex_match_simple(g_opr_fn_info[array_index].uri, identifier, (GRegexCompileFlags)0,
            (GRegexMatchFlags)0)) {
            *opr_fn_info = &g_opr_fn_info[array_index];
            ret = VOS_OK;
            break;
        }
    }

    return ret;
}


gboolean is_opr_fn_uri(const gchar *identifier)
{
    gboolean ret = FALSE;

    return_val_do_info_if_fail((NULL != identifier), ret,
        debug_log(DLOG_ERROR, "%s %d: input error", __FUNCTION__, __LINE__));

    for (gint32 array_index = 0; NULL != g_opr_fn_info[array_index].uri; array_index++) {
        if (TRUE == g_regex_match_simple(g_opr_fn_info[array_index].uri, identifier, (GRegexCompileFlags)0,
            (GRegexMatchFlags)0)) {
            ret = TRUE;
            break;
        }
    }

    return ret;
}


gint32 provider_paras_check(PROVIDER_PARAS_S *i_paras)
{
    return_val_if_fail(i_paras, VOS_ERR);
    return VOS_OK;
}


LOCAL gint32 add_severity_obj(guint8 in_oem, json_object *severity_obj, json_object **status_obj)
{
    json_object *huawei = NULL;
    json_object *oem = NULL;

    return_val_do_info_if_expr(NULL == status_obj || NULL == *status_obj, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:input error", __FUNCTION__));

    if (FALSE == in_oem) {
        huawei = json_object_new_object();
        if (NULL == huawei) {
            debug_log(DLOG_ERROR, "%s : json_object_new_object fail", __FUNCTION__);
            return VOS_ERR;
        }
        oem = json_object_new_object();
        if (NULL == oem) {
            json_object_put(huawei);
            debug_log(DLOG_ERROR, "%s : json_object_new_object oem fail", __FUNCTION__);
            return VOS_ERR;
        }
        json_object_object_add(huawei, RFPROP_SEL_SEVERITY, severity_obj);
        json_object_object_add(oem, RFPROP_COMMON_HUAWEI, huawei);
        json_object_object_add(*status_obj, RFPROP_COMMON_OEM, oem);
    } else {
        json_object_object_add(*status_obj, RFPROP_SEL_SEVERITY, severity_obj);
    }

    return VOS_OK;
}


gint32 get_resource_status_property(guint8 *resource_health, guint8 *resource_HealthRollup, const gchar *resource_state,
    json_object **status_obj, guint8 in_oem)
{
    int i = 0;
    gint32 ret = 0;
    gchar* health_prop[2] = {"Health", "HealthRollup"};
    guint8* health[2] = {resource_health, resource_HealthRollup};
    json_object *Prop_obj = NULL;
    json_object *severity_obj = NULL;

    if (NULL == status_obj) {
        return VOS_ERR;
    }

    *status_obj = json_object_new_object();

    if (NULL != resource_state) {
        if (strlen(resource_state)) {
            Prop_obj = json_object_new_string(resource_state);
        } else {
            Prop_obj = NULL;
        }

        json_object_object_add(*status_obj, "State", Prop_obj);
    }
    
    for (; i < 2; i++) {
        if (NULL == health[i]) {
            continue;
        }

        switch (*health[i]) {
            
            case LOGENTRY_STATUS_INFORMATIONAL_CODE:
                Prop_obj = json_object_new_string(LOGENTRY_SEVERITY_OK);
                severity_obj = json_object_new_string(INFORMATIONAL_STRING);
                break;

            case LOGENTRY_STATUS_MINOR_CODE:
                Prop_obj = json_object_new_string(LOGENTRY_SEVERITY_WARNING);
                severity_obj = json_object_new_string(MINOR_STRING);
                break;

            case LOGENTRY_STATUS_MAJOR_CODE:
                Prop_obj = json_object_new_string(LOGENTRY_SEVERITY_WARNING);
                severity_obj = json_object_new_string(MAJOR_STRING);
                break;

            case LOGENTRY_STATUS_CRITICAL_CODE:
                Prop_obj = json_object_new_string(LOGENTRY_SEVERITY_CRITICAL);
                severity_obj = json_object_new_string(CRITICAL_STRING);
                break;

            default:
                Prop_obj = NULL;
                severity_obj = NULL;
                break;
        }

        ret = add_severity_obj(in_oem, severity_obj, status_obj);
        if (VOS_ERR == ret) {
            json_object_put(Prop_obj);
            Prop_obj = NULL;
            json_object_put(severity_obj);
        }
        json_object_object_add(*status_obj, health_prop[i], Prop_obj);
    }

    return VOS_OK;
}


gint32 check_string_val_effective(const gchar *val)
{
    if (NULL == val || 0 == strlen(val) || !g_ascii_strcasecmp(val, "N/A") || !g_ascii_strcasecmp(val, "null") ||
        !g_ascii_strcasecmp(val, "UnKnown")) {
        return VOS_ERR;
    }

    return VOS_OK;
}


gint32 redfish_get_ps_verison(OBJ_HANDLE obj_handle, gchar *ver, gint32 ver_len)
{
    gchar string_value[FWINV_STR_MAX_LEN] = {0};

    
    gint32 ret = dal_get_property_value_string(obj_handle, PROTERY_PS_VERSION, string_value, sizeof(string_value));
    do_val_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__,
        CLASS_NAME_PS, PROTERY_PS_VERSION));

    dal_trim_string(string_value, FWINV_STR_MAX_LEN);

    if (VOS_OK == ret && VOS_OK == check_string_val_effective(string_value)) {
        goto REDFISH_GET_PS_VERSION_ERROR;
    }

    
    (void)memset_s(string_value, sizeof(string_value), 0, sizeof(string_value));
    ret = dal_get_property_value_string(obj_handle, PROTERY_PS_FWVERSION, string_value, sizeof(string_value));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__, CLASS_NAME_PS,
        PROTERY_PS_FWVERSION));

    dal_trim_string(string_value, FWINV_STR_MAX_LEN);

    if (VOS_OK == check_string_val_effective(string_value)) {
        goto REDFISH_GET_PS_VERSION_ERROR;
    }

    return VOS_ERR;

REDFISH_GET_PS_VERSION_ERROR:
    if (ver != NULL && ver_len > 0) {
        int iRet = snprintf_s(ver, ver_len, ver_len - 1, "%s", string_value);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet));
    }
    return VOS_OK;
}

LOCAL gint32 redfish_get_vrd_version(OBJ_HANDLE obj_handle, gchar *ver, gint32 ver_len)
{
    OBJ_HANDLE component_handle;
    gchar stringValue[FWINV_STR_MAX_LEN] = {0};
    gint32 ret;

    ret = dfl_get_referenced_object(obj_handle, PROPERTY_COMPONENT_OBJECT, &component_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dfl_get_referenced_object failed! name=%s. prop=%s. ret=%d.",
            dfl_get_object_name(obj_handle), PROPERTY_COMPONENT_OBJECT, ret);
        return RET_ERR;
    }
    (void)dal_get_property_value_string(obj_handle, PROPERTY_VRDVERSION, stringValue, sizeof(stringValue));

    dal_trim_string(stringValue, FWINV_STR_MAX_LEN);

    if (check_string_val_effective(stringValue) == RET_OK) {
        if (ver != NULL && ver_len > 0) {
            ret = snprintf_s(ver, ver_len, ver_len - 1, "%s", stringValue);
            if (ret <= 0) {
                debug_log(DLOG_ERROR, "snprintf_s fail, ret = %d", ret);
                return RET_ERR;
            }
        }
        return RET_OK;
    }
    return RET_ERR;
}


gint32 redfish_get_firmware_version(OBJ_HANDLE obj_handle, gchar *ver, gint32 ver_len)
{
    gchar string_value[FWINV_STR_MAX_LEN] = {0};
    OBJ_HANDLE refer_handle = 0;
    gint32 iRet = 0;
    gint32 ret = dfl_get_referenced_object(obj_handle, PROPERTY_PCIE_CARD_FIRMWARE_OBJ, &refer_handle);
    if (VOS_OK != ret) {
        
        debug_log(DLOG_DEBUG, "%s, %d: dfl_get_referenced_object fail", __FUNCTION__, __LINE__);
        
        return VOS_ERR;
    }

    
    ret = dal_get_property_value_string(refer_handle, PROPERTY_SOFTWARE_VERSIONSTRING, string_value,
        sizeof(string_value));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d:  dal_get_property_value_string fail", __FUNCTION__, __LINE__);

        return VOS_ERR;
    }

    dal_trim_string(string_value, FWINV_STR_MAX_LEN);

    if (VOS_OK == check_string_val_effective(string_value)) {
        if (NULL != ver && 0 < ver_len) {
            iRet = snprintf_s(ver, ver_len, ver_len - 1, "%s", string_value);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet));
        }
        return VOS_OK;
    }

    return VOS_ERR;
}



LOCAL void redfish_object_verison_print_and_check_return(gchar *ver, gint32 ver_len, gchar *string_value)
{
    errno_t safe_fun_ret = EOK;
    if (NULL != ver && 0 < ver_len) {
        safe_fun_ret = snprintf_s(ver, ver_len, ver_len - 1, "%s", string_value);
        do_val_if_expr(safe_fun_ret <= 0,
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }
}


LOCAL gint32 redfish_get_raid_version(OBJ_HANDLE objHandle, gchar *ver, gint32 verLen)
{
    OBJ_HANDLE upgradeHandle = 0;
    OBJ_HANDLE raidControllerHandle = 0;

    if (VOS_OK != dal_get_specific_object_position(objHandle, CLASS_RAID_CONTROLLER_NAME, &raidControllerHandle) ||
        VOS_OK != dfl_get_referenced_object(raidControllerHandle, PROPERTY_RAID_CONTROLLER_CPLD_UPGRD_CHIP_OBJECT,
        &upgradeHandle)) {
        return VOS_ERR;
    }
    return redfish_get_firmware_version(objHandle, ver, verLen);
}


LOCAL gint32 redfish_get_pcie_version(OBJ_HANDLE objHandle, gchar *ver, gint32 verLen)
{
    gint32 ret;
    guint8 funcClass = 0xff;

    ret = dal_get_property_value_byte(objHandle, PROPERTY_PCIECARD_FUNCTIONCLASS, &funcClass);
    return_val_if_expr(ret != VOS_OK, VOS_ERR);

    if (funcClass == PCIECARD_FUNCTION_RAID) {
        return redfish_get_raid_version(objHandle, ver, verLen);
    }

    
    if (PCIECARD_FUNCTION_ACCELERATE == funcClass) {
        return redfish_get_firmware_version(objHandle, ver, verLen);
    }

    return VOS_ERR;
}


LOCAL gint32 redfish_get_dft_version(OBJ_HANDLE objHandle, gchar *ver, gint32 verLen)
{
    guchar fruId = 0;
    guchar versionType = 0;
    gchar stringValue[FWINV_STR_MAX_LEN] = {0};
    gint32 ret;

    ret = dal_get_property_value_byte(objHandle, PROPERTY_VERSION_TYPE, &versionType);
    do_val_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte.\n", __FUNCTION__));

    ret = dal_get_property_value_byte(objHandle, PROPERTY_FRU_ID, &fruId);
    do_val_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte.\n", __FUNCTION__));
    // versionType为24 fruId为2 认为可升级
    if (versionType != VERSION_TYPE_FABRIC || fruId != FABRIC_FRU_ID) {
        return VOS_ERR;
    }

    ret = dal_get_property_value_string(objHandle, PROPERTY_VERSION_STR, stringValue, sizeof(stringValue));
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get prop fail, %s, %s", __FUNCTION__, CLASS_DFT_VERSION, PROPERTY_VERSION_STR));

    dal_trim_string(stringValue, FWINV_STR_MAX_LEN);
    redfish_object_verison_print_and_check_return(ver, verLen, stringValue);

    return VOS_OK;
}

LOCAL gint32 redfish_get_mcu_version(OBJ_HANDLE objHandle, gchar *ver, gint32 verLen)
{
    gint32 ret;
    gchar stringValue[FWINV_STR_MAX_LEN] = {0};
 
    ret = dal_get_property_value_string(objHandle, PROPERTY_SOFTWARE_VERSIONSTRING, stringValue, sizeof(stringValue));
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR, debug_log(DLOG_ERROR, "%s: get prop fail, %s, %s", __FUNCTION__,
        PROPERTY_SOFTWARE_VERSIONSTRING, PROPERTY_VERSION_STR));
 
    dal_trim_string(stringValue, FWINV_STR_MAX_LEN);
    redfish_object_verison_print_and_check_return(ver, verLen, stringValue);
 
    return VOS_OK;
}
 

LOCAL gint32 redfish_get_other_version(OBJ_HANDLE objHandle, gchar *ver, gint32 verLen, const gchar *propName)
{
    gchar stringValue[FWINV_STR_MAX_LEN] = {0};
    gint32 ret;

    
    ret = dal_get_property_value_string(objHandle, propName, stringValue, sizeof(stringValue));
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:  get prop fail, %s, %s\n", __FUNCTION__, dfl_get_object_name(objHandle), propName));

    dal_trim_string(stringValue, FWINV_STR_MAX_LEN);
    return_val_if_fail(VOS_OK == check_string_val_effective(stringValue), VOS_ERR);

    redfish_object_verison_print_and_check_return(ver, verLen, stringValue);
    return VOS_OK;
}

gint32 redfish_get_object_verison(const gchar *class_name, OBJ_HANDLE obj_handle, const gchar *prop_name, gchar *ver,
    gint32 ver_len)
{
    gint32 ret = VOS_OK;
    guint8 uint8_value = 0xff;
    OBJ_HANDLE component_handle = 0;

    return_val_if_fail(NULL != class_name && NULL != prop_name, VOS_ERR);

    if (!g_strcmp0(class_name, CLASS_UPGRADE_MCU_SOFTWARE_NAME)) {
        return redfish_get_mcu_version(obj_handle, ver, ver_len);
    }
    
    
    if (g_strcmp0(class_name, CLASS_RAID_CONTROLLER_NAME) && g_strcmp0(class_name, BMC_CLASEE_NAME) &&
        g_strcmp0(class_name, BIOS_CLASS_NAME) && g_strcmp0(class_name, CLASS_VRD_UPGRADE_MGNT) &&
        g_strcmp0(class_name, CLASS_DFT_VERSION) && g_strcmp0(class_name, CLASS_PERIPHERAL_FIRMWARE_NAME) &&
        g_strcmp0(class_name, CLASS_RETIMER_NAME)&& g_strcmp0(class_name, CLASS_ME_INFO)) {
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_FIRMWARE_PRESENCE, &uint8_value);
        return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: get prop fail, %s, %s", __FUNCTION__, class_name, PROPERTY_FIRMWARE_PRESENCE));
        return_val_if_fail(uint8_value != 0, VOS_ERR);
    }
    

    
    if (!g_strcmp0(class_name, CLASS_NAME_PS)) {
        return redfish_get_ps_verison(obj_handle, ver, ver_len);
    }
    
    
    if (!g_strcmp0(class_name, CLASS_RAIDCARD_NAME)) {
        return redfish_get_raid_version(obj_handle, ver, ver_len);
    }
    
    if (!g_strcmp0(class_name, CLASS_PCIECARD_NAME)) {
        return redfish_get_pcie_version(obj_handle, ver, ver_len);
    }
    

    
    if (!g_strcmp0(class_name, CLASS_DFT_VERSION)) {
        return redfish_get_dft_version(obj_handle, ver, ver_len);
    }

    
    if (!g_strcmp0(class_name, CLASS_VRD_UPGRADE_MGNT)) {
        return redfish_get_vrd_version(obj_handle, ver, ver_len);
    }

    
    
    if (!g_strcmp0(class_name, CLASS_CPUBOARD_NAME) || !g_strcmp0(class_name, CLASS_IOBOARD_NAME) ||
        !g_strcmp0(class_name, CLASS_HDDBACKPLANE_NAME)) {
        ret = dfl_get_referenced_object(obj_handle, PROPERTY_IOBOARD_REFCOMPONENT, &component_handle);
        if (ret != VOS_OK) {
            
            debug_log(DLOG_DEBUG, "%s: dfl_get_referenced_object fail class: %s property: %s.", __FUNCTION__,
                class_name, PROPERTY_IOBOARD_REFCOMPONENT);
            
            return VOS_ERR;
        }
        debug_log(DLOG_DEBUG, "%s: classname: %s componentname: %s.\n", __FUNCTION__, class_name,
            dfl_get_object_name(component_handle));
    }
    
    
    return redfish_get_other_version(obj_handle, ver, ver_len, prop_name);
}


gint32 redfish_get_property_value_uint16(PROPERTY_PROVIDER_S *self, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guint16 value = 0;
    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso) && (NULL != self) && (NULL != i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input parm error", __FUNCTION__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    ret = dal_get_object_handle_nth(self->pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_FOUND,
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri));
    
    
    (void)dal_get_property_value_uint16(obj_handle, self->pme_prop_name, &value);

    *o_result_jso = json_object_new_int(value);

    return HTTP_OK;
}


gint32 redfish_get_property_value_string(PROPERTY_PROVIDER_S *self, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    char strbuf[STRING_LEN_MAX] = {0};
    gint32 ret;

    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso) && (NULL != self) && (NULL != i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = dal_get_object_handle_nth(self->pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_FOUND,
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri));

    
    ret = dal_get_property_value_string(obj_handle, self->pme_prop_name, strbuf, sizeof(strbuf));

    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    
    if (rf_string_check(strbuf) != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    

    
    *o_result_jso = json_object_new_string((const gchar *)strbuf);
    return HTTP_OK;
}

gint32 redfish_set_property_value_uint16(PROPERTY_PROVIDER_S *self, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso, const gchar *method_name, guint8 method_privilege)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    gint32 param;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gchar strbuf[STRING_LEN_MAX] = "\0";

    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != self) && (NULL != i_paras) && (NULL != method_name),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & method_privilege, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, self->property_name, o_message_jso,
        self->property_name));
    
    return_val_do_info_if_fail((NULL != i_paras->val_jso), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_TYPE_ERR, self->property_name, o_message_jso, RF_VALUE_NULL,
        self->property_name));

    
    
    return_val_do_info_if_fail(json_type_int == json_object_get_type(i_paras->val_jso), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, self->property_name, o_message_jso,
        dal_json_object_get_str(i_paras->val_jso), self->property_name));
    

    param = json_object_get_int(i_paras->val_jso);

    
    return_val_do_info_if_expr(param < 0 || param > G_MAXUINT16, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, self->property_name, o_message_jso,
        dal_json_object_get_str(i_paras->val_jso), self->property_name));
    

    input_list = g_slist_append(input_list, g_variant_new_uint16((guint16)param));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        self->pme_class_name, method_name, 0, i_paras->user_role_privilege, input_list, &output_list);
    

    uip_free_gvariant_list(input_list);

    
    
    
    uip_free_gvariant_list(output_list);

    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

        case RFERR_WRONG_PARAM: 
            (void)snprintf_s(strbuf, sizeof(strbuf), sizeof(strbuf) - 1, "%d", param);
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, self->property_name, o_message_jso, (const gchar *)strbuf,
                self->property_name);
            return HTTP_BAD_REQUEST;

        case RFERR_NO_RESOURCE: 
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_NOT_FOUND;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


gint32 redfish_alarm_severities_change(guchar *data, gchar *str_alarm, gint32 length)
{
    if (NULL == data || NULL == str_alarm) {
        debug_log(DLOG_ERROR, "%s, %d:  fail. \n", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    
    if (length == 0) {
        return_val_do_info_if_expr(0 == g_strcmp0(str_alarm, NONE_STRING), VOS_OK, (*data = NONE_NUM_LEVEL));
        return_val_do_info_if_expr(0 == g_strcmp0(str_alarm, NORMAL_STRING), VOS_OK, (*data = NORMAL_NUM_LEVEL));
        return_val_do_info_if_expr(0 == g_strcmp0(str_alarm, MINOR_STRING), VOS_OK, (*data = MINO_NUM_LEVEL));
        return_val_do_info_if_expr(0 == g_strcmp0(str_alarm, MAJOR_STRING), VOS_OK, (*data = MAJOR_NUM_LEVEL));
        return_val_do_info_if_expr(0 == g_strcmp0(str_alarm, CRITICAL_STRING), VOS_OK, (*data = CRITICA_NUM_LEVEL));
        debug_log(DLOG_ERROR, "%s, %d:  fail. \n", __FUNCTION__, __LINE__);
        return VOS_ERR;
    } else {
        if ((*data & 0x01) == 1) {
            return_val_if_expr(0 < snprintf_s(str_alarm, length, length - 1, "%s", NORMAL_STRING), VOS_OK);
        } else if ((*data & 0x02) == 2) {
            return_val_if_expr(0 < snprintf_s(str_alarm, length, length - 1, "%s", MINOR_STRING), VOS_OK);
        } else if ((*data & 0x04) == 4) {
            return_val_if_expr(0 < snprintf_s(str_alarm, length, length - 1, "%s", MAJOR_STRING), VOS_OK);
        } else if ((*data & 0x08) == 8) {
            return_val_if_expr(0 < snprintf_s(str_alarm, length, length - 1, "%s", CRITICAL_STRING), VOS_OK);
        } else if (*data == 0) {
            
            return_val_if_expr(0 < snprintf_s(str_alarm, length, length - 1, "%s", NONE_STRING), VOS_OK);
        }

        debug_log(DLOG_ERROR, "%s, %d:  fail. \n", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }
}

property_handler_t find_property_provider(const PROPERTY_PROVIDER_S *provider, guint32 provider_count,
    const char *property_name, guint32 *provider_index)
{
    guint32 i;

    check_fail_return_val(provider && provider_count && property_name && provider_index, NULL);

    for (i = 0; i < provider_count; i++) {
        if (!g_strcmp0(provider[i].property_name, property_name)) {
            *provider_index = i;

            debug_log(DLOG_DEBUG, "property_name is %s provider found", property_name);
            check_success_return_val(provider[i].pfn_set, provider[i].pfn_set);
            check_success_return_val(provider[i].pfn_action, provider[i].pfn_action);
        }
    }

    debug_log(DLOG_DEBUG, "property_name is %s provider not found", property_name);

    return NULL;
}


LOCAL void get_resource_provider_list(RESOURCE_PROVIDER_S **resource_provider_list_ptr, guint32 *array_len)
{
    guint8 software_type = 0;
    (void)dal_get_software_type(&software_type);
    if (MGMT_SOFTWARE_TYPE_EM == software_type || software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        *resource_provider_list_ptr = get_hmm_resource_provider_info();
        *array_len = get_hmm_resource_provider_info_size();
    } else {
        *resource_provider_list_ptr = get_resource_provider_info();
        *array_len = get_resource_provider_info_size();
    }
}


LOCAL void get_resource_provider_node_tree(GNode **resource_provider_node_ptr)
{
    guint8 software_type = 0;
    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_EM || software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        *resource_provider_node_ptr = g_hmm_resource_provider_node_tree;
    } else {
        *resource_provider_node_ptr = g_resource_provider_node_tree;
    }

    return;
}


gint32 get_resource_provider(const gchar *uri, RESOURCE_PROVIDER_S **resource_provider)
{
    guint32 idx;
    gchar **uri_segments = NULL;
    guint32 array_len;

    
    RESOURCE_PROVIDER_S *resource_provider_list_ptr = NULL; // 指向板子对应软件型号的provider列表的指针
    GNode *provider_root_node = NULL;                       // 指向板子对应软件型号的provider的根节点
    GNode *parent = NULL;
    GNode *leaf = NULL;
    RSC_URI_PATTERN_NODE_DATA_S *node_data = NULL;
    guint32 provider_index;
    const gchar *tmp_uri = uri;

    get_resource_provider_node_tree(&provider_root_node);
    if (provider_root_node == NULL) {
        debug_log(DLOG_ERROR, "%s: Cannot get provider root node.", __FUNCTION__);
        return RET_ERR;
    }

    
    return_val_if_fail((NULL != uri) && (NULL != resource_provider), VOS_ERR);
    

    do_val_if_expr(SLASH_CHAR == *tmp_uri, (tmp_uri++));

    uri_segments = g_strsplit(tmp_uri, SLASH_FLAG_STR, 0);
    return_val_do_info_if_expr(NULL == uri_segments, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: cann't split uri [%s] with [%s]", __FUNCTION__, uri, SLASH_FLAG_STR));

    array_len = g_strv_length(uri_segments);

    parent = provider_root_node;
    leaf = provider_root_node;

    
    for (idx = 1; idx < array_len; idx++) {
        leaf = __match_child_rsc_provider_node(parent, uri_segments[idx], array_len - idx - 1);
        return_val_do_info_if_expr(NULL == leaf, VOS_ERR,
            debug_log(DLOG_DEBUG, "%s: cann't find provider node for [%s], current segment is %s", __FUNCTION__, uri,
            uri_segments[idx]);
            g_strfreev(uri_segments));

        parent = leaf;
    }

    g_strfreev(uri_segments);

    node_data = (RSC_URI_PATTERN_NODE_DATA_S *)leaf->data;
    provider_index = node_data->provider_index;

    debug_log(DLOG_MASS, "%s: find provide node for [%s] successfully, resource index is %d, node name is %s",
        __FUNCTION__, uri, provider_index, node_data->node_name);

    get_resource_provider_list(&resource_provider_list_ptr, &array_len);
    return_val_do_info_if_expr(provider_index >= array_len, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s: invalid resource provider index [%u]", __FUNCTION__, provider_index));

    *resource_provider = &resource_provider_list_ptr[provider_index];

    return VOS_OK;
}

gint32 redfish_action_format_check(const gchar *str_action, struct json_object *jso_allowable_values,
    struct json_object *jso_body, json_object **o_message_jso)
{
    const gchar *str_param = NULL;
    const gchar *str_value = NULL;
    const gchar *str_key = NULL;
    const gchar *str_val = NULL;
    guint i, count;
    struct json_object *jso_value = NULL;
    struct json_object *jso_array = NULL;
    char *name = NULL;
    struct lh_entry *entry_key = NULL;
    struct lh_entry *entry_next_key = NULL;
    struct lh_table *obj_ref = json_object_get_object(jso_body);

    return_val_do_info_if_fail((NULL != str_action) && (NULL != jso_allowable_values) && (NULL != jso_body) &&
        (NULL != o_message_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    str_key = ex_json_object_object_get_key(jso_allowable_values);
    jso_array = ex_json_object_object_get_val(jso_allowable_values);

    str_param = ex_json_object_object_get_key(jso_body);
    jso_value = ex_json_object_object_get_val(jso_body);

    return_val_do_info_if_fail((NULL != str_key) && (NULL != jso_array) && (NULL != str_param),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif

    // parameter中只要有不正确的, 都返回unknow
    
    if (obj_ref)
        for (entry_key = obj_ref->head;
            (entry_key ? (name = (char *)entry_key->k, entry_next_key = entry_key->next, entry_key) : 0);
            entry_key = entry_next_key) {
            
            if (0 != g_ascii_strncasecmp(str_key, name, STRING_LEN_MAX)) {
                debug_log(DLOG_DEBUG, "[%s]: ERROR: %s", __FUNCTION__, MSGID_ACT_PARA_MISSING);
                (void)create_message_info(MSGID_ACT_PARA_UNKNOWN, NULL, o_message_jso, str_action, name);
                return HTTP_BAD_REQUEST;
            }
        }

    // parameter value类型不正确
    return_val_do_info_if_fail((NULL != jso_value) && (json_type_string == json_object_get_type(jso_value)),
        HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_TYPE_ERR, NULL, o_message_jso,
            dal_json_object_to_json_string(jso_value), str_param, str_action));

    str_value = dal_json_object_get_str(jso_value);

    count = json_object_array_length(jso_array);

    for (i = 0; i < count; i++) {
        str_val = dal_json_object_get_str(json_object_array_get_idx(jso_array, i));
        if (0 == g_ascii_strncasecmp(str_val, str_value, STRING_LEN_MAX)) {
            return HTTP_OK;
        }
    }

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
    debug_log(DLOG_DEBUG, "[%s]: ERROR: %s", __FUNCTION__, MSGID_ACT_PARA_FORMAT_ERR);
    (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, str_value, str_param, str_action);
    return HTTP_BAD_REQUEST;
}


gint32 redfish_get_board_type(guchar *board_type)
{
    return dal_rf_get_board_type(board_type);
}


gint32 redfish_get_x86_enable_type(guchar *enable_type)
{
    return_val_do_info_if_fail(NULL != enable_type, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    return dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_X86, enable_type);
}


gint32 redfish_get_support_sys_power_type(guchar* enable_type)
{
    if (enable_type == NULL) {
        debug_log(DLOG_ERROR, "failed:input param error");
        return RET_ERR;
    }

    return dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_SSPC, enable_type);
}


gint32 redfish_get_board_slot(gchar *slot_string, gint32 slot_len)
{
    return dal_rf_get_board_slot(slot_string, slot_len);
}


gint32 rf_gen_enc_contained_component_chassis_slot(gchar *chassis_slot, guint32 max_slot_len)
{
    gint32 ret;
    guchar board_type = 0;

    ret = dal_rf_get_board_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "cann't get chassis board type"));
    
    if (BOARD_MM == board_type) {
        return_val_do_info_if_expr(0 == max_slot_len, VOS_ERR,
            debug_log(DLOG_ERROR, "%s failed:input param error, max_slot_len is 0.", __FUNCTION__));
        ret = snprintf_s(chassis_slot, max_slot_len, max_slot_len - 1, "%s", RF_ENCLOSURE_CHASSIS_PREFIX);
        return_val_do_info_if_fail(ret > 0, VOS_ERR,
            debug_log(DLOG_ERROR, "format enclosure chassis id failed, ret is %d", ret));
    } else {
        ret = dal_rf_get_board_slot(chassis_slot, max_slot_len);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "get main board chassis slot failed"));
    }

    return VOS_OK;
}


LOCAL gboolean check_uri_valid(const gchar* uri_string, const gchar* uri_format, const gchar* id)
{
    gint32 uri_prefix_len;
    gchar  uri_prefix[MAX_URI_LENGTH];
    gchar  id_prefix[PSLOT_MAX_LEN];
    gint32 iRet;

    if (id == NULL) {
        iRet = redfish_get_board_slot(id_prefix, sizeof(id_prefix));
        if (iRet != RET_OK) {
            debug_log(DLOG_ERROR, "%s: function return err of redfish_slot_id, ret = %d", __FUNCTION__, iRet);
            return FALSE;
        }
    } else {
        (void)memset_s(id_prefix, PSLOT_MAX_LEN, 0, PSLOT_MAX_LEN);
        iRet = strcpy_s(id_prefix, sizeof(id_prefix), id);
        if (iRet != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, iRet);
            return FALSE;
        }
    }

    (void)memset_s(uri_prefix, MAX_URI_LENGTH, 0, MAX_URI_LENGTH);
    iRet = strcpy_s(uri_prefix, MAX_URI_LENGTH, uri_format);
    if (iRet != EOK) {
        debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d", __FUNCTION__, iRet);
        return FALSE;
    }

    iRet = strcat_s(uri_prefix, MAX_URI_LENGTH, id_prefix);
    if (iRet != EOK) {
        debug_log(DLOG_ERROR, "%s: strcat_s fail, ret = %d", __FUNCTION__, iRet);
        return FALSE;
    }

    uri_prefix_len = strlen(uri_prefix);
    if ((strlen(uri_string) >= uri_prefix_len) &&
        (g_ascii_strncasecmp(uri_string, uri_prefix, uri_prefix_len) == 0) &&
        ((*(uri_string + uri_prefix_len) == '\0') || (*(uri_string + uri_prefix_len) == '/'))) {
        return TRUE;
    }

    return FALSE;
}


gboolean redfish_check_system_uri_valid(gchar *uri_string)
{
    return check_uri_valid((const gchar*)uri_string, RF_SYSTEMS_URI, NULL);
}


gint32 find_chassis_component_handle_with_id(const gchar *chassis_id, OBJ_HANDLE *component_handle)
{
    GSList *obj_list = NULL;
    GSList *obj_iter = NULL;
    OBJ_HANDLE obj_handle;
    gint32 ret;
    gchar               component_chassis_id[MAX_MEM_ID_LEN] = {0};
    CHASSIS_COMPONENT_INFO chassis_component_info = { 0 };
    guchar comp_id = 0;
    guchar comp_device_type = 0;
    guchar comp_slot = 0;

    return_val_do_info_if_fail((NULL != chassis_id) && (NULL != component_handle), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    ret = redfish_get_board_slot(component_chassis_id, MAX_MEM_ID_LEN);

    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "get mainboard slot failed"));

    if (0 == g_ascii_strcasecmp(chassis_id, component_chassis_id)) {
        ret = dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE, COMPONENT_TYPE_MAINBOARD,
            component_handle);
        do_val_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR,
            "cann't get mainboard component handle with component id (%d)", COMPONENT_TYPE_MAINBOARD));

        return ret;
    }

    
    ret = rf_get_chassis_component_info_by_id(chassis_id, &chassis_component_info, &comp_id);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "get component info with id (%s) failed", chassis_id));

    
    *component_handle = 0;
    ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed, get object list for %s failed", __FUNCTION__, CLASS_COMPONENT));

    for (obj_iter = obj_list; NULL != obj_iter; obj_iter = obj_iter->next) {
        obj_handle = (OBJ_HANDLE)obj_iter->data;

        (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &comp_device_type);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICENUM, &comp_slot);

        
        if ((comp_device_type == chassis_component_info.component_type) &&
            ((NULL == chassis_component_info.component_num_prop) || (comp_slot == comp_id))) {
            *component_handle = obj_handle;
            break;
        }
    }

    g_slist_free(obj_list);

    return VOS_OK;
}


LOCAL gboolean rf_handle_chassis_uri_valid_check(gchar *uri_string, OBJ_HANDLE *component_handle,
    gboolean is_allow_node_enc)
{
    gint32 ret;
    gchar *chassis_id = NULL;
    guchar board_type = 0;
    guchar component_type = 0;
    guint32 product_id = 0;
    OBJ_HANDLE product_handle = 0;

    return_val_do_info_if_fail((NULL != uri_string) && (NULL != component_handle), FALSE,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    ret = rf_get_regex_match_result_nth(URI_PATTERN_CHASSIS_RELATED_RSC, uri_string, 1, &chassis_id);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_ERROR, "match %s with %s failed", uri_string, URI_PATTERN_CHASSIS_RELATED_RSC));

    ret = find_chassis_component_handle_with_id(chassis_id, component_handle);
    g_free(chassis_id);

    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_ERROR, "cann't get component handle for input chassis uri"));

    (void)dal_get_property_value_byte(*component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
    (void)dal_rf_get_board_type(&board_type);

    
    if (!is_allow_node_enc) {
        return_val_if_expr(((BOARD_MM != board_type) && (component_type != COMPONENT_TYPE_MAINBOARD)), FALSE);
    }
    
    else if (check_pangea_node_to_enc(*component_handle) == FALSE) {
        (void)dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &product_handle);
        (void)dal_get_property_value_uint32(product_handle, PROPERTY_PRODUCT_ID, &product_id);
        if (dal_check_if_vsmm_supported()) {
            return TRUE;
        }
        if (board_type == BOARD_BLADE &&
            product_id != PRODUCT_ID_ATLAS &&
            product_id != PRODUCT_ID_XSERIAL_SERVER &&
            component_type == COMPONENT_TYPE_CHASSIS_BACKPLANE) {
                return FALSE;
        }
    }

    do_val_if_expr(0 != *component_handle, debug_log(DLOG_DEBUG,
        "chassis component found successfully, component object name :%s", dfl_get_object_name(*component_handle)));
    return TRUE;
}


gboolean rf_check_chassis_uri_valid_allow_node_enc(gchar *uri_string, OBJ_HANDLE *component_handle)
{
    gboolean b_ret;

    b_ret = rf_handle_chassis_uri_valid_check(uri_string, component_handle, TRUE);
    return b_ret;
}


gboolean redfish_check_chassis_uri_valid(gchar *uri_string, OBJ_HANDLE *component_handle)
{
    gboolean b_ret;

    b_ret = rf_handle_chassis_uri_valid_check(uri_string, component_handle, FALSE);
    return b_ret;
}


gint32 rf_get_regex_match_result_nth(const gchar *pattern, const gchar *string, guchar position, gchar **result)
{
    GRegex *re;
    GMatchInfo *match_info = NULL;
    gboolean b_ret;

    re = g_regex_new(pattern, (GRegexCompileFlags)0, (GRegexMatchFlags)0, NULL);

    b_ret = g_regex_match(re, string, (GRegexMatchFlags)0, &match_info);
    g_regex_unref(re);

    
    return_val_do_info_if_fail(TRUE == b_ret, VOS_ERR, g_match_info_free(match_info);
        debug_log(DLOG_ERROR, "match failed for %s with %s", string, pattern));
    

    *result = g_match_info_fetch(match_info, position);

    g_match_info_free(match_info);

    return VOS_OK;
}


gboolean redfish_check_chassis_rack_uri_valid(gchar *uri_string, OBJ_HANDLE *obj_handle)
{
    gboolean match_flag = FALSE;
    gint32 ret;

    return_val_do_info_if_fail((NULL != uri_string) && (NULL != obj_handle), FALSE,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    return_val_do_info_if_expr(0 == strlen(uri_string), FALSE,
        debug_log(DLOG_ERROR, "%s Uri is illegal.", __FUNCTION__));

    // 获取RackAssetMgnt对象句柄
    ret = dal_get_object_handle_nth(CLASS_RACK_ASSET_MGMT, 0, obj_handle);
    if (VOS_OK == ret) {
        match_flag = TRUE;
    }

    return match_flag;
}


gboolean redfish_check_smm_chassis_uri_valid(gchar *uri_string)
{
    return check_uri_valid((const gchar*)uri_string, RF_CHASSIS_UI, SMM_CHASSIS_ENCLOSURE);
}


gboolean redfish_check_smm_manager_uri_valid(gchar *uri_string)
{
    return check_uri_valid((const gchar*)uri_string, RF_MANAGERS_URI, NULL);
}


gint32 redfish_health_count_to_level(guint32 minor, guint32 major, guint32 critical, guint8 *health)
{
    if (health == NULL) {
        return VOS_ERR;
    }
    if (0 != critical) {
        *health = EVENT_SEVERITY_CRITICAL;
        return VOS_OK;
    }
    if (0 != major) {
        *health = EVENT_SEVERITY_MAJOR;
        return VOS_OK;
    }
    if (0 != minor) {
        *health = EVENT_SEVERITY_MINOR;
        return VOS_OK;
    }
    *health = EVENT_SEVERITY_NORMAL;
    return VOS_OK;
}


gint32 redfish_get_smm_health(guint8 *health)
{
    gint32 ret;
    guint32 minor = 0;
    guint32 major = 0;
    guint32 critical = 0;
    if (health == NULL) {
        return VOS_ERR;
    }
    ret = smm_get_health_events_count(&minor, &major, &critical);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get smm health events count failed ,err code %d", __func__, ret);
        *health = EVENT_SEVERITY_NORMAL;
        return VOS_ERR;
    }
    return redfish_health_count_to_level(minor, major, critical, health);
}


gint32 redfish_get_blade_health(guint8 slave_addr, guint8 *health)
{
    gint32 ret;
    guint32 data[6] = { 0 };

    if (health == NULL) {
        return VOS_ERR;
    }

    ret = foreach_blade_health_event(count_single_blade_health_event, slave_addr, (gpointer)data);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "Get blade addr:%x health event failed.\r\n", slave_addr);
        return VOS_ERR;
    }
    return redfish_health_count_to_level(data[0], data[1], data[2], health);
}


gboolean redfish_check_manager_uri_valid(gchar *uri_string)
{
    return check_uri_valid((const gchar*)uri_string, RF_MANAGERS_URI, NULL);
}


gboolean check_chassis_uri_pangea_enc(const gchar *uri_string)
{
    if (!dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_PACIFIC_CTRL) &&
        !dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_ARCTIC_CTRL)) {
        return FALSE;
    }
    return check_uri_valid((const gchar*)uri_string, RF_CHASSIS_UI, RF_ENCLOSURE_CHASSIS_PREFIX);
}



gint32 get_odata_id_object(gchar *pre_uri, gchar *next_uri, json_object **object)
{
    int iRet;
    gint32 ret;
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    gchar uri[MAX_URI_LENGTH] = {0};
    json_object *str_object = NULL;

    if (NULL == object || NULL == pre_uri || NULL == next_uri) {
        return VOS_ERR;
    }

    ret = redfish_get_board_slot(slot_str, sizeof(slot_str));
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: get redfish_get_slot fail.\n", __FUNCTION__, __LINE__));

    iRet = snprintf_s(uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s%s/%s", pre_uri, slot_str, next_uri);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    str_object = json_object_new_string((const gchar *)uri);
    return_val_if_expr(NULL == str_object, VOS_ERR);

    *object = json_object_new_object();
    return_val_do_info_if_expr(NULL == *object, VOS_ERR, json_object_put(str_object);
        debug_log(DLOG_ERROR, "%s, %d: get redfish_get_slot fail.\n", __FUNCTION__, __LINE__));

    
    json_object_object_add(*object, ODATA_ID, str_object);

    return VOS_OK;
}


gint32 get_file_transfer_progress(gint32 *progress)
{
    GSList *output_list = NULL;
    gint32 ret;
    gint32 status;

    if (NULL == progress) {
        return VOS_ERR;
    }

    ret = uip_call_class_method(NULL, NULL, NULL, CLASS_NAME_TRANSFERFILE, OBJ_NAME_TRANSFERFILE,
        METHOD_GET_TRANSFER_STATUS, NULL, &output_list);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "Get transfer file download progress failed.");
        return ret;
    }

    status = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    *progress = status;

    
    uip_free_gvariant_list(output_list);
    return VOS_OK;
}


gint32 get_provider_param_from_session(const char *session_id, PROVIDER_PARAS_S *o_provider_param)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    GSList *caller_info = NULL;
    OBJ_HANDLE obj_handle;
    gchar         session_info_str[REDFISH_SESSION_STR_LEN] = {0};
    const gchar *session_p = NULL;
    json_object *session_jso = NULL;
    json_object *val_jso = NULL;

    check_fail_return_val(o_provider_param, RF_FAILED);

    ret = dal_get_object_handle_nth(CLASS_SESSION, 0, &obj_handle);
    return_val_if_fail(VOS_OK == ret, ret);

    // input_list中为session_id,即校验和加密值
    input_list = g_slist_append(input_list, g_variant_new_string(session_id));

    // caller_info第一个参数用于区分Redfish会话与其他会话，第二个参数用于决定是否会话上次激活时间
    // 用于鉴权通过时需要刷新激活时间
    // 对于通过URI查询会话信息时，不需刷新激活时间
    caller_info = g_slist_append(caller_info, g_variant_new_string(USER_LOGIN_INTERFACE_REDFISH_STRING));
    caller_info = g_slist_append(caller_info, g_variant_new_int32(REDFISH_REFRESH));

    // 调用user模块"GetOnlineUser"方法，判断是否存在有效会话，存在则获取会话信息
    ret = dfl_call_class_method(obj_handle, METHOD_SESSION_GETONLINEUSER, caller_info, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
    return_val_do_info_if_fail((VOS_OK == ret), VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d: dfl_call_class_method %s fail", __FUNCTION__, __LINE__,
        METHOD_SESSION_GETONLINEUSER));

    // 返回值为json格式字符串，里边包含所有会话相关的信息
    session_p = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    return_val_do_info_if_fail((NULL != session_p) && (0 != strlen(session_p)), VOS_ERR,
        debug_log(DLOG_DEBUG, "%s %d: get session info fail", __FUNCTION__, __LINE__);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref));

    (void)strncpy_s(session_info_str, REDFISH_SESSION_STR_LEN, session_p, REDFISH_SESSION_STR_LEN - 1);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    // 将会话信息字符串转换为json对象，逐项取出保存在结构体中
    // 此处与user模块方法中填充的信息项目一一对应
    session_jso = json_tokener_parse((const gchar *)session_info_str);
    clear_sensitive_info(session_info_str, sizeof(session_info_str));

    (void)json_object_object_get_ex(session_jso, RF_SESSION_TYPE, &val_jso);
    
    if ((o_provider_param->is_from_webui == 0) &&
        (strcmp(dal_json_object_get_str(val_jso), USER_LOGIN_INTERFACE_GUI_STRING) == 0 ||
        strcmp(dal_json_object_get_str(val_jso), USER_LOGIN_INTERFACE_GUI_SSO_STRING) == 0)) {
        
        // 防止绕过WEB的token、二次认证等校验
        // 使用WEB的sessionid会话信息进行配置修改
        json_object_put(session_jso);
        return VOS_ERR;
    }
    val_jso = NULL;

    (void)json_object_object_get_ex(session_jso, RF_SESSION_USER_PRI, &val_jso);
    o_provider_param->user_role_privilege = json_object_get_int(val_jso);
    val_jso = NULL;

    (void)json_object_object_get_ex(session_jso, RF_SESSION_NAME, &val_jso);
    (void)strncpy_s(o_provider_param->user_name, SESSION_USER_NAME_MAX_LEN + 1, dal_json_object_get_str(val_jso),
        SESSION_USER_NAME_MAX_LEN);
    val_jso = NULL;

    
    (void)json_object_object_get_ex(session_jso, RF_SESSION_IP, &val_jso);
    (void)strncpy_s(o_provider_param->client, SESSION_IP_LEN, dal_json_object_get_str(val_jso), SESSION_IP_LEN - 1);
    val_jso = NULL;
    

    (void)json_object_object_get_ex(session_jso, RF_SESSION_ROLEID, &val_jso);
    (void)strncpy_s(o_provider_param->user_roleid, USER_ROLEID_MAX_LEN + 1, dal_json_object_get_str(val_jso),
        USER_ROLEID_MAX_LEN);
    val_jso = NULL;

    (void)json_object_object_get_ex(session_jso, RF_SESSION_ID, &val_jso);
    (void)strncpy_s(o_provider_param->session_id, SESSION_ID_LEN + 1, dal_json_object_get_str(val_jso), SESSION_ID_LEN);

    (void)json_object_object_get_ex(session_jso, RF_SESSION_AUTH_TYPE, &val_jso);
    o_provider_param->auth_type = json_object_get_int(val_jso);

    json_object_put(session_jso);

    return VOS_OK;
}

gint32 check_delete_property_by_class(json_object *jso, const gchar *prop_path, json_bool *delete_flage)
{
    json_bool ret_flag;
    gchar **path_segments = NULL;
    guint path_segment_len;
    json_object *property_jso = NULL;
    GSList *prop_list = NULL;
    GSList *prop_node = NULL;

    return_val_if_expr(((NULL == jso) || (NULL == prop_path) || (NULL == delete_flage)), VOS_ERR);
    *delete_flage = FALSE;

    path_segments = g_strsplit(prop_path, "/", 0);
    return_val_if_expr(NULL == path_segments, VOS_ERR);

    path_segment_len = g_strv_length(path_segments);
    return_val_do_info_if_expr(path_segment_len < 1, VOS_ERR, g_strfreev(path_segments); path_segments = NULL);

    ret_flag = get_property_obj_from_segments(jso, path_segments, path_segment_len - 1, &property_jso);
    return_val_do_info_if_expr((!ret_flag), VOS_ERR, g_strfreev(path_segments); path_segments = NULL);

    ret_flag = json_object_get_user_data(property_jso, (void **)&prop_list);
    return_val_do_info_if_expr((!ret_flag), VOS_ERR, g_strfreev(path_segments); path_segments = NULL);

    for (prop_node = prop_list; prop_node; prop_node = prop_node->next) {
        if (0 == g_strcmp0(path_segments[path_segment_len - 1], (const gchar *)prop_node->data)) {
            *delete_flage = TRUE;
            break;
        }
    }

    g_strfreev(path_segments);
    path_segments = NULL;
    return VOS_OK;
}


gint32 generate_del_common_response_headers(PROVIDER_PARAS_S *i_param, json_object **header_array_jso, const gchar *uri,
    json_object *message_array_jso)
{
    gint32 ret;

    // 生成公共响应头
    ret = generate_common_response_headers(header_array_jso, uri, message_array_jso);

    return ret;
}


gint32 generate_post_rsc_etag(PROVIDER_PARAS_S *i_param, const gchar *member_id, json_object **o_header_array_jso,
    json_object *message_array_jso)
{
    int iRet;
    REQUEST_INFO_S *new_resource_info = NULL;
    gchar *uri_lower = NULL;
    json_object *resource_jso = NULL;
    json_object *redfish_jso = NULL;
    gint32 ret;
    json_bool ret_json;
    json_object *header_jso = NULL;

    
    PROVIDER_PARAS_S new_param;

    (void)memset_s(&new_param, sizeof(new_param), 0, sizeof(new_param));
    (void)memcpy_s(&new_param, sizeof(PROVIDER_PARAS_S), i_param, sizeof(PROVIDER_PARAS_S));
    

    
    new_resource_info = (REQUEST_INFO_S *)g_malloc0(sizeof(REQUEST_INFO_S));
    return_val_do_info_if_fail((NULL != new_resource_info), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s %d: malloc REQUEST_INFO_S memory fail", __FUNCTION__, __LINE__));

    iRet = snprintf_s(new_resource_info->relative_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s/%s", new_param.uri,
        member_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    // 填充新生成资源request_info部分必需值
    // 转换URI信息为小写保存
    uri_lower = g_ascii_strdown(new_resource_info->relative_uri, strlen(new_resource_info->relative_uri));
    goto_label_do_info_if_fail(NULL != uri_lower, EXIT, ret = HTTP_INTERNAL_SERVER_ERROR;
        debug_log(DLOG_ERROR, "%s %d: change uri to lower fail", __FUNCTION__, __LINE__));
    (void)strncpy_s(new_resource_info->uri_lower, MAX_URI_LENGTH, uri_lower, MAX_URI_LENGTH - 1);
    g_free(uri_lower);
    uri_lower = NULL;
    debug_log(DLOG_DEBUG, "%s :uri_lower: %s\n", __FUNCTION__, new_resource_info->uri_lower);

    (void)strncpy_s(new_resource_info->headers[0].header_title, MAX_HEADER_TITLE_LEN, RESP_HEADER_X_AUTH_TOKEN,
        strlen(RESP_HEADER_X_AUTH_TOKEN));
    (void)strncpy_s(new_resource_info->headers[0].header_content, MAX_HEADER_CONTENT_LEN, member_id,
        MAX_HEADER_CONTENT_LEN - 1);

    // 根据URI获取资源配置文件路径
    ret = get_res_info_from_uri(new_resource_info, &new_param, message_array_jso);
    goto_label_do_info_if_fail(VOS_OK == ret, EXIT,
        debug_log(DLOG_ERROR, "%s %d: get_res_info_from_uri fail", __FUNCTION__, __LINE__));

    // 载入资源配置文件
    resource_jso = json_object_from_file(new_resource_info->resource_path);
    goto_label_do_info_if_fail((NULL != resource_jso), EXIT, ret = HTTP_NOT_FOUND;
        debug_log(DLOG_ERROR, "%s get resource from file fail", __FUNCTION__));

    // 获取配置文件中资源相关属性
    ret_json = json_object_object_get_ex(resource_jso, REDFISH_OBJECT, &redfish_jso);
    goto_label_do_info_if_fail(ret_json, EXIT, ret = HTTP_NOT_FOUND; json_object_put(resource_jso);
        debug_log(DLOG_ERROR, "%s get REDFISH_OBJECT from file fail", __FUNCTION__));

    // 更新资源对象属性值
    ret = generate_get_response_body(new_resource_info, &new_param, &redfish_jso, message_array_jso);
    goto_label_do_info_if_fail(VOS_OK == ret, EXIT, json_object_put(resource_jso);
        debug_log(DLOG_ERROR, "%s generate_get_response_body fail", __FUNCTION__));

    // 生成ETag响应头
    ret = generate_etag_response_header(new_resource_info->uri_lower, redfish_jso, NULL, &header_jso);
    if (VOS_OK == ret) {
        (void)json_object_array_add(*o_header_array_jso, header_jso);
    } else {
        debug_log(DLOG_ERROR, "%s %d :generate_etag_response_header error, ret is %d", __FUNCTION__, __LINE__, ret);
    }
    json_object_put(resource_jso);

EXIT:
    clear_sensitive_info(new_resource_info, sizeof(REQUEST_INFO_S));
    g_free(new_resource_info);
    

    return ret;
}



gint32 generate_rsc_create_response_headers(PROVIDER_PARAS_S *i_param, gchar *id, json_object **header_array_jso,
    json_object *message_array_jso)
{
    int iRet;
    gint32 ret;
    json_object *header_jso = NULL;
    gchar location[MAX_URI_LENGTH] = {0};
    json_object *message_info_jso = NULL;

    // 生成公共响应头
    ret = generate_common_response_headers(header_array_jso, i_param->uri, message_array_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: generate_common_response_headers error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso));

    // 生成Location头
    iRet = snprintf_s(location, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s/%s", i_param->uri, id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    header_jso = json_object_new_object();
    json_object_object_add(header_jso, RESP_HEADER_LOCATION, json_object_new_string((const gchar *)location));
    clear_sensitive_info(location, sizeof(location));

    ret = json_object_array_add(*header_array_jso, header_jso);
    
    return_val_do_info_if_expr(0 != ret, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(header_jso);
        debug_log(DLOG_ERROR, "%s:json_object_array_add error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(message_array_jso, message_info_jso));
    
    // 非WEB请求生成ETag头，生成失败不阻塞正常返回
    if (!i_param->is_from_webui) {
        (void)generate_post_rsc_etag(i_param, id, header_array_jso, message_array_jso);
    }

    return ret;
}


const gchar *get_rf_event_health_string(guchar event_type, guchar severity_level)
{
    const gchar*    health_string[] = {RF_STATUS_OK, RF_STATUS_WARNING, RF_STATUS_CRITICAL};
    guint32 health_str_index;

    if (event_type > EVENT_TYPE_DFT) {
        debug_log(DLOG_ERROR, "unknown event type:%d", event_type);

        return NULL;
    }

    if (severity_level > HEALTH_CRITICAL) {
        debug_log(DLOG_ERROR, "unknown health severity level:%d", severity_level);

        return NULL;
    }

    switch (event_type) {
            
        case EVENT_TYPE_SYS:
        case EVENT_TYPE_DFT:
            
            health_str_index = (severity_level > 1) ? severity_level - 1 : severity_level;
            
            break;

        default:
            health_str_index = severity_level;
            break;
    }

    if (health_str_index >= G_N_ELEMENTS(health_string)) {
        debug_log(DLOG_ERROR, "wrong event type with severity level, event type:%d, severity level:%d", event_type,
            severity_level);

        return NULL;
    }

    return health_string[health_str_index];
}


LOCAL gchar *getdirname(const gchar *path)
{
    gchar *p = (gchar *)strrchr(path, '/');
    gint32 sz;

    if (p) {
        sz = p - path;
        p = strdup(path);
        if (p != NULL) {
            p[sz] = '\0';
        }
    }

    return p;
}


LOCAL void mkdir_recursive_mode(const gchar *path)
{
    if (path == NULL) {
        return;
    }

    gchar *d = getdirname(path);
    mode_t mode = 0777;

    if (d) {
        mkdir_recursive_mode(d);
        
        free(d);
    }

    if (vos_get_file_accessible(path) != TRUE) {
        (void)mkdir(path, mode);
        (void)chown(path, REDFISH_USER_UID, APPS_USER_GID);
        (void)chmod(path, 0744); // 目录权限除属主以外，其他用户可读，故权限为0744
    }

    return;
}


json_object *get_resource_cfg_json(const gchar* relative_file_name)
{
    gchar resource_path[MAX_PATH_LENGTH + 1] = {0};
    gchar tmp_resource_path[MAX_PATH_LENGTH + 1] = {0};
 
    // 拼接在内存文件系统中的模板文件文件名
    gint32 ret = snprintf_s(tmp_resource_path, sizeof(tmp_resource_path), sizeof(tmp_resource_path) - 1, "%s%s",
        DEV_TMP_DIR, relative_file_name);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s : snprintf_s relative_file_name fail", __FUNCTION__);
        return NULL;
    }
 
    // 如果文件在内存文件系统中存在，则直接从该文件系统中读取文件
    if (vos_get_file_accessible(tmp_resource_path)) {
        return json_object_from_file(tmp_resource_path);
    }
 
    // 获取模板文件的目录名
    gchar origin_file_path[MAX_FILEPATH_LENGTH + 1] = {0}; // 文件相对路径的目录
    if (vos_get_file_dir(tmp_resource_path, origin_file_path, sizeof(origin_file_path) - 1) != TRUE) {
        debug_log(DLOG_DEBUG, "%s : get file directory fail", __FUNCTION__);
        return NULL;
    }
    
    size_t dir_len = strlen(origin_file_path);
    if (origin_file_path[dir_len - 1] == '/') {
        origin_file_path[dir_len - 1] = 0;
    }
    // 如果目录不存在则需要创建
    if (g_file_test(origin_file_path, G_FILE_TEST_EXISTS) != TRUE) {
        (void)mkdir_recursive_mode(origin_file_path);
    }

    // 获取flash文件系统的模板文件名称
    ret = snprintf_s(resource_path, sizeof(resource_path), sizeof(resource_path) - 1, "%s%s",
        CONFG_FILE_PREFIX, relative_file_name);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s : snprintf_s resource_path fail", __FUNCTION__);
        return NULL;
    }
 
    // 将模板文件从flash拷贝到内存中
    vos_file_copy(tmp_resource_path, resource_path);
    (void)chown(tmp_resource_path, REDFISH_USER_UID, APPS_USER_GID);
    (void)chmod(tmp_resource_path, 0644); // 拷贝到内存文件系统中的文件权限为0644

    return json_object_from_file(tmp_resource_path);
}


LOCAL json_object *get_resource_json(const gchar* uri_lower)
{
    gchar relative_file_name[MAX_PATH_LENGTH + 1] = {0};

    // 生成redfish模板文件的相对路径
    gint32 ret = snprintf_s(relative_file_name, sizeof(relative_file_name), sizeof(relative_file_name) - 1, "%s/%s",
        uri_lower, CONFG_FILE_SUFFIX);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s : snprintf_s tmp_resource_path fail", __FUNCTION__);
        return NULL;
    }
 
    return get_resource_cfg_json(relative_file_name);
}


LOCAL gint32 _get_rsc_json_data_from_provider(PROVIDER_PARAS_S *i_paras, const gchar *rsc_uri,
    RESOURCE_PROVIDER_S *rsc_provider, json_object **o_rsc_jso)
{
    gint32 ret;
    gchar *uri_lower = NULL;
    json_object *rsc_cfg_jso = NULL;
    json_object *rsc_jso = NULL;
    json_object *message_array_jso = NULL;
    get_provider_info_function resource_provider_fn;
    PROPERTY_PROVIDER_S *prop_provider = NULL;
    guint32 count;
    gchar *rsp_str = NULL;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != rsc_uri) && (NULL != rsc_provider) && (NULL != o_rsc_jso),
        VOS_ERR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    uri_lower = g_ascii_strdown(rsc_provider->resource_path, strlen(rsc_provider->resource_path));
    return_val_do_info_if_fail(NULL != uri_lower, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get lower string failed", __FUNCTION__));

    rsc_cfg_jso = get_resource_json(uri_lower);
    g_free(uri_lower);
    if (rsc_cfg_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: get_resource_json fail", __FUNCTION__);
        return RET_ERR;
    }

    (void)json_object_object_get_ex(rsc_cfg_jso, REDFISH_OBJECT, &rsc_jso);
    return_val_do_info_if_fail(NULL != rsc_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "get rsc tempalte fail");
        (void)json_object_put(rsc_cfg_jso));

    (void)json_object_get(rsc_jso);
    (void)json_object_put(rsc_cfg_jso);

    if (NULL != rsc_provider->do_get) {
        
        ret = fill_odata_head_info(rsc_uri, rsc_jso);
        do_val_if_fail(RF_OK == ret, debug_log(DLOG_INFO, "fill odata head info failed, ret is %d", ret));
        

        message_array_jso = json_object_new_array();
        return_val_do_info_if_fail(NULL != message_array_jso, VOS_ERR,
            debug_log(DLOG_ERROR, "alloc new json array failed");
            (void)json_object_put(rsc_jso));

        ret = rsc_provider->do_get(i_paras, rsc_jso, message_array_jso, &rsp_str);
        (void)json_object_put(message_array_jso);
        if (rsp_str != NULL) {
            g_free(rsp_str);
        }

        return_val_do_info_if_fail(RF_OK == ret, VOS_ERR,
            debug_log(DLOG_ERROR, "get rsc for %s failed, ret is %d", rsc_uri, ret);
            (void)json_object_put(rsc_jso));
    } else {
        resource_provider_fn = rsc_provider->resource_provider_fn;
        return_val_do_info_if_fail(NULL != resource_provider_fn, VOS_ERR,
            debug_log(DLOG_ERROR, "get rsc json data failed, no resource provider found for %s", rsc_uri);
            (void)json_object_put(rsc_jso));

        rf_provider_paras_free(i_paras);
        ret = resource_provider_fn(i_paras, &prop_provider, &count);
        return_val_do_info_if_fail((VOS_OK == ret) || (HTTP_OK == ret), ret,
            debug_log(DLOG_ERROR, "rsc for %s doesn't exist", rsc_uri);
            (void)json_object_put(rsc_jso));

        message_array_jso = json_object_new_array();
        return_val_do_info_if_fail(NULL != message_array_jso, VOS_ERR,
            debug_log(DLOG_ERROR, "alloc new json array failed");
            (void)json_object_put(rsc_jso));
        ret = provider_get_prop_values(i_paras, &rsc_jso, message_array_jso, prop_provider, count);
        (void)json_object_put(message_array_jso);

        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_ERROR, "get rsc for %s with property provider failed, ret is %d", rsc_uri, ret);
            (void)json_object_put(rsc_jso));
    }

    *o_rsc_jso = rsc_jso;

    return VOS_OK;
}


gint32 rf_validate_rsc_exist(const gchar *rsc_uri, json_object **rsc_jso)
{
    errno_t safe_fun_ret;
    gint32 ret;
    RESOURCE_PROVIDER_S *resource_provider = NULL;
    get_provider_info_function resource_provider_fn;
    PROVIDER_PARAS_S i_paras;
    gchar *member_id_index = NULL;
    gchar rsc_uri_dup[MAX_URI_LENGTH] = {0};
    PROPERTY_PROVIDER_S *provider = NULL;
    guint32 count;
    gchar *uri_lower = NULL;

    if (rsc_uri == NULL) {
        debug_log(DLOG_ERROR, "%s failed:rsc_uri is NULL", __FUNCTION__);
        return VOS_ERR;
    }

    size_t len = strlen(rsc_uri);
    if (len >= MAX_URI_LENGTH) {
        debug_log(DLOG_ERROR, "%s failed:rsc_uri length is too long", __FUNCTION__);
        return VOS_ERR;
    }

    uri_lower = g_ascii_strdown(rsc_uri, (gssize)len);
    return_val_do_info_if_fail((NULL != uri_lower), VOS_ERR,
        debug_log(DLOG_ERROR, "trans %s to lower failed", rsc_uri));

    safe_fun_ret = strncpy_s(rsc_uri_dup, sizeof(rsc_uri_dup), uri_lower, sizeof(rsc_uri_dup) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    g_free(uri_lower);

    redfish_trim_uri(rsc_uri_dup, sizeof(rsc_uri_dup));

    
    ret = get_resource_provider(rsc_uri_dup, &resource_provider);
    if (VOS_OK != ret) {
        debug_log(DLOG_MASS, "%s failed:cann't get resource provider for %s", __FUNCTION__, rsc_uri_dup);

        return VOS_ERR;
    }

    (void)memset_s(&i_paras, sizeof(PROVIDER_PARAS_S), 0, sizeof(PROVIDER_PARAS_S));

    (void)strncpy_s(i_paras.uri, MAX_URI_LENGTH, rsc_uri_dup, MAX_URI_LENGTH - 1);
    (void)strncpy_s(i_paras.user_name, SESSION_USER_NAME_MAX_LEN + 1, NA_STR, strlen(NA_STR));
    (void)strncpy_s(i_paras.client, SESSION_IP_LEN, INNER_IP_ADDR, strlen(INNER_IP_ADDR));
    i_paras.is_satisfy_privi = 1;
    i_paras.user_role_privilege = 0xff;
    i_paras.skip = 0;
    i_paras.top = INVALID_INTEGER_VALUE;

    member_id_index = g_strrstr(rsc_uri_dup, SLASH_FLAG_STR);
    return_val_do_info_if_fail(NULL != member_id_index, VOS_ERR,
        debug_log(DLOG_ERROR, "get member id from %s failed", rsc_uri_dup));

    (void)strncpy_s(i_paras.member_id, MAX_MEM_ID_LEN, member_id_index + 1, MAX_MEM_ID_LEN - 1);

    
    resource_provider_fn = resource_provider->resource_provider_fn;

    if (resource_provider_fn) {
        ret = resource_provider_fn(&i_paras, &provider, &count);
        
        if ((VOS_OK != ret) && (HTTP_OK != ret)) {
            rf_provider_paras_free(&i_paras);
            debug_log(DLOG_MASS, "rsc at %s doesn't exist, ret is %d", rsc_uri, ret);
            return VOS_ERR;
        }
        
    }

    
    return_val_do_info_if_expr(NULL == rsc_jso, VOS_OK, rf_provider_paras_free(&i_paras));
    ret = _get_rsc_json_data_from_provider(&i_paras, rsc_uri, resource_provider, rsc_jso);
    
    (void)rf_provider_paras_free(&i_paras);
    return ret;
    
}

gint32 get_location_devicename(OBJ_HANDLE component_handle, gchar *string_value, gint32 string_value_len)
{
    gint32 ret;
    gchar component_location[FWINV_STR_MAX_LEN] = {0};
    gchar component_devicename[FWINV_STR_MAX_LEN] = {0};

    ret = dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_LOCATION, component_location,
        FWINV_STR_MAX_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s, %d:  dal_get_property_value_string fail\n", __FUNCTION__, __LINE__));
    ret = dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_DEVICE_NAME, component_devicename,
        FWINV_STR_MAX_LEN);
    
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s, %d:  dal_get_property_value_string fail\n", __FUNCTION__, __LINE__));
    

    dal_clear_string_blank(component_location, sizeof(component_location));
    dal_clear_string_blank(component_devicename, sizeof(component_devicename));

    ret = snprintf_s(string_value, string_value_len, string_value_len - 1, "%s%s", component_location,
        component_devicename);
    return_val_do_info_if_expr(VOS_OK >= ret, VOS_ERR, debug_log(DLOG_ERROR, "%s:snprintf_s fail.", __FUNCTION__));

    return VOS_OK;
}



gint32 provider_get_prop_values(PROVIDER_PARAS_S *provider_param, json_object **o_resource_jso,
    json_object *o_message_array_jso, const PROPERTY_PROVIDER_S *provider, const guint32 count)
{
    gint32 ret = VOS_OK;
    json_object *prop_value_jso = NULL;
    json_object *prop_temp_jso = NULL;
    json_object *message_info = NULL;
    guint32 array_index;
    json_bool b_ret;
    gint32 sleep_count = 0; // 当获取多个属性时，需要适时的释放CPU以降低CPU占用率

    // 拷贝生成属性值获取接口层入参信息
    for (array_index = 0; array_index < count; array_index++) {
        if (NULL != provider[array_index].pfn_get) {
            sleep_count++;
            if (sleep_count % 10 == 0) { // 每十个资源，暂停获取，短暂释放CPU
                (void)vos_task_delay(10);
            }
            // 得到是否满足权限标识，传递给下层接口使用
            provider_param->is_satisfy_privi =
                provider_param->user_role_privilege & (guint16)provider[array_index].require_privilege;
            provider_param->index = array_index;
            message_info = NULL;
            prop_value_jso = NULL;

            
            (void)json_object_object_get_ex(*o_resource_jso, provider[array_index].property_name, &prop_value_jso);
            prop_temp_jso = prop_value_jso;
            ret = provider[array_index].pfn_get(provider_param, &message_info, &prop_value_jso);
            // 正常返回，且存在属性值对象，则更新
            // 正常返回，不存在属性值对象，则采用配置属性值
            if (HTTP_OK == ret) {
                
                // prop_value_jso等于prop_temp_jso时，其引用计数必须大于1，防止后续调用json_object_object_add异常释放
                // prop_value_jso不等于prop_temp_jso时，不需要判断引用计数，可以直接后续调用
                if (NULL != prop_value_jso &&
                    ((prop_temp_jso != prop_value_jso) || ((prop_temp_jso == prop_value_jso) &&
                    (prop_value_jso->_ref_count > 1)))) { // 此处保证接口资源属性有更新才add，否则不add
                                                          
                    json_object_object_add(*o_resource_jso, provider[array_index].property_name, prop_value_jso);
                }
            }
            // 内部错误，获取属性值失败，则更新配置属性值为null
            else if (HTTP_INTERNAL_SERVER_ERROR == ret) {
                

                debug_log(DLOG_MASS, "%s %d: pfn_get fail", __FUNCTION__, __LINE__);

                b_ret = json_object_has_key(*o_resource_jso, provider[array_index].property_name);
                do_val_if_expr(TRUE == b_ret,
                    json_object_object_add(*o_resource_jso, provider[array_index].property_name, NULL));

                
            }
            // 如果为其他失败场景，直接返回错误消息
            else {
                
                // 防止下层接口异常场景仍然返回值信息(确保prop_value_jso不是模板数据再释放内存)
                do_if_expr(NULL != prop_value_jso && prop_value_jso != prop_temp_jso, json_object_put(prop_value_jso));
                

                
                if (NULL != message_info) {
                    (void)json_object_array_add(o_message_array_jso, message_info);
                } else {
                    debug_log(DLOG_MASS, "%s %d: message_info is null", __FUNCTION__, __LINE__);
                }

                
                return ret;
            }

            // 防止下层接口多生成错误消息的场景，此处释放以免内存泄露(理论不应该出现)
            if (NULL != message_info) {
                json_object_put(message_info);
                message_info = NULL;
            }
        }
    }

    return VOS_OK;
}



LOCAL gint32 _rsc_delete_property(json_object *object, const gchar *del_key)
{
    gint32 array_len = 0;
    gint32 array_index = 0;

    return_val_if_fail((NULL != object) && (NULL != del_key), VOS_ERR);

    if (json_type_array == json_object_get_type(object)) {
        array_len = json_object_array_length(object);

        for (array_index = 0; array_index < array_len; array_index++) {
            (void)json_object_object_del_ex(json_object_array_get_idx(object, array_index), del_key);
        }

        return VOS_OK;
    } else {
        (void)json_object_object_del(object, del_key);
        return VOS_OK;
    }
}


gint32 redfish_check_uri_type(const gchar *uri, gint32 *type)
{
    gchar uri_temp[MAX_URL_LEN] = {0};
    errno_t temp;

    return_val_do_info_if_expr((NULL == uri) || (NULL == type), VOS_ERR,
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    
    temp = strncpy_s(uri_temp, sizeof(uri_temp), uri, strlen(uri));
    
    return_val_do_info_if_fail(EOK == temp, VOS_ERR, debug_log(DLOG_ERROR, "%s : strncpy_s fail.", __FUNCTION__));

    if ((NULL == strstr(uri_temp, FILE_URL_HTTPS_HEAD)) && (NULL == strstr(uri_temp, FILE_URL_SFTP_HEAD)) &&
        (NULL == strstr(uri_temp, FILE_URL_NFS_HEAD)) && (NULL == strstr(uri_temp, FILE_URL_CIFS_HEAD)) &&
        (NULL == strstr(uri_temp, FILE_URL_SCP_HEAD))) {
        *type = URI_TYPE_LOCAL;
    } else {
        *type = URI_TYPE_REMOTE;
    }

    return VOS_OK;
}


gint32 rsc_del_none_etag_affected_property(json_object *object, SPECIAL_PROP_S *prop_config, guint32 config_count)
{
    json_object *tmp_obj_s = NULL;
    json_object *tmp_obj = NULL;

    gint32 depth = 0;
    gint32 temp_id;
    guint8 software_type = 0;

    if (NULL == object) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }
    (void)dal_get_software_type(&software_type);
    for (temp_id = 0; prop_config[temp_id].num != 0; temp_id++) {
        tmp_obj_s = object;

        for (depth = 0; depth < prop_config[temp_id].num && depth < MAX_PROP_DEPTH; depth++) {
            if (depth == (prop_config[temp_id].num - 1)) {
                (void)_rsc_delete_property(tmp_obj_s, prop_config[temp_id].prop[depth]);
                break;
            }

            if (TRUE != json_object_object_get_ex(tmp_obj_s, prop_config[temp_id].prop[depth], &tmp_obj)) {
                break;
            }

            tmp_obj_s = tmp_obj;
            
            if (MGMT_SOFTWARE_TYPE_EM == software_type) {
                if (json_type_array == json_object_get_type(tmp_obj_s) && depth + 1 < (prop_config[temp_id].num - 1)) {
                    tmp_obj_s = json_object_array_get_idx(tmp_obj_s, 0);
                }
            }

        }
    }

    return VOS_ERR;
}


gint32 rf_save_import_content_to_file(PROVIDER_PARAS_S *i_paras, const gchar *import_type_str, const gchar *content_str,
    gint32 cert_format_type, gchar *file_path, guint32 file_path_len, guchar *cert_store_type,
    json_object **o_message_jso)
{
    gboolean b_ret = 0;
    gint32 ret = 0;
    errno_t str_ret = EOK;
    gchar               download_url_buf[MAX_URL_LEN] = {0};
    GSList *input_list = NULL;
    const gchar *file_content = NULL;
    guchar *decode_content = NULL;
    gsize decode_content_len = (gsize)-1;
    
    const gchar *err_msg_id = NULL;

    return_val_do_info_if_fail((NULL != import_type_str) && (NULL != content_str) && (NULL != file_path) &&
        (NULL != cert_store_type) && (NULL != o_message_jso),
        VOS_ERR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));
    

    
    if (0 == g_strcmp0(import_type_str, RFACTION_FORMAT_TEXT)) {
        
        str_ret = strncpy_s(file_path, file_path_len, RF_IMPORT_CONTENT_FILE_PATH, strlen(RF_IMPORT_CONTENT_FILE_PATH));
        
        goto_label_do_info_if_fail(EOK == str_ret, err_exit,
            debug_log(DLOG_ERROR, "copy %s failed", RF_IMPORT_CONTENT_FILE_PATH));
        
        file_content = content_str;

        if (strlen(file_content) > IMPORT_CERTIFICATE_MAX_LEN) {
            debug_log(DLOG_ERROR, "%s, the import ceritificate is too long", __FUNCTION__);
            goto err_exit;
        }

        do_val_if_expr(TRUE == cert_format_type, decode_content = g_base64_decode(content_str, &decode_content_len);
            file_content = (const gchar *)decode_content);

        goto_label_do_info_if_fail(NULL != file_content, err_exit, debug_log(DLOG_ERROR, "file content is null"));

        b_ret = g_file_set_contents(file_path, file_content, (gssize)decode_content_len, NULL);
        g_free(decode_content);

        goto_label_do_info_if_fail(TRUE == b_ret, err_exit, debug_log(DLOG_ERROR, "save file content failed"));

        (void)chown(file_path, REDFISH_USER_UID, APPS_USER_GID);
        (void)chmod(file_path, 660);

        *cert_store_type = FILE_TYPE_TEXT;
        
    } else if (0 == g_strcmp0(import_type_str, RFACTION_FORMAT_FILE)) {
        
        
        if (content_str[0] == '/') {
            errno_t safe_fun_ret = strncpy_s(file_path, file_path_len, content_str, file_path_len - 1);
            do_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            *cert_store_type = FILE_TYPE_LOCAL;
            return VOS_OK;
        }

        *cert_store_type = FILE_TYPE_URI;

        ret = snprintf_s(download_url_buf, MAX_URL_LEN, MAX_URL_LEN - 1, "download;%d;%s", FILE_ID_CERTIFICATE,
            content_str);
        goto_label_do_info_if_fail(ret > 0, err_exit,
            debug_log(DLOG_ERROR, "format certificate download url failed, ret is %d", ret));

        input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)download_url_buf));
        (void)memset_s(download_url_buf, MAX_URL_LEN, 0, MAX_URL_LEN);
        input_list = g_slist_append(input_list, g_variant_new_string(" "));

        
        ret = uip_redfish_call_class_method_with_userinfo(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
            (i_paras->auth_type == LOG_TYPE_LOCAL), (const gchar *)i_paras->user_roleid, OBJ_HANDLE_COMMON,
            CLASS_NAME_TRANSFERFILE, METHOD_INITRIAL_FILE_TRANSFER, AUTH_DISABLE, 0, input_list, NULL);
        

        uip_free_gvariant_list(input_list);

        return_val_do_info_if_fail(VOS_OK == ret, ret,
            debug_log(DLOG_ERROR, "initial file transfer failed, ret is %d", ret);
            (void)parse_file_transfer_err_code(ret, o_message_jso));
        
    } else {
        
        goto err_exit;
        
    }

    return VOS_OK;

    
err_exit:
    err_msg_id = (CUSTOM_CERT_TYPE == cert_format_type) ? MSGID_ENCRYPTED_CERT_IMPORT_FAILED : MSGID_CERT_IMPORT_FAILED;
    (void)create_message_info(err_msg_id, NULL, o_message_jso);

    return VOS_ERR;
    
}


json_object *url_hash_table_lookup(GHashTable *url_hash_table, const gchar *key)
{
    gpointer value = g_hash_table_lookup(url_hash_table, (gconstpointer)key);
    if (value) {
        return (json_object *)value;
    } else {
        return NULL;
    }
}


gint32 url_hash_table_insert(GHashTable *url_hash_table, const gchar *key, json_object *rsc_jso)
{
    gchar *new_key = strdup(key);
    if (NULL == new_key) {
        debug_log(DLOG_ERROR, "strdup failed");
        return VOS_ERR;
    }
    g_hash_table_insert(url_hash_table, (gpointer)new_key, (gpointer)rsc_jso);
    return VOS_OK;
}


LOCAL void free_url_key(gpointer data)
{
    gchar *key = (gchar *)data;
    free(key);
}


LOCAL void free_json_value(gpointer data)
{
    json_object *j_object = (json_object *)data;
    (void)json_object_put(j_object);
}


GHashTable *url_hash_table_init(void)
{
    GHashTable *url_hash_table = NULL;
    url_hash_table = g_hash_table_new_full(g_str_hash, g_str_equal, free_url_key, free_json_value);
    if (url_hash_table == NULL) {
        debug_log(DLOG_ERROR, "new hash table failed!");
        return NULL;
    }
    return url_hash_table;
}


void url_hash_table_destroy(GHashTable *url_hash_table)
{
    g_hash_table_destroy(url_hash_table);
}


LOCAL gint32 _init_map_array(const gchar *parent_url, SPECIAL_URL_MAP_S *map_array, guint32 map_array_len,
    guint32 *map_array_index)
{
    int iRet;
    guint32 i;
    gchar board_slot[MAX_RSC_ID_LEN];
    gboolean flag_value = FALSE;

    return_val_if_expr(NULL == parent_url || NULL == map_array || NULL == map_array_index, VOS_ERR);

    (void)redfish_get_board_slot(board_slot, sizeof(board_slot));

    for (i = 0; i < map_array_len; i++) {
        switch (i) {
            case URL_INDEX_DRIVES:
                iRet = snprintf_s(map_array[i].source_url, sizeof(map_array[i].source_url),
                    sizeof(map_array[i].source_url) - 1, URI_FORMAT_DISK_PRE, board_slot);
                return_val_do_info_if_expr(iRet <= 0, VOS_ERR,
                    debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet));
                if (g_ascii_strcasecmp(parent_url, map_array[i].source_url) == 0) {
                    iRet = snprintf_s(map_array[i].target_url, sizeof(map_array[i].source_url),
                        sizeof(map_array[i].source_url) - 1, URI_FORMAT_CHASSIS, board_slot);
                    if (iRet > 0) {
                        flag_value = TRUE;
                    }
                } else {
                    debug_log(DLOG_DEBUG, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                }
                break;
            default:
                debug_log(DLOG_ERROR, "%s: invalid i = %d", __FUNCTION__, i);
                return VOS_ERR;
        }

        break_if_expr(TRUE == flag_value);
    }

    return_val_if_expr(i == map_array_len, VOS_ERR);
    *map_array_index = i;

    return VOS_OK;
}


LOCAL gint32 _special_parent_url(const gchar *parent_url, json_object **parent_jso, guint32 *o_map_array_index)
{
    
    SPECIAL_URL_MAP_S map_array[] = {
        {MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, DRIVES_MEMBERS_JSON_POINTER}
    };
    gint32 ret_val;
    guint32 map_array_index = G_MAXINT32;
    json_object *members_jso = NULL;
    gint32 segment_num;
    gchar **segment_info = NULL;

    return_val_if_expr(NULL == parent_url || NULL == parent_jso || NULL == o_map_array_index, VOS_ERR);

    ret_val = _init_map_array(parent_url, map_array, G_N_ELEMENTS(map_array), &map_array_index);
    return_val_do_info_if_fail(VOS_OK == ret_val && map_array_index < G_N_ELEMENTS(map_array), VOS_ERR,
        debug_log(DLOG_DEBUG, "%s: _init_map_array failed", __FUNCTION__));

    *parent_jso = NULL;

    ret_val = rf_validate_rsc_exist(map_array[map_array_index].target_url, parent_jso);
    return_val_do_info_if_fail(VOS_OK == ret_val, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s:validate parent rsc exist for %s failed, ret is %d", __FUNCTION__,
        map_array[map_array_index].target_url, ret_val));

    segment_info = g_strsplit(map_array[map_array_index].members_json_pointer, SLASH_FLAG_STR, -1);
    
    return_val_do_info_if_expr(NULL == segment_info, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s: g_strsplit failed", __FUNCTION__);
        json_object_put(*parent_jso));
    

    segment_num = g_strv_length(segment_info);
    ret_val = get_property_obj_from_segments(*parent_jso, segment_info, segment_num, &members_jso);
    return_val_do_info_if_fail(TRUE == ret_val, VOS_ERR, g_strfreev(segment_info); json_object_put(*parent_jso);
        debug_log(DLOG_DEBUG, "%s: get_property_obj_from_segments failed", __FUNCTION__));

    g_strfreev(segment_info);

    json_object_object_add(*parent_jso, RFPROP_MEMBERS, json_object_get(members_jso));
    *o_map_array_index = map_array_index;

    return VOS_OK;
}


LOCAL gint32 _is_valid_url(guint32 map_array_index, const gchar *url)
{
    gint32 ret_val = VOS_ERR;
    OBJ_HANDLE obj_handle = 0;

    return_val_if_expr(NULL == url, VOS_ERR);

    switch (map_array_index) {
        case URL_INDEX_DRIVES:
            ret_val = redfish_check_every_drives_get_handle(CLASS_HDD_NAME, url, &obj_handle);
            break;

        default:
            debug_log(DLOG_ERROR, "%s: invalid map_array_index= %d", __FUNCTION__, map_array_index);
    }

    return ret_val;
}


LOCAL void foreach_members(json_object* members_jso, GSList** uri_list, guint32 map_array_index,
    const gchar* iter_string, const gchar* pattern_id_index, gint32* iter_index)
{
    json_object*        jso_iter = NULL;
    const gchar*        member_uri = NULL;
    gchar*              id_index = NULL;
    gchar               formatted_uri_buf[MAX_URI_LENGTH] = {0};
    gchar*              uri_path_dup = NULL;
    gint32 ret;

    json_object_array_foreach(members_jso, jso_iter) {
        member_uri = NULL;
        (void)get_element_str(jso_iter, RFPROP_ODATA_ID, &member_uri);
        if (member_uri == NULL) {
            debug_log(DLOG_ERROR, "cann't get property %s from %s", RFPROP_ODATA_ID, dal_json_object_get_str(jso_iter));
            continue;
        }

        // 从Managers集合资源中获取当前在位ID集合，替换Chassis通配中的ID
        if (strcmp(iter_string, URI_CHASSIS) == 0) {
            id_index = g_strstr_len(member_uri, -1, URI_MANAGERS);
            if (id_index == NULL) {
                debug_log(DLOG_ERROR, "cann't get %s path from %s", iter_string, member_uri);
                continue;
            }
            ret = snprintf_s(formatted_uri_buf, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s%s%s", iter_string,
                id_index + strlen(URI_MANAGERS), pattern_id_index + strlen(RF_PATTERN_ID));
        } else {
            id_index = g_strstr_len(member_uri, -1, iter_string);
            if (id_index == NULL) {
                debug_log(DLOG_ERROR, "cann't get %s path from %s", iter_string, member_uri);
                continue;
            }
            ret = snprintf_s(formatted_uri_buf, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s%s%s",
                iter_string, id_index + strlen(iter_string), pattern_id_index + strlen(RF_PATTERN_ID));
        }

        if (ret <= 0) {
            debug_log(DLOG_ERROR, "format uri string failed, ret is %d", ret);
            continue;
        }
            
        if (map_array_index != G_MAXUINT32) {
            if (_is_valid_url(map_array_index, formatted_uri_buf) != VOS_OK) {
                continue;
            }
        }
            

        uri_path_dup = g_strdup((const gchar*)formatted_uri_buf);
        if (uri_path_dup == NULL) {
            debug_log(DLOG_ERROR, "dup string for %s failed", formatted_uri_buf);
            continue;
        }

        *uri_list = g_slist_insert(*uri_list, (gpointer)uri_path_dup, *iter_index);
        *iter_index += 1;
    }
    return;
}

LOCAL json_object *get_parent_rsc_jso(GHashTable *hash_table, gchar *iter_string,
                                      guint32 *map_array_index, gchar *pattern_id_index)
{
    json_object *parent_rsc_jso = NULL;
    const gchar *managers = URI_MANAGERS;
    const gchar *local_iter_string = iter_string;

    // chassis的ID集合包含了所有板子ID（包括不在位），managers的ID集合只包含在位的板子ID
    // 替换ID时，只考虑在位板子的ID，因此用manager ID集合替换chassis ID集合
    if (strncmp(iter_string, CHASSIS_PATTERN, strlen(CHASSIS_PATTERN)) == 0) {
        local_iter_string = managers;
    }

    
    *pattern_id_index = '\0';

    
    parent_rsc_jso = url_hash_table_lookup(hash_table, local_iter_string);
    if (parent_rsc_jso != NULL) {
        return json_object_get(parent_rsc_jso);
    } else if (rf_validate_rsc_exist(local_iter_string, &parent_rsc_jso) == VOS_OK) {
        url_hash_table_insert(hash_table, local_iter_string, parent_rsc_jso);
        return json_object_get(parent_rsc_jso);
    } else if (_special_parent_url(iter_string, &parent_rsc_jso, map_array_index) == VOS_OK) {
        return parent_rsc_jso;
    } else {
        debug_log(DLOG_DEBUG, "validate parent rsc exist for %s failed", iter_string);
    }
    return NULL;
}

LOCAL void _store_formatted_uri_to_list(GSList** uri_list, GHashTable *hash_table, guint8 *recursive_depth)
{
#define ALLOW_MAX_RECURSIVE_DEPTH 15
    GSList*             list_iter = NULL;
    gint32              iter_index = 0;
    gchar*              iter_string = NULL;
    gchar*              pattern_id_index = NULL;
    json_object         *parent_rsc_jso = NULL;
    json_object*        members_jso = NULL;
    gboolean            continue_search_flag = FALSE;
    guint32             map_array_index = G_MAXUINT32;

    if (uri_list == NULL) {
        return;
    }

    if (*recursive_depth > ALLOW_MAX_RECURSIVE_DEPTH) {
        debug_log(DLOG_ERROR, "%s: current recursive depth %u reaches threshold", __FUNCTION__, *recursive_depth);
        return;
    } else {
        *recursive_depth += 1;
    }

    
    for (list_iter = *uri_list; list_iter != NULL; iter_index += 1, list_iter = g_slist_nth(*uri_list, iter_index)) {
        iter_string = (gchar *)list_iter->data;
        pattern_id_index = g_strstr_len(iter_string, -1, RF_PATTERN_ID);
        if (pattern_id_index == NULL) {
            continue;
        }

        parent_rsc_jso = get_parent_rsc_jso(hash_table, iter_string, &map_array_index, pattern_id_index);
        if (parent_rsc_jso == NULL) {
            continue;
        }

        continue_search_flag = TRUE;
        (void)json_object_object_get_ex(parent_rsc_jso, RFPROP_MEMBERS, &members_jso);
        if ((members_jso == NULL) || (json_type_array != json_object_get_type(members_jso))) {
            (void)json_object_put(parent_rsc_jso);
            parent_rsc_jso = NULL;
            debug_log(DLOG_ERROR, "can't find %s info for rsc %s, or isn't array type", RFPROP_MEMBERS, iter_string);
            continue;
        }

        
        *uri_list = g_slist_delete_link(*uri_list, list_iter);

        debug_log(DLOG_DEBUG, "delete %s from list, cur list length:%d", iter_string, g_slist_length(*uri_list));

        foreach_members(members_jso, uri_list, map_array_index, iter_string, pattern_id_index, &iter_index);
        map_array_index = G_MAXUINT32;
        
        g_free(iter_string);
        if (parent_rsc_jso != NULL) {
            (void)json_object_put(parent_rsc_jso);
            parent_rsc_jso = NULL;
        }
    }

    
    if (continue_search_flag == TRUE) {
        _store_formatted_uri_to_list(uri_list, hash_table, recursive_depth);
    }

    return;
}


gint32 fill_rsc_id_for_uri_pattern(const gchar *uri_pattern, GSList **uri_list, GHashTable *hash_table)
{
    gchar *uri_dup = NULL;
    GSList *list = NULL;
    guint8 recursive_depth = 0;

    return_val_do_info_if_fail((NULL != uri_pattern) && (NULL != uri_list), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    *uri_list = NULL;

    if (NULL == g_strrstr_len(uri_pattern, -1, SERVICE_ROOT_PATH)) {
        return VOS_ERR;
    }

    uri_dup = g_strdup(uri_pattern);
    return_val_do_info_if_fail(NULL != uri_dup, VOS_ERR,
        debug_log(DLOG_ERROR, "dup string for %s failed", uri_pattern));

    list = g_slist_append(list, (gpointer)uri_dup);
    _store_formatted_uri_to_list(&list, hash_table, &recursive_depth);

    debug_log(DLOG_DEBUG, "uri pattern :%s, list length:%d", uri_pattern, g_slist_length(list));

    *uri_list = list;

    return VOS_OK;
}


gint32 rf_resolve_action_name_from_rsc_jso(json_object *resource_jso, const gchar *action_path,
    gchar *resolved_action_name, guint32 action_name_len)
{
    json_object *tmp2_jso = NULL;

    gchar buf[MAX_ACTION_NAME_LEN] = {0};
    gchar buf2[MAX_ACTION_NAME_LEN] = {0};
    gchar *flag = NULL;

    return_val_do_info_if_fail((NULL != resource_jso) && (NULL != action_path) && (NULL != resolved_action_name),
        RF_FAILED, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    gint32 ret = snprintf_s(buf, MAX_ACTION_NAME_LEN, MAX_ACTION_NAME_LEN - 1, "%s", action_path);
    return_val_do_info_if_expr(ret < 0, RF_FAILED,
        debug_log(DLOG_ERROR, "%s %d: snprintf_s fail", __FUNCTION__, __LINE__));

    gchar *tmp = buf;

    json_object *tmp1_jso = resource_jso;

    
    while (NULL != tmp) {
        if ((flag = g_strstr_len(tmp, strlen(tmp), "/")) == NULL) {
            break;
        }
        *flag = '\0';

        (void)json_object_object_case_get(tmp1_jso, tmp, NULL, &tmp2_jso);
        return_val_do_info_if_expr(tmp2_jso == NULL, RF_ACTION_NOT_SUPPORTED,
            debug_log(DLOG_DEBUG, "%s: no action found, action name is %s", __FUNCTION__, action_path));

        tmp1_jso = tmp2_jso;
        tmp2_jso = NULL;

        tmp = flag + 1;
        flag = NULL;
    }

    
    if (NULL != tmp) {
        ret = snprintf_s(buf2, MAX_ACTION_NAME_LEN, MAX_ACTION_NAME_LEN - 1, "#%s", tmp);
        return_val_do_info_if_expr(ret < 0, RF_FAILED,
            debug_log(DLOG_ERROR, "%s %d: snprintf_s fail", __FUNCTION__, __LINE__));

        (void)json_object_object_get_ex(tmp1_jso, (const gchar *)buf2, &tmp2_jso);
        return_val_do_info_if_expr(NULL == tmp2_jso, RF_ACTION_NOT_SUPPORTED,
            debug_log(DLOG_DEBUG, "%s: no action found, action name is %s", __FUNCTION__, action_path));
        tmp = NULL;
    }

    
    tmp = g_ascii_strdown(action_path, strlen(action_path));
    return_val_do_info_if_expr(NULL == tmp, RF_FAILED,
        debug_log(DLOG_ERROR, "%s %d: change uri to lower fail", __FUNCTION__, __LINE__));

    ret = snprintf_s(buf, MAX_ACTION_NAME_LEN, MAX_ACTION_NAME_LEN - 1, "%s", action_path);
    return_val_do_info_if_expr(
        ret < 0, RF_FAILED, debug_log(DLOG_ERROR, "%s %d: snprintf_s fail", __FUNCTION__, __LINE__); g_free(tmp));

    if (NULL != (flag = g_strrstr(tmp, "actions/"))) {
        return_val_do_info_if_expr(action_name_len == 0, RF_FAILED, g_free(tmp); tmp = NULL;
            debug_log(DLOG_ERROR, "%s failed:input param error, action_name_len is 0.", __FUNCTION__));
        ret = snprintf_s(resolved_action_name, action_name_len, action_name_len - 1, "%s",
            &buf[flag - tmp + strlen("actions/")]);
        return_val_do_info_if_expr(
            ret < 0, RF_FAILED, debug_log(DLOG_ERROR, "%s: snprintf_s fail", __FUNCTION__); g_free(tmp); tmp = NULL);

        debug_log(DLOG_DEBUG, "[%s]: Action: %s", __FUNCTION__, resolved_action_name);
    }

    g_free(tmp);

    return RF_OK;
}


gint32 rf_validate_action_uri(const gchar *action_uri, GHashTable *hash_table)
{
    gchar               action_name[MAX_ACTION_NAME_LEN] = {0};
    gchar *action_flag = NULL;
    gint32 ret;
    json_object *rsc_jso = NULL;
    gchar               action_uri_dup[MAX_URI_LENGTH] = {0};
    errno_t str_ret;

    return_val_do_info_if_fail((NULL != action_uri), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    str_ret = strncpy_s(action_uri_dup, MAX_URI_LENGTH, action_uri, strlen(action_uri));
    return_val_do_info_if_fail(EOK == str_ret, VOS_ERR, debug_log(DLOG_ERROR, "copy string %s failed", action_uri));

    action_flag = g_strrstr(action_uri_dup, OEM_ACTION_SEGMENT);
    if (NULL == action_flag) {
        action_flag = g_strrstr(action_uri_dup, ACTION_SEGMENT);
    }

    return_val_do_info_if_fail(NULL != action_flag, VOS_ERR,
        debug_log(DLOG_ERROR, "validate action uri %s failed:cann't find action segment", action_uri));

    
    *action_flag = '\0';

    
    action_flag++;

    // 调接口之前，先查hash表，找到后，直接使用
    rsc_jso = url_hash_table_lookup(hash_table, action_uri_dup);
    if (NULL == rsc_jso) {
        ret = rf_validate_rsc_exist(action_uri_dup, &rsc_jso);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_DEBUG, "valid action uri %s failed, rsc at uri %s doesn't exist", action_uri,
            action_uri_dup));
        url_hash_table_insert(hash_table, action_uri_dup, rsc_jso);
    }

    ret = rf_resolve_action_name_from_rsc_jso(rsc_jso, action_flag, action_name, MAX_ACTION_NAME_LEN);
    if (RF_OK == ret) {
        ret = VOS_OK;
    } else {
        debug_log(DLOG_DEBUG, "validate action uri %s failed, ret is %d", action_uri, ret);
    }

    return ret;
}


gint32 rf_check_array_length_valid(json_object *array, gint32 limit_length, json_object **message_jso,
    const gchar *json_pointer)
{
    gint32 array_len;
    gint32 i;
    gint32 deleted_prop = FALSE;
    gint32 null_object_num = 0;
    json_object *item = NULL;

    return_val_do_info_if_expr((NULL == array) || (NULL == message_jso) || (NULL == json_pointer), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    return_val_do_info_if_fail(json_type_array == json_object_get_type(array), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:is not json array", __FUNCTION__));

    array_len = json_object_array_length(array);

    // 空数组返回无有效成员
    return_val_do_info_if_expr(0 == array_len, VOS_ERR,
        (void)create_message_info(MSGID_ARRAY_REQUIRED_ITEM, json_pointer, message_jso, json_pointer));

    // 成员不是对象，退出
    return_val_do_info_if_fail(json_type_object == json_object_get_type(json_object_array_get_idx(array, 0)), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:is not json array", __FUNCTION__));

    // 长度大于输入的限制返回长度越界
    return_val_do_info_if_expr(array_len > limit_length, VOS_ERR,
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, json_pointer, message_jso, json_pointer));

    // 遍历数组，找出空对象的个数
    for (i = 0; i < array_len; i++) {
        item = json_object_array_get_idx(array, i);
        
        continue_if_expr(NULL == item);
        
        if (0 == g_strcmp0(JSON_NULL_OBJECT_STR, dal_json_object_get_str(item))) {
            null_object_num++;

            if (item->_userdata) {
                deleted_prop = TRUE;
            }
        }
    }

    // 全是空对象，并且协议层未删除属性，产生消息，否则不产生
    if (array_len == null_object_num) {
        if (FALSE == deleted_prop) {
            (void)create_message_info(MSGID_ARRAY_REQUIRED_ITEM, json_pointer, message_jso, json_pointer);
        }

        return VOS_ERR;
    }

    return VOS_OK;
}

gboolean check_array_object(json_object *object)
{
    gint32 i;
    gint32 length;
    json_object *item_jso = NULL;
    json_object *user_date = NULL;

    if (json_type_array != json_object_get_type(object)) {
        return FALSE;
    }

    length = json_object_array_length(object);

    for (i = 0; i < length; i++) {
        item_jso = json_object_array_get_idx(object, i);
        if (TRUE == json_object_get_user_data(item_jso, (void **)&user_date) && NULL != user_date) {
            return FALSE;
        }

        if (VOS_OK != g_strcmp0(JSON_NULL_OBJECT_STR, dal_json_object_get_str(item_jso))) {
            return FALSE;
        }
    }

    return TRUE;
}


void rf_check_property_deleted(json_object *value_jso, const gchar *property_str, gboolean *flag)
{
    json_bool bool_jso = FALSE;
    GSList *prop_list = NULL;
    GSList *prop_node = NULL;

    if (NULL != value_jso) {
        bool_jso = json_object_get_user_data(value_jso, (void **)&prop_list);
        return_if_expr(!bool_jso);

        for (prop_node = prop_list; prop_node; prop_node = prop_node->next) {
            if (0 == g_strcmp0(property_str, (const gchar *)prop_node->data)) {
                (*flag) = TRUE;
                break;
            }
        }
    }
}


gint32 rf_string_check(const gchar *data)
{
    if (NULL == data || 0 == strlen(data) || 0 == g_strcmp0("N/A", data) || 0 == g_strcmp0("Unknown", data) ||
        (g_strcmp0(" Unknown", data) == 0) || 0 == g_strcmp0("NA", data)) {
        return VOS_ERR;
    }

    return VOS_OK;
}


void rf_add_property_jso_string(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso)
{
    guint32 ret;
    json_object *property_jso = NULL; // 属性 值的 json格式
    gchar prop_val[PROP_VAL_MAX_LENGTH] = {0};

    return_do_info_if_expr((NULL == property) || (NULL == rf_property) || (NULL == o_result_jso),
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    if (0 == obj_handle) {
        json_object_object_add(o_result_jso, rf_property, NULL);
        return;
    }

    
    ret = dal_get_property_value_string(obj_handle, property, prop_val, PROP_VAL_MAX_LENGTH);
    return_do_info_if_fail(VOS_OK == ret, json_object_object_add(o_result_jso, rf_property, NULL); debug_log(DLOG_ERROR,
        "%s: get %s.%s failed, ret=%d.", __FUNCTION__, dfl_get_object_name(obj_handle), property, ret));

    
    return_do_info_if_fail((rf_string_check(prop_val) == VOS_OK),
        json_object_object_add(o_result_jso, rf_property, NULL));

    property_jso = json_object_new_string((const gchar *)prop_val);
    do_if_expr(NULL == property_jso, debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__));

    json_object_object_add(o_result_jso, rf_property, property_jso);

    return;
}

void rf_add_property_jso_double(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso)
{
    gdouble prop_val = 0;
    guint32 ret;
    json_object *property_jso = NULL; // 属性 值的 json格式

    return_do_info_if_expr((NULL == property) || (NULL == rf_property) || (NULL == o_result_jso),
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    if (0 == obj_handle) {
        json_object_object_add(o_result_jso, rf_property, NULL);
        return;
    }

    
    ret = dal_get_property_value_double(obj_handle, property, &prop_val);
    return_do_info_if_fail(VOS_OK == ret, json_object_object_add(o_result_jso, rf_property, NULL); debug_log(DLOG_ERROR,
        "%s: get %s.%s failed, ret=%d.", __FUNCTION__, dfl_get_object_name(obj_handle), property, ret));

    
    return_do_info_if_expr(((0 > prop_val) || (100 < prop_val)),
        json_object_object_add(o_result_jso, rf_property, NULL));

    property_jso = ex_json_object_new_double(prop_val, "%.2f");
    do_if_expr(NULL == property_jso, debug_log(DLOG_ERROR, "%s:ex_json_object_new_double fail", __FUNCTION__));

    json_object_object_add(o_result_jso, rf_property, property_jso);

    return;
}


void rf_add_property_jso_byte(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso)
{
    guint32 ret;
    json_object *property_jso = NULL; // 属性 值的 json格式
    guchar prop_val = 0;

    return_do_info_if_expr((NULL == property) || (NULL == rf_property) || (NULL == o_result_jso),
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    if (0 == obj_handle) {
        json_object_object_add(o_result_jso, rf_property, NULL);
        return;
    }

    
    ret = dal_get_property_value_byte(obj_handle, property, &prop_val);
    return_do_info_if_fail(VOS_OK == ret, json_object_object_add(o_result_jso, rf_property, NULL); debug_log(DLOG_ERROR,
        "%s: get %s.%s failed, ret=%d.", __FUNCTION__, dfl_get_object_name(obj_handle), property, ret));

    
    
    if (INVALID_DATA_BYTE != prop_val) {
        property_jso = json_object_new_int(prop_val);
        do_if_expr(NULL == property_jso, debug_log(DLOG_ERROR, "%s:json_object_new_int fail", __FUNCTION__));
    }
    
    json_object_object_add(o_result_jso, rf_property, property_jso);

    return;
}


void rf_add_property_jso_bool(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso)
{
    json_object *property_jso = NULL; // 属性 值的 json格式
    guint8 prop_val = 0;

    return_do_info_if_expr((NULL == property) || (NULL == rf_property) || (NULL == o_result_jso),
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    if (0 == obj_handle) {
        json_object_object_add(o_result_jso, rf_property, NULL);
        return;
    }

    
    gint32 ret = dal_get_property_value_byte(obj_handle, property, &prop_val);
    return_do_info_if_fail(VOS_OK == ret, json_object_object_add(o_result_jso, rf_property, NULL); debug_log(DLOG_ERROR,
        "%s: get %s.%s failed, ret=%d.", __FUNCTION__, dfl_get_object_name(obj_handle), property, ret));

    
    if (INVALID_DATA_BYTE != prop_val) {
        property_jso = json_object_new_boolean(prop_val);
        do_if_expr(NULL == property_jso, debug_log(DLOG_ERROR, "%s:json_object_new_int fail", __FUNCTION__));
    }

    json_object_object_add(o_result_jso, rf_property, property_jso);

    return;
}


void rf_add_property_jso_presence(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso)
{
    json_object *property_jso = NULL;
    guint8 prop_val = 0;

    return_do_info_if_expr((NULL == property) || (NULL == rf_property) || (NULL == o_result_jso),
        debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    return_do_info_if_expr(0 == obj_handle, json_object_object_add(o_result_jso, rf_property, NULL));

    
    guint32 ret = dal_get_property_value_byte(obj_handle, property, &prop_val);
    return_do_info_if_fail(VOS_OK == ret, json_object_object_add(o_result_jso, rf_property, NULL);
        debug_log(DLOG_ERROR, "%s: get property value fail(%d).", __FUNCTION__, ret));

    if (TRUE == prop_val) {
        property_jso = json_object_new_boolean(TRUE);
    } else {
        property_jso = json_object_new_boolean(FALSE);
    }
    do_if_expr(NULL == property_jso, debug_log(DLOG_ERROR, "%s: json_object_new_boolean fail.", __FUNCTION__));

    json_object_object_add(o_result_jso, rf_property, property_jso);

    return;
}


void rf_add_property_jso_uint16(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso)
{
    guint16 prop_val = 0;

    return_do_info_if_expr((NULL == property) || (NULL == rf_property) || (NULL == o_result_jso),
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    return_do_info_if_expr(0 == obj_handle, json_object_object_add(o_result_jso, rf_property, NULL));

    
    gint32 ret = dal_get_property_value_uint16(obj_handle, property, &prop_val);
    return_do_info_if_fail(VOS_OK == ret, json_object_object_add(o_result_jso, rf_property, NULL); debug_log(DLOG_ERROR,
        "%s: get %s.%s failed, ret=%d.", __FUNCTION__, dfl_get_object_name(obj_handle), property, ret));

    
    return_do_info_if_expr(G_MAXUINT16 == prop_val, json_object_object_add(o_result_jso, rf_property, NULL));
    

    
    json_object *property_jso = json_object_new_int(prop_val);
    do_if_expr(NULL == property_jso, debug_log(DLOG_ERROR, "%s:json_object_new_int fail", __FUNCTION__));

    json_object_object_add(o_result_jso, rf_property, property_jso);

    return;
}


void rf_add_property_jso_int16(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso)
{
    json_object *property_jso = NULL; // 属性值的json格式
    gint16 prop_val = 0;
    guint32 ret;

    if ((property == NULL) || (rf_property == NULL) || (o_result_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__);
        return;
    }

    if (obj_handle == 0) {
        json_object_object_add(o_result_jso, rf_property, NULL);
        return;
    }

    
    ret = dal_get_property_value_int16(obj_handle, property, &prop_val);
    if (ret != VOS_OK) {
        json_object_object_add(o_result_jso, rf_property, NULL);
        debug_log(DLOG_ERROR, "%s: get %s.%s failed, ret=%d.", __FUNCTION__, dfl_get_object_name(obj_handle), property,
            ret);
        return;
    }
    if (prop_val == G_MAXINT16) {
        json_object_object_add(o_result_jso, rf_property, NULL);
        return;
    }

    
    property_jso = json_object_new_int(prop_val);
    if (property_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_int fail", __FUNCTION__);
    }

    json_object_object_add(o_result_jso, rf_property, property_jso);

    return;
}


void rf_add_property_jso_uint16_hex(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso)
{
    json_object *property_jso = NULL;
    guint16 prop_val = 0;
    guint32 ret;
    gchar prop_hex[MAX_STRBUF_LEN + 1] = {0};

    return_do_info_if_expr((NULL == property) || (NULL == rf_property) || (NULL == o_result_jso),
        debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    return_do_info_if_expr(0 == obj_handle, json_object_object_add(o_result_jso, rf_property, NULL));

    
    ret = dal_get_property_value_uint16(obj_handle, property, &prop_val);
    return_do_info_if_fail(VOS_OK == ret, json_object_object_add(o_result_jso, rf_property, NULL);
        debug_log(DLOG_ERROR, "%s: get property value fail(%d).", __FUNCTION__, ret));

    
    return_do_info_if_expr(0 == prop_val || G_MAXUINT16 == prop_val,
        json_object_object_add(o_result_jso, rf_property, NULL));
    (void)snprintf_s(prop_hex, sizeof(prop_hex), sizeof(prop_hex) - 1, "0x%04x", prop_val);

    property_jso = json_object_new_string((const gchar *)prop_hex);
    do_if_expr(NULL == property_jso, debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __FUNCTION__));

    json_object_object_add(o_result_jso, rf_property, property_jso);

    return;
}


void rf_add_prop_json_double(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso, gchar *format)
{
    guint32 ret;
    json_object *property_jso = NULL; // 属性 值的 json格式
    gdouble prop_val = 0;

    return_do_info_if_expr((NULL == property) || (NULL == rf_property) || (NULL == o_result_jso),
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    if (0 == obj_handle) {
        json_object_object_add(o_result_jso, rf_property, NULL);
        return;
    }

    
    ret = dal_get_property_value_double(obj_handle, property, &prop_val);

    return_do_info_if_fail(VOS_OK == ret, json_object_object_add(o_result_jso, rf_property, NULL); debug_log(DLOG_ERROR,
        "%s: get %s.%s failed, ret=%d.", __FUNCTION__, dfl_get_object_name(obj_handle), property, ret));
    
    return_do_info_if_expr(G_MAXUINT16 == prop_val, json_object_object_add(o_result_jso, rf_property, NULL));
    
    
    if ((prop_val > 0 && prop_val < RF_MIN_FLOAT_NUMBER) || (prop_val > -RF_MIN_FLOAT_NUMBER && prop_val < 0)) {
        prop_val = 0;
    }
    
    if (format != NULL) {
        property_jso = ex_json_object_new_double(prop_val, format);
    } else {
        property_jso = ex_json_object_new_double(prop_val, "%.2f");
    }
    do_if_expr(NULL == property_jso, debug_log(DLOG_ERROR, "%s:ex_json_object_new_double fail", __FUNCTION__));

    json_object_object_add(o_result_jso, rf_property, property_jso);

    return;
}


void rf_add_property_jso_uint32(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso)
{
    guint32 ret;
    json_object *property_jso = NULL; // 属性 值的 json格式
    guint32 prop_val = 0;

    return_do_info_if_expr((NULL == property) || (NULL == rf_property) || (NULL == o_result_jso),
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    if (0 == obj_handle) {
        json_object_object_add(o_result_jso, rf_property, NULL);
        return;
    }

    
    ret = dal_get_property_value_uint32(obj_handle, property, &prop_val);
    return_do_info_if_fail(VOS_OK == ret, json_object_object_add(o_result_jso, rf_property, NULL); debug_log(DLOG_ERROR,
        "%s: get %s.%s failed, ret=%d.", __FUNCTION__, dfl_get_object_name(obj_handle), property, ret));

    
    property_jso = json_object_new_int(prop_val);
    do_if_expr(NULL == property_jso, debug_log(DLOG_ERROR, "%s:json_object_new_int fail", __FUNCTION__));

    json_object_object_add(o_result_jso, rf_property, property_jso);

    return;
}


void rf_add_action_prop(json_object *action_prop_jso, const gchar *rsc_uri, const gchar *action_name)
{
    int ret;
    json_object *single_action_jso = NULL;
    gchar          action_name_pointer[MAX_URI_LENGTH] = {0};
    gchar *slash_flag = NULL;
    const gchar *action_flag = action_name;
    gchar          target[MAX_URI_LENGTH] = {0};
    gchar          actioninfo_uri[MAX_URI_LENGTH] = {0};

    single_action_jso = json_object_new_object();
    if (NULL == single_action_jso) {
        debug_log(DLOG_ERROR, "alloc new json object failed");

        return;
    }

    slash_flag = g_strrstr(action_name, SLASH_FLAG_STR);
    do_val_if_expr(NULL != slash_flag, (action_flag = slash_flag + 1));

    ret = snprintf_s(action_name_pointer, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "#%s", action_flag);
    return_do_info_if_expr(ret <= 0, json_object_put(single_action_jso);
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));
    ret = snprintf_s(target, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FOMRAT_ACTION_URI, rsc_uri, action_name);
    return_do_info_if_expr(ret <= 0, json_object_put(single_action_jso);
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));

    (void)rf_get_actioninfo_uri(rsc_uri, action_name, actioninfo_uri, MAX_URI_LENGTH);

    json_object_object_add(single_action_jso, RFPROP_TARGET, json_object_new_string((const gchar *)target));
    json_object_object_add(single_action_jso, RFPROP_ACTION_INFO,
        json_object_new_string((const gchar *)actioninfo_uri));
    json_object_object_add(action_prop_jso, (const gchar *)action_name_pointer, single_action_jso);

    return;
}


gint32 rf_get_actioninfo_uri(const gchar *rsc_uri, const gchar *action_name, gchar *actioninfo_uri, guint32 uri_len)
{
    gchar *dot_flag = NULL;
    gint32 ret;

    return_val_do_info_if_fail((NULL != rsc_uri) && (NULL != action_name) && (NULL != actioninfo_uri), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    dot_flag = g_strrstr(action_name, DOT_STR);
    return_val_do_info_if_fail(NULL != dot_flag, VOS_ERR,
        debug_log(DLOG_ERROR, "cann't find dot flag from %s", action_name));

    ret = snprintf_s(actioninfo_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s/%sActionInfo", rsc_uri, dot_flag + 1);
    return_val_do_info_if_fail(ret > 0, VOS_ERR, debug_log(DLOG_ERROR, "format string failed, ret is %d", ret));

    return VOS_OK;
}


gint32 rf_update_rsc_actioninfo_uri(const gchar **rsc_action_array, guint32 action_count,
    const gchar *original_action_info_uri, const gchar *rsc_uri, gchar *formatted_actioninfo_uri, guint32 uri_len)
{
    gchar *dot_flag = NULL;
    gchar *action_name = NULL;
    guint32 i;
    gint32 ret;
    return_val_do_info_if_expr(uri_len == 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error, uri_len is 0.", __FUNCTION__));
    return_val_do_info_if_fail((NULL != rsc_action_array) && (NULL != original_action_info_uri) && (NULL != rsc_uri) &&
        (NULL != formatted_actioninfo_uri),
        VOS_ERR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    for (i = 0; i < action_count; i++) {
        dot_flag = g_strrstr(rsc_action_array[i], DOT_STR);
        return_val_do_info_if_fail(NULL != dot_flag, VOS_ERR,
            debug_log(DLOG_ERROR, "cann't find dot flag from %s", rsc_action_array[i]));

        action_name = dot_flag + 1;

        if (g_str_case_rstr(original_action_info_uri, action_name)) {
            ret = snprintf_s(formatted_actioninfo_uri, uri_len, uri_len - 1, "%s/%sActionInfo", rsc_uri, action_name);
            return_val_do_info_if_fail(ret > 0, VOS_ERR, debug_log(DLOG_ERROR, "format string failed, ret is %d", ret));

            return VOS_OK;
        }
    }

    debug_log(DLOG_ERROR, "cann't match any info for action info uri(%s)", original_action_info_uri);

    return VOS_ERR;
}


gint32 rf_bios_get_registryversion(gchar *value, guint32 value_len, gint32 flag)
{
    errno_t str_ret = EOK;
    OBJ_HANDLE obj_handle = 0;
    gchar string_value[MAX_RSC_ID_LEN] = {0};
    gint32 i = 0;

    // 入参检查
    if (NULL == value) {
        debug_log(DLOG_ERROR, "Input value is NULL");
        return VOS_ERR;
    }

    // 获取 RegistryVersion
    gint32 ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__, BIOS_CLASS_NAME));

    ret = dal_get_property_value_string(obj_handle, BIOS_REGISTRY_VERSION, string_value, MAX_RSC_ID_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__, BIOS_CLASS_NAME,
        BIOS_REGISTRY_VERSION));

    debug_log(DLOG_DEBUG, "Bios RegistryVersion : %s", string_value);

    if (RF_REGISTRY_VERSION_V == flag) {
        // //RegistryVersion : V1.00   -->  BiosAttributeRegistry.v1_0_0
        str_ret = strncpy_s(value, value_len, "BiosAttributeRegistry.v", strlen("BiosAttributeRegistry.v"));
        return_val_if_expr(EOK != str_ret, VOS_ERR);

        while (string_value[i] != '\0') {
            if (string_value[i] >= '0' && string_value[i] <= '9') {
                ret = snprintf_s(value + strlen(value), value_len - strlen(value), value_len - strlen(value) - 1, "%c_",
                    string_value[i]);
                return_val_if_expr(ret < 0, VOS_ERR);
            }

            i++;
        }

        if ('_' == value[strlen(value) - 1]) {
            value[strlen(value) - 1] = '\0';
        }
    } else if (RF_REGISTRY_VERSION_DOT == flag) {
        // RegistryVersion : V1.00   -->  BiosAttributeRegistry.1.0.0
        str_ret = strncpy_s(value, value_len, "BiosAttributeRegistry", strlen("BiosAttributeRegistry"));
        return_val_if_expr(EOK != str_ret, VOS_ERR);

        while (string_value[i] != '\0') {
            if (string_value[i] >= '0' && string_value[i] <= '9') {
                ret = snprintf_s(value + strlen(value), value_len - strlen(value), value_len - strlen(value) - 1, ".%c",
                    string_value[i]);
                return_val_if_expr(ret < 0, VOS_ERR);
            }

            i++;
        }
    }

    debug_log(DLOG_DEBUG, "Redfish RegistryVersion : %s", value);

    return VOS_OK;
}

gint32 return_value_judgment(gint32 arr_subscript, gint32 *return_array)
{
    gint32 result_count;
    gint32 result_err = 0;
    gint32 result;

    
    for (result_count = 0; result_count < arr_subscript; result_count++) {
        
        
        if (VOS_OK == return_array[result_count] || HTTP_OK == return_array[result_count]) {
            
            return HTTP_OK;
        }

        if (!(HTTP_NOT_IMPLEMENTED == return_array[result_count] || VOS_OK == return_array[result_count])) {
            result_err++;
        }
    }

    
    if (result_err > 0) {
        result = HTTP_BAD_REQUEST;
    } else {
        result = HTTP_NOT_IMPLEMENTED;
    }

    return result;
}


gint32 redfish_judge_mntportcab_valid(guint8 net_type)
{
    gint32 result = VOS_ERR;
    guint8 dedicate_enable = 0;
    guint8 lomncsi_enable = 0;
    guint8 pciencsi_enable = 0;
    guint8 aggregation_enable = 0;
    guint8 lom2ncsi_enable = 0;
    guint8 ocpncsi_enable = 0;
    guint8 ocp2ncsi_enable = 0;
    guint8 pcie_mgnt_enable = 0;
    OBJ_HANDLE obj_handle = 0;

    (void)dal_get_object_handle_nth(ETH_CLASS_NAME_MGNTPORTCAP, 0, &obj_handle);

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_MNGTPORTCAP_DEDICATEDENABLE, &dedicate_enable);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_MNGTPORTCAP_LOMNCSIENABLE, &lomncsi_enable);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_MNGTPORTCAP_PCIENCSIENABLE, &pciencsi_enable);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_MNGTPORTCAP_AGGREGATIONENABLE, &aggregation_enable);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_MNGTPORTCAP_LOM2NCSIENABLE, &lom2ncsi_enable);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_MNGTPORTCAP_OCPENABLE, &ocpncsi_enable);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_MNGTPORTCAP_OCP2ENABLE, &ocp2ncsi_enable);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_MNGTPORTCAP_PCIEMGNT_ENABLE, &pcie_mgnt_enable);

    switch (net_type) {
        case NET_TYPE_DEDICATED:
            do_info_if_true(dedicate_enable, (result = VOS_OK));
            break;

        case NET_TYPE_AGGREGATION:
            do_info_if_true(aggregation_enable, (result = VOS_OK));
            break;

        case NET_TYPE_LOM:
            do_info_if_true(lomncsi_enable, (result = VOS_OK));
            break;

        case NET_TYPE_LOM2:
            do_info_if_true(lom2ncsi_enable, (result = VOS_OK));
            break;

        case NET_TYPE_PCIE:
            do_info_if_true(pciencsi_enable, (result = VOS_OK));
            break;

        case NET_TYPE_OCP_CARD:
            do_info_if_true(ocpncsi_enable, (result = VOS_OK));
            break;
        case NET_TYPE_OCP2_CARD:
            do_info_if_true(ocp2ncsi_enable, (result = VOS_OK));
            break;

        case NET_TYPE_CABINET_VLAN:
            result = (pcie_mgnt_enable == ENABLE) ? VOS_OK : VOS_ERR;
            break;

        default:
            debug_log(DLOG_ERROR, "%s: invalid netcard type is %d", __FUNCTION__, net_type);
    }

    return result;
}


gint32 rf_get_property_plain_data(GVariant *encrypted_data_var, gchar **plain_data)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret = 0;
    gchar *old_buf = NULL;
    gchar *new_buf = NULL;
    const gchar **str_v = NULL;
    gsize encrypted_data_array_len = 0;
    gsize i;
    gchar *plain_data_item = NULL;
    gsize cur_buf_len = 0;
    gsize pt_len = 0;

    return_val_if_expr((encrypted_data_var == NULL || plain_data == NULL), VOS_ERR);

    str_v = g_variant_get_strv(encrypted_data_var, &encrypted_data_array_len);
    return_val_do_info_if_expr(str_v == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get encrypted data array failed", __FUNCTION__));

    plain_data_item = (gchar *)g_malloc0(MAX_PT_LEN);
    return_val_do_info_if_expr(plain_data_item == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: alloc new memory failed", __FUNCTION__);
        g_free(str_v););

    for (i = 0; i < encrypted_data_array_len; i++) {
        
        ret = kmc_get_decrypted_data(DOMAIN_ID_RD_EVT_SUBSCRIPTION, str_v[i], plain_data_item, MAX_PT_LEN, &pt_len);
        break_do_info_if_expr(ret != VOS_OK,
            debug_log(DLOG_ERROR, "get plain data failed, ret is %d, current index:%" G_GSIZE_FORMAT, ret, i));

        cur_buf_len += strlen(plain_data_item);

        new_buf = (gchar *)g_malloc0(cur_buf_len + 1);
        break_do_info_if_expr(new_buf == NULL,
            debug_log(DLOG_ERROR, "alloc new buf failed, buf len is %" G_GSIZE_FORMAT, cur_buf_len));

        if (old_buf != NULL) {
            safe_fun_ret = memmove_s(new_buf, cur_buf_len, old_buf, strlen(old_buf));
            do_val_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            clear_sensitive_info(old_buf, strlen(old_buf));
            g_free(old_buf);
        }

        
        safe_fun_ret = memmove_s(new_buf + strlen(new_buf), cur_buf_len - strlen(new_buf), plain_data_item,
            strlen(plain_data_item));
        do_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

        old_buf = new_buf;
        cur_buf_len = strlen(new_buf);

        (void)memset_s(plain_data_item, strlen(plain_data_item), 0, strlen(plain_data_item));
    }

    clear_sensitive_info(plain_data_item, pt_len);
    g_free(plain_data_item);
    
    g_free(str_v);
    

    return_val_do_info_if_expr(i != encrypted_data_array_len, VOS_ERR,
        debug_log(DLOG_ERROR, "parse one of more encrypted data failed, current index:%" G_GSIZE_FORMAT, i);
        g_free(old_buf));

    *plain_data = new_buf;

    return VOS_OK;
}


void get_smm_name_from_redfish_uri(const gchar *prefix, const gchar *uri, gchar *smm_name, gint32 buffer_len)
{
    // URI : prefix/HMM[12]/xxxx
    // 用户输入链接是hmm_name中大小写不一致,将hmm_name格式化为HMM[12]
    (void)snprintf_s(smm_name, buffer_len, buffer_len - 1, "HMM%d", uri[strlen(prefix) + 3] - '0');
}


LOCAL gboolean verify_ipmi_msg_oem_head(const guchar *response_data)
{
    guint32 huawei_id;

    if (NULL == response_data) {
        debug_log(DLOG_ERROR, "(%s)response data is null.", __func__);
        return VOS_ERR;
    }

    huawei_id = MAKE_DWORD(0, response_data[4], response_data[3], response_data[2]);
    if (huawei_id != HUAWEI_MFG_ID) {
        debug_log(DLOG_ERROR, "(%s)Manufacture field in response data is not correct, received 0x%x 0x%x 0x%x",
            __func__, response_data[2], response_data[3], response_data[4]);
        return VOS_ERR;
    }

    return VOS_OK;
}

gint32 redfish_ipmi_send_msg_to_blade(IPMIMSG_BLADE_T *header, gsize resp_size, gpointer *response,
    gboolean wait_response)
{
    IPMI_REQ_MSG_HEAD_S req_msg_header;
    GVariant *property_value = NULL;
    OBJ_HANDLE obj_handle = 0x00;
    gint32 ret;
    guint8 alias_name;
    gint8 type;

    if (NULL == header || NULL == response) {
        return VOS_ERR;
    }

    alias_name = header->slave_addr;

    property_value = g_variant_new_byte(alias_name);

    // 根据目的 IPMB Slave address获取句柄
    (void)dfl_get_specific_object(IPMI_CHANNEL_CLASS_NAME, PROPERTY_IPMITRAGET_DEST_ADDR, property_value, &obj_handle);
    g_variant_unref(property_value);
    property_value = NULL;

    // get target type
    ret = dfl_get_property_value(obj_handle, PROPERTY_IPMITRAGET_TYPE, &property_value);
    return_val_if_fail(VOS_OK == ret, ret);

    type = g_variant_get_byte(property_value);
    g_variant_unref(property_value);
    property_value = NULL;

    req_msg_header.target_type = (TARGET_TYPE_E)type;

    // get instance
    ret = dfl_get_property_value(obj_handle, PROPERTY_TARGET_INSTANCE, &property_value);
    return_val_if_fail(VOS_OK == ret, ret);

    req_msg_header.target_instance = g_variant_get_byte(property_value);
    g_variant_unref(property_value);

    req_msg_header.lun = header->lun;
    req_msg_header.netfn = header->netfun;
    req_msg_header.cmd = header->cmd;
    req_msg_header.src_len = header->src_len;

    ret = ipmi_send_request(&req_msg_header, header->data, resp_size, response, wait_response);

    return ret;
}

gint32 redfish_get_blade_bmc_ip_config_by_cmd(guint8 slave_addr, guint8 net_func, guint8 cmd, guint8 filter,
    gchar *result, guint16 result_len)
{
    debug_log(DLOG_DEBUG, "%s %d,filter=%d", __FUNCTION__, __LINE__, filter);
    gint32 ret;
    IPMIMSG_BLADE_T request_head;
    gpointer response = NULL;
    const guchar *response_data = NULL;
    guchar data[IPMI_DATA_FILTER_LEN] = { 0 };
    return_val_if_expr(NULL == result, VOS_ERR);

    request_head.lun = 0;
    request_head.netfun = net_func;
    request_head.slave_addr = slave_addr;
    request_head.cmd = cmd;

    request_head.src_len = 4;
    data[0] = 0x01;
    data[1] = filter;
    data[2] = 0x00;
    data[3] = 0x00;
    request_head.data = data;

    ret = redfish_ipmi_send_msg_to_blade(&request_head, MAX_POSSIBLE_IPMI_FRAME_LEN, &response, TRUE);
    return_val_do_info_if_expr(VOS_OK != ret, ret,
        debug_log(DLOG_ERROR, "(%s) Send lan config ipmi request to blade%d BMC failed.ret:%d", __func__, slave_addr,
        ret));
    response_data = get_ipmi_src_data(response);
    return_val_do_info_if_expr(NULL == response_data, VOS_ERR,
        debug_log(DLOG_ERROR, "(%s) Response data is null", __func__);
        g_free(response));

    return_val_do_info_if_expr(response_data[0] != COMP_CODE_SUCCESS, ret, ret = response_data[0]; g_free(response);
        response = NULL;);

    switch (filter) {
        case IPMI_IPV4_ADDRESS_OFFSET: // 获取IP地址
        case IPMI_IPV4_MASK_OFFSET:    // 获取子网掩码
        case IPMI_IPV4_GATEWAY_OFFSET: // 获取默认网关
            result[0] = '\0';
            if (0 != response_data[2] || 0 != response_data[3] || 0 != response_data[4] || 0 != response_data[5]) {
                (void)snprintf_s(result, result_len, result_len - 1, "%d.%d.%d.%d", response_data[2], response_data[3],
                    response_data[4], response_data[5]);
            }
            debug_log(DLOG_DEBUG, "(%s) get blade%d Response: %d.%d.%d.%d", __func__, slave_addr, response_data[2],
                response_data[3], response_data[4], response_data[5]);
            break;
        case IPMI_IPV4_MODE_OFFSET: // 获取ip模式
            result[0] = response_data[2] & 0x0f;
            debug_log(DLOG_DEBUG, "(%s) get blade%d (0x03--ip;0x06--submask;0x0c--gateway) Response: %d", __func__,
                slave_addr, response_data[2]);

            break;
        case IPMI_MAC_ADDRESS_OFFSET: // 获取mac地址
            debug_log(DLOG_DEBUG, "%s get blade%d mac Response: %02X:%02X:%02X:%02X:%02X:%02X", __func__, slave_addr,
                response_data[2], response_data[3], response_data[4], response_data[5], response_data[6],
                response_data[7]);
            (void)snprintf_s(result, result_len, result_len - 1, "%02x%02x%02x%02x%02x%02x", response_data[2],
                response_data[3], response_data[4], response_data[5], response_data[6], response_data[7]);
            break;
        case IPMI_IPV6_MODE_OFFSET: // 获取ipv6的IP模式
            if (verify_ipmi_msg_oem_head(response_data)) {
                debug_log(DLOG_ERROR, "(%s)verify_ipmi_msg_oem_head from blade%d failed.\r\n", __func__, slave_addr);
                g_free(response);
                return VOS_ERR;
            }

            result[0] = response_data[5] & 0x0f;
            debug_log(DLOG_DEBUG, "%s get blade%d ip6mode Response: %d", __func__, slave_addr, response_data[5]);
            break;
        case IPMI_IPV6_PREFIX_OFFSET: // 获取ipv6的前缀
            return_val_do_info_if_expr(verify_ipmi_msg_oem_head(response_data), VOS_ERR,
                debug_log(DLOG_ERROR, "(%s)verify_ipmi_msg_oem_head from blade%d failed.\r\n", __func__, slave_addr);
                g_free(response));
            result[0] = response_data[5];
            debug_log(DLOG_DEBUG, "%s get blade%d ip6prefix Response: %d", __func__, slave_addr, response_data[5]);
            break;
        default:
            break;
    }

    g_free(response);
    response = NULL;
    return VOS_OK;
}

#ifdef ARM64_HI1711_ENABLED

gint32 rf_check_lsw_resource_legality(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    OBJ_HANDLE handle = 0;

    return_val_do_info_if_expr(((i_paras == NULL) || (prop_provider == NULL) || (count == NULL)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, input param error", __FUNCTION__));

    
    ret = dfl_get_object_handle(OBJ_LSW_PUBLIC_CENTER_CP, &handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_NOT_FOUND,
        debug_log(DLOG_DEBUG, "%s, do not support this command", __FUNCTION__));

    return VOS_OK;
}

/*****************************************************************************
* Function    : rf_check_lsw_blade_name
* Description : 校验uri中的blade_name，同时返回XML中的对象名blade_name
* Return      : gint32
* Others      :
* History     :
*      Creat by m00446762 for  lsw 2019-8-27
*****************************************************************************/
gint32 rf_check_lsw_blade_name(gchar* uri, const gchar** blade_name)
{
    gint32 ret;
    gchar blade_name_uri[MAX_NAME_SIZE] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    const gchar *object_name = NULL;

    ret = rf_get_uri_info_by_position(uri, PLANE_NAME_POSITION, blade_name_uri, MAX_NAME_SIZE);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s, get uri info fail: %d.\n", __FUNCTION__, ret));

    ret = dfl_get_object_list(CLASS_NAME_IPMBETH_BLADE, &obj_list);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s, get obj_list fail: %d.\n", __FUNCTION__, ret));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        object_name = dfl_get_object_name((OBJ_HANDLE)obj_node->data);
        do_if_expr(((strcmp(object_name, BLADE37) == 0) || (strcmp(object_name, BLADE38) == 0)), continue);

        
        if (g_ascii_strcasecmp(blade_name_uri, object_name) == 0) {
            *blade_name = object_name;
            g_slist_free(obj_list);
            return VOS_OK;
        }
    }
    g_slist_free(obj_list);
    return VOS_ERR;
}


gint32 rf_format_const_string_upper_head(const gchar *src_string, guint32 src_length, gchar *dst_string,
    guint32 dst_length)
{
    gint32 ret;

    ret = strncpy_s(dst_string, dst_length, src_string, src_length);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail: %d\n", __FUNCTION__, ret));

    format_string_as_upper_head(dst_string, dst_length);
    return VOS_OK;
}


gint32 rf_check_lsw_port_by_blade(OBJ_HANDLE port_handle, const gchar *blade_name)
{
    OBJ_HANDLE blade_handle = 0;
    gint32 ret;
    guint8 slot_id_uri = 0;
    guint8 slot_id_port = 0;

    ret = dfl_get_object_handle(blade_name, &blade_handle);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s, get %s handle fail: %d.\n", __FUNCTION__, blade_name, ret));

    ret = dal_get_property_value_byte(blade_handle, PROPERTY_IPMBETH_BLADE_ID, &slot_id_uri);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s, get blade slot_id fail: %d.\n", __FUNCTION__, ret));

    ret = dal_get_property_value_byte(port_handle, PROPERTY_LSW_CENTER_SLOTID, &slot_id_port);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s, get port slot_id fail: %d.\n", __FUNCTION__, ret));

    
    return_val_if_expr(((slot_id_uri == 0) || (slot_id_port != slot_id_uri)), VOS_ERR);

    return VOS_OK;
}


gint32 rf_check_format_blade_name(PROVIDER_PARAS_S *i_paras, gchar *blade_name_format, gsize max_blade_name_len)
{
    gint32 ret;
    const gchar *blade_name = NULL;

    ret = rf_check_lsw_blade_name(i_paras->uri, &blade_name);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s, uri blade_name is not match: %d.\n", __FUNCTION__, ret));

    ret = rf_format_const_string_upper_head(blade_name, strlen(blade_name), blade_name_format, max_blade_name_len);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s, format upper head fail: %d.\n", __FUNCTION__, ret));

    return VOS_OK;
}


gint32 rf_get_uri_info_by_position(gchar *uri, guint32 position, gchar *uri_name, gint32 uri_len)
{
    gchar **split_array;
    gint32 ret;

    split_array = g_strsplit(uri, "/", 0);
    return_val_do_info_if_expr(split_array == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, split_array is NULL.", __FUNCTION__));
    return_val_do_info_if_expr(g_strv_length(split_array) < position, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s, array size %d is error.", __FUNCTION__, g_strv_length(split_array));
        g_strfreev(split_array));

    ret = strncpy_s(uri_name, uri_len, split_array[position - 1], strlen(split_array[position - 1]));
    g_strfreev(split_array);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail: %d\n", __FUNCTION__, ret));
    return VOS_OK;
}


gint32 rf_check_lsw_port_by_position(OBJ_HANDLE object_handle, gchar *port_name, gsize max_port_name_len)
{
    gint32 ret;
    const gchar *object_name = dfl_get_object_name(object_handle);
    guint8 position = 0;
    guint8 slot_id = 0;
    guint8 valid = 0;

    ret = dal_get_property_value_string(object_handle, PROPERTY_LSW_CENTER_PORT_NAME, port_name, max_port_name_len);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s, get %s port name fail: %d.\n", __FUNCTION__, object_name, ret));

    ret = dal_get_property_value_byte(object_handle, PROPERTY_LSW_PORT_POSITION, &position);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s, get %s position fail: %d.\n", __FUNCTION__, object_name, ret));

    ret = dal_get_property_value_byte(object_handle, PROPERTY_LSW_CENTER_SLOTID, &slot_id);
    return_val_do_info_if_expr(ret != VOS_OK, ret,
        debug_log(DLOG_ERROR, "%s, get %s slot id fail: %d.\n", __FUNCTION__, object_name, ret));

    
    if ((((slot_id == SMM1_SLOT_ID) || (slot_id == SMM2_SLOT_ID)) && (position == LSW_OUT_PANEL_PORT)) ||
        ((slot_id != SMM1_SLOT_ID) && (slot_id != SMM2_SLOT_ID))) {
        ret = dal_get_property_value_byte(object_handle, PROPERTY_LSW_NODE_VALID, &valid);
        return_val_do_info_if_expr(((ret != VOS_OK) || (valid != LSW_NODE_VALID)), VOS_ERR,
            debug_log(DLOG_DEBUG, "%s, get %s node valid fail: %d.\n", __FUNCTION__, object_name, ret));

        return VOS_OK;
    }

    return VOS_ERR;
}
#endif


LOCAL gint32 rf_get_encrypted_data(const gchar *plaintext, gchar **ciphertext)
{
    gint32 ret;
    gchar *ciphertext_result = NULL;
    gsize ciphertext_result_len = 0;

    ciphertext_result = (gchar *)g_malloc0(MAX_ENCRYPTO_DATA_LEN);
    if (ciphertext_result == NULL) {
        debug_log(DLOG_ERROR, "%s: Alloc memory failed", __FUNCTION__);
        return VOS_ERR;
    }

    
    ret = kmc_get_encrypt_data_if_mm_board(DOMAIN_ID_RD_EVT_SUBSCRIPTION, plaintext, ciphertext_result,
        MAX_ENCRYPTO_DATA_LEN, &ciphertext_result_len);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: Encrypt redfish data failed, ret = %d", __FUNCTION__, ret);
        g_free(ciphertext_result);
        return ret;
    }

    *ciphertext = ciphertext_result;

    return VOS_OK;
}


gint32 rf_get_property_encrypted_data(const gchar *property_plain_data, GVariant **encrypted_data_var)
{
    errno_t safe_fun_ret = EOK;
    guint32 ret = 0;
    gsize plain_data_len;
    gsize i;
    gsize encrypt_segment_num;
    gchar **encrypt_str_arr;
    GVariant *data_var = NULL;
    gchar               cur_plain_buffer[ENCRYPTED_DATA_MAX_LEN + 1] = {0};
    guint32 cur_plain_buffer_len;
    const gchar *cur_index = property_plain_data;

    return_val_if_expr(
        (property_plain_data == NULL) || (encrypted_data_var == NULL) || (property_plain_data[0] == '\0'), VOS_ERR);

    plain_data_len = strlen(property_plain_data);
    encrypt_segment_num = (plain_data_len - 1) / ENCRYPTED_DATA_MAX_LEN + 1;

    
    encrypt_str_arr = (gchar **)g_malloc0((encrypt_segment_num + 1) * sizeof(gchar *));
    return_val_do_info_if_expr(encrypt_str_arr == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "alloc encrypt str array failed"));

    for (i = 0; i < encrypt_segment_num; i++, cur_index += ENCRYPTED_DATA_MAX_LEN) {
        cur_plain_buffer_len = strlen(cur_index) > ENCRYPTED_DATA_MAX_LEN ? ENCRYPTED_DATA_MAX_LEN : strlen(cur_index);

        (void)memset_s(cur_plain_buffer, ENCRYPTED_DATA_MAX_LEN + 1, 0, ENCRYPTED_DATA_MAX_LEN + 1);
        safe_fun_ret = memmove_s(cur_plain_buffer, ENCRYPTED_DATA_MAX_LEN + 1, cur_index, cur_plain_buffer_len);
        do_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

        ret = rf_get_encrypted_data(cur_plain_buffer, &encrypt_str_arr[i]);
        break_do_info_if_expr(ret != VOS_OK,
            debug_log(DLOG_ERROR, "encrypt data failed, current index is %" G_GSIZE_FORMAT ", ret is %d", i, ret));
    }

    goto_label_do_info_if_expr((i != encrypt_segment_num) || (ret != VOS_OK), quit,
        debug_log(DLOG_ERROR, "generate encrypted data array failed, current index:%" G_GSIZE_FORMAT ", ret is %d", i,
        ret);
        ret = VOS_ERR);

    data_var = g_variant_new_strv((const gchar * const *)encrypt_str_arr, -1);
    
    goto_label_do_info_if_expr(
        data_var == NULL, quit, debug_log(DLOG_ERROR, "%s: alloc new variant failed", __FUNCTION__); ret = VOS_ERR);
    

    *encrypted_data_var = data_var;

quit:
    i = 0;
    while (encrypt_str_arr[i] != NULL) {
        clear_sensitive_info(encrypt_str_arr[i], strlen(encrypt_str_arr[i]));
        g_free(encrypt_str_arr[i]);
        encrypt_str_arr[i] = NULL;

        i++;
    }
    g_free(encrypt_str_arr);

    return ret;
}


gint32 rf_get_elabel_handle_of_fru(OBJ_HANDLE fru_obj_handle, OBJ_HANDLE *elabel_obj_handle)
{
    gint32 ret;

    return_val_do_info_if_fail((0 != fru_obj_handle) && (NULL != elabel_obj_handle), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    
    ret = dfl_get_referenced_object(fru_obj_handle, PROPERTY_FRU_ELABEL_RO, elabel_obj_handle);
    return_val_if_expr(VOS_OK == ret, VOS_OK);

    
    ret = dal_get_specific_object_position(fru_obj_handle, CLASS_ELABEL, elabel_obj_handle);
    return_val_if_expr(VOS_OK == ret, VOS_OK);

    
    ret = dal_get_specific_object_position(fru_obj_handle, CLASS_PERIPHERAL_ELABEL, elabel_obj_handle);

    return ret;
}

LOCAL void custom_chassis_mode_value(json_object *rsc_jso)
{
    gint32 ret;
    OBJ_HANDLE chassis_handle = 0;
    json_object* property_jso = NULL;
    gchar model_var[PROP_VAL_MAX_LENGTH] = {0};

    ret = dal_get_object_handle_nth(CLASS_NAME_CHASSIS, 0, &chassis_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "get object[Chassis] handle fail. ret = %d", ret);
        return;
    }

    ret = dal_get_property_value_string(chassis_handle, PROPERTY_CHASSIS_CUSTOM_DATA1, model_var, PROP_VAL_MAX_LENGTH);
    if (ret != DFL_OK) {
        debug_log(DLOG_INFO, "get property[CustomData1] value  fail. ret = %d", ret);
        return;
    }
    property_jso = json_object_new_string((const gchar*)model_var);
    if (property_jso == NULL) {
        debug_log(DLOG_INFO, "json_object_new_string fail");
        return;
    }

    if (rf_string_check(model_var) == RET_OK) {
        json_object_object_add(rsc_jso, RFPROP_MODEL, property_jso);
    }

    return;
}


gint32 rf_update_rsc_component_info(OBJ_HANDLE obj_handle, json_object *rsc_jso)
{
    gchar obj_class[MAX_NAME_SIZE];
    gchar               referencd_object[MAX_NAME_SIZE] = {0};
    gchar               referencd_property[MAX_NAME_SIZE] = {0};
    gint32 ret;
    
    OBJ_HANDLE fru_handle = 0;
    OBJ_HANDLE elabel_handle = 0;
    
    guint8 device_type = 0;

    return_val_if_fail(0 != obj_handle, VOS_ERR);

    ret = dfl_get_class_name(obj_handle, obj_class, MAX_NAME_SIZE);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "cann't get object class for %s, ret is %d", dfl_get_object_name(obj_handle), ret));

    if (0 != g_strcmp0(CLASS_COMPONENT, obj_class)) {
        return VOS_ERR;
    }

    ret = dfl_get_referenced_property(obj_handle, PROPERTY_COMPONENT_FRUID, referencd_object, referencd_property,
        MAX_NAME_SIZE, MAX_NAME_SIZE);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "get referenced fru property failed, ret is %d", ret));

    ret = dfl_get_object_handle((const gchar *)referencd_object, &fru_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "get object handle with object name (%s) failed, ret is %d", referencd_object, ret));

    
    ret = rf_get_elabel_handle_of_fru(fru_handle, &elabel_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "cann't get referenced elabel object, ret is %d", ret));
    

    
    rf_add_property_jso_string(elabel_handle, PROPERTY_ELABEL_PRODUCT_MFG_NAME, RFPROP_MANUFACTURE, rsc_jso);
    rf_add_property_jso_string(elabel_handle, PROPERTY_ELABEL_PRODUCT_SN, RFPROP_SERIAL_NUMBER, rsc_jso);
    rf_add_property_jso_string(elabel_handle, PROPERTY_ELABEL_PRODUCT_PN, RFPROP_PART_NUMBER, rsc_jso);
    rf_add_property_jso_string(elabel_handle, PROPERTY_ELABEL_PRODUCT_NAME, RFPROP_MODEL, rsc_jso);
    rf_add_property_jso_string(elabel_handle, PROPERTY_ELABEL_PRODUCT_ASSET_TAG, RFPROP_ASSET_TAG, rsc_jso);

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &device_type);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get property value of %s failed, ret = %d.", PROPERTY_COMPONENT_DEVICE_TYPE, ret);
    }
    if (device_type == FRU_TYPE_BACKPLANE) {
        if (dal_check_if_vsmm_supported()) {
            rf_add_property_jso_string(elabel_handle, PROPERTY_ELABEL_CHASSIS_SN, RFPROP_SERIAL_NUMBER, rsc_jso);
        }
        custom_chassis_mode_value(rsc_jso);
    }

    return VOS_OK;
}


void rf_provider_paras_free(PROVIDER_PARAS_S *provider_paras)
{
    return_do_info_if_fail((NULL != provider_paras),
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    g_slist_free(provider_paras->parent_obj_handle_list);
    provider_paras->parent_obj_handle_list = NULL;

    return;
}


LOCAL gint32 _get_component_uri_with_slot(OBJ_HANDLE component_obj, const gchar *uri_format, const void *user_data,
    gchar *component_uri, guint32 uri_len)
{
    gchar               slot_name[MAX_RSC_NAME_LEN] = {0};
    gint32 ret;

    ret = redfish_get_board_slot(slot_name, MAX_RSC_NAME_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "get chassis id for mainboard failed"));
    return_val_do_info_if_expr(uri_len == 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error, uri_len is 0.", __FUNCTION__));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(component_uri, uri_len, uri_len - 1, uri_format, slot_name);
#pragma GCC diagnostic pop
    return_val_do_info_if_fail(ret > 0, VOS_ERR, debug_log(DLOG_ERROR, "format string failed, ret is %d", ret));

    return VOS_OK;
}


LOCAL gint32 _get_unit_uri_with_slot(OBJ_HANDLE component_obj, const gchar *uri_format, const void *user_data,
    gchar *component_uri, guint32 uri_len)
{
    gchar               slot_name[MAX_RSC_NAME_LEN] = {0};
    gint32 ret;
    guint8 unum = 0;
    ret = dal_get_property_value_byte(component_obj, PROPERTY_COMPONENT_DEVICENUM, &unum);
    return_val_if_expr(VOS_OK != ret, VOS_ERR);

    ret = snprintf_s(slot_name, sizeof(slot_name), sizeof(slot_name) - 1, "%s%u", RF_UNIT_CHASSIS_PREFIX, unum);
    return_val_if_expr(ret < 0, VOS_ERR);
    return_val_do_info_if_expr(uri_len == 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error, uri_len is 0.", __FUNCTION__));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(component_uri, uri_len, uri_len - 1, uri_format, slot_name);
#pragma GCC diagnostic pop
    return_val_do_info_if_fail(ret > 0, VOS_ERR, debug_log(DLOG_ERROR, "format string failed, ret is %d", ret));

    return VOS_OK;
}


LOCAL gint32 _get_board_component_uri(OBJ_HANDLE component_obj, const gchar *uri_format, const void *user_data,
    gchar *component_uri, guint32 uri_len)
{
    gchar               slot_name[MAX_RSC_NAME_LEN] = {0};
    gchar               rsc_name[MAX_RSC_NAME_LEN] = {0};

    gint32 ret = rf_gen_enc_contained_component_chassis_slot(slot_name, MAX_RSC_NAME_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "get enc contained component chassis slot failed"));

    ret = get_location_devicename(component_obj, rsc_name, MAX_RSC_NAME_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "get board resource id for %s failed", dfl_get_object_name(component_obj)));
    return_val_do_info_if_expr(uri_len == 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error, uri_len is 0.", __FUNCTION__));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(component_uri, uri_len, uri_len - 1, uri_format, slot_name, rsc_name);
#pragma GCC diagnostic pop
    return_val_do_info_if_fail(ret > 0, VOS_ERR, debug_log(DLOG_ERROR, "format string failed, ret is %d", ret));

    return VOS_OK;
}


LOCAL gint32 _get_pcie_card_component_uri(OBJ_HANDLE component_obj, const gchar *uri_format, const void *user_data,
    gchar *component_uri, guint32 uri_len)
{
    gint32 ret;
    OBJ_HANDLE comp_chassis_handle = 0;
    gchar               pcie_card_rsc_id[MAX_RSC_NAME_LEN] = {0};
    gchar               chassis_rsc_id[MAX_RSC_NAME_LEN] = {0};
    guchar component_slot = 0;

    ret = rf_get_object_location_handle(0, component_obj, &comp_chassis_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "get location handle for [%s] failed", dfl_get_object_name(component_obj)));

    ret = rf_gen_chassis_component_id(NULL, comp_chassis_handle, chassis_rsc_id, MAX_RSC_NAME_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "generate chassis rsc id for [%s] failed", dfl_get_object_name(comp_chassis_handle)));

    (void)dal_get_property_value_byte(component_obj, PROPERTY_COMPONENT_DEVICENUM, &component_slot);

    ret = snprintf_s(pcie_card_rsc_id, MAX_RSC_NAME_LEN, MAX_RSC_NAME_LEN - 1, "%s%u", RF_PCIECARD_PREFIX,
        component_slot);
    return_val_do_info_if_fail(ret > 0, VOS_ERR,
        debug_log(DLOG_ERROR, "format pcie card rsc id failed, ret is %d", ret));
    return_val_do_info_if_expr(uri_len == 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error, uri_len is 0.", __FUNCTION__));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(component_uri, uri_len, uri_len - 1, uri_format, chassis_rsc_id, pcie_card_rsc_id);
#pragma GCC diagnostic pop
    return (ret > 0) ? VOS_OK : VOS_ERR;
}


LOCAL gint32 _get_component_uri_with_slot_and_rsc_id(const gchar *rsc_type, const gchar *slot_name,
    const gchar *uri_format, const gchar *user_data, OBJ_HANDLE component_obj, gchar *component_uri, guint32 uri_len)
{
    gint32 ret;
    guchar rsc_id = 0;

    return_val_do_info_if_fail((NULL != uri_format) && (NULL != component_uri) && (NULL != slot_name), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    ret = dal_get_property_value_byte(component_obj, PROPERTY_COMPONENT_DEVICENUM, &rsc_id);
    return_val_do_info_if_fail((VOS_OK == ret), VOS_ERR,
        debug_log(DLOG_ERROR, "cann't get property [%s] from object handle [%s] failed, ret is %d",
        PROPERTY_COMPONENT_DEVICENUM, dfl_get_object_name(component_obj), ret));

    
    do_val_if_fail(0 == g_strcmp0(rsc_type, URI_CLASS_CPU), rsc_id--);
    return_val_do_info_if_expr(uri_len == 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error, uri_len is 0.", __FUNCTION__));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(component_uri, uri_len, uri_len - 1, uri_format, slot_name, (gint32)rsc_id);
#pragma GCC diagnostic pop
    return_val_do_info_if_fail(ret > 0, VOS_ERR, debug_log(DLOG_ERROR, "format component uri failed, ret is %d", ret));

    return VOS_OK;
}


LOCAL gint32 _get_power_supply_component_uri(OBJ_HANDLE component_obj, const gchar *uri_format, const void *user_data,
    gchar *component_uri, guint32 uri_len)
{
    gchar               slot_name[MAX_RSC_NAME_LEN] = {0};
    gint32 ret;

    ret = rf_gen_enc_contained_component_chassis_slot(slot_name, MAX_RSC_NAME_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "get enclosure related chassis slot failed"));

    return _get_component_uri_with_slot_and_rsc_id(URI_CLASS_PS, slot_name, uri_format, (const gchar *)user_data,
        component_obj, component_uri, uri_len);
}


LOCAL gint32 _get_fan_component_uri(OBJ_HANDLE component_obj, const gchar *uri_format, const void *user_data,
    gchar *component_uri, guint32 uri_len)
{
    gchar               slot_name[MAX_RSC_NAME_LEN] = {0};
    gint32 ret;

    ret = rf_gen_enc_contained_component_chassis_slot(slot_name, MAX_RSC_NAME_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "get enclosure related chassis slot failed"));
    if (check_pangea_node_to_enc(component_obj)) {
        ret = snprintf_s(slot_name, MAX_RSC_NAME_LEN, MAX_RSC_NAME_LEN - 1, "%s", RF_ENCLOSURE_CHASSIS_PREFIX);
    }
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "get enclosure related chassis slot failed");
        return VOS_ERR;
    }

    return _get_component_uri_with_slot_and_rsc_id(URI_CLASS_FAN, slot_name, uri_format, (const gchar *)user_data,
        component_obj, component_uri, uri_len);
}


LOCAL gint32 _get_processor_component_uri(OBJ_HANDLE component_obj, const gchar *uri_format, const void *user_data,
    gchar *component_uri, guint32 uri_len)
{
    gchar               slot_name[MAX_RSC_NAME_LEN] = {0};
    gint32 ret;

    ret = redfish_get_board_slot(slot_name, MAX_RSC_NAME_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "get enclosure related chassis slot failed"));

    return _get_component_uri_with_slot_and_rsc_id(URI_CLASS_CPU, slot_name, uri_format, (const gchar *)user_data,
        component_obj, component_uri, uri_len);
}


LOCAL gint32 _get_memory_component_uri(OBJ_HANDLE component_obj, const gchar *uri_format, const void *user_data,
    gchar *component_uri, guint32 uri_len)
{
    guint8 device_num = 0;
    gint32 ret;
    OBJ_HANDLE memory_handle;
    gchar               memory_rsc_id[MAX_RSC_ID_LEN] = {0};
    gchar               chassis_comp_slot[MAX_RSC_ID_LEN] = {0};

    
    (void)dal_get_property_value_byte(component_obj, PROPERTY_COMPONENT_DEVICENUM, &device_num);
    ret = dal_get_specific_object_byte(CLASS_MEMORY, PROPERTY_MEM_DIMM_NUM, device_num, &memory_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "get memory object failed,ret = %d, device num = %d", ret, device_num));
    

    ret = get_location_devicename(memory_handle, memory_rsc_id, MAX_RSC_ID_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "cann't get location and devicename for object [%s]",
        dfl_get_object_name(memory_handle)));

    ret = redfish_get_board_slot(chassis_comp_slot, MAX_RSC_ID_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: cann't get board slot for mainboard", __FUNCTION__));
    return_val_do_info_if_expr(uri_len == 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error, uri_len is 0.", __FUNCTION__));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(component_uri, uri_len, uri_len - 1, uri_format, chassis_comp_slot, memory_rsc_id);
#pragma GCC diagnostic pop
    return_val_do_info_if_fail(ret > 0, VOS_ERR, debug_log(DLOG_ERROR, "format memory uri failed, ret is %d", ret));

    return VOS_OK;
}


gint32 rf_gen_component_uri(OBJ_HANDLE component_handle, gchar *rsc_uri, guint32 rsc_uri_len)
{
    gint32 ret;
    guint32 i;
    guchar component_type;
    get_component_uri_func get_uri_func;
    const gchar *uri_format = NULL;
    COMPONENT_TO_URI_INFO_S        component_to_uri_info_array[] = {
        {COMPONENT_TYPE_CPU,                URI_FORMAT_SYSTEM_REPROCESSORS,    _get_processor_component_uri},
        {COMPONENT_TYPE_MEMORY,             URI_FORMAT_SYSTEM_REMEMORYS,       _get_memory_component_uri},
        {COMPONENT_TYPE_HARDDISK,           URI_FORMAT_CHASSIS_DRIVE,          _get_board_component_uri},
        {COMPONENT_TYPE_PS,                 URI_FORMAT_PS,                     _get_power_supply_component_uri},
        {COMPONENT_TYPE_FAN,                URI_FORMAT_FAN,                    _get_fan_component_uri},
        {COMPONENT_TYPE_HDD_BACKPLANE,      URI_FORMAT_CHASSIS_BOARD,          _get_board_component_uri},
        {COMPONENT_TYPE_RAID_CARD,          URI_FORMAT_CHASSIS_BOARD,          _get_board_component_uri},
        {COMPONENT_TYPE_PCIE_CARD,          URI_FORMAT_CHASSIS_PCIEDEVICES_URI, _get_pcie_card_component_uri},
        {COMPONENT_TYPE_MEZZ,               URI_FORMAT_CHASSIS_BOARD,          _get_board_component_uri},
        {COMPONENT_TYPE_NIC_CARD,           URI_FORMAT_CHASSIS_BOARD,          _get_board_component_uri},
        {COMPONENT_TYPE_MEMORY_RISER,       URI_FORMAT_CHASSIS_BOARD,          _get_board_component_uri},
        {COMPONENT_TYPE_PCIE_RISER,         URI_FORMAT_CHASSIS_BOARD,          _get_board_component_uri},
        {COMPONENT_TYPE_MAINBOARD,          URI_FORMAT_CHASSIS_BOARD,          _get_board_component_uri},
        {COMPONENT_TYPE_CHASSIS,            URI_FORMAT_CHASSIS,                _get_component_uri_with_slot},
        {COMPONENT_TYPE_BMC,                URI_FORMAT_MANAGER,                _get_component_uri_with_slot},
        {COMPONENT_TYPE_SWITCH_MEZZ,        URI_FORMAT_CHASSIS_BOARD,          _get_board_component_uri},
        {COMPONENT_TYPE_SD_CARD,            URI_FORMAT_CHASSIS_DRIVE,          _get_board_component_uri},
        {COMPONENT_TYPE_IO_BOARD,           URI_FORMAT_CHASSIS_BOARD,          _get_board_component_uri},
        {COMPONENT_TYPE_BOARD_CPU,          URI_FORMAT_CHASSIS_BOARD,          _get_board_component_uri},
        {COMPONENT_TYPE_SYSTEM,             URI_FORMAT_SYSTEM,                 _get_component_uri_with_slot},
        {COMPONENT_TYPE_EXPAND_BOARD,       URI_FORMAT_CHASSIS_BOARD,          _get_board_component_uri},
        {COMPONENT_TYPE_LED_BOARD,          URI_FORMAT_CHASSIS_BOARD,          _get_board_component_uri},
        {COMPONENT_TYPE_PIC_CARD,           URI_FORMAT_CHASSIS_BOARD,          _get_board_component_uri},
        {COMPONENT_TYPE_BUTTON,             URI_FORMAT_CHASSIS,                _get_component_uri_with_slot},
        {COMPONENT_TYPE_EXPANDER,           URI_FORMAT_CHASSIS_BOARD,          _get_board_component_uri},
        {COMPONENT_TYPE_EXPBOARD,           URI_FORMAT_CHASSIS_BOARD,          _get_board_component_uri},
        {COMPONENT_TYPE_GPU_BOARD,          URI_FORMAT_CHASSIS_BOARD,          _get_board_component_uri},
        {COMPONENT_TYPE_UNIT,               URI_FORMAT_CHASSIS,                _get_unit_uri_with_slot},
        {COMPONENT_TYPE_RMM,                URI_FORMAT_MANAGER,                _get_component_uri_with_slot},
        {COMPONENT_TYPE_RACK,               URI_FORMAT_CHASSIS,                _get_component_uri_with_slot},
        {COMPONENT_TYPE_MESH_CARD,          URI_FORMAT_CHASSIS_BOARD,          _get_board_component_uri},
        
        {COMPONENT_TYPE_LEAKDET_CARD,       URI_FORMAT_CHASSIS_BOARD,          _get_board_component_uri}
    };

    ret = dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "get device type failed, ret is %d", ret));

    ret = VOS_ERR;
    guint32 len = G_N_ELEMENTS(component_to_uri_info_array);
    for (i = 0; i < len; i++) {
        if (component_type == component_to_uri_info_array[i].component_device_type) {
            get_uri_func = component_to_uri_info_array[i].uri_get_func;
            uri_format = component_to_uri_info_array[i].uri_format;

            ret = get_uri_func(component_handle, uri_format, NULL, rsc_uri, rsc_uri_len);
        }
    }

    return ret;
}


gint32 rf_check_rsc_uri_valid_with_property_list(const gchar *rsc_id, const gchar *class_name, GSList *property_list,
    OBJ_HANDLE *obj_handle)
{
    GSList *obj_list = NULL;
    GSList *obj_list_iter = NULL;
    
    OBJ_HANDLE obj_handle_iter = 0;
    
    gint32 ret;
    gchar               connected_property_val[MAX_RSC_ID_LEN] = {0};
    const gchar *obj_name = NULL;
    GSList *property_list_iter = NULL;
    const gchar *property_name = NULL;
    gint32 property_val_remain_len;

    return_val_do_info_if_fail((NULL != rsc_id) && (NULL != class_name) && (NULL != property_list) &&
        (NULL != obj_handle),
        VOS_ERR, debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    ret = dfl_get_object_list(class_name, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: cann't get object list for [%s], ret is %d", __FUNCTION__, class_name, ret));

    
    for (obj_list_iter = obj_list; NULL != obj_list_iter; obj_list_iter = obj_list_iter->next) {
        obj_handle_iter = (OBJ_HANDLE)obj_list_iter->data;

        obj_name = dfl_get_object_name(obj_handle_iter);
        continue_do_info_if_fail(NULL != obj_name, debug_log(DLOG_ERROR,
            "%s failed: cann't get object name with handle [%" OBJ_HANDLE_FORMAT "]", __FUNCTION__, obj_handle_iter));

        property_list_iter = property_list;
        (void)memset_s(connected_property_val, sizeof(connected_property_val), 0, sizeof(connected_property_val));

        while (NULL != property_list_iter) {
            property_name = g_variant_get_string((GVariant *)property_list_iter->data, NULL);

            property_val_remain_len = sizeof(connected_property_val) - strlen(connected_property_val);

            break_do_info_if_fail(property_val_remain_len > 0, debug_log(DLOG_ERROR,
                "%s failed: no enough property val len for [%s:%s]", __FUNCTION__, obj_name, property_name));

            
            ret = dal_get_property_value_string(obj_handle_iter, property_name,
                connected_property_val + strlen(connected_property_val), property_val_remain_len);
            break_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR,
                "%s failed: cann't get property iter value for [%s:%s]", __FUNCTION__, obj_name, property_name));

            property_list_iter = property_list_iter->next;
        }

        
        continue_do_info_if_fail(NULL == property_list_iter, debug_log(DLOG_ERROR,
            "%s failed: connect property list val failed for object [%s]", __FUNCTION__, obj_name));

        (void)dal_clear_string_blank(connected_property_val, sizeof(connected_property_val));

        if (0 == g_ascii_strcasecmp(connected_property_val, rsc_id)) {
            break;
        }
    }

    g_slist_free(obj_list);

    if (NULL != obj_list_iter) {
        *obj_handle = obj_handle_iter;
    } else {
        debug_log(DLOG_ERROR, "%s failed: check member id with [%s] failed for class [%s]", __FUNCTION__, rsc_id,
            class_name);

        return VOS_ERR;
    }

    return VOS_OK;
}


gint32 rf_get_rsc_odata_prop(const gchar *uri_template, const gchar *rsc_id, gchar *o_odata_prop,
    guint32 o_odata_prop_len)
{
    int iRet;
    gint32 ret;
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    if (NULL == uri_template || NULL == o_odata_prop || 0 == o_odata_prop_len) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return VOS_ERR;
    }

    ret = redfish_get_board_slot(slot, sizeof(slot));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot failed.", __FUNCTION__);
        return VOS_ERR;
    }

    if (NULL == rsc_id) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
        iRet = snprintf_s(o_odata_prop, o_odata_prop_len, o_odata_prop_len - 1, uri_template, slot);
#pragma GCC diagnostic pop
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    } else {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
        iRet = snprintf_s(o_odata_prop, o_odata_prop_len, o_odata_prop_len - 1, uri_template, slot, rsc_id);
#pragma GCC diagnostic pop
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    }

    return VOS_OK;
}

gint32 get_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    gchar *uri_string)
{
    gint32 ret;
    gchar odata_context[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    return_val_do_info_if_fail((NULL != o_result_jso && NULL != o_message_jso &&
        VOS_OK == provider_paras_check(i_paras) && NULL != uri_string),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, uri_string, slot_id);
#pragma GCC diagnostic pop
    return_val_do_info_if_expr(VOS_OK >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : snprintf_s uri fail", __FUNCTION__));

    *o_result_jso = json_object_new_string((const gchar *)odata_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

gint32 get_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    gchar *uri_string)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras) ||
        NULL == uri_string) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, uri_string, slot);
#pragma GCC diagnostic pop
    return_val_do_info_if_expr(VOS_OK >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : snprintf_s uri fail", __FUNCTION__));

    *o_result_jso = json_object_new_string((const gchar *)uri);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


gint32 ret_value_judge(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, gint32 ret)
{
    switch (ret) {
        case RFERR_SUCCESS:
        case VOS_OK:
            return HTTP_OK;

        
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_NOT_FOUND;

        
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


gint32 get_ums_server_flag(guint8 *ums_server_flag)
{
    static OBJ_HANDLE ums_handle = 0;

    if (ums_handle == 0) {
        if (dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &ums_handle) != DFL_OK) {
            return VOS_ERR;
        }
    }

    if (dal_get_property_value_byte(ums_handle, UMS_PROPERTY_SERV_FLG, ums_server_flag) != DFL_OK) {
        return VOS_ERR;
    }

    return VOS_OK;
}


gint32 get_installable_bma_state(void)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guint8 support_state = 0xFF;
    guint8 connect_state = 0xFF;
    GSList *input_list = NULL;
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMA"};

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMA_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name fail.", __FUNCTION__);
    }

    ret = dal_get_object_handle_nth(CLASS_IBMA_UMS_NAME, 0, &obj_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "%s: get the %s ums object fail, ret=%d.", __FUNCTION__, custom_name, ret);
        return VOS_ERR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_IBMA_UMS_STATE, &support_state);
    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "%s, get %s state fail. ret=%d", __FUNCTION__, custom_name, ret);
        return VOS_ERR;
    }
    
    if ((support_state != IBMA_SUPPORT_STATE_ENABLE) && (support_state != IBMA_SUPPORT_STATE_NO_PACKET)) {
        debug_log(DLOG_DEBUG, "%s, the %s don't support. state=%d", __FUNCTION__, custom_name, support_state);
        return BMA_PART_TABLE_NOT_EXIST;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_IBMA_UMS_CONNECT_STATE, &connect_state);
    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "%s, get %s connect state fail. ret=%d", __FUNCTION__, custom_name, ret);
        return VOS_ERR;
    }
    
    if ((connect_state == UMS_CONNECT_STATE_CONNECTED) || (connect_state == UMS_CONNECT_STATE_DISCONNECT_FAIL)) {
        return BMA_IS_WORKING;
    }
    
    input_list = g_slist_append(input_list, g_variant_new_byte(UMS_IBMA_FILE_ID_ALL));
    ret = uip_call_class_method_redfish(REDFISH_REQ_FROM_OTHER_CLIENT, "", "", obj_handle, CLASS_IBMA_UMS_NAME,
        UMS_METHOD_CHECK_FILE_STATE, AUTH_DISABLE, USERROLE_BASICSETTING, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "%s: file do not exist, ret = %d", __FUNCTION__, ret);
        return BMA_FILE_NOT_EXIST;
    }

    return VOS_OK;
}


gint32 rf_check_para_and_priv(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return_val_do_info_if_expr(
        ((o_result_jso == NULL) || (o_message_jso == NULL) || (provider_paras_check(i_paras) != VOS_OK)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    return_val_do_info_if_expr(i_paras->is_satisfy_privi == 0, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return VOS_OK;
}


gint32 rf_get_product_id(guint32 *product_id)
{
    
    if (dal_is_customized_machine()) {
        OBJ_HANDLE product_handle = OBJ_HANDLE_COMMON;
        (void)dfl_get_object_handle(BMC_PRODUCT_NAME_APP, &product_handle);
        return dal_get_property_value_uint32(product_handle, BMC_CB_PRODUCT_ID_NAME, product_id);
    } else {
        return dal_get_product_id(product_id);
    }
}


gboolean check_pangea_node_to_enc(OBJ_HANDLE handle)
{
    guint8  software_id = 0xff;
    guchar  component_type = 0;

    (void)dal_get_software_type(&software_id);
    if (software_id != MGMT_SOFTWARE_TYPE_PANGEA_PACIFIC_CTRL &&
        software_id != MGMT_SOFTWARE_TYPE_PANGEA_ARCTIC_CTRL) {
        return FALSE;
    }

    
    (void)dal_get_property_value_byte(handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
    switch (component_type) {
        case COMPONENT_TYPE_CHASSIS_BACKPLANE:
        case COMPONENT_TYPE_MAINBOARD:
            return TRUE;
        default:
            return FALSE;
    }
}


gboolean check_pangea_uri_in_blade(const gchar* pattern, const gchar* string)
{
    gboolean ret;
    gchar*  tmp_string = NULL;

    if (!dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
        return FALSE;
    }

    if (strlen(string) == 0) {
        debug_log(DLOG_ERROR, "%s: input string is null", __FUNCTION__);
        return FALSE;
    }
    tmp_string = g_ascii_strdown(string, strlen(string));
    if (tmp_string == NULL) {
        debug_log(DLOG_ERROR, "%s: string is NULL.", __FUNCTION__);
        return FALSE;
    }
    ret = g_regex_match_simple(pattern, tmp_string, (GRegexCompileFlags)0, (GRegexMatchFlags)0);
    if (ret != TRUE) {
        debug_log(DLOG_DEBUG, "%s: g_regex_match_simple failed", __FUNCTION__);
    }
    g_free(tmp_string);
    return ret;
}

gint32 rf_gen_rsc_id_with_location_and_devicename(OBJ_HANDLE object_handle, gchar *output_str, guint32 str_len)
{
    gint32 ret;
    gchar dev_name[MAX_PROP_NAME] = {0};
    gchar location[MAX_PROP_NAME] = {0};

    if (str_len < 1) {
        debug_log(DLOG_ERROR, "%s: str_len is error, str_len = %u", __FUNCTION__, str_len);
        return RET_ERR;
    }

    (void)dal_get_property_value_string(object_handle, PROPERTY_LOCATION, location, sizeof(location));
    (void)dal_get_property_value_string(object_handle, PROPERTY_DEVICE_NAME, dev_name, sizeof(dev_name));

    dal_clear_string_blank(dev_name, sizeof(dev_name));
    dal_clear_string_blank(location, sizeof(location));

    ret = snprintf_s(output_str, str_len, str_len - 1, "%s%s", location, dev_name);
    if (ret <= 0) {
        debug_log(DLOG_MASS, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 get_rack_node_inner_ipv4_mask(guint32 *ipv4_addr_digit, guint32 *ipv4_mask_digit, guint32 inner_net_num,
    guint32 *count)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    guint8 out_type;
    gchar ipv4_addr[RACK_NODE_INNER_NET_MAX_NUM][IPV4_IP_STR_SIZE + 1] = { 0 };
    gchar ipv4_mask[RACK_NODE_INNER_NET_MAX_NUM][IPV4_MASK_STR_SIZE + 1] = { 0 };

    if ((ipv4_addr_digit == NULL) || (ipv4_mask_digit == NULL) || (inner_net_num == 0) || (count == NULL)) {
        debug_log(DLOG_ERROR, "%s : input param err", __FUNCTION__);
        return VOS_ERR;
    }

    ret = dfl_get_object_list(ETH_CLASS_NAME_ETHGROUP, &obj_list);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s : Failed to get object list of %s, ret = %d", __FUNCTION__, ETH_CLASS_NAME_ETHGROUP,
            ret);
        return VOS_ERR;
    }

    *count = 0;
    for (obj_note = obj_list; obj_note != NULL; obj_note = obj_note->next) {
        out_type = 0;
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, ETH_GROUP_ATTRIBUTE_OUT_TYPE, &out_type);
        if (out_type == RACK_NODE_INNER_ETHERNET) {
            if (*count >= inner_net_num) {
                g_slist_free(obj_list);
                return VOS_ERR;
            }
            (void)dal_get_property_value_string((OBJ_HANDLE)obj_note->data, ETH_GROUP_ATTRIBUTE_IP_ADDR,
                ipv4_addr[*count], IPV4_IP_STR_SIZE + 1);
            (void)dal_get_property_value_string((OBJ_HANDLE)obj_note->data, ETH_GROUP_ATTRIBUTE_SUB_MASK,
                ipv4_mask[*count], IPV4_MASK_STR_SIZE + 1);

            (void)inet_pton(AF_INET, ipv4_addr[*count], &ipv4_addr_digit[*count]);
            (void)inet_pton(AF_INET, ipv4_mask[*count], &ipv4_mask_digit[*count]);

            debug_log(DLOG_INFO, "%s : IPv4[%s], IPv4 digital[0x%08x]", __FUNCTION__, ipv4_addr[*count],
                ipv4_addr_digit[*count]);
            debug_log(DLOG_INFO, "%s : Subnet Mask[%s], Subnet Mask digital[0x%08x]", __FUNCTION__, ipv4_mask[*count],
                ipv4_mask_digit[*count]);

            *count = (*count) + 1;
        }
    }
    g_slist_free(obj_list);

    if ((*count) == 0) {
        return VOS_ERR_NOTSUPPORT;
    }
    return VOS_OK;
}


gboolean is_the_request_from_rack_inner_device(const gchar *client)
{
    gint32 ret;
    guint32 i;
    guint32 client_digit = 0;
    guint32 ip_num = 0;
    guint32 ipv4_addr_digit[RACK_NODE_INNER_NET_MAX_NUM] = { 0 };
    guint32 ipv4_mask_digit[RACK_NODE_INNER_NET_MAX_NUM] = { 0 };

    if (client == NULL) {
        return FALSE;
    }

    debug_log(DLOG_INFO, "%s : client[%s]", __FUNCTION__, client);

    ret = get_rack_node_inner_ipv4_mask(ipv4_addr_digit, ipv4_mask_digit, RACK_NODE_INNER_NET_MAX_NUM, &ip_num);
    if (ret != VOS_OK) {
        return FALSE;
    }

    (void)inet_pton(AF_INET, client, &client_digit);
    for (i = 0; i < ip_num; ++i) {
        if ((ipv4_addr_digit[i] & ipv4_mask_digit[i]) == (client_digit & ipv4_mask_digit[i])) {
            
            return TRUE;
        }
    }
    return FALSE;
}


LOCAL void set_irm_inner_access_node_timestamp(RF_INTERFACE_TYPE_E type)
{
    gint32 ret;
    OBJ_HANDLE bmc_handle = 0;
    GSList *input_list = NULL;
    guint8 timestamp_type;

    ret = dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &bmc_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "%s : Failed to get object handle of class[%s], ret = %d", __FUNCTION__, BMC_CLASEE_NAME,
            ret);
        return;
    }

    timestamp_type = (guint8)type;

    input_list = g_slist_append(input_list, g_variant_new_byte(timestamp_type));
    ret = dfl_call_class_method(bmc_handle, METHOD_UPDATE_IRM_COMM_TIMESTAMP, NULL, input_list, NULL);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s : Call class method %s failed, ret = %d", __FUNCTION__,
            METHOD_UPDATE_IRM_COMM_TIMESTAMP, ret);
    }
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
}


void update_irm_inner_access_node_timestamp(const gchar *client, RF_INTERFACE_TYPE_E type)
{
    if (is_the_request_from_rack_inner_device(client) == TRUE) {
        
        set_irm_inner_access_node_timestamp(type);
    }
}

gint32 is_valid_integer_jso_or_null(json_object* jso, const gchar* prop_name,
    gint64 lower_limit, gint64 upper_limit, json_object** o_message_jso)
{
    gint64 origin_num;

    if (jso == NULL) {
        return RET_OK;
    }

    
    if (json_object_get_type(jso) != json_type_int) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, prop_name,
            o_message_jso, dal_json_object_get_str(jso), prop_name);
        return HTTP_BAD_REQUEST;
    }

    origin_num = json_object_get_int64(jso);
    
    if ((origin_num < lower_limit) || (origin_num > upper_limit)) {
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, prop_name,
            o_message_jso, dal_json_object_get_str(jso), prop_name);
        return HTTP_BAD_REQUEST;
    }

    return RET_OK;
}


gint32 get_ld_id_from_uri(guint8 controller_id, const gchar *i_obj_json_string, guint16 *ld_id)
{
    guint16 logical_id = 0;
    guint8 logical_controller_id = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    gchar str_buf[MAX_STRBUF_LEN + 1] = {0};
    OBJ_HANDLE controller_obj_handle = 0;
    gchar slot[MAX_RSC_ID_LEN + 1] = {0};
    gchar logical_of_controller[MAX_STRBUF_LEN + 1] = {0};

    if (i_obj_json_string == NULL || ld_id == NULL) {
        return RET_ERR;
    }

    if (redfish_get_board_slot(slot, sizeof(slot)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get slot id error.", __FUNCTION__);
        return RET_ERR;
    }

    
    gint32 ret = dfl_get_object_list(CLASS_LOGICAL_DRIVE_NAME, &obj_list);
    if (ret != DFL_OK || obj_list == NULL) {
        debug_log(DLOG_ERROR, "%s: get obj_list fail.", __FUNCTION__);
        return RET_ERR;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_REF_RAID_CONTROLLER,
            logical_of_controller, sizeof(logical_of_controller));
        (void)dfl_get_object_handle(logical_of_controller, &controller_obj_handle);
        (void)dal_get_property_value_byte(controller_obj_handle, PROPERTY_RAID_CONTROLLER_ID, &logical_controller_id);

        
        (void)dal_get_property_value_uint16((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_TARGET_ID, &logical_id);

        // 比较控制器id
        if (controller_id != logical_controller_id) {
            continue;
        }

        (void)memset_s(str_buf, sizeof(str_buf), 0, sizeof(str_buf));
        if (snprintf_s(str_buf, sizeof(str_buf), sizeof(str_buf) - 1, RFPROP_SYSTEM_REVOLUME, slot, controller_id,
            logical_id) <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s failed.", __FUNCTION__);
            continue;
        }

        if (g_ascii_strcasecmp(str_buf, i_obj_json_string) == 0) {
            g_slist_free(obj_list);
            *ld_id = logical_id;
            return RET_OK;
        }
    }

    g_slist_free(obj_list);
    return RET_ERR;
}
