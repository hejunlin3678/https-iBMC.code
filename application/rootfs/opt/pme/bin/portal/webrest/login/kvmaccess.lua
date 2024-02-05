local http = require 'http'
local defs = require 'define'
local cjson = require 'cjson'
local dflib = require 'dflib'
local utils = require 'utils'
local core = require 'dflib.core'
local tasks = require "tasks"

local O = dflib.object
local gbyte = core.GVariant.new_byte
local gint32 = core.GVariant.new_int32
local gstring = core.GVariant.new_string
local get_system_name = utils.get_system_name
local reply_not_found = http.reply_not_found
local json_encode = cjson.encode
local reply_bad_request = http.reply_bad_request
local reply_method_not_implemented = http.reply_method_not_implemented

local g_kvm_session = {}
local g_kvm_session_count = 0

local KVM_SESSSION_TIMEOUT = 15
local KVM_SESSSION_MAXNUM = 500

local KEY_CODE_LENGH = 8
local KEY_SLAT_LENGH = 64
local KEY_EXIT_LENGH = 64

local OPEN_WAY_JAVA = 0
local OPEN_WAY_HTML5 = 1

local SUPPORTED_BROWSER = {
    {"firefox/(%d+%.?%d*)", 26}, {"chrome/(%d+%.?%d*)", 21}, {"version/(%d+%.?%d*)", 10.0}, {"msie%s(%d+%.?%d*)", 5.0},
    {"trident/(%d+%.?%d*)", 8}
}

local DEFAULT_HEADER = {['content-type'] = "text/html; charset=UTF-8"}
local DEFAULT_JSON_HEADER = {['content-type'] = "application/json; charset=UTF-8"}

local KEY_DIRECT_DRSP_CONTEXT_KVM =
        '<meta http-equiv="X-UA-Compatible" content="IE=9">\n\
            <script>\n\
                var kvmHtml5Info = {\n\
                    openWay:"%s",\n\
                    lp:"%s",\n\
                    lang:"%s",\n\
                };\n\
            window.sessionStorage.removeItem("kvmHtml5Info");\n\
            window.sessionStorage.setItem("kvmHtml5Info", JSON.stringify(kvmHtml5Info));\n\
            window.location.href=\"https://%s/#/kvm_h5\";\n\
            </script>'

local KVM_OUT_FORMAT_JNLP = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
    <jnlp spec=\"1.0+\" codebase=\"https://%s\">\n\
    <information>\n\
    <title>Remote Virtual Console   IP : %s   SN : %s</title>\n\
    <vendor>iBMC</vendor>\n\
    </information>\n\
    <resources><j2se version=\"1.7+\" /><jar href=\"/bmc/pages/jar/%s?authParam=%d\" main=\"true\"/></resources>\n\
    <applet-desc name=\"Remote Virtual Console   IP : %s   SN : %s\" main-class=\"com.kvm.KVMApplet\" \
    width=\"950\" height=\"700\" >\n\
    <param name=\"verifyValue\" value=\"%d\"/>  \n\
    <param name=\"mmVerifyValue\" value=\"%d\"/>\n\
    <param name=\"decrykey\" value=\"%s\"/> \n\
    <param name=\"local\" value=\"%s\"/>\n\
    <param name=\"compress\" value=\"%d\"/> \n\
    <param name=\"vmm_compress\" value=\"%d\"/>\n\
    <param name=\"port\" value=\"%d\"/>\n\
    <param name=\"vmmPort\" value=\"%d\"/>  \n\
    <param name=\"privilege\" value=\"%d\"/>\n\
    <param name=\"bladesize\" value=\"1\"/> \n\
    <param name=\"IPA\" value=\"%s\"/>\n\
    <param name=\"IPB\" value=\"%s\"/>\n\
    <param name=\"verifyValueExt\" value=\"%s\"/>   \n\
    <param name=\"title\" value=\"%s\"/>\n\
    </applet-desc>\n\
    <security>\n\
    <all-permissions/>\n\
    </security>\n\
    </jnlp>"

