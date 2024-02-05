local dflib = require "dflib"
local c = require "dflib.core"
local utils = require "utils"
local http = require "http"
local cjson = require "cjson"
local logging = require "logging"
local bit = require "bit"
local C = dflib.class
local O = dflib.object
local null = cjson.null
local read_history = utils.read_history
local EVENT_WORK_STATE_S = c.EVENT_WORK_STATE_S
local rf_string_check = utils.rf_string_check
local call_method = utils.call_method
local call_remote_method2_async = utils.call_remote_method2_async
local reply_internal_server_error = http.reply_internal_server_error
local reply_ok = http.reply_ok
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_error = http.reply_error
local reply_bad_request = http.reply_bad_request
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local new_fixed_array = GVariant.new_fixed_array
local HTTP_METHOD_NOT_IMPLEMENTED = http.status.HTTP_METHOD_NOT_IMPLEMENTED
local get_system_name = utils.get_system_name

local SENSOR_TYPE_FAN = 4
local PRODUCT_ID_TIANCHI = 0

local ERROR_ENVARRSIZE_INVALID_SPDARRSIZE = 0xf003 -- 温度区间数组与转速数组不满足小于1条件错误码
local ERROR_ENVRANGE_NONASC = 0xf004 -- 温度区间数组不满足升序条件错误码
local ERROR_SPDARR_NONASC = 0xf005 -- 转速数组不满足升序条件错误码
local ERROR_UNENABLE_SMARTMODE = 0xf008 -- 不支持smart cooling模式错误码
local ERROR_ENVARR_LEN_INVALID = 0xf00e -- 环境温度数组长度不合理
local ERROR_ENVARR_VALUE_INVALID = 0xf00f -- 环境温度数组值超出指定范围
local ERROR_SPDARR_VALUE_INVALID = 0xf010 -- 风扇数组值超出指定范围
local ERROR_SETTING_UNSUPPORT = 0xf013 -- 当前环境不支持设置功能
local notSupported = reply_error(HTTP_METHOD_NOT_IMPLEMENTED, "PropertyModificationNotSupported")
local thermalErrMap = {
    [ERROR_UNENABLE_SMARTMODE] = notSupported,
    [ERROR_SETTING_UNSUPPORT] = notSupported,
    [ERROR_ENVRANGE_NONASC] = reply_bad_request("InvalidTemperatureRange"),
    [ERROR_SPDARR_NONASC] = reply_bad_request("InvalidFanSpeed"),
    [ERROR_ENVARRSIZE_INVALID_SPDARRSIZE] = reply_bad_request("WrongValueQty"),
    [ERROR_ENVARR_VALUE_INVALID] = reply_bad_request("PropertyValueOutOfRange"),
    [ERROR_SPDARR_VALUE_INVALID] = reply_bad_request("PropertyValueOutOfRange"),
    [ERROR_ENVARR_LEN_INVALID] = reply_bad_request("PropertyValueExceedsMaxLength")
}

-- 偏移地址，与Class中CustomSettingMask定义一致
local COOLING_CUSTOMMODE_INLETTEMP = 0
local COOLING_CUSTOMMODE_CUPTEMP = 1
local COOLING_CUSTOMMODE_OUTTEMP = 2
local COOLING_CUSTOMMODE_MEMORY = 5
local COOLING_CUSTOMMODE_NPUHBM_TEMP = 9
local COOLING_CUSTOMMODE_NPUAICORE_TEMP = 10
local COOLING_CUSTOMMODE_NPUBOARD_TEMP = 11
local COOLING_CUSTOMMODE_SOCINLET_TEMP = 12
local COOLING_CUSTOMMODE_SOCOUTLET_TEMP = 13

local COOLING_MEDIUM_AIR = 0
local COOLING_MEDIUM_LIQUID = 1
local function MediumToStr(v)
    if v == COOLING_MEDIUM_AIR then
        return "AirCooled"
    elseif v == COOLING_MEDIUM_LIQUID then
        return "LiquidCooled"
    end
end

