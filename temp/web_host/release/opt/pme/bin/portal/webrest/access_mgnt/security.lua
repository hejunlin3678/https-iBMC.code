local dflib = require 'dflib'
local c = require 'dflib.core'
local cjson = require 'cjson'
local http = require 'http'
local bit = require 'bit'
local utils = require 'utils'
local cfg = require 'config'
local math = require 'math'
local logging = require "logging"
local O = dflib.object
local C = dflib.class
local null = cjson.null
local reply_ok = http.reply_ok
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_bad_request = http.reply_bad_request
local reply_error = http.reply_error
local safe_call = http.safe_call
local call_method = utils.call_method
local HasUserMgntPrivilege = utils.HasUserMgntPrivilege
local dal_rf_get_board_type = utils.dal_rf_get_board_type
local HasRoleId = utils.HasRoleId
local math_abs = math.abs
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local guint16 = GVariant.new_uint16
local guint32 = GVariant.new_uint32
local gstring = GVariant.new_string
local ENABLE = cfg.ENABLE
local HTTP_FORBIDDEN = http.status.HTTP_FORBIDDEN
local HTTP_METHOD_NOT_IMPLEMENTED = http.status.HTTP_METHOD_NOT_IMPLEMENTED
local BOARD_SWITCH = cfg.BOARD_SWITCH
local IsMgmtBoard = utils.IsMgmtBoard

local PME_SERVICE_SUPPORT = 1

local RMCP_ADMIN = 4
local RF_ENABLE = 1

local USER_TIME_LIMIT_UNREASONABLE = 0x91 -- 密码有效期设置短于密码最短使用期限
local USER_PARAMETER_OUT_OF_RANGE = 0xC9

local USERROLE_SECURITYMGNT = 0x04
local USERROLE_USERMGNT = 0x10

local VOS_OK = 0
local RFERR_SUCCESS = 200
local RFERR_INSUFFICIENT_PRIVILEGE = 403
local RFERR_NO_RESOURCE = 404
local USER_SET_PASS_COMPLEXITY_CHECK_UNSUPPORT = 9008
local TLS_VERSION_UNSUPPORT_ERR = -3
local TLS_ERR_WITH_MUTUAL_AUTHENTICATIONS = -5

local pwdCompErrMap = {
    [VOS_OK] = reply_ok(),
    [RFERR_SUCCESS] = reply_ok(),
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("InsufficientPrivilege"),
    [RFERR_NO_RESOURCE] = reply_bad_request("ResourceMissingAtURI"),
    [USER_SET_PASS_COMPLEXITY_CHECK_UNSUPPORT] = reply_bad_request("PasswordForbidSetComplexityCheck")
}

local setTlsErrMap = {
    [VOS_OK] = reply_ok(),
    [RFERR_SUCCESS] = reply_ok(),
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("InsufficientPrivilege"),
    [RFERR_NO_RESOURCE] = reply_bad_request("ResourceMissingAtURI"),
    [USER_SET_PASS_COMPLEXITY_CHECK_UNSUPPORT] = reply_bad_request("PasswordForbidSetComplexityCheck"),
    [TLS_VERSION_UNSUPPORT_ERR] = reply_bad_request("TLSVersionConfigurationNotSupported"),
    [TLS_ERR_WITH_MUTUAL_AUTHENTICATIONS] = reply_bad_request("TLSFailedWithTwoFactorCertification")
}

local function HasPrivilege(user, priv)
    return bit.band(user.Privilege, priv) ~= 0
end

local function HasUserMgntPrivilege(user)
    return HasPrivilege(user, USERROLE_USERMGNT)
end

local function HasSecurityMgntPrivilege(user)
    return HasPrivilege(user, USERROLE_SECURITYMGNT)
end

local function reply_forbidden(code, msg)
    return reply_error(HTTP_FORBIDDEN, code, msg)
end

