local dflib = require 'dflib'
local c = require 'dflib.core'
local http = require 'http'
local cjson = require 'cjson'
local user_defs = require 'user_defs'
local GVariant = c.GVariant
local compute_checksum = c.compute_checksum
local C = dflib.class
local DFL_OK = dflib.DFL_OK
local gstring = GVariant.new_string
local gbyte = GVariant.new_byte
local gint32 = GVariant.new_int32
local json_decode = cjson.decode
local reply_ok = http.reply_ok
local reply_bad_request = http.reply_bad_request

local LOG_TYPE_LOCAL = 0
local LOG_TYPE_LDAP_AUTO_MATCH = 1
local G_CHECKSUM_SHA256 = 2
local REDFISH_REFRESH = 1
local UUSER_USERNAME_TOO_LONG = -181
local UUSER_USERPASS_TOO_LONG = -182
local RPC_UNKNOWN = -200
local LOG_TYPE_LDAP_SERVER1 = 2
local LOG_TYPE_LDAP_SERVER6 = 7
local LOG_TYPE_LDAP_KRB_SSO = 12
local LOG_TYPE_KRB_SERVER = 13
local LOG_TYPE_SSO_LOCAL = 14
local LOG_TYPE_SSO_LDAP = 15
local USER_NAME_NOT_CONTAIN_DOMAIN = 1
local USER_NAME_NEED_POST_BACK = 2

local g_verify_account_err_code = {
    user_defs.INVALID_PASSWORD, user_defs.USER_DONT_EXIST, UUSER_USERNAME_TOO_LONG, UUSER_USERPASS_TOO_LONG,
    user_defs.USER_NO_ACCESS, user_defs.USER_PASSWORD_EXPIRED, user_defs.USER_LOGIN_LIMITED,
    user_defs.USER_IS_LOCKED, RPC_UNKNOWN
}

local auth_ret_map = {
    [user_defs.USER_UNSUPPORT] = 'AuthorizationFailed',
    [user_defs.INVALID_PASSWORD] = 'AuthorizationFailed',
    [user_defs.USER_DONT_EXIST] = 'AuthorizationFailed',
    [user_defs.UUSER_USERNAME_TOO_LONG] = 'AuthorizationFailed',
    [user_defs.UUSER_USERPASS_TOO_LONG] = 'AuthorizationFailed',
    [user_defs.USER_NO_ACCESS] = 'NoAccess',
    [user_defs.USER_PASSWORD_EXPIRED] = 'UserPasswordExpired',
    [user_defs.USER_LOGIN_LIMITED] = 'UserLoginRestricted',
    [user_defs.USER_IS_LOCKED] = 'UserLocked'
}

local function verify_local_account_and_get_info(UserName, Password, ClientIp)
    local gUserName = gstring(UserName)
    local gPassword = gstring(Password)
    local gUserType = gbyte(LOG_TYPE_LOCAL)
    local gIpAddr = gstring(ClientIp or '127.0.0.1')
    local inputList = {gUserName, gPassword, gUserType, gIpAddr}
    local callerInfo = {gstring('Redfish'), gUserName, gIpAddr}
    local userInfo = nil
    return C.User[0]:next(function(obj)
        local authUserRsp = obj:call('AuthUser', callerInfo, inputList)
        if authUserRsp[1] ~= DFL_OK then
            return authUserRsp[1]
        end
        userInfo = {
            UserId = authUserRsp[3],
            AuthType = authUserRsp[4],
            Privilege = authUserRsp[5],
            RoleId = authUserRsp[6],
            RealUser = UserName
        }
        return DFL_OK
    end):fetch(), userInfo
end

local function GetSpecialLdapRealUser(username, flag, tmp_str)
    if flag == USER_NAME_NEED_POST_BACK then
        return tmp_str
    elseif flag == USER_NAME_NOT_CONTAIN_DOMAIN then
        return username .. '@' .. tmp_str
    end
    return username
end

local function generate_account_verify_input(domain, username, password, client)
    local g_username = gstring(username)
    local g_password = gstring(password)
    local g_user_type = gbyte(domain)
    local g_ip_addr = gstring(client)
    return {g_username, g_password, g_user_type, g_ip_addr}, {gstring('Redfish'), g_username, g_ip_addr}
end

