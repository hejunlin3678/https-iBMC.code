
#include <dirent.h>
#include "redfish_provider.h"
LOCAL gint32 get_jsonchemas_members_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_jsonchemas_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);


LOCAL PROPERTY_PROVIDER_S g_jsonschemas_provider[] = {
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, RFPROP_ODATA_ID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_jsonchemas_members_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, RFPROP_ODATA_ID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_jsonchemas_members, NULL, NULL, ETAG_FLAG_ENABLE},
};


LOCAL gint32 get_jsonchemas_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gchar jsonchemas_path[MAX_PATH_LENGTH] = {0};
    gchar jsonchemas_file_patch[MAX_PATH_LENGTH] = {0};
    gint32 ret;
    json_object *members_jso = NULL;
    gchar *uri_lower = NULL;
    gchar path_lower[MAX_PATH_LENGTH] = {0};
    DIR *dirp = NULL;
    DIR *dp = NULL;
    struct dirent *ptr = NULL;
    gchar jsonchemas_file[MAX_PATH_LENGTH] = {0};
    json_object *redfish_jso = NULL;
    json_object *resource_jso = NULL;
    json_bool ret_json;
    
    return_val_do_info_if_expr(provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s %d", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    uri_lower = g_ascii_strdown(i_paras->uri, strlen(i_paras->uri));
    return_val_do_info_if_fail(uri_lower != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:g_ascii_strdown fail.", __FUNCTION__, __LINE__));
    ret = snprintf_s(path_lower, MAX_PATH_LENGTH, MAX_PATH_LENGTH - 1, "%s", uri_lower);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR, g_free(uri_lower);
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));
    g_free(uri_lower);

    ret = snprintf_s(jsonchemas_file_patch, MAX_PATH_LENGTH, MAX_PATH_LENGTH - 1, "%s%s", JSONCHEMAS_CONFG_FILE_PREFIX,
        path_lower);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));

    dirp = opendir(jsonchemas_file_patch);
    return_val_do_info_if_fail(dirp != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:opendir fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, closedir(dirp);
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_array fail. ", __FUNCTION__, __LINE__));

    while ((ptr = readdir(dirp)) != NULL) {
        ret = snprintf_s(jsonchemas_file, MAX_PATH_LENGTH, MAX_PATH_LENGTH - 1, "%s%s/%s", JSONCHEMAS_CONFG_FILE_PREFIX,
            path_lower, ptr->d_name);
        return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR, closedir(dirp);
            debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));

        

        if (NULL != (dp = opendir(jsonchemas_file))) {
            closedir(dp);
            if (0 == g_strcmp0(".", ptr->d_name) || 0 == g_strcmp0("..", ptr->d_name)) {
                continue;
            }
            
            ret = snprintf_s(jsonchemas_path, sizeof(jsonchemas_path), sizeof(jsonchemas_path) - 1,
                JSONCHEMAS_PROVIDER_CONFG_FILE, ptr->d_name);
            return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR, closedir(dirp);
                debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));
            
            resource_jso = json_object_from_file(jsonchemas_path);
            return_val_do_info_if_fail(resource_jso != NULL, HTTP_INTERNAL_SERVER_ERROR, closedir(dirp);
                debug_log(DLOG_ERROR, "%s,%d:json_object_from_file fail.", __FUNCTION__, __LINE__));
            
            ret_json = json_object_object_get_ex(resource_jso, RFPROP_ODATA_ID, &redfish_jso);
            return_val_do_info_if_fail(ret_json, HTTP_INTERNAL_SERVER_ERROR, json_object_put(resource_jso);
                closedir(dirp); debug_log(DLOG_ERROR, "%s,%d:json_object_object_get_ex fail.", __FUNCTION__, __LINE__));

            
            members_jso = json_object_new_object();
            return_val_do_info_if_expr(members_jso == NULL, HTTP_INTERNAL_SERVER_ERROR, json_object_put(resource_jso);
                closedir(dirp);
                debug_log(DLOG_ERROR, "%s,%d:   json_object_new_object fail. ", __FUNCTION__, __LINE__));

            json_object_object_add(members_jso, RFPROP_ODATA_ID, json_object_get(redfish_jso));

            ret = json_object_array_add(*o_result_jso, members_jso);
            
            do_val_if_expr(ret != VOS_OK, json_object_put(members_jso);
                debug_log(DLOG_ERROR, "%s %d:json_object_array_add fail", __FUNCTION__, __LINE__));
            json_object_put(resource_jso);
            
        }
    }

    closedir(dirp);
    return HTTP_OK;
}



LOCAL gint32 get_jsonchemas_members_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar jsonchemas_file_patch[MAX_PATH_LENGTH] = {0};
    gchar jsonchemas_file[MAX_PATH_LENGTH] = {0};
    gint32 ret;
    gint32 jsonchemas_count = 0;
    gchar *uri_lower = NULL;
    DIR *dirp = NULL;
    DIR *dp = NULL;
    struct dirent *ptr = NULL;

    
    return_val_do_info_if_expr(provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s %d", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    uri_lower = g_ascii_strdown(i_paras->uri, strlen(i_paras->uri));
    return_val_do_info_if_fail(uri_lower != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:g_ascii_strdown fail.", __FUNCTION__, __LINE__));

    ret = snprintf_s(jsonchemas_file_patch, MAX_PATH_LENGTH, MAX_PATH_LENGTH - 1, "%s%s", JSONCHEMAS_CONFG_FILE_PREFIX,
        uri_lower);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR, g_free(uri_lower);
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));
    
    dirp = opendir(jsonchemas_file_patch);
    return_val_do_info_if_fail(dirp != NULL, HTTP_INTERNAL_SERVER_ERROR, g_free(uri_lower);
        debug_log(DLOG_ERROR, "%s,%d:opendir fail.", __FUNCTION__, __LINE__));

    while ((ptr = readdir(dirp)) != NULL) {
        ret = snprintf_s(jsonchemas_file, MAX_PATH_LENGTH, MAX_PATH_LENGTH - 1, "%s%s/%s", JSONCHEMAS_CONFG_FILE_PREFIX,
            uri_lower, ptr->d_name);
        return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR, closedir(dirp); g_free(uri_lower);
            debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));

        
        if (g_strcmp0(".", ptr->d_name) == 0 || g_strcmp0("..", ptr->d_name) == 0) {
            continue;
        }
        
        dp = opendir(jsonchemas_file);
        if (dp == NULL) {
            continue;
        }

        jsonchemas_count++;
        closedir(dp);
    }

    closedir(dirp);
    g_free(uri_lower);

    *o_result_jso = json_object_new_int(jsonchemas_count);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_int fail.", __FUNCTION__, __LINE__));
    return HTTP_OK;
}


gint32 provider_jsonschemas_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    
    return_val_if_fail((i_paras != NULL) && (prop_provider != NULL) && (count != NULL), HTTP_INTERNAL_SERVER_ERROR);

    *prop_provider = g_jsonschemas_provider;
    *count = sizeof(g_jsonschemas_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
