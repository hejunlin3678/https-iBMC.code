
#include <securec.h>
#include <ctype.h>
#include <glib.h>

#include "json_checker.h"

#define MAX_JSON_KEY 512    // json最大key
#define MAX_JSON_DEPTH 20   // 最大嵌套深度
#define MAX_JSON_LAYER 22   // 最大层数（这里略大于最大深度,方便逻辑处理)
#define JSON_INIT_KEY_NUM 8 // 初始key的数量

#define JSON_VALUE_DELIM ','

enum JsonValueType {
    JSON_VALUE_NUMBER = 1,
    JSON_VALUE_STRING,
    JSON_VALUE_OBJECT,
    JSON_VALUE_ARRAY,
    JSON_VALUE_BOOL,
    JSON_VALUE_NULL,
};

enum JsonNumberStat {
    NUMBER_STATE_ERR = -1,
    NUMBER_STATE_NONE,            // 起始状态
    NUMBER_STATE_ZERO,            // 开头为0
    NUMBER_STATE_INTEGER,         // 整数状态
    NUMBER_STATE_FLOAT,           // 浮点数状态
    NUMBER_STATE_SCIENCE,         // 科学计数法状态
    NUMBER_STATE_SCIENCE_INTEGER, // 科学计数法整数状态
};

typedef gchar(JsonKey)[MAX_JSON_KEY];

struct JsonLayer;
typedef gint (*JsonKeyDealFunc)(const gchar **json, const struct JsonLayer *layer, JsonKey key, gint key_size);

typedef struct JsonLayer {
    gchar path[MAX_JSON_PATH + 1]; // 当前层的路径
    JsonKeySet *key_set;           // object需要保存key值
    gint layer_type;               // 当前类型(object还是数组)
    gint elem_index;               // 当前正在处理的元素索引
    gchar start_delim;             // 开始标志
    gchar end_delim;               // 结束标志
    JsonKeyDealFunc key_deal_func; // 处理json key值得函数
    JsonKey elem_key;              // 当前元素的key
    gint depth;                    // 深度
} JsonLayer;

typedef struct {
    JsonLayer *layers[MAX_JSON_LAYER];
    gint depth; // 堆栈深度
} JsonLayerStack;

LOCAL gint json_object_deal_key(const gchar **json, const JsonLayer *layer, JsonKey key, gint key_size);
LOCAL gint json_array_deal_key(const gchar **json, const JsonLayer *layer, JsonKey key, gint key_size);

LOCAL void json_key_set_free(JsonKeySet *set)
{
    for (gint i = 0; i < set->max_size; i++) {
        free(set->keys[i]);
    }
    free(set->keys);
    free(set);
}


LOCAL gint json_key_set_resize(JsonKeySet *set, gint new_size)
{
    if (new_size <= set->max_size) {
        debug_log(DLOG_ERROR, "can't resize to smaller, old_size=%d new_size=%d", set->max_size, new_size);
        return JSON_ERR;
    }

    gchar **new_keys = (gchar **)malloc(sizeof(gchar *) * new_size);
    if (new_keys == NULL) {
        debug_log(DLOG_ERROR, "malloc JsonKey* array failed, size=%zu", sizeof(JsonKey *) * new_size);
        return JSON_ERR;
    }
    // 增加的部分才需要重新分配
    for (gint i = set->max_size; i < new_size; i++) {
        new_keys[i] = (gchar *)malloc(sizeof(JsonKey));
        if (new_keys[i] == NULL) {
            debug_log(DLOG_ERROR, "malloc JsonKey failed, size=%zu", sizeof(JsonKey));
            for (gint j = set->max_size; j < i; j++) { // 释放已分配的内存
                free(new_keys[j]);
            }
            free(new_keys);
            return JSON_ERR;
        }
    }
    // 复用以前的
    for (gint i = 0; i < set->count && i < new_size; i++) {
        new_keys[i] = set->keys[i];
    }
    free(set->keys);
    set->keys = new_keys;
    set->max_size = new_size;
    return JSON_OK;
}


