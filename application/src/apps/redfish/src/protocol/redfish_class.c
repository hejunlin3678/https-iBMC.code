

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"

#include "redfish_class.h"
#include "json_checker.h"
#include "redfish_message.h"
#include "redfish_util.h"
#include "json_patch_util.h"

#define VAL_INTEGER_LEN (MAX_JSON_INTEGER + 3) // 加3为了保存...
#define JSON_POINTER_BUF_OVERLONG "OverLong"





static json_object *g_class_definition = NULL;
pthread_mutex_t g_class_def_mutex = PTHREAD_MUTEX_INITIALIZER;

static rf_retvalue _format_check(json_object *jso_schema, json_object *jso_property_schema, json_object *jso_value);
static gint _duplicate_check(const gchar *jso_body_str, const gchar *jso_action_str, json_object *jso_extend_arr,
    JsonError *error);

static rf_retvalue _get_related_property_path(GSList *property_path_list, const char *property_name,
    char json_pointer_path[MAX_JSON_POINTER_PATH_LEN], gint32 json_pointer_path_size);


static const gchar *g_sensitive_word_list[] = {"key", "passwd", "pwd", "password", "token", NULL};


static json_object *_get_global_class_jso(void)
{
    
    if (NULL == g_class_definition) {
        g_class_definition = json_object_new_object();
    }

    return g_class_definition;
    
}


static void _free_global_class_jso(void)
{
    (void)json_object_put(g_class_definition);
    g_class_definition = NULL;

    return;
}


static void _ref_str_to_real_type(const char *type_prefix, const char *ref_str, char *type, guint32 type_len)
{
    int iRet = -1;
    GRegex *regex = NULL;
    GMatchInfo *match_info = NULL;
    gchar *other_type = NULL;
    
    if (g_strrstr(ref_str, SCHEMA_FILE_SUFFIX)) {
        regex = g_regex_new(REF_REGEX_OTHER_FILE, (GRegexCompileFlags)0, (GRegexMatchFlags)0, NULL);
        (void)g_regex_match(regex, ref_str, (GRegexMatchFlags)0, &match_info);

        
        if (3 == g_match_info_get_match_count(match_info)) {
            return_do_info_if_expr(type_len == 0, g_regex_unref(regex);
                debug_log(DLOG_ERROR, "%s failed:input param error, type_len is 0.", __FUNCTION__));
            gchar *other_type_prefix = g_match_info_fetch(match_info, 1);
            other_type = g_match_info_fetch(match_info, 2);
            iRet = snprintf_s(type, type_len, type_len - 1, "%s.%s", other_type_prefix, other_type);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet));
            g_free(other_type_prefix);
            g_free(other_type);
        } else {
            debug_log(DLOG_ERROR, "match %s with %s failed, cann't get type info", ref_str, REF_REGEX_OTHER_FILE);
        }
    } else { 
        regex = g_regex_new(REF_REGEX_LOCAL_FILE, (GRegexCompileFlags)0, (GRegexMatchFlags)0, NULL);
        (void)g_regex_match(regex, ref_str, (GRegexMatchFlags)0, &match_info);

        
        if (2 == g_match_info_get_match_count(match_info)) {
            return_do_info_if_expr(type_len == 0, g_regex_unref(regex);
                debug_log(DLOG_ERROR, "%s failed:input param error, type_len is 0.", __FUNCTION__));
            gchar *local_type = g_match_info_fetch(match_info, 1);
            
            iRet = snprintf_s(type, type_len, type_len - 1, "%s.%s", type_prefix, local_type);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet));
            
            g_free(local_type);
        } else {
            debug_log(DLOG_ERROR, "match %s with %s failed, cann't get type info", ref_str, REF_REGEX_LOCAL_FILE);
        }
    }

    g_match_info_free(match_info);
    g_regex_unref(regex);

    return;
}


static void _reorganize_ref_link(const char *type_prefix, json_object *type_def_jso)
{
    json_type jso_type = json_object_get_type(type_def_jso);
    struct lh_entry *entry_key = NULL;
    struct lh_entry *entry_next_key = NULL;
    struct lh_table *obj_ref = json_object_get_object(type_def_jso);
    struct json_object *val = NULL;

    if (json_type_object == jso_type) {
        json_object *ref_jso = NULL;
        char               ref_type_name[MAX_CLASS_NAME_LEN] = {0};

        (void)json_object_object_get_ex(type_def_jso, JSON_SCHEMA_REFERENCE, &ref_jso);

        
        check_success_do(ref_jso,
            _ref_str_to_real_type(type_prefix, dal_json_object_get_str(ref_jso), ref_type_name, MAX_CLASS_NAME_LEN);
            json_object_object_add(type_def_jso, JSON_SCHEMA_REFERENCE, json_object_new_string(ref_type_name)));

        // require GCC 4.6
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif

        if (obj_ref)
            for (entry_key = obj_ref->head;
                (entry_key ? (val = (struct json_object *)entry_key->v, entry_next_key = entry_key->next, entry_key) :
                             0);
                entry_key = entry_next_key) {
                _reorganize_ref_link(type_prefix, val);
            }

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
    } else if (json_type_array == jso_type) {
        json_object *array_iter = NULL;

        json_object_array_foreach(type_def_jso, array_iter)
        {
            _reorganize_ref_link(type_prefix, array_iter);
        }
    } else {
        return;
    }

    return;
}


static gint32 _load_schema_file(json_object *global_jso, const gchar *file_name)
{
    int iRet;
    json_object *jso_schema = NULL;
    
    json_object *jso_definition = NULL;
    json_object *jso_properties = NULL;
    json_object *jso_property = NULL;
    gchar type_prefix[MAX_CLASS_NAME_LEN] = {0};
    gchar type_full_name[MAX_CLASS_NAME_LEN] = {0};
    gchar *str = NULL;
    errno_t str_ret;
    const gchar *file_name_tmp = NULL;
    

    
    json_object *namespace_type_jso = NULL; 

    str = g_strrstr(file_name, "/");

    file_name_tmp = (str) ? (str + 1) : file_name;

    
    str_ret = strncpy_s(type_prefix, MAX_CLASS_NAME_LEN, file_name_tmp, strlen(file_name_tmp));
    return_val_do_info_if_fail(str_ret == EOK, -1, debug_log(DLOG_ERROR, "invalid file name:%s", file_name));

    str = g_strrstr(type_prefix, SCHEMA_FILE_SUFFIX);
    return_val_do_info_if_fail(str, -1, debug_log(DLOG_ERROR, "invalid file type:%s", file_name));

    
    *str = '\0';

    
    check_fail_do_return_val(FALSE == json_object_has_key(global_jso, (const gchar *)type_prefix), 0,
        debug_log(DLOG_DEBUG, "%s has been loaded", file_name));

    

    jso_schema = json_object_from_file(file_name);
    if (!jso_schema) {
        debug_log(DLOG_ERROR, "Load json file: %s failed.", file_name);
        return -1;
    }

    if (FALSE == json_object_object_get_ex(jso_schema, JSON_SCHEMA_DEFINITION, &jso_definition)) {
        (void)json_object_put(jso_schema);

        return -1;
    }

    check_fail_do_return_val(json_type_object == json_object_get_type(jso_definition), -1,
        (void)json_object_put(jso_schema));

    
    namespace_type_jso = json_object_new_object();
    

    // require GCC 4.6
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
    
    json_object_object_foreach0(jso_definition, name, val)
    {
        // 过滤掉action (json-schema自带action定义无意义)
        if ((TRUE == json_object_object_get_ex(val, JSON_SCHEMA_PROPERTY, &jso_properties)) &&
            (TRUE == json_object_object_get_ex(jso_properties, JSON_SCHEMA_TARGET, &jso_property))) {
            continue;
        }

        
        
        iRet = snprintf_s(type_full_name, MAX_CLASS_NAME_LEN, MAX_CLASS_NAME_LEN - 1, "%s.%s", type_prefix, name);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        
        json_object_object_add(namespace_type_jso, (const gchar *)type_full_name, json_object_get(val));
        
    }
    
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif

    
    
    _reorganize_ref_link(type_prefix, jso_definition);

    
    json_object_object_add(global_jso, (const gchar *)type_prefix, namespace_type_jso);
    

    (void)json_object_put(jso_schema);

    return 0;
}


