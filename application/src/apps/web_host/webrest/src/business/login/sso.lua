local http = require 'http'
local defs = require 'define'
local dflib = require 'dflib'
local utils = require 'utils'
local tools = require 'tools'
local core = require 'dflib.core'
local kvmaccess = require 'kvmaccess'
local logging = require "logging"
local O = dflib.object
local gbyte = core.GVariant.new_byte
local gstring = core.GVariant.new_string

local M = {}

local AUTH_RET_SSO_MAP = {
    [0xD5] = 0x90, -- USER_UNSUPPORT
    [0x82] = 0x90, -- INVALID_PASSWORD
    [0x81] = 0x90, -- USER_DONT_EXIST
    [-181] = 0x90, -- UUSER_USERNAME_TOO_LONG
    [-182] = 0x90, -- UUSER_USERPASS_TOO_LONG
    [0x88] = 0x88, -- USER_NO_ACCESS
    [0x89] = 0x89, -- USER_PASSWORD_EXPIRED
    [0x90] = 0x90, -- USER_LOGIN_LIMITED
    [0x83] = 0x90, -- USER_IS_LOCKED
    [0xDA] = 0x90 -- USER_LDAP_LOGIN_FAIL
}

local function creat_token(data, clientip)
    local server_id = 0
    local user_id = 1
    local role = ''
    local result
    local resp
    local ok
    local input_list
    if not data.username or not data.password or not data.ip then
        logging:error("Creat token failed")
        return http.reply_bad_request("InvalidParam")
    end

    local user_info = {
        callerInfo = {
            gstring('Redfish-WebUI'),
            gstring(data.username),
            gstring(clientip)
        }
    }
    if data.logtype == 'ldap' then
        input_list = {
            gstring(data.username),
            gstring(data.password),
            gbyte(defs.LOG_TYPE_LDAP_AUTO_MATCH),
            gstring(data.ip)
        }

        ok, resp = utils.call_method(user_info, O.User1, 'AuthUser', nil, input_list)
        if ok then
            result = resp[1]
            if result == dflib.DFL_OK then
                role = resp[4]
                user_id = resp[3]
                server_id = resp[6]
                local flag = resp[7]
                local domain = resp[8]
                if flag == defs.USER_NAME_NOT_CONTAIN_DOMAIN then
                    data.username = string.format("%s@%s", data.username, domain)
                elseif flag == defs.USER_NAME_NEED_POST_BACK then
                    data.username = domain
                end
            end
        end
    elseif data.logtype == 'local' then
        input_list = {
            gstring(data.username),
            gstring(data.password),
            gbyte(defs.LOG_TYPE_LOCAL),
            gstring(data.ip)
        }
        ok, resp = utils.call_method(user_info, O.User1, 'AuthUser', nil, input_list)
        if ok then
            result = resp[1]
            if result == dflib.DFL_OK then
                user_id = resp[3]
            end
        end
    else
        logging:error("Creat token failed")
        return http.reply_bad_request("Invalid param")
    end

    if result == dflib.DFL_OK then
        user_info = {
            callerInfo = {
                gstring('Redfish-WebUI'),
                gstring(data.username),
                gstring(clientip)
            }
        }
        input_list = {
            gbyte(server_id),
            gstring(data.ip),
            gstring(data.username),
            gbyte(user_id),
            gbyte(0),
            gstring(role)
        }
        ok, resp = utils.call_method(user_info, O.Session, 'GetSSOSessionID', nil, input_list)
        if ok then
            return http.reply_ok_encode_json({ret = tostring(result), token = resp[1]})
        end
    end

    return http.reply_ok_encode_json({ret = tostring(AUTH_RET_SSO_MAP[result] or result)})
end

local function destroy_token(data, clientip)
    if not data.token then
        logging:error("Destroy token failed")
        return http.reply_bad_request("InvalidParam")
    end
    local user_info = {
        callerInfo = {
            gstring('Redfish-WebUI'),
            gstring(""),
            gstring(clientip)
        }
    }
    local input_list = {gstring(data.token)}

    utils.call_method(user_info, O.Session, 'DestroySsoSession', nil, input_list)

    return http.reply_ok_encode_json({ret = 0})
