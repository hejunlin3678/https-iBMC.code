local c = require "dflib.core"
local Promise = require "dflib.promise"
local init = c.init
local get_property_value = c.get_property_value
local get_object_handles = c.get_object_handles
local get_object_handle_nth = c.get_object_handle_nth
local get_object_handle = c.get_object_handle
local get_referenced_property = c.get_referenced_property
local get_referenced_object = c.get_referenced_object
local get_object_name = c.get_object_name
local get_class_name = c.get_class_name
local get_object_count = c.get_object_count
local call_class_method = c.call_class_method
local call_remote_class_method2_async = c.call_remote_class_method2_async
local call_class_method_async = c.call_class_method_async
local get_datetime = c.get_datetime
local set_property_value = c.set_property_value
local get_position = c.get_position
local get_compatibility_enabled = c.get_compatibility_enabled
local get_security_enhanced_compatible_board_v1 = c.get_security_enhanced_compatible_board_v1
local get_security_enhanced_compatible_board_v3 = c.get_security_enhanced_compatible_board_v3
local Promise_resolve = Promise.resolve
local Promise_reject = Promise.reject
local Promise_fetch = Promise.fetch
local Promise_is_promise = Promise.is_promise
local Promise_new = Promise.new

local DFL_OK = 0
local RFERR_SUCCESS = 200
local dalib = {}

-- Err
local ErrObj_mt = {}
ErrObj_mt.__index = function(t, n)
    if n == "next" or n == "fold" or type(n) == "number" then
        return function()
            return Promise_reject(t[1])
        end
    end
    return Promise_reject(t[1])
end

local function new_errobj(fmt, ...)
    return setmetatable({string.format(fmt, ...)}, ErrObj_mt)
end

-- object
local Object_mt = {}
local function new_object(_, obj_name, handle)
    if not handle then
        assert(obj_name)
        local ok, h = get_object_handle(obj_name)
        if ok ~= DFL_OK then
            return new_errobj("get object handle (%s) failed: %d", obj_name, ok)
        end
        handle = h
    end

    return setmetatable({__object_name = obj_name, __handle = handle}, Object_mt)
end

local function throw_error(OnReject, fmt, ...)
    return OnReject(string.format(fmt, ...))
end

local function obj_next(obj, f)
    local ok, val = pcall(f, obj)
    return ok and Promise_resolve(val) or Promise_reject(val)
end

local function fix_obj_name(obj)
    local obj_name = obj.__object_name
    if not obj_name then
        local handle = obj.__handle
        if handle then
            obj_name = get_object_name(handle)
            obj.__object_name = obj_name
        end
    end
    return obj_name or "unknow obj"
end

local function property(obj, prop_name)
    local handle = obj.__handle
    local ok, prop = get_property_value(handle, prop_name)
    if ok ~= DFL_OK then
        return throw_error(Promise_reject, "get object property (%s.%s) failed: %d", fix_obj_name(obj), prop_name,
            ok)
    end
    return Promise_resolve(prop)
end

local function ref_property(obj, prop_name)
    local handle = obj.__handle
    local ok, ref_obj_name, ref_prop_name = get_referenced_property(handle, prop_name)
    if ok ~= DFL_OK then
        return throw_error(Promise_reject, "get object ref property (%s.%s) failed: %d", fix_obj_name(obj),
            prop_name, ok)
    end
    return Promise_resolve({obj_name = ref_obj_name, prop_name = ref_prop_name})
end

local function ref_object(obj, prop_name)
    local handle = obj.__handle
    local ok, h = get_referenced_object(handle, prop_name)
    if ok ~= DFL_OK then
        return throw_error(Promise_reject, "get object ref object (%s.%s) failed: %d", fix_obj_name(obj),
            prop_name, ok)
    end
    return Promise_resolve(new_object(nil, false, h))
end

