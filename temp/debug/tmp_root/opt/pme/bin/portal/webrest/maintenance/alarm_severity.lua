local dflib = require "dflib"
local c = require "dflib.core"
local http = require "http"
local utils = require "utils"
local cfg = require "config"
local reply_ok = http.reply_ok
local reply_ok_encode_json = http.reply_ok_encode_json
local safe_call = http.safe_call
local C = dflib.class
local O = dflib.object
local call_method = utils.call_method
local GVariant = c.GVariant
local gbyte = GVariant.new_byte

local NORMAL_STRING = cfg.NORMAL_STRING
local MINOR_STRING = cfg.MINOR_STRING
local MAJOR_STRING = cfg.MAJOR_STRING
local CRITICAL_STRING = cfg.CRITICAL_STRING

local SEVERITY_NORMAL = cfg.SEVERITY_NORMAL
local SEVERITY_MINOR = cfg.SEVERITY_MINOR
local SEVERITY_MAJOR = cfg.SEVERITY_MAJOR
local SEVERITY_CRITICAL = cfg.SEVERITY_CRITICAL

local function change_severity_num_to_str(v)
    if v == SEVERITY_NORMAL then
        return NORMAL_STRING
    elseif v == SEVERITY_MINOR then
        return MINOR_STRING
    elseif v == SEVERITY_MAJOR then
        return MAJOR_STRING
    elseif v == SEVERITY_CRITICAL then
        return CRITICAL_STRING
    else
        return nil
    end
end

local function change_severity_str_to_num(v)
    if v == NORMAL_STRING then
        return SEVERITY_NORMAL
    elseif v == MINOR_STRING then
        return SEVERITY_MINOR
    elseif v == MAJOR_STRING then
        return SEVERITY_MAJOR
    elseif v == CRITICAL_STRING then
        return SEVERITY_CRITICAL
    else
        return nil
    end
end
local BLADE_TYPE_SWI = 0xC0

local M = {}
function M.GetRemovedEventSeverity()
    return {
        PSU = O.PsRemovedInfo.Severity:next(change_severity_num_to_str):fetch_or(),
        Fan = O.FanRemovedInfo.Severity:next(change_severity_num_to_str):fetch_or(),
        Blade = O.BladeRemovedInfo.Severity:next(change_severity_num_to_str):fetch_or(),
        Swi = C.IpmbEthBlade('BladeType', BLADE_TYPE_SWI):next(function()
            return O.SwiRemovedInfo.Severity:next(change_severity_num_to_str):fetch_or()
        end):fetch_or(),
        SMM = O.SMMRemovedInfo.Severity:next(change_severity_num_to_str):fetch_or(),
    }
end

local EVENT_OBJ_STATE_ASSERT = 1
local EVENT_OBJ_STATE_DEASSERT = 0
function M.UpdateRemovedEventSeverity(data, user)
    local removeInfoHandle = {
        ['PSU'] = O.PsRemovedInfo,
        ['Fan'] = O.FanRemovedInfo,
        ['Blade'] = O.BladeRemovedInfo,
        ['Swi'] = O.SwiRemovedInfo,
        ['SMM'] = O.SMMRemovedInfo,
    }
    local removePlcyHandle = {
        ['PSU'] = O.PsRemovedPlcy,
        ['Fan'] = O.FanRemovedPlcy,
        ['Blade'] = O.BladeRemovedPlcy,
        ['Swi'] = O.SwiRemovedPlcy,
        ['SMM'] = O.SMMRemovedPlcy,
    }
    local result = reply_ok()
    local ok, ret
    for k, v in pairs(data) do
        local severity = change_severity_str_to_num(v)
        local oldSeverity = removeInfoHandle[k].Severity:fetch()
        if severity == oldSeverity then
            goto continue
        end
        result:join(safe_call(function()
            if severity == SEVERITY_NORMAL and oldSeverity ~= SEVERITY_NORMAL then
                local objName = dflib.object_name(removePlcyHandle[k])
                C.EventMonitor():fold(function(obj)
                    if obj.State:fetch() == EVENT_OBJ_STATE_ASSERT and obj.PolicyObj:fetch() == objName then
                        ok, ret = call_method(user, obj, 'DelEventFromMonitor', nil, nil)
                        if not ok then
                            return ret
                        end
                    end
                end)
            end
            ok, ret = call_method(user, removeInfoHandle[k], 'SetSeverity', nil, {gbyte(severity)})
            if not ok then
                return ret
            end
            local deassertFlag = severity == SEVERITY_NORMAL and EVENT_OBJ_STATE_DEASSERT or EVENT_OBJ_STATE_ASSERT
            ok, ret = call_method(user, removePlcyHandle[k], 'SetDeassertFlag', nil, {gbyte(deassertFlag)})
            if not ok then
                return ret
            end
        end))
        ::continue::
    end
    if result:isOk() then
        return reply_ok_encode_json(M.GetRemovedEventSeverity())
    end
    result:appendErrorData(M.GetRemovedEventSeverity())
    return result
end

return M
