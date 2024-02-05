local cfg = require "config"
local utils = require "utils"
local defs = require 'define'
local dflib = require "dflib"
local c = require "dflib.core"
local cjson = require "cjson"
local http = require "http"
local tools = require "tools"
local download = require "download"
local logging = require "logging"
local file_exists = utils.file_exists
local C = dflib.class
local O = dflib.object
local null = cjson.null
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local gstring = GVariant.new_string
local call_method = utils.call_method
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_bad_request = http.reply_bad_request
local reply_ok = http.reply_ok
local HasUserMgntPrivilege = utils.HasUserMgntPrivilege
local reply_forbidden = http.reply_forbidden
local dal_set_file_owner = utils.dal_set_file_owner
local dal_set_file_mode = utils.dal_set_file_mode

-- License 服务错误码
local LM_ERROR_E = {
    LM_OK = 0,
    LM_ERR = 1,
    LM_ERR_PARAMETER = 2,
    LM_ERR_NOT_INSTALLED = 3,
    LM_ERR_BUSY = 4,
    LM_ERR_VERIFY = 5,
    LM_ERR_ACTIVATE = 6,
    LM_ERR_MAX = 7
}

local LICENSE_FILE_SOURCE_BMC = "iBMC"
local LICENSE_FILE_SOURCE_FD = "FusionDirector"
local LICENSE_FILE_SOURCE_ESIGHT = "eSight"

local LICENSE_FILE_SOURCE_BMC_INDEX = 0
local LICENSE_FILE_SOURCE_FD_INDEX = 1
local LICENSE_FILE_SOURCE_ESIGHT_INDEX = 2

local errMapRevoke = {
    [LM_ERROR_E.LM_ERR] = reply_bad_request('LicenseRevokeFailed'),
    [LM_ERROR_E.LM_ERR_NOT_INSTALLED] = reply_bad_request('LicenseNotInstall')
}

local errMapDelete = {
    [LM_ERROR_E.LM_ERR] = reply_bad_request('LicenseDeleteFailed'),
    [LM_ERROR_E.LM_ERR_NOT_INSTALLED] = reply_bad_request('LicenseNotInstall')
}

local errMapexport = {
    [LM_ERROR_E.LM_ERR] = reply_bad_request("LicenseExportFailed"),
    [LM_ERROR_E.LM_ERR_NOT_INSTALLED] = reply_bad_request("LicenseNotInstall")
}

local installRetMap = {
    [LM_ERROR_E.LM_ERR] = reply_bad_request("LicenseInstallFailed"),
    [LM_ERROR_E.LM_ERR_VERIFY] = reply_bad_request("LicenseVerifyFailed"),
    [LM_ERROR_E.LM_ERR_ACTIVATE] = reply_bad_request("LicenseActivateFailed")
}

local M = {}

local function InstalledStatus2Str(status)
    if status == 0 then
        return "NotInstalled"
    elseif status == 1 then
        return "Installed"
    elseif status == 0x80 then
        return "Installing"
    else
        logging:error("invalid status value is %d", status)
        return null
    end
end

local function LicenseClass2Str(class)
    if class == 0 then
        return "NotInstalled"
    elseif class == 1 then
        return "Advanced"
    else
        logging:error("invalid class value is %d", class)
        return null
    end
end

local function LicenseStatus2Str(status)
    if status == 0 then
        return "N/A"
    elseif status == 1 then
        return "Default"
    elseif status == 2 then
        return "GracePeriod"
    elseif status == 3 then
        return "Normal"
    elseif status == 4 then
        return "Emergency"
    elseif status == 5 then
        return "Commissioning"
    elseif status == 0xff then
        return "Unknown"
    else
        logging:error("invalid Status value is %d", status)
        return null
    end
end

local function FileState2Str(value)
    if value == 0 then
        return "Normal"
    elseif value == 1 then
        return "Incorrect"
    elseif value == 0xff then
        return "N/A"
    else
        logging:error("invalid value is %d", value)
        return null
    end
end

local function MatchStatus2Str(value)
    if value == 0 then
        return "Matched"
    elseif value == 1 then
        return "Unmatched"
    elseif value == 0xff then
        return "N/A"
    else
        logging:error("invalid value is %d", value)
        return null
    end
