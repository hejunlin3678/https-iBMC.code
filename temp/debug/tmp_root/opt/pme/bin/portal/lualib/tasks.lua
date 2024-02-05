local tasks = require "portal.tasks"
local warp_handle = require "warp_handle"
local table_remove = table.remove
local table_insert = table.insert
local unpack = unpack or table.unpack
local coroutine_create = coroutine.create
local coroutine_yield = coroutine.yield
local coroutine_resume = coroutine.resume
local task_exit = tasks.exit
local task_send = tasks.send
local traceback = debug.traceback
local newSessionId = tasks.newSessionId

local current_task = nil

local coroutine_pool = setmetatable({}, {__mode = "kv"})
local function co_create(f)
    local co = table_remove(coroutine_pool)
    if co == nil then
        co = coroutine_create(function(...)
            f(...)
            while true do
                f = nil
                coroutine_pool[#coroutine_pool + 1] = co
                f = coroutine_yield()
                f(coroutine_yield())
            end
        end)
    else
        coroutine_resume(co, f)
    end
    return co
end

local function run_co(err, co, ...)
    local ok, res = coroutine_resume(co, ...)
    if not ok then
        local newErr = tostring(traceback(co, tostring(res)))
        if err then
            err = err .. "/n" .. newErr
        else
            err = newErr
        end
    end
    return err
end

local Task = {}
Task.__index = Task

local function dispatch(task, sessionId, data)
    local err = nil
    local sessions = task.session_coroutines
    local co = sessions[sessionId]
    if co == nil then
        err = string.format("dispatch [%d]: invalid sessionId %d", task.taskID, sessionId)
    else
        sessions[sessionId] = nil
        err = run_co(err, co, data)
    end

    -- 前一个 co 运行期间 fork 的 co 延迟到这里调度
    local forks = task.fork_coroutines
    while true do
        local co = table_remove(forks, 1)
        if co == nil then
            break
        end
        err = run_co(err, co)
    end

    if err then
        error(err)
    end
end

function Task.new()
    local task = setmetatable({session_coroutines = {}, fork_coroutines = {}}, Task)
    task.taskID = tasks.newTask(function(...)
        local old_task = current_task
        current_task = task
        local ok, err = pcall(dispatch, task, ...)
        current_task = old_task
        if not ok then
            error(err)
        end
    end)
    return task
end

function Task:sleep(millis)
    local sessions = self.session_coroutines
    local sessionId = tasks.timeout(millis)
    assert(sessions[sessionId] == nil, string.format("sleep [%d]: invalid sessionId %d", self.taskID, sessionId))
    sessions[sessionId] = coroutine.running()
    coroutine_yield()
    sessions[sessionId] = nil
end

function Task:fork(func, ...)
    local n = select("#", ...)
    local co
    if n == 0 then
        co = co_create(func)
    else
        local args = {...}
        co = co_create(function()
            func(unpack(args, 1, n))
        end)
    end

    if self ~= current_task then
        -- 不在当前任务上下文,放入 session 队列等待执行
        local sessionId = newSessionId(self.taskID)
        assert(sessionId ~= 0, string.format("fork [%d]: invalid sessionId ", self.taskID, sessionId))

        local sessions = self.session_coroutines
        assert(sessions[sessionId] == nil, "invalid session")
        sessions[sessionId] = co
        task_send(self.taskID, sessionId)
    else
        -- 插入到 fork 队列,新 fork 出的 coroutine 延迟到当前 coroutine 返回后才会被调度运行
        table_insert(self.fork_coroutines, co)
    end
    return co
end

function Task:exit()
    task_exit()
end

local M = {}

function M.start(start_func)
    local task = Task.new()
    if start_func then
        task:fork(start_func)
    end
    return task
end

function M.sleep(millis)
    assert(current_task, "sleep must be called in task")
    current_task:sleep(millis)
end

function M.fork(func, ...)
    assert(current_task, "fork must be called in task")
    current_task:fork(func, ...)
end

M.register = tasks.register
M.getTaskId = tasks.getTaskId
M.exit = task_exit

function M.current()
    return current_task
end

return M