static json_bool _get_action(const gchar *action_name, json_object **jso_action)
{
    int iRet;
    char            action_name_wprefix[MAX_CLASS_NAME_LEN] = {0};
    const char *case_key = NULL;
    gint32 ret;
    json_object *global_jso = NULL;
    json_object *namespace_def_jso = NULL;

    global_jso = _get_global_class_jso(); 
    ret = _load_schema_file(global_jso, ACTION_SCHEMA_PATH);
    return_val_do_info_if_fail(0 == ret, FALSE,
        debug_log(DLOG_ERROR, "get action schema for %s failed, cann't load schema", action_name));
    
    iRet = snprintf_s(action_name_wprefix, MAX_CLASS_NAME_LEN, MAX_CLASS_NAME_LEN - 1, "%s.%s", ACTION_SCHEMA_PREFIX,
        action_name);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    
    
    (void)json_object_object_get_ex(global_jso, ACTION_SCHEMA_PREFIX, &namespace_def_jso);

    if (TRUE == json_object_object_case_get(namespace_def_jso, action_name_wprefix, &case_key, jso_action)) {
        return TRUE;
    }

    debug_log(DLOG_ERROR, "cann't get schema jso for %s", action_name);

    return FALSE;
}


static rf_retvalue _oncreate_property_check(json_object *jso_class, json_object *jso_body, json_object *jso_extend_arr)
{
    json_object *jso_needed_property = NULL;
    json_object *jso_item = NULL;
    json_object *jso_message = NULL;
    rf_retvalue retv = RF_OK;
    guint32 i;
    guint32 obj_need_len;

    
    if ((FALSE == json_object_object_get_ex(jso_class, JSON_SCHEMA_CREATE_REQUIRED, &jso_needed_property)) ||
        (json_object_get_type(jso_needed_property) != json_type_array)) {
        // 通过status code反馈此错误
        debug_log(DLOG_ERROR, "cann't get required_on_create info for %s, class=%s", dal_json_object_get_str(jso_body),
            dal_json_object_get_str(jso_class));
        return RF_REQUEST_NOT_ALLOWED;
    }

    
    obj_need_len = json_object_array_length(jso_needed_property);

    for (i = 0; i < obj_need_len; i++) {
        json_object *enum_item = json_object_array_get_idx(jso_needed_property, i);
        const gchar *enum_val = dal_json_object_get_str(enum_item);

        if (FALSE == json_object_object_get_ex(jso_body, enum_val, &jso_item) ||
            json_type_null == json_object_get_type(jso_item)) { 
            char related_property_buf[MAX_CHARACTER_NUM] = {0};
            int ret;

            retv = RF_PROP_MISSING;
            
            ret = snprintf_s(related_property_buf, MAX_CHARACTER_NUM, MAX_CHARACTER_NUM - 1, "%s", enum_val);
            
            check_fail_return_val(ret >= 0, RF_FAILED);

            if (VOS_OK == create_message_info(MSGID_CREATE_FAILED_REQ_PROP, (const gchar *)related_property_buf,
                &jso_message, (const gchar *)related_property_buf)) {
                (void)json_object_array_add(jso_extend_arr, jso_message);
            }

            
            return retv;
        }
    }

    return retv;
}


static rf_retvalue _action_parameter_check(const gchar *jso_action_str, json_object *jso_action_parameter,
    json_object *jso_body, json_object *jso_extend_arr)
{
    json_object *jso_item = NULL;
    json_object *jso_message = NULL;
    rf_retvalue retv = RF_OK;
    
    json_object *optional_flag_jso = NULL;
    json_bool optional_flag;
    

    check_fail_return_val(json_type_object == json_object_get_type(jso_action_parameter), RF_OK);

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
    json_object_object_foreach0(jso_action_parameter, name, val)
    {
        (void)json_object_object_get_ex(val, JSON_SCHMEA_OPTIONAL_FLAG, &optional_flag_jso);

        
        optional_flag = json_object_get_boolean(optional_flag_jso);
        if ((FALSE == json_object_object_get_ex(jso_body, name, &jso_item)) && (FALSE == optional_flag)) {
            if (VOS_OK == create_message_info(MSGID_ACT_PARA_MISSING, NULL, &jso_message, jso_action_str, name)) {
                (void)json_object_array_add(jso_extend_arr, jso_message);
            }

            retv = RF_PROP_MISSING;
        }
    }
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif

    return retv;
}


static rf_retvalue _format_check_string(json_object *jso_property, json_object *jso_value)
{
    json_object *pattern = NULL;
    json_object *format = NULL;
    json_object *jso_enum = NULL;
    const char *pattern_str = NULL;
    const char *format_str = NULL;
    const char *value_str = NULL;

    if (json_type_string != json_object_get_type(jso_value)) {
        return RF_PROP_TYPE_ERR;
    }

    value_str = dal_json_object_get_str(jso_value);
    if (TRUE == json_object_object_get_ex(jso_property, JSON_SCHEMA_PATTERN, &pattern)) {
        pattern_str = dal_json_object_get_str(pattern);
        return (rf_retvalue)(
            (TRUE == g_regex_match_simple(pattern_str, value_str, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0)) ?
            RF_OK :
            RF_PROP_FORMAT_ERR);
    } else if (TRUE == json_object_object_get_ex(jso_property, JSON_SCHEMA_FORMAT, &format)) {
        format_str = dal_json_object_get_str(format);
        if (0 == strcmp(format_str, JSON_SCHEMA_FORMAT_URI)) {
            return (rf_retvalue)(
                (TRUE == g_regex_match_simple(REGEX_PATTERN_URL, value_str, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0)) ?
                RF_OK :
                RF_PROP_FORMAT_ERR);
        } else if (0 == strcmp(format_str, JSON_SCHEMA_FORMAT_DATETIME)) {
            return (rf_retvalue)((TRUE ==
                g_regex_match_simple(REGEX_PATTERN_DATETIME, value_str, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0)) ?
                RF_OK :
                RF_PROP_FORMAT_ERR);
        }
    } else if (TRUE == json_object_object_get_ex(jso_property, JSON_SCHEMA_ENUM, &jso_enum)) {
        int i;
        if (json_type_array != json_object_get_type(jso_enum)) {
            return (rf_retvalue)RF_PROP_FORMAT_ERR;
        }

        for (i = 0; i < json_object_array_length(jso_enum); i++) {
            json_object *enum_item = json_object_array_get_idx(jso_enum, i);

            if (0 == strcmp(dal_json_object_get_str(enum_item), value_str)) {
                return (rf_retvalue)RF_OK;
            }
        }

        return (rf_retvalue)RF_PROP_NOT_IN_LIST;
    }

    return (rf_retvalue)RF_OK;
}


static rf_retvalue _format_check_null(const json_object *jso_value)
{
    return (rf_retvalue)((NULL == jso_value) ? RF_OK : RF_PROP_TYPE_ERR);
}


static rf_retvalue _format_check_number(const gchar *type_str, json_object *jso_property, json_object *jso_value)
{
    json_object *jso_minimum = NULL;
    json_object *jso_maximum = NULL;
    double value;
    double maximum, minimum;

    if (0 == g_strcmp0(type_str, "number")) {
        return_val_if_expr((json_type_double != json_object_get_type(jso_value)) &&
            (json_type_int != json_object_get_type(jso_value)),
            RF_PROP_TYPE_ERR);
    } else if (0 == g_strcmp0(type_str, "integer")) {
        return_val_if_expr(json_type_int != json_object_get_type(jso_value), RF_PROP_TYPE_ERR);
    } else {
        return RF_FAILED;
    }

    value = json_object_get_double(jso_value);

    
    if (TRUE == json_object_object_get_ex(jso_property, JSON_SCHEMA_MINIMUM, &jso_minimum)) {
        minimum = json_object_get_double(jso_minimum);
        if (value < minimum) {
            return RF_PROP_NOT_IN_LIST;
        }
    }

    if (TRUE == json_object_object_get_ex(jso_property, JSON_SCHEMA_MAXIMUM, &jso_maximum)) {
        maximum = json_object_get_double(jso_maximum);
        if (value > maximum) {
            return RF_PROP_NOT_IN_LIST;
        }
    }

    
    return RF_OK;
}


static rf_retvalue _format_check_boolean(json_object *jso_value)
{
    return (rf_retvalue)((json_type_boolean == json_object_get_type(jso_value)) ? RF_OK : RF_PROP_TYPE_ERR);
}