local function verify_special_ldap_account_and_get_info(domain, username, password, client)
    if (domain < LOG_TYPE_LDAP_SERVER1 or domain > LOG_TYPE_LDAP_SERVER6) and (domain ~= LOG_TYPE_KRB_SERVER) and
        (domain ~= LOG_TYPE_LDAP_KRB_SSO) and (domain ~= LOG_TYPE_SSO_LOCAL) and (domain ~= LOG_TYPE_SSO_LDAP) then
        return -1
    end
    local input_list, caller_info = generate_account_verify_input(domain, username, password, client)
    local user_info = nil
    return C.User[0]:next(function(obj)
        local auth_user_rsp = obj:call('AuthUser', caller_info, input_list)
        if auth_user_rsp[1] ~= DFL_OK then
            return auth_user_rsp[1]
        end
        user_info = {
            UserId = auth_user_rsp[3],
            AuthType = domain,
            Privilege = auth_user_rsp[9],
            RoleId = auth_user_rsp[4],
            RealUser = GetSpecialLdapRealUser(username, auth_user_rsp[7], auth_user_rsp[8])
        }
        return DFL_OK
    end):fetch(), user_info
end

local function verify_ldap_account_and_get_info(username, password, client)
    local input_list, caller_info = generate_account_verify_input(LOG_TYPE_LDAP_AUTO_MATCH, username, password,
        client)
    local user_info = nil
    return C.User[0]:next(function(obj)
        local auth_user_rsp = obj:call('AuthUser', caller_info, input_list)
        if auth_user_rsp[1] ~= DFL_OK then
            return auth_user_rsp[1]
        end
        local flag = auth_user_rsp[7]
        user_info = {
            UserId = auth_user_rsp[3],
            AuthType = auth_user_rsp[6],
            Privilege = auth_user_rsp[9],
            RoleId = auth_user_rsp[4],
            RealUser = (flag == USER_NAME_NOT_CONTAIN_DOMAIN) and username .. '@' .. auth_user_rsp[8] or username
        }
        return DFL_OK
    end):fetch(), user_info
end

local function verify_account_and_get_info(UserName, Password, ClientIp)
    local rets = {}
    local ret, data = verify_local_account_and_get_info(UserName, Password, ClientIp)
    if ret == DFL_OK then
        return ret, data
    end
    rets[#rets + 1] = ret

    -- 自动匹配时，优先使用krb校验
    ret, data = verify_special_ldap_account_and_get_info(LOG_TYPE_KRB_SERVER, UserName, Password, ClientIp)
    if ret == DFL_OK then
        return ret, data
    end
    rets[#rets + 1] = ret

    ret, data = verify_ldap_account_and_get_info(UserName, Password, ClientIp)
    if ret == DFL_OK then
        return ret, data
    end
    rets[#rets + 1] = ret

    local max_code = -1
    for _, verify_ret in ipairs(rets) do
        for _, r in ipairs(g_verify_account_err_code) do
            if verify_ret == r and verify_ret > max_code then
                max_code = verify_ret
            end
        end
    end
    return max_code, nil
end

local M = {}

local errNoValidSession = http.reply_error(http.status.HTTP_UNAUTHORIZED, 'NoValidSession')
function M.AuthToken(data)
    if data.basic then
        local basic = data.basic
        local ret, userInfo = verify_account_and_get_info(basic.UserName, basic.Password, basic.ClientIp)
        if ret ~= DFL_OK then
            return reply_bad_request(auth_ret_map[ret] or 'LoginFailed')
        end
        return reply_ok({
            Privilege = userInfo.Privilege,
            UserName = basic.UserName,
            ClientIp = basic.ClientIp,
            RoleId = userInfo.RoleId,
            SessionId = '',
            AuthType = userInfo.AuthType
        })
    elseif data.token then
        return C.Session[0]:next(function(obj)
            local sidCheckSum = compute_checksum(G_CHECKSUM_SHA256, data.token):sub(1, 16)
            local callerInfo = {gstring('Redfish'), gint32(REDFISH_REFRESH)}
            local inputList = {gstring(sidCheckSum)}
            local sessionRsp = obj:call('GetOnlineUser', callerInfo, inputList)
            if sessionRsp[1] and #sessionRsp[1] > 0 then
                local session = json_decode(sessionRsp[1])
                return reply_ok({
                    Privilege = session.user_privilege,
                    UserName = session.loginname,
                    ClientIp = session.loginipaddress,
                    RoleId = session.user_roleid,
                    SessionId = session.session_id,
                    AuthType = session.authenticateType
                })
            else
                return errNoValidSession
            end
        end):fetch()
    end
    return errNoValidSession
end

return M