LOCAL JsonKeySet *json_key_set_new(gint max_size)
{
    JsonKeySet *set = (JsonKeySet *)malloc(sizeof(JsonKeySet));
    if (set == NULL) {
        debug_log(DLOG_ERROR, "malloc JsonKeySet failed, size=%zu", sizeof(JsonKeySet));
        return NULL;
    }
    (void)memset_s(set, sizeof(JsonKeySet), 0, sizeof(JsonKeySet));
    if (json_key_set_resize(set, max_size) == JSON_ERR) {
        free(set);
        return NULL;
    }
    return set;
}


LOCAL gint json_key_set_add(JsonKeySet *set, const JsonKey key)
{
    if (set->count == set->max_size) {
        if (json_key_set_resize(set, set->max_size * 2) == JSON_ERR) {
            debug_log(DLOG_ERROR, "json_key_set_resize failed");
            return JSON_ERR;
        }
    }
    if (strcpy_s(set->keys[set->count++], sizeof(JsonKey), key) != EOK) {
        return JSON_ERR;
    }
    return JSON_OK;
}


LOCAL void json_layer_free(JsonLayer *layer)
{
    if (layer->key_set != NULL) {
        json_key_set_free(layer->key_set);
    }
    free(layer);
}


LOCAL JsonLayer *json_layer_new(gint depth, const gchar *path, gint layer_type)
{
    JsonLayer *layer = (JsonLayer *)malloc(sizeof(JsonLayer));
    if (layer == NULL) {
        debug_log(DLOG_ERROR, "malloc JsonLayer failed, size=%zu", sizeof(JsonLayer));
        return NULL;
    }
    (void)memset_s(layer, sizeof(JsonLayer), 0, sizeof(JsonLayer));
    layer->layer_type = layer_type;
    layer->key_set = NULL;
    if (layer_type == JSON_LAYER_ARRAY) {
        layer->start_delim = '[';
        layer->end_delim = ']';
        layer->key_deal_func = json_array_deal_key;
    } else {
        layer->key_set = json_key_set_new(JSON_INIT_KEY_NUM);
        if (layer->key_set == NULL) {
            debug_log(DLOG_ERROR, "json_key_set_new failed");
            json_layer_free(layer);
            return NULL;
        }
        layer->start_delim = '{';
        layer->end_delim = '}';
        layer->key_deal_func = json_object_deal_key;
    }
    if (strcpy_s(layer->path, sizeof(layer->path), path) != EOK) {
        debug_log(DLOG_ERROR, "strcpy_s path failed, path_len=%zu", strlen(path));
        json_layer_free(layer);
        return NULL;
    }
    layer->elem_index = -1;
    layer->depth = depth;
    return layer;
}


LOCAL gint json_layer_stack_push(JsonLayerStack *layer_stack, JsonLayer *layer)
{
    if (layer_stack->depth == MAX_JSON_LAYER) {
        return JSON_ERR;
    }
    layer_stack->layers[layer_stack->depth++] = layer;
    return JSON_OK;
}


LOCAL gboolean json_stack_contain_layer(JsonLayerStack *layer_stack, JsonLayer *layer) 
{
    gint32 layer_index;

    for (layer_index = layer_stack->depth - 1; layer_index >= 0; layer_index--) {
        if (layer_stack->layers[layer_index] == layer) {
            return TRUE;
        }
    }

    return FALSE;
}


LOCAL JsonLayer *json_layer_stack_pop(JsonLayerStack *layer_stack)
{
    if (layer_stack->depth < 1) {
        return NULL;
    }
    return layer_stack->layers[--layer_stack->depth];
}


LOCAL gint json_layer_stack_is_empty(JsonLayerStack *layer_stack)
{
    return layer_stack->depth <= 0;
}


LOCAL JsonLayerStack *Json_layer_stack_new(void)
{
    JsonLayerStack *layer_stack = (JsonLayerStack *)g_malloc0(sizeof(JsonLayerStack));
    if (layer_stack == NULL) {
        debug_log(DLOG_ERROR, "malloc layer layer_stack failed, size=%zu", sizeof(JsonLayerStack));
        return NULL;
    }
    return layer_stack;
}


