local dflib = require 'dflib'
local c = require 'dflib.core'
local utils = require 'utils'
local cjson = require 'cjson'
local bit = require 'bit'
local http = require 'http'
local thermal = require 'smm_thermal'
local power = require 'power'
local C = dflib.class
local O = dflib.object
local null = cjson.null
local reply_bad_request = http.reply_bad_request
local call_method = utils.call_method
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local reply_ok = http.reply_ok
local reply_internal_server_error = http.reply_internal_server_error
local guint16 = GVariant.new_uint16
local get_fru_info = thermal.get_fru_info
local AcDcToStr = power.AcDcToStr
local GetState = power.GetState


local SET_SLEEPCONFIG_MANUAL_CLOSE = -2
local SET_SLEEPNFIG_CONFIG_ERR = 1
local SET_SLEEPCONFIG_UNKNOW_ERR = -1
local UPDATE_SLEEP_MODE_ERR_MAP = {
    [SET_SLEEPCONFIG_MANUAL_CLOSE] = reply_bad_request(
        'PowerSetSleepConfigManualClose',
        'Failed to set the PSU hibernation mode because the PSU has been manually turned off.'
    ),
    [SET_SLEEPNFIG_CONFIG_ERR] = reply_bad_request(
        'PowerSetSleepConfigConfigErr', 'The current PSU configuration does not support hibernation.'
    ),
    [SET_SLEEPCONFIG_UNKNOW_ERR] = reply_internal_server_error(),
}

local RET_OK = 0
local function CatchError(ok, ret, errMap, action)
    if not ok then
        return ret
    end
    if errMap[ret[1]] then
        return errMap[ret[1]]:clone()
    end
    if ret[1] ~= nil and ret[1] ~= RET_OK then
        return reply_bad_request('PropertyNotModified', 'Fail to ' .. action)
    end
end

local PROTOCOL_TCE_PMBUS = 7
local PROTERY_PS_INPUTMODE_VALUE_DC = 0
local PROTERY_PS_INPUTMODE_VALUE_AC = 1
local PROTERY_PS_INPUTMODE_VALUE_AC_OR_DC = 2

local PEM_FRU_POSITION_BASE = 20

