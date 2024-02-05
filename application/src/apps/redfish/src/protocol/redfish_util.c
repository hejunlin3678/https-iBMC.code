

#include "pme_app/pme_app.h"

#include "pme_app/uip/uip.h"

#include "redfish_util.h"

#include "redfish_provider.h"

#include "json_patch_util.h"

#include <sys/time.h>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "redfish_http.h"

#define HW_ACCESS_FAIL_MASK 0x8000
#define HW_ACCESS_IN_UPDATE_PROGRESS 0x4000

LOCAL pthread_mutex_t g_export_task_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef union _tag_g_variant_type_data {
    gchar c;
    gdouble d;
    gboolean b;
    gint i;
    const gchar *s;
} VARIANT_TYPE_DATA;

gint32 digital_str_bits(const gchar *token_begin)
{
    gint32 i = 0;
    const gchar *str_temp = NULL;

    return_val_if_fail(NULL != token_begin, 0);

    str_temp = token_begin;

    while ('\0' != *str_temp) {
        if (*str_temp >= '0' && *str_temp <= '9') {
            i++;
        } else {
            break;
        }

        str_temp++;
    }

    return i;
}

gint32 num_format_string_translate(const gchar *num_format_str, gint32 arg_num, gchar **format_str_out)
{
    gchar *dup_str = NULL;
    const gchar *p_index = NULL;
    gint32 i = 0;
    gint32 real_arg_num = 0;

    return_val_do_info_if_fail((NULL != num_format_str) && (NULL != format_str_out), VOS_ERR,
        debug_log(DLOG_ERROR, "%s : input param error", __FUNCTION__));

    dup_str = g_strdup(num_format_str);
    return_val_do_info_if_fail(NULL != dup_str, VOS_ERR, debug_log(DLOG_ERROR, "%s : dup_str is null", __FUNCTION__));
    p_index = num_format_str;

    while ('\0' != *p_index) {
        gint32 num_bits = digital_str_bits(p_index + 1);
        
        if ('%' == *p_index && num_bits != 0) {
            dup_str[i++] = '%';
            dup_str[i++] = 's';
            p_index = p_index + 1 + num_bits;
            real_arg_num++;
        }
        
        else {
            dup_str[i++] = *p_index;
            p_index++;
        }
    }

    dup_str[i] = '\0';

    if (-1 != arg_num && arg_num != real_arg_num) {
        g_free(dup_str);
        return -1;
    }

    *format_str_out = dup_str;

    return 0;
}


int split_path(char *path, guint32 path_size, char *segments[])
{
    char *p_version_segment = NULL;
    int i = 0;
    char *p = NULL;
    guint32 path_len = strlen(path);

    return_val_if_expr(path == NULL || (g_strcmp0(path, "") == 0), -1);
    return_val_if_expr(path_len >= path_size, -1);

    if (NULL != (p_version_segment = strstr(path, SERVICE_ROOT_PATH))) {
        path = p_version_segment + strlen(SERVICE_ROOT_PATH);
    }

    
    if (*path == '/') {
        path++;
    }

    if (path[strlen(path) - 1] == '/') {
        path[strlen(path) - 1] = '\0';
    }

    check_fail_return_val(g_strcmp0(path, ""), 0);

    while ((p = strsep(&path, "/")) != NULL) {
        if (i >= MAX_URI_SEGMENT_NUM)
            return -1;
        segments[i++] = p;
    }

    return i;
}