local RFPROP_TLS_10 = 'TLS1.0'
local RFPROP_TLS_11 = 'TLS1.1'
local RFPROP_TLS_12 = 'TLS1.2'
local RFPROP_TLS_13 = 'TLS1.3'
local function TlsVersion(v)
    local result = {}
    if bit.band(v, 0x01) ~= 0 then
        result[#result + 1] = RFPROP_TLS_10
    end
    if bit.band(v, 0x2) ~= 0 then
        result[#result + 1] = RFPROP_TLS_11
    end
    if bit.band(v, 0x4) ~= 0 then
        result[#result + 1] = RFPROP_TLS_12
    end
    if bit.band(v, 0x8) ~= 0 then
        result[#result + 1] = RFPROP_TLS_13
    end
    return result
end

local function GetTlsVersionVal(tlsVersions)
    local tlsVersion = 0xFFF0
    for _, v in ipairs(tlsVersions) do
        if v == RFPROP_TLS_10 then
            tlsVersion = bit.bor(tlsVersion, 1)
        elseif v == RFPROP_TLS_11 then
            tlsVersion = bit.bor(tlsVersion, 2)
        elseif v == RFPROP_TLS_12 then
            tlsVersion = bit.bor(tlsVersion, 4)
        elseif v == RFPROP_TLS_13 then
            tlsVersion = bit.bor(tlsVersion, 8)
        end
    end
    return tlsVersion
end

local function GetEmergencyLoginUser(user, obj)
    if HasUserMgntPrivilege(user) then
        return obj.ExcludeUser:next(function(uid)
            return C.User('UserId', uid):next(function(userObj)
                return userObj.UserName
            end)
        end):fetch_or('')
    else
        return null
    end
end

local function AuthModeValToStr(v)
    return v == 1 and 'Password' or 'PublicKey'
end

local function AuthModeStrToVal(v)
    if v == 'Password' then
        return 1
    elseif v == 'PublicKey' then
        return 0
    end
    error(string.format('invalid auth mod %s', v))
end

local function MatchStartEndTime(s)
    local ret = {}
    if s and #s > 0 then
        for v in s:gmatch('([^/]+)') do
            ret[#ret + 1] = v
        end
    end
    return ret[1], ret[2]
end

local function GetRoleEnable(roleId, tmpValue)
    if roleId == 0 then
        return bit.band(0x1, tmpValue) ~= 0
    elseif roleId == 1 then
        return bit.band(0x2, tmpValue) ~= 0
    elseif roleId == 2 then
        return bit.band(0x4, tmpValue) ~= 0
    end
    return false
end

local function SetRoleEnable(permitId, roleId, status)
    local bb = bit.lshift(1, roleId)
    if status then
        return bit.bor(permitId, bb)
    else
        return bit.band(permitId, bit.bnot(bb))
    end
end

local function CheckStartEndTime(StartTime, EndTime)
    if StartTime ~= nil and EndTime ~= nil then
        if #StartTime ~= #EndTime then
            return reply_bad_request('PropertyValueFormatError', 'StartTime/EndTime')
        end
    elseif StartTime ~= nil then
        return reply_bad_request('PropertyMissing', 'EndTime')
    elseif EndTime ~= nil then
        return reply_bad_request('PropertyMissing', 'StartTime')
    end
end

local function CheckIp(Ip)
    if not Ip then
        return
    end

    local mm = Ip:match('/(%d+)$', -3)
    if not mm then
        return
    end

    local v = tonumber(mm)
    if v >= 1 and tonumber(mm) <= 32 then
        return
    end

    return reply_bad_request('PropertyValueFormatError', 'Ip/mask')
end

local function ChekcLoginRuleInput(data)
    local err = CheckStartEndTime(data.StartTime, data.EndTime)
    if err then
        return err
    end

    err = CheckIp(data.IP)
    if err then
        return err
    end
end

local function GetAvailableUsers(user)
    return C.User():fold(function(obj, acc)
        if obj.IsUserEnable:fetch_or() ~= 1 then
            return acc
        end
        local roleId = obj.UserRoleId:fetch()
        if C.UserRole('RoleId', roleId):next(function(userRoleObj)
            return userRoleObj.RoleName
        end):eq('Administrator'):fetch_or(false) then
            local userName = obj.UserName:fetch()
            if not HasUserMgntPrivilege(user) and (user.AuthType ~= 0 or user.UserName ~= userName) then
                return acc
            end
            if userName and #userName > 0 then
                acc[#acc + 1] = userName
            end
        end
        return acc
    end, {}):fetch()
end

local function SystemLockDownSupport()
    return C.PMEServiceConfig[0]:next(function(obj)
        return obj.SystemLockDownSupport
    end):eq(PME_SERVICE_SUPPORT):fetch_or(false)
end

local function GetSystemLockDownStatus(obj)
    return obj.SystemLockDownStatus:next(function(v)
        return v == ENABLE
    end):fetch_or(null)
end

local SYS_LOCKDOWN_FORBID = 0 -- 开启系统锁定后禁止SET操作
local function redfish_system_lockdown_check(obj, allow)
    if SystemLockDownSupport() and GetSystemLockDownStatus(obj) == true then
        if allow == SYS_LOCKDOWN_FORBID then
            return false
        end
    end
    return true
end

local function SetEnableUserMgnt(user, obj, Enabled)
    if Enabled == nil then
        return
    end

    if not HasUserMgntPrivilege(user) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'OSUserManagementEnabled'})
    end

    return safe_call(function()
        local v = Enabled and 1 or 0
        local ok, ret = call_method(user, obj, 'SetEnableUserMgnt', nil, gbyte(v))
        if not ok then
            return ret
        end
    end)
end

local function SelfReturnFunction(errCode)
    if pwdCompErrMap[errCode] then
        return pwdCompErrMap[errCode]
    end
    return http.reply_internal_server_error()
end

local function SetEnablePwdComplexity(user, _, Enabled)
    if Enabled == nil then
        return
    end

    if not HasSecurityMgntPrivilege(user) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'PasswordComplexityCheckEnabled'})
    end

    return safe_call(function()
        local v = Enabled and 1 or 0
        local pwdSettingObj = O.PasswdSetting
        local ok, ret = call_method(user, pwdSettingObj, 'SetEnablePwdComplexity', SelfReturnFunction, gbyte(v))
        if not ok then
            return ret
        end
        return SelfReturnFunction(ret[1])
    end)
end

local function SetSSHPasswordAuthentication(user, obj, Mode)
    if Mode == nil then
        return
    end

    if not HasUserMgntPrivilege(user) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'SSHAuthenticationMode'})
    end

    return safe_call(function()
        local val = AuthModeStrToVal(Mode)
        local ok, ret = call_method(user, obj, 'SetSSHPasswordAuthentication', nil, gbyte(val))
        if not ok then
            return ret
        end
    end)