local KVM_OUT_FORMAT_CLIENT_APP =
    "{\"DirectKVM\":[%d],\"VerifyValue\":[%s],\"Decrykey\":[\"%s\"],\"Privilege\":[%s],\"Compress\":[%d],\"VmmCompress\":[%d],\"KvmPort\":[%d],\"VmmPort\":[%d],\"SerialNumber\":<%s>,\"VerifyValueExt\":<%s>,\"LockTime\":(%d)}"

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
    [LANGUAGE_RU] = LANGUAGE_RU_RU
}

local M = {}

local function set_kvm_session(code_key, code_key_ext, salt_str, privilege)
    if g_kvm_session_count > KVM_SESSSION_MAXNUM then
        return
    end

    local sessionid = core.generate_kvm_sessionid()
    local hash = core.compute_checksum(defs.G_CHECKSUM_SHA256, sessionid)
    local content = string.format(
        "{\"code_key\":%d, \"privilege\": %d, \"salt_str\":\"%s\", \"code_key_ext\":\"%s\"}", code_key,
        privilege, salt_str, code_key_ext)
    if not g_kvm_session[hash] then
        g_kvm_session_count = g_kvm_session_count + 1
        tasks.fork(function()
            local t = os.time()
            while os.time() - t < KVM_SESSSION_TIMEOUT do
                tasks.sleep(1000)
            end

            if g_kvm_session[hash] then
                g_kvm_session_count = g_kvm_session_count - 1
                g_kvm_session[hash] = nil
            end
        end)
    end
    g_kvm_session[hash] = content
    return sessionid
end

local function get_kvm_session(sessionid)
    local session_hash = core.compute_checksum(defs.G_CHECKSUM_SHA256, sessionid)

    if g_kvm_session[session_hash] ~= nil then
        local content = g_kvm_session[session_hash]
        g_kvm_session[session_hash] = nil
        g_kvm_session_count = g_kvm_session_count - 1
        return true, cjson.decode(content)
    end

    return false, nil
end

local function is_support_kvm(open_way, user_agent)
    local KVMEnable = O.PMEServiceConfig.KVMEnable:next(utils.ValueToBool):fetch_or(false)
    if not KVMEnable then
        return false, DEFAULT_HEADER, "<h2>BMC does not support kvm. Please use a later version.</h2>"
    end

    if open_way == OPEN_WAY_JAVA then
        if O.PMEServiceConfig.JavaKVMEnable:fetch() == 0 then
            return false, DEFAULT_HEADER, "<h2>BMC does not support kvm. Please use a later version.</h2>"
        end
        return true, nil, nil
    end

    local user_agent_lower = string.lower(user_agent)
    for _, v in pairs(SUPPORTED_BROWSER) do
        local version = tonumber(string.match(user_agent_lower, v[1]))
        if version ~= nil and tonumber(version) <= v[2] then
            return false, DEFAULT_HEADER,
                "<h2>Your browser does not support HTML5. Please use a browser of a later version.</h2>"
        end
    end
    return true, nil, nil
end

