local http = require 'http'
local defs = require 'define'
local dflib = require 'dflib'
local tools = require 'tools'
local utils = require 'utils'
local core = require 'dflib.core'
local O = dflib.object
local gbyte = core.GVariant.new_byte
local gstring = core.GVariant.new_string
local get_system_name = utils.get_system_name
local reply_not_found = http.reply_not_found

-- 双因素登录响应码
local RSP_LOGIN_BY_USER = 0x0
local RSP_LOGIN_BY_CERT = 0x1
local RSP_LOGIN_FAILED = 0x2
local RSP_LOGOUT_BY_CERT = 0x3
local RSP_LOGIN_LIMITED = 0x5
local RSP_SESSION_LIMITED = 0x6
local RSP_USER_LOCKED = 0x7

local M = {}

function M.MutualLogin(clientip, serial, issuer, subject, logout, old_session_id)
    local sys_name = get_system_name()
    if sys_name == 'RMM' then
        return reply_not_found()
    end
    local result = RSP_LOGIN_BY_USER
    local session = ''
    local token = ''
    local header = {}

    if old_session_id and (#old_session_id > 1024 or #old_session_id == 0) and
        not string.match(old_session_id, "^[a-zA-Z0-9]+$") then
        return http.reply_bad_request("Invalid param")
    end

    if serial and #serial > 1024 and not string.match(serial, "^[a-zA-Z0-9]+$") then
        return http.reply_bad_request("Invalid param")
    end

    serial = serial and serial or ""
    issuer = issuer and string.sub(string.match(issuer, "CN=[^,]+"), 4, -1) or ""
    subject = subject and string.sub(string.match(subject, "CN=[^,]+"), 4, -1) or ""

    if O.MutualAuthentication.MutualAuthenticationState:fetch() == 1 then
        if logout ~= nil then
            return http.reply_ok_encode_json({stateCode = RSP_LOGOUT_BY_CERT, Session = session, token = token}, header)
        end

        local user_info = {
            callerInfo = {gstring('Redfish-WebUI'), gstring(''), gstring(clientip)}
        }
        local input_list = {
            gstring(serial),
            gstring(issuer),
            gstring(subject),
            gstring(clientip),
            gbyte(0x10)
        }
        local ok, resp = utils.call_method(user_info, O.User1, 'AuthAndLoginByCert',
                                        function(ret)
                                            result = ret
                                            return true
                                        end, input_list)
        if ok then
            if resp[1] ~= nil and #resp[1] > 0 then
                result = RSP_LOGIN_BY_CERT
                session = core.compute_checksum(defs.G_CHECKSUM_SHA256, resp[1]):sub(1, 16)
                token = tools.get_token_by_session(session)
                --删除session
                if old_session_id then
                    utils.DeleteOldSession(clientip, old_session_id)
                end
                header = {
                    ['Set-Cookie'] = string.format('SessionId=%s; Path=/; Secure; Httponly; SameSite=Strict', resp[1])
                }
            else
                result = RSP_SESSION_LIMITED
            end
        elseif result == defs.USER_LOGIN_LIMITED then
            result = RSP_LOGIN_LIMITED
        elseif result == defs.USER_NO_ACCESS then
            result = RSP_LOGIN_LIMITED
        elseif result == defs.USER_IS_LOCKED then
            result = RSP_USER_LOCKED
        else
            result = RSP_LOGIN_FAILED
        end
    end

    return http.reply_ok_encode_json({stateCode = result, Session = session, token = token}, header)
end

function M.KBRSSOLogin(authorization)
    if authorization == nil or string.match(authorization, "[\n\r]") ~= nil then
        return http.reply(http.status.HTTP_UNAUTHORIZED, nil, {['WWW-Authenticate'] = 'Negotiate'})
    end

    return http.reply_ok(nil, {['WWW-Authenticate'] = authorization})
end

return M