static rf_retvalue _format_check_object(json_object *jso_schema, json_object *jso_property_schema,
    json_object *jso_value)
{
    const char *value_str = NULL;
    json_object *jso_properties = NULL;
    json_object *jso_property = NULL;
    json_object *jso_property_schema2 = NULL;
    json_bool bool_ret;
    rf_retvalue retv;

    if (FALSE == json_object_object_get_ex(jso_property_schema, JSON_SCHEMA_PROPERTY, &jso_properties)) {
        return RF_PROP_FORMAT_ERR;
    }

    // 有@odata.id，则value表现形式为uri，否则就是一个完整的json object
    if (TRUE == json_object_object_get_ex(jso_properties, JSON_SCHEMA_ODATAID, &jso_property)) {
        value_str = dal_json_object_get_str(jso_value);
        return (TRUE == g_regex_match_simple(REGEX_PATTERN_URL, value_str, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0)) ?
            RF_OK :
            RF_PROP_FORMAT_ERR;
    } else {
        // require GCC 4.6
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
        
        json_object_object_foreach0(jso_value, name, val)
        {
            
            bool_ret = json_object_object_get_ex(jso_properties, name, &jso_property_schema2);
            check_fail_continue(TRUE == bool_ret);

            retv = _format_check(jso_schema, jso_property_schema2, val);
            check_fail_return_val(RF_OK == retv, retv);
        }
        
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif

        return RF_OK;
    }
}


static rf_retvalue _format_check_type(const char *typestr, json_object *jso_schema, json_object *jso_property,
    json_object *jso_value)
{
    if (strcmp(typestr, "string") == 0) {
        return _format_check_string(jso_property, jso_value);
    } else if (strcmp(typestr, "null") == 0) {
        return _format_check_null(jso_value);
    } else if (strcmp(typestr, "number") == 0) {
        return _format_check_number("number", jso_property, jso_value);
    } else if (strcmp(typestr, "boolean") == 0) {
        return _format_check_boolean(jso_value);
    } else if (strcmp(typestr, "object") == 0) {
        return _format_check_object(jso_schema, jso_property, jso_value);
    }

    return RF_PROP_TYPE_ERR;
}


static rf_retvalue _format_check_multitype(json_object *jso_type, json_object *jso_schema, json_object *jso_property,
    json_object *jso_value)
{
    int i;
    rf_retvalue retv;
    
    json_object *jso = NULL;
    
    const char *typestr = NULL;

    for (i = 0; i < json_object_array_length(jso_type); i++) {
        jso = json_object_array_get_idx(jso_type, i);
        typestr = dal_json_object_get_str(jso);

        retv = _format_check_type(typestr, jso_schema, jso_property, jso_value);

        // 只要有一个检查通过就认为通过
        check_success_return_val(RF_OK == retv, retv);
    }

    return RF_PROP_TYPE_ERR;
}

static rf_retvalue _prop_value_format_check_singletype(const char *type_str, json_object *prop_schema_jso,
    json_object *prop_val_jso)
{
    
    if (g_strcmp0(type_str, "string") == 0) {
        return _format_check_string(prop_schema_jso, prop_val_jso);
    } else if (g_strcmp0(type_str, "null") == 0) {
        return _format_check_null(prop_val_jso);
    } else if (g_strcmp0(type_str, "number") == 0) {
        return _format_check_number("number", prop_schema_jso, prop_val_jso);
    } else if (g_strcmp0(type_str, "boolean") == 0) {
        return _format_check_boolean(prop_val_jso);
    } else if (g_strcmp0(type_str, "integer") == 0) {
        return _format_check_number("integer", prop_schema_jso, prop_val_jso);
    } else {
        debug_log(DLOG_ERROR, "Invalid type when checking prop val, type is %s\n", type_str);

        return RF_PROP_TYPE_ERR;
    }
}

static rf_retvalue _prop_value_format_check_multitype(json_object *type_jso, json_object *prop_schema_jso,
    json_object *prop_val_jso)
{
    json_object *type_kind_jso = NULL;
    rf_retvalue retv;

    json_object_array_foreach(type_jso, type_kind_jso)
    {
        const char *type_str = dal_json_object_get_str(type_kind_jso);
        retv = _prop_value_format_check_singletype(type_str, prop_schema_jso, prop_val_jso);
        check_success_return_val(RF_OK == retv || RF_PROP_TYPE_ERR != retv, retv);
    }

    return RF_PROP_TYPE_ERR;
}


static rf_retvalue _format_check_array(json_object *jso_schema, json_object *jso_property, json_object *jso_value)
{
    int i;
    rf_retvalue retv;
    json_object *jso = NULL;
    json_object *jso_items = NULL;

    if (json_type_array != json_object_get_type(jso_value)) {
        return RF_PROP_FORMAT_ERR;
    }

    if (FALSE == json_object_object_get_ex(jso_property, JSON_SCHEMA_ITEMS, &jso_items)) {
        return RF_PROP_FORMAT_ERR;
    }

    for (i = 0; i < json_object_array_length(jso_value); i++) {
        jso = json_object_array_get_idx(jso_value, i);

        retv = _format_check(jso_schema, jso_items, jso);

        // 只要有一个检查不通过就认为不通过
        check_fail_return_val(RF_OK == retv, retv);
    }

    return RF_OK;
}


static rf_retvalue _format_check(json_object *jso_schema, json_object *jso_property_schema, json_object *jso_value)
{
    json_object *jso_ref = NULL;
    json_object *type = NULL;
    json_type t;
    const char *typestr = NULL;
    rf_retvalue retv;

    if (TRUE == json_object_object_get_ex(jso_property_schema, JSON_SCHEMA_REFERENCE, &jso_ref)) {
        if (FALSE == json_object_object_get_ex(jso_schema, dal_json_object_get_str(jso_ref), &jso_property_schema)) {
            return RF_PROP_UNKNOWN;
        }
    }

    if (FALSE == json_object_object_get_ex(jso_property_schema, JSON_SCHEMA_TYPE, &type)) {
        return RF_PROP_TYPE_ERR;
    }

    t = json_object_get_type(type);
    // type 为 json_type_array 和 "type" 为 "array"不是一回事
    // type 为 array --->   "type" : [ "string", "null" ]
    if (json_type_array == t) {
        retv = _format_check_multitype(type, jso_schema, jso_property_schema, jso_value);
    } else {
        typestr = dal_json_object_get_str(type);
        if (strcmp(typestr, "array") == 0) {
            retv = _format_check_array(jso_schema, jso_property_schema, jso_value);
        } else {
            retv = _format_check_type(typestr, jso_schema, jso_property_schema, jso_value);
        }
    }

    return retv;
}

static rf_retvalue _prop_value_format_check(json_object *prop_val_jso, json_object *prop_schema_jso)
{
    json_object *type_jso = NULL;
    json_bool b_ret;
    json_type t;
    rf_retvalue retv;

    

    b_ret = json_object_object_get_ex(prop_schema_jso, JSON_SCHEMA_TYPE, &type_jso);
    check_fail_return_val(b_ret, RF_PROP_TYPE_ERR);

    t = json_object_get_type(type_jso);
    if (json_type_array == t) {
        retv = _prop_value_format_check_multitype(type_jso, prop_schema_jso, prop_val_jso);
    } else {
        retv = _prop_value_format_check_singletype(dal_json_object_get_str(type_jso), prop_schema_jso, prop_val_jso);
    }

    return retv;
}


static json_object *_parse_json_str(const gchar *json_str, json_object *jso_extend_arr, gboolean post_check,
    const char *action_str)
{
    json_object *jso;
    json_object *jso_message = NULL;

    jso = json_tokener_parse(json_str);
    
    
    if ((json_object_get_type(jso) != json_type_object) ||
        (!g_strcmp0(JSON_NULL_OBJECT_STR, dal_json_object_get_str(jso)) && NULL == action_str)) {
        if (VOS_OK == create_message_info(MSGID_MALFORMED_JSON, NULL, &jso_message)) {
            (void)json_object_array_add(jso_extend_arr, jso_message);
        }

        (void)json_object_put(jso);
        // 如果不是一个json对象也置空
        jso = NULL;
        
        return jso;
        
    }

    JsonError *json_error = json_error_new();
    if (json_error == NULL) {
        debug_log(DLOG_ERROR, "json new json_error failed");
        (void)json_object_put(jso);
        return NULL;
    }
    gint ret = _duplicate_check(json_str, action_str, jso_extend_arr, json_error);
    
    if (ret == JSON_ERR_KEY_REPEAT && !post_check) {
        for (gint i = 0; i < json_error->repeat_key_set->count; i++) {
            char property_path[MAX_JSON_PATH] = {0};
            if (snprintf_s(property_path, sizeof(property_path), sizeof(property_path) - 1, "%s/%s", json_error->path,
                json_error->repeat_key_set->keys[i]) == -1) {
                debug_log(DLOG_ERROR, "snprintf_s failed");
                continue;
            }
            json_object_object_del_ex(jso, property_path);
        }
        json_error_free(json_error);
        return jso;
    } else if (ret != JSON_OK) {
        (void)json_object_put(jso);
        json_error_free(json_error);
        return NULL;
    }

    json_error_free(json_error);
    return jso;
}


