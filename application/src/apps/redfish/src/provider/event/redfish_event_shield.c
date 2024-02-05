

#include "redfish_provider.h"
#include "redfish_event.h"

#define EVENT_SHIELD_CONFIG_FILE    "/data/opt/pme/conf/redfish/event_sheild_config.json"
#define EVENT_SHIELD_CONFIG_BAK_FILE "/dev/shm/event_sheild_config.json"

typedef enum {
    SHIELD_OPERATION_SET_OK = 0,
    SHIELD_OPERATION_SET_ERR,
    SHIELD_OPERATION_CLEAR_OK
} SHIELD_OPERATION_RESULT;


LOCAL json_object *g_shield_resources = NULL;

LOCAL pthread_mutex_t g_event_shield_mutex = PTHREAD_MUTEX_INITIALIZER;

void backup_event_shield_resources_conf(void)
{
    
    if (vos_get_file_accessible(EVENT_SHIELD_CONFIG_FILE) != TRUE ||
        vos_get_file_length(EVENT_SHIELD_CONFIG_FILE) == 0) {
        debug_log(DLOG_ERROR, "%s: %s not exist", __FUNCTION__, EVENT_SHIELD_CONFIG_FILE);
        return;
    }

    
    gint32 ret = vos_file_copy(EVENT_SHIELD_CONFIG_BAK_FILE, EVENT_SHIELD_CONFIG_FILE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: vos_file_copy failed, ret = %d", __FUNCTION__, ret);
    }
    return;
}

void restore_event_shield_resources_conf(void)
{
    
    if (vos_get_file_accessible(EVENT_SHIELD_CONFIG_BAK_FILE) != TRUE ||
        vos_get_file_length(EVENT_SHIELD_CONFIG_BAK_FILE) == 0) {
        debug_log(DLOG_ERROR, "%s: %s not exist", __FUNCTION__, EVENT_SHIELD_CONFIG_BAK_FILE);
        return;
    }

    
    gint32 ret = vos_file_copy(EVENT_SHIELD_CONFIG_FILE, EVENT_SHIELD_CONFIG_BAK_FILE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: vos_file_copy failed, ret = %d", __FUNCTION__, ret);
        return;
    }

    
    vos_rm_filepath(EVENT_SHIELD_CONFIG_BAK_FILE);
    return;
}

LOCAL void record_shield_resources_operation_log(const gchar *user_name, const gchar *client,
    SHIELD_OPERATION_RESULT oper_type)
{
    GSList *caller_info = NULL;

    caller_info = g_slist_append(caller_info, g_variant_new_string(RFPROP_LOGIN_INTERFACE_REDFISH));
    caller_info = g_slist_append(caller_info, g_variant_new_string(user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(client));
    if (oper_type == SHIELD_OPERATION_SET_OK) {
        proxy_method_operation_log(caller_info, RFPROP_LOGIN_INTERFACE_REDFISH,
            "Configure resources shielded in event reporting successfully");
    } else if (oper_type == SHIELD_OPERATION_SET_ERR) {
        proxy_method_operation_log(caller_info, RFPROP_LOGIN_INTERFACE_REDFISH,
            "Configure resources shielded in event reporting failed");
    } else {
        proxy_method_operation_log(caller_info, RFPROP_LOGIN_INTERFACE_REDFISH,
            "Clear resources shielded in event reporting successfully");
    }

    uip_free_gvariant_list(caller_info);
    return;
}


LOCAL gint32 write_event_shield_resources_conf(json_object *cfg_json)
{
    gint32 ret;
    _cleanup_json_object_ json_object *shield_jso = NULL;
    const gchar* shield_str = NULL;
    _cleanup_fclose_ FILE *fp = NULL;

    shield_jso = json_object_new_object();
    if (shield_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__);
        return RET_ERR;
    }

    json_object_object_add(shield_jso, RFPROP_EVT_SHIELD_RESOURCE, json_object_get(cfg_json));
    shield_str = dal_json_object_get_str(shield_jso);
    if (shield_str == NULL) {
        debug_log(DLOG_ERROR, "%s: get shield str failed", __FUNCTION__);
        return RET_ERR;
    }

    
    fp = g_fopen(EVENT_SHIELD_CONFIG_FILE, "w+");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "%s: open file failed", __FUNCTION__);
        return RET_ERR;
    }

    (void)fchmod(fileno(fp), (S_IRUSR | S_IWUSR));
    (void)fchown(fileno(fp), REDFISH_USER_UID, APPS_USER_GID);

    ret = fwrite(shield_str, 1, strlen(shield_str), fp);
    if (ret != strlen(shield_str)) {
        debug_log(DLOG_ERROR, "%s: write file failed", __FUNCTION__);
        return RET_ERR;
    }

    (void)fflush(fp);
    (void)fsync(fileno(fp));
    return RET_OK;
}


