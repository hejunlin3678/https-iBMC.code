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

#include <pme/common/uip_sensor_alarm.h>

#include "common.h"

// EVENT_WORK_STATE_S
LOCAL int l_EVENT_WORK_STATE_S_index(lua_State* L)
{
    EVENT_WORK_STATE_S* s   = luaL_checkudata(L, 1, MT_EVENT_WORK_STATE_S);
    const char*         key = luaL_checkstring(L, 2);
    if (strcmp(key, "disable_scanning") == 0) {
        lua_pushinteger(L, s->disable_scanning);
    } else if (strcmp(key, "disable_all") == 0) {
        lua_pushinteger(L, s->disable_all);
    } else if (strcmp(key, "disable_scanning_local") == 0) {
        lua_pushinteger(L, s->disable_scanning_local);
    } else if (strcmp(key, "disable_override") == 0) {
        lua_pushinteger(L, s->disable_override);
    } else if (strcmp(key, "disable_access_error") == 0) {
        lua_pushinteger(L, s->disable_access_error);
    } else if (strcmp(key, "initial_update_progress") == 0) {
        lua_pushinteger(L, s->initial_update_progress);
    } else if (strcmp(key, "auto_re_arm") == 0) {
        lua_pushinteger(L, s->auto_re_arm);
    } else if (strcmp(key, "ignore_if_disable") == 0) {
        lua_pushinteger(L, s->ignore_if_disable);
    } else if (strcmp(key, "bits") == 0) {
        lua_pushinteger(L, *(guint8*)s);
    } else {
        return luaL_error(L, "unknow field EVENT_WORK_STATE_S.%s", key);
    }
    return 1;
}

LOCAL int l_EVENT_WORK_STATE_S_new_index(lua_State* L)
{
    EVENT_WORK_STATE_S* s   = luaL_checkudata(L, 1, MT_EVENT_WORK_STATE_S);
    const char*         key = luaL_checkstring(L, 2);
    guint8              v   = (guint8)luaL_checkinteger(L, 3);
    if (strcmp(key, "disable_scanning") == 0) {
        s->disable_scanning = v;
    } else if (strcmp(key, "disable_all") == 0) {
        s->disable_all = v;
    } else if (strcmp(key, "disable_scanning_local") == 0) {
        s->disable_scanning_local = v;
    } else if (strcmp(key, "disable_override") == 0) {
        s->disable_override = v;
    } else if (strcmp(key, "disable_access_error") == 0) {
        s->disable_access_error = v;
    } else if (strcmp(key, "initial_update_progress") == 0) {
        s->initial_update_progress = v;
    } else if (strcmp(key, "auto_re_arm") == 0) {
        s->auto_re_arm = v;
    } else if (strcmp(key, "ignore_if_disable") == 0) {
        s->ignore_if_disable = v;
    } else if (strcmp(key, "bits") == 0) {
        *(guint8*)s = v;
    } else {
        return luaL_error(L, "unknow field EVENT_WORK_STATE_S.%s", key);
    }
    return 0;
}

