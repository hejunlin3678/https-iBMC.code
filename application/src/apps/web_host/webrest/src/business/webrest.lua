local ROOT = ROOT_PATH .. '/lualib'
package.path = ROOT .. '/?.lua;' .. ROOT .. '/?/init.lua;' .. package.path
package.path = ROOT .. '/webrest_handles/?.lua;' .. ROOT .. '/webrest_handles/?/init.lua;' .. package.path
package.cpath = '/usr/lib/?.so;' .. '/usr/lib64/?.so;' .. ROOT_PATH .. '/?.so;' .. package.cpath

local warp_handle = require 'warp_handle'
local webtask = require 'webrest.task'
local tasks = require 'tasks'
local portal = require 'portal'
local logging = require 'logging'
local cjson = require 'cjson'
local http = require 'http'
local HTTP_OK = http.status.HTTP_OK
local is_http_reply = http.is_reply
local default_header = http.default_header
local json_encode = cjson.encode
cjson.encode_empty_table_as_object(false)

logging:setPrint(portal.log)
logging:setLevel(portal.logLevel)

local handles = {}
local function load_module(module_name)
    local m = require(module_name)
    for k, v in pairs(m) do
        assert(type(v) == 'function', string.format('error: lua handle %s.%s is not function', module_name, k))
        assert(not handles[k], string.format('error: lua handle duplicate %s.%s', module_name, k))
        handles[k] = v
    end
end

package.path = ROOT_PATH .. '/webrest/?.lua;' .. package.path
package.path = ROOT_PATH .. '/webrest/login/?.lua;' .. package.path
package.path = ROOT_PATH .. '/webrest/home/?.lua;' .. package.path
package.path = ROOT_PATH .. '/webrest/system/?.lua;' .. package.path
package.path = ROOT_PATH .. '/webrest/maintenance/?.lua;' .. package.path
package.path = ROOT_PATH .. '/webrest/access_mgnt/?.lua;' .. package.path
package.path = ROOT_PATH .. '/webrest/service/?.lua;' .. package.path
package.path = ROOT_PATH .. '/webrest/bmc_setting/?.lua;' .. package.path
package.path = ROOT_PATH .. '/webrest/chassis/?.lua;' .. package.path

load_module("uploadfile")
load_module('sso')
load_module('local')
load_module('mutual')
load_module('kvmaccess')
load_module('overview')
load_module('product_info')
load_module('systeminfo')
load_module('units')
load_module('boards')
load_module('performance')
load_module('storage')
load_module('power')
load_module('thermal')
load_module('bios')
load_module('fusion_partition')
load_module('battery')
load_module('asset')

load_module('alarm_email')
load_module('alarm_syslog')
load_module('alarm_trap')
load_module('alarm_severity')
load_module('fdm')
load_module('systemlog')
load_module('ibmclog')
load_module('worklog')

load_module('account')
load_module('certupdate')
load_module('ldap')
load_module('kerberos')
load_module('twofactorauth')
load_module('security')
load_module('session')
load_module('tpcm')
load_module('bootcertificates')

load_module('port')
load_module('web')
load_module('kvm')
load_module('vmm')
load_module('vnc')
load_module('snmp')

load_module('network')
load_module('time')
load_module('upgrade')
load_module('configuration')
load_module('language')
load_module('license')
load_module('ibma')
load_module('sp')
load_module('usb')
load_module('failover')

load_module('smm_thermal')
load_module('smm_power')
load_module('swi')

-- 创建 webrest 服务
local server = tasks.start(function()
    local self = tasks.current()
    logging:info('start webrest server %d', self.taskID)

    -- 主循环主动垃圾回收
    while true do
        self:sleep(300)
        collectgarbage('step')
    end
end)

webrest = {}
local oldAddTaskInfo = webtask.addTaskInfo
webtask.addTaskInfo = function(name, dowload_type, cb)
    return oldAddTaskInfo(name, dowload_type, warp_handle(server, cb))
end

for k, h in pairs(handles) do
    webrest[k] = warp_handle(server, function(...)
        local result = h(...)
        if is_http_reply(result) then
            return result:fixErrorStatus()
        else
            return {HTTP_OK, json_encode(result), default_header}
        end
    end)
end