void get_event_shielded_resources_conf(void)
{
    json_object *cfg_jso = NULL;
    json_object *obj_json = NULL;

    
    if (vos_get_file_accessible(EVENT_SHIELD_CONFIG_FILE) != TRUE ||
        vos_get_file_length(EVENT_SHIELD_CONFIG_FILE) == 0) {
        debug_log(DLOG_DEBUG, "%s: %s not exist", __FUNCTION__, EVENT_SHIELD_CONFIG_FILE);
        return;
    }

    
    cfg_jso = json_object_from_file((const gchar*)EVENT_SHIELD_CONFIG_FILE);
    if (cfg_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_from_file error", __FUNCTION__);
        return;
    }

    json_object_object_get_ex(cfg_jso, RFPROP_EVT_SHIELD_RESOURCE, &obj_json);
    if (obj_json == NULL) {
        debug_log(DLOG_ERROR, "%s: get %s object error", __FUNCTION__, RFPROP_EVT_SHIELD_RESOURCE);
        json_object_put(cfg_jso);
        return;
    }

    
    pthread_mutex_lock(&g_event_shield_mutex);
    if (g_shield_resources != NULL) {
        json_object_put(g_shield_resources);
    }
    g_shield_resources = json_object_get(obj_json);
    pthread_mutex_unlock(&g_event_shield_mutex);

    json_object_put(cfg_jso);
    return;
}


LOCAL void delete_json_object_ex(json_object *rsc_jso, gchar *segment_info[], gint32 segment_num)
{
    gint32 index;
    json_object *segment_obj = rsc_jso;
    json_object *tmp_obj = NULL;
    json_object *item_jso = NULL;
    gint32 array_len;
    gint32 array_index;

    for (index = 0; index < segment_num; index++) {
        
        if (json_object_object_get_ex(segment_obj, segment_info[index], &tmp_obj) != TRUE) {
            debug_log(DLOG_DEBUG, "%s: not find attributes(%s).", __FUNCTION__, segment_info[index]);
            return;
        }

        
        if (index == segment_num - 1) {
            json_object_object_del(segment_obj, segment_info[index]);
            debug_log(DLOG_DEBUG, "%s: delete json object(%s).", __FUNCTION__, segment_info[segment_num - 1]);
            return;
        }

        
        if (json_object_get_type(tmp_obj) == json_type_array) {
            array_len = json_object_array_length(tmp_obj);
            for (array_index = 0; array_index < array_len; array_index++) {
                item_jso = json_object_array_get_idx(tmp_obj, array_index);
                delete_json_object_ex(item_jso, &segment_info[index + 1], segment_num - index - 1);
            }
            break;
        }
        segment_obj = tmp_obj;
    }
}


LOCAL gchar** split_shield_resource_string(const gchar *shield_str, gint32 *segment_cnt)
{
    gint32 segment_num;
    _cleanup_gstrv_ gchar **shield_segments = NULL;

    
    shield_segments = g_strsplit(shield_str, "/", -1);
    if (shield_segments == NULL) {
        debug_log(DLOG_ERROR, "%s: g_strsplit failed.", __FUNCTION__);
        return NULL;
    }

    segment_num = g_strv_length(shield_segments);
    if (segment_num == 0) {
        debug_log(DLOG_ERROR, "%s: g_strv_length = 0.", __FUNCTION__);
        return NULL;
    }

    if (*shield_segments[segment_num - 1] == '\0') {
        segment_num--;
    }

    *segment_cnt = segment_num;
    return TAKE_PTR(shield_segments);
}


LOCAL void delete_shield_attributes(const gchar* attribute_name, json_object *rsc_jso)
{
    _cleanup_gstrv_ gchar **attribute_segments = NULL;
    gint32 segment_num;

    attribute_segments = split_shield_resource_string(attribute_name, &segment_num);
    if (attribute_segments == NULL) {
        debug_log(DLOG_ERROR, "%s: segments shield resource attributes failed.", __FUNCTION__);
        return;
    }

    delete_json_object_ex(rsc_jso, attribute_segments, segment_num);
    return;
}