end

function M.SSOLogin(data, clientip)
    local func = ''
    for k, v in pairs(data) do
        if k == 'function' then
            func = v
            break
        end
    end

    if func == 'addtoken' then
        return creat_token(data, clientip)
    else
        return destroy_token(data, clientip)
    end
end

local LANGUAGE_ZH_CN = "zh-CN"
local LANGUAGE_ZH = "zh"
local LANGUAGE_EN_US = "en-US"
local LANGUAGE_EN = "en"
local LANGUAGE_FR_FR = "fr-FR"
local LANGUAGE_FR = "fr"
local LANGUAGE_JA_JA = "ja-JA"
local LANGUAGE_JA = "ja"
local LANGUAGE_RU_RU = "ru-RU"
local LANGUAGE_RU = "ru"

local LANGUAGE_TABLE = {
    [LANGUAGE_ZH] = LANGUAGE_ZH_CN,
    [LANGUAGE_EN] = LANGUAGE_EN_US,
    [LANGUAGE_FR] = LANGUAGE_FR_FR,
    [LANGUAGE_JA] = LANGUAGE_JA_JA,
    [LANGUAGE_RU] = LANGUAGE_RU_RU,
}

local function get_valid_language(lang)
    local languagestr = ""
    local languageset = utils.GetLanguageSet(true)

    if lang ~= nil then
        if lang == "cn" then
            lang = LANGUAGE_ZH;
        end

        if string.find(languageset, lang) then
            languagestr = string.format('window.localStorage.setItem("locale", "%s");', LANGUAGE_TABLE[lang])
        end
    end

    return languagestr
end

local ERR_SESSION_LIMITED = 0x1000
local ERR_LOGIN_FAILED = 130
local ERR_NOT_SUPPORT_KVM = 145
local SESSION_MAXINUM = 4
local SSO_RSP_CONTEXT_TPL =
    '<meta http-equiv="X-UA-Compatible" content="IE=9">\n\
        <script>\n\
            var ssoLoginInfo = {\n\
                index:"%s",\n\
                token:"%s",\n\
                stateCode:"%d",\n\
            };\n\
        %s\n\
        window.localStorage.removeItem("ssoLoginInfo");\n\
        window.localStorage.setItem("ssoLoginInfo", JSON.stringify(ssoLoginInfo));\n\
        window.location.href="https://%s";\n\
        </script>'
local SSO_PREFIX_PARA_VALUE = "/v1/bmcPage/#/home"
local SSO_RSP_CONTEXT_PREFIX =
    '<meta http-equiv="X-UA-Compatible" content="IE=9">\n\
        <script>\n\
            var ssoLoginInfo = {\n\
                index:"%s",\n\
                token:"%s",\n\
                stateCode:"%d",\n\
            };\n\
        %s\n\
        window.localStorage.removeItem("ssoLoginInfo");\n\
        window.localStorage.setItem("ssoLoginInfo", JSON.stringify(ssoLoginInfo));\n\
        window.location.href="%s";\n\
        </script>'
local SSO_RSP_CONTEXT_KVM =
        '<meta http-equiv="X-UA-Compatible" content="IE=9">\n\
            <script>\n\
                var kvmHtml5Info = {\n\
                    redirect:"%s",\n\
                    lang:"%s",\n\
                    kvmmode:"%s",\n\
                    openway:"%s",\n\
                };\n\
            %s\n\
            window.sessionStorage.removeItem("kvmHtml5Info");\n\
            window.sessionStorage.setItem("kvmHtml5Info", JSON.stringify(kvmHtml5Info));\n\
            window.location.href=\"https://%s/#/kvm_h5\";\n\
            </script>'

