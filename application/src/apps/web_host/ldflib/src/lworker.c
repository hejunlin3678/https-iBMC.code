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

#include "assert.h"
#include "common.h"
#include "worker.h"

LOCAL spinlock_t g_lock   = {0};
LOCAL worker_t*  g_worker = NULL;

LOCAL int l_worker_unref(lua_State* L)
{
    worker_t** u = luaL_checkudata(L, 1, MT_WORKER);
    if (*u == NULL) {
        return 0;
    }

    *u = NULL;

    if (unref_worker(g_worker) != 1) {
        return 0;
    }

    // 最后一个 worker 被销毁,把缓存的全局 g_worker 也销毁
    spinlock_lock(&g_lock);
    if (ref_worker_count(g_worker) == 1) {
        unref_worker(g_worker);
        g_worker = NULL;
    }
    spinlock_unlock(&g_lock);
    return 0;
}

LOCAL int l_work_unref(lua_State* L)
{
    work_t** u = luaL_checkudata(L, 1, MT_WORK);
    if (*u == NULL) {
        return 0;
    }

    work_t* work = *u;
    *u           = NULL;

    unref_work(work);
    return 0;
}

LOCAL int l_work_query(lua_State* L)
{
    work_t* work = *(work_t**)luaL_checkudata(L, 1, MT_WORK);
    if (work != NULL) {
        int ret = query_work(work, L);
        if (ret >= 0) {
            return ret;
        }
    }

    lua_pushboolean(L, 1);
    return 1;
}

LOCAL luaL_Reg l_worker[] = {
    {"__gc", l_worker_unref},
    {"unref", l_worker_unref},
    {NULL, NULL},
};

LOCAL luaL_Reg l_work[] = {
    {"__gc", l_work_unref},
    {"unref", l_work_unref},
    {"query", l_work_query},
    {NULL, NULL},
};

void init_worker(lua_State* L)
{
    // worker mt
    luaL_newmetatable(L, MT_WORKER);
#if LUA_VERSION_NUM == 501
    luaL_register(L, NULL, l_worker);
#else
    luaL_setfuncs(L, l_worker, 0);
#endif
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    lua_pop(L, 1);

    // work mt
    luaL_newmetatable(L, MT_WORK);
#if LUA_VERSION_NUM == 501
    luaL_register(L, NULL, l_work);
#else
    luaL_setfuncs(L, l_work, 0);
#endif
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    lua_pop(L, 1);
}

#define WORKER_THARD_COUNT 4
#define MAX_WORK_COUNT     256
const char* WORKER_KEY = "DFLIB_WORKER";

LOCAL int create_worker(lua_State* L)
{
    lua_pop(L, 1);
    worker_t** t = lua_newuserdata(L, sizeof(worker_t*));
    *t           = NULL;
    luaL_getmetatable(L, MT_WORKER);
    lua_setmetatable(L, -2);

    spinlock_lock(&g_lock);
    if (g_worker == NULL) {
        g_worker = new_worker(WORKER_THARD_COUNT, MAX_WORK_COUNT);
        if (g_worker == NULL) {
            spinlock_unlock(&g_lock);
            return 0;
        }
    }

    ref_worker(g_worker);
    *t = g_worker;
    spinlock_unlock(&g_lock);

    // worker 的生命周期由 lua 全局变量管理
    lua_pushvalue(L, -1);
    lua_setglobal(L, WORKER_KEY);
    return 1;
}

worker_t* get_worker(lua_State* L)
{
    lua_getglobal(L, WORKER_KEY);
    if (lua_isnil(L, -1) && create_worker(L) == 0) {
        return NULL;
    }

    worker_t** u = luaL_checkudata(L, -1, MT_WORKER);
    lua_pop(L, 1);
    return *u;
}