LOCAL void json_layer_stack_free(JsonLayerStack *stack)
{
    while (1) {
        JsonLayer *layer = json_layer_stack_pop(stack);
        if (layer == NULL) {
            break;
        }
        json_layer_free(layer);
    }
    free(stack);
}


static inline gint json_char_is_valid(const gchar *json_str)
{
    return json_str != NULL && *json_str != '\0';
}


LOCAL gint json_strip_space(const gchar **json)
{
    const gchar *p = *json;
    while (1) {
        if (!json_char_is_valid(p)) {
            debug_log(DLOG_ERROR, "%s: gchar is not valid", __FUNCTION__);
            return JSON_ERR;
        }
        if (!isspace(*p)) {
            break;
        }
        p++;
    }
    *json = p;
    return JSON_OK;
}


LOCAL gboolean check_json_string_val_end(const gchar* s_start, const gchar* s_current)
{
    
    if (*s_current != '"') {
        return FALSE;
    }
 
    s_current--;
    const gchar* p = s_current;
    while (s_current >= s_start) {
        if (*s_current == '\\') {
            s_current--;
        } else {
            break;
        }
    }
 
    
    if ((p - s_current) % 2 == 0) {
        return TRUE;
    }
 
    return FALSE;
}


LOCAL gint json_get_string(const gchar **json, gchar *key, gint key_len)
{
    const gchar *p = *json;
    if (*p != '"') {
        debug_log(DLOG_ERROR, "json string format wrong");
        return JSON_ERR;
    }
    p++; // 跳过第一个"
    const gchar *start_p = p;
    while (1) {
        if (!json_char_is_valid(p)) {
            debug_log(DLOG_ERROR, "%s: gchar is not valid", __FUNCTION__);
            return JSON_ERR;
        }
        if (check_json_string_val_end(start_p, p)) {
            break;
        }
        p++;
    }
    gint str_len = p - start_p;
    if (key != NULL) {
        if (str_len >= key_len) {
            debug_log(DLOG_ERROR, "key too long, len=%d, max=%d", str_len, key_len);
            return JSON_ERR_KEY_TOO_LONG;
        }
        if (strncpy_s(key, key_len, start_p, str_len) != 0) {
            debug_log(DLOG_ERROR, "strcpy json failed");
            return JSON_ERR;
        }
    }
    *json = p + 1; // 跳过'"'
    return JSON_OK;
}


LOCAL gint json_strip_space_to_char_next(const gchar **json, gchar ch)
{
    if (json_strip_space(json) == JSON_ERR) {
        return JSON_ERR;
    }
    const gchar *p = *json;
    if (*p != ch) {
        return JSON_ERR;
    }
    *json = p + 1; // 跳过ch
    return JSON_OK;
}


LOCAL gint json_object_strip_to_value(const gchar **json)
{
    if (json_strip_space_to_char_next(json, ':') == JSON_ERR) {
        return JSON_ERR;
    }
    return JSON_OK;
}


LOCAL gint json_object_deal_key(const gchar **json, const JsonLayer *layer, JsonKey key, gint key_size)
{
    gint elem_index = layer->elem_index;
    if (json_strip_space(json) == JSON_ERR) { // 跳过空格
        debug_log(DLOG_ERROR, "object strip space failed, index=%d, path=%s", elem_index, layer->path);
        return JSON_ERR;
    }
    gint err_code = json_get_string(json, key, key_size);
    if (err_code != JSON_OK) {
        debug_log(DLOG_ERROR, "object get key failed, index=%d, path=%s", elem_index, layer->path);
        return JSON_ERR;
    }
    if (strlen(key) > MAX_JSON_KEY) { // key超长
        debug_log(DLOG_ERROR, "object key too long, index=%d, key=%s, path=%s", elem_index, key, layer->path);
        return JSON_ERR_KEY_TOO_LONG;
    }
    // 加入集合
    if (json_key_set_add(layer->key_set, key) == JSON_ERR) {
        debug_log(DLOG_ERROR, "dict key cpy failed, index=%d, key=%s, path=%s", elem_index, key, layer->path);
        return JSON_ERR;
    }
    // 路径长度
    if (strlen(layer->path) + strlen(key) > MAX_JSON_PATH) {
        debug_log(DLOG_ERROR, "json path too long, index=%d, path=%s", elem_index, layer->path);
        return JSON_ERR_PATH_TOO_LONG;
    }
    // 跳到value的
    if (json_object_strip_to_value(json) == JSON_ERR) {
        debug_log(DLOG_ERROR, "dict value strip space failed, index=%d, key=%s, path=%s", elem_index, key, layer->path);
        return JSON_ERR;
    }
    return JSON_OK;
}