gint32 json_custom_get_elem_str(json_object *i_jso, const gchar *i_element_name, gchar *o_str, guint32 i_str_len)
{
    errno_t safe_fun_ret;
    json_object *str_jso = NULL;
    const gchar *str = NULL;
    json_bool b_ret;

    return_val_do_info_if_fail(((NULL != i_jso) && (NULL != i_element_name) && (NULL != o_str) && (i_str_len > 0)),
        RF_FAILED, debug_log(DLOG_ERROR, "input param error"));

    b_ret = json_object_object_get_ex(i_jso, i_element_name, &str_jso);
    return_val_do_info_if_fail((b_ret && (json_type_string == json_object_get_type(str_jso))), RF_FAILED,
        debug_log(DLOG_MASS, "get %s error", i_element_name));

    // 转换为字符串
    str = dal_json_object_get_str(str_jso);
    return_val_do_info_if_fail((NULL != str), RF_FAILED, debug_log(DLOG_ERROR, "dal_json_object_get_str fail"));

    safe_fun_ret = strncpy_s(o_str, i_str_len, str, i_str_len - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    return RF_OK;
}

LOCAL gint32 get_odata_type(const char *class_with_version, char *odata_type, int len)
{
    int rc;

    rc = snprintf_s(odata_type, len, len - 1, "#%s", class_with_version);
    check_fail_return_val(rc > 0, RF_PARAM_ERROR);

    return RF_OK;
}


LOCAL gint32 get_odata_head_info(const char *original_path, char *odata_id, int odata_id_len, char *odata_context,
    int odata_context_max_len, char *odata_type, int odata_type_max_len)
{
    int iRet = -1;
    errno_t safe_fun_ret;
    char           odata_id_dump[MAX_URI_LENGTH] = {0};
    char*          path_segments[MAX_URI_SEGMENT_NUM] = {NULL};
    int path_segment_num;
    int i;
    json_object *cfg_file_jso = NULL;
    const char *class_with_version = NULL;
    gboolean b_ret;
    char           segment_buf[MAX_SEGMENT_CHARACTER_NUM] = {0};
    json_object *cur_class_jso = NULL;
    const char *real_segment = NULL;
    const char *metadata_segment = "/redfish/v1/$metadata#";

    check_fail_return_val(original_path, RF_FAILED);

    safe_fun_ret = strcpy_s(odata_id_dump, sizeof(odata_id_dump), original_path);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    path_segment_num = split_path(odata_id_dump, sizeof(odata_id_dump), path_segments);

    (void)strcpy_s(odata_context, MAX_URI_LENGTH, metadata_segment);
    (void)strcpy_s(odata_id, MAX_URI_LENGTH, SERVICE_ROOT_PATH);
    cfg_file_jso = get_resource_cfg_json(RSC_CFG_FILE_PATH);

    
    check_success_do(0 == path_segment_num,
        (void)strncat_s(odata_context, MAX_URI_LENGTH, RF_CLASS_SERVICEROOT, strlen(RF_CLASS_SERVICEROOT)));

    
    for (i = 0; i < path_segment_num; i++) {
        b_ret = json_object_object_case_get(cfg_file_jso, path_segments[i], &real_segment, &cur_class_jso);
        if (b_ret) {
            
            iRet =
                snprintf_s(segment_buf, MAX_SEGMENT_CHARACTER_NUM, MAX_SEGMENT_CHARACTER_NUM - 1, "%s/", real_segment);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            safe_fun_ret = strncat_s(odata_context, MAX_URI_LENGTH, segment_buf, strlen(segment_buf));
            do_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

            safe_fun_ret = strncat_s(odata_id, MAX_URI_LENGTH, "/", strlen("/"));
            do_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            safe_fun_ret = strncat_s(odata_id, MAX_URI_LENGTH, real_segment, strlen(real_segment));
            do_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        } else if (json_object_object_get_ex(cfg_file_jso, RF_ID_EXP, &cur_class_jso)) {
            
            safe_fun_ret = strncat_s(odata_context, MAX_URI_LENGTH, "Members/", strlen("Members/"));
            do_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

            
            if (i != path_segment_num - 1) {
                safe_fun_ret = strncat_s(odata_context, MAX_URI_LENGTH, path_segments[i], strlen(path_segments[i]));
                do_if_expr(safe_fun_ret != EOK,
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
                safe_fun_ret = strncat_s(odata_context, MAX_URI_LENGTH, "/", strlen("/"));
                do_if_expr(safe_fun_ret != EOK,
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            }

            safe_fun_ret = strncat_s(odata_id, MAX_URI_LENGTH, "/", strlen("/"));
            do_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            safe_fun_ret = strncat_s(odata_id, MAX_URI_LENGTH, path_segments[i], strlen(path_segments[i]));
            do_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        } else {
            debug_log(DLOG_INFO, "Generate @odata.context error, current path segment is %s", path_segments[i]);
            
            (void)json_object_put(cfg_file_jso);
            return RF_FAILED;
            
        }

        (void)json_object_get(cur_class_jso);
        (void)json_object_put(cfg_file_jso);

        cfg_file_jso = cur_class_jso;
    }

    
    if (path_segment_num > 1) {
        safe_fun_ret = strncat_s(odata_context, MAX_URI_LENGTH, "$entity", strlen("$entity"));
        do_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }

    
    if (1 == path_segment_num) {
        int len = strlen(odata_context);

        odata_context[len - 1] = '\0';
    }

    b_ret = get_element_str(cfg_file_jso, REDFISH_CLASS_NAME, &class_with_version);

    check_fail_do_return_val_spec(b_ret, RF_FAILED, (void)json_object_put(cfg_file_jso),
        debug_log(DLOG_ERROR, "cann't get class info from %s", odata_id));

    
    if (g_regex_match_simple(SESSION_CLASS_REGEX, class_with_version, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0)) {
        safe_fun_ret =
            strncpy_s(odata_context, odata_context_max_len, SESSION_ODATA_CONTEXT, strlen(SESSION_ODATA_CONTEXT));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }

    int ret = get_odata_type(class_with_version, odata_type, odata_type_max_len);
    (void)json_object_put(cfg_file_jso);

    return ret;
}


gint32 fill_odata_head_info(const char *odata_id_str, json_object *rsc_jso)
{
    char   odata_context[MAX_URI_LENGTH] = {0};
    char   odata_type[MAX_CHARACTER_NUM] = {0};
    char   odata_id[MAX_URI_LENGTH] = {0};
    int ret;

    check_fail_return_val(odata_id_str && rsc_jso, RF_PARAM_ERROR);

    ret = get_odata_head_info(odata_id_str, odata_id, MAX_URI_LENGTH, odata_context, MAX_URI_LENGTH, odata_type,
        MAX_CHARACTER_NUM);
    if (RF_OK == ret) {
        json_object_object_add(rsc_jso, ODATA_CONTEXT, json_object_new_string((const gchar *)odata_context));
        json_object_object_add(rsc_jso, ODATA_TYPE, json_object_new_string((const gchar *)odata_type));
        json_object_object_add(rsc_jso, ODATA_ID, json_object_new_string((const gchar *)odata_id));

        return RF_OK;
    }

    return RF_FAILED;
}


gint32 get_str_val_from_map_info(INT2STR_MAP_S *map_info, guint32 size, gint int_val, const gchar **str_val)
{
    guint32 i;

    return_val_do_info_if_fail((NULL != map_info) && (NULL != str_val), RF_FAILED,
        debug_log(DLOG_ERROR, "input param error"));
    *str_val = NULL;

    for (i = 0; i < size; i++) {
        if (int_val == map_info[i].int_val) {
            *str_val = map_info[i].str_val;

            return RF_OK;
        }
    }

    return RF_FAILED;
}


const gchar *g_str_case_rstr(const gchar *haystack, const gchar *needle)
{
    int needle_len;
    int haystack_len;
    int i;

    if ((NULL == haystack) || (NULL == needle)) {
        return NULL;
    }

    needle_len = strlen(needle);
    haystack_len = strlen(haystack);
    if ((needle_len == 0) || (haystack_len == 0)) {
        return NULL;
    }

    i = haystack_len;

    while (i--) {
        int ret;

        ret = g_ascii_strncasecmp(haystack + i, needle, needle_len);
        if (0 == ret) {
            return (const gchar *)(haystack + i);
        }
    }

    return NULL;
}


gint32 str_delete_char(gchar *str, gchar c)
{
    gint len;
    gint i = 0;
    gint j;

    return_val_do_info_if_fail((NULL != str), VOS_ERR, debug_log(DLOG_ERROR, "input param error"));

    len = strlen(str);
    j = i;

    while (i < len) {
        if (str[i] != c) {
            str[j] = str[i];
            j++;
        }

        i++;
    }

    str[j] = '\0';

    return VOS_OK;
}


gint32 string_v_variant_to_json_data(GVariant *var, json_object **arr_jso)
{
    const gchar *var_type_str = NULL;
    const gchar **str_v;
    gsize i;
    json_object *jso = NULL;
    gint32 ret;
    json_object *str_jso = NULL;
    gsize parm_temp = 0;

    var_type_str = g_variant_get_type_string(var);
    return_val_do_info_if_fail(0 == g_strcmp0((const gchar *)G_VARIANT_TYPE_STRING_ARRAY, var_type_str), VOS_ERR,
        debug_log(DLOG_ERROR, "error use %s:support string array variant type only, type:%s", __FUNCTION__,
        var_type_str));

    str_v = g_variant_get_strv(var, &parm_temp);
    return_val_do_info_if_fail(NULL != str_v, VOS_ERR, debug_log(DLOG_ERROR, "get string array from variant failed"));

    jso = json_object_new_array();

    
    if ((parm_temp != 1) || (0 != g_strcmp0(RF_STR_EMPTY, str_v[0]))) {
        for (i = 0; i < parm_temp; i++) {
            str_jso = json_object_new_string(str_v[i]);

            continue_do_info_if_fail(NULL != str_jso, debug_log(DLOG_ERROR, "new json string failed"));

            ret = json_object_array_add(jso, str_jso);
            continue_do_info_if_fail(0 == ret, debug_log(DLOG_ERROR, "add item to json array failed, ret is %d", ret);
                (void)json_object_put(str_jso));
        }
    }

    

    g_free(str_v);
    *arr_jso = jso;

    return VOS_OK;
}


static guint32 _get_gvariant_type_fixed_size(gchar type_char)
{
    switch (type_char) {
        case 'i':
            return sizeof(int);

        case 'b':
            return sizeof(gboolean);

        case 'y':
            return sizeof(gchar);

        case 'd':
            return sizeof(double);

        default:
            debug_log(DLOG_ERROR, "unsupported gvariant type:%c", type_char);
            return 0;
    }
}


gint32 array_variant_to_json_data(GVariant *var, json_object **arr_jso)
{
    gsize n;
    gsize element_type_size;
    const gchar *type_str = NULL;
    gchar element_type_char;
    gconstpointer fixed_size_data = NULL;
    gsize i;
    json_object *jso = NULL;

    type_str = g_variant_get_type_string(var);
    return_val_do_info_if_fail((NULL != type_str) && ('a' == type_str[0]) && (strlen(type_str) > 1), VOS_ERR,
        debug_log(DLOG_ERROR, "error use %s:input param error", __FUNCTION__));

    element_type_char = type_str[1];
    element_type_size = _get_gvariant_type_fixed_size(element_type_char);
    
    return_val_do_info_if_fail(element_type_size > 0, VOS_ERR,
        debug_log(DLOG_ERROR, "error use %s:support fixed size type array only, checked type:%c", __FUNCTION__,
        element_type_char));
    
    
    fixed_size_data = g_variant_get_fixed_array(var, &n, element_type_size);
    return_val_do_info_if_fail(NULL != fixed_size_data, VOS_ERR,
        debug_log(DLOG_ERROR, "g_variant_get_fixed_array error"));

    jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != jso, VOS_ERR, debug_log(DLOG_ERROR, "g_variant_get_fixed_array error"));
    

    for (i = 0; i < n; i++) {
        switch (element_type_char) {
            case 'i':
            case 'y':
                json_object_array_add(jso, json_object_new_int(((const int *)fixed_size_data)[i]));
                break;

            case 'b':
                json_object_array_add(jso, json_object_new_boolean(((const gboolean *)fixed_size_data)[i]));
                break;

            case 'd':
                json_object_array_add(jso, ex_json_object_new_double(((const gdouble*)fixed_size_data)[i], "%.2f"));
                break;

            default:
                debug_log(DLOG_ERROR, "%s failed, unsupported type :%c", __FUNCTION__, element_type_char);

                (void)json_object_put(jso);

                return VOS_ERR;
        }
    }

    *arr_jso = jso;

    return VOS_OK;
}


gint32 rf_get_json_array_from_variant(GVariant *var, json_object **arr_jso)
{
    gint32 ret;
    const gchar *type_str;

    type_str = g_variant_get_type_string(var);
    if (NULL == type_str) {
        debug_log(DLOG_ERROR, "%s failed:cann't get variant type", __FUNCTION__);

        return VOS_ERR;
    }

    if (0 == g_strcmp0(type_str, "as")) {
        ret = string_v_variant_to_json_data(var, arr_jso);
    } else if ('a' == type_str[0]) {
        ret = array_variant_to_json_data(var, arr_jso);
    } else {
        debug_log(DLOG_ERROR, "unsupported variant type:%s", (const gchar *)type_str);
        ret = VOS_ERR;
    }

    return ret;
}


gint32 rf_get_json_data_from_variant(GVariant *var, json_object **jso)
{
    const gchar *type_str = NULL;
    gchar type_char;
    
    json_object *jso_tmp = NULL;
    
    VARIANT_TYPE_DATA data;

    type_str = g_variant_get_type_string(var);
    if (NULL == type_str) {
        debug_log(DLOG_ERROR, "%s failed:cann't get variant type", __FUNCTION__);

        return VOS_ERR;
    }

    
    if (strlen(type_str) > 1) {
        debug_log(DLOG_ERROR, "%s failed:unsuppoted gvariant type %s", __FUNCTION__, type_str);

        return VOS_ERR;
    }

    type_char = type_str[0];

    switch (type_char) {
        case 'b':
            data.b = g_variant_get_boolean(var);
            jso_tmp = json_object_new_boolean(data.b);
            break;

        case 'y':
            data.i = g_variant_get_byte(var);
            jso_tmp = json_object_new_int(data.i);
            break;

        case 'd':
            data.d = g_variant_get_double(var);
            jso_tmp = ex_json_object_new_double(data.d, "%.2f");
            break;

        case 'i':
            data.i = g_variant_get_int32(var);
            jso_tmp = json_object_new_int(data.i);
            break;

        case 'q':
            data.i = g_variant_get_uint16(var);
            jso_tmp = json_object_new_int(data.i);
            break;

        case 's':
            data.s = g_variant_get_string(var, NULL);
            jso_tmp = json_object_new_string(data.s);
            break;

        default:
            debug_log(DLOG_ERROR, "unsupported variant type:%c", type_char);
            return VOS_ERR;
    }

    *jso = jso_tmp;

    return VOS_OK;
}


gint32 rf_obj_handle_data_to_rsc_data(OBJ_HANDLE obj_handle, const gchar *obj_prop_name, const gchar *rsc_prop_name,
    json_object *rsc_jso)
{
    gint32 ret;
    const gchar *obj_name = NULL;
    GVariant *var = NULL;
    const gchar *type_str = NULL;
    json_object *val_jso = NULL;

    return_val_do_info_if_fail((NULL != obj_prop_name) && (NULL != rsc_prop_name) && (NULL != rsc_jso), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    obj_name = dfl_get_object_name(obj_handle);
    return_val_do_info_if_fail((NULL != obj_name), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:invalid object handle", __FUNCTION__));

    ret = dfl_get_property_value(obj_handle, obj_prop_name, &var);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "get property %s for object %s failed", obj_prop_name, obj_name);

        return VOS_ERR;
    }

    type_str = g_variant_get_type_string(var);
    return_val_do_info_if_fail((NULL != type_str), VOS_ERR, debug_log(DLOG_ERROR, "get g_variant type string failed");
        g_variant_unref(var));

    
    if ('a' == type_str[0]) {
        ret = rf_get_json_array_from_variant(var, &val_jso);

        
        do_val_if_fail(VOS_OK == ret, (val_jso = json_object_new_array()));
    } else {
        ret = rf_get_json_data_from_variant(var, &val_jso);
    }

    g_variant_unref(var);

    
    do_val_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "trans g_variant data to json data failed, ret is %d", ret));

    json_object_object_add(rsc_jso, rsc_prop_name, val_jso);

    return VOS_OK;
}