end

local function SetTLSVersion(user, obj, tlsVersions)
    if tlsVersions == nil then
        return
    end

    if not HasSecurityMgntPrivilege(user) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'TLSVersion'})
    end

    return safe_call(function()
        local version = GetTlsVersionVal(tlsVersions)
        local ok, ret = call_method(user, obj, 'SetTLSVersion', setTlsErrMap, guint16(version))
        if not ok then
            return ret
        end
    end)
end

local function SetMinimumPasswordLength(user, _, MinimumPasswordLength)
    if MinimumPasswordLength == nil then
        return
    end
    if not HasSecurityMgntPrivilege(user) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'MinimumPasswordLength'})
    end

    return safe_call(function()
        local pwdSettingObj = O.PasswdSetting
        local ok, ret = call_method(user, pwdSettingObj, 'SetMinPasswordLength', nil, gbyte(MinimumPasswordLength))
        if not ok then
            return ret
        end
    end)
end

local function SetPwdExpiredAndMinimumTime(user, obj, PasswordValidityDays, MinimumPasswordAgeDays)
    if PasswordValidityDays == nil and MinimumPasswordAgeDays == nil then
        return
    end

    if not HasSecurityMgntPrivilege(user) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'PwdExpiredAndMinimumTime'})
    end

    if not PasswordValidityDays then
        PasswordValidityDays = obj.PwdExpiredTime:fetch()
    end

    if not MinimumPasswordAgeDays then
        MinimumPasswordAgeDays = obj.MinimumPwdAge:fetch()
    end

    return safe_call(function()
        local input = {guint32(PasswordValidityDays), guint32(MinimumPasswordAgeDays)}
        local ok, ret = call_method(user, obj, 'WebSetPwdExpiredAndMinimumTime', function(errId)
            if errId == USER_TIME_LIMIT_UNREASONABLE then
                local msg = string.format('minimum_pwd_age(%d) is no less than pwd_expired_time(%d) minus 10',
                    MinimumPasswordAgeDays, PasswordValidityDays)
                return reply_bad_request('MinPwdAgeAndPwdValidityRestrictEachOther', msg)
            end
        end, input)
        if not ok then
            return ret
        end
    end)