LOCAL gint json_array_deal_key(const gchar **json, const JsonLayer *layer, JsonKey key, gint key_size)
{
    if (snprintf_s(key, key_size, key_size - 1, "%d", layer->elem_index) == -1) {
        return JSON_ERR;
    }
    return JSON_OK;
}


LOCAL gint json_get_value_type(const gchar **json)
{
    if (json_strip_space(json) == JSON_ERR) {
        return JSON_ERR;
    }
    gchar ch = **json;
    if (ch == '"') {
        return JSON_VALUE_STRING;
    } else if (ch == '-' || isdigit(ch)) { // 数字
        return JSON_VALUE_NUMBER;
    } else if (ch == '{') {
        return JSON_VALUE_OBJECT;
    } else if (ch == '[') {
        return JSON_VALUE_ARRAY;
    } else if (ch == 'f' || ch == 't') {
        return JSON_VALUE_BOOL;
    } else if (ch == 'n') {
        return JSON_VALUE_NULL;
    }
    return JSON_ERR;
}


LOCAL gint json_get_layer_key_path(const JsonLayer *layer, gchar *path, gint path_size)
{
    if (strlen(layer->elem_key) > 0) {            // 有key值
        if (snprintf_truncated_s(path, path_size, // 这儿允许截断，因为路径可能太长了，就返回截断后的值
            "%s/%s", layer->path, layer->elem_key) == -1) {
            debug_log(DLOG_ERROR, "snprintf_s layer path failed");
            return JSON_ERR;
        }
    } else { // 没有key值，就直接用layer的路径
        if (strcpy_s(path, path_size, layer->path) != EOK) {
            debug_log(DLOG_ERROR, "strcpy layer path failed");
            return JSON_ERR;
        }
    }
    return JSON_OK;
}


LOCAL gint json_new_layer_call(JsonLayerStack *layer_stack, JsonLayer *layer, gint new_layer_type)
{
    gchar layer_path[MAX_JSON_PATH];
    if (json_get_layer_key_path(layer, layer_path, sizeof(layer_path)) == JSON_ERR) {
        debug_log(DLOG_ERROR, "get layer path failed, key=%s, path=%s", layer->elem_key, layer->path);
        return JSON_ERR;
    }
    JsonLayer *new_layer = json_layer_new(layer->depth + 1, layer_path, new_layer_type);
    if (new_layer == NULL) {
        debug_log(DLOG_ERROR, "new layer failed, layer_type=%d, path=%s", new_layer_type, layer->path);
        return JSON_ERR;
    }
    
    if (json_layer_stack_push(layer_stack, layer) == JSON_ERR) {
        debug_log(DLOG_ERROR, "push cur layer failed, path=%s", layer->path);
        json_layer_free(new_layer);
        return JSON_ERR;
    }
    
    if (json_layer_stack_push(layer_stack, new_layer) == JSON_ERR) {
        debug_log(DLOG_ERROR, "push new layer failed path=%s", new_layer->path);
        json_layer_free(new_layer);
        return JSON_ERR;
    }
    return JSON_OK;
}