LOCAL gint32 general_duplicate_err_message(const gchar *jso_action_str, JsonError *error, json_object *jso_extend_arr)
{
    json_object *jso_message = NULL;

    for (gint i = 0; i < error->repeat_key_set->count; i++) {
        
        if (jso_action_str == NULL) {
            if ((create_message_info(MSGID_PROP_DUPLICATE, (const gchar *)error->repeat_key_set->keys[i], &jso_message, 
                (const gchar *)error->repeat_key_set->keys[i]) == RET_OK) && (jso_message != NULL)) {
                
                (void)json_object_array_add(jso_extend_arr, jso_message);
            }
        } else {
            if ((create_message_info(MSGID_ACT_PARA_DUPLICATE, NULL, &jso_message, jso_action_str,
                error->repeat_key_set->keys[i]) == RET_OK) && (jso_message != NULL)) {
                (void)json_object_array_add(jso_extend_arr, jso_message);
            }
        }
    }

    return JSON_OK;
}


LOCAL gint _duplicate_check(const gchar *jso_body_str, const gchar *jso_action_str, json_object *jso_extend_arr,
    JsonError *error)
{
    json_object *jso_message = NULL;
    
    const gchar *msg_id = NULL;

    
    gint ret = json_check(jso_body_str, error);
    if (ret == JSON_ERR_INTEGER_OVERFLOW) {
        
        msg_id = (error->layer_type == JSON_LAYER_OBJECT) ? MSGID_PROP_FORMAT_ERR : MSGID_PROP_ITEM_FORMAT_ERR;

        gchar int_val_str[VAL_INTEGER_LEN] = {0};
        
        gint str_ret =
            snprintf_s(int_val_str, sizeof(int_val_str), sizeof(int_val_str) - 1, "%s...", error->overflow_interger);
        if (str_ret <= 0) {
            debug_log(DLOG_ERROR, "%s failed:connect string error, ret:%d", __FUNCTION__, str_ret);
            return JSON_ERR;
        }
        // 去掉开头的#/
        const char *related_property_path = error->path + strlen("#/");
        (void)create_message_info(msg_id, related_property_path, &jso_message, (const gchar*)int_val_str, 
            related_property_path);
        if (json_object_array_add(jso_extend_arr, jso_message) != 0) {
            (void)json_object_put(jso_message);
        }
    } else if (ret == JSON_ERR_KEY_REPEAT) {
        if (general_duplicate_err_message(jso_action_str, error, jso_extend_arr) != JSON_OK) {
            debug_log(DLOG_ERROR, "%s general_duplicate_err_message fail", __FUNCTION__);
            return ret;
        }
    } else if (ret != JSON_OK) {
        (void)create_message_info(MSGID_UNRECOGNIZED_REQ_BODY, NULL, &jso_message);
        (void)json_object_array_add(jso_extend_arr, jso_message);
    }
    return ret;
}


static json_bool _missing_check(json_object *schema_jso, json_object *jso_body, const gchar *jso_action_str,
    json_object *jso_extend_arr)
{
    rf_retvalue retv;
    json_object *jso_action = NULL;
    json_object *jso_message = NULL;

    debug_log(DLOG_INFO, "schema_jso=%s", dal_json_object_get_str(schema_jso));
    if (NULL == jso_action_str) {
        
        retv = _oncreate_property_check(schema_jso, jso_body, jso_extend_arr);
        if (retv != RF_OK) {
            debug_log(DLOG_ERROR, "_oncreate_property_check failed. jso_str:%s",
                dal_json_object_get_str(jso_extend_arr));
            return FALSE;
        }
    } else {
        

        if (FALSE == _get_action(jso_action_str, &jso_action)) {
            if (VOS_OK == create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, &jso_message, jso_action_str)) {
                (void)json_object_array_add(jso_extend_arr, jso_message);
            }

            debug_log(DLOG_ERROR, "%s : jso_msg:%s", __FUNCTION__, dal_json_object_get_str(jso_message));
            return FALSE;
        }

        
        retv = _action_parameter_check(jso_action_str, jso_action, jso_body, jso_extend_arr);
        if (RF_OK != retv) {
            debug_log(DLOG_ERROR, "_action_parameter_check for %s failed. jso_str:%s", jso_action_str,
                dal_json_object_get_str(jso_extend_arr));
            return FALSE;
        }
    }

    return TRUE;
}

static rf_retvalue _check_readonly_prop(json_object *prop_schema_jso)
{
    json_object *readonly_prop_jso = NULL;

    if (json_object_object_get_ex(prop_schema_jso, JSON_SCHEMA_READONLY, &readonly_prop_jso) &&
        json_object_get_boolean(readonly_prop_jso)) {
        return RF_PROP_READONLY;
    } else { 
        return RF_OK;
    }
}


