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

#define LUA_LIB

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pme/common/mscm_vos.h>
#include <pme/common/uip_sensor_alarm.h>
#include <pme/dflib/dflapi.h>
#include <pme/dflib/dflstatic.h>
#include <stdio.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <securec.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

#include "common.h"
#include "lgvariant.h"
#include "worker.h"

#define MAX_FILEPATH_LENGTH 256
#define MAX_TOKEN_LENGTH    64

enum LUA_PARAMETER_LIST {
    LUA_PARAM_LIST_1 = 1,
    LUA_PARAM_LIST_2,
    LUA_PARAM_LIST_3,
    LUA_PARAM_LIST_4,
    LUA_PARAM_LIST_5,
    LUA_PARAM_LIST_6,
    LUA_PARAM_LIST_7,
    LUA_PARAM_LIST_8,
};

enum RET_CODE_LIST {
    RET_CODE_ERR = -1,
    RET_CODE_0,
    RET_CODE_1,
    RET_CODE_2,
    RET_CODE_3,
};

void      init_uip_sensor_alarm(lua_State* L);
int       init_sharedata(lua_State* L);
void      init_worker(lua_State* L);
worker_t* get_worker(lua_State* L);

LOCAL int ldfl_init(lua_State* L)
{
    const char* module_name = luaL_checkstring(L, 1);
    lua_pushboolean(L, dfl_init(module_name) == DFL_OK ? 1 : 0);
    return 1;
}

LOCAL int ldfl_get_object_handle(lua_State* L)
{
    OBJ_HANDLE  handle = 0;
    const char* name   = luaL_checkstring(L, 1);
    gint32      ret    = dfl_get_object_handle(name, &handle);
    lua_pushinteger(L, ret);
    if (ret != DFL_OK) {
        return 1;
    }

    lua_pushinteger(L, handle);
    return 2;
}

LOCAL int _ldfl_get_object_list(lua_State* L)
{
    GSList* list = (GSList*)tolightuserdata(L, 1);
    lua_newtable(L);
    int i = 1;
    for (GSList* iter = list; iter != NULL; iter = g_slist_next(iter)) {
        OBJ_HANDLE object_handle = (intptr_t)iter->data;
        lua_pushinteger(L, object_handle);
        lua_rawseti(L, -2, i++);
    }
    return 1;
}

LOCAL int ldfl_get_object_list(lua_State* L)
{
    GSList*     list = NULL;
    const char* name = luaL_checkstring(L, 1);
    gint32      ret  = dfl_get_object_list(name, &list);
    lua_pushinteger(L, ret);
    if (ret != DFL_OK) {
        return 1;
    }

    lua_pushcfunction(L, _ldfl_get_object_list);
    pushlightuserdata(L, list);
    if (lua_pcall(L, 1, 1, 0) == LUA_OK) {
        g_slist_free(list);
        return 2;
    }

    g_slist_free(list);
    // 将错误信息替换之前 push 的 ret 值
    lua_replace(L, -2);
    return 1;
}

LOCAL int ldfl_get_object_handle_nth(lua_State* L)
{
    GSList*     list = NULL;
    const char* name = luaL_checkstring(L, 1);
    guint       n    = (guint)luaL_checkinteger(L, 2);
    gint32      ret  = dfl_get_object_list(name, &list);
    lua_pushinteger(L, ret);
    if (ret != DFL_OK) {
        return 1;
    }

    GSList* obj_list_nth = g_slist_nth(list, n);
    if (obj_list_nth == NULL) {
        g_slist_free(list);
        return 1;
    }

    OBJ_HANDLE object_handle = (intptr_t)obj_list_nth->data;
    g_slist_free(list);
    lua_pushinteger(L, object_handle);
    return 2;
}

LOCAL int ldal_get_property_value(lua_State* L)
{
    GVariant*   val           = NULL;
    OBJ_HANDLE  handle        = (OBJ_HANDLE)luaL_checkinteger(L, 1);
    const char* property_name = luaL_checkstring(L, 2);
    gint32      ret           = dfl_get_property_value(handle, property_name, &val);
    lua_pushinteger(L, ret);
    if (ret != DFL_OK) {
        return 1;
    }

    lua_pushcfunction(L, safe_push_variant_value);
    pushlightuserdata(L, val);
    if (lua_pcall(L, 1, 1, 0) == LUA_OK) {
        g_variant_unref(val);
        return 2;
    }

    g_variant_unref(val);
    // 将错误信息替换之前 push 的 ret 值
    lua_replace(L, -2);
    return 1;
}

LOCAL int ldal_get_timestamp(lua_State* L)
{
    lua_pushinteger(L, vos_get_cur_time_stamp());
    return 1;
}

