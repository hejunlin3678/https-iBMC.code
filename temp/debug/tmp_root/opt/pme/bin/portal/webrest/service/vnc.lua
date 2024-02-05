local utils = require "utils"
local dflib = require "dflib"
local c = require "dflib.core"
local cjson = require "cjson"
local null = cjson.null
local http = require "http"
local bit = require "bit"
local logging = require "logging"
local C = dflib.class
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local gint32 = GVariant.new_int32
local gstring = GVariant.new_string
local call_method = utils.call_method
local HasSecurityMgntPrivilege = utils.HasSecurityMgntPrivilege
local HasKvmMgntPrivilege = utils.HasKvmMgntPrivilege
local reply_internal_server_error = http.reply_internal_server_error
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_bad_request = http.reply_bad_request
local reply_not_found = http.reply_not_found

-- kvm模块错误码定义
local RF_ERROR = -1
local VNC_IN_USE = 12
local VNC_PSWD_LEN_ERR = 2
local VNC_PSWD_CMPLX_CHK_FAIL = 3
local RPC_UNKNOWN = -200
local RPC_NO_CONNECT_EX = -201
local RFERR_INSUFFICIENT_PRIVILEGE = 403
local COMP_CODE_INVALID_CMD = 0xC1
local COMP_CODE_OUTOF_RANGE = 0xC9

local portErrMap = {
    [COMP_CODE_INVALID_CMD] = reply_bad_request("PropertyModificationNotSupported",
        "The property %1 modification failed because it is not supported."),
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("PropertyModificationNeedPrivilege",
        "The property %1 cannot be modified because of insufficient permission."),
    [COMP_CODE_OUTOF_RANGE] = reply_bad_request("PortIdModificationFailed",
        "The value %1 for the property %2 is not in the range of 1 to 65535 or already exists."),
    [RF_ERROR] = reply_bad_request("PortIdModificationFailed",
        "The value %1 for the property %2 is not in the range of 1 to 65535 or already exists.")
}

local stateErrMap = {
    [COMP_CODE_INVALID_CMD] = reply_bad_request("PropertyModificationNotSupported",
        "The property %1 modification failed because it is not supported."),
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("PropertyModificationNeedPrivilege",
        "The property %1 cannot be modified because of insufficient permission."),
    [RPC_UNKNOWN] = reply_bad_request("ServiceRestartInProgress",
        "The service is being restarted, please try again after the service is restarted."),
    [RPC_NO_CONNECT_EX] = reply_bad_request("ServiceRestartInProgress",
        "The service is being restarted, please try again after the service is restarted."),
    [COMP_CODE_OUTOF_RANGE] = reply_bad_request("PropertyValueNotInList",
        "The value %1 for the property %2 is not in the list of acceptable values."),
    [RF_ERROR] = reply_bad_request("PropertyValueNotInList",
        "The value %1 for the property %2 is not in the list of acceptable values.")
}

local errMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("PropertyModificationNeedPrivilege",
        "The property %1 cannot be modified because of insufficient permission."),
    [RPC_UNKNOWN] = reply_bad_request("ServiceRestartInProgress",
        "The service is being restarted, please try again after the service is restarted."),
    [RPC_NO_CONNECT_EX] = reply_bad_request("ServiceRestartInProgress",
        "The service is being restarted, please try again after the service is restarted."),
    [VNC_IN_USE] = reply_bad_request("VncInUse",
        "Failed to set the SSL encryption status because the VNC is in use."),
    [VNC_PSWD_LEN_ERR] = reply_bad_request("InvalidPasswordLength"),
    [VNC_PSWD_CMPLX_CHK_FAIL] = reply_bad_request("PasswordComplexityCheckFail",
        "Failed to send the request because the password does not meet password complexity requirements.")
}

local M = {}