json_object *get_class_schema(const char *class_name)
{
    errno_t safe_fun_ret;
    json_object *object_schema_jso = NULL;
    json_object *global_jso = _get_global_class_jso();
    const char *case_key = NULL;
    char             schema_file_name[MAX_FILE_NAME] = {0};
    char             class_namespace[MAX_FILE_NAME] = {0};
    gchar *pos = NULL;
    gint32 ret;
    gchar *lower_class_name = NULL;
    json_object *namespace_def_jso = NULL;

    lower_class_name = g_ascii_strdown(class_name, -1);
    check_fail_do_return_val(NULL != lower_class_name, NULL, debug_log(DLOG_ERROR, "cann't get class name."));

    
    safe_fun_ret = strncpy_s(class_namespace, sizeof(class_namespace), lower_class_name, sizeof(class_namespace) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    pos = g_strrstr(class_namespace, ".");
    check_fail_do_return_val(pos, NULL, debug_log(DLOG_ERROR, "cann't get point index from %s", class_namespace);
        g_free(lower_class_name));
    *pos = '\0';

    
    safe_fun_ret =
        strncpy_s(schema_file_name, sizeof(schema_file_name), JSON_SCHEMA_FILE_PATH, strlen(JSON_SCHEMA_FILE_PATH));
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    safe_fun_ret = strncat_s(schema_file_name, MAX_FILE_NAME, class_namespace, strlen(class_namespace));
    do_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    g_free(lower_class_name);

    
    safe_fun_ret = strncat_s(schema_file_name, MAX_FILE_NAME, SCHEMA_FILE_SUFFIX, strlen(SCHEMA_FILE_SUFFIX));
    do_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    ret = _load_schema_file(global_jso, schema_file_name);
    check_fail_do_return_val(0 == ret, NULL, debug_log(DLOG_ERROR, "load schema from %s failed", schema_file_name));

    
    (void)json_object_object_get_ex(global_jso, (const gchar *)class_namespace, &namespace_def_jso);
    check_fail_do_return_val(namespace_def_jso, NULL,
        debug_log(DLOG_ERROR, "cann't get namespace definition for %s", class_namespace));

    (void)json_object_object_case_get(namespace_def_jso, class_name, &case_key, &object_schema_jso);
    debug_log(DLOG_INFO, "schema_file=%s, class_name=%s, namespace=%s", schema_file_name, class_name, class_namespace);

    return object_schema_jso;
}


LOCAL void _put_patter_properties_user_data(struct json_object *jso, void *user_data)
{
    return_if_fail((NULL != jso) && (NULL != user_data));

    (void)json_object_put((json_object *)user_data);

    return;
}


LOCAL void _add_pattern_properties_to_user_data(json_object *object_schema, json_object *property_jso)
{
    json_object *pattern_properties_jso = NULL;
    json_object *additional_property_flag_jso = NULL;
    json_bool additional_property_flag;

    (void)json_object_object_get_ex(object_schema, JSON_SCHEMA_PATTERN_PROPERTIES, &pattern_properties_jso);
    (void)json_object_object_get_ex(object_schema, JSON_SCHEMA_ADDITIONAL_PROPERTIES, &additional_property_flag_jso);

    additional_property_flag = json_object_get_boolean(additional_property_flag_jso);
    if ((TRUE == additional_property_flag) && (NULL != pattern_properties_jso)) {
        json_object_set_user_data(property_jso, (void *)json_object_get(pattern_properties_jso),
            (json_object_delete_fn *)_put_patter_properties_user_data);
    }

    return;
}


static json_object *_get_object_schema(json_object *jso)
{
    json_object *obj_schema_jso = NULL;

    if (json_object_has_key(jso, JSON_SCHEMA_REFERENCE)) {
        
        json_object *ref_jso = NULL;
        const char *ref_type_str = NULL;

        (void)json_object_object_get_ex(jso, JSON_SCHEMA_REFERENCE, &ref_jso);
        ref_type_str = dal_json_object_get_str(ref_jso);

        return _get_object_schema(get_class_schema(ref_type_str));
    } else if (json_object_has_key(jso, JSON_SCHEMA_PROPERTY)) {
        
        (void)json_object_object_get_ex(jso, JSON_SCHEMA_PROPERTY, &obj_schema_jso);

        
        _add_pattern_properties_to_user_data(jso, obj_schema_jso);
        

        return obj_schema_jso;
    } else if (json_object_has_key(jso, JSON_SCHEMA_ANYOF)) {
        
        json_object *anyof_jso = NULL;
        json_object *array_item_jso = NULL;
        json_object *ref_jso = NULL;
        const char *ref_str = NULL;
        size_t array_len;

        (void)json_object_object_get_ex(jso, JSON_SCHEMA_ANYOF, &anyof_jso);
        //  如果anyof数组长度大于 2，取最后一个，因为最后一个是最新的schema
        array_len = json_object_array_length(anyof_jso);
        if (array_len > 2) {
            array_item_jso = json_object_array_get_idx(anyof_jso, array_len - 1);
        } else {
            array_item_jso = json_object_array_get_idx(anyof_jso, 0);
        }
        (void)json_object_object_get_ex(array_item_jso, JSON_SCHEMA_REFERENCE, &ref_jso);
        ref_str = dal_json_object_get_str(ref_jso);

        return _get_object_schema(get_class_schema(ref_str));
    } else {
        return jso;
    }
}

static json_object *_get_items_object_schema(json_object *jso)
{
    json_object *items_jso = NULL;
    json_bool b_ret;

    b_ret = json_object_object_get_ex(jso, JSON_SCHEMA_ITEMS, &items_jso);
    check_fail_do_return_val(b_ret, NULL,
        debug_log(DLOG_ERROR, "schema %s doesn't has a key %s.", dal_json_object_get_str(jso), JSON_SCHEMA_ITEMS));

    return _get_object_schema(items_jso);
}

static void _push_property_path(GSList **property_list, char *property_path)
{
    *property_list = g_slist_append(*property_list, (gpointer)property_path);
}

static void _pop_property_path(GSList **property_list)
{
    GSList *property_path_tail = g_slist_last(*property_list);

    *property_list = g_slist_delete_link(*property_list, property_path_tail);
}


static void merge_json_pointer_path(gpointer data, gpointer userdata)
{
    errno_t nRet;
    gchar *json_pointer_path = (gchar *)userdata;
    const gchar *cur_path_segment = (const gchar *)data;

    return_if_fail(data);
    return_if_fail(userdata);

    
    if ((strlen(json_pointer_path) + strlen(cur_path_segment)) >= (MAX_JSON_POINTER_PATH_LEN - 1)) {
        nRet = strcpy_s(json_pointer_path, MAX_JSON_POINTER_PATH_LEN, JSON_POINTER_BUF_OVERLONG);
        return_do_info_if_fail((nRet == EOK),
            debug_log(DLOG_DEBUG, "merge_json_pointer_path strcpy_s failed %d.", nRet));
    }

    
    if (json_pointer_path[0] != '\0') {
        nRet = strcat_s(json_pointer_path, MAX_JSON_POINTER_PATH_LEN, "/");
        return_do_info_if_fail((EOK == nRet),
            debug_log(DLOG_DEBUG, "merge_json_pointer_path stcat_s failed %d.", nRet));
    }

    nRet = strcat_s(json_pointer_path, MAX_JSON_POINTER_PATH_LEN, cur_path_segment);
    return_do_info_if_fail((EOK == nRet),
        debug_log(DLOG_DEBUG, "merge_json_pointer_path stcat_s segment failed %d.", nRet));
    
}

static rf_retvalue _get_related_property_path(GSList *property_path_list, const char *property_name,
    char json_pointer_path[MAX_JSON_POINTER_PATH_LEN], gint32 json_pointer_path_size)
{
    errno_t safe_fun_ret;
    

    g_slist_foreach(property_path_list, merge_json_pointer_path, json_pointer_path);

    // 对象分支子属性为空属非法对象，不会传到这接口；数组分支子成员无property_name，校验返回RF_OK
    return_val_if_fail(NULL != property_name, RF_OK);

    if ((strlen(json_pointer_path) + strlen(property_name)) >= json_pointer_path_size - 1) {
        return RF_MALFORMED_JSON;
    }
    
    if ('\0' != json_pointer_path[0]) {
        safe_fun_ret = strncat_s(json_pointer_path, json_pointer_path_size, "/", strlen("/"));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }
    safe_fun_ret = strncat_s(json_pointer_path, json_pointer_path_size, property_name, strlen(property_name));
    do_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    
    
    return RF_OK;
}


static gboolean is_sensitive_prop_name(const gchar *prop_name)
{
    gchar *lower = NULL;
    if (prop_name == NULL) {
        return FALSE;
    }

    for (gint32 i = 0; i < G_N_ELEMENTS(g_sensitive_word_list); i++) {
        if (g_sensitive_word_list[i] == NULL) {
            return FALSE;
        }

        // 忽略大小写查找敏感词
        lower = g_ascii_strdown(prop_name, strlen(prop_name));
        if (lower != NULL && strstr(lower, g_sensitive_word_list[i]) != NULL) {
            g_free(lower);
            return TRUE;
        }

        g_free(lower);
        lower = NULL;
    }

    return FALSE;
}


static json_bool _construct_action_message(rf_retvalue err_type, const gchar *jso_action_str,
    GSList *property_path_list, const gchar *json_key, const gchar *val, json_object *jso_extend_arr)
{
    json_object *jso_message;
    jso_message = NULL;
    
    char                action_param_path[MAX_JSON_POINTER_PATH_LEN] = {0};
    rf_retvalue ret;
    

    
    check_fail_do(val, val = RF_NULL_STR);
    

    
    ret = _get_related_property_path(property_path_list, json_key, action_param_path, MAX_JSON_POINTER_PATH_LEN);
    do_val_if_fail(RF_OK == ret, (err_type = ret));
    

    if (is_sensitive_prop_name(action_param_path) == TRUE) {
        val = RF_SENSITIVE_INFO;
    }
    switch (err_type) {
        case RF_PROP_TYPE_ERR:
            (void)create_message_info(MSGID_ACT_PARA_TYPE_ERR, NULL, &jso_message, val,
                (const gchar *)action_param_path, jso_action_str);
            break;

        case RF_PROP_FORMAT_ERR:
        case RF_PROP_NOT_IN_LIST:
            (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, &jso_message, val,
                (const gchar *)action_param_path, jso_action_str);
            break;

        
        case RF_PROP_UNKNOWN:
            (void)create_message_info(MSGID_ACT_PARA_UNKNOWN, NULL, &jso_message, jso_action_str,
                (const gchar *)action_param_path);
            break;
        
        
        case RF_PROP_NAME_INVALID_FORMAT:
            (void)create_message_info(MSGID_PROP_NAME_FORMAT_ERROR, NULL, &jso_message,
                (const gchar *)action_param_path);
            break;
            

        default:
            break;
    }

    if (jso_message) {
        (void)json_object_array_add(jso_extend_arr, jso_message);
    }

    return TRUE;
}


static json_bool _construct_property_message(rf_retvalue err_type, GSList *property_path_list, const gchar *json_key,
    const gchar *val, json_object *jso_extend_arr)
{
    json_object *jso_message = NULL;
    char related_property_path[MAX_JSON_POINTER_PATH_LEN] = {0};
    rf_retvalue ret;

    jso_message = NULL;

    
    check_fail_do(val, val = RF_NULL_STR);
    

    ret = _get_related_property_path(property_path_list, json_key, related_property_path, MAX_JSON_POINTER_PATH_LEN);
    check_fail_do(RF_OK == ret, err_type = ret);

    val = (is_sensitive_prop_name(related_property_path) == TRUE) ? RF_SENSITIVE_INFO : val;
    switch (err_type) {
        case RF_PROP_TYPE_ERR:

            
            (void)create_message_info(MSGID_PROP_TYPE_ERR, (const gchar *)related_property_path, &jso_message, val,
                (const gchar *)related_property_path);
            break;

        case RF_PROP_FORMAT_ERR:
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, (const gchar *)related_property_path, &jso_message, val,
                (const gchar *)related_property_path);
            break;

        case RF_PROP_NOT_IN_LIST:
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, (const gchar *)related_property_path, &jso_message, val,
                (const gchar *)related_property_path);
            break;

        case RF_PROP_READONLY:
            (void)create_message_info(MSGID_PROP_NOT_WRITABLE, (const gchar *)related_property_path, &jso_message,
                (const gchar *)related_property_path);
            break;

        case RF_PROP_UNKNOWN:
            (void)create_message_info(MSGID_PROP_UNKNOWN, (const gchar *)related_property_path, &jso_message,
                (const gchar *)related_property_path);
            break;

            
        case RF_MALFORMED_JSON:
            (void)create_message_info(MSGID_MALFORMED_JSON, NULL, &jso_message);
            break;

        
        case RF_PROP_NAME_INVALID_FORMAT:
            (void)create_message_info(MSGID_PROP_NAME_FORMAT_ERROR, (const gchar *)related_property_path, &jso_message,
                (const gchar *)related_property_path);
            break;

        
        case RF_ARRAY_REQUIRED_ITEM:
            (void)create_message_info(MSGID_ARRAY_REQUIRED_ITEM, (const gchar *)related_property_path, &jso_message,
                (const gchar *)related_property_path);
            break;
        default: 
            break;
    }

    if (jso_message) {
        (void)json_object_array_add(jso_extend_arr, jso_message);
    }

    return TRUE;
}


