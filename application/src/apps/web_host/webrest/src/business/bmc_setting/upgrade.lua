local utils = require "utils"
local dflib = require "dflib"
local c = require "dflib.core"
local cjson = require "cjson"
local http = require "http"
local logging = require "logging"
local reboot = require "reboot"
local C = dflib.class
local O = dflib.object
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local gstring = GVariant.new_string
local call_method = utils.call_method
local reply_bad_request = http.reply_bad_request
local reply_ok = http.reply_ok

local RFERR_INSUFFICIENT_PRIVILEGE = 403
local RFERR_NO_RESOURCE = 404
local IN_UPGRADEING = -2

local errMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("InsufficientPrivilege",
        "There are insufficient privileges for the account or credentials associated with the current session to perform the requested operation."),
    [RFERR_NO_RESOURCE] = reply_bad_request("ResourceMissingAtURI",
        "The resource at the URI /UI/Rest/UpdateService/SwitchImage was not found."),
    [IN_UPGRADEING] = reply_bad_request("InUpgrading")
}

local M = {}

function M.GetUpdateService(user)
    logging:debug("GetUpdateService...")
    local obj = O.BMC
    local ok, ret = call_method(user, O.Upgrade, "GetComponentsInfo")
    if not ok then
        return ret
    end

    local UpgradeResourceList = cjson.decode(ret[1])

    local BMCValidMode = false
    if O.PRODUCT.BMCValidMode:fetch_or(nil) == 0 then
        BMCValidMode = true
    end

    -- 1710没有tmp区
    local AvailableBMCVersionStr = obj.BMCAvailableVer:fetch_or(nil)
    if (obj.ChipName:fetch_or(nil) == "Hi1710") then
        AvailableBMCVersionStr = ""
    end

    local ActiveModeCode = C.Upgrade[0].ActiveMode:fetch_or(nil)
    local ActiveModeStr = "Immediately"
    if ActiveModeCode == 0 then
        ActiveModeStr = "ResetBMC"
    end

    return {
        ActiveMode = ActiveModeStr,
        ActiveModeSupported = BMCValidMode,
        UpgradeResource = UpgradeResourceList,
        FirmwareVersion = {
            ActiveBMCVersion = obj.PMEVer:fetch_or(nil),
            BackupBMCVersion = obj.PMEBackupVer:fetch_or(nil),
            AvailableBMCVersion = AvailableBMCVersionStr
        },
        UpdatingFlag = C.Upgrade[0].InUpgradeFlag:next(function(flag)
            if flag == 0 then
                return false
            else
                return true
            end
        end):fetch_or(false)
    }
end

local WAIT_RESET_TIMEOUT = 3000
local CANCEL_RESET_TIMEOUT = 500
function M.Reset(user)
    logging:debug("Manager rebootting...")
    local ret = C.BMC[0]:next(function(obj)
        local hook = reboot.hook(WAIT_RESET_TIMEOUT)
        local ok, err = call_method(user, obj, "RebootPme", errMap)
        hook.cancel(CANCEL_RESET_TIMEOUT)
        if not ok then
            return err
        end
    end):fetch_or(reply_bad_request('ResetOperationNotAllowed'))
    if ret then
        return ret
    end
    return reply_ok()
end

function M.SwitchImage(user)
    logging:debug("manager rollback...")
    local hook = reboot.hook(WAIT_RESET_TIMEOUT)
    local ok, err = call_method(user, O.Upgrade, "SetRollback", errMap)
    hook.cancel(CANCEL_RESET_TIMEOUT)
    if not ok then
        return err
    end
    return reply_ok()
end

local function FirmwareUpdateAct(fields, user, filePath)
    if C.Upgrade[0].InUpgradeFlag:fetch() == 1 then
        return false, reply_bad_request("ModifyFailedWithIPVersion",
            "A file transfer task is being performed or an upgrade operation is in progress.")
    end
    if C.TransferFile[0].IsTransfering:fetch() ~= 0 then
        logging:error("upgrade failed, another file transfer task is running")
        return false, reply_bad_request("FirmwareUpgradeError", "Upgrade failed, another file transfer task is running.")
    end

    if O.PRODUCT.BMCValidMode:fetch() == 0 then
        local activeMode = 1
        if fields.ActiveMode ~= nil then
            if fields.ActiveMode == "ResetBMC" then
                activeMode = 0
            elseif fields.ActiveMode == "Immediately" then
                activeMode = 1
            else
                logging:error("upgrade failed, Input parameter(active mode) error!")
                return false, reply_bad_request("FirmwareUpgradeError", "Set active mode failed.")
            end

            local ok = pcall(
                function()
                    call_method(user, O.Upgrade, "SetActiveMode", nil, gbyte(activeMode))
                end)
            if not ok then
                return false, reply_bad_request('PropertyNotModified', 'Fail to set \'ActiveMode\'')
            end
        end
    end

    local ret = C.Upgrade[0]:next(function(obj)
        local ok, err = call_method(user, obj, "InitrialUpgrade", errMap, gstring(filePath), gbyte(0))
        if not ok then
            return err
        end
    end):fetch()

    if ret then
        return false, ret
    end
    return true, reply_ok()
end

function M.FirmwareUpdate(formdata)
    local fields = formdata.fields
    local user = formdata.user
    if utils.CheckFormData(formdata) == false then
        logging:error('upgrade failed')
        return reply_bad_request('InvalidParam')
    end
    local filePath = utils.GetFormDataFilePath(formdata)
    if not filePath then
        return reply_bad_request("InvalidFile")
    end

    local update_ok, res = FirmwareUpdateAct(fields, user, filePath)
    if not update_ok then
        c.rm_filepath(filePath)
    end
    return res
end

local function GetTaskState(errorCode)
    if errorCode ~= 0 then
        return "Warning"
    end

    return C.Upgrade[0].InUpgradeFlag:next(function(flag)
        if flag == 0 then
            return "Completed"
        else
            return "Running"
        end
    end):fetch_or("Completed")
end

function M.GetUpdateProgress(user)
    logging:debug("GetUpdateProgress...")
    local percentage = C.Upgrade[0].UpgradeState:fetch()
    local errorCode = C.Upgrade[0].ErrCode:fetch()
    return {
        TaskName = "Upgrade Task",
        Percentage = percentage .. "%",
        TaskState = GetTaskState(errorCode),
        ErrorCode = errorCode,
        Component = C.Upgrade[0].CompnentStr:fetch()
    }
end

return M