local COOLING_ENERGY_SAVING_MODE = 0x10
local COOLING_LOW_NOISE_MODE = 0x11
local COOLING_HIGH_PERFORMANCE_MODE = 0x12
local COOLING_USER_DEFINED_MODE = 0x13
local COOLING_LIQUID_MODE = 0x14
local function SmartModeToStr(v)
    if v == COOLING_ENERGY_SAVING_MODE then
        return "EnergySaving"
    elseif v == COOLING_LOW_NOISE_MODE then
        return "LowNoise"
    elseif v == COOLING_HIGH_PERFORMANCE_MODE then
        return "HighPerformance"
    elseif v == COOLING_USER_DEFINED_MODE then
        return "Custom"
    elseif v == COOLING_LIQUID_MODE then
        return "LiquidCooling"
    end
end

local function SmartModeStrToVal(v)
    if v == "EnergySaving" then
        return COOLING_ENERGY_SAVING_MODE
    elseif v == "LowNoise" then
        return COOLING_LOW_NOISE_MODE
    elseif v == "HighPerformance" then
        return COOLING_HIGH_PERFORMANCE_MODE
    elseif v == "Custom" then
        return COOLING_USER_DEFINED_MODE
    elseif v == "LiquidCooling" then
        return COOLING_LIQUID_MODE
    end
    error("invalid cooling mode")
end

local function makeFanName(fanId, FOrR, isTwins)
    if isTwins then
        if FOrR == "F" then
            return "Fan" .. fanId .. " Front"
        elseif FOrR == "R" then
            return "Fan" .. fanId .. " Rear"
        end
    end
    return "Fan" .. fanId
end

local FANTYPE_F_SPEED = "Fspeed"
local FANTYPE_R_SPEED = "Rspeed"
local FAN_PROPERTY_FRPM = "FRPM"
local FAN_PROPERTY_RRPM = "RRPM"

local function GetFanCurrentSpeed(obj, F_Or_R)
    local currentSpeedProp
    if F_Or_R == "F" then
        currentSpeedProp = FAN_PROPERTY_FRPM
    elseif F_Or_R == "R" then
        currentSpeedProp = FAN_PROPERTY_RRPM
    else
        currentSpeedProp = FAN_PROPERTY_FRPM
    end
    return obj[currentSpeedProp]:fetch_or(0)
end

local function get_fan_speed_percent(obj, FOrR)
    local maxSpeedProp
    if FOrR == "F" then
        maxSpeedProp = FANTYPE_F_SPEED
    elseif FOrR == "R" then
        maxSpeedProp = FANTYPE_R_SPEED
    else
        maxSpeedProp = FANTYPE_F_SPEED
    end
    local Model = obj.Model:fetch()
    local currentFanSpeed = GetFanCurrentSpeed(obj, FOrR)
    return C.CLASSFANTYPE("Name", Model):next(function(fanTypeObj)
        local maxFanSpeed = fanTypeObj[maxSpeedProp]:fetch()
        assert(maxFanSpeed > 0)
        return math.floor(currentFanSpeed * 100 / maxFanSpeed)
    end):fetch_or()
end

local CPU_TJMAX_MIN_VAL = 50
local CPU_TJMAX_MAX_VAL = 255
local CPU_TOBJ_MAX_VAL = 80
local function set_smart_cooling_mode_cpu(user, cpuValue)
    local ok, reply = pcall(function()
        local validMinValue = 0
        local validMaxValue = CPU_TOBJ_MAX_VAL
        local coolingObj = O.Cooling
        local cpuMaxValue = C.MeInfo[0].CpuTjmax:fetch_or()
        if cpuMaxValue and CPU_TJMAX_MAX_VAL >= cpuMaxValue and CPU_TJMAX_MIN_VAL <= cpuMaxValue then
            validMaxValue = cpuMaxValue - coolingObj.CpuCoremTobjTjmaxDiff:fetch()
            if CPU_TJMAX_MIN_VAL > validMaxValue then
                logging:error("cpu valid max value error: CPU_TJMAX_MIN_VAL=%d, validMaxValue = %d",
                    CPU_TJMAX_MIN_VAL, validMaxValue)
                error("cpu tjmax - tjmaxdiff value error")
            end
            validMinValue = coolingObj.CustomCpuCoremTobjRangeMin:fetch()
        end
        if cpuValue < validMinValue or cpuValue > validMaxValue then
            return reply_bad_request("PropertyValueOutOfRange", {validMinValue, validMaxValue}, "CPUTargetTemperatureCelsius")
        end
        local ok, reply = call_method(user, coolingObj, "SetCpuTobj", thermalErrMap, gbyte(cpuValue))
        if not ok then
            return reply
        end
    end)
    if not ok then
        return reply_internal_server_error(nil, reply)
    end
    return reply or reply_ok()