LOCAL int ldal_get_file_accessible(lua_State* L)
{
    const char* file_name = luaL_checkstring(L, 1);
    lua_pushboolean(L, vos_get_file_accessible(file_name));
    return 1;
}

LOCAL int lvos_get_file_length(lua_State* L)
{
    const char* file_name = luaL_checkstring(L, 1);
    lua_pushnumber(L, vos_get_file_length(file_name));
    return 1;
}

LOCAL int lvos_rm_filepath(lua_State* L)
{
    const char* path = luaL_checkstring(L, 1);
    lua_pushboolean(L, vos_rm_filepath(path));
    return 1;
}

LOCAL int lvos_file_copy(lua_State* L)
{
    const char* desfilepath = luaL_checkstring(L, 1);
    const char* sourcepath  = luaL_checkstring(L, 2);
    lua_pushinteger(L, vos_file_copy(desfilepath, sourcepath));
    return 1;
}

#define MAX_SYSTEM_PARAM_COUNT 100
LOCAL int lvos_system_s(lua_State* L)
{
    const char* path = luaL_checkstring(L, 1);
    int         argv = lua_gettop(L) - 1;
    if (argv >= MAX_SYSTEM_PARAM_COUNT) {
        return luaL_error(L, "too many args");
    }

    const char* args[MAX_SYSTEM_PARAM_COUNT + 1] = {NULL};
    args[0] = path;
    for (int i = 1; i <= argv; i++) {
        args[i] = luaL_checkstring(L, i + 1);
    }
    args[argv + 1] = NULL;

    lua_pushinteger(L, vos_system_s(path, args));
    return 1;
}

LOCAL int l_file_stat(lua_State* L)
{
    struct stat file_stat;
    const char* path = luaL_checkstring(L, 1);
    int         ret  = stat(path, &file_stat);
    if (ret != 0) {
        return luaL_error(L, "stat failed: %d", ret);
    }

    lua_newtable(L);
    lua_pushinteger(L, file_stat.st_ctime);
    lua_setfield(L, -2, "st_ctime");
    lua_pushinteger(L, file_stat.st_uid);
    lua_setfield(L, -2, "st_uid");
    return 1;
}

LOCAL int is_parent_path(const char* n)
{
    if (*n++ != '.') {
        return 0;
    }

    char ch = *n++;
    return ch == '\0' || (ch == '.' && *n == '\0');
}

LOCAL int l_raw_file_list(lua_State* L)
{
    DIR* dir       = (DIR*)tolightuserdata(L, 1);
    int  only_file = lua_toboolean(L, 2);

    int i = 1;
    lua_newtable(L);
    struct dirent* entry = NULL;
    while ((entry = readdir(dir)) != NULL) {
        const char* n = entry->d_name;
        if (is_parent_path(n)) {
            continue;
        }

        if (only_file == 1 && entry->d_type == DT_DIR) {
            continue;
        }

        lua_pushstring(L, n);
        lua_rawseti(L, -2, i++);
    }
    return 1;
}

LOCAL int l_file_list(lua_State* L)
{
    const char* path = luaL_checkstring(L, 1);

    int only_file = 0;
    if (lua_gettop(L) > 1) {
        luaL_checktype(L, 2, LUA_TBOOLEAN);
        only_file = lua_toboolean(L, 2);
    }

    DIR* dir = opendir(path);
    if (dir == NULL) {
        return luaL_error(L, "opendir error");
    }

    lua_pushcfunction(L, l_raw_file_list);
    pushlightuserdata(L, dir);
    lua_pushboolean(L, only_file);
    if (lua_pcall(L, 2, 1, 0) != LUA_OK) {
        closedir(dir);
        return lua_error(L);
    }

    closedir(dir);
    return 1;
}

LOCAL int l_statvfs(lua_State* L)
{
    struct statvfs vfs;
    const char*    path = luaL_checkstring(L, 1);

    int ret = statvfs(path, &vfs);
    if (ret != 0) {
        return luaL_error(L, "statvfs error: %d", ret);
    }

    unsigned long int block_size = vfs.f_bsize;

    lua_createtable(L, 0, 3);
    lua_pushnumber(L, vfs.f_blocks * block_size);
    lua_setfield(L, -2, "total_size");
    lua_pushnumber(L, vfs.f_bfree * block_size);
    lua_setfield(L, -2, "free_size");
    lua_pushnumber(L, vfs.f_bavail * block_size);
    lua_setfield(L, -2, "avail_size");
    return 1;
}

LOCAL gint16 _ldal_timestamp_offset(gulong time_stamp, struct tm* local_tm)
{
    struct tm local_tm_;
    if (local_tm == NULL) {
        local_tm = &local_tm_;
    }

    tzset();
    localtime_r((time_t*)&time_stamp, local_tm);
    return local_tm->tm_gmtoff / 60;
}