end

local function SetUserInactTimeLimit(user, obj, UserInactTimeLimit)
    if UserInactTimeLimit == nil then
        return
    end

    if not HasSecurityMgntPrivilege(user) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'AccountInactiveTimelimitDays'})
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, 'SetUserInactTimeLimit', function(errId)
            if errId == USER_PARAMETER_OUT_OF_RANGE then
                return reply_bad_request('PropertyValueNotInList')
            end
        end, guint32(UserInactTimeLimit))
        if not ok then
            return ret
        end
    end)
end

local function SetExcludeUser(user, obj, name)
    if name == nil then
        return
    end

    if not HasSecurityMgntPrivilege(user) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'EmergencyLoginUser'})
    end

    return safe_call(function()
        local uid = 0
        if #name > 0 then
            if not HasUserMgntPrivilege(user) then
                return reply_forbidden('InsufficientPrivilege')
            end
            local ret = C.User('UserName', name):next(function(userObj)
                local privilege = userObj.Privilege:fetch()
                local isUserEnabled = userObj.IsUserEnable:fetch()
                if privilege ~= RMCP_ADMIN or isUserEnabled ~= RF_ENABLE then
                    return reply_bad_request('EmergencyLoginUserSettingFail')
                end
                return userObj.UserId
            end):fetch_or(reply_bad_request('InvalidUserName'))
            if http.is_reply(ret) then
                return ret
            end
            uid = ret
        end

        local ok, ret = call_method(user, obj, 'SetExcludeUser', nil, gbyte(uid))
        if not ok then
            return ret
        end
    end)
end

local function SetOldPwdCount(user, obj, oldPwdCount)
    if oldPwdCount == nil then
        return
    end

    if not HasSecurityMgntPrivilege(user) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'PreviousPasswordsDisallowedCount'})
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, 'SetOldPwdCount', nil, gbyte(oldPwdCount))
        if not ok then
            return ret
        end
    end)
end

local function SetAuthFailMaxExtension(user, obj, val)
    if val == nil then
        return
    end

    if not HasSecurityMgntPrivilege(user) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'AccountLockoutThreshold'})
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, 'SetAuthFailMaxExtension', nil, guint16(val))
        if not ok then
            return ret
        end
    end)
end

local function SetAuthFailLockTimeExtension(user, obj, val)
    if val == nil then
        return
    end

    if not HasSecurityMgntPrivilege(user) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'AccountLockoutDuration'})
    end

    return safe_call(function()
        if not HasSecurityMgntPrivilege(user) then
            return reply_bad_request('PropertyModificationNeedPrivilege')
        end

        local ok, ret = call_method(user, obj, 'SetAuthFailLockTimeExtension', nil, guint16(val * 60))
        if not ok then
            return ret
        end
    end)
end

local function SetCertOverdueWarnTime(user, obj, val)
    if val == nil then
        return
    end

    if not HasSecurityMgntPrivilege(user) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'CertificateOverdueWarningTime'})
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, 'SetCertOverdueWarnTime', function(errId)
            if errId == USER_PARAMETER_OUT_OF_RANGE then
                return reply_bad_request('PropertyValueNotInList')
            end
        end, guint32(val))
        if not ok then
            return ret
        end
    end)
end

local function SetSystemLockDownEnabled(user, obj, Enabled)
    if Enabled == nil then
        return
    end

    if not HasSecurityMgntPrivilege(user) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'SystemLockDownEnabled'})
    end

    if SystemLockDownSupport() then
        return safe_call(function()
            local v = Enabled and 1 or 0
            local ok, ret = call_method(user, obj, 'SetSystemLockDownStatus', nil, gbyte(v))
            if not ok then
                return ret
            end
        end)
    else
        return reply_bad_request('PropertyModificationNotSupported')
    end
end