end

local function call_remote_method(user, coolingObj, method_name, thermalErrMap, mode)
    if O.PRODUCT.MgmtSoftWareType:fetch() == 0x23 then --pangeav6 pacific--
        return call_remote_method2_async(user, coolingObj, method_name, thermalErrMap, mode)
    else
        return call_method(user, coolingObj, method_name, thermalErrMap, mode)
    end
end


local AIR_COOLING_TYPE = 0
local LIQUID_COOLING_TYPE = 1
local function set_smart_cooling_mode(user, Mode)
    local mode = SmartModeStrToVal(Mode)
    return C.Cooling[0]:next(function(obj)
        return obj.Medium:next(function(coolingMedium)
            if coolingMedium == AIR_COOLING_TYPE then
                -- 风冷模式仅支持0x10到0x13
                return mode <= COOLING_USER_DEFINED_MODE and mode >= COOLING_ENERGY_SAVING_MODE
            elseif coolingMedium == LIQUID_COOLING_TYPE then
                -- 液冷模式仅支持0x13到0x14
                return mode <= COOLING_LIQUID_MODE and mode >= COOLING_USER_DEFINED_MODE
            end
            return true
        end):next(function(ok)
            if not ok then
                return reply_bad_request("PropertyValueNotInList")
            end
            local ok, ret = call_remote_method(user, obj, "SetSmartCoolingMode", thermalErrMap, gbyte(mode))
            if not ok then
                return ret
            end
            return reply_ok()
        end)
    end):fetch()
end

local function set_smart_cooling_mode_outlet(user, outletValue)
    local ok, reply = pcall(function()
        local minValue = 0
        local maxValue = 0
        local coolingObj = O.Cooling
        coolingObj:next(function(obj)
            return obj.CustomOutletTobjRange
        end):next(function(range)
            minValue = range[1]
            maxValue = range[2]
        end):catch()
        if outletValue < minValue or outletValue > maxValue then
            return reply_bad_request("PropertyValueOutOfRange", {minValue, maxValue}, "OutletTargetTemperatureCelsius")
        end
        local ok, reply = call_remote_method(user, coolingObj, "SetOutletTobj", thermalErrMap, gbyte(outletValue))
        if not ok then
            return reply
        end
    end)
    if not ok then
        return reply_internal_server_error(nil, reply)
    end
    return reply or reply_ok()
end

local function set_smart_cooling_mode_memory(user, memoryValue)
    local ok, reply = pcall(function()
        local minValue = 0
        local maxValue = 0
        local coolingObj = O.Cooling
        coolingObj:next(function(obj)
            return obj.CustomMemoryTobjRange
        end):next(function(range)
            minValue = range[1]
            maxValue = range[2]
        end):catch()
        if memoryValue < minValue or memoryValue > maxValue then
            return reply_bad_request("PropertyValueOutOfRange", {minValue, maxValue}, "MemoryTargetTemperatureCelsius")
        end
        local ok, reply = call_method(user, coolingObj, "SetMemoryTobj", thermalErrMap, gbyte(memoryValue))
        if not ok then
            return reply
        end
    end)
    if not ok then
        return reply_internal_server_error(nil, reply)
    end
    return reply or reply_ok()
end

