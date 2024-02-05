PENDING = 1
FULFILLED = 2
REJECTED = 3
POOLING = 4
local tremove = table.remove

local _CLASS_NAME = "Promise"

local Promise = {}
local promise_mt = {}
local promise_pool = setmetatable({}, {__mode = "kv"})

local function pool_get_promise()
    local self = tremove(promise_pool)
    if not self then
        self = setmetatable({_CLASS_NAME, PENDING, false, false, false}, promise_mt)
        self[5] = nil
    else
        assert(self[2] == POOLING)
        self[2] = PENDING
    end
    return self
end

local function pool_put_promise(self)
    assert(self[2] ~= POOLING)
    self[2] = POOLING
    self[3] = false
    self[4] = false
    self[5] = nil
    promise_pool[#promise_pool + 1] = self
end

promise_mt.__index = promise_mt

local function dispatch(resolve, reject, success, value)
    if success then
        if resolve then
            resolve(value)
        end
    elseif reject then
        reject(value)
    end
end

function promise_mt:next(onResolve, onReject)
    if (type(onResolve) ~= "function") then
        onResolve = function(val)
            return val
        end
    end

    return Promise.new(function(resolve, reject)
        if (self[2] == FULFILLED) then
            dispatch(resolve, reject, pcall(onResolve, self[5]))
        elseif (self[2] == REJECTED) then
            if (type(onReject) ~= "function") then
                reject(self[5])
            else
                dispatch(resolve, reject, pcall(onReject, self[5]))
            end
        elseif (self[2] == PENDING) then
            self[3] = function(value)
                dispatch(resolve, reject, pcall(onResolve, value))
            end

            self[4] = function(reason)
                if (type(onReject) ~= "function") then
                    reject(reason)
                else
                    dispatch(resolve, reject, pcall(onReject, reason))
                end
            end
        end
    end)
end

function promise_mt:catch(onReject)
    if type(onReject) == "function" then
        return self:next(nil, onReject)
    end
    return self:next(nil, function()
        return onReject
    end)
end

function promise_mt:fetch()
    local val = self[5]
    local status = self[2]
    pool_put_promise(self)
    if status == REJECTED then
        if val then
            error(val)
        end
    else
        return val
    end
end

function promise_mt:fetch_or(val)
    return self:catch(val):fetch()
end

function promise_mt:eq(val)
    return self:next(function(v)
        assert(v == val, "neq check failed")
        return self[5]
    end)
end

function promise_mt:neq(val)
    return self:next(function(v)
        assert(v ~= val, "neq check failed")
        return self[5]
    end)
end

local function is_promise(t)
    return type(t) == "table" and t[1] == _CLASS_NAME
end

local function promise_reject(self, reason)
    if (self[2] == PENDING) then
        self[2] = REJECTED
        self[5] = reason
        local reject = self[4]
        if reject then
            reject(reason)
        end
    end
end

local promise_resolve
promise_resolve = function(self, value)
    if is_promise(value) then
        value:next(function(ret)
            promise_resolve(self, ret)
        end, function(err)
            promise_reject(self, err)
        end)
        pool_put_promise(value)
    elseif (self[2] == PENDING) then
        self[2] = FULFILLED
        self[5] = value
        local resolve = self[3]
        if resolve then
            resolve(value)
        end
    end
end

function Promise.new(excutor)
    local self = pool_get_promise()
    dispatch(nil, promise_reject, pcall(excutor, function(value)
        return promise_resolve(self, value)
    end, function(reason)
        return promise_reject(self, reason)
    end))
    return self
end

function Promise.resolve(value)
    local self = pool_get_promise()
    promise_resolve(self, value)
    return self
end

function Promise.reject(reason)
    local self = pool_get_promise()
    promise_reject(self, reason)
    return self
end

Promise.is_promise = is_promise

return Promise