local function set_property(obj, prop_name, value)
    local handle = obj.__handle
    local ok
    if type(value) == "table" and #value == 2 then
        ok = set_property_value(handle, prop_name, value[1], value[2])
    else
        ok = set_property_value(handle, prop_name, value)
    end
    if ok ~= DFL_OK then
        error(string.format("set object property (%s.%s) failed: %d", fix_obj_name(obj), prop_name, ok))
    end
end

local function _call_class_method(obj, method_name, user_data_list, input_list)
    local ok, ret = call_class_method(obj.__handle, method_name, user_data_list, input_list)
    if ok ~= DFL_OK and ok ~= RFERR_SUCCESS then
        error(string.format("call object method (%s.%s) failed: %d", fix_obj_name(obj), method_name, ok))
    end
    return ret
end

local function _call_class_method_async(obj, method_name, user_data_list, input_list)
    return call_class_method_async(obj.__handle, method_name, user_data_list, input_list)
end

local function _call_remote_class_method2_async(ip, port, obj, method_name, user_data_list, input_list)
    return call_remote_class_method2_async(ip, port, fix_obj_name(obj), method_name, user_data_list, input_list)
end

Object_mt.__index = function(t, n)
    if n == "next" then
        return obj_next
    elseif n == "ref" then
        return ref_property
    elseif n == "ref_obj" then
        return ref_object
    elseif n == "call" then
        return _call_class_method
    elseif n == "remote_async_call" then
        return _call_remote_class_method2_async
    elseif n == "async_call" then
        return _call_class_method_async
    else
        return property(t, n)
    end
end

Object_mt.__newindex = set_property
Object_mt.__call = _call_class_method

-- class
local Class_mt = {}
local function new_class(_, class_name)
    local o = {__class_name = class_name, __handles = false}
    return setmetatable(o, Class_mt)
end

local function safe_call(f, ...)
    local ok, r = pcall(f, ...)
    if not ok then
        return new_errobj(r)
    end
    return r
end

local function qry_class_handles(class, prop_name, val)
    assert(not class.__handles)
    local class_name = class.__class_name
    local ok, handles = get_object_handles(class_name)
    if ok ~= DFL_OK then
        return new_errobj("qry class handles (%s) failed: %d", class_name, ok)
    end
    if #handles == 0 then
        return new_errobj("qry class handles (%s) failed: handle list empty", class_name)
    end

    if prop_name ~= nil then
        assert(val ~= nil)
        for _, h in ipairs(handles) do
            local ok, prop = get_property_value(h, prop_name)
            if ok == DFL_OK and prop == val then
                class.__handles = {h}
                return class
            end
        end
        return new_errobj("qry class handle (%s.%s) failed: not match", class_name, prop_name)
    end
    class.__handles = handles
    return class
end

local function qry_class_handle_nth(class, n)
    assert(not class.__handles, "qry_handle_nth: handle existed")
    local class_name = class.__class_name
    local ok, h = get_object_handle_nth(class_name, n)
    if ok ~= DFL_OK then
        return new_errobj("get class handle nth (%s[%d]) failed: %d", class_name, n, ok)
    end
    class.__handles = {h}
    return class
end

local function fold(class, f, InitialAcc)
    local accVal = InitialAcc
    local handles = class.__handles
    if handles then
        for _, h in ipairs(handles) do
            local obj = new_object(nil, false, h)
            local ok, val, exit = pcall(f, obj, accVal)
            if not ok then
                return Promise_reject(val)
            end
            if Promise_is_promise(val) then
                ok, val = pcall(Promise_fetch, val)
                if not ok then
                    return Promise_reject(val)
                end
            end
            accVal = val
            if exit == false then
                break
            end
        end
    end
    return Promise_resolve(accVal)
end

local function class_next(class, f)
    local handles = class.__handles
    if not handles or #handles == 0 then
        return Promise_reject()
    end
    if not f or type(f) == 'number' then
        local h = class.__handles[f or 1]
        if h then
            return Promise_resolve(new_object(nil, false, h))
        end
    elseif type(f) == 'function' then
        local h = class.__handles[1]
        local ok, val = pcall(f, new_object(nil, false, h))
        return ok and Promise_resolve(val) or Promise_reject(val)
    end
    return Promise_reject()
