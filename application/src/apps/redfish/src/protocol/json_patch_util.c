

#include "json_patch_util.h"
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "json.h"
#include "json_checker.h"
#include "json/json_object_private.h"
#include "redfish_util.h"

typedef struct json_object_object JSON_OBJECT_T;
typedef struct json_object_string JSON_STRING_T;


void json_object_set_user_data(struct json_object *jso, void *user_data, json_object_delete_fn *user_delete)
{
    check_fail_return(jso && user_data && user_delete);

    if (jso->_user_delete) {
        jso->_user_delete(jso, jso->_userdata);
    }

    jso->_userdata = user_data;
    jso->_user_delete = user_delete;
}


json_bool json_object_get_user_data(struct json_object *jso, void **user_data)
{
    check_success_do(user_data, (*user_data = NULL));

    check_fail_return_val(jso && user_data, FALSE);

    *user_data = jso->_userdata;
    return TRUE;
}


json_bool get_element_int(json_object *jso, const char *element_name, gint32 *int_value)
{
    check_success_do(int_value, (*int_value = 0));
    check_fail_do_return_val(jso && element_name && int_value, FALSE, debug_log(DLOG_ERROR, "Invalid Param!"));

    json_object *int_jso = NULL;
    json_bool b_ret = json_object_object_get_ex(jso, element_name, &int_jso);
    if ((!b_ret) || (json_type_int != json_object_get_type(int_jso))) {
        debug_log(DLOG_ERROR, "Missing %s in jso or property is not of int type!", element_name);

        return FALSE;
    }

    *int_value = json_object_get_int(int_jso);
    return TRUE;
}


json_bool get_element_boolean(json_object *jso, const char *element_name, json_bool *bool_value)
{
    check_success_do(bool_value, (*bool_value = 0));
    check_fail_do_return_val(jso && element_name && bool_value, FALSE, debug_log(DLOG_ERROR, "Invalid Param!"));

    json_object *bool_jso = NULL;
    json_bool b_ret = json_object_object_get_ex(jso, element_name, &bool_jso);
    if (FALSE == b_ret) {
        debug_log(DLOG_DEBUG, "Missing %s in jso.", element_name);
        return FALSE;
    }

    if (json_type_boolean != json_object_get_type(bool_jso)) {
        debug_log(DLOG_ERROR, "%s property is not of boolean type!", element_name);
        return FALSE;
    }

    *bool_value = json_object_get_boolean(bool_jso);
    return TRUE;
}


json_bool is_json_object_array_element_exist(json_object *array_jso, json_object *element_jso)
{
    check_fail_return_val(json_type_array == json_object_get_type(array_jso), FALSE);

    int i;
    
    int len = json_object_array_length(array_jso);

    for (i = 0; i < len; i++) {
        json_object *iter_jso = json_object_array_get_idx(array_jso, i);

        if ((json_object_get_type(iter_jso) == json_object_get_type(element_jso)) &&
            (0 == g_strcmp0(dal_json_object_get_str(iter_jso), dal_json_object_get_str(element_jso)))) {
            return TRUE;
        }
    }
    
    return FALSE;
}


int json_object_array_element_occur_number(json_object *jso, json_object *element_jso)
{
    
    check_fail_return_val(json_type_array == json_object_get_type(jso), FALSE);
    

    int number = 0;
    int i;
    
    int len = json_object_array_length(jso);

    for (i = 0; i < len; i++) {
        json_object *iter_jso = json_object_array_get_idx(jso, i);
        if ((json_object_get_type(iter_jso) == json_object_get_type(element_jso)) &&
            (0 == strcmp(dal_json_object_get_str(iter_jso), dal_json_object_get_str(element_jso)))) {
            number++;
        }
    }
    
    return number;
}


json_bool json_object_array_delete_idx(json_object *jso, int idx)
{
    
    check_fail_return_val(jso && json_type_array == json_object_get_type(jso), FALSE);
    

    int array_len = json_object_array_length(jso);
    if ((array_len <= 0) || (idx < 0) || (idx >= array_len)) {
        return FALSE;
    }

    struct array_list *arr = json_object_get_array(jso);

    
    check_fail_return_val(arr, FALSE);
    

    arr->free_fn(arr->array[idx]);

    int i;
    for (i = idx; i < (array_len - 1); i++) {
        arr->array[i] = arr->array[i + 1];
    }

    arr->length--;

    return TRUE;
}