LOCAL gint json_number_state_change(gchar ch, gint state, gint *sign, gint *science_sign)
{
    if (isdigit(ch)) { // 数字
        if (state == NUMBER_STATE_NONE) {
            return ch == '0' ? NUMBER_STATE_ZERO : NUMBER_STATE_INTEGER; // 第一个整数是0特殊处理
        } else if (state == NUMBER_STATE_SCIENCE) {                      // 科学计数法
            return NUMBER_STATE_SCIENCE_INTEGER;                         // 科学计数法开始
        } else if (state == NUMBER_STATE_ZERO) {                         // 不允许前缀0
            return NUMBER_STATE_ERR;                                     // 状态错误
        }
        return state; // 保持状态
    } else if (ch == '-') {
        if (state == NUMBER_STATE_NONE) {
            *sign = -1;
        } else if (state == NUMBER_STATE_SCIENCE) { // '-'只允许出现在开头
            *science_sign = -1;
        } else {
            return NUMBER_STATE_ERR; // 状态错误
        }
        return state; // 保持状态状态
    } else if (ch == '.') {
        if (state == NUMBER_STATE_INTEGER || state == NUMBER_STATE_ZERO) { // 只有整数或者0可以切换为浮点状态
            return NUMBER_STATE_FLOAT;
        }
        return NUMBER_STATE_ERR;
    } else if (ch == 'e' || ch == 'E') {
        if (state == NUMBER_STATE_INTEGER || state == NUMBER_STATE_FLOAT) { // 只有整数和浮点可以切换成科学计数法状态
            return NUMBER_STATE_SCIENCE;
        }
        return NUMBER_STATE_ERR;
    } else { // 出现其他字符
        return NUMBER_STATE_ERR;
    }
}


LOCAL gint json_deal_number(const gchar **json, JsonLayer *layer, JsonError *error)
{
    gdouble accuray = 0.1;       // 浮点精度
    gdouble science_coeff = 0.0; // 科学计数法的底数
    gint64 science_exp = 0;      // 科学计数法的指数
    gint64 number_int = 0;       // 整数值,用来判断溢出

    gint sign = 1;         // 数值符号
    gint science_sign = 1; // 科学计数法符号

    gint int_overflow = 0;
    gint number_state = NUMBER_STATE_NONE;
    const gchar *p = *json;
    for (;; p++) {
        if (!json_char_is_valid(p)) {
            debug_log(DLOG_ERROR, "%s: gchar is not valid", __FUNCTION__);
            return JSON_ERR;
        }
        // 状态切换
        gint new_state = json_number_state_change(*p, number_state, &sign, &science_sign);
        if (new_state == NUMBER_STATE_ERR) { // 状态错误, 解析结束
            break;
        }
        number_state = new_state;
        if (!isdigit(*p)) { // 非数字continue
            continue;
        }
        // 解析数字
        if (number_state == NUMBER_STATE_INTEGER) {
            gint64 new_number = number_int * 10 + (gint64)sign * (*p - '0');
            if ((sign > 0 && new_number < number_int) || (sign < 0 && new_number > number_int)) { // 判断溢出
                int_overflow = 1;
            }
            number_int = new_number;
            // 保存值
            science_coeff = science_coeff * 10 + sign * (*p - '0');
        } else if (number_state == NUMBER_STATE_FLOAT) { // 小数位
            science_coeff += sign * (*p - '0') * accuray;
            accuray *= 0.1;
        } else if (number_state == NUMBER_STATE_SCIENCE_INTEGER) { // 保存科学计数的值
            science_exp = science_exp * 10 + (gint64)science_sign * (*p - '0');
        } else {
            debug_log(DLOG_ERROR, "unkown number state, state=%d", number_state);
        }
    }
    // 只要前面的整数部分溢出了就都算溢出
    if (int_overflow == 1) {
        // 保存字符串 
        if (memcpy_s(error->overflow_interger, MAX_JSON_INTEGER - 1, *json,
            ((p - *json) > (MAX_JSON_INTEGER - 1) ? (MAX_JSON_INTEGER - 1) : (p - *json))) != 0) {
            debug_log(DLOG_ERROR, "memcpy interger failed");
            return JSON_ERR;
        }
        return JSON_ERR_INTEGER_OVERFLOW;
    }
    *json = p;

    return JSON_OK;
}