LOCAL int ldal_timestamp_offset(lua_State* L)
{
    struct tm local_tm;
    gulong    time_stamp = lua_gettop(L) >= 1 ? (gulong)luaL_checkinteger(L, 1) : vos_get_cur_time_stamp();
    lua_pushinteger(L, _ldal_timestamp_offset(time_stamp, &local_tm));
    return 1;
}

LOCAL int ldal_datetime(lua_State* L)
{
    struct tm temp_tm;
    gint32    ret;
    gulong    tmp_time = lua_gettop(L) >= 1 ? (gulong)luaL_checkinteger(L, 1) : vos_get_cur_time_stamp();
    (void)_ldal_timestamp_offset(tmp_time, &temp_tm);

    gint16 time_zone = _ldal_timestamp_offset(vos_get_cur_time_stamp(), NULL);

    char time_zone_str[TIMEZONE_MAX_LEN + 1];
    if ((time_zone < 0) && (time_zone > -60)) {
        ret = snprintf_s(time_zone_str, TIMEZONE_MAX_LEN, TIMEZONE_MAX_LEN - 1, "-00:%02d", (int)abs(time_zone % 60));
    } else {
        ret = snprintf_s(time_zone_str, TIMEZONE_MAX_LEN, TIMEZONE_MAX_LEN - 1, "%+03d:%02d", (int)(time_zone / 60),
                         (int)abs(time_zone % 60));
    }
    if (ret <= 0) {
        return luaL_error(L, "ldal_datetime: snprintf_s fail, ret = %d", ret);
    }
    time_zone_str[TIMEZONE_MAX_LEN] = 0;

    char time_string[MAX_TIME_STRING_LEN + 1];
    (void)strftime(time_string, MAX_TIME_STRING_LEN, "%Y-%m-%dT%T", &temp_tm);
    time_string[MAX_TIME_STRING_LEN] = 0;

    char buffer[MAX_TIME_STRING_LEN + TIMEZONE_MAX_LEN + 1];
    ret = snprintf_truncated_s(buffer, sizeof(buffer), "%s%s", time_string, time_zone_str);
    if (ret <= 0) {
        return luaL_error(L, "ldal_datetime: snprintf_truncated_s fail, ret = %d", ret);
    }
    lua_pushlstring(L, buffer, strlen(buffer));
    return 1;
}

LOCAL int timeout_lua_gettime(lua_State* L)
{
    struct timeval v;
    gettimeofday(&v, (struct timezone*)NULL);
    /* Unix Epoch time (time since January 1, 1970 (UTC)) */
    lua_pushnumber(L, v.tv_sec + v.tv_usec / 1.0e6);
    return 1;
}

LOCAL int ldfl_get_object_name(lua_State* L)
{
    OBJ_HANDLE   handle = (OBJ_HANDLE)luaL_checkinteger(L, 1);
    const gchar* name   = dfl_get_object_name(handle);
    if (name == NULL) {
        return 0;
    }

    lua_pushstring(L, name);
    return 1;
}

LOCAL int ldfl_get_class_name(lua_State* L)
{
    gchar class_name[MAX_NAME_SIZE + 1] = {0};

    OBJ_HANDLE handle = (OBJ_HANDLE)luaL_checkinteger(L, 1);
    gint32     ret    = dfl_get_class_name(handle, class_name, MAX_NAME_SIZE);

    lua_pushinteger(L, ret);
    if (ret != DFL_OK) {
        return 1;
    }

    lua_pushlstring(L, class_name, strlen(class_name));
    return 2;
}

LOCAL int ldfl_get_object_count(lua_State* L)
{
    guint32      count      = 0;
    const gchar* class_name = (const gchar*)luaL_checkstring(L, 1);
    gint32       ret        = dfl_get_object_count(class_name, &count);
    lua_pushinteger(L, ret);
    if (ret != DFL_OK) {
        return 1;
    }

    lua_pushinteger(L, count);
    return 2;
}

LOCAL int ldfl_get_referenced_property(lua_State* L)
{
    OBJ_HANDLE   handle        = (OBJ_HANDLE)luaL_checkinteger(L, 1);
    const gchar* property_name = luaL_checkstring(L, 2);

    gchar  refobject_name[MAX_NAME_SIZE + 1]   = {0};
    gchar  refproperty_name[MAX_NAME_SIZE + 1] = {0};
    gint32 ret = dfl_get_referenced_property(handle, property_name, refobject_name, refproperty_name, MAX_NAME_SIZE,
                                             MAX_NAME_SIZE);
    lua_pushinteger(L, ret);
    if (ret != DFL_OK) {
        return 1;
    }

    lua_pushlstring(L, refobject_name, strlen(refobject_name));
    lua_pushlstring(L, refproperty_name, strlen(refproperty_name));
    return 3;
}