gint32 rf_obj_handle_data_trans_to_rsc_data(OBJ_HANDLE obj_handle, const gchar *obj_prop_name,
    const gchar *rsc_prop_name, json_object *rsc_jso)
{
    GVariant *var = NULL;
    gint32 ret;
    const gchar *obj_name = NULL;
    const gchar *type_str = NULL;
    gchar type_char;
    json_object *val_jso = NULL;

    return_val_do_info_if_fail((NULL != obj_prop_name) && (NULL != rsc_prop_name) && (NULL != rsc_jso), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    obj_name = dfl_get_object_name(obj_handle);
    return_val_do_info_if_fail((NULL != obj_name), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:invalid object handle", __FUNCTION__));

    ret = dfl_get_property_value(obj_handle, obj_prop_name, &var);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "get property %s for object %s failed", obj_prop_name, obj_name);

        return VOS_ERR;
    }

    type_str = g_variant_get_type_string(var);
    return_val_do_info_if_fail((NULL != type_str), VOS_ERR, debug_log(DLOG_ERROR, "get g_variant type string failed");
        g_variant_unref(var));

    type_char = type_str[0];

    switch (type_char) {
        case 'a':
            ret = rf_get_json_array_from_variant(var, &val_jso);
            
            do_val_if_fail(VOS_OK == ret, (val_jso = json_object_new_array()));
            break;

        case 'i':
            if (HW_ACCESS_IN_UPDATE_PROGRESS == g_variant_get_uint32(var) ||
                HW_ACCESS_FAIL_MASK == g_variant_get_uint32(var)) {
                
                debug_log(DLOG_DEBUG, "trans g_variant data %d to null", g_variant_get_uint32(var));
                break;
            } else {
                ret = rf_get_json_data_from_variant(var, &val_jso);
                break;
            }

        case 'q':
            if (HW_ACCESS_IN_UPDATE_PROGRESS == g_variant_get_uint16(var) ||
                HW_ACCESS_FAIL_MASK == g_variant_get_uint16(var)) {
                
                debug_log(DLOG_DEBUG, "trans g_variant data %d to null", g_variant_get_uint16(var));
                break;
            } else {
                ret = rf_get_json_data_from_variant(var, &val_jso);
                break;
            }

        case 's':
            if (0 == strcmp("", g_variant_get_string(var, NULL)) ||
                0 == strcmp(INVALID_DATA_STRING, g_variant_get_string(var, NULL))) {
                
                debug_log(DLOG_DEBUG, "trans g_variant data %s to null", g_variant_get_string(var, NULL));
                break;
            } else {
                ret = rf_get_json_data_from_variant(var, &val_jso);
                break;
            }

        default:
            ret = rf_get_json_data_from_variant(var, &val_jso);
    }

    g_variant_unref(var);

    
    do_val_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "trans g_variant data to json data failed, ret is %d", ret));

    json_object_object_add(rsc_jso, rsc_prop_name, val_jso);

    return VOS_OK;
}


void rf_trim_https_uri(gchar *uri, guint32 uri_size)
{
    gint32 len;
    gchar *trim_uri = NULL;
    gint32 i;
    gint32 j;
    guint32 uri_len = strlen(uri);
    return_if_expr(uri_len >= uri_size);

    
    trim_uri = g_strstr_len(uri, strlen(HTTPS_PREFIX), HTTPS_PREFIX);
    trim_uri = (NULL != trim_uri) ? (trim_uri + strlen(HTTPS_PREFIX)) : uri;

    (void)g_strdelimit(trim_uri, "\\", '/');

    i = 0;
    j = 0;
    len = strlen(trim_uri);

    while (i < len) {
        
        if (!g_ascii_strncasecmp(&trim_uri[i], CUR_DIR_STR, strlen(CUR_DIR_STR))) {
            i += strlen(CUR_DIR_STR);
        } else {
            trim_uri[j++] = trim_uri[i];
            i++;
        }
    }

    
    j--;

    while ((j >= 0) && (' ' == trim_uri[j])) {
        j--;
    }

    
    while ((j >= 0) && ('/' == trim_uri[j])) {
        j--;
    }

    trim_uri[j + 1] = '\0';

    return;
}