LOCAL gboolean is_shield_resources_uri(const gchar* rsc_url, const gchar *shield_url)
{
    _cleanup_gstrv_ gchar **shield_segments = NULL;
    _cleanup_gstrv_ gchar **rsc_segments = NULL;
    gint32 rsc_segments_num = 0;
    gint32 shield_segments_num = 0;
    gint32 index;

    shield_segments = split_shield_resource_string(shield_url, &shield_segments_num);
    if (shield_segments == NULL) {
        debug_log(DLOG_ERROR, "%s: segments shield resource url failed.", __FUNCTION__);
        return FALSE;
    }

    rsc_segments = split_shield_resource_string(rsc_url, &rsc_segments_num);
    if (rsc_segments == NULL) {
        debug_log(DLOG_ERROR, "%s: segments shield resource url failed.", __FUNCTION__);
        return FALSE;
    }

    
    if (rsc_segments_num != shield_segments_num) {
        return FALSE;
    }

    for (index = 0; index < shield_segments_num; index++) {
        
        if (g_strcmp0(shield_segments[index], "*") == 0) {
            continue;
        }

        
        if (g_ascii_strcasecmp(shield_segments[index], rsc_segments[index]) != 0) {
            return FALSE;
        }
    }

    return TRUE;
}


gint32 delete_shield_resources(const gchar* url, json_object **rsc_jso, RSC_EVT_SHIELD_TYPE *sheild_type)
{
    json_object *item_jso = NULL;
    json_object *temp_json = NULL;
    json_object *attribute_jso = NULL;
    const gchar *rsc_uri = NULL;
    const gchar *attribute_str = NULL;

    if (url == NULL || rsc_jso == NULL || *rsc_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: input param err.", __FUNCTION__);
        return RET_ERR;
    }

    pthread_mutex_lock(&g_event_shield_mutex);
    if (g_shield_resources == NULL) {
        pthread_mutex_unlock(&g_event_shield_mutex);
        *sheild_type = EVT_SHIELD_NONE;
        return RET_OK;
    }

    *sheild_type = EVT_SHIELD_ATTRUBITES;

    
    json_object_array_foreach(g_shield_resources, item_jso) {
        rsc_uri = NULL;
        get_element_str(item_jso, RFPROP_EVT_SHIELD_RESOURCE_PATH, &rsc_uri);
        if (is_shield_resources_uri(url, rsc_uri) != TRUE) {
            continue;
        }

        
        json_object_object_get_ex(item_jso, RFPROP_ATTRIBUTES, &temp_json);
        if (temp_json == NULL) {
            pthread_mutex_unlock(&g_event_shield_mutex);
            
            *sheild_type = EVT_SHIELD_RESOURCE;
            return RET_OK;
        }

        
        json_object_array_foreach(temp_json, attribute_jso) {
            attribute_str = dal_json_object_get_str(attribute_jso);
            if (attribute_str == NULL) {
                continue;
            }

            
            delete_shield_attributes(attribute_str, *rsc_jso);
        }
    }
    pthread_mutex_unlock(&g_event_shield_mutex);
    return RET_OK;
}


LOCAL void clear_event_shield_resources_conf(void)
{
    
    pthread_mutex_lock(&g_event_shield_mutex);
    if (g_shield_resources != NULL) {
        json_object_put(g_shield_resources);
        g_shield_resources = NULL;
    }
    pthread_mutex_unlock(&g_event_shield_mutex);

    
    vos_rm_filepath(EVENT_SHIELD_CONFIG_FILE);
    return;
}