// SENSOR_READINGMASK_BIT
LOCAL int l_SENSOR_READINGMASK_BIT_index(lua_State* L)
{
    SENSOR_READINGMASK_BIT* s   = luaL_checkudata(L, 1, MT_SENSOR_READINGMASK_BIT);
    const char*             key = luaL_checkstring(L, 2);
    if (strcmp(key, "readable_lower_noncritical") == 0) {
        lua_pushinteger(L, s->readable_lower_noncritical);
    } else if (strcmp(key, "readable_lower_critical") == 0) {
        lua_pushinteger(L, s->readable_lower_critical);
    } else if (strcmp(key, "readable_lower_nonrecoverable") == 0) {
        lua_pushinteger(L, s->readable_lower_nonrecoverable);
    } else if (strcmp(key, "readable_upper_noncritical") == 0) {
        lua_pushinteger(L, s->readable_upper_noncritical);
    } else if (strcmp(key, "readable_upper_critical") == 0) {
        lua_pushinteger(L, s->readable_upper_critical);
    } else if (strcmp(key, "readable_upper_nonrecoverable") == 0) {
        lua_pushinteger(L, s->readable_upper_nonrecoverable);
    } else if (strcmp(key, "reserved3") == 0) {
        lua_pushinteger(L, s->reserved3);
    } else if (strcmp(key, "settable_lower_noncritical") == 0) {
        lua_pushinteger(L, s->settable_lower_noncritical);
    } else if (strcmp(key, "settable_lower_critical") == 0) {
        lua_pushinteger(L, s->settable_lower_critical);
    } else if (strcmp(key, "settable_lower_nonrecoverable") == 0) {
        lua_pushinteger(L, s->settable_lower_nonrecoverable);
    } else if (strcmp(key, "settable_upper_noncritical") == 0) {
        lua_pushinteger(L, s->settable_upper_noncritical);
    } else if (strcmp(key, "settable_upper_critical") == 0) {
        lua_pushinteger(L, s->settable_upper_critical);
    } else if (strcmp(key, "settable_upper_nonrecoverable") == 0) {
        lua_pushinteger(L, s->settable_upper_nonrecoverable);
    } else if (strcmp(key, "reserved4") == 0) {
        lua_pushinteger(L, s->reserved4);
    } else if (strcmp(key, "bits") == 0) {
        lua_pushinteger(L, *(guint16*)s);
    } else {
        return luaL_error(L, "unknow field SENSOR_READINGMASK_BIT.%s", key);
    }
    return 1;
}

LOCAL int l_SENSOR_READINGMASK_BIT_new_index(lua_State* L)
{
    SENSOR_READINGMASK_BIT* s   = luaL_checkudata(L, 1, MT_SENSOR_READINGMASK_BIT);
    const char*             key = luaL_checkstring(L, 2);
    guint16                 v   = (guint16)luaL_checkinteger(L, 3);
    if (strcmp(key, "readable_lower_noncritical") == 0) {
        s->readable_lower_noncritical = v;
    } else if (strcmp(key, "readable_lower_critical") == 0) {
        s->readable_lower_critical = v;
    } else if (strcmp(key, "readable_lower_nonrecoverable") == 0) {
        s->readable_lower_nonrecoverable = v;
    } else if (strcmp(key, "readable_upper_noncritical") == 0) {
        s->readable_upper_noncritical = v;
    } else if (strcmp(key, "readable_upper_critical") == 0) {
        s->readable_upper_critical = v;
    } else if (strcmp(key, "readable_upper_nonrecoverable") == 0) {
        s->readable_upper_nonrecoverable = v;
    } else if (strcmp(key, "reserved3") == 0) {
        s->reserved3 = v;
    } else if (strcmp(key, "settable_lower_noncritical") == 0) {
        s->settable_lower_noncritical = v;
    } else if (strcmp(key, "settable_lower_critical") == 0) {
        s->settable_lower_critical = v;
    } else if (strcmp(key, "settable_lower_nonrecoverable") == 0) {
        s->settable_lower_nonrecoverable = v;
    } else if (strcmp(key, "settable_upper_noncritical") == 0) {
        s->settable_upper_noncritical = v;
    } else if (strcmp(key, "settable_upper_critical") == 0) {
        s->settable_upper_critical = v;
    } else if (strcmp(key, "settable_upper_nonrecoverable") == 0) {
        s->settable_upper_nonrecoverable = v;
    } else if (strcmp(key, "reserved4") == 0) {
        s->reserved4 = v;
    } else if (strcmp(key, "bits") == 0) {
        *(guint16*)s = v;
    } else {
        return luaL_error(L, "unknow field SENSOR_READINGMASK_BIT.%s", key);
    }
    return 0;
}