LOCAL gint json_deal_string(const gchar **json_str, const JsonLayer *layer, JsonError *error)
{
    gint err_code = json_get_string(json_str, NULL, 0);
    if (err_code == JSON_ERR) {
        debug_log(DLOG_ERROR, "unkown string,  handle string failed, path=%s", layer->path);
        return JSON_ERR;
    }
    return JSON_OK;
}


LOCAL gint json_deal_bool(const gchar **json_str, const JsonLayer *layer, JsonError *error)
{
    const gchar *bool_str = "true";
    if (**json_str == 'f') {
        bool_str = "false";
    }
    if (strncmp(*json_str, bool_str, strlen(bool_str)) != 0) {
        debug_log(DLOG_ERROR, "unkown bool, handle bool failed, path=%s", layer->path);
        return JSON_ERR;
    }
    *json_str += strlen(bool_str);
    return JSON_OK;
}


LOCAL gint json_deal_null(const gchar **json_str, const JsonLayer *layer, JsonError *error)
{
    const gchar *null_str = "null";
    if (strncmp(*json_str, null_str, strlen(null_str)) != 0) {
        debug_log(DLOG_ERROR, "unkown null, handle null failed, path=%s", layer->path);
        return JSON_ERR;
    }
    *json_str += strlen(null_str);
    return JSON_OK;
}


// 处理value
LOCAL gint json_deal_common_value(const gchar **json, gint value_type, JsonLayer *layer, JsonError *error)
{
    switch (value_type) {
        case JSON_VALUE_NUMBER:
            return json_deal_number(json, layer, error);
        case JSON_VALUE_STRING:
            return json_deal_string(json, layer, error);
        case JSON_VALUE_BOOL:
            return json_deal_bool(json, layer, error);
        case JSON_VALUE_NULL:
            return json_deal_null(json, layer, error);
        default:
            return JSON_ERR;
    }
}


LOCAL gint json_strip_to_elem(const JsonLayer *layer, const gchar **json)
{
    // 第一个元素之前没有逗号
    if (layer->elem_index == 0) {
        return JSON_OK;
    }
    if (json_strip_space_to_char_next(json, JSON_VALUE_DELIM) == JSON_ERR) { // 跳到下一个元素
        debug_log(DLOG_ERROR, "strip to elem failed");
        return JSON_ERR;
    }
    return JSON_OK;
}


LOCAL gint json_layer_is_end(const JsonLayer *layer, const gchar **json)
{
    return json_strip_space_to_char_next(json, layer->end_delim) == JSON_OK;
}


LOCAL gint json_deal_layer(const gchar **json, JsonLayer *layer, gint *new_layer_type, JsonError *error)
{
    if (layer->elem_index == -1) { // 第一次调用
        // 跳过开始符
        if (json_strip_space_to_char_next(json, layer->start_delim) == JSON_ERR) {
            debug_log(DLOG_ERROR, "strip to '%c' failed, path=%s", layer->start_delim, layer->path);
            return JSON_ERR;
        }
    }
    // 继续处理下一个的元素
    layer->elem_index++;
    for (;; layer->elem_index++) {
        layer->elem_key[0] = '\0'; // 清除key
        // 处理是否结束
        if (json_layer_is_end(layer, json)) {
            debug_log(DLOG_DEBUG, "handle layer finish, path=%s", layer->path);
            break;
        }
        // 跳到层元素
        if (json_strip_to_elem(layer, json) == JSON_ERR) {
            debug_log(DLOG_ERROR, "strip elem failed, index=%d, path=%s", layer->elem_index, layer->path);
            return JSON_ERR;
        }
        // 处理key
        (void)memset_s(layer->elem_key, sizeof(layer->elem_key), 0, sizeof(layer->elem_key));
        gint err_code = layer->key_deal_func(json, layer, layer->elem_key, sizeof(layer->elem_key));
        if (err_code != JSON_OK) {
            debug_log(DLOG_ERROR, "deal key failed, layer_type=%d, path=%s", layer->layer_type, layer->path);
            return err_code;
        }

        // 处理value
        gint type = json_get_value_type(json);
        if (type == JSON_ERR) {
            debug_log(DLOG_ERROR, "get value type failed, index=%d, path=%s", layer->elem_index, layer->path);
            return JSON_ERR;
        }
        // 如果是object或者array跳转到新的层执行
        if (type == JSON_VALUE_OBJECT || type == JSON_VALUE_ARRAY) {
            *new_layer_type = (type == JSON_VALUE_ARRAY) ? JSON_LAYER_ARRAY : JSON_LAYER_OBJECT;
            break; // 退出当前层的处理
        } else {
            err_code = json_deal_common_value(json, type, layer, error);
            if (err_code != JSON_OK) {
                return err_code;
            }
        }
    }
    return JSON_OK;
}

