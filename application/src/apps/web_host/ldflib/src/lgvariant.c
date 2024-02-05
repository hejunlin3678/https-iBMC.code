/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 *
 * this file licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 * http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author: zhangjianping
 * Create: 2020-12-25 06:48:47
 * Description: export libdflib to lua
 */

#include "lgvariant.h"
#include <securec.h>

LOCAL GVariant** new_variant(lua_State* L)
{
    GVariant** u = lua_newuserdata(L, sizeof(GVariant*));
    *u           = NULL;
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_setmetatable(L, -2);
    return u;
}

LOCAL int lg_variant_new_byte(lua_State* L)
{
    guchar     val = (guchar)luaL_checkinteger(L, 1);
    GVariant** u   = new_variant(L);

    *u = g_variant_new_byte(val);
    if (*u == NULL) {
        return luaL_error(L, "g_variant_new_byte failed");
    }

    return 1;
}

LOCAL int lg_variant_new_int16(lua_State* L)
{
    gint16     val = (gint16)luaL_checkinteger(L, 1);
    GVariant** u   = new_variant(L);

    *u = g_variant_new_int16(val);
    if (*u == NULL) {
        return luaL_error(L, "g_variant_new_int16 failed");
    }

    return 1;
}

LOCAL int lg_variant_new_uint16(lua_State* L)
{
    guint16    val = (guint16)luaL_checkinteger(L, 1);
    GVariant** u   = new_variant(L);

    *u = g_variant_new_uint16(val);
    if (*u == NULL) {
        return luaL_error(L, "g_variant_new_uint16 failed");
    }

    return 1;
}

LOCAL int lg_variant_new_int32(lua_State* L)
{
    gint32     val = (gint32)luaL_checkinteger(L, 1);
    GVariant** u   = new_variant(L);

    *u = g_variant_new_int32(val);
    if (*u == NULL) {
        return luaL_error(L, "g_variant_new_int32 failed");
    }

    return 1;
}

LOCAL int lg_variant_new_uint32(lua_State* L)
{
    guint32    val = (guint32)luaL_checkinteger(L, 1);
    GVariant** u   = new_variant(L);

    *u = g_variant_new_uint32(val);
    if (*u == NULL) {
        return luaL_error(L, "g_variant_new_uint32 failed");
    }

    return 1;
}

LOCAL int lg_variant_new_double(lua_State* L)
{
    gdouble    val = (gdouble)luaL_checknumber(L, 1);
    GVariant** u   = new_variant(L);

    *u = g_variant_new_double(val);
    if (*u == NULL) {
        return luaL_error(L, "g_variant_new_double failed");
    }

    return 1;
}

LOCAL int lg_variant_new_string(lua_State* L)
{
    const char* val = luaL_checkstring(L, 1);
    GVariant**  u   = new_variant(L);

    *u = g_variant_new_string(val);
    if (*u == NULL) {
        return luaL_error(L, "g_variant_new_string failed");
    }

    return 1;
}

LOCAL int lg_variant_new_byte_string(lua_State* L)
{
    size_t size;
    const char* val = luaL_checklstring(L, 1, &size);
    GVariant**  u   = new_variant(L);

    *u = g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, val, size, 1);
    if (*u == NULL) {
        return luaL_error(L, "g_variant_new_fixed_array failed");
    }

    return 1;
}

LOCAL int lg_variant_new_vstring(lua_State* L)
{
#define MAX_NEW_VSTRING_LENGTH 256
    luaL_checktype(L, 1, LUA_TTABLE);
#if LUA_VERSION_NUM == 501
    gssize len = (gssize)lua_objlen(L, 1);
#else
    gssize len = (gssize)lua_rawlen(L, 1);
#endif
    if (len > MAX_NEW_VSTRING_LENGTH) {
        return luaL_error(L, "lg_variant_new_vstring failed: too large array");
    }

    const gchar* strv[MAX_NEW_VSTRING_LENGTH] = {NULL};
    for (int i = 0; i < len; ++i) {
        lua_rawgeti(L, 1, i + 1);
        strv[i] = luaL_checkstring(L, -1);
        lua_pop(L, 1);
    }
    GVariant** u = new_variant(L);

    *u = g_variant_new_strv(strv, len);
    if (*u == NULL) {
        return luaL_error(L, "lg_variant_new_vstring failed");
    }

    return 1;
}

LOCAL int lg_variant_new_bool(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    gboolean   val = (gboolean)lua_toboolean(L, 1);
    GVariant** u   = new_variant(L);

    *u = g_variant_new_boolean(val);
    if (*u == NULL) {
        return luaL_error(L, "g_variant_new_boolean failed");
    }

    return 1;
}