local function SetAntiDNSRebindEnabled(user, obj, enabled)
    if enabled == nil then
        return
    end

    if not HasSecurityMgntPrivilege(user) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'AntiDNSRebindEnabled'})
    end

    return safe_call(function ()
        local v = enabled and 1 or 0
        local ok, ret = call_method(user, obj, 'SetAntiDNSRebindStatus', nil, gbyte(v))
        if not ok then
            return ret
        end
    end)
end

local function SetPermitRuleIds(user, roleId, Status)
    if Status == nil then
        return
    end

    return safe_call(function()
        local obj = O.SecurityEnhance
        local permitId = SetRoleEnable(obj.PermitRuleIds:fetch(), roleId, Status)
        local ok, ret = call_method(user, obj, 'SetPermitRuleIds', nil, gbyte(permitId))
        if not ok then
            return ret
        end
    end)
end

local function SetTimeRuleInfo(user, obj, StartTime, EndTime)
    if StartTime == nil or EndTime == nil then
        return
    end

    return safe_call(function()
        local v = #StartTime == 0 and '' or (StartTime .. '/' .. EndTime)
        local ok, ret = call_method(user, obj, 'SetTimeRuleInfo', nil, gstring(v))
        if not ok then
            return ret
        end
    end)
end

local function SetIpRuleInfo(user, obj, val)
    if val == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, 'SetIpRuleInfo', nil, gstring(val))
        if not ok then
            return ret
        end
    end)
end

local function SetMacRuleInfo(user, obj, val)
    if val == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, 'SetMacRuleInfo', nil, gstring(val))
        if not ok then
            return ret
        end
    end)
end

local function CheckAndSetRight(user, obj, name, status)
    return safe_call(function()
        local v = status and 1 or 0
        if obj[name]:fetch() == v then
            return
        end

        local ok, ret = call_method(user, obj, 'Set' .. name, nil, gbyte(v))
        if not ok then
            return ret
        end
    end)
end

local function SetBannerState(user, obj, val)
    if val == nil then
        return
    end

    return safe_call(function()
        if not redfish_system_lockdown_check(obj, SYS_LOCKDOWN_FORBID) then
            return reply_bad_request('SystemLockdownForbid')
        end

        local v = val and 1 or 0
        local ok, ret = call_method(user, obj, 'SetBannerState', nil, gbyte(v))
        if not ok then
            return ret
        end
    end)
end

local function SetBannerContent(user, obj, val)
    if val == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, 'SetBannerContent', nil, gstring(val))
        if not ok then
            return ret
        end
    end)
end

local M = {}

function M.GetAdvancedSecurity(user)
    local EnableStrongPassword = O.PasswdSetting:next(function(obj)
        return obj.EnableStrongPassword:fetch() ~= 0
    end):fetch_or(false)
    local MinPasswordLength = O.PasswdSetting.MinPasswordLength:fetch() 
    return O.SecurityEnhance:next(function(obj)
        local SystemLockDownEnabled = nil
        if SystemLockDownSupport() then
            SystemLockDownEnabled = GetSystemLockDownStatus(obj)
        end
        local result = {
            PasswordComplexityCheckEnabled = EnableStrongPassword,
            SSHAuthenticationMode = AuthModeValToStr(obj.SSHPasswordAuthentication:fetch()),
            TLSVersion = TlsVersion(obj.TLSVersion:fetch()),
            PasswordValidityDays = obj.PwdExpiredTime:fetch(),
            MinimumPasswordLength = MinPasswordLength,
            MinimumPasswordAgeDays = obj.MinimumPwdAge:fetch(),
            AccountInactiveTimelimitDays = obj.UserInactTimeLimit:fetch(),
            AvailableUsers = GetAvailableUsers(user),
            PreviousPasswordsDisallowedCount = obj.OldPwdCount:fetch(),
            AccountLockoutThreshold = obj.AuthFailMaxExtension:fetch(),
            AccountLockoutDuration = math_abs(obj.AuthFailLockTimeExtension:fetch() / 60),
            CertificateOverdueWarningTime = obj.CertOverdueWarnTime:fetch(),
            SystemLockDownEnabled = SystemLockDownEnabled,
            AntiDNSRebindEnabled = obj.AntiDNSRebindStatus:fetch() == 1,
        }
        if not IsMgmtBoard() then
            result.OSUserManagementEnabled = obj.EnableUserMgnt:fetch() == 1
        end
        -- 具有用户配置权限才可以看到紧急用户
        if HasUserMgntPrivilege(user) then
            result.EmergencyLoginUser = GetEmergencyLoginUser(user, obj)
        end
        return result
    end):fetch()
