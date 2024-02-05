local cjson = require 'cjson'
local dflib = require 'dflib'
local utils = require 'utils'
local http = require 'http'
local c = require 'dflib.core'
local logging = require 'logging'
local O = dflib.object
local C = dflib.class
local null = cjson.null
local call_method = utils.call_method
local guint16 = c.GVariant.new_uint16

local HISTORY_FILE_FORMAT = '%d%d%d%d/%d%d/%d%d %d%d:%d%d:%d%d#%d+'

local SMS_BMA_GLOBAL_STATE_NOTOK = 1
local function is_bma_active()
    local ok, ret = call_method(nil, O.sms0, 'GetBMAConnectState')
    if not ok then
        logging:error('GetBMAConnectState failed: %s', ret)
        return false
    end

    if ret[1] == SMS_BMA_GLOBAL_STATE_NOTOK then
        return false
    end

    return true
end

local ME_ACCESS_TYPE_SMBUS_PECI = 1
local function supported_processer_and_memory_history()
    if is_bma_active() then
        return true
    end

    if C.MeInfo[0].SupportUtiliseMode:fetch() == 0 or C.MeInfo[0].MeAccessType:fetch() == ME_ACCESS_TYPE_SMBUS_PECI then
        return false
    end

    return true
end

local function HandleUtilisePercent(data)
    for i = 1, #data do
        data[i] = tonumber(string.format('%.2f', tonumber(data[i]) / 100))
    end
    return data
end

local M = {}