local function open_kvm(req_info, auth_info)
    local header
    local response
    if req_info.open_way == OPEN_WAY_HTML5 then
        header = DEFAULT_JSON_HEADER
        response = json_encode({
            kvmHtml5Info = {
                SN = O.BMC.DeviceSerialNumber:fetch(),
                verifyValue = string.format("%s", auth_info.code_key),
                mmVerifyValue = string.format("%s", auth_info.code_key),
                decrykey = auth_info.salt_str,
                ['local'] = req_info.language,
                compress = O.Kvm0.EncryptState:fetch(),
                vmm_compress = O.Vmm0.EncryptState:fetch(),
                port = string.format("%s", O.Kvm0.Port:fetch()),
                vmmPort = string.format("%s", O.Vmm0.Port:fetch()),
                privilege = string.format("%s", auth_info.privilege),
                bladesize = "1",
                IPA = req_info.server_name,
                IPB = req_info.server_name,
                verifyValueExt = auth_info.code_key_ext,
                languagestr = req_info.languagestr
            },
            hrefUrl = string.format("https://%s/#/kvm_h5", req_info.hostip)
        })
    else
        local title = string.format("   IP : %s   SN : %s", req_info.server_name, O.BMC.DeviceSerialNumber:fetch())
        local ok, jarname = pcall(function()
            for _, f in ipairs(core.file_list("/opt/pme/web/htdocs/bmc/pages/jar/", true)) do
                if f:match("vconsole[0-9a-zA-Z.]*") then
                    return f
                end
            end
            return ''
        end)
        if not ok then
            jarname = ''
        end

        header = {
            ['content-type'] = "application/x-java-jnlp-file",
            ['Content-Disposition'] = "filename = kvm.jnlp"
        }
        response = string.format(KVM_OUT_FORMAT_JNLP, req_info.hostip, req_info.server_name,
            O.BMC.DeviceSerialNumber:fetch(), jarname, auth_info.code_key, req_info.server_name,
            O.BMC.DeviceSerialNumber:fetch(), auth_info.code_key, auth_info.code_key, auth_info.salt_str,
            req_info.language, O.Kvm0.EncryptState:fetch(), O.Vmm0.EncryptState:fetch(), O.Kvm0.Port:fetch(),
            O.Vmm0.Port:fetch(), auth_info.privilege, req_info.server_name, req_info.server_name,
            auth_info.code_key_ext, title)
    end
    return header, response
end

local function access_kvm_by_url(auth_param, req_info)
    local auth_info = {code_key = "", salt_str = "", code_key_ext = "", privilege = 3}
    if O.Kvm0.EncryptState:fetch() == 1 then
        if string.len(auth_param) == (KEY_CODE_LENGH + KEY_SLAT_LENGH) then
            auth_info.code_key = tonumber("0x" .. string.sub(auth_param, 1, KEY_CODE_LENGH))
            auth_info.salt_str = string.sub(auth_param, KEY_CODE_LENGH + 1, KEY_CODE_LENGH + KEY_SLAT_LENGH)
        elseif string.len(auth_param) == (KEY_CODE_LENGH + KEY_SLAT_LENGH + KEY_EXIT_LENGH) then
            auth_info.code_key = tonumber("0x" .. string.sub(auth_param, 1, KEY_CODE_LENGH))
            auth_info.salt_str = string.sub(auth_param, KEY_CODE_LENGH + 1, KEY_CODE_LENGH + KEY_SLAT_LENGH)
        else
            return DEFAULT_HEADER, "<h2>Invalid url.</h2>"
        end
    else
        auth_info.code_key = tonumber("0x" .. string.sub(auth_param, 1, 8))
    end

    if (tonumber("0x" .. string.sub(auth_param, 1, 1)) >= 8) then
        auth_info.code_key = auth_info.code_key - 0x80000000 - 0x80000000
    end

    return open_kvm(req_info, auth_info)
end

local function access_kvm_by_session(sessionid, req_info)
    local ok, auth_info = get_kvm_session(sessionid)

    if ok then
        local header, response = open_kvm(req_info, auth_info)
        header['Set-Cookie'] = 'KvmSession=""; Path=/; Secure; Httponly; SameSite=Strict'
        return header, response
    end
    return DEFAULT_HEADER, "<h2>Invalid url.</h2>"
end