void redfish_trim_uri(gchar *uri, guint32 uri_size)
{
    
    
    gchar *index_str = NULL;
    

    if (NULL == uri) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);

        return;
    }

    
    index_str = g_strrstr(uri, URI_JSON_POINTER_FLAG);
    do_val_if_expr(NULL != index_str, (*index_str = '\0'));

    return rf_trim_https_uri(uri, uri_size);
    
}


void rf_gen_json_object_etag(json_object *jso, gchar *etag, guint32 etag_len)
{
    errno_t safe_fun_ret;
    int iRet;
    gchar *check_sum = NULL;
    const gchar *jso_string = NULL;
    gchar etag_tmp[ETAG_LEN - 4] = {0};

    return_do_info_if_fail(NULL != etag, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    jso_string = dal_json_object_to_json_string(jso);
    return_do_info_if_fail(NULL != jso_string, debug_log(DLOG_ERROR, "%s failed:cann't get json string", __FUNCTION__));

    // 以资源对象字符串计算校验和sha256加密值
    check_sum = g_compute_checksum_for_string(G_CHECKSUM_SHA256, jso_string, strlen(jso_string));
    return_do_info_if_fail(NULL != check_sum, debug_log(DLOG_ERROR, "g_compute_checksum_for_string fail"));
    // 取check_sum前8个字节
    safe_fun_ret = strncpy_s(etag_tmp, ETAG_LEN - 4, check_sum, ETAG_LEN - 5);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    g_free(check_sum);

    return_do_info_if_expr(etag_len == 0,
        debug_log(DLOG_ERROR, "%s failed:input param error, max_len is 0.", __FUNCTION__));
    // 生成etag字符串
    iRet = snprintf_s(etag, etag_len, etag_len - 1, "W/\"%s\"", etag_tmp);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    return;
}


gint32 rf_get_sp_json_file(guint8 file_id, const gchar *file_name, json_object **obj)
{
    gint32 ret;
    const gchar *u_file_data = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gsize element_num = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar *file_char = NULL;

    if ((file_id >= UMS_TRANS_FILE_ID_CNT) || (file_name == NULL) || (obj == NULL)) {
        debug_log(DLOG_ERROR, "%s, %d:  Invalid parameter.\n", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle  fail", __FUNCTION__, __LINE__));

    
    input_list = g_slist_append(input_list, g_variant_new_byte(file_id));
    input_list = g_slist_append(input_list, g_variant_new_string(file_name));
    
    ret = uip_call_class_method_redfish(REDFISH_REQ_FROM_OTHER_CLIENT, "", "", obj_handle, SP_UMS_CLASS_NAME,
        UMS_METHOD_READ_FILE, 0, 0, input_list, &output_list);
    
    uip_free_gvariant_list(input_list);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s, %d:  ret : %d  fail", __FUNCTION__, __LINE__, ret));

    
    u_file_data = (const gchar *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(output_list, 0), &element_num,
        sizeof(gchar));
    return_val_do_info_if_expr(NULL == u_file_data, VOS_ERR, uip_free_gvariant_list(output_list);
        debug_log(DLOG_ERROR, "%s, %d:  file_data is null", __FUNCTION__, __LINE__));

    
    file_char = (gchar *)g_malloc0(element_num + 1);
    if (NULL == file_char) {
        debug_log(DLOG_ERROR, "%s, %d: g_malloc0 file_char fail", __FUNCTION__, __LINE__);
        uip_free_gvariant_list(output_list);
        return VOS_ERR;
    }
    if (EOK != memmove_s(file_char, element_num + 1, u_file_data, element_num)) {
        debug_log(DLOG_ERROR, "%s, %d: memmove_s fail", __FUNCTION__, __LINE__);
        uip_free_gvariant_list(output_list);
        g_free(file_char);
        return VOS_ERR;
    }
    
    *obj = json_tokener_parse((const gchar *)file_char);
    g_free(file_char);
    uip_free_gvariant_list(output_list);

    return VOS_OK;
}


gint32 rf_set_sp_json_file(guint8 file_id, const gchar *file_name, json_object *obj)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    const gchar *data = NULL;

    if ((file_id >= UMS_TRANS_FILE_ID_CNT) || (file_name == NULL) || (obj == NULL)) {
        debug_log(DLOG_ERROR, "%s, %d:  Invalid parameter.\n", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle  fail", __FUNCTION__, __LINE__));

    data = dal_json_object_get_str(obj);
    return_val_do_info_if_expr(NULL == data, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle  fail", __FUNCTION__, __LINE__));

    
    input_list = g_slist_append(input_list, g_variant_new_byte(file_id));
    input_list = g_slist_append(input_list, g_variant_new_string(file_name));
    input_list =
        g_slist_append(input_list, g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, data, strlen(data), sizeof(guchar)));

    
    ret = uip_call_class_method_redfish(REDFISH_REQ_FROM_OTHER_CLIENT, "", "", obj_handle, SP_UMS_CLASS_NAME,
        UMS_METHOD_WRITE_FILE, 0, 0, input_list, &output_list);
    

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    return ret;
}


LOCAL gint32 get_labels_data(gchar *line_buf, gint32 num_of_labels, gchar **labels, json_object **o_result_jso,
    READ_HISTORY_TYPE data_type)
{
    errno_t safe_fun_ret;
    gint32 i;
    gint32 ret = 0;
    gchar line[BUFF_LEN + 1] = {0};
    gchar **str_arr = NULL;
    gint32 data = 0;
    json_object *item_jso = NULL;

    
    return_val_do_info_if_fail((NULL != line_buf) && (0 < num_of_labels) && (NULL != labels) && (NULL != o_result_jso),
        VOS_ERR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    
    
    safe_fun_ret = strncpy_s(line, BUFF_LEN + 1, line_buf, BUFF_LEN);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    

    
    dal_trim_string(line, BUFF_LEN + 1);
    str_arr = g_strsplit(line, "#", num_of_labels + 1);
    return_val_do_info_if_fail(NULL != str_arr, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: split line fail, line is %s.", __FUNCTION__, line));
    goto_label_do_info_if_fail(num_of_labels + 1 == g_strv_length(str_arr), exit,
        debug_log(DLOG_ERROR, "%s: num_of_labels is %d, g_strv_length(str_arr) is %d.", __FUNCTION__, num_of_labels,
        g_strv_length(str_arr)));

    
    item_jso = json_object_new_object();
    goto_label_do_info_if_fail(NULL != item_jso, exit, debug_log(DLOG_ERROR, "%s: new item_jso fail.", __FUNCTION__));

    
    json_object_object_add(item_jso, "Time", json_object_new_string(str_arr[0]));

    
    for (i = 1; i <= num_of_labels; i++) {
        if (NULL == str_arr[i] || 0 == strlen(str_arr[i])) {
            
            data = 0;
        } else {
            
            ret = vos_str2int(str_arr[i], 10, &data, NUM_TPYE_INT32);
            goto_label_do_info_if_fail(VOS_OK == ret, exit,
                debug_log(DLOG_ERROR, "%s: error data, data is %s.", __FUNCTION__, str_arr[i]));
        }

        
        
        
        if (data_type == READ_HISTORY_DOUBLE) {
            json_object_object_add(item_jso, labels[i - 1], ex_json_object_new_double((((gdouble)data) / 100), "%.2f"));
        } else {
            json_object_object_add(item_jso, labels[i - 1], json_object_new_int(data));
        }
        
    }

    g_strfreev(str_arr);
    *o_result_jso = item_jso;

    return VOS_OK;

exit:
    g_strfreev(str_arr);
    do_if_expr(NULL != item_jso, json_object_put(item_jso));

    return VOS_ERR;
}


gint32 read_history_data_from_file(gchar *file_path, gint32 num_of_labels, gchar **labels, json_object **o_result_jso,
    READ_HISTORY_TYPE data_type)
{
    gint32 ret = 0;
    FILE *fp = NULL;
    gchar line_buf[BUFF_LEN + 1] = {0};
    json_object *item_jso = NULL;
    json_object *data_array_jso = NULL;
    gchar *resolved_path = NULL;

    
    return_val_do_info_if_fail((NULL != file_path) && (0 < num_of_labels) && (NULL != labels) && (NULL != o_result_jso),
        VOS_ERR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    resolved_path = realpath(file_path, NULL);
    return_val_do_info_if_fail(NULL != resolved_path, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s: realpath get resolved path fail", __FUNCTION__));

    
    fp = fopen(resolved_path, "r");
    goto_label_do_info_if_fail(NULL != fp, exit,
        debug_log(DLOG_ERROR, "%s: open file fail.", __FUNCTION__));

    
    do_info_if_true(NULL != resolved_path, g_free(resolved_path));
    resolved_path = NULL;

    
    goto_label_do_info_if_fail(NULL != fgets(line_buf, BUFF_LEN, fp), exit,
        debug_log(DLOG_ERROR, "%s: empty file.", __FUNCTION__));

    
    data_array_jso = json_object_new_array();
    goto_label_do_info_if_fail(NULL != data_array_jso, exit,
        debug_log(DLOG_ERROR, "%s: new json array fail.", __FUNCTION__));

    
    while (NULL != fgets(line_buf, BUFF_LEN, fp)) {
        
        ret = get_labels_data(line_buf, num_of_labels, labels, &item_jso, data_type);
        continue_do_info_if_fail((VOS_OK == ret) && (NULL != item_jso),
            debug_log(DLOG_ERROR, "%s: get data fail, line is %s.", __FUNCTION__, line_buf));

        json_object_array_add(data_array_jso, item_jso);
        item_jso = NULL;
        (void)memset_s(line_buf, sizeof(line_buf), 0, sizeof(line_buf));
    }

    do_info_if_true(NULL != fp, (void)fclose(fp); fp = NULL);
    *o_result_jso = data_array_jso;

    return VOS_OK;

exit:
    do_info_if_true(NULL != fp, (void)fclose(fp); fp = NULL);
    do_info_if_true(NULL != data_array_jso, json_object_put(data_array_jso));
    do_info_if_true(NULL != resolved_path, g_free(resolved_path));

    return VOS_ERR;
}


gint32 get_universal_provider_odata_info(void *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    const gchar *uri_template)
{
    int iRet;
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    PROVIDER_PARAS_S *tmp_paras = NULL;

    tmp_paras = (PROVIDER_PARAS_S *)i_paras;

    if (NULL == o_result_jso || o_message_jso == NULL || uri_template == NULL ||
        VOS_OK != provider_paras_check(tmp_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(tmp_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));

    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, uri_template, slot);
#pragma GCC diagnostic pop
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


gint32 get_file_lines(const gchar *log_path, gint32 *count)
{
    gchar *real_path = NULL;
    FILE *pf = NULL;
    gint32 log_count = 0;
    gchar tmp_buf[MAX_BUFF_SIZE] = {0};

    return_val_do_info_if_expr(log_path == NULL || count == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:Parameter invalid", __FUNCTION__));

    real_path = realpath(log_path, NULL);
    return_val_do_info_if_expr(NULL == real_path, VOS_ERR, debug_log(DLOG_ERROR, "%s:Parameter invalid", __FUNCTION__));

    return_val_do_info_if_expr(TRUE != vos_get_file_accessible(real_path), VOS_ERR, g_free(real_path); real_path = NULL;
        debug_log(DLOG_ERROR, "%s:Parameter invalid", __FUNCTION__));

    pf = fopen(real_path, "r");
    return_val_do_info_if_expr(pf == NULL, VOS_ERR, g_free(real_path); real_path = NULL;
        debug_log(DLOG_ERROR, "%s:Open file failed", __FUNCTION__));

    while (fgets(tmp_buf, MAX_BUFF_SIZE, pf)) {
        if (!(strlen(tmp_buf) == 1 && strcmp(tmp_buf, "\n") == 0)) {
            log_count++;
        }
    }

    g_free(real_path);
    real_path = NULL;
    (void)fclose(pf);
    pf = NULL;

    *count = log_count;
    return VOS_OK;
}


gint32 find_fullfilename_from_directory(const gchar *directory_path, const gchar *prefix_file_name,
    gchar *full_file_name, gsize length)
{
    gint32 ret = VOS_ERR;
    DIR *dir = NULL;
    struct dirent *ptr = NULL;

    if (directory_path == NULL || prefix_file_name == NULL || full_file_name == NULL || length <= 0) {
        debug_log(DLOG_ERROR, "%s: Parameter invalid.", __FUNCTION__);
        return VOS_ERR;
    }

    if ((dir = opendir(directory_path)) == NULL) {
        debug_log(DLOG_ERROR, "%s: Open directory failed.", __FUNCTION__);
        return VOS_ERR;
    }

    while ((ptr = readdir(dir)) != NULL) {
        if (ptr->d_type == 8 && strstr(ptr->d_name, prefix_file_name) != NULL) {
            ret = VOS_OK;
            (void)strncpy_s(full_file_name, length, ptr->d_name, length - 1);
            break;
        }
    }

    closedir(dir);
    return ret;
}


LOCAL gint32 get_file_transfer_status(TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;
    gint32 file_down_progress = 0;

    return_val_if_expr((monitor_fn_data == NULL) || (message_obj == NULL), RF_FAILED);

    // 获取文件上传的状态
    ret = get_file_transfer_progress(&file_down_progress);
    return_val_do_info_if_expr(VOS_OK != ret, RF_FAILED, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        monitor_fn_data->task_progress = TASK_NO_PROGRESS;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
        debug_log(DLOG_ERROR, "get file packege download progress failed"));

    
    ret = parse_file_transfer_err_code(file_down_progress, message_obj);
    return_val_do_info_if_expr(RF_OK != ret, RF_FAILED, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        monitor_fn_data->task_progress = TASK_NO_PROGRESS;
        debug_log(DLOG_ERROR, "file transfer error occured, error code:%d", file_down_progress));

    
    if (RF_FINISH_PERCENTAGE != file_down_progress) {
        monitor_fn_data->task_progress = 10 + (file_down_progress * 90) / 100;
    } else {
        monitor_fn_data->task_state = RF_TASK_COMPLETED;
        monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
        (void)create_message_info(MSGID_SUCCESS, NULL, message_obj);
    }

    return RF_OK;
}


LOCAL gint32 upload_file(EXPORT_TRANSACTION_INFO_S *export_info, json_object **o_message_jso)
{
    int iRet;
    GSList *input_list = NULL;
    OBJ_HANDLE file_transfer_handle = 0;
    gint32 ret;
    gchar          file_transfer_uri[MAX_URI_PATH_LENGTH] = {0};

    return_val_if_expr((export_info == NULL) || (o_message_jso == NULL), VOS_ERR);

    iRet = snprintf_s(file_transfer_uri, sizeof(file_transfer_uri), sizeof(file_transfer_uri) - 1, "upload;%d;%s",
        export_info->file_id, export_info->content);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    (void)dal_get_object_handle_nth(CLASS_NAME_TRANSFERFILE, 0, &file_transfer_handle);

    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)file_transfer_uri));
    (void)memset_s(file_transfer_uri, MAX_URI_PATH_LENGTH, 0, MAX_URI_PATH_LENGTH);
    input_list = g_slist_append(input_list, g_variant_new_string(export_info->session_id));

    
    ret = uip_call_class_method_redfish(export_info->from_webui_flag, export_info->user_name, export_info->client,
        file_transfer_handle, CLASS_NAME_TRANSFERFILE, METHOD_INITRIAL_FILE_TRANSFER, AUTH_DISABLE, AUTH_PRIV_NONE,
        input_list, NULL);
    
    uip_free_gvariant_list(input_list);

    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR, (void)parse_file_transfer_err_code(ret, o_message_jso);
        debug_log(DLOG_ERROR, "cann't use methos %s", METHOD_INITRIAL_FILE_TRANSFER));
    return ret;
}


void record_method_operation_log(guchar from_webui_flag, gboolean export_success, gchar *user_name, gchar *client,
    gchar *file_category)
{
    GSList *caller_info = NULL;

    
    caller_info = g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(from_webui_flag)));
    
    caller_info = g_slist_append(caller_info, g_variant_new_string(user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(client));
    if (TRUE == export_success) {
        (void)proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Export %s to local successfully",
            file_category);
    } else {
        (void)proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Export %s failed", file_category);
    }

    (void)uip_free_gvariant_list(caller_info);
}

LOCAL void export_file_transaction_info_to_local(EXPORT_TRANSACTION_INFO_S *export_info,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    if (g_strcmp0(export_info->content, export_info->src) != 0) {
        // redfish模块降权限后，调用代理方法拷贝文件
        (void)proxy_copy_file(export_info->from_webui_flag, export_info->user_name, export_info->client,
            export_info->src, export_info->content, APACHE_UID, APPS_USER_GID, export_info->need_del);
    }
    monitor_fn_data->task_state = RF_TASK_COMPLETED;
    monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
    (void)create_message_info(MSGID_SUCCESS, NULL, message_obj);
    (void)record_method_operation_log(export_info->from_webui_flag, TRUE, export_info->user_name, export_info->client,
        export_info->file_category);
}


LOCAL gint32 export_status_monitor_util(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret = 0;
    guint8 progress = 0;
    EXPORT_TRANSACTION_INFO_S *export_info = NULL;

    
    monitor_fn_data->task_state = RF_TASK_RUNNING;
    export_info = (EXPORT_TRANSACTION_INFO_S *)monitor_fn_data->user_data;
    if (export_info->state == 0) { // 导出阶段, 0代表生成文件阶段，1代表传输文件阶段
        // 获取文件生成进度，完成则触发导出
        if (export_info->get_status) {
            ret = export_info->get_status(message_obj, &progress);
            goto_label_do_info_if_expr(ret != VOS_OK, __error,
                (void)record_method_operation_log(export_info->from_webui_flag, FALSE, export_info->user_name,
                export_info->client, export_info->file_category));
        } else {
            progress = RF_FINISH_PERCENTAGE;
        }

        monitor_fn_data->task_progress = progress * 10 / 100;
        if (progress == RF_FINISH_PERCENTAGE) {
            export_info->state = 1;
            if (export_info->type == 0) {
                // 导出本地
                export_file_transaction_info_to_local(export_info, monitor_fn_data, message_obj);
                if (monitor_fn_data->task_state == RF_TASK_COMPLETED) {
                    ret = dal_change_file_owner(export_info->auth_type, export_info->content, export_info->user_name,
                        S_IRUSR | S_IWUSR);
                    goto_if_expr(ret != VOS_OK, __error);
                }
            } else {
                // 导出远端
                ret = upload_file(export_info, message_obj);
                goto_if_expr(ret != VOS_OK, __error);
            }
        }
    } else {
        get_file_transfer_status(monitor_fn_data, message_obj);
    }
    

    if ((monitor_fn_data->task_state == RF_TASK_COMPLETED || monitor_fn_data->task_state == RF_TASK_EXCEPTION) &&
        export_info->need_del && strlen(export_info->src) > 0) {
        if (g_strcmp0(export_info->content, export_info->src) != 0) {
            // redfish模块降权限后，调用代理方法删除文件
            (void)proxy_delete_file(export_info->from_webui_flag, export_info->user_name, export_info->client,
                export_info->src);
        }
    }

    return RF_OK;

__error:
    if (export_info->need_del && strlen(export_info->src) > 0) {
        // redfish模块降权限后，调用代理方法删除文件
        (void)proxy_delete_file(export_info->from_webui_flag, export_info->user_name, export_info->client,
            export_info->src);
    }
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    monitor_fn_data->task_progress = TASK_NO_PROGRESS;
    return RF_OK;
}


LOCAL void fill_export_action_args(const char* origin_obj_path, json_object* body_jso,
    EXPORT_TRANSACTION_INFO_S *export_info, EXPORT_ACTION_ARGS *action_args)
{
    action_args->is_from_webui = export_info->from_webui_flag;
    action_args->param_jso = body_jso;
    action_args->user_role_privilege = export_info->permission;
    (void)strncpy_s(action_args->user_name, sizeof(action_args->user_name), export_info->user_name,
        sizeof(action_args->user_name) - 1);
    (void)strncpy_s(action_args->client, sizeof(action_args->client), export_info->client,
        sizeof(action_args->client) - 1);
    (void)strncpy_s(action_args->uri, sizeof(action_args->uri), origin_obj_path, sizeof(action_args->uri) - 1);
}


static gint32 export_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;
    EXPORT_ACTION_ARGS action_args = {0};
    EXPORT_TRANSACTION_INFO_S *export_info = NULL;

    if (origin_obj_path == NULL || monitor_fn_data == NULL || monitor_fn_data->user_data == NULL ||
        message_obj == NULL || body_jso == NULL) {
        debug_log(DLOG_INFO, "%s: input param is invalid.", __FUNCTION__);
        return RF_FAILED;
    }

    export_info = (EXPORT_TRANSACTION_INFO_S *)monitor_fn_data->user_data;

    
    if (export_info->generate_file != NULL && monitor_fn_data->task_progress == TASK_NO_PROGRESS &&
        monitor_fn_data->task_state == RF_TASK_NEW) {
        fill_export_action_args(origin_obj_path, body_jso, export_info, &action_args);
        ret = export_info->generate_file(&action_args, message_obj);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: generate_file failed, ret = %d.", __FUNCTION__, ret);
            
            json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            (void)record_method_operation_log(export_info->from_webui_flag, FALSE, export_info->user_name,
                export_info->client, export_info->file_category);
            return RF_OK;
        }
    }

    ret = export_status_monitor_util(origin_obj_path, body_jso, monitor_fn_data, message_obj);

    
    if (monitor_fn_data->task_state == RF_TASK_EXCEPTION || monitor_fn_data->task_state == RF_TASK_COMPLETED ||
        monitor_fn_data->task_state == RF_TASK_KILLED) {
        json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
    }

    return ret;
}


void get_dir_from_path(const gchar *path, gchar *dir, guint32 dir_length)
{
    gchar *file_name = NULL;
    errno_t safe_fun_ret;

    if (path == NULL || dir == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error\n", __FUNCTION__);
        return;
    }

    file_name = g_strrstr(path, "/");
    if (file_name != NULL) {
        safe_fun_ret = strncpy_s(dir, dir_length, (const gchar *)path, (file_name - path));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    }
}


gboolean is_valid_file_transfer_protocol(const gchar *uri)
{
    if (uri == NULL) {
        return FALSE;
    }

    if ((g_str_has_prefix(uri, FILE_URL_HTTPS_HEAD) == TRUE) || (g_str_has_prefix(uri, FILE_URL_SFTP_HEAD) == TRUE) ||
        (g_str_has_prefix(uri, FILE_URL_CIFS_HEAD) == TRUE) || (g_str_has_prefix(uri, FILE_URL_SCP_HEAD) == TRUE) ||
        (g_str_has_prefix(uri, FILE_URL_NFS_HEAD) == TRUE)) {
        return TRUE;
    }

    return FALSE;
}


LOCAL void execut_export_transaction_free_func(void *user_data)
{
    EXPORT_TRANSACTION_INFO_S *tmp_data = (EXPORT_TRANSACTION_INFO_S *)user_data;

    if (tmp_data->content != NULL) {
        memset_s(tmp_data->content, sizeof(tmp_data->content), 0, sizeof(tmp_data->content));
    }

    g_free(user_data);
    return;
}


LOCAL gint32 execut_export_transaction(void *paras, json_object **o_message_jso, json_object **o_result_jso,
    EXPORT_TRANSACTION_INFO_S *export_info)
{
    gint32 ret;
    json_bool ret_bool;
    const gchar *value_str = NULL;
    json_object *value_jso = NULL;
    TASK_MONITOR_INFO_S *monitor_info = NULL;
    PROVIDER_PARAS_S *i_paras = (PROVIDER_PARAS_S *)paras;

    
    export_info->from_webui_flag = i_paras->is_from_webui;
    

    export_info->auth_type = i_paras->auth_type;

    // 初始化导出参数
    export_info->state = 0;
    
    (void)strncpy_s(export_info->client, sizeof(export_info->client), i_paras->client, sizeof(export_info->client) - 1);
    (void)strncpy_s(export_info->session_id, sizeof(export_info->session_id), i_paras->session_id,
        sizeof(export_info->session_id) - 1);
    (void)strncpy_s(export_info->user_name, sizeof(export_info->user_name), i_paras->user_name,
        sizeof(export_info->user_name) - 1);
    
    ret_bool = json_object_object_get_ex(i_paras->val_jso, RFACTION_PARAM_CONTENT, &value_jso);
    return_val_do_info_if_fail(ret_bool && (NULL != value_jso), HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s, %d: get content value fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, NULL, RFACTION_PARAM_CONTENT);
        (void)record_method_operation_log(i_paras->is_from_webui, FALSE, export_info->user_name, export_info->client,
        export_info->file_category));
    value_str = dal_json_object_get_str(value_jso);
    (void)strncpy_s(export_info->content, MAX_URI_LENGTH, value_str, MAX_URI_LENGTH - 1);

    // 导出URI 通用性校验
    export_info->type = 1;
    if (export_info->content[0] == '/') {
        if (strlen(export_info->content) > MAX_FILEPATH_LENGTH - 1) {
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, NULL, o_message_jso, RF_SENSITIVE_INFO,
                RFACTION_PARAM_CONTENT);
            (void)record_method_operation_log(i_paras->is_from_webui, FALSE, export_info->user_name,
                export_info->client, export_info->file_category);
            (void)memset_s(export_info->content, sizeof(export_info->content), 0, sizeof(export_info->content));
            return HTTP_BAD_REQUEST;
        }

        if (dal_check_real_path2(export_info->content, TMP_PATH_WITH_SLASH) != VOS_OK) {
            (void)create_message_info(MSGID_INVALID_PATH, NULL, o_message_jso, RF_SENSITIVE_INFO,
                RFACTION_PARAM_CONTENT);
            (void)record_method_operation_log(i_paras->is_from_webui, FALSE, export_info->user_name,
                export_info->client, export_info->file_category);
            (void)memset_s(export_info->content, sizeof(export_info->content), 0, sizeof(export_info->content));
            return HTTP_BAD_REQUEST;
        }

        if (g_file_test((const gchar *)export_info->content, G_FILE_TEST_IS_DIR) == TRUE) {
            (void)create_message_info(MSGID_NOT_ALLOW_OVERWRITE_DIR, NULL, o_message_jso);
            (void)record_method_operation_log(i_paras->is_from_webui, FALSE, export_info->user_name,
                export_info->client, export_info->file_category);
            return HTTP_BAD_REQUEST;
        }

        export_info->type = 0;
    } else if (is_valid_file_transfer_protocol((const gchar *)export_info->content) == FALSE) {
        (void)create_message_info(MSGID_FILE_TRANSFER_PROTOCOL_MISMATCH, RFACTION_PARAM_CONTENT, o_message_jso);
        (void)record_method_operation_log(i_paras->is_from_webui, FALSE, export_info->user_name, export_info->client,
            export_info->file_category);
        (void)memset_s(export_info->content, sizeof(export_info->content), 0, sizeof(export_info->content));
        return HTTP_BAD_REQUEST;
    }

    // 任务重复性检查
    (void)pthread_mutex_lock(&g_export_task_mutex);
    ret = check_export_task_exist(export_status_monitor, i_paras->val_jso, (const gchar*)i_paras->uri, NULL);
    if (ret != FALSE) {
        (void)memset_s(export_info->content, sizeof(export_info->content), 0, sizeof(export_info->content));
        (void)create_message_info(MSGID_TASK_DUPLICATE_EXPORT_ERR, NULL, o_message_jso);
        (void)record_method_operation_log(i_paras->is_from_webui, FALSE, export_info->user_name, export_info->client,
            export_info->file_category);
        (void)pthread_mutex_unlock(&g_export_task_mutex);
        return HTTP_BAD_REQUEST;
    }

    // 创建TASK 来监控文件生成和传输的进度
    monitor_info = task_monitor_info_new((GDestroyNotify)execut_export_transaction_free_func);
    goto_label_do_info_if_expr((monitor_info == NULL), err_exit,
        (void)pthread_mutex_unlock(&g_export_task_mutex); debug_log(DLOG_ERROR, "task_monitor_info_new failed"));

    monitor_info->user_data = g_malloc0(sizeof(EXPORT_TRANSACTION_INFO_S));
    goto_label_do_info_if_expr((monitor_info->user_data == NULL), err_exit,
        (void)pthread_mutex_unlock(&g_export_task_mutex); debug_log(DLOG_ERROR, "malloc export info failed"));
    (void)memcpy_s(monitor_info->user_data, sizeof(EXPORT_TRANSACTION_INFO_S), export_info,
        sizeof(EXPORT_TRANSACTION_INFO_S));
    monitor_info->task_progress = TASK_NO_PROGRESS;
    monitor_info->rsc_privilege = export_info->permission;
    ret = create_new_task(NULL, export_status_monitor, monitor_info, i_paras->val_jso, i_paras->uri, o_result_jso);
    (void)pthread_mutex_unlock(&g_export_task_mutex);
    goto_label_do_info_if_fail(ret == RF_OK, err_exit, debug_log(DLOG_ERROR, "create new task failed"));
    (void)memset_s(export_info->content, sizeof(export_info->content), 0, sizeof(export_info->content));
    return HTTP_ACCEPTED;

err_exit:

    if (monitor_info != NULL) {
        task_monitor_info_free(monitor_info);
    }

    if (export_info->content != NULL) {
        (void)memset_s(export_info->content, sizeof(export_info->content), 0, sizeof(export_info->content));
    }

    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    (void)record_method_operation_log(i_paras->is_from_webui, FALSE, export_info->user_name, export_info->client,
        export_info->file_category);

    return HTTP_INTERNAL_SERVER_ERROR;
}