LOCAL gint json_key_cmp(const void *p1, const void *p2)
{
// require GCC 4.6
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif

    const gchar* s1 = *((const gchar**)p1);
    const gchar* s2 = *((const gchar**)p2);
    return strncmp(s1, s2, MAX_JSON_KEY);

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
}


LOCAL gint CreateFullPath(const gchar *path, const gchar *key, gchar *duplicatedKeyFullPath, gint len)
{
    gint32 ret;
    if (g_strcmp0(path, "#") == 0) {
        ret = snprintf_s(duplicatedKeyFullPath, len, len - 1, "%s", key);
    } else {
        ret = snprintf_s(duplicatedKeyFullPath, len, len - 1, "%s/%s", path + strlen("#/"), key);           
    }
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "snprintf_s in %s failed, ret = %d", __FUNCTION__, ret);
        return JSON_ERR;
    } 
    return JSON_OK;
}


LOCAL gint json_layer_deal_repeat_keys(JsonLayer *layer, JsonKeySet *repeat_key_set)
{
    // 数组不做检查
    if (layer->layer_type == JSON_LAYER_ARRAY) {
        return 0;
    }
    // 对键排序
    qsort(layer->key_set->keys, layer->key_set->count, sizeof(gchar *), json_key_cmp);
    gint repeat_flag = 0;
    gchar duplicatedKey[MAX_JSON_PATH];
    for (gint i = 1; i < layer->key_set->count; i++) {
        (void) memset_s(duplicatedKey, sizeof(duplicatedKey), 0, sizeof(duplicatedKey));
        if (g_strcmp0(layer->key_set->keys[i - 1], layer->key_set->keys[i]) == 0) {
            if (repeat_flag == 1) { // 已经计算过了
                continue;
            }
            if (CreateFullPath(layer->path, layer->key_set->keys[i], duplicatedKey, MAX_JSON_PATH) == JSON_ERR) {
                break;
            }
            if (json_key_set_add(repeat_key_set, duplicatedKey) == JSON_ERR) {
                break;
            }
            repeat_flag = 1;
        } else {
            repeat_flag = 0; // 充值flag
        }
    }
    return repeat_key_set->count;
}


JsonError *json_error_new(void)
{
    JsonError *error = (JsonError *)malloc(sizeof(JsonError));
    if (error == NULL) {
        debug_log(DLOG_ERROR, "malloc json error failed, size=%zu", sizeof(JsonError));
        return NULL;
    }
    (void)memset_s(error, sizeof(JsonError), 0, sizeof(JsonError));
    error->repeat_key_set = json_key_set_new(JSON_INIT_KEY_NUM);
    if (error->repeat_key_set == NULL) {
        debug_log(DLOG_ERROR, "json_kye_set new failed");
        free(error);
        return NULL;
    }
    return error;
}


void json_error_free(JsonError *error)
{
    if (error == NULL) {
        return;
    }
    json_key_set_free(error->repeat_key_set);
    free(error);
}


