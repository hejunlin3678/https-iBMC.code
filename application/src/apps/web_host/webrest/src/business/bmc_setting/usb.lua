local utils = require 'utils'
local dflib = require 'dflib'
local c = require 'dflib.core'
local http = require 'http'
local logging = require "logging"
local C = dflib.class
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local gstring = GVariant.new_string
local call_method = utils.call_method
local reply_internal_server_error = http.reply_internal_server_error
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_bad_request = http.reply_bad_request
local reply_forbidden = http.reply_forbidden
local reply_not_found = http.reply_not_found
local HasUserMgntPrivilege = utils.HasUserMgntPrivilege
local reply_ok = http.reply_ok

local RFERR_WRONG_PARAM = 400
local RFERR_INSUFFICIENT_PRIVILEGE = 403

local M = {}

local function is_support_usbmgmt_service()
    return C.USBMgmt[0]:next(function(obj)
        local is_support = obj.Support:fetch()
        local is_present = obj.Presence:fetch()
        if is_support ~= 1 or is_present ~= 1 then
            return false
        end
        return true
    end):fetch_or(false)
end

function M.GetUSBMgmtService()
    if not is_support_usbmgmt_service() then
        return reply_not_found()
    end
    return C.USBMgmt[0]:next(function(obj)
        return {
            ServiceEnabled = obj.Enable:fetch() == 1 and true or false,
            USBDevicePresence = obj.DeviceStatus:fetch() ~= 0 and true or false
        }
    end):fetch()
end

local function SetUSBEnable(data, user)
    if data.ServiceEnabled == nil then
        return
    end

    return C.USBMgmt[0]:next(function(obj)
        local ok, err = call_method(user, obj, 'SetEnable', function(err_id)
            if err_id == RFERR_WRONG_PARAM then
                return reply_bad_request('PropertyValueFormatError', {'ServiceEnabled'}, {'ServiceEnabled'})
            elseif err_id == RFERR_INSUFFICIENT_PRIVILEGE then
                return reply_bad_request('PropertyModificationNeedPrivilege', {'ServiceEnabled'},
                    {'ServiceEnabled'})
            end
        end, gbyte(data.ServiceEnabled and 1 or 0))
        if not ok then
            return err
        end
    end):fetch()
end

local function SetUSBUncompressPassword(data, user)
    local pwd = data.USBUncompressPassword
    if pwd ~= nil then
        if not HasUserMgntPrivilege(user) then
            return reply_forbidden('InsufficientPrivilege')
        end

        local ok, err = call_method(user, C.USBMgmt[0]:next():fetch(), "SetUncompressPassword", nil, gstring(pwd))
        if not ok then
            logging:error(err)
            return reply_bad_request('PropertyNotModified', 'Fail to update \'UncompressPassword\'')
        end
    end     
    return
end

function M.SetUSBMgmtService(user, data)
    local result = reply_ok()

    result:join(SetUSBEnable(data, user))
    result:join(SetUSBUncompressPassword(data, user))

    if result:isOk() == false then
        return reply_internal_server_error('SetUSBMgmt', result)
    end

    return reply_ok_encode_json(M.GetUSBMgmtService(user))
end

return M
