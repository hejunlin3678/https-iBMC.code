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

#include "common.h"
#include "lgvariant.h"

typedef struct _sharedata {
    spinlock_t lock;
    char*      key;
    GVariant*  value;
    int        ref;
} sharedata_t;

LOCAL spinlock_t  g_lock  = {0};
LOCAL GHashTable* g_table = NULL;

// sharedata

LOCAL void sharedata_free(gpointer data)
{
    sharedata_t* s = data;
    g_assert(s->ref == 0);

    g_free(s->key);
    if (s->value != NULL) {
        g_variant_unref(s->value);
        s->value = NULL;
    }

    g_free(data);
}

LOCAL GHashTable* table(void)
{
    if (g_table == NULL) {
        g_table = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, sharedata_free);
    }

    return g_table;
}

LOCAL sharedata_t* get_sharedata(GHashTable* t, const char* key)
{
    sharedata_t* s = (sharedata_t*)g_hash_table_lookup(t, key);
    if (s == NULL) {
        return NULL;
    }

    ATOMIC_INC(&s->ref);
    return s;
}

LOCAL sharedata_t* create_sharedata(GHashTable* t, const char* key)
{
    sharedata_t* s = g_malloc(sizeof(sharedata_t));
    if (s == NULL) {
        return NULL;
    }

    char* tkey = g_strdup(key);
    if (tkey == NULL) {
        g_free(s);
        return NULL;
    }

    spinlock_init(&s->lock);
    s->key   = tkey;
    s->value = NULL;
    s->ref   = 1;

    g_hash_table_insert(t, tkey, s);
    return s;
}

LOCAL void remove_sharedata(sharedata_t* s)
{
    if (ATOMIC_DEC(&s->ref) != 0) {
        return;
    }

    spinlock_lock(&g_lock);

    // double check
    if (s->ref != 0) {
        spinlock_unlock(&g_lock);
        return;
    }

    GHashTable* t = table();
    if (t == NULL) {
        spinlock_unlock(&g_lock);
        return;
    }

    g_hash_table_remove(t, s->key);
    spinlock_unlock(&g_lock);
}

// lua function

LOCAL int lnew_sharedata(lua_State* L)
{
    const char*   key = luaL_checkstring(L, 1);
    sharedata_t** u   = lua_newuserdata(L, sizeof(sharedata_t*));

    *u = NULL;
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_setmetatable(L, -2);

    spinlock_lock(&g_lock);
    GHashTable* t = table();
    if (t == NULL) {
        spinlock_unlock(&g_lock);
        return 0;
    }

    sharedata_t* s = get_sharedata(t, key);
    if (s == NULL) {
        s = create_sharedata(t, key);
    }
    spinlock_unlock(&g_lock);

    *u = s;
    return (s == NULL) ? 0 : 1;
}

LOCAL int lget_sharedata(lua_State* L)
{
    const char*   key = luaL_checkstring(L, 1);
    sharedata_t** u   = lua_newuserdata(L, sizeof(sharedata_t*));

    *u = NULL;
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_setmetatable(L, -2);

    spinlock_lock(&g_lock);
    GHashTable* t = table();
    if (t == NULL) {
        spinlock_unlock(&g_lock);
        return 0;
    }

    sharedata_t* s = get_sharedata(t, key);
    spinlock_unlock(&g_lock);

    *u = s;
    return (s == NULL) ? 0 : 1;
}

LOCAL int l_sharedata_gc(lua_State* L)
{
    sharedata_t** u = luaL_checkudata(L, 1, MT_SHAREDATA);

    sharedata_t* s = *u;
    if (s != NULL) {
        remove_sharedata(s);
        *u = NULL;
    }

    return 0;
}

LOCAL int l_sharedata_get(lua_State* L)
{
    sharedata_t* s = *(sharedata_t**)luaL_checkudata(L, 1, MT_SHAREDATA);
    if (s == NULL) {
        return 0;
    }

    spinlock_lock(&(s->lock));

    if (s->value == NULL) {
        spinlock_unlock(&(s->lock));
        return 0;
    }

    lua_pushcfunction(L, safe_push_variant_value);
    pushlightuserdata(L, s->value);
    if (lua_pcall(L, 1, 1, 0) == LUA_OK) {
        spinlock_unlock(&(s->lock));
        return 1;
    }
    spinlock_unlock(&(s->lock));

    return lua_error(L);
}

LOCAL int l_sharedata_set(lua_State* L)
{
    sharedata_t* s = *(sharedata_t**)luaL_checkudata(L, 1, MT_SHAREDATA);
    if (s == NULL) {
        return luaL_error(L, "invalid sharedata");
    }

    GVariant* v = *(GVariant**)luaL_checkudata(L, 2, MT_GVARIANT);
    if (v == NULL) {
        return luaL_error(L, "invalid sharedata value");
    }

    spinlock_lock(&(s->lock));

    if (s->value != NULL) {
        g_variant_unref(s->value);
        s->value = NULL;
    }

    s->value = g_variant_ref(v);

    spinlock_unlock(&(s->lock));
    return 0;
}

LOCAL luaL_Reg l_sharedata_mt[] = {
    {"__gc", l_sharedata_gc},
    {"release", l_sharedata_gc},
    {"value", l_sharedata_get},
    {"update", l_sharedata_set},
    {NULL, NULL},
};

LOCAL luaL_Reg l_sharedata[] = {{"new", lnew_sharedata}, {"get", lget_sharedata}, {NULL, NULL}};

int init_sharedata(lua_State* L)
{
    lua_createtable(L, 0, sizeof(l_sharedata) / sizeof((l_sharedata)[0]) - 1);

    luaL_newmetatable(L, MT_SHAREDATA);
#if LUA_VERSION_NUM == 501
    luaL_register(L, NULL, l_sharedata_mt);
#else
    luaL_setfuncs(L, l_sharedata_mt, 0);
#endif
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

#if LUA_VERSION_NUM == 501
    luaL_openlib(L, NULL, l_sharedata, 1);
#else
    luaL_setfuncs(L, l_sharedata, 1);
#endif

    return 1;
}