function M.SSOAccess(data, clientip, hostname, old_session_id, header_token)
    if old_session_id and (#old_session_id > 1024 or #old_session_id == 0) and not string.match(old_session_id, "^[a-zA-Z0-9]+$") then
        return http.reply_bad_request("Invalid param")
    end
    if header_token and header_token ~= '' and (#header_token > 1024 or not string.match(header_token, "^[a-zA-Z0-9]+$")) then
        return http.reply_bad_request("Invalid param")
    end

    local result = 0
    local token = ''
    local header = {
        ['content-type'] = "text/html; charset=UTF-8"
    }
    local session = ''
    local languagestr = get_valid_language(data.lang)
    local user_info = {
        callerInfo = {gstring('Redfish-WebUI'), gstring(''), gstring(clientip)}
    }

    -- url中的token值为header，则表明请求头的cookie中携带着真实token,然后利用此token去进行后续的鉴权
    if data.redirect == 'console' and data.openway == 'html5' and data.token == 'header' and header_token then
        data.token = header_token
    -- url中的token值不为header，则表明url中携带着真实token,将url中的token放在Cookie中，url中的其他参数放在html文件中，返回给前端实现页面跳转
    elseif data.redirect == 'console' and data.openway == 'html5' and data.token ~= 'header' then
        header['Set-Cookie'] = string.format('token=%s; Path=/; Secure; Httponly; SameSite=Strict', data.token)
        local response = string.format(SSO_RSP_CONTEXT_KVM, data.redirect, data.lang, data.kvmmode, data.openway, languagestr, hostname)
        return http.reply_ok(response, header)
    end

    if data.redirect == 'index' then  -- sso跳转到首页
        local input_list = {gstring(data.token), gstring(clientip)}
        local ok, resp =
            utils.call_method(
            user_info,
            O.Session,
            'AddSSOSessionID',
            function(ret)
                result = ret
                return true
            end,
            input_list
        )

        if ok then
            session = core.compute_checksum(defs.G_CHECKSUM_SHA256, resp[1]):sub(1, 16)
            token = tools.get_token_by_session(session)
            --删除session
            if old_session_id then
                utils.DeleteOldSession(clientip, old_session_id)
            end
            header['Set-Cookie'] = string.format('SessionId=%s; Path=/; Secure; Httponly; SameSite=Strict', resp[1])
        else
            if result == SESSION_MAXINUM then
                result = ERR_SESSION_LIMITED
            else
                result = ERR_LOGIN_FAILED
            end
        end

        if data.prefix == SSO_PREFIX_PARA_VALUE then
            return http.reply_ok(string.format(SSO_RSP_CONTEXT_PREFIX, session, token, result, languagestr,
                SSO_PREFIX_PARA_VALUE), header)
        end

        return http.reply_ok(string.format(SSO_RSP_CONTEXT_TPL, session, token, result, languagestr, hostname),
                            header)
    elseif data.redirect == 'console' then      -- sso 跳转到kvm
        if O.PMEServiceConfig.KVMEnable:fetch() == 0 then
            return http.reply_ok(string.format(SSO_RSP_CONTEXT_TPL, '', '', ERR_NOT_SUPPORT_KVM, '', hostname),
            header)
        end
        local mode = data.kvmmode == 'dedicate' and 1 or 0
        local input_list = {gstring(data.token), gstring(clientip), gbyte(mode)}
        local ok, resp =
            utils.call_method(
            user_info,
            O.Kvm0,
            'SS0GetIdentifier',
            function(ret)
                result = ret
                return true
            end,
            input_list
        )

        if ok then
            local response
            local auth_info = {
                code_key = resp[1],
                salt_str = resp[2],
                privilege = resp[3],
                code_key_ext = resp[4],
            }
            header, response = kvmaccess.OpenKVMBySSO(hostname, data, auth_info)
            -- 利用token鉴权成功后，将Cookie中token置空
            header['Set-Cookie'] = "token=; Path=/; Secure; Httponly; SameSite=Strict"
            return http.reply_ok(response, header)
        end

        return http.reply_ok(string.format(SSO_RSP_CONTEXT_TPL, '', '', result, languagestr, hostname), header)
    end
end

return M
