local cfg = require 'config'
local utils = require "utils"
local dflib = require "dflib"
local c = require "dflib.core"
local cjson = require "cjson"
local http = require "http"
local logging = require "logging"
local C = dflib.class
local null = cjson.null
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local call_method = utils.call_method
local reply_bad_request = http.reply_bad_request
local reply_ok = http.reply_ok

local UMS_TYPE_E = {
    UMS_TYPE_SP = 0, -- SP 
    UMS_TYPE_IBMA = 1, -- IBMA
    UMS_TYPE_MAX = 2
}

local BMA_FILE_STATE = {
    BMA_VOS_OK = 0,
    BMA_IS_WORKING = 2,
    BMA_FILE_NOT_EXIST = 3,
    BMA_PART_TABLE_NOT_EXIST = 4,
    INVALID_DATA = 5
}

local IBMA_SUPPORT_STATE_E = {
    IBMA_SUPPORT_STATE_ENABLE = 0, -- NAND Flash中含有iBMA分区且分区中存在iBMA安装包
    IBMA_SUPPORT_STATE_NO_PARTITION = 1, -- NAND Flash中无iBMA分区
    IBMA_SUPPORT_STATE_NO_PACKET = 2, -- 分区中无iBMA安装包
    IBMA_SUPPORT_STATE_INVALID = 0xFF -- 默认无效值
}

local UMS_CONNECT_STATE_E = {
    UMS_CONNECT_STATE_IDLE = 0, -- 未连接
    UMS_CONNECT_STATE_CONNECTED = 1, -- 已连接
    UMS_CONNECT_STATE_CONNECT_FAIL = 2, -- 连接失败
    UMS_CONNECT_STATE_DISCONNECT_FAIL = 3, -- 断连失败
    UMS_CONNECT_STATE_INVALID = 0xFF    -- 默认无效值
}

local UMS_IBMA_FILE_ID_ALL = 1

local MANAGER_PROPERTY_VMMCONNECT = "Connect"
local MANAGER_PROPERTY_VMMDISCONNECT = "Disconnect"

local RFERR_INSUFFICIENT_PRIVILEGE = 403
local RFERR_NO_RESOURCE = 404

local LANGUAGE_NULL_OR_EMPTY = -2001
local LANGUAGE_ONLY_ONE_LANGUAGE = -2002
local LANGUAGE_NO_UNINSTALLED_ZH_OR_EN = -2003
local LANGUAGE_NOT_INSTALLED = -2004

local errMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("InsufficientPrivilege",
        "There are insufficient privileges for the account or credentials associated with the current session to perform the requested operation."),
    [RFERR_NO_RESOURCE] = reply_bad_request("ResourceMissingAtURI",
        "The resource at the URI /UI/Rest/UpdateService/SwitchImage was not found."),
    [LANGUAGE_NULL_OR_EMPTY] = reply_bad_request("ActionParameterValueFormatError",
        "The value for the parameter in the action is of a different format than the parameter can accept."),
    [LANGUAGE_ONLY_ONE_LANGUAGE] = reply_bad_request("OnlyOneLanguageInstalled",
        "Uninstall the language failed because there is only one language installed."),
    [LANGUAGE_NO_UNINSTALLED_ZH_OR_EN] = reply_bad_request("NotSupportZhAndEnUninstalled",
        "Uninstall the language failed because Chinese and English cannot be uninstalled."),
    [LANGUAGE_NOT_INSTALLED] = reply_bad_request("LanguageNotInstalled",
        "Uninstall the language failed because this language has not been installed.")
}

local function CheckStrEffective(str)
    if str == "N/A" or str == "UnKnown" or str == "Undefined" or str == "" or str == nil then
        logging:info("check str effective fail.")
        return null
    else
        return str
    end
end

local function iBMAPacketStatusToStr(status)
    if status == IBMA_SUPPORT_STATE_E.IBMA_SUPPORT_STATE_ENABLE then
        return "iBMAPacketIsAvailable"
    elseif status == IBMA_SUPPORT_STATE_E.IBMA_SUPPORT_STATE_NO_PARTITION then
        return "iBMAPartitionTableNotExist"
    elseif status == IBMA_SUPPORT_STATE_E.IBMA_SUPPORT_STATE_NO_PACKET then
        return "iBMAPacketNotExist"
    elseif status == IBMA_SUPPORT_STATE_E.IBMA_SUPPORT_STATE_INVALID then
        return null
    end
end

local M = {}