gint32 create_export_transaction(void *paras, json_object **o_message_jso, json_object **o_result_jso,
    EXPORT_TRANSACTION_INFO_S *export_info)
{
    gint32 ret;
    PROVIDER_PARAS_S *i_paras = (PROVIDER_PARAS_S *)paras;

    if (i_paras == NULL || i_paras->val_jso == NULL) {
        debug_log(DLOG_INFO, "%s: input param is invalid.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (o_message_jso == NULL || o_result_jso == NULL || export_info == NULL) {
        debug_log(DLOG_INFO, "%s: input param is invalid.", __FUNCTION__);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 权限校验
    if ((i_paras->user_role_privilege & export_info->permission) == 0) {
        debug_log(DLOG_INFO, "%s: insufficient privilege.", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        (void)record_method_operation_log(i_paras->is_from_webui, FALSE, i_paras->user_name, i_paras->client,
            export_info->file_category);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        return HTTP_FORBIDDEN;
    }

    ret = execut_export_transaction(i_paras, o_message_jso, o_result_jso, export_info);
    if (ret != HTTP_ACCEPTED) {
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    }
    return ret;
}


void record_update_schema_method_operation_log(guchar from_webui_flag, const gchar *user_name, const gchar *client,
    gboolean update_success)
{
    GSList *caller_info = NULL;

    caller_info = g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(from_webui_flag)));
    caller_info = g_slist_append(caller_info, g_variant_new_string(user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(client));
    if (update_success == TRUE) {
        (void)proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Update sp schema file successfully");
    } else {
        (void)proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Update sp schema file failed");
    }
    (void)uip_free_gvariant_list(caller_info);
    return;
}


gint32 shelf_managers_systems_sort_by_odataid(const void *j1, const void *j2)
{
    json_object *jso1 = NULL;
    json_object *jso2 = NULL;

    const gchar *str1 = NULL;
    const gchar *str2 = NULL;

    return_val_if_expr(!j1 || !j2, 0);

    jso1 = *((json_object * const *)j1);
    jso2 = *((json_object * const *)j2);

    
    if (!jso1 || !jso2) {
        return jso1 ? 1 : (jso2 ? -1 : 0);
    }

    jso1 = json_object_object_get(jso1, RFPROP_ODATA_ID);
    jso2 = json_object_object_get(jso2, RFPROP_ODATA_ID);
    if (!jso1 || !jso2) {
        return jso1 ? 1 : (jso2 ? -1 : 0);
    }

    str1 = dal_json_object_get_str(jso1);
    str2 = dal_json_object_get_str(jso2);
    if (!str1 || !str2) {
        return str1 ? 1 : (str2 ? -1 : 0);
    }

    return strcmp(str1, str2);
}


void add_val_str_to_rsp_body(const gchar *key, const gchar *val, gboolean is_last_prop, gchar *buffer,
    guint32 buffer_len)
{
    gint32 ret;
    gchar *tmp_str = NULL;
    guint32 tmp_str_len;

    if (key == NULL || val == NULL || buffer == NULL) {
        return;
    }

    if (strlen(key) > (G_MAXINT32 - APPEND_BUFFER_LEN - strlen(val))) {
        debug_log(DLOG_ERROR, "%s val string is too long.", __FUNCTION__);
        return;
    }

    
    tmp_str_len = strlen(key) + strlen(val) + APPEND_BUFFER_LEN;
    tmp_str = (gchar *)g_malloc0(tmp_str_len);
    if (tmp_str == NULL) {
        debug_log(DLOG_ERROR, "%s: call g_malloc0 failed", __FUNCTION__);
        return;
    }

    if (strlen(val) == 0 || g_strcmp0(val, INVALID_DATA_STRING) == 0 ||
        g_strcmp0(val, INVALID_DATA_STRING_VALUE) == 0) {
        
        ret = snprintf_s(tmp_str, tmp_str_len, tmp_str_len - 1, "\"%s\":null%s", key, (is_last_prop ? "" : ","));
    } else {
        ret = snprintf_s(tmp_str, tmp_str_len, tmp_str_len - 1, "\"%s\":\"%s\"%s", key, val, (is_last_prop ? "" : ","));
    }

    if (ret <= 0) {
        g_free(tmp_str);
        debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret:%d", __FUNCTION__, ret);
        return;
    }

    ret = strcat_s(buffer, buffer_len, tmp_str);
    g_free(tmp_str);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strcat_s failed, ret:%d", __FUNCTION__, ret);
    }

    return;
}


gint32 set_rsp_body_content(const gchar *content, gchar **o_rsp_body_str)
{
    gint32 ret;
    guint32 buffer_len;
    const guint32 append_str_len = 3;

    if (o_rsp_body_str == NULL || content == NULL) {
        return VOS_ERR;
    }

    buffer_len = strlen(content) + append_str_len;
    *o_rsp_body_str = (gchar *)g_malloc0(buffer_len);
    if (*o_rsp_body_str == NULL) {
        debug_log(DLOG_ERROR, "%s: call g_malloc0 failed", __FUNCTION__);
        return VOS_ERR;
    }

    ret = snprintf_s(*o_rsp_body_str, buffer_len, buffer_len - 1, "{%s}", content);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret:%d", __FUNCTION__, ret);
        g_free(*o_rsp_body_str);
        *o_rsp_body_str = NULL;
        return VOS_ERR;
    }

    return VOS_OK;
}


