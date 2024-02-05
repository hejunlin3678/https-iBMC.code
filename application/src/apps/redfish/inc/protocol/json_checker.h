

#ifndef __JSON_CHECKER_H__
#define __JSON_CHECKER_H__


#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include <glib.h>

#ifdef _cplusplus
extern "C" {
#endif

#define JSON_ERR_PATH_TOO_LONG (-6)    // 路径太长
#define JSON_ERR_DEPTH_OVERFLOW (-5)   // 深度溢出
#define JSON_ERR_KEY_TOO_LONG (-4)     // key太长
#define JSON_ERR_KEY_REPEAT (-3)       // key重复
#define JSON_ERR_INTEGER_OVERFLOW (-2) // 整数溢出
#define JSON_ERR (-1)
#define JSON_OK 0

#define MAX_JSON_PATH 512 // json最大路径长度

enum JsonLayerType {
    JSON_LAYER_ARRAY = 1,
    JSON_LAYER_OBJECT,
};

typedef struct {
    gchar **keys;
    gint count;
    gint max_size;
} JsonKeySet; // 用来保存Object的key


#define MAX_JSON_INTEGER 20 // 最大整数长度(INT64_MAX为19位，预留1位作为字符串结束符')

typedef struct JsonError {
    JsonKeySet *repeat_key_set;                // 重复的key
    gchar overflow_interger[MAX_JSON_INTEGER]; // 溢出的整数
    gint layer_type;                           // 当前层的类型
    gchar path[MAX_JSON_PATH];                 // 当前层的类型
} JsonError;

gint json_check(const gchar *json_str, JsonError *error);
JsonError *json_error_new(void);
void json_error_free(JsonError *error);

#ifdef _cplusplus
}
#endif 


#endif 