end

function M.UpdateAdvancedSecurity(data, user)
    return O.SecurityEnhance:next(function(obj)
        local result = reply_ok()
        if not redfish_system_lockdown_check(obj, SYS_LOCKDOWN_FORBID) then
            local count = 0
            for _, _ in pairs(data) do
                count = count + 1
            end
            if count == 1 and data.SystemLockDownEnabled ~= nil then
                result:join(SetSystemLockDownEnabled(user, obj, data.SystemLockDownEnabled))
            else
                return reply_bad_request('SystemLockdownForbid')
            end
        else
            result:join(SetEnableUserMgnt(user, obj, data.OSUserManagementEnabled))
            result:join(SetEnablePwdComplexity(user, obj, data.PasswordComplexityCheckEnabled))
            result:join(SetSSHPasswordAuthentication(user, obj, data.SSHAuthenticationMode))
            result:join(SetTLSVersion(user, obj, data.TLSVersion))
            result:join(SetPwdExpiredAndMinimumTime(user, obj, data.PasswordValidityDays,
                data.MinimumPasswordAgeDays))
            result:join(SetMinimumPasswordLength(user, obj, data.MinimumPasswordLength))
            result:join(SetUserInactTimeLimit(user, obj, data.AccountInactiveTimelimitDays))
            result:join(SetExcludeUser(user, obj, data.EmergencyLoginUser))
            result:join(SetOldPwdCount(user, obj, data.PreviousPasswordsDisallowedCount))
            result:join(SetAuthFailMaxExtension(user, obj, data.AccountLockoutThreshold))
            result:join(SetAuthFailLockTimeExtension(user, obj, data.AccountLockoutDuration))
            result:join(SetCertOverdueWarnTime(user, obj, data.CertificateOverdueWarningTime))
            result:join(SetSystemLockDownEnabled(user, obj, data.SystemLockDownEnabled))
            result:join(SetAntiDNSRebindEnabled(user, obj, data.AntiDNSRebindEnabled))
        end

        if result:isOk() then
            return reply_ok_encode_json(M.GetAdvancedSecurity(user))
        end
        result:appendErrorData(M.GetAdvancedSecurity(user))
        return result
    end):fetch()
end