LOCAL int ldfl_set_property_value(lua_State* L)
{
    DF_OPTIONS   option        = 0;
    OBJ_HANDLE   handle        = (OBJ_HANDLE)luaL_checkinteger(L, 1);
    const gchar* property_name = luaL_checkstring(L, 2);
    GVariant**   v             = luaL_checkudata(L, 3, MT_GVARIANT);
    if (lua_gettop(L) >= 4) {
        option = (DF_OPTIONS)luaL_checkinteger(L, 4);
    }
    gint32 ret = dfl_set_property_value(handle, property_name, *v, option);
    lua_pushinteger(L, ret);
    return 1;
}

LOCAL int lstatic_register_module(lua_State* L)
{
    const gchar* class_name = luaL_checkstring(L, 1);
    gint32       ret        = static_register_module(class_name);
    lua_pushinteger(L, ret);
    return 1;
}

LOCAL int lstatic_declaration_class(lua_State* L)
{
    const gchar* class_name = luaL_checkstring(L, 1);
    gint32       ret        = static_declaration_class(class_name);
    lua_pushinteger(L, ret);
    return 1;
}

LOCAL int safe_gvariant_table_to_slist(lua_State* L)
{
    GSList** list = (GSList**)tolightuserdata(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
#if LUA_VERSION_NUM == 501
    size_t len = lua_objlen(L, 2);
#else
    size_t len = lua_rawlen(L, 2);
#endif
    for (size_t i = len; i > 0; --i) {
        lua_rawgeti(L, 2, i);
        GVariant** v = luaL_checkudata(L, -1, MT_GVARIANT);
        *list        = g_slist_prepend(*list, *v);
        lua_pop(L, 1);
    }
    return 0;
}

LOCAL int safe_gvariant_slist_to_table(lua_State* L)
{
    GSList* list = (GSList*)tolightuserdata(L, 1);
    lua_createtable(L, g_slist_length(list), 0);
    int i = 1;
    for (GSList* node = list; node; node = g_slist_next(node)) {
        push_variant_value(L, (GVariant*)node->data);
        lua_rawseti(L, -2, i++);
    }
    return 1;
}

LOCAL int gvariant_table_to_slist(lua_State* L, int idx, GSList** list)
{
    lua_pushcfunction(L, safe_gvariant_table_to_slist);
    pushlightuserdata(L, list);
    lua_pushvalue(L, idx);
    return lua_pcall(L, 2, 0, 0) == LUA_OK;
}

LOCAL int ldfl_call_class_method(lua_State* L)
{
    GSList* user_data_list = NULL;
    GSList* input_list     = NULL;
    GSList* output_list    = NULL;

    OBJ_HANDLE   handle      = (OBJ_HANDLE)luaL_checkinteger(L, 1);
    const gchar* method_name = luaL_checkstring(L, 2);

    if (lua_gettop(L) >= 3 && !lua_isnil(L, 3)) {
        if (!gvariant_table_to_slist(L, 3, &user_data_list)) {
            g_slist_free(user_data_list);
            return lua_error(L);
        }
    }

    if (lua_gettop(L) >= 4 && !lua_isnil(L, 4)) {
        if (!gvariant_table_to_slist(L, 4, &input_list)) {
            g_slist_free(user_data_list);
            g_slist_free(input_list);
            return lua_error(L);
        }
    }
    gint32 ret = dfl_call_class_method(handle, method_name, user_data_list, input_list, &output_list);
    g_slist_free(user_data_list);
    g_slist_free(input_list);
    lua_pushinteger(L, ret);
    if (ret != DFL_OK) {
        return 1;
    }

    lua_pushcfunction(L, safe_gvariant_slist_to_table);
    pushlightuserdata(L, output_list);
    if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return lua_error(L);
    }
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return 2;
}

typedef struct {
    OBJ_HANDLE handle;
    gchar*     method_name;
    GSList*    user_data_list;
    GSList*    input_list;

    GSList* output_list;
    gint32  ret;
} call_class_method_async_t;

typedef struct {
    /* 参数从上到下对应 dfl_call_remote_class_method2 的第一个到最后一个参数 */
    gchar* ip;
    gint32 port;
    gchar* obj_name;
    gchar* method_name;
    GSList* user_data_list;
    GSList* input_list;
    GSList* output_list;
    gint32 ret;
} call_remote_class_method2_async_t;

LOCAL void do_call_class_method_async(void* data)
{
    call_class_method_async_t* p = data;
    p->ret = dfl_call_class_method(p->handle, p->method_name, p->user_data_list, p->input_list, &p->output_list);
}

LOCAL void do_call_remote_class_method2_async(gpointer data)
{
    call_remote_class_method2_async_t* p = data;
    p->ret = dfl_call_remote_class_method2(p->ip, p->port, p->obj_name, p->method_name,
        p->user_data_list, p->input_list, &p->output_list);
}