end

local function ESNValid2Str(value)
    if value == 0 then
        return "Valid"
    elseif value == 1 then
        return "Invalid"
    elseif value == 0xff then
        return "N/A"
    else
        logging:error("invalid value is %d", value)
        return null
    end
end

local function CurrentState2Str(status)
    if status == 0 then
        return "N/A"
    elseif status == 1 then
        return "Default"
    elseif status == 2 then
        return "GracePeriod"
    elseif status == 3 then
        return "Normal"
    elseif status == 4 then
        return "Emergency"
    elseif status == 5 then
        return "Commissioning"
    else
        logging:error("invalid Status value is %d", status)
        return null
    end
end

local function IsControlled2Str(value)
    if value == 0 then
        return "Controlled"
    elseif value == 1 then
        return "Uncontrolled"
    else
        logging:error("invalid value is %d", value)
        return null
    end
end

local function GetFeatureKey()
    local featureKeyArray = C.LicenseFeatureKey():fold(function(obj, array)
        array[#array + 1] = {
            Group = obj.GroupName:fetch(),
            IsControlled = obj.IsNoControl:next(IsControlled2Str):fetch(),
            Name = obj.FeatureName:fetch(),
            RemoteControlValue = obj.RemoteControlValue:fetch(),
            State = obj.CurrentState:next(CurrentState2Str):fetch(),
            ValidDate = obj.ValidDate:fetch(),
            Value = (obj.KeyValue:fetch() ~= 0 or obj.RemoteControlValue:fetch() ~= 0) and 1 or 0
        }
        return array
    end, {}):fetch()

    return featureKeyArray
end

local function GetFeatureKeyByArray(array)
    local featureKeyArray = {}
    for i = 1, #array, 1 do
        featureKeyArray[#featureKeyArray + 1] = {
            Group = O.LicenseFeatureKey.GroupName:fetch(),
            IsControlled = O.LicenseFeatureKey.IsNoControl:next(IsControlled2Str):fetch(),
            Name = O.LicenseFeatureKey.FeatureName:fetch(),
            RemoteControlValue = O.LicenseFeatureKey.RemoteControlValue:fetch(),
            State = O.LicenseFeatureKey.CurrentState:next(CurrentState2Str):fetch(),
            ValidDate = O.LicenseFeatureKey.ValidDate:fetch(),
            Value = O.LicenseFeatureKey.KeyValue:fetch()
        }
    end
    return featureKeyArray
end

local function GetSaleItemArray()
    local saleItemArray = C.LicenseSaleItem():fold(function(obj, array)
        array[#array + 1] = {
            Name = obj.SaleName:fetch(),
            Description = obj.SaleDesc:fetch(),
            IsControlled = obj.IsNoControl:next(IsControlled2Str):fetch(),
            ValidDate = obj.ValidDate:fetch(),
            Value = obj.SaleValue:fetch(),
            FeatureKey = GetFeatureKeyByArray(obj.FeatureKey:fetch())
        }
        return array
    end, {}):fetch()

    return saleItemArray
end

function M.GetLicenseService()
    logging:debug(" GetLicenseService... ")
    return {
        DeviceESN = C.LicenseManage[0].ProductESN:fetch(),
        InstalledStatus = C.LicenseManage[0].InstallStatus:next(InstalledStatus2Str):fetch(),
        LicenseClass = C.LicenseManage[0].LicenseClass:next(LicenseClass2Str):fetch(),
        LicenseStatus = C.LicenseManage[0].LicenseStatus:next(LicenseStatus2Str):fetch(),
        RevokeTicket = C.LicenseManage[0].RevokeTicket:fetch(),
        AlarmInfo = {
            FileState = C.LicenseAlarmInfo[0].FileError:next(FileState2Str):fetch(),
            ProductESNMatchState = C.LicenseAlarmInfo[0].ProductESNNotMatch:next(MatchStatus2Str):fetch(),
            ProductVersionMatchState = C.LicenseAlarmInfo[0].ProductVerNotMatch:next(MatchStatus2Str):fetch(),
            ProductESNValidState = C.LicenseAlarmInfo[0].ProductESNInvalid:next(ESNValid2Str):fetch(),
            RemainCommissioningDay = C.LicenseAlarmInfo[0].RemainCommissioningDays:fetch(),
            RemainGraceDay = C.LicenseAlarmInfo[0].RemainGraceDays:fetch()
        },
        LicenseInfo = {
            GeneralInfo = {
                CopyRight = C.LicenseGeneralInfo[0].LicenseCopyRight:fetch(),
                CreateTime = C.LicenseGeneralInfo[0].LicenseCreateTime:fetch(),
                Creator = C.LicenseGeneralInfo[0].LicenseCreator:fetch(),
                GraceDay = C.LicenseGeneralInfo[0].LicenseGraceDay:fetch(),
                Issuer = C.LicenseGeneralInfo[0].LicenseIssuer:fetch(),
                LSN = C.LicenseGeneralInfo[0].LicenseSN:fetch(),
                LicenseType = C.LicenseGeneralInfo[0].LicenseType:fetch()
            },
            KeyInfo = {FeatureKey = GetFeatureKey()},
            SaleInfo = {
                OfferingProduct = {
                    ProductName = C.LicenseSaleInfo[0].ProductName:fetch_or(nil),
                    SaleItem = GetSaleItemArray()
                }
            }
        }
    }
