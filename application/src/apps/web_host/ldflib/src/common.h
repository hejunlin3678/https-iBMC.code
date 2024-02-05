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

#ifndef __COMMON_H__
#define __COMMON_H__
#include <glib.h>
#include <lauxlib.h>
#include <lua.h>

#if LUA_VERSION_NUM != 503
#define LUA_OK 0
#endif

#define LOCAL static

#define DFL_OK                  (0)
#define VOS_OK                  (0)
#define RFERR_SUCCESS           (0x000000 | 200)
#define ERRCODE_WRONG_PARAMETER (-2001)

#define MT_GVARIANT               "mt_g_variant"
#define MT_EVENT_WORK_STATE_S     "mt_EVENT_WORK_STATE_S"
#define MT_SENSOR_READINGMASK_BIT "mt_SENSOR_READINGMASK_BIT"
#define MT_SENSOR_CAPABILITY_S    "mt_SENSOR_CAPABILITY_S"
#define MT_SENSOR_STATE_BIT_T     "mt_SENSOR_STATE_BIT_T"
#define MT_WORKER                 "mt_worker"
#define MT_WORK                   "mt_work"
#define MT_SHAREDATA              "mt_sharedata"
#define MAX_TIME_STRING_LEN       128
#define TIMEZONE_MAX_LEN          48

#if defined(LUA_JROOT) && (LONG_MAX > ((1UL << 31) - 1))
static inline void pushlightuserdata(lua_State* L, void* p)
{
    lua_pushinteger(L, (lua_Integer)(p));
}

static inline void* tolightuserdata(lua_State* L, int idx)
{
    return (void*)lua_tointeger(L, idx);
}
#else
static inline void pushlightuserdata(lua_State* L, void* p)
{
    lua_pushlightuserdata(L, p);
}

static inline void* tolightuserdata(lua_State* L, int idx)
{
    return lua_touserdata(L, idx);
}
#endif

typedef struct _spinlock {
    int v;
} spinlock_t;

static inline void spinlock_init(spinlock_t* lock)
{
    lock->v = 0;
}

static inline void spinlock_lock(spinlock_t* lock)
{
    while (__sync_lock_test_and_set(&(lock->v), 1)) {
    }
}

static inline void spinlock_unlock(spinlock_t* lock)
{
    __sync_lock_release(&(lock->v));
}

static inline int ATOMIC_INC(int* p)
{
    return __sync_add_and_fetch(p, 1);
}

static inline int ATOMIC_DEC(int* p)
{
    return __sync_sub_and_fetch(p, 1);
}

#endif /* __COMMON_H__ */