LOCAL void destory_call_remote_class_method2_async(gpointer data)
{
    call_remote_class_method2_async_t* p = data;
    if (p->ip) {
        free(p->ip);
    }

    if (p->obj_name) {
        free(p->obj_name);
    }

    if (p->method_name) {
        free(p->method_name);
    }

    if (p->user_data_list) {
        g_slist_free(p->user_data_list);
    }

    if (p->input_list) {
        g_slist_free(p->input_list);
    }

    if (p->output_list) {
        g_slist_free_full(p->output_list, (GDestroyNotify)g_variant_unref);
    }

    free(data);
}

LOCAL int query_call_remote_class_method2_async(gpointer data, gpointer context)
{
    lua_State* L = context;
    if (data == NULL) {
        lua_pushboolean(L, 0);
        return RET_CODE_1;
    }

    call_remote_class_method2_async_t* p = data;
    lua_pushboolean(L, 1);
    lua_pushinteger(L, p->ret);
    if (p->ret != DFL_OK) {
        return RET_CODE_2;
    }

    lua_pushcfunction(L, safe_gvariant_slist_to_table);
    pushlightuserdata(L, p->output_list);
    if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
        return lua_error(L);
    }
    return RET_CODE_3;
}

LOCAL void destory_call_class_method_async(void* data)
{
    call_class_method_async_t* p = data;
    if (p->method_name) {
        free(p->method_name);
    }
    if (p->user_data_list) {
        g_slist_free(p->user_data_list);
    }
    if (p->input_list) {
        g_slist_free(p->input_list);
    }
    if (p->output_list) {
        g_slist_free_full(p->output_list, (GDestroyNotify)g_variant_unref);
    }
    free(data);
}

LOCAL int query_call_class_method_async(void* data, void* context)
{
    lua_State* L = context;
    if (data == NULL) {
        lua_pushboolean(L, 0);
        return 1;
    }

    call_class_method_async_t* p = data;
    lua_pushboolean(L, 1);
    lua_pushinteger(L, p->ret);
    if (p->ret != DFL_OK) {
        return 2;
    }

    lua_pushcfunction(L, safe_gvariant_slist_to_table);
    pushlightuserdata(L, p->output_list);
    if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
        return lua_error(L);
    }
    return 3;
}

LOCAL int ldfl_call_class_method_async(lua_State* L)
{
    OBJ_HANDLE   handle      = (OBJ_HANDLE)luaL_checkinteger(L, 1);
    const gchar* method_name = luaL_checkstring(L, 2);

    worker_t* worker = get_worker(L);
    if (worker == NULL) {
        return luaL_error(L, "new worker failed");
    }

    work_t** u = lua_newuserdata(L, sizeof(work_t*));
    *u         = NULL;
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_setmetatable(L, -2);

    call_class_method_async_t* p = malloc(sizeof(call_class_method_async_t));
    if (p == NULL) {
        return luaL_error(L, "new call_class_method_async_t failed");
    }

    p->handle         = 0;
    p->method_name    = NULL;
    p->user_data_list = NULL;
    p->input_list     = NULL;
    p->output_list    = NULL;
    p->ret            = -1;
    work_t* work =
        new_work(do_call_class_method_async, destory_call_class_method_async, query_call_class_method_async, p);
    if (work == NULL) {
        free(p);
        return luaL_error(L, "new_work failed");
    }

    // 到这里 work 生命周期由 lua 管理,不需要手动释放内部资源,
    // 销毁时 destory_call_class_method_async 会自动释放
    *u = work;

    if (lua_gettop(L) >= 3 && !lua_isnil(L, 3)) {
        if (!gvariant_table_to_slist(L, 3, &p->user_data_list)) {
            return lua_error(L);
        }
    }

    if (lua_gettop(L) >= 4 && !lua_isnil(L, 4)) {
        if (!gvariant_table_to_slist(L, 4, &p->input_list)) {
            return lua_error(L);
        }
    }

    p->handle      = handle;
    p->method_name = strdup(method_name);
    if (p->method_name == NULL) {
        return luaL_error(L, "new method_name failed");
    }
    p->ret = -1;

    if (add_work(worker, work) == 0) {
        return 0;
    }
    return 1;
}