local function GetAvailiablePackages()
    local packagesArray = {}
    local packetNum = C.iBMAUSBMassStorage[0].PacketNum:fetch()
    local OSTypeArray = C.iBMAUSBMassStorage[0].OsType:fetch()
    local versionArray = C.iBMAUSBMassStorage[0].Version:fetch()
    logging:debug("GetAvailiablePackages, packetNum:%d, OSTypeArray:%d, versionArray:%d", packetNum, #OSTypeArray,
        #versionArray)

    if packetNum == 0 then
        return null
    end
    if #OSTypeArray ~= packetNum or #versionArray ~= packetNum then
        logging:error("the os type(or version) number isn't equal with packet number")
        return null
    end
    for i = 1, packetNum, 1 do
        packagesArray[#packagesArray + 1] = {OSType = OSTypeArray[i], Version = versionArray[i]}
    end

    return packagesArray
end

local function getInstallableiBmaState(user)
    local iBMAUSBMassStorageHandle = C.iBMAUSBMassStorage[0]:next():fetch()
    local supportState = iBMAUSBMassStorageHandle.State:fetch_or(IBMA_SUPPORT_STATE_E.IBMA_SUPPORT_STATE_INVALID)
    if supportState ~= IBMA_SUPPORT_STATE_E.IBMA_SUPPORT_STATE_ENABLE and supportState ~= IBMA_SUPPORT_STATE_E.IBMA_SUPPORT_STATE_NO_PACKET then
        return BMA_FILE_STATE.BMA_PART_TABLE_NOT_EXIST
    end
    local connectState = iBMAUSBMassStorageHandle.ConnectState:fetch_or(UMS_CONNECT_STATE_E.IBMA_SUPPORT_STATE_INVALID)
    if connectState == UMS_CONNECT_STATE_E.UMS_CONNECT_STATE_CONNECTED or connectState == UMS_CONNECT_STATE_E.UMS_CONNECT_STATE_DISCONNECT_FAIL then
        return BMA_FILE_STATE.BMA_IS_WORKING
    end

    local ok, ret = pcall(function ()
        call_method(user, iBMAUSBMassStorageHandle, "CheckFileState", nil, gbyte(UMS_IBMA_FILE_ID_ALL))
    end)
    if not ok then
        logging:error("iBMA file do not exist")
        return BMA_FILE_STATE.BMA_FILE_NOT_EXIST
    end
    return BMA_FILE_STATE.BMA_VOS_OK
end

local function getiBMASupportState(user)
    local enable = C.PMEServiceConfig[0].iBMAOnBoardEnable:fetch_or(cfg.DISABLE)
    if enable == cfg.DISABLE then
        return false
    end

    local bmaFlag = getInstallableiBmaState(user)
    if bmaFlag == BMA_FILE_STATE.BMA_VOS_OK or bmaFlag == BMA_FILE_STATE.BMA_IS_WORKING then
        return true
    end
    return false
end

local function getiBMAPacketStatus(user)
    if getiBMASupportState(user) then
        return C.iBMAUSBMassStorage[0].State:next(iBMAPacketStatusToStr):fetch_or()
    end
end

function M.GetiBMA(user)
    logging:debug(" GetiBMA... ")
    return {
        AvailiableInstallationPackages = GetAvailiablePackages(),
        iBMAPacketStatus = getiBMAPacketStatus(user),
        Inserted = C.iBMAUSBMassStorage[0]:next(function(obj)
            if obj.State:fetch() ~= IBMA_SUPPORT_STATE_E.IBMA_SUPPORT_STATE_ENABLE then
                logging:info("GetiBMA, the ibma don't support. state=%d", obj.State:fetch())
                return null
            end
            if obj.ConnectState:fetch() == UMS_CONNECT_STATE_E.UMS_CONNECT_STATE_CONNECTED or
                obj.ConnectState:fetch() == UMS_CONNECT_STATE_E.UMS_CONNECT_STATE_DISCONNECT_FAIL then
                return true
            else
                return false
            end
        end):fetch(),
        InstalledPackage = {
            Version = C.SMS[0].FirmwareVersion:next(CheckStrEffective):fetch(),
            DriverVersion = C.SMS[0].VNICDriveVersion:next(CheckStrEffective):fetch(),
            RunningStatus = C.SMS[0].Status:next(CheckStrEffective):fetch()
        }
    }
end

local function USBStickControlConnect(user)
    local ret = C.iBMAUSBMassStorage[0]:next(function(obj)
        local ok, ret = call_method(user, obj, "CreatUms", errMap, gbyte(0), gbyte(0))
        if not ok then
            logging:error(" USBStickControlConnect error ")
            return ret
        end
    end):fetch()
    if ret then
        return ret
    end
end

local function USBStickControlDisconnect(user)
    local ret = C.iBMAUSBMassStorage[0]:next(function(obj)
        local ok, ret = call_method(user, obj, "CloseUms", errMap)
        if not ok then
            logging:error(" USBStickControlDisconnect error ")
            return ret
        end
    end):fetch()
    if ret then
        return ret
    end
end

function M.USBStickControl(user, data)
    logging:debug(" USBStickControl...")
    local ret
    local controlType = data.Type
    if C.iBMAUSBMassStorage[0].State:fetch() ~= IBMA_SUPPORT_STATE_E.IBMA_SUPPORT_STATE_ENABLE then
        logging:error("GetiBMA, the ibma don't support. state=%d", C.iBMAUSBMassStorage[0].State:fetch())
        return reply_bad_request("ActionNotSupported")
    end

    local handle = C.iBMAUSBMassStorage[0]
    if handle.UmsType:fetch() >= UMS_TYPE_E.UMS_TYPE_MAX then
        logging:error("GetiBMA, ums type is invalid. umstype=%d", handle.UmsType:fetch())
        return reply_bad_request("InternalError")
    end
    if controlType == MANAGER_PROPERTY_VMMCONNECT then
        ret = USBStickControlConnect(user)
    elseif controlType == MANAGER_PROPERTY_VMMDISCONNECT then
        ret = USBStickControlDisconnect(user)
    end
    if ret then
        return ret
    end
    return reply_ok()
end

return M