static json_bool _construct_property_item_message(rf_retvalue item_err_type, GSList *property_path_list,
    const gchar *json_key, const gchar *val, json_object *jso_extend_arr)
{
    json_object *jso_message = NULL;
    char related_property_path[MAX_JSON_POINTER_PATH_LEN] = {0};
    rf_retvalue ret;

    jso_message = NULL;

    
    check_fail_do(val, val = RF_NULL_STR);
    

    
    ret = _get_related_property_path(property_path_list, NULL, related_property_path, MAX_JSON_POINTER_PATH_LEN);
    check_fail_do(RF_OK == ret, item_err_type = ret);
    

    if (is_sensitive_prop_name(related_property_path) == TRUE) {
        val = RF_SENSITIVE_INFO;
    }
    switch (item_err_type) {
        case RF_PROP_TYPE_ERR:
            
            (void)create_message_info(MSGID_PROP_ITEM_TYPE_ERR, (const gchar *)related_property_path, &jso_message,
                val, (const gchar *)related_property_path);
            break;

        case RF_PROP_FORMAT_ERR:
            (void)create_message_info(MSGID_PROP_ITEM_FORMAT_ERR, (const gchar *)related_property_path,
                &jso_message, val, (const gchar *)related_property_path);
            break;

        case RF_PROP_NOT_IN_LIST:
            (void)create_message_info(MSGID_PROP_ITEM_NOT_IN_LIST, (const gchar *)related_property_path,
                &jso_message, val, (const gchar *)related_property_path);
            break;

            
        case RF_MALFORMED_JSON:
            (void)create_message_info(MSGID_MALFORMED_JSON, NULL, &jso_message);
            break;

        default: 
            break;
    }

    if (jso_message) {
        (void)json_object_array_add(jso_extend_arr, jso_message);
    }

    return TRUE;
}


LOCAL rf_retvalue _get_pattern_property_schema(json_object *object_jso, const gchar *prop_name,
    json_object **schema_jso)
{
    json_object *pattern_properties_schema_jso = NULL;
    gboolean match_result = FALSE;

    (void)json_object_get_user_data(object_jso, (void **)&pattern_properties_schema_jso);

    
    return_val_do_info_if_fail((NULL != pattern_properties_schema_jso), RF_PROP_UNKNOWN,
        debug_log(DLOG_DEBUG, "cann't find pattern jso for %s", prop_name));

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
    json_object_object_foreach0(pattern_properties_schema_jso, prop_pattern, val)
    {
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif

        match_result = g_regex_match_simple(prop_pattern, prop_name, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0);
        if (TRUE == match_result) {
            *schema_jso = val;
            break;
        }

        debug_log(DLOG_DEBUG, "pattern %s , property %s:match failed", prop_pattern, prop_name);
    }

    

    return (rf_retvalue)((TRUE == match_result) ? RF_OK : RF_PROP_NAME_INVALID_FORMAT);
}


static rf_retvalue _get_and_check_prop(json_bool patch_check, const char *action_name, json_object *prop_schema_jso,
    json_object *prop_val_jso, json_object *jso_extend_arr, GSList *property_path_list, const gchar *prop_name)
{
    json_type type = json_object_get_type(prop_val_jso);
    rf_retvalue retv = RF_FAILED;
    json_object *object_schema = NULL;
    json_bool b_ret = 0;
    int i = 0;
    json_object *jso_message = NULL;
    void *user_data = NULL;

    // require GCC 4.6
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif

    object_schema = _get_object_schema(prop_schema_jso);

    
    
    if (patch_check) {
        retv = _check_readonly_prop(prop_schema_jso);
        check_success_return_val(RF_PROP_READONLY == retv, retv);
        retv = _check_readonly_prop(object_schema);
        check_success_return_val(RF_PROP_READONLY == retv, retv);
    }

    

    
    if (json_type_array != type && json_type_object != type) {
        return _prop_value_format_check(prop_val_jso, object_schema);
    }

    
    if (json_type_object == type) {
        
        
        
        check_fail_return_val(!json_object_has_key(object_schema, JSON_SCHEMA_TYPE), RF_PROP_TYPE_ERR);
        
        
        (void)json_object_get_user_data(prop_val_jso, &user_data);

        
        if (action_name == NULL) {
            check_success_do_return_val(!user_data &&
                !g_strcmp0(JSON_NULL_OBJECT_STR, dal_json_object_get_str(prop_val_jso)),
                RF_FAILED, json_object_array_clean(jso_extend_arr);
                (void)create_message_info(MSGID_MALFORMED_JSON, NULL, &jso_message);
                (void)json_object_array_add(jso_extend_arr, jso_message));
        }

        
        
        

        check_fail_do_return_val(object_schema, RF_FAILED,
            debug_log(DLOG_ERROR, "cann't get schema for %s.", dal_json_object_get_str(prop_val_jso));
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &jso_message);
            (void)json_object_array_add(jso_extend_arr, jso_message));
        

        json_object_object_foreach0(prop_val_jso, name, val)
        {
            json_object *iter_prop_schema_jso = NULL;

            b_ret = json_object_object_get_ex(object_schema, name, &iter_prop_schema_jso);

            
            
            do_val_if_fail(TRUE == b_ret,
                (retv = _get_pattern_property_schema(object_schema, name, &iter_prop_schema_jso)));
            
            if (b_ret || (RF_OK == retv)) {
                
                _push_property_path(&property_path_list, name);
                retv = _get_and_check_prop(patch_check, action_name, iter_prop_schema_jso, val, jso_extend_arr,
                    property_path_list, name);
                
                _pop_property_path(&property_path_list);
                
                check_success_return_val(RF_FAILED == retv, retv);
                
            }
            

            if (action_name) {
                _construct_action_message(retv, action_name, property_path_list, name, dal_json_object_get_str(val),
                    jso_extend_arr);
            } else {
                _construct_property_message(retv, property_path_list, name, dal_json_object_get_str(val),
                    jso_extend_arr);
            }

            
            

            check_success_do(RF_OK != retv, json_object_object_del_ex(prop_val_jso, name));
            
        }
    } else if (json_type_array == type) { 
        json_object *obj_iter = NULL;

        object_schema = _get_items_object_schema(prop_schema_jso);
        check_fail_do_return_val(object_schema, RF_PROP_TYPE_ERR,
            debug_log(DLOG_ERROR, "cann't get schema for %s.", dal_json_object_get_str(prop_val_jso)));

        json_object_array_foreach(prop_val_jso, obj_iter)
        {
            char index_buf[MAX_ARRAY_INDEX_LEN] = {0};
            
            (void)snprintf_s(index_buf, MAX_ARRAY_INDEX_LEN, MAX_ARRAY_INDEX_LEN - 1, "%d", i++);
            
            
            // 允许数组成员是空对象来指定数组元素设置
            continue_if_expr_true(VOS_OK == g_strcmp0(JSON_NULL_OBJECT_STR, dal_json_object_get_str(obj_iter)));
            
            _push_property_path(&property_path_list, (char *)index_buf);
            
            retv = _get_and_check_prop(patch_check, action_name, object_schema, obj_iter, jso_extend_arr,
                property_path_list, NULL);
            _pop_property_path(&property_path_list);

            
            _construct_property_item_message(retv, property_path_list, index_buf, dal_json_object_get_str(obj_iter),
                jso_extend_arr);
            

            
            
            // 增加RF_ARRAY_ERROR 返回码，为了使对象中的数组检查到错误不直接退出格式检查
            check_fail_return_val(RF_OK == retv, RF_ARRAY_ERROR);
            
            
        }
    } else {
        debug_log(DLOG_ERROR, "%s %d:Invalid type :%d.", __FUNCTION__, __LINE__, type);
    }

    return RF_OK;
}