local CPU_HISTORY_FILE = '/opt/pme/pram/cpu_utilise_webview.dat'
function M.GetProcesserUsage()
    local result = {}
    if supported_processer_and_memory_history() then
        local file = io.open(CPU_HISTORY_FILE, 'r')
        if not file then
            error('open file failed')
        end
        local record = file:read()
        while record do
            if string.match(record, HISTORY_FILE_FORMAT) then
                local data = utils.split(record, '#')
                result[#result + 1] = {Time = data[1], UtilisePercent = tonumber(data[2])}
            end
            record = file:read()
        end
        file:close()
    end

    return {
        UsagePercent = C.MeInfo[0].CpuUtilise:neq(0xffff):fetch_or(null),
        ThresholdPercent = C.MeInfo[0].CpuUtiliseThre:fetch(),
        History = result
    }
end

function M.UpdateProcesserUsage(user, data)
    local result = http.reply_ok()

    if data.ThresholdPercent ~= nil then
        result:join(http.safe_call(function()
            local ok, ret = utils.call_method(user, C.MeInfo[0]:next():fetch(), 'SetCpuUtiliseThre', nil,
                guint16(data.ThresholdPercent))
            if not ok then
                return ret
            end
        end))
    end

    if result:isOk() then
        return http.reply_ok_encode_json(M.GetProcesserUsage())
    end
    return result
end

local MEMORY_HISTORY_FILE = '/opt/pme/pram/memory_os_webview.dat'
function M.GetMemoryUsage()
    local result = {}
    local capacity
    if supported_processer_and_memory_history() then
        local file = io.open(MEMORY_HISTORY_FILE, 'r')
        if not file then
            error('open file failed')
        end
        local record = file:read()
        while record do
            if string.match(record, HISTORY_FILE_FORMAT) then
                local data = utils.split(record, '#')
                result[#result + 1] = {Time = data[1], UtilisePercent = tonumber(data[2])}
            end
            record = file:read()
        end
        file:close()
        
        capacity = 0
        C.Memory():fold(function(obj)
            obj.Presence:neq(0):next(function()
                local tmp_cap = obj.Capacity:fetch()
                if string.match(tmp_cap, 'MB') or string.match(tmp_cap, 'mb') then
                    capacity = capacity + (string.match(tmp_cap, '%d+') / 1024)
                elseif string.match(tmp_cap, 'GB') or string.match(tmp_cap, 'gb') then
                    capacity = capacity + string.match(tmp_cap, '%d+')
                end
            end):catch()
        end):catch()
    end

    local free
    if O.ComputerSystem0.MemUsage:fetch() == 0xffff or O.ComputerSystem0.TotalSystemMemoryGiB:fetch() == 0xffff then
        free = null
    else
        free = tonumber(string.format('%.2f', (((100 - O.ComputerSystem0.MemUsage:fetch()) / 100) * 10000  *
            (O.ComputerSystem0.TotalSystemMemoryGiB:fetch() * 100) / 1000000)))
    end
    return {
        FreeGiB = free,
        CachedGiB = O.ComputerSystem0.SystemMemoryCachedGiB:neq(0xffff):fetch_or(null),
        BuffersGiB = O.ComputerSystem0.SystemMemoryBuffersGiB:neq(0xffff):fetch_or(null),
        CapacityGiB = capacity or null,
        UsagePercent = C.MeInfo[0].MemUtilise:neq(0xffff):fetch_or(null),
        ThresholdPercent = C.MeInfo[0].MemUtiliseThre:fetch(),
        History = result
    }
end

function M.UpdateMemoryUsage(user, data)
    local result = http.reply_ok()

    if data.ThresholdPercent ~= nil then
        result:join(http.safe_call(function()
            local ok, ret = utils.call_method(user, C.MeInfo[0]:next():fetch(), 'SetMemUtiliseThre', nil,
                guint16(data.ThresholdPercent))
            if not ok then
                return ret
            end
        end))
    end

    if result:isOk() then
        return http.reply_ok_encode_json(M.GetMemoryUsage())
    end
    return result
end

function M.GetDiskUsage()
    local patitions = {}

    if is_bma_active() then
        C.OSDrivePartition():fold(function(obj)
            patitions[#patitions + 1] = {
                PartitionName = obj.OSDriveName:fetch(),
                CapacityGB = tonumber(obj.TotalCapacityGiB:fetch()),
                UsedGB = tonumber(obj.UsedCapacityGiB:fetch()),
                UsagePercent = tonumber(obj.Usage:fetch())
            }
        end):catch()
    end

    return {ThresholdPercent = C.MeInfo[0].DiskPartitionUsageThre:fetch(), Partitions = patitions}
end

function M.UpdateDiskUsage(user, data)
    local result = http.reply_ok()

    if data.ThresholdPercent ~= nil then
        result:join(http.safe_call(function()
            local ok, ret = utils.call_method(user, C.MeInfo[0]:next():fetch(), 'SetDiskPartitionUsageThre', nil,
                guint16(data.ThresholdPercent))
            if not ok then
                return ret
            end
        end))
    end

    if result:isOk() then
        return http.reply_ok_encode_json(M.GetDiskUsage())
    end
    return result
end

local function GetBindWidthUsage(port)
    local usage = port.BandwidthUsage:next(function (usage)
        if usage > 100 * 100 then
            return null
        end
        return usage
    end):fetch_or(null)
    if usage ~= null then
        return usage / 100
    end
    return usage
end

function M.GetNetworkBandwidthUsage()
    local history = {}

    if is_bma_active() then
        C.NetCard():fold(function(obj)
            obj.VirtualNetCardFlag:neq(1):next(function()
                obj.BWUWaveFileName:neq('N/A'):next(function()
                    local file = io.open('/opt/pme/pram/' .. obj.BWUWaveFileName:fetch(), 'r')
                    if not file then
                        error('open file failed')
                    end
                    local result = {}
                    local record = file:read()
                    while record do
                        if string.match(record, HISTORY_FILE_FORMAT) then
                            local data = utils.split(record, '#')
                            result[#result + 1] = {
                                Time = table.remove(data, 1),
                                UtilisePercent = HandleUtilisePercent(data)
                            }
                        end
                        record = file:read()
                    end
                    file:close()
                    local current_percents = {}
                    local index = 1
                    while index <= obj.PortNum:fetch() do
                        C.BusinessPort():fold(function(port)
                            if port.RefNetCard:fetch() == dflib.object_name(obj) and index == port.SilkNum:fetch() then
                                table.insert(current_percents, GetBindWidthUsage(port))
                            end
                            return true
                        end)
                        index = index + 1
                    end
                    history[#history + 1] = {
                        BWUWaveTitle = obj.BWUWaveTitle:fetch(),
                        PortCount = obj.PortNum:fetch(),
                        CurrentUtilisePercents = current_percents,
                        UtilisePercents = result
                    }
                end)
            end)
        end):catch()
    end

    return {ThresholdPercent = O.ComputerSystem0.BandwidthUsageThre:fetch(), History = history}
end

function M.UpdateNetworkBandwidthUsage(user, data)
    local result = http.reply_ok()

    if data.ThresholdPercent ~= nil then
        result:join(http.safe_call(function()
            local ok, ret = utils.call_method(user, O.ComputerSystem0, 'SetBandwidthUsageThre', nil,
                guint16(data.ThresholdPercent))
            if not ok then
                return ret
            end
        end))
    end

    if result:isOk() then
        return http.reply_ok_encode_json(M.GetNetworkBandwidthUsage())
    end
    return result
end

function M.ClearNetworkBandwidthHistory(user, data)
    local result = http.reply_ok()

    if #data.BWUWaveTitle > 128 then
        return http.reply_bad_request("Invalid param")
    end

    if data.BWUWaveTitle ~= nil then
        C.NetCard():fold(function(obj)
            obj.BWUWaveTitle:eq(data.BWUWaveTitle):next(function()
                result:join(http.safe_call(function()
                    local ok, ret = utils.call_method(user, obj, 'ClearNetworkBWUWaveRecord')
                    if not ok then
                        return ret
                    end
                end))
            end)
        end)
    end

    return result
end

return M
