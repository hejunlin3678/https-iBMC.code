local utils = require "utils"
local dflib = require "dflib"
local c = require "dflib.core"
local http = require "http"
local logging = require "logging"
local cjson = require "cjson"
local C = dflib.class
local O = dflib.object
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local gstring = GVariant.new_string
local gvstring = GVariant.new_vstring
local call_method = utils.call_method
local reply_internal_server_error = http.reply_internal_server_error
local reply_ok_encode_json = http.reply_ok_encode_json
local BoolToValue = utils.BoolToValue
local reply_bad_request = http.reply_bad_request
local reply_ok = http.reply_ok
local OperateLog = utils.OperateLog

local WRONG_UPLOAD_FILE = -2

local errMap = {
    [WRONG_UPLOAD_FILE] = reply_bad_request('WrongUploadFile')
}

local M = {}

function M.GetSP()
    logging:debug("GetSP...")
    local usbobj = C.USBMassStorage[0]
    return {
        SPDeviceInfoCollectEnabled = usbobj.SPDeviceInfoCollectEnable:next(utils.ValueToBool)
            :fetch_or(false),
        SPStartEnabled = usbobj.SpConfigFileReady:next(utils.ValueToBool):fetch_or(false)
    }
end

local function SetSPStartEnabled(data, user)
    if data.SPStartEnabled == nil then
        return nil
    end

    local ok, ret = call_method(user, C.USBMassStorage[0]:next():fetch(), "SetSpCfgReady", errMap, gbyte(BoolToValue(data.SPStartEnabled)))
    if not ok then
        return ret
    end

    local ret = O.Redfish:next(function(obj)
        local ok, err = call_method(user, obj, "StartSPFirmwareUpgradeMonitor", errMap)
        if not ok then
            return err
        end
    end):fetch()

    if ret then
        return ret
    end
end

function M.SetSP(user, data)
    logging:debug("SetSP...")
    local ret
    ret = C.USBMassStorage[0]:next(function(obj)
        if data.SPDeviceInfoCollectEnabled ~= nil then
            local flag
            if data.SPDeviceInfoCollectEnabled == true then
                flag = 1
            else
                flag = 0
            end
            local ok, err = call_method(user, obj, "SetSPDeviceInfoCollectEnable", errMap, gbyte(flag))
            if not ok then
                return err
            end
        end
    end):catch(function(err)
        return reply_internal_server_error("SetSPDeviceInfoCollectEnable", err)
    end):fetch()

    if ret then
        ret:appendErrorData(SetSPStartEnabled(data, user))
    else
        ret = SetSPStartEnabled(data, user)
    end

    if ret then
        ret:appendErrorData(M.GetSP(user))
        return ret
    end
    return reply_ok_encode_json(M.GetSP(user))
end

local function RmUploadSPFirmware(formdata)
    local fields = formdata.fields
    local user = formdata.user
    local headers = formdata.headers
    local inputList = {}

    --删除form_data中非file_field的文件类型fields所指向的文件
    for key, value in pairs(fields) do
        if headers and headers[key] and headers[key].type == 'FILE' then
            logging:error("upload file: %s", value)
            table.insert(inputList, value)
        end
    end

    local ok, ret = call_method(user, C.USBMassStorage[0]:next():fetch(), "ClearFirmware", errMap, gvstring(inputList))
    if not ok then
        logging:error("clear firmware %d", ret)
        return
    end

    return
end

local function GetUploadSPFirmwareCount(formdata)
    local fields = formdata.fields
    local headers = formdata.headers
    local count = 0

    for key, value in pairs(fields) do
        if headers and headers[key] and headers[key].type == 'FILE' then
            count = count + 1
        end
    end

    logging:error("upload file count: %d", count)
    return count
end

function M.SPFWUpdate(formdata)
    local fields = formdata.fields
    local user = formdata.user
    
    if not fields.ImagePath or not fields.SignaturePath then
        RmUploadSPFirmware(formdata)
        utils.OperateLog(user, 'Import firmware failed')
        return reply_bad_request('MissingRequiredField')
    end

    local headers = formdata.headers
    if not headers or not headers["ImagePath"] or not headers["SignaturePath"]
        or headers["ImagePath"].type ~= 'FILE' or headers["SignaturePath"].type ~= 'FILE' then
        RmUploadSPFirmware(formdata)
        utils.OperateLog(user, 'Import firmware failed')
        return reply_bad_request('WrongFileFormat')
    end

    if fields.SignaturePath ~= fields.ImagePath .. ".asc" and fields.SignaturePath ~= fields.ImagePath .. ".p7s" then
        RmUploadSPFirmware(formdata)
        utils.OperateLog(user, 'Import firmware failed')
        return reply_bad_request('FirmwareNotMatchSignFile')
    end

    if GetUploadSPFirmwareCount(formdata) ~= 2 then
        RmUploadSPFirmware(formdata)
        utils.OperateLog(user, 'Import firmware failed')
        return reply_bad_request('TooMuchRequestParam')
    end

    local ret = C.USBMassStorage[0]:next(function(obj)
        local ok, err = call_method(user, obj, "UpdateLocalFwSP", errMap, gstring(fields.ImagePath), gstring(fields.SignaturePath))
        if not ok then
            RmUploadSPFirmware(formdata)
            return err
        end
    end):fetch()

    if ret then
        return ret
    end 
end

function M.GetSPUpdateFileList(user)
    local ok, ret = call_method(user, C.USBMassStorage[0]:next():fetch(), "GetLocalFwSP", errMap)
    if not ok then
        return ret
    end

    return {
        UpdateFileList = cjson.decode(ret[1])
    }
end

function M.DeleteSPUpdateFile(user)
    local ok, ret = call_method(user, C.USBMassStorage[0]:next():fetch(), "DeleteLocalFwSP", errMap)
    if not ok then
        return ret
    end
    
    return ret
end

return M
