/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 * Description: 针对开源软件json-c函数进行进一步封装以便提供给lua脚本调用，进行json格式数据的编解码
 * Author: yuanquanzheng
 *
 * Note:
 * lua与C之间相互调用使用堆栈进行数据传递，堆栈中可以存储所有的C和lua相关的数据类型
 *
 * lua_State中存放的是lua虚拟机中的环境表，注册表，运行堆栈，虚拟机的上下文等数据，C语言可以通过该变量获取从lua
 * 传入过来的函数参数以及返回函数返回值
 *
 * lua虚拟栈存在两个索引，正索引和负索引:
 *      正索引从栈底开始索引，栈底索引固定为1, 依次向栈顶递增
 *      负索引从栈顶开始索引，栈顶索引固定为-1，依次项栈底递减
 *
 *        正索引          负索引
 * 栈顶     4   ==========  -1
 *          3   ==========  -2
 *          2   ==========  -3
 * 栈底     1   ==========  -4
 *
 * lua_gettop: 获取当前栈的元素个数
 * lua_settop: 设置栈顶到指定索引处，超出该索引部分将自动出栈丢弃，内存自动回收
 * lua_objlen：获取指定索引处，对象的个数
 * lua_rawgeti: 从栈中指定索引的table中获取table中指定位置的元素值(元素值将被压栈到栈顶)
 * lua_pop: 从指定栈中弹出指定个数元素
 * lua_pushnil： 向栈中压入一个nil
 * lua_next: 从栈中指定索引的表中取出下一个元素值，使用前需要将栈顶压入当前的key值，压入nil则取第一个val
 * lua_rawseti：将当前压入到栈中的元素设置到栈中指定索引的表中的指定位置
 */
#include <arpa/inet.h>
#include <pme/common/mscm_vos.h>
#include <stdio.h>
#include <securec.h>
#include <glib.h>
#include <lauxlib.h>
#include <lua.h>
#include "json.h"
#include "json_tokener.h"

static void push_json_value(lua_State *L, json_object *val);
static json_object *new_json(lua_State *L, int idx);
static gint32 g_encode_empty_table_as_object = 0;

static const gchar* json_object_get_str(struct json_object *obj)
{
    json_type type = json_object_get_type(obj);
    if (type == json_type_string || type == json_type_null) {
        return json_object_get_string(obj);
    }

    return json_object_to_json_string_ext(obj, JSON_C_TO_STRING_PLAIN);
}

/*
 * Description: 仿造lua5.3特性实现从虚拟栈负索引，转正索引的函数
 */
static int lua_absindex(lua_State *L, int idx)
{
    if (idx < 0) {
        return idx + lua_gettop(L) + 1;
    }

    return idx;
}

/*
 * Description: 从堆栈中取出一个数组类型的数据，并生成json数组对象
 */
static json_object *new_json_array(lua_State *L, int idx)
{
    size_t len = lua_objlen(L, idx);
    int it = lua_absindex(L, idx);
    json_object* obj = json_object_new_array();

    for (size_t i = 0; i < len; ++i) {
        lua_rawgeti(L, it, i + 1);
        json_object_array_add(obj, new_json(L, -1));
        lua_pop(L, 1);
    }

    return obj;
}

/*
 * Description: 判断当前对象是否是空对象
 * Note:
 *    往队列中压入nil后按对象方式取一个值，取不到则为空对象
 */
static int is_empty_object(lua_State *L, int idx)
{
    int it = lua_absindex(L, idx);
    int m_top = lua_gettop(L);

    lua_pushnil(L);
    int b_empty = lua_next(L, it);
    lua_settop(L, m_top);

    return b_empty == 0;
}

/*
 * Description: 从堆栈中取出一个对象数据, 并生成对应的json数据
 */
static json_object *new_json_object(lua_State *L, int idx)
{
    if (is_empty_object(L, idx) && !g_encode_empty_table_as_object) {
        return json_object_new_array();
    }

    json_object *obj = json_object_new_object();
    int it = lua_absindex(L, idx);

    lua_pushnil(L); // 栈顶压入nil
    while (lua_next(L, it)) {
        const char *key  = lua_tostring(L, -2);
        json_object_object_add(obj, key, new_json(L, -1));
        lua_pop(L, 1);
    }

    lua_pop(L, 1); // 弹出一个元素
    
    return obj;
}

/*
 * Description: 从堆栈的指定索引处，取出一个元素并生成json对象
 */