LOCAL size_t fixed_size(lua_State* L, const GVariantType* t)
{
    if (g_variant_type_is_subtype_of(t, G_VARIANT_TYPE_BYTE)) {
        return sizeof(guint8);
    } else if (g_variant_type_is_subtype_of(t, G_VARIANT_TYPE_INT16)) {
        return sizeof(gint16);
    } else if (g_variant_type_is_subtype_of(t, G_VARIANT_TYPE_UINT16)) {
        return sizeof(guint16);
    } else if (g_variant_type_is_subtype_of(t, G_VARIANT_TYPE_INT32)) {
        return sizeof(gint32);
    } else if (g_variant_type_is_subtype_of(t, G_VARIANT_TYPE_UINT32)) {
        return sizeof(guint32);
    } else if (g_variant_type_is_subtype_of(t, G_VARIANT_TYPE_DOUBLE)) {
        return sizeof(gdouble);
    } else if (g_variant_type_is_subtype_of(t, G_VARIANT_TYPE_BOOLEAN)) {
        return sizeof(guchar);
    } else {
        return luaL_error(L, "array does not fixed size");
    }
}

LOCAL int lg_variant_new_fixed_array(lua_State* L)
{
    errno_t secure_rv;
    luaL_checktype(L, 1, LUA_TTABLE);
#if LUA_VERSION_NUM == 501
    size_t len = lua_objlen(L, 1);
#else
    size_t len = lua_rawlen(L, 1);
#endif
    if (len == 0) {
        return luaL_error(L, "g_variant_new_fixed_array: param not allow empty");
    }

    lua_rawgeti(L, 1, 1);
    GVariant**          pelement = luaL_checkudata(L, -1, MT_GVARIANT);
    const GVariantType* t        = g_variant_get_type(*pelement);
    const gchar*        t_str    = g_variant_get_type_string(*pelement);
    lua_pop(L, 1);

    size_t element_size = fixed_size(L, t);
    void*  elements     = lua_newuserdata(L, element_size * len);

    guint8* p = elements;
    for (size_t i = 1; i <= len; ++i) {
        lua_rawgeti(L, 1, i);
        GVariant** v = luaL_checkudata(L, -1, MT_GVARIANT);
        if (!g_variant_is_of_type(*v, t)) {
            return luaL_error(L, "g_variant_new_fixed_array: type not match, expected %s, got %s", t_str,
                              g_variant_get_type_string(*v));
        }
        secure_rv = memcpy_s(p, element_size, g_variant_get_data(*v), element_size);
        if (secure_rv != EOK) {
            return luaL_error(L, "g_variant_new_fixed_array: memcpy_s fail, ret = %d", secure_rv);
        }
        p += element_size;
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    GVariant** u = new_variant(L);

    *u = g_variant_new_fixed_array(t, elements, len, element_size);
    if (*u == NULL) {
        return luaL_error(L, "g_variant_new_fixed_array failed");
    }

    return 1;
}

LOCAL int lg_variant_unref(lua_State* L)
{
    GVariant** u = luaL_checkudata(L, 1, MT_GVARIANT);
    if (*u == NULL) {
        return 0;
    }

    g_variant_unref(*u);
    *u = NULL;
    return 0;
}

LOCAL int lg_variant_value(lua_State* L)
{
    GVariant** v = luaL_checkudata(L, 1, MT_GVARIANT);
    push_variant_value(L, *v);
    return 1;
}

LOCAL int lg_variant_type(lua_State* L)
{
    GVariant**   v = luaL_checkudata(L, 1, MT_GVARIANT);
    const gchar* t = g_variant_get_type_string(*v);
    lua_pushstring(L, t);
    return 1;
}

LOCAL int safe_push_stringv_value(lua_State* L)
{
    const gchar** s = (const gchar**)tolightuserdata(L, 1);
    int           n = (int)lua_tointeger(L, 2);
    lua_createtable(L, n, 0);
    for (gsize i = 0; i < n; i++) {
        lua_pushstring(L, s[i]);
        lua_rawseti(L, -2, i + 1);
    }
    return 1;
}

int safe_push_variant_value(lua_State* L)
{
    GVariant* val = (GVariant*)tolightuserdata(L, 1);
    push_variant_value(L, val);
    return 1;
}

void push_variant_value(lua_State* L, GVariant* v)
{
    if (g_variant_is_of_type(v, G_VARIANT_TYPE_BYTE)) {
        lua_pushinteger(L, g_variant_get_byte(v));
    } else if (g_variant_is_of_type(v, G_VARIANT_TYPE_INT16)) {
        lua_pushinteger(L, g_variant_get_int16(v));
    } else if (g_variant_is_of_type(v, G_VARIANT_TYPE_UINT16)) {
        lua_pushinteger(L, g_variant_get_uint16(v));
    } else if (g_variant_is_of_type(v, G_VARIANT_TYPE_INT32)) {
        lua_pushinteger(L, g_variant_get_int32(v));
    } else if (g_variant_is_of_type(v, G_VARIANT_TYPE_UINT32)) {
        lua_pushinteger(L, g_variant_get_uint32(v));
    } else if (g_variant_is_of_type(v, G_VARIANT_TYPE_DOUBLE)) {
        lua_pushnumber(L, g_variant_get_double(v));
    } else if (g_variant_is_of_type(v, G_VARIANT_TYPE_STRING)) {
        gsize        len = 0;
        const gchar* s   = g_variant_get_string(v, &len);
        lua_pushlstring(L, s, len);
    } else if (g_variant_is_of_type(v, G_VARIANT_TYPE_BOOLEAN)) {
        lua_pushboolean(L, g_variant_get_boolean(v) ? 1 : 0);
    } else if (g_variant_is_of_type(v, G_VARIANT_TYPE_STRING_ARRAY)) {
        gsize         n = 0;
        const gchar** s = g_variant_get_strv(v, &n);
        lua_pushcfunction(L, safe_push_stringv_value);
        pushlightuserdata(L, s);
        lua_pushinteger(L, (lua_Integer)n);
        if (lua_pcall(L, 2, 1, 0) != LUA_OK) {
            g_free(s);
            lua_error(L);
            return;
        }
        g_free(s);
    } else if (g_variant_is_container(v)) {
        gsize n = g_variant_n_children(v);
        lua_createtable(L, n, 0);
        for (gsize i = 0; i < n; i++) {
            GVariant* child = g_variant_get_child_value(v, i);
            lua_pushcfunction(L, safe_push_variant_value);
            pushlightuserdata(L, child);
            if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
                g_variant_unref(child);
                lua_error(L);
                return;
            }
            g_variant_unref(child);
            lua_rawseti(L, -2, i + 1);
        }
    } else {
        lua_pushnil(L);
    }
}