local function set_smart_cooling_mode_npu_hbm(user, npuHbmValue)
    local ok, reply = pcall(function()
        local minValue = 0
        local maxValue = 0
        local coolingObj = O.Cooling
        coolingObj:next(function(obj)
            return obj.CustomNpuHbmTobjRange
        end):next(function(range)
            minValue = range[1]
            maxValue = range[2]
        end):catch()
        if npuHbmValue < minValue or npuHbmValue > maxValue then
            return reply_bad_request("PropertyValueOutOfRange", {minValue, maxValue}, "NPUHbmTargetTemperatureCelsius")
        end
        local ok, reply = call_method(user, coolingObj, "SetNPUHbmTobj", thermalErrMap, gbyte(npuHbmValue))
        if not ok then
            return reply
        end
    end)
    if not ok then
        return reply_internal_server_error(nil, reply)
    end
    return reply or reply_ok()
end

local function set_smart_cooling_mode_npu_ai_core(user, npuAiCoreValue)
    local ok, reply = pcall(function()
        local minValue = 0
        local maxValue = 0
        local coolingObj = O.Cooling
        coolingObj:next(function(obj)
            return obj.CustomNpuAiCoreTobjRange
        end):next(function(range)
            minValue = range[1]
            maxValue = range[2]
        end):catch()
        if npuAiCoreValue < minValue or npuAiCoreValue > maxValue then
            return reply_bad_request("PropertyValueOutOfRange", {minValue, maxValue}, "NPUAiCoreTargetTemperatureCelsius")
        end
        local ok, reply = call_method(user, coolingObj, "SetNPUAiCoreTobj", thermalErrMap, gbyte(npuAiCoreValue))
        if not ok then
            return reply
        end
    end)
    if not ok then
        return reply_internal_server_error(nil, reply)
    end
    return reply or reply_ok()
end

local function set_smart_cooling_mode_npuboard_temp(user, npuBoardValue)
    local ok, reply = pcall(function()
        local minValue = 0
        local maxValue = 0
        local coolingObj = O.Cooling
        coolingObj:next(function(obj)
            return obj.CustomNpuBoardTobjRange
        end):next(function(range)
            minValue = range[1]
            maxValue = range[2]
        end):catch()
        if npuBoardValue < minValue or npuBoardValue > maxValue then
            return reply_bad_request("PropertyValueOutOfRange", {minValue, maxValue}, "NPUBoardTargetTemperatureCelsius")
        end
        local ok, reply = call_method(user, coolingObj, "SetNPUBoardTobj", thermalErrMap, gbyte(npuBoardValue))
        if not ok then
            return reply
        end
    end)
    if not ok then
        return reply_internal_server_error(nil, reply)
    end
    return reply or reply_ok()
end

local function set_smart_cooling_mode_soc_board_inlet(user, socInletValue)
    local minValue = 0
    local maxValue = 0
    local coolingObj = O.Cooling
    coolingObj:next(function(obj)
        return obj.CustomSoCBoardInletTobjRange
    end):next(function(range)
        minValue = range[1]
        maxValue = range[2]
    end):catch()
    if socInletValue < minValue or socInletValue > maxValue then
        return reply_bad_request("PropertyValueOutOfRange", {minValue, maxValue}, "SoCBoardInletTargetTemperatureCelsius")
    end
    local ok, reply = call_method(user, coolingObj, "SetSoCBoardInletTobj", thermalErrMap, gbyte(socInletValue))
    if not ok then
        return reply
    end
    return reply_ok()
end
 
local function set_smart_cooling_mode_soc_board_outlet(user, socOutletValue)
    local minValue = 0
    local maxValue = 0
    local coolingObj = O.Cooling
    coolingObj:next(function(obj)
        return obj.CustomSoCBoardOutletTobjRange
    end):next(function(range)
        minValue = range[1]
        maxValue = range[2]
    end):fetch()
    if socOutletValue < minValue or socOutletValue > maxValue then
        return reply_bad_request("PropertyValueOutOfRange", {minValue, maxValue}, "SoCBoardOutletTargetTemperatureCelsius")
    end
    local ok, reply = call_method(user, coolingObj, "SetSoCBoardOutletTobj", thermalErrMap, gbyte(socOutletValue))
    if not ok then
        return reply
    end
    return reply_ok()