end

local function object_count(class)
    return Promise_new(function(OnResolve, OnReject)
        local class_name = class.__class_name
        local ok, count = get_object_count(class_name)
        if ok ~= DFL_OK then
            return throw_error(OnReject, "get class (%s) object count failed: %d", class_name, ok)
        end
        OnResolve(count)
    end)
end

Class_mt.__index = function(t, n)
    if n == "fold" then
        return fold
    elseif n == "next" then
        return class_next
    elseif type(n) == "number" then
        return safe_call(qry_class_handle_nth, t, n)
    else
        return class_next(t, function(obj)
            return obj[n]
        end)
    end
end

Class_mt.__call = function(...)
    return safe_call(qry_class_handles, ...)
end

Class_mt.__len = object_count

dalib.class = setmetatable({}, {
    __index = new_class,
    __call = function(_, class_name, ...)
        return new_class(nil, class_name)(...)
    end
})

dalib.object = setmetatable({}, {
    __index = new_object,
    __call = function(_, obj_name)
        return new_object(nil, obj_name)
    end
})

function dalib.object_name(obj)
    if getmetatable(obj) == Object_mt then
        return fix_obj_name(obj)
    end
    error("need object")
end

function dalib.class_name(obj)
    if getmetatable(obj) == Object_mt then
        local handle = obj.__handle
        if handle then
            local ok, n = get_class_name(handle)
            if ok == DFL_OK then
                return n
            end
        end
        error("invalid handle")
    end
    error("need object")
end

function dalib.get_position(obj)
    if getmetatable(obj) == Object_mt then
        local handle = obj.__handle
        if handle then
            return get_position(handle)
        end
        error("invalid handle")
    end
    error("need object")
end

function dalib.get_compatibility_enabled()
    local lua_state = {}
    return get_compatibility_enabled(lua_state)
end

function dalib.get_security_enhanced_compatible_board_v1()
    local lua_state = {}
    return get_security_enhanced_compatible_board_v1(lua_state)
end

function dalib.get_security_enhanced_compatible_board_v3()
    local lua_state = {}
    return get_security_enhanced_compatible_board_v3(lua_state)
end

function dalib.object_count(class_name)
    return object_count({__class_name = class_name}):fetch()
end

local function fetch(t)
    local res = {}
    for k, v in pairs(t) do
        if Promise_is_promise(v) then
            local r = v:fetch()
            if type(r) == "table" then
                res[k] = fetch(r)
            else
                res[k] = r
            end
        elseif type(v) == "table" then
            res[k] = fetch(v)
        else
            res[k] = v
        end
    end
    return res
end

dalib.fetch = fetch

function dalib.datetime()
    return get_datetime()
end

function dalib.init(appname)
    init(appname)
end

dalib.DFL_OK = DFL_OK
dalib.RFERR_SUCCESS = RFERR_SUCCESS

function dalib.zip(c1, c2)
    return {
        fold = function(_, f, InitialAcc)
            local c1_handles = c1.__handles or {}
            local c2_handles = c2.__handles or {}
            local c1_len = #c1_handles
            local c2_len = #c2_handles

            local index = 0
            if c1_len < c2_len then
                return c2:fold(function(objC2, acc)
                    index = index + 1
                    local objC1 = (index <= c1_len) and new_object(nil, false, c1_handles[index]) or nil
                    return f(objC1, objC2, acc)
                end, InitialAcc)
            else
                return c1:fold(function(objC1, acc)
                    index = index + 1
                    local objC2 = (index <= c2_len) and new_object(nil, false, c2_handles[index]) or nil
                    return f(objC1, objC2, acc)
                end, InitialAcc)
            end
        end
    }
end

return dalib