LOCAL gint32 check_shield_resources_odata(PROVIDER_PARAS_S *i_paras, 
    json_object *item_jso, const gchar* array_name, json_object *message_array)
{
    gchar prop_name[MAX_RSC_NAME_LEN] = {0};
    json_object *temp_json = NULL;
    json_object *message = NULL;

    (void)snprintf_truncated_s(prop_name, sizeof(prop_name), "%s/Resource", array_name);

    
    json_object_object_get_ex(item_jso, RFPROP_EVT_SHIELD_RESOURCE_PATH, &temp_json);
    if (temp_json == NULL) {
        
        if (strstr(dal_json_object_to_json_string(i_paras->delete_data), prop_name) != NULL) {
            debug_log(DLOG_DEBUG, "%s: %s has been deleted", __FUNCTION__, prop_name);
            return RET_ERR;
        }
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex failed.", __FUNCTION__);
        create_message_info(MSGID_PROP_MISSING, (const gchar*)prop_name, &message, (const gchar*)prop_name);
        json_object_array_add(message_array, message);
        return RET_ERR;
    }

    
    if (json_object_get_string_len(temp_json) > MAX_URI_LENGTH) {
        debug_log(DLOG_ERROR, "%s: json_object_get_string_len > %d .", __FUNCTION__, MAX_URI_LENGTH);
        create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, (const gchar *)prop_name, &message, 
            dal_json_object_get_str(temp_json), (const gchar *)prop_name, MAX_URI_LENGTH_STR);
        json_object_array_add(message_array, message);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 check_attributes_param_valid(json_object* tmp_json, gint32 array_index, const gchar* prop_name,
    json_object** message)
{
    gchar index_name[MAX_RSC_NAME_LEN] = {0}; 
    const gchar* json_str = NULL;
    gint32 prop_len;
    json_object *array_json = NULL;

    memset_s(index_name, sizeof(index_name), 0, sizeof(index_name));
    (void)snprintf_truncated_s(index_name, sizeof(index_name), "%s/%d", prop_name, array_index);

    array_json = json_object_array_get_idx(tmp_json, array_index);
    if (array_json == NULL) {
        create_message_info(MSGID_PROP_TYPE_ERR, (const gchar*)index_name, message, RF_VALUE_NULL, 
            (const gchar*)index_name);
        debug_log(DLOG_DEBUG, "%s: array json is null", __FUNCTION__);
        return RET_ERR;        
    }
    
    json_str = dal_json_object_get_str(array_json);
    if (json_object_get_type(array_json) != json_type_string) {
        debug_log(DLOG_DEBUG, "%s: attributes param type is mismatch", __FUNCTION__);
        create_message_info(MSGID_PROP_ITEM_TYPE_ERR, (const gchar*)index_name, message, json_str, 
            (const gchar*)index_name);
        return RET_ERR;
    } 
    
    prop_len = json_object_get_string_len(array_json);
    if (prop_len > 0 && prop_len <= MAX_URI_LENGTH) {
        return RET_OK;
    } else if (prop_len == 0) {
        create_message_info(MSGID_PROP_TYPE_ERR, (const gchar*)index_name, message, json_str, 
            (const gchar*)index_name);
        return RET_ERR;
    } else {
        
        create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, (const gchar*)index_name, message, json_str, 
            (const gchar*)index_name, MAX_URI_LENGTH_STR);
        return RET_ERR;
    }
    return RET_OK;    
}


LOCAL gint32 check_shield_resources_attributes(PROVIDER_PARAS_S *i_paras, json_object *item_jso,
    const gchar* array_name, json_object *message_array)
{
    const gint32 MAX_ATTRIBUTE_CNT = 16;
    gint32 array_index;
    json_object *tmp_json = NULL;
    json_object *message = NULL;
    gchar prop_name[MAX_RSC_NAME_LEN] = {0};
    gint32 ret = RET_OK;

    (void)snprintf_truncated_s(prop_name, sizeof(prop_name), "%s/Attributes", array_name);

    
    json_object_object_get_ex(item_jso, RFPROP_ATTRIBUTES, &tmp_json);
    if (tmp_json == NULL) {
        
        if (strstr(dal_json_object_to_json_string(i_paras->delete_data), prop_name) != NULL) {
            debug_log(DLOG_DEBUG, "%s: %s has been deleted", __FUNCTION__, prop_name);
            return RET_ERR;
        }
        return ret;
    }
 
    
    gint32 array_len = json_object_array_length(tmp_json);
    if (array_len > MAX_ATTRIBUTE_CNT) {
        debug_log(DLOG_ERROR, "%s: json object array cnt error(%d >= %d).", __FUNCTION__, array_len, MAX_ATTRIBUTE_CNT);
        create_message_info(MSGID_ARRAY_ITEM_EXCEED, (const gchar *)prop_name, &message, (const gchar *)prop_name);
        json_object_array_add(message_array, message);
        return RET_ERR;
    }

    
    if (array_len == 0) {
        create_message_info(MSGID_PROP_TYPE_ERR, (const gchar*)prop_name, &message, 
            dal_json_object_get_str(tmp_json), (const gchar*)prop_name);
        json_object_array_add(message_array, message);
        return RET_ERR;
    }

    for (array_index = 0; array_index < array_len; array_index++) {
        if (check_attributes_param_valid(tmp_json, array_index, (const gchar*)prop_name, &message) != RET_OK) {
            json_object_array_add(message_array, message);
            ret = RET_ERR;
        }   
    }
    return ret;
}