local M = {}
function M.GetChassisPowerSupply()
    local psNum = O.ProductComponent.PsNum:fetch()
    local powerSupply = {}
    for i = 1, psNum do
        local powerName = string.format('PSU%d', i)
        C.OnePower('DeviceName', powerName):next(function(obj)
            return {
                Name = powerName,
                Health = utils.get_property_and_severity(obj.Health:fetch_or(null)),
                PowerCapacityWatts = obj.CommunicationState:neq(1):next(function()
                    return obj.PsRate:fetch_or(null)
                end):fetch_or(null),
                PowerOutputWatts = obj.CommunicationState:neq(1):next(function()
                    return obj.Power:fetch_or(null)
                end):fetch_or(null),
                SupplyType = obj.AcDc:next(AcDcToStr):fetch_or(null),
                FirmwareVersion = obj.PsFwVer:fetch_or(null),
                HardwareVersion = obj.PsVer:fetch_or(null),
                SleepEnabled = C.PsSleep[0].SleepStatusBitmap:next(function(state)
                    local psuState = bit.band(bit.rshift(state, i - 1), 0x0001)
                    return utils.ValueToBool(psuState)
                end):fetch_or(null),
                State = GetState(obj),
                Fru = get_fru_info(i - 1, PEM_FRU_POSITION_BASE),
            }
        end):catch(function()
            return {
                Name = powerName,
                Health = null,
                PowerCapacityWatts = null,
                PowerOutputWatts = null,
                SupplyType = null,
                FirmwareVersion = null,
                HardwareVersion = null,
                State = 'Absent',
                SleepEnabled = null,
                Fru = null,
            }
        end):next(function(v)
            powerSupply[#powerSupply + 1] = v
        end)
    end

    return {
        SleepModeEnabled = utils.ValueToBool(C.PsSleep[0].Enable:fetch()),
        SleepMode = C.PsSleep[0].Enable:eq(1):next(function()
            if C.PsSleep[0].Mode:fetch() == 0 then
                return 'N+1'
            else
                return 'N+N'
            end
        end):fetch_or(nil),
        SupplyList = powerSupply,
    }
end

local BLADE_TYPE_COMPUTE_NODE = 0
local EMM_DEFAULT_MULTI = 2
local function is_compute_node(obj)
    if obj.BladeType:fetch() ~= BLADE_TYPE_COMPUTE_NODE then
        return false
    end
    local base = C.IpmbAddr[0].SlaveAddrBase:fetch()
    local multi = C.IpmbAddr[0].SlaveAddrMulti:fetch()
    if multi == 0 then
        multi = EMM_DEFAULT_MULTI
    end
    local blade_slot = (obj.SlaveAddr:fetch() / multi) - base
    if blade_slot == 37 or blade_slot == 38 then
        return false
    end
    return true
end

local function get_minimum_power_value(blade_list)
    local demand_value = 0
    local extra_value = 0
    local set_manually_value = 0
    local current_power = 0
    for _, v in pairs(blade_list) do
        if v.IsManual then
            if v.LimitInWatts < 65535 - demand_value then
                demand_value = demand_value + v.LimitInWatts
                set_manually_value = set_manually_value + v.LimitInWatts
            else
                return null
            end
        else
            if v.MinimumValueWatts < 65535 - demand_value then
                demand_value = demand_value + v.MinimumValueWatts
            else
                return null
            end
        end
        current_power = current_power + v.PowerConsumedWatts
    end
    local total_power = O.syspower.Power:fetch()
    if total_power > current_power then
        extra_value = total_power - current_power
        demand_value = demand_value + extra_value
    else
        return null
    end
    return extra_value + set_manually_value, demand_value
end

local function get_limit_blade_info()
    return C.IpmbEthBlade():fold(function(obj, blade)
        if is_compute_node(obj) == false or obj.Presence:fetch() == 0 then
            return blade
        end
        blade[#blade + 1] = {
            Id = 'Slot' .. obj.Id:fetch_or(null),
            PowerConsumedWatts = obj.CurrentPower:fetch_or(null),
            LimitInWatts = obj.PowerCappingValue:fetch_or(null),
            IsManual = obj.PowerCappingManualState:next(utils.ValueToBool):fetch_or(null),
            LimitException = obj.PowerCappingFailAction:next(function(action)
                if action == 1 then
                    return 'HardPowerOff'
                else
                    return 'NoAction'
                end
            end):fetch_or(null),
            MinimumValueWatts = obj.PowerCappingBaseValue:fetch_or(null),
        }
        return blade
    end, {}):fetch()
end
function M.GetChassisPower()
    local powerObj = O.shelfPowerCapping
    local limitBlade = get_limit_blade_info()
    local minimumValue
    local maximumValue
    local recommendValue
    local ok, ret = call_method(nil, powerObj, 'GetShelfPowerCappingValueRange', nil)
    if ok then
        recommendValue = ret[1]
        maximumValue = ret[2]
        minimumValue = get_minimum_power_value(limitBlade)
    end
    return {
        PowerLimitEnabled = powerObj.Enable:next(utils.ValueToBool):fetch_or(null),
        LimitInWatts = powerObj.Value:fetch_or(null),
        PowerConsumedWatts = powerObj.CurrentPower:fetch_or(null),
        PowerLimitThreshold = powerObj.Threshold:fetch_or(null),
        PowerLimitActivated = powerObj.ActiveState:next(utils.ValueToBool):fetch_or(null),
        PowerLimitMode = powerObj.Mode:next(function(mode)
            if mode == 0 then
                return 'Equal'
            elseif mode == 1 then
                return 'Manual'
            else
                return 'Proportion'
            end
        end):fetch_or(null),
        MinimumValue = minimumValue,
        MaximumValue = maximumValue,
        RecommendMinimumValue = recommendValue,
        PowerLimitBladeInfo = limitBlade,
    }
end

function M.UpdatePowerSleepMode(data, user)
    local enable = 1
    local mode = 0
    local obj = C.PsSleep[0]:next():fetch()
    if data.SleepModeEnabled == nil and data.SleepMode == nil then
        return reply_bad_request('UnrecognizedRequestBody')
    end
    if data.SleepModeEnabled ~= nil then
        enable = utils.BoolToValue(data.SleepModeEnabled)
    end
    if data.SleepMode ~= nil then
        if data.SleepModeEnabled == nil and obj.Enable:fetch() == 0 then
            return reply_bad_request(
                       'PowerModeNotSupport',
                       'Indicates that the PSU hibernate mode cannot be changed because PSU hibernation is not enabled.'
                   )
        end
        if data.SleepMode == 'N+1' then
            mode = 0
        else
            mode = 1
        end
    end
    local ok, ret = call_method(user, obj, 'SetPsSleepConfig', UPDATE_SLEEP_MODE_ERR_MAP, {gbyte(enable), gbyte(mode)})
    if not ok then
        return err
    end
    return reply_ok()
end

local function get_blade_ipmbaddr(blade_num)
    local ipmb_addr = 0x80 + 2 * blade_num
    if ipmb_addr > 255 then
        return nil
    end
    return ipmb_addr
end

local function get_max_blade_power_value(blade_obj)
    local shelf_capping_value = C.ShelfPowerCapping[0].Value:fetch()
    local blade_current_power_total = 0
    local blade_capping_value_manually = 0
    C.IpmbEthBlade():fold(function(obj)
        if not is_compute_node(obj) then
            return
        end
        local state = obj.PowerCappingManualState:next(utils.ValueToBool):fetch()
        local blade_value = obj.PowerCappingValue:fetch()
        local blade_current_power = obj.CurrentPower:fetch()
        if obj.Presence:fetch() == 1 then
            blade_current_power_total = blade_current_power_total + blade_current_power
            if obj == blade_obj then
                return
            end
            if state then
                blade_capping_value_manually = blade_capping_value_manually + blade_value
            end
        end
    end)
    local extra_value = O.syspower.Power:fetch() - blade_current_power_total
    if shelf_capping_value < extra_value + blade_capping_value_manually then
        return false
    end
    return shelf_capping_value - (extra_value + blade_capping_value_manually)
end

local function set_powercontrol_limit_blade(blade, user)
    for _, v in pairs(blade) do
        local id = v.Id:match('^Slot(%d+)$')
        if not id or tonumber(id) > 32 then
            return reply_bad_request('InvalidSlotId', 'Invalid Slot Id')
        end
        local ipmb_addr = get_blade_ipmbaddr(tonumber(id))
        local blade_obj = C.IpmbEthBlade('SlaveAddr', ipmb_addr):next():fetch()
        if blade_obj.Presence:fetch() ~= 1 then
            return reply_bad_request('BladeNotPresent', 'The blade is not present.')
        end
        if v.LimitInWatts then
            local max_value = get_max_blade_power_value(blade_obj)
            if not max_value then
                goto continue
            end
            if max_value > 9999 then
                max_value = 9999
            end
            if v.LimitInWatts > max_value then
                goto continue
            end
            local ok, ret = call_method(user, blade_obj, 'SetPowerCappingValue', nil, {guint16(v.LimitInWatts)})
            if not ok then
                return ret
            end
        end
        if v.LimitException then
            local enable
            if v.LimitException == 'HardPowerOff' then
                enable = 1
            elseif v.LimitException == 'NoAction' then
                enable = 0
            end
            local ok, ret = call_method(user, blade_obj, 'SetFailAction', nil, {gbyte(enable)})
            if not ok then
                return ret
            end
        end
        ::continue::
    end
    return reply_ok()
end

function M.UpdateChassisPower(data, user)
    local powerObj = O.shelfPowerCapping
    if data.LimitInWatts then
        local minimumValue, demandValue = get_minimum_power_value(get_limit_blade_info())
        if demandValue > C.PowerCapping[0].TopValue:fetch() then
            return reply_bad_request('CapacityValueLessThanDemand', 'Capacity value %1 is less than demand value %2.')
        end
        if data.LimitInWatts < minimumValue then
            return reply_bad_request(
                       'PowerCappingValueLessThanAllowed', 'Power capping value %1 is less than allowed value %2.'
                   )
        end
        if data.LimitInWatts > C.PowerCapping[0].TopValue:fetch() then
            return reply_bad_request(
                       'PowerCappingValueGreatThanCapacity', 'Power capping value %1 is great than capacity value %2.'
                   )
        end
        local ok, ret = call_method(user, powerObj, 'SetShelfPowerCappingValue', nil, {guint16(data.LimitInWatts)})
        if not ok then
            return ret
        end
    end
    if data.PowerLimitMode then
        local mode
        if data.PowerLimitMode == 'Equal' then
            mode = 0
        elseif data.PowerLimitMode == 'Manual' then
            mode = 1
        elseif data.PowerLimitMode == 'Proportion' then
            mode = 2
        end
        local ok, ret = call_method(user, powerObj, 'SetShelfPowerCappingMode', nil, {gbyte(mode)})
        if not ok then
            return ret
        end
    end
    if data.PowerLimitEnabled ~= nil then
        local enableState = utils.BoolToValue(data.PowerLimitEnabled)
        local ok, ret = call_method(user, powerObj, 'SetShelfPowerCappingEnable', nil, {gbyte(enableState)})
        if not ok then
            return ret
        end
    end
    if data.PowerLimitThreshold then
        if data.PowerLimitThreshold > powerObj.MaxThreshold:fetch() or data.PowerLimitThreshold > 0xff then
            return reply_bad_request('PropertyValueOutOfRange')
        end
        local ok, ret = call_method(user, powerObj, 'SetShelfPowerCappingThreshod', nil, {gbyte(data.PowerLimitThreshold)})
        if not ok then
            return ret
        end
    end
    if data.PowerLimitBladeInfo then
        local ret = set_powercontrol_limit_blade(data.PowerLimitBladeInfo, user)
        if not ret:isOk() then
            return ret
        end
    end
    return reply_ok(M.GetChassisPower())
end

return M