function M.GetLoginRule()
    local enable = O.SecurityEnhance.PermitRuleIds:fetch()
    return C.PermitRule():fold(function(obj, acc)
        local ID = obj.RuleId:fetch()
        local startTime, endTime = MatchStartEndTime(obj.TimeRuleInfo:fetch())
        if startTime == nil then
            startTime = ""
        end
        if endTime == nil then
            endTime = ""
        end
        acc[#acc + 1] = {
            ID = ID,
            StartTime = startTime,
            EndTime = endTime,
            IP = obj.IpRuleInfo:fetch(),
            Mac = obj.MacRuleInfo:fetch(),
            Status = GetRoleEnable(ID, enable)
        }
        return acc
    end, {}):next(function(acc)
        return {Count = #acc, Members = acc}
    end):fetch()
end

function M.UpdateLoginRule(data, user)
    local err = ChekcLoginRuleInput(data)
    if err then
        return err
    end

    return C.PermitRule('RuleId', data.ID):next(function(obj)
        local result = reply_ok()

        result:join(SetPermitRuleIds(user, data.ID, data.Status))
        result:join(SetTimeRuleInfo(user, obj, data.StartTime, data.EndTime))
        result:join(SetIpRuleInfo(user, obj, data.IP))
        result:join(SetMacRuleInfo(user, obj, data.Mac))

        if result:isOk() then
            return reply_ok_encode_json(M.GetLoginRule(user))
        end
        result:appendErrorData(M.GetLoginRule(user))
        return result
    end):fetch()
end

function M.GetRightManagement(user)
    return C.UserRole():fold(function(obj, acc)
        if not HasUserMgntPrivilege(user) and not HasRoleId(user, obj.RoleId:fetch()) then
            return acc
        end
        acc[#acc + 1] = {
            ID = obj.RoleId:fetch(),
            Name = obj.RoleName:fetch(),
            UserConfigEnabled = obj.UserMgnt:fetch() == 1,
            BasicConfigEnabled = obj.BasicSetting:fetch() == 1,
            SecurityConfigEnabled = obj.SecurityMgnt:fetch() == 1,
            PowerControlEnabled = obj.PowerMgnt:fetch() == 1,
            DiagnosisEnabled = obj.DiagnoseMgnt:fetch() == 1,
            QueryEnabled = obj.ReadOnly:fetch() == 1,
            ConfigureSelfEnabled = obj.ConfigureSelf:fetch() == 1
        }
        if not IsMgmtBoard() then
            acc[#acc].RemoteControlEnabled = obj.KVMMgnt:fetch() == 1
            acc[#acc].VMMEnabled = obj.VMMMgnt:fetch() == 1
        end
        return acc
    end, {}):next(function(acc)
        return {Count = #acc, Members = acc}
    end):fetch()
end

function M.UpdateRightManagement(data, user)
    local reauth = utils.ReAuthUser(user, data.ReauthKey)
    if reauth then
        utils.OperateLog(user, 'Failed to set right management, because the current user password is incorrect or the account is locked')
        return reauth
    end
    if dal_rf_get_board_type():eq(BOARD_SWITCH):fetch_or(false) then
        logging:error("Update rightmanagement failed")
        return reply_error(HTTP_METHOD_NOT_IMPLEMENTED, 'PropertyModificationNotSupported')
    end

    if not ((2 <= data.ID and data.ID <= 8) or data.ID == 15) then
        logging:error("Update rightmanagement failed")
        return reply_bad_request("InvalidParam")
    end

    if (2 <= data.ID and data.ID <= 4) or data.ID == 15 then
        logging:error("Update rightmanagement failed")
        return reply_bad_request("PropertyModificationNotSupported")
    end

    return C.UserRole('RoleId', data.ID):next(function(obj)
        local result = reply_ok()

        result:join(CheckAndSetRight(user, obj, 'UserMgnt', data.UserConfigEnabled))
        result:join(CheckAndSetRight(user, obj, 'BasicSetting', data.BasicConfigEnabled))
        if not IsMgmtBoard() then
            result:join(CheckAndSetRight(user, obj, 'KVMMgnt', data.RemoteControlEnabled))
            result:join(CheckAndSetRight(user, obj, 'VMMMgnt', data.VMMEnabled))
        end
        result:join(CheckAndSetRight(user, obj, 'SecurityMgnt', data.SecurityConfigEnabled))
        result:join(CheckAndSetRight(user, obj, 'PowerMgnt', data.PowerControlEnabled))
        result:join(CheckAndSetRight(user, obj, 'DiagnoseMgnt', data.DiagnosisEnabled))
        result:join(CheckAndSetRight(user, obj, 'ConfigureSelf', data.ConfigureSelfEnabled))

        if result:isOk() then
            return reply_ok_encode_json(M.GetRightManagement(user))
        end
        result:appendErrorData(M.GetRightManagement(user))
        return result
    end):fetch()
end

function M.GetSecurityBanner(user)
    return O.SecurityEnhance:next(function(obj)
        return {
            Enabled = obj.BannerState:fetch() == 1,
            SecurityBanner = obj['BannerContent']:fetch(),
            DefaultSecurityBanner = obj['d:BannerContent']:fetch()
        }
    end):fetch()
end

function M.UpdateSecurityBanner(data, user)
    return O.SecurityEnhance:next(function(obj)
        local result = reply_ok()

        result:join(SetBannerState(user, obj, data.Enabled))
        result:join(SetBannerContent(user, obj, data.SecurityBanner))

        if result:isOk() then
            return reply_ok_encode_json(M.GetSecurityBanner(user))
        end
        result:appendErrorData(M.GetSecurityBanner(user))
        return result
    end):fetch()
end

return M
