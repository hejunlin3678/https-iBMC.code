local dflib = require 'dflib'
local c = require 'dflib.core'
local defs = require 'define'
local utils = require 'utils'
local cfg = require 'config'
local http = require 'http'
local cjson = require 'cjson'
local download = require 'download'
local bit = require 'bit'
local logging = require 'logging'
local null = cjson.null
local C = dflib.class
local O = dflib.object
local rf_string_check = utils.rf_string_check

local M = {}

local function GetBatteryWorkingState(obj)
    local workstate = obj.WorkStatus:fetch_or(null)
    if workstate == 0 then 
        state = 'Offline'
    elseif workstate == 1 then
        state = 'Init'
    elseif workstate == 2 then
        state = 'Idle'
    elseif workstate == 3 then
        state = 'Charging'
    elseif workstate == 4 then
        state = 'FullCharged'
    elseif workstate == 5 then
        state = 'Discharging'
    elseif workstate == 6 then
        state = 'OnlineTest'
    elseif workstate == 7 then
        state = 'Protect'
    elseif workstate == 8 then
        state = 'Shutdown'    
    elseif workstate == 9 then
        state = 'Sleep'
    else
        state = 'Undefined'
    end
    return state
end

local function GetBatteryProperty()
    return C.BBUModule():fold(function(obj, acc)
        acc[#acc + 1] = {
            DeviceLocator = obj.SilkText:next(rf_string_check):fetch_or(null),
            Id = obj.Id:fetch_or(null),
            Manufacturer = obj.BattManu:next(rf_string_check):fetch_or(null),
            Model = obj.BattModel:next(rf_string_check):fetch_or(null),
            FirmwareVersion = obj.FirmwareVersion:next(rf_string_check):fetch_or(null),
            PartNumber = obj.BattPartNum:next(rf_string_check):fetch_or(null),
            WorkingState = GetBatteryWorkingState(obj),
            RatedCapacityWattHour = obj.RateCap:fetch_or(null),
            DischargeTimes = obj.BattDischargeTimes:fetch_or(null),
            WorkingHours = obj.WorkTime:fetch_or(null),
            RemainingPowerPercent = obj.PerRemCap:fetch_or(null),
            RemainingHealthPercent = obj.StateOfHealth:fetch_or(null)
        }
        return acc
    end, {}):fetch_or()
end

local function GetTotalRemainCapacitywattHour()
    local RateCap = 0
    local StateOfHealth = 0
    local remaincapacitywattHour = 0
    local totalremaincapacitywattHour = 0

    C.BBUModule():fold(function(obj)
        local bburatecap = 0
        RateCap = obj.RateCap:fetch_or(0)
        StateOfHealth = obj.StateOfHealth:fetch_or(0)
        remaincapacitywattHour = remaincapacitywattHour + RateCap * StateOfHealth / 100
    end)
    
    totalremaincapacitywattHour = (remaincapacitywattHour * 100 - ((remaincapacitywattHour * 100) % 100)) / 100

    return totalremaincapacitywattHour
end

local function GetTotalRatedCapacitywattHour()
    local totalratedCapacitywattHour = 0
    C.BBUModule():fold(function(obj)
        local RateCap = obj.RateCap:fetch_or(0)
        totalratedCapacitywattHour = totalratedCapacitywattHour + RateCap
    end)
    return totalratedCapacitywattHour
end

function M.GetBatteryInfo()
    return {
        BBUInfo = GetBatteryProperty(),
        RatedCapacityWattHour = GetTotalRatedCapacitywattHour(),
        RemainCapacityWattHour = GetTotalRemainCapacitywattHour()
    }
end  

return M