LOCAL int ldfl_call_remote_class_method2_async(lua_State* L)
{
    const gchar* ip = luaL_checkstring(L, LUA_PARAM_LIST_1);
    gint32 port = (gint32)luaL_checkinteger(L, LUA_PARAM_LIST_2);
    const gchar* obj_name = luaL_checkstring(L, LUA_PARAM_LIST_3);
    const gchar* method_name = luaL_checkstring(L, LUA_PARAM_LIST_4);

    worker_t* worker = get_worker(L);
    if (worker == NULL) {
        return luaL_error(L, "new worker failed");
    }

    work_t** u = lua_newuserdata(L, sizeof(work_t*)); // -2
    *u = NULL;
    lua_pushvalue(L, lua_upvalueindex(1)); // -1
    lua_setmetatable(L, -2); // -2 表示work 表的索引

    call_remote_class_method2_async_t* p = malloc(sizeof(call_remote_class_method2_async_t));
    if (p == NULL) {
        return luaL_error(L, "new call_remote_class_method2_async_t failed");
    }
    (void)memset_s(p, sizeof(call_remote_class_method2_async_t), 0, sizeof(call_remote_class_method2_async_t));
    p->ret = RET_CODE_ERR;

    work_t* work =
        new_work(do_call_remote_class_method2_async, destory_call_remote_class_method2_async,
            query_call_remote_class_method2_async, p);
    if (work == NULL) {
        free(p);
        return luaL_error(L, "new_work failed");
    }
    // 到这里 work 生命周期由 lua 管理,不需要手动释放内部资源,
    // 销毁时 destory_call_remote_class_method2_async 会自动释放
    *u = work;

    if (lua_gettop(L) >= 5 && !lua_isnil(L, LUA_PARAM_LIST_5)) { /* 第5个参数为NULL时报错退出 */
        if (!gvariant_table_to_slist(L, LUA_PARAM_LIST_5, &p->user_data_list)) {
            return lua_error(L);
        }
    }

    if (lua_gettop(L) >= 6 && !lua_isnil(L, LUA_PARAM_LIST_6)) { /* 第6个参数为NULL时报错退出 */
        if (!gvariant_table_to_slist(L, LUA_PARAM_LIST_6, &p->input_list)) {
            return lua_error(L);
        }
    }

    /* 拷贝的原因：lua的资源由lua自身释放，拷贝的资源由destroy函数释放，防止出现一边未使用，被另一边释放的问题 */
    p->ip = strdup(ip);
    p->port = port;
    p->obj_name  = strdup(obj_name);
    p->method_name = strdup(method_name);
    p->ret = RET_CODE_ERR;
    if (p->method_name == NULL || p->obj_name == NULL || p->ip == NULL) {
        return luaL_error(L, "strdup failed");
    }
    if (add_work(worker, work) == 0) {
        return RET_CODE_0;
    }
    return RET_CODE_1;
}

LOCAL int ldfl_get_referenced_object(lua_State* L)
{
    OBJ_HANDLE   ref_handle    = 0;
    OBJ_HANDLE   handle        = (OBJ_HANDLE)luaL_checkinteger(L, 1);
    const gchar* property_name = luaL_checkstring(L, 2);
    gint32       ret           = dfl_get_referenced_object(handle, property_name, &ref_handle);
    lua_pushinteger(L, ret);
    if (ret != DFL_OK) {
        return 1;
    }

    lua_pushinteger(L, ref_handle);
    return 2;
}

LOCAL int ldfl_get_position(lua_State* L)
{
    OBJ_HANDLE handle = (OBJ_HANDLE)luaL_checkinteger(L, 1);
    gint32     ret    = dfl_get_position(handle);
    lua_pushinteger(L, ret);
    return 1;
}

LOCAL int ldfl_get_COMPATIBILITY_ENABLED(lua_State* L)
{
#if COMPATIBILITY_ENABLED == 1
    lua_pushinteger(L, 1);
#else
    lua_pushinteger(L, 0);
#endif
    return 1;
}

LOCAL int ldfl_get_SECURITY_ENHANCED_COMPATIBLE_BOARD_V1(lua_State* L)
{
#if SECURITY_ENHANCED_COMPATIBLE_BOARD_V1 == 1
    lua_pushinteger(L, 1);
#else
    lua_pushinteger(L, 0);
#endif
    return 1;
}

LOCAL int ldfl_get_SECURITY_ENHANCED_COMPATIBLE_BOARD_V3(lua_State* L)
{
#if SECURITY_ENHANCED_COMPATIBLE_BOARD_V3 == 1
    lua_pushinteger(L, 1);
#else
    lua_pushinteger(L, 0);
#endif
    return 1;
}

LOCAL int lgenerate_kvm_sessionid(lua_State* L)
{
    gint32 ret;
    gint32 ilen = 0;
    gchar sessionid[MAX_TOKEN_LENGTH + 1] = {0};
    guchar random_char[32] = {0};

    ret = vos_get_random_array(random_char, sizeof(random_char));
    if (ret != VOS_OK) {
        return luaL_error(L, "lgenerate_kvm_sessionid: vos_get_random_array fail, ret = %d", ret);
    }
    for (gint32 i = 0; i < sizeof(random_char); i++) {
        ilen += snprintf_truncated_s(sessionid + ilen, MAX_TOKEN_LENGTH + 1 - ilen, "%02x", random_char[i]);
    }
    sessionid[sizeof(sessionid) - 1] = '\0';

    lua_pushlstring(L, sessionid, strlen(sessionid));
    return 1;
}