function M.GetVNC()
    local VNCEnable = C.PMEServiceConfig[0].VNCEnable:next(utils.ValueToBool):fetch_or(false)
    if not VNCEnable then
        return reply_not_found()
    end
    local VNC = C.VNC[0]
    return {
        Enabled = VNC.EnableState:eq(1):next(function()
            return true
        end):fetch_or(false),
        Port = VNC.Port:fetch(),
        SessionTimeoutMinutes = VNC.Timeout:fetch(),
        KeyboardLayout = VNC.KeyboardLayout:fetch(),
        PasswordValidityDays = VNC.PwdRemainDay:fetch(),
        LoginRule = VNC.PermitRuleIds:next(function(type)
            local loginRuleArray = {}
            local loginRuleBitArr = {1, 2, 4}
            local loginRuleStrArr = {"Rule1", "Rule2", "Rule3"}
            for key, value in ipairs(loginRuleBitArr) do
                if bit.band(type, value) == value then
                    loginRuleArray[#loginRuleArray + 1] = loginRuleStrArr[key]
                end
            end
            return loginRuleArray
        end):fetch(),
        ActivatedSessionsType = C.VncLink():fold(function(obj)
            local propIp = obj.link_ip:fetch()
            if propIp == '0' then
                return null
            end
            local linkMode = obj.link_mode:fetch()
            if linkMode == 1 then
                return 'Shared', false
            elseif linkMode == 0 then
                return 'Private', false
            end
            return null
        end):fetch_or(null),

        LoginRuleInfo = C.PermitRule():fold(function(obj, acc)
            local enableValue = C.SecurityEnhance[0].PermitRuleIds:fetch()
            local TimeRuleInfo = obj.TimeRuleInfo:fetch()
            local StartTime, EndTime = TimeRuleInfo:match("([^/]+)/(.+)")
            local loginRuleBitArr = {1, 2, 4}
            local loginRuleStrArr = {"Rule1", "Rule2", "Rule3"}
            local loginRuleEnable
            acc[#acc + 1] = {
                ID = obj.RuleId:next(function(id)
                    if bit.band(loginRuleBitArr[id + 1], enableValue) == loginRuleBitArr[id + 1] then
                        loginRuleEnable = true
                    else
                        loginRuleEnable = false
                    end

                    if 0 <= id and id < 3 then
                        return loginRuleStrArr[id + 1]
                    else
                        logging:error("LoginRuleId is out of range. Id: %d", id)
                    end
                end):fetch(),
                Enabled = loginRuleEnable,
                StartTime = StartTime,
                EndTime = EndTime,
                IP = obj.IpRuleInfo:fetch(),
                Mac = obj.MacRuleInfo:fetch()
            }
            return acc
        end, {}):fetch(),

        SSLEncryptionEnabled = VNC.SSLEnableState:eq(1):next(
            function()
                return true
            end):fetch_or(false),
        PasswordComplexityCheckEnabled = C.PasswdSetting[0].EnableStrongPassword:next(
            function(value)
                if value == 0 then
                    return false
                elseif value == 1 then
                    return true
                else
                    logging:error("EnableStrongPassword is out of range. EnableStrongPassword: %d", value)
                end
            end):fetch(),
        MaximumNumberOfSessions = 5,
        NumberOfActiveSessions = VNC.ConnectNum:fetch()
    }
end

function M.SetVNC(user, data)
    local VNCEnable = C.PMEServiceConfig[0].VNCEnable:next(utils.ValueToBool):fetch_or(false)
    if not VNCEnable then
        return reply_bad_request("PropertyModificationNotSupported")
    end
    local ret
    -- VNC
    ret = C.VNC[0]:next(function(obj)
        if data.Port then
            if not HasSecurityMgntPrivilege(user) then
                return reply_bad_request("PropertyModificationNeedPrivilege", {"Port"})
            end
            local ok, err = call_method(user, obj, "SetPort", portErrMap, gint32(data.Port))
            if not ok then
                return err
            end
        end
        if data.Enabled == true then
            if not HasSecurityMgntPrivilege(user) then
                return reply_bad_request("PropertyModificationNeedPrivilege", {"Enabled"})
            end
            local ok, err = call_method(user, obj, "SetState", stateErrMap, gbyte(1))
            if not ok then
                return err
            end
        elseif data.Enabled == false then
            if not HasSecurityMgntPrivilege(user) then
                return reply_bad_request("PropertyModificationNeedPrivilege", {"Enabled"})
            end
            local ok, err = call_method(user, obj, "SetState", stateErrMap, gbyte(0))
            if not ok then
                return err
            end
        end
        if data.SessionTimeoutMinutes ~= nil then
            if not HasKvmMgntPrivilege(user) then
                return reply_bad_request("PropertyModificationNeedPrivilege", {"SessionTimeoutMinutes"})
            end
            local ok, err = call_method(user, obj, "SetTimeout", errMap, gint32(data.SessionTimeoutMinutes))
            if not ok then
                return err
            end
        end
        if data.KeyboardLayout then
            if not HasKvmMgntPrivilege(user) then
                return reply_bad_request("PropertyModificationNeedPrivilege", {"KeyboardLayout"})
            end
            local ok, err = call_method(user, obj, "SetKeyboardLayout", errMap, gstring(data.KeyboardLayout))
            if not ok then
                return err
            end
        end
        if data.LoginRule then
            if not HasKvmMgntPrivilege(user) then
                return reply_bad_request("PropertyModificationNeedPrivilege", {"LoginRule"})
            end
            local value = 0
            for _, rule in pairs(data.LoginRule) do
                if rule == "Rule1" then
                    value = value + 1
                elseif rule == "Rule2" then
                    value = value + 2
                elseif rule == "Rule3" then
                    value = value + 4
                end
            end
            local ok, err = call_method(user, obj, "SetPermitRuleIds", errMap, gbyte(value))
            if not ok then
                return err
            end
        end
        if data.SSLEncryptionEnabled == true then
            if not HasKvmMgntPrivilege(user) then
                return reply_bad_request("PropertyModificationNeedPrivilege", {"SSLEncryptionEnabled"})
            end
            local ok, err = call_method(user, obj, "SetSSLEnableState", errMap, gbyte(1))
            if not ok then
                return err
            end
        elseif data.SSLEncryptionEnabled == false then
            if not HasKvmMgntPrivilege(user) then
                return reply_bad_request("PropertyModificationNeedPrivilege", {"SSLEncryptionEnabled"})
            end
            local ok, err = call_method(user, obj, "SetSSLEnableState", errMap, gbyte(0))
            if not ok then
                return err
            end
        end
    end):catch(function()
        return reply_bad_request("ServiceRestartInProgress",
        "The service is being restarted, please try again after the service is restarted.")
    end):fetch()

    if ret then
        ret:appendErrorData(M.GetVNC())
        return ret
    end

    return reply_ok_encode_json(M.GetVNC())
end

function M.SetVNCPassword(user, data)
    local VNCEnable = C.PMEServiceConfig[0].VNCEnable:next(utils.ValueToBool):fetch_or(false)
    if not VNCEnable then
        return reply_bad_request("PropertyModificationNotSupported")
    end
    local reauth = utils.ReAuthUser(user, data.ReauthKey)
    if reauth then
        utils.OperateLog(user, 'Failed to set vnc password, because the current user password is incorrect or the account is locked')
        return reauth
    end
    local ret = C.VNC[0]:next(function(obj)
        if data.VNCPassword then
            local ok, err = call_method(user, obj, "SetPwdValue", errMap, gstring(data.VNCPassword))
            if not ok then
                return err
            end
        end
    end):catch(function(err)
        return reply_internal_server_error("SetVNC VNC", err)
    end):fetch()

    if ret then
        ret:appendErrorData(M.GetVNC())
        return ret
    end

    return reply_ok_encode_json(M.GetVNC())
end
return M