end

function M.DeleteLicense(user)
    logging:debug(" DeleteLicense...")
    -- 权限检查
    if not HasUserMgntPrivilege(user) then
        return reply_forbidden('InsufficientPrivilege')
    end
    local ret = C.LicenseManage[0]:next(function(obj)
        local ok, err = call_method(user, obj, "DeleteLicense", errMapDelete)
        if not ok then
            return err
        end
    end):fetch()
    if ret then
        return ret
    end
    return reply_ok()
end

function M.DisableLicense(user)
    logging:debug(" DisableLicense...")
    local ok, outputlist
    -- 权限检查
    if not HasUserMgntPrivilege(user) then
        return reply_forbidden('InsufficientPrivilege')
    end
    local ret = C.LicenseManage[0]:next(function(obj)
        ok, outputlist = call_method(user, obj, "RevokeLicense", errMapRevoke)
        if not ok then
            return outputlist
        end
    end):fetch()
    if ret then
        return ret
    end
    return reply_ok_encode_json({RevokeTicket = outputlist[1]})
end

function M.ExportLicense(user)
    logging:debug("ExportLicense begin")
    -- 权限检查
    if not HasUserMgntPrivilege(user) then
        return reply_forbidden('InsufficientPrivilege')
    end
    return C.LicenseManage[0]:next(function(obj)
        local generate_cb = function(filePath)
            local licensePath = filePath .. ".xml"
            local ok, ret = utils.call_method_async(user, obj, "ExportLicense", errMapexport, gstring(licensePath))
            if not ok then
                logging:error("ExportLicense error")
                return ret
            end
            return reply_ok()
        end

        local progress_cb = function(filePath)
            local licensePath = filePath .. ".xml"
            if file_exists(licensePath) then
                dal_set_file_owner(licensePath, cfg.REDFISH_USER_UID, cfg.APPS_USER_GID)
                dal_set_file_mode(licensePath, cfg.FILE_MODE_600)
                tools.movefile(licensePath, filePath)
                return 100
            end
            return 0
        end

        return download(user, "license.xml", 'license', defs.DOWNLOAD_LICENSE, generate_cb, progress_cb)
    end):catch(function(err)
        return reply_bad_request("ActionNotSupported", err)
    end):fetch()
end

function M.InstallLicense(formdata)
    local user = formdata.user
    if utils.CheckFormData(formdata) == false then
        utils.OperateLog(user, 'Failed to import license')
        return reply_bad_request('InvalidParam')
    end
    local filePath = utils.GetFormDataFilePath(formdata)
    if not filePath then
        return reply_bad_request("InvalidFile")
    end

    local ret = C.LicenseManage[0]:next(function(obj)
        local ok, result =
            call_method(user, obj, "InstallLicense", installRetMap, gstring(filePath), gbyte(LICENSE_FILE_SOURCE_BMC_INDEX))
        if not ok then
            c.rm_filepath(filePath)
            logging:error(" InstallLicense error ")
            return result
        end
    end):fetch()
    if ret then
        return ret
    end
    return reply_ok()
end

return M