void json_object_array_clean(json_object *jso)
{
    struct array_list *arr = NULL;
    int i;
    json_type jso_type;

    jso_type = json_object_get_type(jso);
    if (json_type_array != jso_type) {
        debug_log(DLOG_ERROR, "%s:jso is not of array type,real type is %s.", __FUNCTION__,
            json_type_to_name(jso_type));
        return;
    }

    arr = json_object_get_array(jso);
    if (NULL == arr) {
        debug_log(DLOG_ERROR, "%s:array object is null.", __FUNCTION__);
        return;
    }

    for (i = 0; i < arr->length; i++) {
        if (arr->array[i]) {
            arr->free_fn(arr->array[i]);
            arr->array[i] = NULL;
        }
    }

    free(arr->array);
    
    arr->array = NULL;

    arr->size = 0;
    arr->length = 0;
    

    return;
}


json_bool get_property_obj_from_segments(json_object *obj, char *segment_info[], int segment_num,
    json_object **property_jso)
{
    check_fail_return_val(obj && segment_info && property_jso, FALSE);
    *property_jso = NULL;

    json_object *cur_jso, *jso_found;
    jso_found = cur_jso = obj;

    int i;
    int idx;
    gint32 value;

    for (i = 0; i < segment_num; i++) {
        json_type o_type = json_object_get_type(cur_jso);

        switch (o_type) {
            case json_type_array: {
                value = -1;
                if (dal_strtoi(segment_info[i], &value, DECIMAL_NUM) != RET_OK) {
                    debug_log(DLOG_ERROR, "%s:dal_strtoi failed", __FUNCTION__);
                    return FALSE;
                }
                idx = (int)value;
                jso_found = json_object_array_get_idx(cur_jso, idx);
                break;
            }

            case json_type_object:
                (void)json_object_object_get_ex(cur_jso, segment_info[i], &jso_found);
                break;

            default:
                *property_jso = NULL;
                return FALSE;
        }

        cur_jso = jso_found;
    }

    *property_jso = jso_found;
    return (jso_found != NULL) ? TRUE : FALSE;
}