LOCAL int lcompute_checksum(lua_State* L)
{
    size_t len = 0;
    guint8 t   = (guint8)luaL_checkinteger(L, 1);
    if (t > G_CHECKSUM_SHA384) {
        lua_pushstring(L, "invalid CHECKSUM type");
        return lua_error(L);
    }

    const gchar* str = luaL_checklstring(L, 2, &len);
    gchar*       r   = g_compute_checksum_for_string((GChecksumType)t, str, len);
    if (r == NULL) {
        return 0;
    }

    lua_pushlstring(L, r, strlen(r));
    (void)memset_s(r, strlen(r), 0, strlen(r));
    g_free(r);
    return 1;
}

LOCAL int l_new_EVENT_WORK_STATE_S(lua_State* L)
{
    guint8  v = lua_gettop(L) >= 1 ? (guint8)luaL_checkinteger(L, 1) : 0;
    guint8* p = lua_newuserdata(L, sizeof(EVENT_WORK_STATE_S));
    *p        = v;
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_setmetatable(L, -2);
    return 1;
}

LOCAL int l_new_SENSOR_READINGMASK_BIT(lua_State* L)
{
    guint16  v = lua_gettop(L) >= 1 ? (guint16)luaL_checkinteger(L, 1) : 0;
    guint16* p = lua_newuserdata(L, sizeof(SENSOR_READINGMASK_BIT));
    *p         = v;
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_setmetatable(L, -2);
    return 1;
}

LOCAL int l_new_SENSOR_STATE_BIT_T(lua_State* L)
{
    guint16  v = lua_gettop(L) >= 1 ? (guint16)luaL_checkinteger(L, 1) : 0;
    guint16* p = lua_newuserdata(L, sizeof(SENSOR_STATE_BIT_T));
    *p         = v;
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_setmetatable(L, -2);
    return 1;
}

LOCAL int l_new_SENSOR_CAPABILITY_S(lua_State* L)
{
    guint8  v = lua_gettop(L) >= 1 ? (guint8)luaL_checkinteger(L, 1) : 0;
    guint8* p = lua_newuserdata(L, sizeof(SENSOR_CAPABILITY_S));
    *p        = v;
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_setmetatable(L, -2);
    return 1;
}

LOCAL int l_usleep(lua_State* L)
{
    usleep((int)luaL_checkinteger(L, 1));
    return 0;
}

const char* G_LOCKTYPE[] = {"shared", "unlock", "exclusive", NULL};
LOCAL int  l_flock(lua_State* L)
{
    FILE** f  = luaL_checkudata(L, 1, "FILE*");
    int    fd = fileno(*f);
    if (fd < 0) {
        return luaL_error(L, "file handle is closed");
    }

    unsigned int operation = 0;
    switch (luaL_checkoption(L, 2, "exclusive", G_LOCKTYPE)) {
        case 0:
            operation = LOCK_SH;
            break;
        case 1:
            operation = LOCK_UN;
            break;
        default:
            operation = LOCK_EX;
            break;
    }

    if (lua_gettop(L) >= 3 && lua_toboolean(L, 3)) {
        operation |= LOCK_NB;
    }

    if (flock(fd, (int)operation) >= 0) {
        lua_pushboolean(L, 1);
        return 1;
    }

    lua_pushnil(L);
    if (errno == EWOULDBLOCK) {
        lua_pushstring(L, "would block");
    } else {
        gchar err_buf[128]; // 设定错误码转错误信息自定义缓冲区大小为128
        (void)memset_s(err_buf, sizeof(err_buf), 0, sizeof(err_buf));
        (void)strerror_r(errno, err_buf, sizeof(err_buf));
        lua_pushstring(L, err_buf);
    }
    return 2;
}

LOCAL int ldfl_inet_pton(lua_State* L)
{
    guint32      ip_digit = 0;
    guint32      af       = luaL_checkinteger(L, 1);
    const gchar* ip       = (const gchar*)luaL_checkstring(L, 2);
    (void)inet_pton(af, ip, &ip_digit);
    lua_pushinteger(L, ip_digit);
    return 1;
}

LOCAL int ldfl_realpath(lua_State* L)
{
    char         resolved_path[PATH_MAX];
    const gchar* path      = (const gchar*)luaL_checkstring(L, 1);
    const gchar* real_path = realpath(path, resolved_path);
    if (real_path == NULL && errno != ENOENT) {
        lua_pushstring(L, "get realpath failed");
        return lua_error(L);
    }

    lua_pushstring(L, real_path);
    return 1;
}