void delete_user_specific_file(PROVIDER_PARAS_S *i_paras, json_object *val_jso, gchar* prop_name)
{   
    const gchar* file_name = NULL;

    (void)get_element_str(val_jso, (const gchar*)prop_name, &file_name);
    if (file_name == NULL || file_name[0] != '/') {
        return;
    }

    if (dal_check_real_path(file_name) != RET_OK) {
        return;
    }

    if (g_file_test(file_name, G_FILE_TEST_IS_DIR) == TRUE) {
        return;
    }

    if (dal_check_file_opt_user(i_paras->auth_type, (const gchar *)i_paras->user_name, file_name, 
        (const gchar *)i_paras->user_roleid) != TRUE) {
        return;
    }

    (void)proxy_delete_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client, file_name);
}

gint32 rf_get_property_value_string_as_jso(OBJ_HANDLE obj_handle, const gchar *prop, json_object **jso)
{
    if (jso == NULL || prop == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid parameter!", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    gchar val_str[MAX_STRBUF_LEN] = {0};
    gint32 ret = dal_get_property_value_string(obj_handle, prop, val_str, MAX_STRBUF_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get property %s of %s fail! ret = %d", __FUNCTION__, prop,
            dfl_get_object_name(obj_handle), ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *jso = json_object_new_string(val_str);
    return (*jso == NULL) ? HTTP_INTERNAL_SERVER_ERROR : HTTP_OK;
}

gint32 rf_get_property_value_int32_as_jso(OBJ_HANDLE obj_handle, const gchar *prop, json_object **jso)
{
    if (jso == NULL || prop == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid parameter!", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    gint32 val = 0;
    gint32 ret = dal_get_property_value_int32(obj_handle, prop, &val);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get property %s of %s fail! ret = %d", __FUNCTION__, prop,
            dfl_get_object_name(obj_handle), ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *jso = json_object_new_int(val);
    return (*jso == NULL) ? HTTP_INTERNAL_SERVER_ERROR : HTTP_OK;
}

gint32 rf_get_property_value_int16_as_jso(OBJ_HANDLE obj_handle, const gchar *prop, json_object **jso)
{
    if (jso == NULL || prop == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid parameter!", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    gint16 val = 0;
    gint32 ret = dal_get_property_value_int16(obj_handle, prop, &val);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get property %s of %s fail! ret = %d", __FUNCTION__, prop,
            dfl_get_object_name(obj_handle), ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *jso = json_object_new_int(val);
    return (*jso == NULL) ? HTTP_INTERNAL_SERVER_ERROR : HTTP_OK;
}

gint32 rf_get_property_value_uint32_as_jso(OBJ_HANDLE obj_handle, const gchar *prop, json_object **jso)
{
    if (jso == NULL || prop == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid parameter!", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    guint32 val = 0;
    gint32 ret = dal_get_property_value_uint32(obj_handle, prop, &val);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get property %s of %s fail! ret = %d", __FUNCTION__, prop,
            dfl_get_object_name(obj_handle), ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *jso = json_object_new_int(val);
    return (*jso == NULL) ? HTTP_INTERNAL_SERVER_ERROR : HTTP_OK;
}

gint32 rf_get_property_value_uint16_as_jso(OBJ_HANDLE obj_handle, const gchar *prop, json_object **jso)
{
    if (jso == NULL || prop == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid parameter!", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    guint16 val = 0;
    gint32 ret = dal_get_property_value_uint16(obj_handle, prop, &val);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get property %s of %s fail! ret = %d", __FUNCTION__, prop,
            dfl_get_object_name(obj_handle), ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *jso = json_object_new_int(val);
    return (*jso == NULL) ? HTTP_INTERNAL_SERVER_ERROR : HTTP_OK;
}

gint32 rf_get_property_value_uint8_as_jso(OBJ_HANDLE obj_handle, const gchar *prop, json_object **jso)
{
    if (jso == NULL || prop == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid parameter!", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    guchar val = 0;
    gint32 ret = dal_get_property_value_byte(obj_handle, prop, &val);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get property %s of %s fail! ret = %d", __FUNCTION__, prop,
            dfl_get_object_name(obj_handle), ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *jso = json_object_new_int(val);
    return (*jso == NULL) ? HTTP_INTERNAL_SERVER_ERROR : HTTP_OK;
}