LOCAL gint json_check_handle(JsonLayerStack *layer_stack, const gchar **json, JsonError *error)
{
    JsonLayer *layer = NULL;
    gint err_code = JSON_OK;

    while (!json_layer_stack_is_empty(layer_stack)) {
        // 弹出当前层的执行环境
        layer = json_layer_stack_pop(layer_stack);
        if (layer == NULL) {
            debug_log(DLOG_ERROR, "no layer info");
            return JSON_ERR;
        }
        // 判断深度
        if (layer->depth > MAX_JSON_DEPTH) {
            debug_log(DLOG_ERROR, "layer depth overflow, depth=%d, path=%s", layer->depth, layer->path);
            err_code = JSON_ERR_DEPTH_OVERFLOW;
            goto EXIT;
        }
        // 处理当前层
        gint new_layer_type = -1;
        err_code = json_deal_layer(json, layer, &new_layer_type, error);
        if (err_code != JSON_OK) {
            debug_log(DLOG_ERROR, "deal layer failed, path=%s", layer->path);
            goto EXIT;
        }

        if (new_layer_type != -1) { // 有新的层需要处理，保存当前层的状态，压入新的层
            if (json_new_layer_call(layer_stack, layer, new_layer_type) == JSON_ERR) { 
                err_code = JSON_ERR;
                debug_log(DLOG_ERROR, "new layer call failed, key=%s, path=%s", layer->elem_key, layer->path);
                goto EXIT;
            }
            continue;
        }
        // 没有中断，代表本层已经处理完成了
        // 检查是否有重复key
        if (json_layer_deal_repeat_keys(layer, error->repeat_key_set) > 0) {
            err_code = JSON_ERR_KEY_REPEAT;
            debug_log(DLOG_ERROR, "layer key repeated, path=%s", layer->path);
        }

        // 如果当前Layer不为空，并且不在栈中，需要释放分配的内存空间，在栈中的layer由栈释放时一起释放
        if (json_stack_contain_layer(layer_stack, layer) == FALSE) {
            json_layer_free(layer); // 已经出栈的layer需要释放
            layer = NULL;
        }
    }

EXIT:
    if (layer != NULL) {
            // 构造error的信息
        error->layer_type = layer->layer_type;
        if (err_code == JSON_ERR_INTEGER_OVERFLOW && 
            json_get_layer_key_path(layer, error->path, sizeof(error->path)) == JSON_ERR) {
            debug_log(DLOG_ERROR, "get layer path in %s failed.", __FUNCTION__);
        }
       
        // 如果当前Layer不为空，并且不在栈中，需要释放分配的内存空间，在栈中的layer由栈释放时一起释放
        if (json_stack_contain_layer(layer_stack, layer) == FALSE) {
            json_layer_free(layer); // 已经出栈的layer需要释放
        }
    }    
    
    return err_code;
}

gint json_check(const gchar *json_str, JsonError *error)
{
    gint err_code;
    const gchar **json = &json_str;

    if (json_str == NULL) {
        return JSON_ERR;
    }
    // 分配栈
    JsonLayerStack *layer_stack = Json_layer_stack_new();
    if (layer_stack == NULL) {
        debug_log(DLOG_ERROR, "new layer stack failed");
        return JSON_ERR;
    }
    // 整个json就是一个大的object
    JsonLayer *layer = json_layer_new(1, "#", JSON_LAYER_OBJECT);
    if (layer == NULL) {
        debug_log(DLOG_ERROR, "new frist json layer failed, layer_type=%d", JSON_LAYER_OBJECT);
        json_layer_stack_free(layer_stack);
        return JSON_ERR;
    }

    if (json_layer_stack_push(layer_stack, layer) == JSON_ERR) { // 这儿不可能失败
        debug_log(DLOG_ERROR, "push layer failed, path=%s", layer->path);
        json_layer_free(layer); // 释放当前层
        json_layer_stack_free(layer_stack);
        return JSON_ERR;
    }
    // 处理开始
    err_code = json_check_handle(layer_stack, json, error);
    json_layer_stack_free(layer_stack); // 释放所有层
    return err_code;
}