int ex_case_get_jso_from_path(json_object *jso, const char *prop_path, const char **real_prop_name,
    json_object **prop_jso)
{
    errno_t safe_fun_ret;
    char            prop_path_dup[MAX_URI_LENGTH] = {0};
    char*           prop_segments[MAX_URI_SEGMENT_NUM] = {NULL};
    int prop_segment_num;
    int i;
    json_type o_type;
    json_object *cur_jso = NULL;
    json_object *jso_found = NULL;
    int idx;
    gint32 value;

    return_val_do_info_if_fail((NULL != jso) && (NULL != prop_path) && (NULL != prop_jso), RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    jso_found = jso;
    cur_jso = jso;
    *prop_jso = NULL;
    
    safe_fun_ret = strncpy_s(prop_path_dup, MAX_URI_LENGTH, prop_path, MAX_URI_LENGTH - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    prop_segment_num = split_path(prop_path_dup, sizeof(prop_path_dup), prop_segments);
    return_val_do_info_if_fail(0 != prop_segment_num, RF_FAILED,
        debug_log(DLOG_ERROR, "get no segment from %s", prop_path));

    for (i = 0; i < prop_segment_num; i++) {
        o_type = json_object_get_type(cur_jso);

        switch (o_type) {
            
            case json_type_array: {
                value = -1;
                if (dal_strtoi(prop_segments[i], &value, DECIMAL_NUM) != RET_OK) {
                    debug_log(DLOG_ERROR, "%s:dal_strtoi failed", __FUNCTION__);
                    return RF_FAILED;
                }
                idx = (int)value;
                jso_found = json_object_array_get_idx(cur_jso, idx);
                break;
            }

            
            case json_type_object:
                (void)json_object_object_case_get(cur_jso, prop_segments[i], real_prop_name, &jso_found);
                break;

            
            default:
                *prop_jso = NULL;
                debug_log(DLOG_ERROR, "cann't get property segment jso , current segment is %s", prop_segments[i]);

                return RF_FAILED;
        }

        cur_jso = jso_found;
    }

    *prop_jso = jso_found;

    return (jso_found) ? RF_OK : RF_FAILED;
}

json_bool json_object_has_key(json_object *jso, const char *json_key)
{
    if (!jso || !json_key || json_type_object != json_object_get_type(jso)) {
        return FALSE;
    }

    return lh_table_lookup_entry(((struct json_object_object *)jso)->c_object, (const void *)json_key) ? TRUE : FALSE;
}

void free_property_extra_info(json_object *jso, void *userdata)
{
    if ((NULL == jso) || (NULL == userdata)) {
        return;
    }
    
    g_slist_free_full((GSList *)userdata, (GDestroyNotify)g_free);
    
}


const char *ex_json_object_object_get_key(json_object *jso)
{
    struct lh_table *obj_ref = json_object_get_object(jso);

    if ((NULL != obj_ref) && (NULL != obj_ref->head)) {
        return (const char *)obj_ref->head->k;
    }

    return NULL;
}

struct json_object *ex_json_object_object_get_val(json_object *jso)
{
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
    struct lh_table *obj_ref = json_object_get_object(jso);

    if ((NULL != obj_ref) && (NULL != obj_ref->head)) {
        return (struct json_object *)obj_ref->head->v;
    }

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif

    return NULL;
}

json_bool ex_json_object_object_get_keyval(json_object *jso, const gchar **key, json_object **val)
{
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
    struct lh_table *obj_ref = json_object_get_object(jso);

    if ((NULL != obj_ref) && (NULL != obj_ref->head)) {
        if (NULL != key) {
            *key = (const char *)obj_ref->head->k;
        }
        if (NULL != val) {
            *val = (struct json_object *)obj_ref->head->v;
        }
        return TRUE;
    }

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif

    return FALSE;
}

gint ex_json_object_array_get_object_item_nth(json_object *jso_array, const char *item_key)
{
    int i;
    int n;
    json_object *jso_item = NULL;
    struct lh_table *obj_ref = NULL;
    return_val_if_fail(
        (NULL != jso_array) && (NULL != item_key) && (json_type_array == json_object_get_type(jso_array)), -1);

    n = json_object_array_length(jso_array);

    for (i = 0; i < n; i++) {
        jso_item = json_object_array_get_idx(jso_array, i);
        if (NULL == jso_item) {
            debug_log(DLOG_ERROR, "json_object_array_get_idx %d fail", n);
            continue;
        }

        // 此处用于消除foreach编译告警"assignment discards 'const' qualifier from pointer target type"
#pragma GCC diagnostic push // require GCC 4.6
#pragma GCC diagnostic ignored "-Wcast-qual"
        obj_ref = json_object_get_object(jso_item);
        
        if (NULL == obj_ref) {
            debug_log(DLOG_ERROR, "json_object_get_object %d fail", n);
            continue;
        }
        

        if (0 == g_ascii_strncasecmp((const gchar *)obj_ref->head->k, item_key, strlen(item_key))) {
            return i;
        }

#pragma GCC diagnostic pop // require GCC 4.6
    }

    return -1;
}
void json_object_object_del_ex(json_object *jso, const char *json_pointer_path)
{
    gchar **path_segments;
    guint path_segment_len;
    json_bool b_ret;
    
    json_object *property_jso = NULL;
    
    GSList *deleted_prop_list_new = NULL;
    GSList *deleted_prop_list_old = NULL;
    gchar *prop_name_dup = NULL;
    
    check_fail_return(NULL != jso);
    check_fail_return(NULL != json_pointer_path);

    check_success_do(*json_pointer_path == '#', json_pointer_path++);
    check_success_do(*json_pointer_path == '/', json_pointer_path++);

    path_segments = g_strsplit(json_pointer_path, "/", -1);
    if (NULL == path_segments) {
        return;
    }

    path_segment_len = g_strv_length(path_segments);
    if (0 == path_segment_len) {
        g_strfreev(path_segments);
        return;
    }

    if ('\0' == *path_segments[path_segment_len - 1]) {
        path_segment_len--;
    }

    b_ret = get_property_obj_from_segments(jso, path_segments, path_segment_len - 1, &property_jso);
    if (!b_ret) {
        debug_log(DLOG_ERROR, "cann't get jso from %s for %s", json_pointer_path, dal_json_object_get_str(jso));

        g_strfreev(path_segments);

        return;
    }

    
    if (path_segment_len) {
        
        (void)json_object_get_user_data(property_jso, (void **)&deleted_prop_list_old);

        prop_name_dup = g_strdup(path_segments[path_segment_len - 1]);
        if (NULL == prop_name_dup) {
            debug_log(DLOG_ERROR, "dup key failed");
            g_strfreev(path_segments);

            return;
        }

        deleted_prop_list_new = g_slist_copy_deep(deleted_prop_list_old, (GCopyFunc)g_strdup, NULL);
        deleted_prop_list_new = g_slist_append(deleted_prop_list_new, prop_name_dup);

        json_object_set_user_data(property_jso, deleted_prop_list_new, free_property_extra_info);
        
        json_object_object_del(property_jso, path_segments[path_segment_len - 1]);
    }

    

    g_strfreev(path_segments);

    return;
}


int json_object_key_index(json_object *jso, const char *name)
{
    int i = 0;
    char *key = NULL;
    struct lh_entry *entry_key = NULL;
    struct lh_entry *entry_next_key = NULL;
    struct lh_table *obj_ref = json_object_get_object(jso);

    check_fail_return_val(json_type_object == json_object_get_type(jso), -1);
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif

    
    if (obj_ref) {
        for (entry_key = obj_ref->head;
            (entry_key ? (key = (char *)entry_key->k, entry_next_key = entry_key->next, entry_key) : 0);
            entry_key = entry_next_key) {
            
            if (!g_strcmp0(name, key)) {
                return i;
            }

            i++;
        }
    }

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif

    return -1;
}

static gint _lh_entry_key_compare_func(gconstpointer d1, gconstpointer d2, gpointer data)
{
    const struct lh_entry *entry1 = (const struct lh_entry *)d1;
    const struct lh_entry *entry2 = (const struct lh_entry *)d2;
    json_object *pattern_jso = (json_object *)data;
    const char *key1 = NULL;
    const char *key2 = NULL;
    int key_index1;
    int key_index2;

    if (((NULL == entry1) && (NULL == entry2)) || (NULL == pattern_jso)) {
        return 0;
    }

    if (NULL == entry1) {
        return -1;
    }

    if (NULL == entry2) {
        return 1;
    }

    key1 = (const char *)entry1->k;
    key2 = (const char *)entry2->k;

    key_index1 = json_object_key_index(pattern_jso, key1);
    key_index2 = json_object_key_index(pattern_jso, key2);
    if ((key_index1 == -1) || (key_index2 == -1)) {
        debug_log(DLOG_ERROR, "cann't find key %s or key %s from %s", key1, key2, dal_json_object_get_str(pattern_jso));

        return 0;
    }

    return (key_index1 - key_index2);
}

static struct lh_entry *_lh_entry_sort_merge(struct lh_entry *lh_e1, struct lh_entry *lh_e2,
    GCompareDataFunc compare_func, gpointer user_data)
{
    struct lh_entry entry_tmp;
    struct lh_entry *lh_e;
    struct lh_entry *lh_eprev;
    gint cmp;

    lh_e = &entry_tmp;
    lh_eprev = NULL;

    while (lh_e1 && lh_e2) {
        cmp = compare_func(lh_e1, lh_e2, user_data);
        if (cmp <= 0) {
            lh_e->next = lh_e1;
            lh_e1 = lh_e1->next;
        } else {
            lh_e->next = lh_e2;
            lh_e2 = lh_e2->next;
        }

        lh_e = lh_e->next;
        lh_e->prev = lh_eprev;
        lh_eprev = lh_e;
    }

    lh_e->next = lh_e1 ? lh_e1 : lh_e2;
    lh_e->next->prev = lh_e;

    return entry_tmp.next;
}

static struct lh_entry *_lh_entry_sort_real(struct lh_entry *entry, GCompareDataFunc compare_func, gpointer user_data)
{
    struct lh_entry *lh_e1 = NULL;
    struct lh_entry *lh_e2 = NULL;

    if (!entry) {
        return NULL;
    }

    if (!entry->next) {
        return entry;
    }

    lh_e1 = entry;
    lh_e2 = entry->next;

    while ((lh_e2 = lh_e2->next) != NULL) {
        if ((lh_e2 = lh_e2->next) == NULL) {
            break;
        }

        lh_e1 = lh_e1->next;
    }

    lh_e2 = lh_e1->next;
    lh_e1->next = NULL;

    return _lh_entry_sort_merge(_lh_entry_sort_real(entry, compare_func, user_data),
        _lh_entry_sort_real(lh_e2, compare_func, user_data), compare_func, user_data);
}


static struct lh_entry *lh_entry_sort_with_data(struct lh_entry *entry, GCompareDataFunc compare_func,
    gpointer user_data)
{
    return _lh_entry_sort_real(entry, compare_func, user_data);
}


void standardize_req_json_data(json_object *pattern_jso, json_object *req_jso)
{
    json_type t_pattern = json_object_get_type(pattern_jso);
    json_type t_data = json_object_get_type(req_jso);
    struct lh_entry *entry = NULL;

    return_do_info_if_fail((json_type_object == t_pattern) && (json_type_object == t_data),
        debug_log(DLOG_ERROR, "standardize jso failed, not object type"));

    ((JSON_OBJECT_T *)req_jso)->c_object->head = lh_entry_sort_with_data(((JSON_OBJECT_T *)req_jso)->c_object->head,
        _lh_entry_key_compare_func, (gpointer)pattern_jso);

    lh_foreach(((JSON_OBJECT_T *)req_jso)->c_object, entry)
    {
        if (NULL == entry->next) {
            break;
        }
    }

    ((JSON_OBJECT_T *)req_jso)->c_object->tail = entry;

    return;
}


static json_bool lh_table_case_lookup_ex(struct lh_table *t, const void *k, const void **k_found, const void **v)
{
    json_bool found = FALSE;
    struct lh_entry *find = NULL;
    int i;

    if (t == NULL || k == NULL) {
        debug_log(DLOG_ERROR, "Invalid Parameter");
        return found;
    }

    for (i = 0; i < t->size; ++i) {
        if (t->table[i].k == LH_EMPTY) { // 如果key是空字符串，继续检查下一个
            continue;
        }
        if (t->table[i].k != LH_FREED && strcasecmp((const char *)k, (const char *)t->table[i].k) == 0) {
            found = TRUE;
            find = &(t->table[i]);
            break;
        }
    }

    if (found && find != NULL) {
        *k_found = find->k;
        *v = find->v;
    }

    return found;
}


json_bool json_object_object_case_get(struct json_object *jso, const char *case_key, const char **key,
    struct json_object **value)
{
    const char *tmp_key = NULL;

    
    if (NULL == key) {
        key = &tmp_key; 
    }

    if (value == NULL) {
        return FALSE;
    }
    *value = NULL;

    if (NULL == jso) {
        return FALSE;
    }

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif

    switch (jso->o_type) {
        case json_type_object:
            return lh_table_case_lookup_ex(((JSON_OBJECT_T *)jso)->c_object, (void *)case_key, (const void **)key,
                (const void **)value);

        default:
            if (value != NULL) {
                *value = NULL;
            }

            return FALSE;
    }

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
}

json_bool json_object_object_add_rdf(struct json_object *obj, const char *objectkey, struct json_object *val)
{
    if (NULL == obj) {
        (void)json_object_put(val);
        return FALSE;
    }
    json_object_object_add(obj, objectkey, val);
    return TRUE;
}


void json_object_patch(json_object *origin_jso, json_object *patch_body_jso)
{
    json_object *jso_found = NULL;
    json_bool b_ret;
    json_type t;
    json_type t_item;
    json_object *obj_iter = NULL;

    check_fail_do_return(origin_jso && patch_body_jso, debug_log(DLOG_ERROR, "Invalid input para"));

    check_fail_do_return((json_type_object == json_object_get_type(origin_jso)) &&
        (json_type_object == json_object_get_type(patch_body_jso)),
        debug_log(DLOG_ERROR, "input param is not of object type"));

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
    json_object_object_foreach0(patch_body_jso, name, val)
    {
        b_ret = json_object_object_get_ex(origin_jso, name, &jso_found);

        check_fail_do_continue(b_ret, debug_log(DLOG_ERROR, "no such a key:%s", name));

        t = json_object_get_type(val);
        
        if (json_type_object == t) {
            (void)json_object_patch(jso_found, val);
        } else if (json_type_array == t) {
            
            check_fail_continue(json_object_array_length(val));

            
            t_item = json_object_get_type(json_object_array_get_idx(val, 0));
            check_success_do_continue(t_item != json_type_object,
                json_object_object_add(origin_jso, name, json_object_get(val)));

            
            check_fail_do_continue(json_object_array_length(val) <= json_object_array_length(jso_found),
                debug_log(DLOG_ERROR, "patch failed for %s, type is array, length mismatch", name));

            json_object_array_foreach(val, obj_iter)
            {
                json_object_patch(json_object_array_get_idx(jso_found, obj_iter_i), obj_iter);
            }
        } else {
            
            json_object_object_add(origin_jso, name, json_object_get(val));
        }
    }
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
}


json_object *jso_object_object_dup(json_object *jso)
{
    const char *jso_str = NULL;

    if (!jso) {
        return NULL;
    }

    jso_str = dal_json_object_get_str(jso);

    return json_tokener_parse(jso_str);
}

void json_object_set_object(json_object *jso, json_object *jso_data)
{
    json_type t_jso;
    json_type t_jso_data;
    json_object *jso_data_dup = NULL;

    check_fail_return(jso && jso_data);

    t_jso = json_object_get_type(jso);
    t_jso_data = json_object_get_type(jso_data);

    check_fail_do_return(json_type_object == t_jso && json_type_object == t_jso_data,
        debug_log(DLOG_ERROR, "%s failed, input para is not object type", __FUNCTION__));

    
    jso_data_dup = jso_object_object_dup(jso_data);

    lh_table_free(((JSON_OBJECT_T *)jso)->c_object);

    ((JSON_OBJECT_T *)jso)->c_object = ((JSON_OBJECT_T *)jso_data_dup)->c_object;

    
    printbuf_free(jso_data_dup->_pb);
    free(jso_data_dup);

    return;
}


gint32 json_array_duplicate_check(json_object *jso_array, gint32 *duplicate_index)
{
    json_type jso_type;
    json_type element_type;
    gint32 i, j;
    gint32 array_len;
    json_object *i_element = NULL;
    json_object *j_element = NULL;
    const gchar *i_str = NULL;
    const gchar *j_str = NULL;

    jso_type = json_object_get_type(jso_array);
    return_val_do_info_if_fail(json_type_array == jso_type, RF_FAILED,
        debug_log(DLOG_ERROR, "%s :is not json array", __FUNCTION__));

    array_len = json_object_array_length(jso_array);

    for (i = 0; i < array_len; i++) {
        i_element = json_object_array_get_idx(jso_array, i);

        element_type = json_object_get_type(i_element);

        for (j = i + 1; j < array_len; j++) {
            j_element = json_object_array_get_idx(jso_array, j);

            switch (element_type) {
                case json_type_int:
                case json_type_boolean:
                case json_type_double:
                case json_type_string:
                    i_str = dal_json_object_get_str(i_element);
                    j_str = dal_json_object_get_str(j_element);

                    return_val_do_info_if_expr(0 == g_strcmp0(i_str, j_str), RF_OK, (*duplicate_index = i));
                    break;
                default:
                    debug_log(DLOG_ERROR, "%s:invalid element type is %s", __FUNCTION__,
                        json_type_to_name(element_type));
                    return RF_FAILED;
            }
        }
    }

    return RF_FAILED;
}


json_object *ex_json_object_new_double(double d, gchar *format)
{
    json_object *double_jso = NULL;

    double_jso = json_object_new_double(d);
    if (double_jso) {
        double_jso->_to_json_string = &json_object_double_to_json_string;
        double_jso->_userdata = format;
    }

    return double_jso;
}