LOCAL gint32 check_shield_resources_param(PROVIDER_PARAS_S *i_paras, json_object *shield_jso,
    json_object *message_array)
{
    const gchar* prop_name = "Oem/Huawei/ShieldResourcesForSubscriptions";
    const gint32 MAX_RESOURCE_CNT = 128;
    gint32 array_len;
    gint32 array_index;
    json_object *message = NULL;
    json_object *item_jso = NULL;
    gint32 ret;
    gint32 result = RET_OK;
    gchar array_name[MAX_RSC_NAME_LEN] = {0};

    
    array_len = json_object_array_length(shield_jso);
    if (array_len > MAX_RESOURCE_CNT) {
        debug_log(DLOG_ERROR, "%s: json object array cnt error(%d > %d).", __FUNCTION__, array_len, MAX_RESOURCE_CNT);
        create_message_info(MSGID_ARRAY_ITEM_EXCEED, prop_name, &message, prop_name);
        json_object_array_add(message_array, message);
        return RET_ERR;
    }

    
    for (array_index = 0; array_index < array_len; array_index++) {
        (void)snprintf_truncated_s(array_name, sizeof(array_name), "%s/%d", prop_name, array_index);

        item_jso = json_object_array_get_idx(shield_jso, array_index);
        if (item_jso == NULL || json_object_get_type(item_jso) == json_type_null) {
            create_message_info(MSGID_PROP_TYPE_ERR, (const gchar *)array_name, &message, (gchar *)array_name, "null");
            json_object_array_add(message_array, message);
            debug_log(DLOG_ERROR, "%s: %s check failed", __FUNCTION__, array_name);
            result = RET_ERR;
            continue; 
        }

        
        ret = check_shield_resources_odata(i_paras, item_jso, (gchar *)array_name, message_array);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: check %s/Resource failed", __FUNCTION__, array_name);
            result = RET_ERR;
            continue;
        }

        
        ret = check_shield_resources_attributes(i_paras, item_jso, array_name, message_array);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: check %s/Attributes failed", __FUNCTION__, array_name);
            result = RET_ERR;
            continue;
        }
    }

    return result;
}

gint32 set_evt_svc_shield_resources(PROVIDER_PARAS_S *i_paras, json_object *shield_jso, json_object *message_jso)
{
    gint32 ret;

    if (i_paras == NULL || shield_jso == NULL || message_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: param error.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (g_strcmp0(dal_json_object_to_json_string(shield_jso), JSON_NULL_OBJECT_STR) == 0) {
        debug_log(DLOG_DEBUG, "%s: shield_jso is empty object", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }

    
    if (json_object_get_type(shield_jso) == json_type_array && json_object_array_length(shield_jso) == 0) {
        clear_event_shield_resources_conf();
        record_shield_resources_operation_log(i_paras->user_name, i_paras->client, SHIELD_OPERATION_CLEAR_OK);
        return HTTP_OK;
    }

    
    ret = check_shield_resources_param(i_paras, shield_jso, message_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: check shield resources param failed, ret = %d", __FUNCTION__, ret);
        return HTTP_BAD_REQUEST;
    }

    
    ret = write_event_shield_resources_conf(shield_jso);
    if (ret != RET_OK) {
        record_shield_resources_operation_log(i_paras->user_name, i_paras->client, SHIELD_OPERATION_SET_ERR);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    pthread_mutex_lock(&g_event_shield_mutex);
    if (g_shield_resources != NULL) {
        json_object_put(g_shield_resources);
    }
    g_shield_resources = json_object_get(shield_jso);
    pthread_mutex_unlock(&g_event_shield_mutex);

    record_shield_resources_operation_log(i_paras->user_name, i_paras->client, SHIELD_OPERATION_SET_OK);
    return HTTP_OK;
}


void get_evt_svc_shield_resources(json_object *result_jso)
{
    if (result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__);
        return;
    }

    
    if (g_shield_resources == NULL) {
        json_object_object_add(result_jso, RFPROP_EVT_SHIELD_RESOURCE, json_object_new_array());
    } else {
        json_object_object_add(result_jso, RFPROP_EVT_SHIELD_RESOURCE, json_object_get(g_shield_resources));
    }
}