// SENSOR_STATE_BIT_T
LOCAL int l_SENSOR_STATE_BIT_T_index(lua_State* L)
{
    SENSOR_STATE_BIT_T* s   = luaL_checkudata(L, 1, MT_SENSOR_STATE_BIT_T);
    const char*         key = luaL_checkstring(L, 2);
    if (strcmp(key, "assert_nonrecoverable_ugh") == 0) {
        lua_pushinteger(L, s->assert_nonrecoverable_ugh);
    } else if (strcmp(key, "assert_nonrecoverable_ugl") == 0) {
        lua_pushinteger(L, s->assert_nonrecoverable_ugl);
    } else if (strcmp(key, "assert_critical_ugh") == 0) {
        lua_pushinteger(L, s->assert_critical_ugh);
    } else if (strcmp(key, "assert_critical_ugl") == 0) {
        lua_pushinteger(L, s->assert_critical_ugl);
    } else if (strcmp(key, "assert_noncritical_ugh") == 0) {
        lua_pushinteger(L, s->assert_noncritical_ugh);
    } else if (strcmp(key, "assert_noncritical_ugl") == 0) {
        lua_pushinteger(L, s->assert_noncritical_ugl);
    } else if (strcmp(key, "assert_nonrecoverable_lgh") == 0) {
        lua_pushinteger(L, s->assert_nonrecoverable_lgh);
    } else if (strcmp(key, "assert_nonrecoverable_lgl") == 0) {
        lua_pushinteger(L, s->assert_nonrecoverable_lgl);
    } else if (strcmp(key, "assert_critical_lgh") == 0) {
        lua_pushinteger(L, s->assert_critical_lgh);
    } else if (strcmp(key, "assert_critical_lgl") == 0) {
        lua_pushinteger(L, s->assert_critical_lgl);
    } else if (strcmp(key, "assert_noncritical_lgh") == 0) {
        lua_pushinteger(L, s->assert_noncritical_lgh);
    } else if (strcmp(key, "assert_noncritical_lgl") == 0) {
        lua_pushinteger(L, s->assert_noncritical_lgl);
    } else if (strcmp(key, "bits") == 0) {
        lua_pushinteger(L, *(guint16*)s);
    } else {
        return luaL_error(L, "unknow field SENSOR_STATE_BIT_T.%s", key);
    }
    return 1;
}

LOCAL int l_SENSOR_STATE_BIT_T_new_index(lua_State* L)
{
    SENSOR_STATE_BIT_T* s   = luaL_checkudata(L, 1, MT_SENSOR_STATE_BIT_T);
    const char*         key = luaL_checkstring(L, 2);
    guint16             v   = (guint16)luaL_checkinteger(L, 3);
    if (strcmp(key, "assert_nonrecoverable_ugh") == 0) {
        s->assert_nonrecoverable_ugh = v;
    } else if (strcmp(key, "assert_nonrecoverable_ugl") == 0) {
        s->assert_nonrecoverable_ugl = v;
    } else if (strcmp(key, "assert_critical_ugh") == 0) {
        s->assert_critical_ugh = v;
    } else if (strcmp(key, "assert_critical_ugl") == 0) {
        s->assert_critical_ugl = v;
    } else if (strcmp(key, "assert_noncritical_ugh") == 0) {
        s->assert_noncritical_ugh = v;
    } else if (strcmp(key, "assert_noncritical_ugl") == 0) {
        s->assert_noncritical_ugl = v;
    } else if (strcmp(key, "assert_nonrecoverable_lgh") == 0) {
        s->assert_nonrecoverable_lgh = v;
    } else if (strcmp(key, "assert_nonrecoverable_lgl") == 0) {
        s->assert_nonrecoverable_lgl = v;
    } else if (strcmp(key, "assert_critical_lgh") == 0) {
        s->assert_critical_lgh = v;
    } else if (strcmp(key, "assert_critical_lgl") == 0) {
        s->assert_critical_lgl = v;
    } else if (strcmp(key, "assert_noncritical_lgh") == 0) {
        s->assert_noncritical_lgh = v;
    } else if (strcmp(key, "assert_noncritical_lgl") == 0) {
        s->assert_noncritical_lgl = v;
    } else if (strcmp(key, "bits") == 0) {
        *(guint16*)s = v;
    } else {
        return luaL_error(L, "unknow field SENSOR_STATE_BIT_T.%s", key);
    }
    return 0;
}

