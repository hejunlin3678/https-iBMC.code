local portal = require 'portal'
local tasks = require 'tasks'

local M = {}

local REBOOT_PREPARE = 1
local REBOOT_PERFORM = 2
local DFL_OK = 0
local reboot_handles = {}
portal.register_reboot_handler(function(rc, dt)
    for _, v in pairs(reboot_handles) do
        local f
        if rc == REBOOT_PREPARE then
            f = v.on_reboot_prepare
        elseif rc == REBOOT_PERFORM then
            f = v.on_reboot
        end

        if f then
            local ret = f(dt)
            if ret ~= DFL_OK then
                return ret
            end
        end
    end

    return DFL_OK
end)

function M.on_reboot(cb, prepare_cb)
    local id = #reboot_handles + 1
    reboot_handles[id] = {
        on_reboot_prepare = prepare_cb,
        on_reboot = cb
    }
    return {
        cancel = function(delay)
            if not delay then
                reboot_handles[id] = nil
                return
            end
            tasks.fork(function()
                tasks.sleep(delay)
                reboot_handles[id] = nil
            end)
        end
    }
end

function M.hook(max_timeout)
    return M.on_reboot(function(dt)
        return dt > max_timeout and DFL_OK or -1
    end)
end

return M
