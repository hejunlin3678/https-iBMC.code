local ROOT = ROOT_PATH .. "/lualib"
package.path = ROOT .. "/?.lua;" .. ROOT .. "/?/init.lua;" .. package.path
package.path = ROOT_PATH .. "/redfish/?.lua;" .. ROOT_PATH .. "/redfish/?/?.lua;" .. package.path
package.cpath = '/usr/lib/?.so;' .. '/usr/lib64/?.so;' .. ROOT_PATH .. "/?.so;" .. package.cpath

local warp_handle = require "warp_handle"
local handles = require "init"
local tasks = require "tasks"
local portal = require "portal"
local logging = require "logging"
local cjson = require "cjson"
local http = require "http"
local HTTP_OK = http.status.HTTP_OK
local is_http_reply = http.is_reply
local default_header = http.default_header
cjson.encode_empty_table_as_object(false)

logging:setPrint(portal.log)
logging:setLevel(portal.logLevel)

-- 创建 redfish 服务
local server = tasks.start(function()
    local self = tasks.current()
    logging:info("start redfish server %d", self.taskID)

    -- 主循环主动垃圾回收
    while true do
        self:sleep(300)
        collectgarbage("step")
    end
end)

redfish = {}

for k, h in pairs(handles) do
    redfish[k] = warp_handle(server, function(...)
        local result = h(...)
        if is_http_reply(result) then
            return result:fixErrorStatus()
        else
            return {HTTP_OK, result, default_header}
        end
    end)
end