// SENSOR_CAPABILITY_S
LOCAL int l_SENSOR_CAPABILITY_S_index(lua_State* L)
{
    SENSOR_CAPABILITY_S* s   = luaL_checkudata(L, 1, MT_SENSOR_CAPABILITY_S);
    const char*          key = luaL_checkstring(L, 2);
    if (strcmp(key, "event_msg_support") == 0) {
        lua_pushinteger(L, s->event_msg_support);
    } else if (strcmp(key, "threshold_access_support") == 0) {
        lua_pushinteger(L, s->threshold_access_support);
    } else if (strcmp(key, "hysteresis_support") == 0) {
        lua_pushinteger(L, s->hysteresis_support);
    } else if (strcmp(key, "auto_rearm_support") == 0) {
        lua_pushinteger(L, s->auto_rearm_support);
    } else if (strcmp(key, "ignore_support") == 0) {
        lua_pushinteger(L, s->ignore_support);
    } else if (strcmp(key, "bits") == 0) {
        lua_pushinteger(L, *(guint8*)s);
    } else {
        return luaL_error(L, "unknow field SENSOR_CAPABILITY_S.%s", key);
    }
    return 1;
}

LOCAL int l_SENSOR_CAPABILITY_S_new_index(lua_State* L)
{
    SENSOR_CAPABILITY_S* s   = luaL_checkudata(L, 1, MT_SENSOR_CAPABILITY_S);
    const char*          key = luaL_checkstring(L, 2);
    guint16              v   = (guint16)luaL_checkinteger(L, 3);
    if (strcmp(key, "event_msg_support") == 0) {
        s->event_msg_support = v;
    } else if (strcmp(key, "threshold_access_support") == 0) {
        s->threshold_access_support = v;
    } else if (strcmp(key, "hysteresis_support") == 0) {
        s->hysteresis_support = v;
    } else if (strcmp(key, "auto_rearm_support") == 0) {
        s->auto_rearm_support = v;
    } else if (strcmp(key, "ignore_support") == 0) {
        s->ignore_support = v;
    } else if (strcmp(key, "bits") == 0) {
        *(guint8*)s = v;
    } else {
        return luaL_error(L, "unknow field SENSOR_CAPABILITY_S.%s", key);
    }
    return 0;
}

void init_uip_sensor_alarm(lua_State* L)
{
    luaL_newmetatable(L, MT_EVENT_WORK_STATE_S);
    lua_pushliteral(L, "__index");
    lua_pushcfunction(L, l_EVENT_WORK_STATE_S_index);
    lua_settable(L, -3);
    lua_pushliteral(L, "__newindex");
    lua_pushcfunction(L, l_EVENT_WORK_STATE_S_new_index);
    lua_settable(L, -3);
    lua_pop(L, 1);

    luaL_newmetatable(L, MT_SENSOR_READINGMASK_BIT);
    lua_pushliteral(L, "__index");
    lua_pushcfunction(L, l_SENSOR_READINGMASK_BIT_index);
    lua_settable(L, -3);
    lua_pushliteral(L, "__newindex");
    lua_pushcfunction(L, l_SENSOR_READINGMASK_BIT_new_index);
    lua_settable(L, -3);
    lua_pop(L, 1);

    luaL_newmetatable(L, MT_SENSOR_STATE_BIT_T);
    lua_pushliteral(L, "__index");
    lua_pushcfunction(L, l_SENSOR_STATE_BIT_T_index);
    lua_settable(L, -3);
    lua_pushliteral(L, "__newindex");
    lua_pushcfunction(L, l_SENSOR_STATE_BIT_T_new_index);
    lua_settable(L, -3);
    lua_pop(L, 1);

    luaL_newmetatable(L, MT_SENSOR_CAPABILITY_S);
    lua_pushliteral(L, "__index");
    lua_pushcfunction(L, l_SENSOR_CAPABILITY_S_index);
    lua_settable(L, -3);
    lua_pushliteral(L, "__newindex");
    lua_pushcfunction(L, l_SENSOR_CAPABILITY_S_new_index);
    lua_settable(L, -3);
    lua_pop(L, 1);
}