json_bool redfish_post_format_check(const gchar *redfish_class, const gchar *jso_body_str,
    const gchar *action_name, 
    json_object **jso_body_parsed, json_object *jso_extend_arr)
{
    json_object *jso_body = NULL;
    json_object *jso_property_schema = NULL;
    json_bool bool_result;
    GSList *property_path_list = NULL;
    json_bool ret = TRUE;

    check_fail_return_val(redfish_class != NULL, FALSE);
    check_fail_return_val(jso_body_str != NULL, FALSE);
    check_fail_return_val(jso_body_parsed != NULL, FALSE);
    check_fail_return_val(jso_extend_arr != NULL, FALSE);

    *jso_body_parsed = NULL;

    (void)pthread_mutex_lock(&g_class_def_mutex);

    
    jso_body = _parse_json_str(jso_body_str, jso_extend_arr, TRUE, action_name);
    check_fail_do_and_goto(jso_body != NULL, ret = FALSE, quit);

    
    debug_log(DLOG_INFO, "redfish_class=%s", redfish_class);
    bool_result = _missing_check(get_class_schema(redfish_class), jso_body, action_name, jso_extend_arr);

    
    
    if ('\0' == *jso_body_str) {
        json_object *jso_message = NULL;

        if (!action_name || !bool_result) {
            json_object_array_clean(jso_extend_arr);

            if (VOS_OK == create_message_info(MSGID_MALFORMED_JSON, NULL, &jso_message)) {
                (void)json_object_array_add(jso_extend_arr, jso_message);
            }
        }

        goto quit;
    }

    
    check_fail_do_and_goto(TRUE == bool_result, ret = FALSE, quit);

    
    if (NULL != action_name) {
        (void)_get_action(action_name, &jso_property_schema);
    } else {
        jso_property_schema = get_class_schema(redfish_class);
    }
    
    json_object_clear_string(jso_body, RF_SESSION_REAUTH_KEY);
    json_object_object_del(jso_body, RF_SESSION_REAUTH_KEY);
    (void)_get_and_check_prop(FALSE, action_name, jso_property_schema, jso_body, jso_extend_arr, property_path_list,
        NULL);

quit:

    
    if (0 == json_object_array_length(jso_extend_arr) && ret) {
        format_rsc_json(jso_property_schema, jso_body);
        *jso_body_parsed = jso_body;
        ret = TRUE;
    } else {
        
        check_success_do(jso_body, (void)json_object_put(jso_body));
        ret = FALSE;
    }

    _free_global_class_jso();

    (void)pthread_mutex_unlock(&g_class_def_mutex);

    

    return ret;
}


json_bool redfish_patch_format_check(const gchar *redfish_class, const gchar *jso_body_str,
    json_object **jso_body_parsed, json_object *jso_extend_arr)
{
    json_object *jso_body = NULL;
    json_object *jso_message = NULL;
    GSList *property_path = NULL;
    rf_retvalue rf_ret;
    json_bool b_ret = TRUE;
    json_object *obj_schema = NULL;

    check_fail_return_val(redfish_class != NULL, FALSE);
    check_fail_return_val(jso_body_str != NULL, FALSE);
    check_fail_return_val(jso_body_parsed != NULL, FALSE);
    check_fail_return_val(jso_extend_arr != NULL, FALSE);

    (void)pthread_mutex_lock(&g_class_def_mutex);

    
    
    if ('\0' == *jso_body_str) {
        if (VOS_OK == create_message_info(MSGID_MALFORMED_JSON, NULL, &jso_message)) {
            
            do_val_if_expr(VOS_OK != json_object_array_add(jso_extend_arr, jso_message),
                (void)json_object_put(jso_message));
            
        }

        goto quit;
    }

    

    jso_body = _parse_json_str(jso_body_str, jso_extend_arr, FALSE, NULL);
    goto_label_do_info_if_fail(jso_body != NULL, quit, debug_log(DLOG_ERROR, "%s _parse_json_str fail", __FUNCTION__));

    
    json_object_clear_string(jso_body, RF_SESSION_REAUTH_KEY);
    json_object_object_del(jso_body, RF_SESSION_REAUTH_KEY);
    
    
    obj_schema = get_class_schema(redfish_class);
    rf_ret = _get_and_check_prop(TRUE, NULL, obj_schema, jso_body, jso_extend_arr, property_path, NULL);
    
    check_fail_do(RF_OK == rf_ret, b_ret = FALSE);

    format_rsc_json(obj_schema, jso_body);

quit:

    _free_global_class_jso();

    *jso_body_parsed = jso_body;

    (void)pthread_mutex_unlock(&g_class_def_mutex);

    

    
    
    do_if_expr(FALSE == b_ret, json_object_put(*jso_body_parsed); *jso_body_parsed = NULL);
    

    return b_ret;
}


void format_rsc_json(json_object *pattern_jso, json_object *rsc_jso)
{
    json_type t = json_object_get_type(rsc_jso);
    json_object *object_schema = _get_object_schema(pattern_jso);
    json_object *prop_schema_jso = NULL;
    json_object *obj_iter = NULL;

    if (json_type_object == t) {
        standardize_req_json_data(object_schema, rsc_jso);

        json_object_object_foreach0(rsc_jso, prop_key, prop_val)
        {
            (void)json_object_object_get_ex(object_schema, prop_key, &prop_schema_jso);
            check_fail_do_continue(prop_schema_jso, debug_log(DLOG_ERROR, "cann't find property name for %s from %s",
                prop_key, dal_json_object_get_str(object_schema)));

            format_rsc_json(prop_schema_jso, prop_val);
        }
    } else if (json_type_array == t) {
        object_schema = _get_items_object_schema(pattern_jso);
        check_fail_do_return(object_schema,
            debug_log(DLOG_ERROR, "cann't get schema for %s.", dal_json_object_get_str(rsc_jso)));

        json_object_array_foreach(rsc_jso, obj_iter)
        {
            format_rsc_json(object_schema, obj_iter);
        }
    } else {
        return;
    }

    return;
}


void redfish_class_dump(const char *dump_path)
{
    json_object *global_jso;
    global_jso = _get_global_class_jso();

    (void)json_object_to_file_ext(dump_path, global_jso, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_SPACED);

    return;
}

#ifdef UT_TEST
void redfish_class_dump_plain(void)
{
    json_object *global_jso;
    json_object *jso_property = NULL;
    global_jso = _get_global_class_jso();

    json_object_iter iter_property;

    // require GCC 4.6
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
    json_object_object_foreach0(global_jso, name, val)
    {
        if ((TRUE == json_object_is_type(val, json_type_object)) &&
            (TRUE == json_object_object_get_ex(val, "type", &jso_property))) {
            if (0 == g_strcmp0(dal_json_object_get_str(jso_property), "object")) {
                if (TRUE == json_object_object_get_ex(val, "properties", &jso_property)) {
                    json_object_object_foreach_ex(jso_property, iter_property)
                    {
                        printf("%s\t%s\n", name, iter_property.key);
                    }
                }
            }
        }
    }
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif

    return;
}

struct tag_test {
    const gchar *error_type;
    const gchar *redfish_class;
    const gchar *body_str;
};

// normal
#define post1 \
    "{\
    \"Password\" : \"UserName\",\
    \"UserName\" : \"admin\" ,\
    \"RoleId\"   : \"Administrator\"  \
}"

