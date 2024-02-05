local tasks = require "tasks"
local dflib = require 'dflib'
local logging = require "logging"
local c = require "dflib.core"
local taskinfo = require "redfish.task"
local addTaskInfo = taskinfo.addTaskInfo
local getTaskInfo = taskinfo.getTaskInfo
local delTaskInfo = taskinfo.delTaskInfo
local get_datetime = c.get_datetime
local O = dflib.object

local RF_STATUS_OK = "OK"
local RF_STATUS_WARNING = "Warning"
local RF_STATUS_CRITICAL = "Critical"

local RF_TASK_NEW = "New"
local RF_TASK_STARTING = "Starting"
local RF_TASK_RUNNING = "Running"
local RF_TASK_SUSPENDED = "Suspended"
local RF_TASK_INTERRUPTED = "Interrupted"
local RF_TASK_PENDING = "Pending"
local RF_TASK_STOPPING = "Stopping"
local RF_TASK_COMPLETED = "Completed"
local RF_TASK_KILLED = "Killed"
local RF_TASK_EXCEPTION = "Exception"
local RF_TASK_SERVICE = "Service"

local mt_task = {}
mt_task.__index = mt_task

function mt_task:setProgress(p)
    self.Oem[O.BMC.CustomManufacturer:fetch_or("Huawei")].TaskPercentage = p
    return p
end

function mt_task:progress()
    return self.Oem[O.BMC.CustomManufacturer:fetch_or("Huawei")].TaskPercentage
end

function mt_task:changeState(new)
    logging:debug("change task %s[%d] state: %s => %s", self.Name or "", self.Id, self.TaskState, new)
    self.TaskState = new
end

function mt_task:running()
    self:changeState(RF_TASK_RUNNING)
end

function mt_task:pending()
    self:changeState(RF_TASK_PENDING)
end

function mt_task:stop()
    self:changeState(RF_TASK_STOPPING)
end

function mt_task:suspend()
    self:changeState(RF_TASK_SUSPENDED)
end

function mt_task:interrupt()
    self:changeState(RF_TASK_INTERRUPTED)
end

function mt_task:except()
    self:changeState(RF_TASK_EXCEPTION)
    self.TaskStatus = RF_STATUS_WARNING
end

function mt_task:kill()
    self:changeState(RF_TASK_KILLED)
    self.TaskStatus = RF_STATUS_CRITICAL
end

function mt_task:complete()
    self:changeState(RF_TASK_COMPLETED)
    self.TaskStatus = RF_STATUS_OK
end

function mt_task:IsComplete()
    local s = self.TaskState
    return s == RF_TASK_COMPLETED or s == RF_TASK_KILLED or s == RF_TASK_EXCEPTION
end

local M = {}
function M.start(cb, name, deleteTimeout)
    local status = setmetatable({
        TaskState = RF_TASK_STARTING,
        StartTime = get_datetime(),
        Messages = nil,
        Oem = {O.BMC.CustomManufacturer:fetch_or("Huawei") = {TaskPercentage = 0}}
    }, mt_task)

    local ok, id = addTaskInfo(status, name)
    if not ok then
        return false, id and getTaskInfo(id) or nil
    end
    status.Name = name or tostring(id)
    status.Id = id
    tasks.fork(function()
        local state, ret = pcall(cb, status)
        if not state then
            status:except()
            status.Messages = ret
            logging:error("task %s[%d] exception: %s", name or "", id, ret)
            tasks.sleep(10 * 1000) -- 发生异常 10 秒后删除任务
        else
            if not status:IsComplete() then
                status:complete()
            end
            tasks.sleep(deleteTimeout or 60 * 1000 * 10) -- 10 分钟后删除任务
        end
        delTaskInfo(id)
        logging:debug("remove task %s[%d]", name or "", id)
    end)
    return true, status
end

M.STATE = {
    RF_TASK_NEW = RF_TASK_NEW,
    RF_TASK_STARTING = RF_TASK_STARTING,
    RF_TASK_RUNNING = RF_TASK_RUNNING,
    RF_TASK_SUSPENDED = RF_TASK_SUSPENDED,
    RF_TASK_INTERRUPTED = RF_TASK_INTERRUPTED,
    RF_TASK_PENDING = RF_TASK_PENDING,
    RF_TASK_STOPPING = RF_TASK_STOPPING,
    RF_TASK_COMPLETED = RF_TASK_COMPLETED,
    RF_TASK_KILLED = RF_TASK_KILLED,
    RF_TASK_EXCEPTION = RF_TASK_EXCEPTION,
    RF_TASK_SERVICE = RF_TASK_SERVICE
}

return M
