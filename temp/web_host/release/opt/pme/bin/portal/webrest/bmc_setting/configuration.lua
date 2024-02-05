local utils = require "utils"
local defs = require 'define'
local dflib = require "dflib"
local c = require "dflib.core"
local http = require "http"
local logging = require "logging"
local download = require "download"
local cfg = require 'config'
local C = dflib.class
local GVariant = c.GVariant
local gstring = GVariant.new_string
local call_method = utils.call_method
local dal_set_file_owner = utils.dal_set_file_owner
local dal_set_file_mode = utils.dal_set_file_mode
local reply_bad_request = http.reply_bad_request
local reply_ok = http.reply_ok

local RFERR_INSUFFICIENT_PRIVILEGE = 403
local RFERR_NO_RESOURCE = 404
local RFERR_CONFIG_IMPORT_ERR = -1

local errMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("InsufficientPrivilege",
        "There are insufficient privileges for the account or credentials associated with the current session to perform the requested operation."),
    [RFERR_NO_RESOURCE] = reply_bad_request("ResourceMissingAtURI",
        "The resource at the URI /UI/Rest/UpdateService/SwitchImage was not found."),
    [RFERR_CONFIG_IMPORT_ERR] = reply_bad_request("ConfigurationImportingErr",
        "A file transfer task is being performed or another configuration import operation is in progress.")
}

local M = {}

function M.ImportConfig(formdata)
    local user = formdata.user
    local filePath = utils.GetFormDataFilePath(formdata)
    local reauth = utils.ReAuthUser(user, formdata.fields.ReauthKey)
    if utils.CheckFormData(formdata) == false then
        if filePath then
            c.rm_filepath(filePath)
        end
        utils.OperateLog(user, 'Failed to import config')
        return reply_bad_request('InvalidParam')
    end

    if reauth then
        if filePath then
            c.rm_filepath(filePath)
        end
        utils.OperateLog(user,
            'Failed to import config, because the current user password is incorrect or the account is locked')
        return reauth
    end
    if not filePath then
        utils.OperateLog(user, 'Import config failed')
        return reply_bad_request("InvalidFile")
    end

    local ret = C.BMC[0]:next(function(obj)
        local ok, ret = call_method(user, obj, "ImportConfig", errMap, gstring(filePath))
        if not ok then
            c.rm_filepath(filePath)
            return ret
        end
    end):fetch()

    if ret then
        return ret
    end

    return reply_ok()
end

function M.ExportConfig(user)
    logging:debug("ExportConfig begin.")
    local configFile

    local ret = C.BMC[0]:next(function(obj)
        local generate_cb = function(filePath)
            configFile = filePath
            local ok, _ = call_method(user, obj, "ExportConfig", errMap, gstring(filePath))
            if not ok then
                return reply_bad_request("ConfigurationExportingErr",
                    "A file transfer task is being performed or another configuration export operation is in progress.")
            end
            return reply_ok()
        end

        local progress_cb = function()
            local ok, outputlist = call_method(user, obj, "GetImportExportStatus", errMap)
            if ok then
                if outputlist[1] == 100 then
                    if not dal_set_file_owner(configFile, cfg.REDFISH_USER_UID, cfg.APPS_USER_GID) then
                        error('chown config.xml failed')
                    end
                    if not dal_set_file_mode(configFile, cfg.FILE_MODE_600) then
                        error('chmod config.xml failed')
                    end
                end
                return outputlist[1]
            end
            logging:error("Get collecting file download progress failed. err_string:%s", outputlist[1])
            return 0
        end

        return download(user, 'config.xml', 'config file', defs.DOWNLOAD_CONFIG, generate_cb, progress_cb)
    end):fetch()

    return ret
end

function M.GetImportExportConfigProgress(user)
    logging:debug("GetImportExportConfigProgress...")
    return C.BMC[0]:next(function(obj)
        local ok, outputlist = call_method(user, obj, "GetImportExportStatus", errMap)
        if not ok then
            logging:error("Get collecting file download progress failed. err_string:%s", outputlist[1])
            return {TaskName = "Import/Export Config Task", Percentage = "0%"}
        end
        return {
            TaskName = "Import/Export Config Task",
            Percentage = outputlist[1] .. "%",
            Messages = outputlist[2]
        }
    end):fetch()
end

return M