static json_object *new_json(lua_State *L, int idx)
{
    int m_top = lua_gettop(L);
    int type = lua_type(L, idx);
    json_object* obj = NULL;
    switch (type) {
        case LUA_TSTRING:{
            size_t len = 0;
            const char *s  = lua_tolstring(L, idx, &len);
            obj = json_object_new_string_len(s, len);
            break;
        }
        case LUA_TBOOLEAN:
            obj = json_object_new_boolean(lua_toboolean(L, idx));
            break;
        case LUA_TNUMBER:{
            int integer = lua_tointeger(L, idx);
            double number = lua_tonumber(L, idx);
            if (number == (double)integer) {
                obj = json_object_new_int64(integer);
            } else {
                obj = json_object_new_double(lua_tonumber(L, idx));
            }
            break;
        }
        case LUA_TLIGHTUSERDATA:
            if (lua_touserdata(L, idx) == NULL) {
                obj = json_object_new_null();
            } else {
                obj = json_object_new_string(lua_tostring(L, idx));
            }
            break;
        case LUA_TNIL:
            obj = json_object_new_null();
            break;
        case LUA_TTABLE:
            if (lua_objlen(L, idx) > 0) {
                obj = new_json_array(L, idx);
            } else {
                obj = new_json_object(L, idx);
            }
            break;
        default:
            luaL_error(L, "Invalid lua type %d", type);
            break;
    }

    lua_settop(L, m_top);

    return obj;
}

/*
 * Description: lua调用encode函数的具体实现方法，实现从lua数据转成json字符串的功能
 */
static int lcjson_encode(lua_State* L)
{
    json_object *obj = new_json(L, 1);
    
    lua_pushstring(L, json_object_get_str(obj));
    json_object_put(obj);
    return 1;
}

/*
 * Description: 从json数组中取出数组元素并压入栈中
 */
static void push_json_array(lua_State *L, json_object *val)
{
    size_t json_len = json_object_array_length(val);
    lua_newtable(L);
    for (size_t i = 0; i < json_len; ++i) {
        push_json_value(L, json_object_array_get_idx(val, i));
        // 数组中单个元素入栈后，将压入栈的元素设置到对应的位置，此时压入栈的元素在第二个，索引为-2
        lua_rawseti(L, -2, i + 1);
    }
}

/*
 * Description: 从json对象中取出数组元素并压入栈中
 */
static void push_json_object(lua_State *L, json_object *val)
{
    lua_newtable(L);
    json_object_object_foreach(val, json_key, json_value)
    {
        lua_pushstring(L, json_key);
        push_json_value(L, json_value);
        lua_settable(L, -3); // 栈顶两个数据为table中的键值对，故table在第三位，索引为-3
    }
}

/*
 * Description: 根据不同的json数据类型, 将json数据压入栈中
 */
static void push_json_value(lua_State *L, json_object *val)
{
    json_type type = json_object_get_type(val);
    switch (type) {
        case json_type_null:
            lua_pushlightuserdata(L, NULL);
            break;
        case json_type_boolean:
            lua_pushboolean(L, json_object_get_boolean(val));
            break;
        case json_type_double:
            lua_pushnumber(L, json_object_get_double(val));
            break;
        case json_type_int:
            lua_pushinteger(L, json_object_get_int64(val));
            break;
        case json_type_object:
            push_json_object(L, val);
            break;
        case json_type_array:
            push_json_array(L, val);
            break;
        case json_type_string: {
            const gchar* s = json_object_get_str(val);
            if (s != NULL) {
                lua_pushlstring(L, s, json_object_get_string_len(val));
            } else {
                lua_pushnil(L);
            }
            break;
        }
        default:
            luaL_error(L, "Invalid json type %d", type);
            break;
    }
}

/*
 * Description: lua调用decode函数的具体实现方法，实现从json格式的字符串转成lua数据类型
 */
static int lcjson_decode(lua_State* L)
{
    const gchar* json_str = (const gchar*)luaL_checkstring(L, 1);
    enum json_tokener_error error;
    json_object* obj = json_tokener_parse_verbose(json_str, &error);
    if (error != json_tokener_success) {
        luaL_error(L, "decode error: %s", json_tokener_error_desc(error));
    }

    push_json_value(L, obj);
    json_object_put(obj);
    
    return 1;
}

/*
 * Description: 设置lua中空表是否按空对象显示
 */
static int lcjson_encode_empty_table_as_object(lua_State* L)
{
    g_encode_empty_table_as_object = lua_toboolean(L, -1);
    return 0;
}

static luaL_Reg l_cjson[] = {
    {"encode", lcjson_encode},
    {"decode", lcjson_decode},
    {"encode_empty_table_as_object", lcjson_encode_empty_table_as_object},
    {NULL, NULL}
};

LUALIB_API int luaopen_cjson(lua_State* L)
{
    luaL_register(L, "cjson", l_cjson);

    // 声明cjson.null,
    lua_pushlightuserdata(L, NULL);
    // 栈顶为空指针NULL, "null"在第二位置，索引为-2
    lua_setfield(L, -2, "null");

    return 1;
}

