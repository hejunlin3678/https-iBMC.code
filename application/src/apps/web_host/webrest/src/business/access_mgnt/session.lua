local http = require 'http'
local cjson = require 'cjson'
local dflib = require 'dflib'
local utils = require 'utils'
local core = require 'dflib.core'
local cfg = require 'config'
local null = cjson.null
local C = dflib.class
local gstring = core.GVariant.new_string
local gWebUiFlag = utils.gWebUiFlag
local HasUserMgntPrivilege = utils.HasUserMgntPrivilege
local reply_forbidden = http.reply_forbidden

local M = {}

local function GetUserID(authType, userID)
    if authType == cfg.LOG_TYPE_LOCAL and userID > 0 then
        return userID
    else
        return null
    end
end

function M.GetSessions(user)
    return C.Session[0]:next(function(session_obj)
        local caller_info = {gWebUiFlag, gstring(user.UserName), gstring(user.ClientIp)}
        local input_list = {gstring(user.SessionId)}
        local session_rsp = session_obj:call('GetSessionList', caller_info, input_list)
        local sessions = {}
        for _, v in ipairs(cjson.decode(session_rsp[1])) do
            local session_info = session_obj:call('GetSessionInfo', caller_info, {gstring(v.session_checksum)})
            local session = cjson.decode(session_info[1])
            local my_session = false
            if v.session_checksum == user.SessionId then
                my_session = true
            end
            if HasUserMgntPrivilege(user) or my_session then
                sessions[#sessions + 1] = {
                    SessionID = v.session_checksum,
                    UserID = GetUserID(session.authenticateType, session.local_userid),
                    UserName = session.loginname,
                    MySession = my_session,
                    LoginTime = core.get_datetime(session.logintime),
                    IPAddress = session.loginipaddress,
                    Role = utils.roleid_to_str(tonumber(session.user_roleid)),
                    Interface = session.session_type
                }
            end
        end
        return http.reply_ok_encode_json({Members = sessions, Count = #sessions})
    end):fetch()
end

function M.DeleteSession(session_id, user)
    if not session_id or #session_id > 128 or #session_id == 0 and not string.match(session_id, "^[a-zA-Z0-9]+$") then
        return http.reply_bad_request("Invalid param")
    end

    if session_id == user.SessionId then
        return C.Session[0]:next(function(session_obj)
            local caller_info = {gWebUiFlag, gstring(user.UserName), gstring(user.ClientIp)}
            local input_list = {gstring(session_id)}
            session_obj:call('DelSession', caller_info, input_list)
            return http.reply_ok()
        end):fetch()
    end

    if not HasUserMgntPrivilege(user) then
        return reply_forbidden('InsufficientPrivilege')
    end

    local ret = C.Session[0]:next(function(session_obj)
        local caller_info = {gWebUiFlag, gstring(user.UserName), gstring(user.ClientIp)}
        local input_list = {gstring(user.SessionId)}
        local sessions_rsp = session_obj:call('GetSessionList', caller_info, input_list)
        local del_session
        for _, v in ipairs(cjson.decode(sessions_rsp[1])) do
            if v.session_checksum == session_id then
                del_session = {
                    SessionID = v.session_id,
                    UserName = v.loginname,
                    IPAddress = v.loginipaddress,
                    Interface = v.session_type
                }
                break
            end
        end

        if del_session == nil then
            return http.reply_bad_request('ResourceMissingAtURI')
        end

        input_list = {
            gstring(del_session.SessionID), gstring(del_session.Interface), gstring(del_session.UserName),
            gstring(del_session.IPAddress)
        }
        session_obj:call('LogoutUser', caller_info, input_list)
    end):fetch()

    if ret then
        return ret
    end

    return http.reply_ok()
end

return M