local function get_valid_language(uri_data)
    local language = LANGUAGE_EN
    local languagestr = ""
    local languageset = utils.GetLanguageSet(true)

    if uri_data.lp ~= nil then
        if uri_data.lp == "cn" then
            uri_data.lp = LANGUAGE_ZH;
        end

        if string.find(languageset, uri_data.lp) then
            language = uri_data.lp
            languagestr =
                string.format('window.localStorage.setItem("locale", "%s");', LANGUAGE_TABLE[uri_data.lp])
        end
    end

    if uri_data.lang ~= nil then
        if uri_data.lang == "cn" then
            uri_data.lang = LANGUAGE_ZH;
        end

        if string.find(languageset, uri_data.lang) then
            language = uri_data.lang
            languagestr = string.format('window.localStorage.setItem("locale", "%s");',
                LANGUAGE_TABLE[uri_data.lang])
        end
    end

    return language, languagestr
end

function M.AccessKVM(uri_data, user_agent, sessionid, authParam, hostname)
    local old_authParam
    local sys_name = get_system_name()
    if sys_name == 'RMM' then
        return reply_not_found()
    end

    if user_agent and (#user_agent > 1024 or #user_agent == 0) then
        return reply_bad_request("Invalid param")
    end

    if sessionid and (#sessionid > 1024 or #sessionid == 0) and not sessionid:match('^[a-zA-Z0-9]+$') then
        return reply_bad_request("Invalid param")
    end

    if authParam and authParam ~= '' and (#authParam > 1024 or not string.match(authParam, "^[a-zA-Z0-9]+$")) then
        return http.reply_bad_request("Invalid param")
    end

    local server_ip, _ = hostname:match("^(.*):([0-9]+)$")
    if not server_ip then
        server_ip = hostname
    end

    local req_info = {
        hostip = hostname,
        server_name = server_ip,
        open_way = OPEN_WAY_JAVA,
        language = "",
        languagestr = ""
    }

    -- 获取打开方式
    if uri_data.openWay == "html5" or uri_data.openway == "html5" then
        req_info.open_way = OPEN_WAY_HTML5
    else
        req_info.open_way = OPEN_WAY_JAVA
    end

    -- 判断是否支持KVM
    local ok, header, response = is_support_kvm(req_info.open_way, user_agent or '')
    if ok == false then
        return http.reply_ok(response, header)
    end

    -- 获取语言信息
    req_info.language, req_info.languagestr = get_valid_language(uri_data)

    -- 两种访问KVM的方式
    if uri_data.authParam ~= nil then
        if uri_data.authParam:match("[\"'(){};\\]") then
            return reply_bad_request("Invalid param")
        end
        if uri_data.jumpflag == 'notjump' then
            header, response = access_kvm_by_url(uri_data.authParam, req_info)  -- 对应Key直连登录方式，url为/remoteconsole
            return http.reply_ok(response, header)
        end

        if uri_data.authParam ~= 'header' then
            header = {
                ['content-type'] = "text/html; charset=UTF-8"
            }
            header['Set-Cookie'] = string.format('authParam=%s; Path=/; Secure; Httponly; SameSite=Strict', uri_data.authParam)
            response = string.format(KEY_DIRECT_DRSP_CONTEXT_KVM, uri_data.openWay or uri_data.openway or '', uri_data.lp or '', uri_data.lang or '', server_ip)
            return http.reply_ok(response, header)
        elseif authParam then
            old_authParam = uri_data.authParam
            uri_data.authParam = authParam
        end
        header, response = access_kvm_by_url(uri_data.authParam, req_info)  -- 对应Key直连登录方式，url为/UI/Rest/KVMHandler

        if old_authParam == 'header' then
            -- 获取authParam后，将Cookie中authParam置空
            header['Set-Cookie'] = "authParam=; Path=/; Secure; Httponly; SameSite=Strict"
        end
    else
        if sessionid == nil then
            return reply_bad_request("Invalid param")
        end

        header, response = access_kvm_by_session(sessionid, req_info)  -- 对应url直连登录方式
    end

    return http.reply_ok(response, header)
end

function M.AuthKVM(client_ip, req_data)
    local sys_name = get_system_name()
    if sys_name == 'RMM' then
        return reply_not_found()
    end
    local result = 0
    local header
    local response
    local sessionid

    local KVMEnable = O.PMEServiceConfig.KVMEnable:next(utils.ValueToBool):fetch_or(false)
    if not KVMEnable then
        return reply_method_not_implemented()
    end

    local kvm_mode = req_data.IsKvmApp == 0 and 1 or req_data.KvmMode
    local user_info = {callerInfo = {gstring('WEB'), gstring(req_data.user_name), gstring(client_ip)}}
    local input_list = {
        gstring(req_data.user_name), gstring(req_data.check_pwd), gbyte(req_data.logtype), gstring(client_ip),
        gint32(kvm_mode)
    }
    local ok, resp = utils.call_method(user_info, O.Kvm0, 'GetIdentifier', function(ret)
        result = ret
        return true
    end, input_list)
    if ok then
        local code_key = resp[1]
        local salt_str = resp[2]
        local privilege = resp[3]
        local code_key_ext = resp[4]
        if req_data.IsKvmApp == 1 then
            -- 独立KVM认证
            response = string.format(KVM_OUT_FORMAT_CLIENT_APP, 0, code_key, salt_str, privilege,
                O.Kvm0.EncryptState:fetch(), O.Vmm0.EncryptState:fetch(), O.Kvm0.Port:fetch(), O.Vmm0.Port:fetch(),
                O.BMC.DeviceSerialNumber:fetch(), code_key_ext, O.SecurityEnhance.AuthFailLockTime:fetch())
        else
            -- 直连KVM认证
            sessionid = set_kvm_session(code_key, code_key_ext, salt_str, privilege) or ""
            response = "{\"DirectKVM\":[0]}"
            header = {['Set-Cookie'] = string.format('KvmSession=%s; Path=/; Secure; Httponly; SameSite=Strict', sessionid)}
        end
    else
        if req_data.IsKvmApp == 1 then
            response = string.format("{\"DirectKVM\":[%d],\"LockTime\":(%d)}", result,
                O.SecurityEnhance.AuthFailLockTime:fetch())
        else
            response = string.format("{\"DirectKVM\":[%d]}", result)
        end
    end

    return http.reply_ok(response, header)
end

function M.OpenKVMBySSO(hostname, uri_data, auth_info)
    local server_ip, _ = hostname:match("^(.*):([0-9]+)$")
    if not server_ip then
        server_ip = hostname
    end

    local req_info = {
        hostip = hostname,
        server_name = server_ip,
        open_way = OPEN_WAY_JAVA,
        language = "",
        languagestr = ""
    }

    -- 获取打开方式
    if uri_data.openWay ~= nil and uri_data.openWay == "html5" then
        req_info.open_way = OPEN_WAY_HTML5
    elseif uri_data.openway ~= nil and uri_data.openway == "html5" then
        req_info.open_way = OPEN_WAY_HTML5
    else
        req_info.open_way = OPEN_WAY_JAVA
    end

    -- 获取语言信息
    req_info.language, req_info.languagestr = get_valid_language(uri_data)

    return open_kvm(req_info, auth_info)
end

local function parse_url(url)
    local jar_path, query_pos = url:lower():match('/([^/]+)%.jar%?().+$')
    if jar_path and query_pos then
      if jar_path:match('^video') then
        return 'video', url:sub(query_pos)
      elseif jar_path:match('^vconsole') then
        return 'vconsole', url:sub(query_pos)
      end
    end

    return nil, nil
end

function M.AuthJar(url)
    local jar_type, query_str = parse_url(url)
    if not jar_type then
        return reply_bad_request('InvalidParam')
    end

    local ok = utils.call_method(nil, O.Kvm0, 'CheckKvmDownloadCode', nil, gstring(query_str), gstring(jar_type))
    if not ok then
        return reply_bad_request('CheckKvmDownloadCodefailed')
    end
    return http.reply_ok()
end

return M