LOCAL int ldfl_is_dir(lua_State* L)
{
    const gchar* path = (const gchar*)luaL_checkstring(L, 1);
    lua_pushboolean(L, g_file_test(path, G_FILE_TEST_IS_DIR) == 0 ? 0 : 1);
    return 1;
}

LOCAL int ldfl_is_symbol_link(lua_State* L)
{
    const gchar* path = (const gchar*)luaL_checkstring(L, 1);
    lua_pushboolean(L, (g_file_test(path, G_FILE_TEST_IS_SYMLINK) == 0) ? 0 : 1);
    return 1;
}

LOCAL luaL_Reg l[] = {
    {"init", ldfl_init},
    {"get_object_handle", ldfl_get_object_handle},
    {"get_object_handles", ldfl_get_object_list},
    {"get_object_handle_nth", ldfl_get_object_handle_nth},
    {"get_property_value", ldal_get_property_value},
    {"get_object_name", ldfl_get_object_name},
    {"get_class_name", ldfl_get_class_name},
    {"get_object_count", ldfl_get_object_count},
    {"get_timestamp", ldal_get_timestamp},
    {"get_timestamp_offset", ldal_timestamp_offset},
    {"get_file_accessible", ldal_get_file_accessible},
    {"get_file_length", lvos_get_file_length},
    {"get_datetime", ldal_datetime},
    {"gettime", timeout_lua_gettime},
    {"get_referenced_property", ldfl_get_referenced_property},
    {"get_referenced_object", ldfl_get_referenced_object},
    {"get_position", ldfl_get_position},
    {"get_compatibility_enabled", ldfl_get_COMPATIBILITY_ENABLED},
    {"get_security_enhanced_compatible_board_v1", ldfl_get_SECURITY_ENHANCED_COMPATIBLE_BOARD_V1},
    {"get_security_enhanced_compatible_board_v3", ldfl_get_SECURITY_ENHANCED_COMPATIBLE_BOARD_V3},
    {"set_property_value", ldfl_set_property_value},
    {"register_module", lstatic_register_module},
    {"declaration_class", lstatic_declaration_class},
    {"call_class_method", ldfl_call_class_method},
    {"generate_kvm_sessionid", lgenerate_kvm_sessionid},
    {"compute_checksum", lcompute_checksum},
    {"usleep", l_usleep},
    {"flock", l_flock},
    {"inet_pton", ldfl_inet_pton},
    {"realpath", ldfl_realpath},
    {"is_dir", ldfl_is_dir},
    {"is_symbol_link", ldfl_is_symbol_link},
    {"rm_filepath", lvos_rm_filepath},
    {"copy_file", lvos_file_copy},
    {"system_s", lvos_system_s},
    {"file_stat", l_file_stat},
    {"statvfs", l_statvfs},
    {"file_list", l_file_list},
    {NULL, NULL},
};

LUALIB_API int luaopen_dflib_core(lua_State* L)
{
#if LUA_VERSION_NUM == 501
    luaL_register(L, "dflib.core", l);
#else
    luaL_newlib(L, l);
#endif

    init_gvariant(L);
    lua_setfield(L, -2, "GVariant");

    init_sharedata(L);
    lua_setfield(L, -2, "ShareData");

    init_uip_sensor_alarm(L);

    luaL_getmetatable(L, MT_EVENT_WORK_STATE_S);
    lua_pushcclosure(L, l_new_EVENT_WORK_STATE_S, 1);
    lua_setfield(L, -2, "EVENT_WORK_STATE_S");

    luaL_getmetatable(L, MT_SENSOR_READINGMASK_BIT);
    lua_pushcclosure(L, l_new_SENSOR_READINGMASK_BIT, 1);
    lua_setfield(L, -2, "SENSOR_READINGMASK_BIT");

    luaL_getmetatable(L, MT_SENSOR_STATE_BIT_T);
    lua_pushcclosure(L, l_new_SENSOR_STATE_BIT_T, 1);
    lua_setfield(L, -2, "SENSOR_STATE_BIT_T");

    luaL_getmetatable(L, MT_SENSOR_CAPABILITY_S);
    lua_pushcclosure(L, l_new_SENSOR_CAPABILITY_S, 1);
    lua_setfield(L, -2, "SENSOR_CAPABILITY_S");

    init_worker(L);

    luaL_getmetatable(L, MT_WORK);
    lua_pushcclosure(L, ldfl_call_class_method_async, 1);
    lua_setfield(L, -2, "call_class_method_async");

    luaL_getmetatable(L, MT_WORK);
    lua_pushcclosure(L, ldfl_call_remote_class_method2_async, 1);
    lua_setfield(L, -2, "call_remote_class_method2_async"); // -2为索引位置

    return 1;
}
