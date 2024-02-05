local handles = {}

local function load_module(module_name)
    local m = require(module_name)
    assert(type(m) == 'table', string.format('invalid module %s', module_name))
    for k, v in pairs(m) do
        assert(type(v) == 'function', string.format('redfish handle %s.%s is not function', module_name, k))
        assert(not handles[k], string.format('redfish handle duplicate %s.%s', module_name, k))
        handles[k] = v
    end
end

load_module("uploadfile")
load_module('sms_proxy')
load_module('proxy')
load_module('sessionservice')

-- UpdateService
load_module('updateservice.updateservice')

load_module('download_static_file')

return handles
