local dflib = require 'dflib'
local c = require 'dflib.core'
local defs = require 'define'
local utils = require 'utils'
local cfg = require 'config'
local http = require 'http'
local cjson = require 'cjson'
local logging = require 'logging'
local C = dflib.class
local O = dflib.object
local guint16 = c.GVariant.new_uint16
local gstring = c.GVariant.new_string
local redfish_call_method = utils.redfish_call_method
local reply_ok = http.reply_ok
local json_encode = cjson.encode
local reply_bad_request = http.reply_bad_request

local RFERR_INSUFFICIENT_PRIVILEGE = 403
local RFERR_NO_RESOURCE = 404
local SET_RFID_STATUS_ERR = -4007
local SET_RFID_HEIGHT_ERR = -4006
local SET_RFID_EEP_INFO_TOO_LONG_ERR = -4009
local SET_RFID_SWITCH_COUNT_LIMIT_ERR = -4011

local ErrMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request('InsufficientPrivilege'),
    [RFERR_NO_RESOURCE] = reply_bad_request('ResourceMissingAtURI'),
    [SET_RFID_STATUS_ERR] = reply_bad_request('ActionNotSupported'),
    [SET_RFID_HEIGHT_ERR] = reply_bad_request('ActionParameterValueInvalid'),
    [SET_RFID_EEP_INFO_TOO_LONG_ERR] = reply_bad_request('BodyExceedsMaxLength'),
    [SET_RFID_SWITCH_COUNT_LIMIT_ERR] = reply_bad_request('ActionParameterValueInvalid')
}

local M = {}

local function GetUnitFfidInfoString(data)     
    local result = {}
    if data.Model then
        result["1"]= data.Model
    end
    if data.DeviceType then 
        result["2"]= data.DeviceType
    end
    if data.Manufacturer then 
        result["3"]= data.Manufacturer
    end
    if data.SerialNumber then
        result["4"]= data.SerialNumber
    end
    if data.PartNumber then
        result["5"]= data.PartNumber
    end
    if data.UHeight then
        result["6"]= data.UHeight
    end
    if data.LifeCycleYear then
        result["7"]= data.LifeCycleYear
    end
    if data.CheckInTime then
        result["8"]= data.CheckInTime
    end
    if data.WeightKg then
        result["9"]= data.WeightKg
    end
    if data.RatedPowerWatts then
        result["10"]= data.RatedPowerWatts
    end
    if data.AssetOwner then
        result["11"]= data.AssetOwner
    end
    if data.ExtendField and next(data.ExtendField) then
        result["ExtendField"]= json_encode(data.ExtendField)
    end
    return json_encode(result)
end

function M.UpdateAssetInfo(UNum, user, data)
    -- 根据U位编号获取obj
    local unit_Obj
    local id = tonumber(UNum:match('U(%d+)$'))

    C.UnitInfo():fold(function(obj)
        if obj.UNum:fetch() == id then
            unit_Obj = obj
            return obj, false
        end
    end)
    if unit_Obj == nil then
        logging:error('check unit uri fail.')
        return reply_bad_request('InvalidIndex')
    end

    local str = GetUnitFfidInfoString(data)
    local len = #str

    local ok, ret = utils.call_method(user, unit_Obj, 'SetUnitInfo', ErrMap, {guint16(len),gstring(str)})
    if not ok then
        return ret
    end
    return reply_ok()
end
  
return M