end

local function set_smart_cooling_env_temp(user, FanSpeedPercents)
    local cooling = C.Cooling[0]
    local smartCoolingEnable = cooling.SmartCoolingEnable:fetch_or(0)
    local inletSupport = cooling.CustomSettingMask:next(function(mask)
        return bit.band(bit.rshift(mask, COOLING_CUSTOMMODE_INLETTEMP), 0x01)
    end):fetch_or(0)
    if smartCoolingEnable == 0 or inletSupport == 0 then
        return notSupported
    end

    return C.Policy1Class("Index", 0x0100):next(function(obj)
        local gEnvRangeRdL = {}
        for i, v in ipairs(obj.EnvRangeRdL:fetch()) do
            if i > 1 then
                gEnvRangeRdL[#gEnvRangeRdL + 1] = gbyte(v)
            end
        end

        local gFanSpeedPercents = {}
        for _, v in ipairs(FanSpeedPercents) do
            gFanSpeedPercents[#gFanSpeedPercents + 1] = gbyte(v)
        end

        local input_list = {new_fixed_array(gEnvRangeRdL), new_fixed_array(gFanSpeedPercents)}
        local ok, reply = call_method(user, obj, "SetEnvTempSpd", thermalErrMap, input_list)
        if not ok then
            return reply
        end
        return reply_ok()
    end):catch(function(err)
        return reply_internal_server_error(nil, err)
    end):fetch()
end

local function get_prop_with_mask(coolingObj, type, propName)
    return coolingObj.CustomSettingMask:next(function(settingMask)
        if bit.band(bit.rshift(settingMask, type), 0x01) == 0 then
            return null
        end
        return coolingObj[propName]
    end):fetch_or(null)
end

local function get_value_with_mask(coolingObj, type, value)
    return coolingObj.CustomSettingMask:next(function(settingMask)
        if bit.band(bit.rshift(settingMask, type), 0x01) == 0 then
            return null
        end
        return value
    end):fetch_or(null)
end

local function GetThermalHistory()
    local sys_name = get_system_name()
    if sys_name == 'RMM' then
        return nil
    end
    local InletTemperature = C.ThresholdSensor("InnerName", "Inlet_TEMP"):next(
        function(obj)
            return obj.ReadingStatus:next(function(reading_status)
                return EVENT_WORK_STATE_S(reading_status)
            end):next(function(sensor_reading_status)
                if sensor_reading_status.disable_scanning == 1 and sensor_reading_status.disable_scanning_local == 1 and
                    sensor_reading_status.disable_all == 1 and sensor_reading_status.initial_update_progress == 1 and
                    sensor_reading_status.disable_access_error == 1 then
                    return string.format('%.2f', obj.ReaddingConvert:fetch_or(0))
                else
                    return null
                end
            end)
        end):fetch_or(null)

    history = read_history("env_web_view.dat", function(fields)
        return {Time = fields[1], InletTempCelsius = fields[2]}
    end)
    return {CurrentValue = InletTemperature, History = history}
end

local function get_envrange_with_index(policyIdex)
    return C.Policy1Class():fold(function(obj)
        if obj.Index:fetch() == policyIdex then
            local EnvRangeRdL = {}
            for i, v in ipairs(obj.EnvRangeRdL:fetch()) do
                if i > 1 then
                    EnvRangeRdL[#EnvRangeRdL + 1] = v
                end
            end
            return {EnvRangeRdL, obj.EnvRangeSpdH:fetch()}, false
        end
    end):fetch_or() or {null, null}
end

local function support_fantwins()
    local ProductId = O.PRODUCT.ProductId:fetch_or(null)
    if (ProductId == PRODUCT_ID_TIANCHI) then
        return true
    else
        return false
    end
end

local M = {}

function M.GetThermal()
    local SmartCooling = nil
    local sys_name = get_system_name()
    if sys_name ~= 'RMM' then
        SmartCooling = O.Cooling:next(function(obj)
            local CustomOutletTobjRange = obj.CustomOutletTobjRange:fetch()
            local CustomMemoryTobjRange = obj.CustomMemoryTobjRange:fetch()
            local CustomNPUHbmTobjRange = obj.CustomNpuHbmTobjRange:fetch()
            local CustomNPUAiCoreTobjRange = obj.CustomNpuAiCoreTobjRange:fetch()
	    local CustomNPUBoardTobjRange = obj.CustomNpuBoardTobjRange:fetch()
            local CustomSoCBoardInletTobjRange = obj.CustomSoCBoardInletTobjRange:fetch()
            local CustomSoCBoardOutletTobjRange = obj.CustomSoCBoardOutletTobjRange:fetch()
            local EnvRange = {null, null}
            local mode = null
            local customConfig = null
            local fanSpeedPercentRange = {null, null}
            local settingMask = C.Cooling[0].CustomSettingMask:fetch_or(0)

            if bit.band(bit.rshift(settingMask, COOLING_CUSTOMMODE_INLETTEMP), 0x01) == 1 then
                fanSpeedPercentRange = obj.CustomInletSpeedRange:fetch()
                EnvRange = get_envrange_with_index(0x0100)
            end

            if obj.SmartCoolingEnable:fetch_or(0) == 1 then
                mode = SmartModeToStr(obj.DefThermalLevel:fetch_or(null))
                customConfig = {
                    CPUTargetTemperatureCelsius = get_prop_with_mask(obj, COOLING_CUSTOMMODE_CUPTEMP,
                        "CustomCpuCoremTobj"),
                    OutletTargetTemperatureCelsius = get_prop_with_mask(obj, COOLING_CUSTOMMODE_OUTTEMP,
                        "CustomOutletTobj"),
                    MemoryTargetTemperatureCelsius = get_prop_with_mask(obj, COOLING_CUSTOMMODE_MEMORY,
                        "CustomMemoryTobj"),
                    NPUHbmTargetTemperatureCelsius = get_prop_with_mask(obj, COOLING_CUSTOMMODE_NPUHBM_TEMP,
                        "CustomNpuHbmTobj"),
                    NPUAiCoreTargetTemperatureCelsius = get_prop_with_mask(obj, COOLING_CUSTOMMODE_NPUAICORE_TEMP,
                        "CustomNpuAiCoreTobj"),
		    NPUBoardTargetTemperatureCelsius = get_prop_with_mask(obj, COOLING_CUSTOMMODE_NPUBOARD_TEMP,
                        "CustomNpuBoardTobj"),
                    MinCPUTargetTemperatureCelsius = get_prop_with_mask(obj, COOLING_CUSTOMMODE_CUPTEMP,
                        "CustomCpuCoremTobjRangeMin"),
                    MaxCPUTargetTemperatureCelsius = get_prop_with_mask(obj, COOLING_CUSTOMMODE_CUPTEMP,
                        "CustomCpuCoremTobjRangeMax"),
                    SoCBoardInletTargetTemperatureCelsius = get_prop_with_mask(obj, COOLING_CUSTOMMODE_SOCINLET_TEMP,
                        "CustomSoCBoardInletTobj"),
                    SoCBoardOutletTargetTemperatureCelsius = get_prop_with_mask(obj, COOLING_CUSTOMMODE_SOCOUTLET_TEMP,
                        "CustomSoCBoardOutletTobj"),
                    MinOutletTargetTemperatureCelsius = get_value_with_mask(obj, COOLING_CUSTOMMODE_OUTTEMP,
                        CustomOutletTobjRange[1]),
                    MaxOutletTargetTemperatureCelsius = get_value_with_mask(obj, COOLING_CUSTOMMODE_OUTTEMP,
                        CustomOutletTobjRange[2]),
                    MinMemoryTargetTemperatureCelsius = get_value_with_mask(obj, COOLING_CUSTOMMODE_MEMORY,
                        CustomMemoryTobjRange[1]),
                    MaxMemoryTargetTemperatureCelsius = get_value_with_mask(obj, COOLING_CUSTOMMODE_MEMORY,
                        CustomMemoryTobjRange[2]),
                    NPUHbmMinTargetTemperatureCelsius = get_value_with_mask(obj, COOLING_CUSTOMMODE_NPUHBM_TEMP,
                        CustomNPUHbmTobjRange[1]),
                    NPUHbmMaxTargetTemperatureCelsius = get_value_with_mask(obj, COOLING_CUSTOMMODE_NPUHBM_TEMP,
                        CustomNPUHbmTobjRange[2]),
                    NPUAiCoreMinTargetTemperatureCelsius = get_value_with_mask(obj, COOLING_CUSTOMMODE_NPUAICORE_TEMP,
                        CustomNPUAiCoreTobjRange[1]),
                    NPUAiCoreMaxTargetTemperatureCelsius = get_value_with_mask(obj, COOLING_CUSTOMMODE_NPUAICORE_TEMP,
                        CustomNPUAiCoreTobjRange[2]),
		    NPUBoardMinTargetTemperatureCelsius = get_value_with_mask(obj, COOLING_CUSTOMMODE_NPUBOARD_TEMP,
                        CustomNPUBoardTobjRange[1]),
		    NPUBoardMaxTargetTemperatureCelsius = get_value_with_mask(obj, COOLING_CUSTOMMODE_NPUBOARD_TEMP,
                        CustomNPUBoardTobjRange[2]),
                    SoCBoardInletMinTargetTemperatureCelsius = get_value_with_mask(obj, COOLING_CUSTOMMODE_SOCINLET_TEMP,
                        CustomSoCBoardInletTobjRange[1]),
                    SoCBoardInletMaxTargetTemperatureCelsius = get_value_with_mask(obj, COOLING_CUSTOMMODE_SOCINLET_TEMP,
                        CustomSoCBoardInletTobjRange[2]),
                    SoCBoardOutletMinTargetTemperatureCelsius = get_value_with_mask(obj, COOLING_CUSTOMMODE_SOCOUTLET_TEMP,
                        CustomSoCBoardOutletTobjRange[1]),
                    SoCBoardOutletMaxTargetTemperatureCelsius = get_value_with_mask(obj, COOLING_CUSTOMMODE_SOCOUTLET_TEMP,
                        CustomSoCBoardOutletTobjRange[2]),
                    MinFanSpeedPercents = fanSpeedPercentRange[1],
                    MaxFanSpeedPercents = fanSpeedPercentRange[2],
                    TemperatureRangeCelsius = EnvRange[1],
                    FanSpeedPercents = EnvRange[2]
                }
            end

            return {CoolingMedium = MediumToStr(obj.Medium:fetch_or(0xFF)), Mode = mode, CustomConfig = customConfig}
        end):fetch_or(null)
    end

    -- 如果风扇个数和传感器个数一致则为非双胞胎风扇，否则为双胞胎风扇(与SNMP的判断方式一致)
    local fanCount = 0
    local fanObjs = C.FANClass():fold(function(obj, acc)
        acc[obj.FANID:fetch()] = obj
        fanCount = fanCount + 1
        fanModel = obj.Model:next(rf_string_check):fetch_or(null)
        return acc
    end, {}):fetch()

    local fanSensorCount = 0
    local fanTwins = C.CLASSFANTYPE("Name", fanModel).Twins:fetch_or(null)
    local fanSensors = C.ThresholdSensor():fold(function(obj, acc)
        if obj.SensorType:fetch() == SENSOR_TYPE_FAN then
            local InnerName = obj.InnerName:fetch()
            local fanId, FOrR = InnerName:match("^FAN(%d)_([^_]+)")
            if fanId then
                if (not(support_fantwins() == true and fanTwins == 0 and FOrR == "R")) then
                    acc[#acc + 1] = {obj = obj, FOrR = FOrR, FanId = tonumber(fanId)}
                end
                fanSensorCount = fanSensorCount + 1
            end
        end
        return acc
    end, {}):fetch()

    if (support_fantwins() == true) then
        if fanTwins == 1 then
            isTwins = true
        else
            isTwins = false
        end
    else
        isTwins = fanCount ~= fanSensorCount
    end

    local fans = {}
    for _, fanSensor in ipairs(fanSensors) do
        local fanObj = fanObjs[fanSensor.FanId]
        if fanObj then
            if O.PRODUCT.MgmtSoftWareType:fetch_or(null) == 0x24 then
                fans[#fans + 1] = {
                    Name = "Fan" .. fanObj.Location:fetch_or(null),
                    Model = fanObj.Model:next(rf_string_check):fetch_or(null),
                    Speed = fanObj[FAN_PROPERTY_RRPM]:fetch_or(0),
                    SpeedRatio = get_fan_speed_percent(fanObj, fanSensor.FOrR),
                    PartNumber = fanObj.PartNum:fetch()
                }
            else
                local FanName = makeFanName(fanSensor.FanId, fanSensor.FOrR, isTwins)
                fans[#fans + 1] = {
                    Name = FanName,
                    Model = fanObj.Model:next(rf_string_check):fetch_or(null),
                    Speed = GetFanCurrentSpeed(fanObj, fanSensor.FOrR),
                    SpeedRatio = get_fan_speed_percent(fanObj, fanSensor.FOrR),
                    PartNumber = fanObj.PartNum:fetch(),
                    Presence = fanObj.Presence:fetch()
                }
            end
        end
    end

    return {
        SmartCooling = SmartCooling,
        Inlet = GetThermalHistory(),
        Fans = fans
    }
end

function M.UpdateThermal(data, user)
    local result = reply_ok()
    if data.SmartCooling and data.SmartCooling.Mode then
        result:join(set_smart_cooling_mode(user, data.SmartCooling.Mode))
    end

    if data.SmartCooling and data.SmartCooling.CustomConfig then
        local CustomConfig = data.SmartCooling.CustomConfig

        if CustomConfig.CPUTargetTemperatureCelsius then
            result:join(set_smart_cooling_mode_cpu(user, CustomConfig.CPUTargetTemperatureCelsius))
        end

        if CustomConfig.OutletTargetTemperatureCelsius then
            result:join(set_smart_cooling_mode_outlet(user, CustomConfig.OutletTargetTemperatureCelsius))
        end

        if CustomConfig.MemoryTargetTemperatureCelsius then
            result:join(set_smart_cooling_mode_memory(user, CustomConfig.MemoryTargetTemperatureCelsius))
        end

        if CustomConfig.NPUHbmTargetTemperatureCelsius then
            result:join(set_smart_cooling_mode_npu_hbm(user, CustomConfig.NPUHbmTargetTemperatureCelsius))
        end

        if CustomConfig.NPUAiCoreTargetTemperatureCelsius then
            result:join(set_smart_cooling_mode_npu_ai_core(user, CustomConfig.NPUAiCoreTargetTemperatureCelsius))
        end

        if CustomConfig.NPUBoardTargetTemperatureCelsius then
            result:join(set_smart_cooling_mode_npuboard_temp(user, CustomConfig.NPUBoardTargetTemperatureCelsius))
        end

        if CustomConfig.FanSpeedPercents then
            result:join(set_smart_cooling_env_temp(user, CustomConfig.FanSpeedPercents))
        end
        
        if CustomConfig.SoCBoardInletTargetTemperatureCelsius then
            result:join(set_smart_cooling_mode_soc_board_inlet(user, CustomConfig.SoCBoardInletTargetTemperatureCelsius))
        end
 
        if CustomConfig.SoCBoardOutletTargetTemperatureCelsius then
            result:join(set_smart_cooling_mode_soc_board_outlet(user, CustomConfig.SoCBoardOutletTargetTemperatureCelsius))
        end
    end

    if result:isOk() then
        return reply_ok_encode_json(M.GetThermal())
    end
    result:appendErrorData(M.GetThermal())
    return result
end

function M.ClearInletHistory(user)
    return C.EnvRecord[0]:next(function(obj)
        local ok, ret = call_method(user, obj, "ClearHistoryRecord", thermalErrMap)
        if not ok then
            return ret
        end
        return reply_ok()
    end):fetch()
end

M.set_smart_cooling_mode = set_smart_cooling_mode

M.SmartModeToStr = SmartModeToStr

return M