// PropertyDuplicate
#define post2 \
    "{\
    \"Password\" : \"UserName\",\
    \"Password\" : \"UserName2\",\
    \"UserName\" : \"admin\" ,\
    \"RoleId\"   : \"Administrator\"  \
}"

// PropertyUnknown&Missing
#define post3 \
    "{\
    \"Passwor\" : \"UserName\",\
    \"UserName\" : \"admin\" ,\
    \"RoleId\"   : \"Administrator\"  \
}"

// PropertyValueTypeError
#define post4 \
    "{\
    \"Password\" : true,\
    \"UserName\" : \"admin\" ,\
    \"RoleId\"   : \"Administrator\"  \
}"

// PropertyValueTypeError
#define post5 \
    "{\
    \"VLANEnable\" : true,\
    \"VLANId\" : \"4000\"\
}"

// normal
#define post6 \
    "{\
    \"Destination\" : \"http://10.0.0.1\",\
    \"EventTypes\" : [ \"StatusChange\" ],\
    \"Context\" : \"context\",\
    \"Protocol\" : \"Redfish\"\
}"

// PropertyValueNotInList
#define post7 \
    "{\
    \"Destination\" : \"http://10.0.0.1\",\
    \"EventTypes\" : [ \"StatusChange1\" ],\
    \"Context\" : \"context\",\
    \"Protocol\" : \"Redfish\"\
}"

// normal
#define post8 \
    "{\
    \"Destination\": \"http://10.0.0.1\",\
    \"EventTypes\" : [ \"StatusChange\", \"ResourceUpdated\" ],  \
    \"Context\" : \"context\",\
    \"Protocol\" : \"Redfish\"\
}"

// Malformed
#define post9 \
    "{\
    \"Destination: \"http://10.0.0.1\",\
    \"EventTypes\" : [ \"StatusChange\", \"ResourceUpdated\" ],  \
    \"Context\" : \"context\",\
    \"Protocol\" : \"Redfish\"\
}"

// not in the list
#define post10 \
    "{\
    \"Destination\": \"http://10.0.0.1\",\
    \"EventTypes\" : [ \"StatusChange1\", \"ResourceUpdated\" ],  \
    \"Context\" : \"context\",\
    \"Protocol\" : \"Redfish\"\
}"

// format error
#define post11 \
    "{\
    \"Destination\" : \"http://10.0.0.1\",\
    \"EventTypes\" : \"StatusChange\",\
    \"Context\" : \"context\",\
    \"Protocol\" : \"Redfish\"\
}"

#define post12 \
    "{\
                \"Destination\": \"http://10.184.49.151:9090/event_recieve/1\",\
                \"EventTypes\": [\
                \"ResourceAdded\",\
                \"ResourceRemoved\",\
                \"Alert\"\
                ],\
                \"HttpHeaders\":[ \
                {\"server_name\": \"eventserver\"},\
                {\"server_number\": \"0001\"}\
                ],\
                \"Context\": \"http://10.184.49.151:9090/event_recieve/1\",\
                \"Protocol\": \"Redfish\"\
}"

#define post13 \
    "{\
    \"Password\" : \"UserName\",\
    \"UserName\" : \"admin\" ,\
    \"123456\" : \"123456\",\
    \"RoleId\"   : \"Administrator\"\
}"

struct tag_test post_test[] = {
    {"OK",                       "ManagerAccount",   post1},
    {"PropertyDuplicate",        "ManagerAccount",   post2},
    {"Missing&Unknown",          "ManagerAccount",   post3},
    {"PropertyValueTypeError",   "ManagerAccount",   post4},
    {"PropertyValueTypeError",   "VLAN",             post5},
    {"OK",                       "EventDestination", post6},
    {"PropertyValueNotInList",   "EventDestination", post7},
    {"OK",                       "EventDestination", post8},
    {"Malformed",                "EventDestination", post9},
    {"PropertyValueNotInList",   "EventDestination", post10},
    {"PropertyValueFormatError", "EventDestination", post11},
    {"OK",                       "EventDestination", post12},
    {"PropertyUnknown",          "ManagerAccount",   post13},
};

// normal
#define patch1 \
    "{\"BootSourceOverrideTarget\" : \"Pxe\",\
                 \"BootSourceOverrideEnabled\" : \"Once\",\
\"UefiTargetBootSourceOverride\" : \"None\"}"

// duplicate
#define patch2 \
    "{\"BootSourceOverrideTarget\" : \"Pxe\",\
                 \"BootSourceOverrideEnabled\" : \"Once\",\
                 \"BootSourceOverrideEnabled\" : \"Once\",\
\"UefiTargetBootSourceOverride\" : \"None\"}"

// unknown
#define patch3 "{\"BootSourceOverride\" : \"Pxe\"}"

// unknown
#define patch4 \
    "{\"BootSourceOverrideTarget\" : \"Pxe\",\
                 \"BootSourceOverride\" : \"Once\",\
\"UefiTargetBootSourceOverride\" : \"None\"}"

// not in list
#define patch5 \
    "{\"BootSourceOverrideTarget\" : \"Pxee\",\
                 \"BootSourceOverrideEnabled\" : \"Once\",\
\"UefiTargetBootSourceOverride\" : \"None\"}"

// Manager  NotWritable
#define patch6 "{\"ManagerType\" : \"RackManager\"}"

// ValueTypeError
#define patch7 "{\"SessionTimeout\" : \"30\"}"

// SessionService format err
#define patch8 "{\"SessionTimeout\" : 10}"
#define patch11 "{\"SessionTimeout\" : 1000}"
#define patch12 "{\"SessionTimeout\" : 86400}"
#define patch13 "{\"SessionTimeout\" : 86401}"

#define patch9 \
    "{\
                \"ProcessorSummary\": {\
                            \"Count\": 8,\
                            \"Model\": \"Multi-Core Intel(R) Xeon(R) processor 7xxx Series\",\
                            \"Status\": {\
                                \"State\": \"Enabled\",\
                                \"Health\": \"OK\",  \
                                \"HealthRollUp\": \"OK\"\
                            }\
                        }\
}"

#define patch10 \
    "{\
                \"ProcessorSummary\": {\
                            \"Count\": \"hex\",\
                            \"Model\": \"Multi-Core Intel(R) Xeon(R) processor 7xxx Series\",\
                            \"Status\": {\
                                \"State\": \"Enabled\",\
                                \"Health\": \"OK\",  \
                                \"HealthRollUp\": \"OK\"\
                            }\
                        }\
}"

struct tag_test patch_test[] = {
    {"OK",                      "Boot",           patch1},
    {"PropertyDuplicate",       "Boot",           patch2},
    {"PropertyUnknown",         "Boot",           patch3},
    {"PropertyUnknown",         "Boot",           patch4},
    {"PropertyValueNotInList",  "Boot",           patch5},
    {"NotWritable",             "Manager",        patch6},
    {"ValueTypeError",          "SessionService", patch7},
    {"FormatError",             "SessionService", patch8},
    {"OK",                      "ComputerSystem", patch9},
    {"TypeError",               "ComputerSystem", patch10},
    {"OK",                      "SessionService", patch11},
    {"OK",                      "SessionService", patch12},
    {"FormatError",             "SessionService", patch13},
};

int main(int argc, char **argv)
{
    json_object *jso_body_parsed = NULL;
    json_object *jso_err_message = NULL;
    json_object *message_array_jso = NULL;

    redfish_message_init();

    printf("=================redfish_class_dump_plain=================\n");
    redfish_class_dump_plain();
    printf("=================redfish_class_dump=================\n");
    redfish_class_dump();

    int i;

    printf("=== post test start:\n");

    for (i = 0; i < sizeof(post_test) / sizeof(post_test[0]); i++) {
        message_array_jso = json_object_new_array();
        jso_body_parsed = NULL;
        jso_err_message = NULL;
        redfish_post_format_check(post_test[i].redfish_class, post_test[i].body_str, NULL, &jso_body_parsed,
            message_array_jso);
        printf("--------------------------%d--------------------------\r\n", i);
        printf("test body str:\n%s\n", post_test[i].body_str);
        printf("checked body:\n%s\n",
            json_object_to_json_string_ext(jso_body_parsed, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
        printf("test error type: %s\n", post_test[i].error_type);
        printf("message[%d]:\n%s\n\n", json_object_array_length(message_array_jso),
            json_object_to_json_string_ext(message_array_jso, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));

        if (jso_body_parsed) {
            (void)json_object_put(jso_body_parsed);
        }

        if (message_array_jso) {
            (void)json_object_put(message_array_jso);
        }
    }

    return 0;
}

#endif