LOCAL int lis_gvariant(lua_State* L)
{
    int ret = 0;
    if (lua_isuserdata(L, 1) == 1 && lua_getmetatable(L, 1) == 1) {
        lua_pushvalue(L, lua_upvalueindex(1));
        ret = lua_rawequal(L, -1, -2);
        lua_pop(L, 2);
    }
    lua_pushboolean(L, ret);
    return 1;
}

LOCAL int lpush_raw_gvariant(lua_State* L)
{
    GVariant* v = (GVariant*)luaL_checkinteger(L, 1);
    push_variant_value(L, v);
    return 1;
}

LOCAL luaL_Reg l_g_variant[] = {
    {"__gc", lg_variant_unref},
    {"unref", lg_variant_unref},
    {"value", lg_variant_value},
    {"type", lg_variant_type},
    {NULL, NULL},
};

LOCAL luaL_Reg l[] = {
    {"new_byte", lg_variant_new_byte},
    {"new_int16", lg_variant_new_int16},
    {"new_uint16", lg_variant_new_uint16},
    {"new_int32", lg_variant_new_int32},
    {"new_uint32", lg_variant_new_uint32},
    {"new_double", lg_variant_new_double},
    {"new_string", lg_variant_new_string},
    {"new_byte_string", lg_variant_new_byte_string},
    {"new_vstring", lg_variant_new_vstring},
    {"new_bool", lg_variant_new_bool},
    {"new_fixed_array", lg_variant_new_fixed_array},
    {"is_gvariant", lis_gvariant},
    {"push_raw_gvariant", lpush_raw_gvariant},
    {NULL, NULL},
};

int init_gvariant(lua_State* L)
{
    lua_createtable(L, 0, sizeof(l) / sizeof((l)[0]) - 1);

    luaL_newmetatable(L, MT_GVARIANT);
#if LUA_VERSION_NUM == 501
    luaL_register(L, NULL, l_g_variant);
#else
    luaL_setfuncs(L, l_g_variant, 0);
#endif
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

#if LUA_VERSION_NUM == 501
    luaL_openlib(L, NULL, l, 1);
#else
    luaL_setfuncs(L, l, 1);
#endif
    return 1;
}
