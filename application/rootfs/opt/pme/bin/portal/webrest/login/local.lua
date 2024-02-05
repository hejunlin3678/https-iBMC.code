local http = require 'http'
local defs = require 'define'
local cjson = require 'cjson'
local dflib = require 'dflib'
local utils = require 'utils'
local tools = require 'tools'
local cfg = require 'config'
local core = require 'dflib.core'
local logging = require "logging"
local C = dflib.class
local O = dflib.object
local null = cjson.null
local ENABLE = cfg.ENABLE
local DFL_OK = dflib.DFL_OK
local gbyte = core.GVariant.new_byte
local gint32 = core.GVariant.new_int32
local gstring = core.GVariant.new_string
local dal_trim_string = utils.dal_trim_string
local get_security_enhanced_compatible_board_v3 = dflib.get_security_enhanced_compatible_board_v3
local get_system_name = utils.get_system_name
local reply_not_found = http.reply_not_found
local MSG_NEED_FORCE_PASSWD_RESET =
    "This password is an initial password or has been changed by another administrator. Please reset the password for security purposes."
local MSG_NO_CONFIG_SELF_PRIV =
    "This password is an initial password or has been changed by another administrator. Need privilege to reset the password for security purposes."
local MGMT_SOFTWARE_TYPE_EM = cfg.MGMT_SOFTWARE_TYPE_EM
local ASM_STANDBY = cfg.ASM_STANDBY
local PME_SERVICE_SUPPORT = 1 -- PME支持该服务

local METHOD_TYPE_E = {
    E_METHOD_NULL = '',
    E_METHOD_GET = 'GET',
    E_METHOD_CREATE = 'CREATE',
    E_METHOD_ACTION = 'ACTION',
    E_METHOD_PATCH = 'PATCH',
    E_METHOD_PUT = 'PUT',
    E_METHOD_DELETE = 'DELETE',
    E_METHOD_POST = 'POST'
};

local G_SYS_LOCKDOWN_ALLOW_RUI = {
    '/UI/Rest/GenericInfo', -- post
    '/UI/Rest/Maintenance/DownloadOperationLog', -- post
    '/UI/Rest/Maintenance/DownloadRunLog', -- post
    '/UI/Rest/Maintenance/DownloadSecurityLog', -- post
    '/UI/Rest/Maintenance/DownloadBlackBoxOfSdi', -- post
    '/UI/Rest/Maintenance/SystemDiagnostic/DownloadBlackBox', -- post
    '/UI/Rest/Maintenance/SystemDiagnostic/DownloadSerialPortData', -- post
    '/UI/Rest/Maintenance/SystemDiagnostic/DownloadNPULog', -- post
    '/UI/Rest/Maintenance/SystemDiagnostic/DownloadVideo', -- post
    '/UI/Rest/Maintenance/SystemDiagnostic/ExportVideoStartupFile', -- post
    '/UI/Rest/Maintenance/SystemDiagnostic/CaptureScreenshot', -- post
    '/UI/Rest/Services/KVM/GenerateStartupFile', -- post
    '/UI/Rest/BMCSettings/Configuration/ExportConfig', -- post
    '/UI/Rest/BMCSettings/LicenseService/ExportLicense', -- post
    '/UI/Rest/Maintenance/TestSyslogNotification', -- post
    '/UI/Rest/Maintenance/TestEmailNotification', -- post
    '/UI/Rest/Maintenance/TestTrapNotification', -- post
    '/UI/Rest/Services/VMM/Control', -- post
    '/UI/Rest/Dump', -- post
    '/UI/Rest/KeepAlive', -- post
    '/UI/Rest/Maintenance/QueryEvent', -- post
    '/UI/Rest/Maintenance/DownloadEvent', -- post
    '/UI/Rest/System/PowerControl', -- post
    '/UI/Rest/Sessions', -- delete
    '/UI/Rest/Login', -- post
    '/UI/Rest/AccessMgnt/AdvancedSecurity' -- patch
}

local G_RM_NOT_ALLOW_URL = {
    '/UI/Rest/System/Processor',
    '/UI/Rest/System/Memory',
    '/UI/Rest/System/NetworkAdapter',
    '/UI/Rest/System/Boards/DiskBackplane',
    '/UI/Rest/System/Boards/PCIeRiserCard',
    '/UI/Rest/System/Boards/RAIDCard',
    '/UI/Rest/System/Boards/PassThroughCard',
    '/UI/Rest/System/Boards/MEZZCard',
    '/UI/Rest/System/Boards/PCIeTransformCard',
    '/UI/Rest/System/Boards/GPUBoard',
    '/UI/Rest/System/Boards/FanBackplane',
    '/UI/Rest/System/Boards/HorizontalConnectionBoard',
    '/UI/Rest/System/Boards/CPUBoard',
    '/UI/Rest/System/Boards/MemoryBoard',
    '/UI/Rest/System/Boards/M2TransformCard',
    '/UI/Rest/System/Boards/ChassisBackplane',
    '/UI/Rest/System/Boards/BBUModule',
    '/UI/Rest/System/Boards/SecurityModule',
    '/UI/Rest/System/Boards/LCD',
    '/UI/Rest/System/Boards/PeripheralFirmware',
    '/UI/Rest/System/Boards/SDCardController',
    '/UI/Rest/System/Boards/OCPCard',
    '/UI/Rest/System/Boards/PCIeCard',
    '/UI/Rest/System/Boards/CICCard',
    '/UI/Rest/System/Boards/ExpandBoard',
    '/UI/Rest/System/Boards/LeakageDetectionCard',
    '/UI/Rest/System/ProcesserUsage',
    '/UI/Rest/System/MemoryUsage',
    '/UI/Rest/System/DiskUsage',
    '/UI/Rest/System/NetworkBandwidthUsage',
    '/UI/Rest/System/NetworkBandwidthUsage/ClearHistory',
    '/UI/Rest/System/Storage/ViewsSummary',
    '/UI/Rest/System/Power',
    '/UI/Rest/System/Power/ResetStatistics',
    '/UI/Rest/System/Power/History',
    '/UI/Rest/System/PowerControl',
    '/UI/Rest/System/Thermal/ClearInletHistory',
    '/UI/Rest/System/BootOptions',
    '/UI/Rest/System/CPUAdjust',
    '/UI/Rest/System/FusionPartition',
    '/UI/Rest/Maintenance/FDM',
    '/UI/Rest/Maintenance/SystemDiagnostic',
    '/UI/Rest/Maintenance/SystemDiagnostic/DownloadBlackBox',
    '/UI/Rest/Maintenance/SystemDiagnostic/DownloadSerialPortData',
    '/UI/Rest/Maintenance/SystemDiagnostic/DownloadVideo',
    '/UI/Rest/Maintenance/SystemDiagnostic/ExportVideoStartupFile',
    '/UI/Rest/Maintenance/SystemDiagnostic/StopVideo',
    '/UI/Rest/Maintenance/SystemDiagnostic/CaptureScreenshot',
    '/UI/Rest/Maintenance/SystemDiagnostic/DeleteScreenshot',
    '/UI/Rest/Maintenance/SystemDiagnostic/DownloadNPULog',
    '/UI/Rest/Maintenance/WorkRecord',
    '/UI/Rest/AccessMgnt/TwoFactorAuthentication',
    '/UI/Rest/AccessMgnt/TwoFactorAuthentication/ImportClientCertificate',
    '/UI/Rest/AccessMgnt/TwoFactorAuthentication/ImportRootCertificate',
    '/UI/Rest/AccessMgnt/TwoFactorAuthentication/DeleteCertificate',
    '/UI/Rest/AccessMgnt/TwoFactorAuthentication/ImportCrlCertificate',
    '/UI/Rest/AccessMgnt/BootCertificates',
    '/UI/Rest/AccessMgnt/BootCertificates/ImportBootCertActionInfo',
    '/UI/Rest/AccessMgnt/BootCertificates/ImportBootCrlActionInfo',
    '/UI/Rest/AccessMgnt/BootCertificates/ImportSecureBootCertActionInfo',
    '/UI/Rest/AccessMgnt/BootCertificates/SecureBoot',
    '/UI/Rest/Services/KVM',
    '/UI/Rest/Services/KVM/GenerateStartupFile',
    '/UI/Rest/Services/VMM',
    '/UI/Rest/Services/VMM/Control',
    '/UI/Rest/Services/VNC',
    '/UI/Rest/Services/VNC/Password',
    '/UI/Rest/BMCSettings/LicenseService',
    '/UI/Rest/BMCSettings/LicenseService/DeleteLicense',
    '/UI/Rest/BMCSettings/LicenseService/DisableLicense',
    '/UI/Rest/BMCSettings/LicenseService/ExportLicense',
    '/UI/Rest/BMCSettings/LicenseService/InstallLicense',
    '/UI/Rest/BMCSettings/iBMA',
    '/UI/Rest/BMCSettings/iBMA/USBStickControl',
    '/UI/Rest/BMCSettings/SP',
    '/UI/Rest/BMCSettings/USBMgmtService'
}

local G_BMC_NOT_ALLOW_URL = {
    '/UI/Rest/System/BatteryInfo'
}

-- 检查首次登陆状态下的操作是否在白名单范围内
local function check_first_login_allow(method, url, session_hash, user_id)
    local session_url = string.format("%s/%s", '/UI/Rest/Sessions', session_hash)
    local account_url = string.format("%s/%d", '/UI/Rest/AccessMgnt/Accounts', user_id)
    if method == 'DELETE' and url == session_url then
        return true
    elseif method == 'PATCH' and url == account_url then
        return true
    elseif method == 'GET' and url == '/UI/Rest/AccessMgnt/AdvancedSecurity' then
        return true
    elseif method == 'GET' and url == '/UI/Rest/GenericInfo' then
        return true
    end

    return false
end

local function getRoleIdStr(roleIdStr)
    local ret = {}
    for v in roleIdStr:gmatch('(%d+)') do
        ret[#ret + 1] = utils.roleid_to_str(tonumber(v))
    end
    return ret
end

-- 检查是否系统锁定状态
local function check_system_lockdown_state()
    if C.PMEServiceConfig[0].SystemLockDownSupport:fetch_or() == PME_SERVICE_SUPPORT and
        O.SecurityEnhance.SystemLockDownStatus:fetch_or() == ENABLE then
        return true
    end
    return false
end

-- 检查系统锁定状态下是否通过白名单校验
local function check_system_lockdown_allow(method, url)
    if method == 'GET' or not check_system_lockdown_state() then
        return true
    end

    local url_lower = string.lower(url)
    local url_lower_sub17 = string.lower(url:sub(1, 17))
    for k, v in ipairs(G_SYS_LOCKDOWN_ALLOW_RUI) do
        if string.lower(v) == url_lower or (method == 'DELETE' and url_lower_sub17 == '/ui/rest/sessions') then
            return true
        end
    end

    return false
end

-- 检查用户是否为首次登陆需要强制修改密码
local function is_user_force_passwd_reset(initial_state, force_passwd_reset)
    return initial_state and force_passwd_reset
end

local function get_logtype(type, domain)
    local logtype = {}
    local ldap_enable = O.LDAPCommon.Enable:fetch()
    local kbrs_enable = O.KRBCommon.Enable:fetch()

    if type == 'Local' and (domain == 'LocaliBMC' or domain == 'LocaliRM') then
        logtype[#logtype + 1] = defs.LOG_TYPE_LOCAL
    elseif type == 'Kerberos' then
        if kbrs_enable == 1 then
            logtype[#logtype + 1] = defs.LOG_TYPE_KRB_SERVER
        end
    elseif type == 'KerberosSSO' and domain == 'KerberosSSO' then
        if kbrs_enable == 1 then
            logtype[#logtype + 1] = defs.LOG_TYPE_LDAP_KRB_SSO
        end
    elseif type == 'Auto'and domain == 'AutomaticMatching' then
        logtype[#logtype + 1] = defs.LOG_TYPE_LOCAL
        if kbrs_enable == 1 then
            logtype[#logtype + 1] = defs.LOG_TYPE_KRB_SERVER
        end
        if ldap_enable == 1 then
            logtype[#logtype + 1] = defs.LOG_TYPE_LDAP_AUTO_MATCH
        end
    elseif type == 'Ldap' then
        if ldap_enable == 1 then
            C.LDAP():fold(function(ldap_obj)
                if ldap_obj.UserDomain:fetch() == domain then
                    logtype[#logtype + 1] = ldap_obj.ID:fetch() + 1
                end
            end)
        end
    end

    return logtype
end

local function get_user_info(username, logtype, auth_rsp)
    local info = {}
    info.uid = auth_rsp[3]
    info.last_login_time = ''
    info.last_login_ip = ''
    info.channel = defs.REDFISH_WEBUI
    info.pwd_valid_days = null
    info.insecure_prompt_enabled = 'Off'

    if logtype == defs.LOG_TYPE_LOCAL then
        info.auth_type = auth_rsp[4]
        info.privilege = auth_rsp[5]
        info.role_id = auth_rsp[6]
        info.username = username
        C.User('UserId', info.uid):next(function(user_obj)
            info.last_login_time = user_obj.LastLoginTime:next(core.get_datetime):next(
                function(time)
                    return (time:match('^1969') or time:match('^1970')) and null or time
                end):fetch_or(null)
            info.last_login_ip = user_obj.LastLoginIP:neq('N/A'):fetch_or(null)
            info.pwd_valid_days = user_obj.PwdValidDays:neq(0xffff):fetch_or(null)
            info.initial_state = user_obj.InitialState:fetch() == 1
            info.force_passwd_reset = user_obj.FirstLoginPolicy:fetch() == 2
            if O.SecurityEnhance.InitialPwdPrompt:fetch() == 1 then
                info.insecure_prompt_enabled = user_obj.InitialState:fetch() == 1 and 'On' or 'Off'
            else
                info.insecure_prompt_enabled = 'Off'
            end
        end):fetch()
        return info
    end

    info.auth_type = (logtype == defs.LOG_TYPE_LDAP_AUTO_MATCH) and auth_rsp[6] or logtype
    info.privilege = auth_rsp[9]
    info.role_id = auth_rsp[4]
    local flag = auth_rsp[7]
    local domain = auth_rsp[8]
    if flag == defs.USER_NAME_NOT_CONTAIN_DOMAIN then
        info.username = string.format('%s@%s', username, domain)
    elseif flag == defs.USER_NAME_NEED_POST_BACK then
        info.username = domain
    else
        info.username = username
    end

    return info

end

local function bmc_login(data, client_ip, logtype, old_session_id)
    local password = gstring(data.Password)
    local ipaddr = gstring(client_ip)
    local result = -1
    local user_nam

    for k, v in ipairs(logtype) do
        -- 用户认证
        if v == defs.LOG_TYPE_KRB_SERVER then
            local i = data.UserName:find('%(KRB%)')
            user_nam = (i and data.UserName:sub(1, i - 1) or data.UserName)
        else
            user_nam = data.UserName
        end
        local login_type = gbyte(v)
        local input_list = {gstring(user_nam), password, login_type, ipaddr}
        local auth_rsp = O.User1:call('AuthUser', {gstring('WEB'), gstring(data.UserName), ipaddr}, input_list)
        result = auth_rsp[1]
        if result == DFL_OK then
            local info = get_user_info(user_nam, v, auth_rsp)
            return C.Session[0]:next(function(session_obj)
                -- 添加会话
                local add_session_rsp = session_obj:call('AddSession', {
                    gstring('Redfish-WebUI'), gstring(info.username), ipaddr
                }, {
                    gbyte(info.auth_type), gbyte(info.privilege), ipaddr, gstring(info.username), gbyte(info.uid),
                    gbyte(info.channel), gstring(info.role_id)
                })
                local session_id = add_session_rsp[1]
                if not session_id or #session_id == 0 then
                    return http.reply_bad_request('SessionLimitExceeded')
                end
                -- 获取会话信息
                local session_hash = core.compute_checksum(defs.G_CHECKSUM_SHA256, session_id):sub(1, 16)
                local session_rsp = session_obj:call('GetOnlineUser',
                    {gstring('Redfish'), gint32(defs.REFRESH_ENABLE)}, {gstring(session_hash)})
                local session = cjson.decode(session_rsp[1])
                local csrf_token = tools.get_token_by_session(session_hash)
                local response = {
                    XCSRFToken = csrf_token,
                    LastLoginTime = info.last_login_time,
                    LastLoginIP = info.last_login_ip,
                    DaysBeforeExpiration = info.pwd_valid_days,
                    InsecurePromptEnabled = info.insecure_prompt_enabled,
                    Session = {
                        SessionID = session_hash,
                        UserID = session.local_userid,
                        UserName = info.username,
                        LoginTime = core.get_datetime(session.logintime),
                        IP = session.loginipaddress,
                        Role = getRoleIdStr(session.user_roleid)
                    }
                }

                if old_session_id then
                    utils.DeleteOldSession(client_ip, old_session_id)
                end
                local header = {
                    ['Token'] = csrf_token,
                    ['Set-Cookie'] = string.format('SessionId=%s; Path=/; Secure; Httponly; SameSite=Strict', session_id)
                }
                -- 判断用户是否为首次登陆需要重置密码，是则返回最小信息并附加首次登陆的错误提示消息
                if get_security_enhanced_compatible_board_v3() == 0 then
                    if is_user_force_passwd_reset(info.initial_state, info.force_passwd_reset) then
                        local configure_self_priv = C.UserRole('RoleId', tonumber(session.user_roleid)).ConfigureSelf:fetch_or(0)
                        if configure_self_priv == 0 then
                            logging:error('User has no configure self privilege')
                            utils.DeleteOldSession(client_ip, session_id)
                            return http.reply_bad_request('InsufficientPrivilege', MSG_NO_CONFIG_SELF_PRIV)
                        end
                        local error_msg_first_login = {
                            code = 'PasswordNeedReset',
                            message = MSG_NEED_FORCE_PASSWD_RESET
                        }
                        local simple_resp = {
                            XCSRFToken = csrf_token,
                            Session = {
                                SessionID = session_hash,
                                UserID = session.local_userid
                            },
                            error = {
                                error_msg_first_login
                            }
                        }
                        logging:info('User need reset password')
                        return http.reply_ok_encode_json(simple_resp, header)
                    end
                end
                return http.reply_ok_encode_json(response, header)
            end):fetch()
        else
            if data.Type ~= 'Auto' or k == #logtype then
                local user = {}
                user.UserName = data.Type == 'KerberosSSO' and 'SSO user(KRB)' or data.UserName
                user.ClientIp = client_ip
                utils.OperateLog(user, '%s(%s) login failed', user.UserName, client_ip)
            end
        end
    end

    return http.reply_unauthorized(defs.AUTH_RET_MAP[result] or 'LoginFailed',
        'Authorization failed because the user name or password is incorrect, or your account is locked.')
end

local function GetLoginFQDN()
    local host_name = O.BMC.HostName:next(dal_trim_string):fetch_or(null)
    local domain_name = O.DNSSetting.DomainName:next(dal_trim_string):fetch_or(null)
    if host_name == null or domain_name == null then
        return null
    end
    if host_name == '' then
        return domain_name
    elseif domain_name == '' then
        return host_name
    end
    return host_name .. '.' .. domain_name
end

local function GetLoginKerberosDomain()
    if O.KRBCommon.Enable:fetch_or(0) ~= 1 then
        return nil
    end
    if O.KRBServer.UserDomain:fetch_or('') == '' then
        return ''
    end
    return O.KRBServer.UserDomain:fetch_or('') .. '(KRB)'
end

local function get_software_name()
    local system_name = O.PRODUCT.ProductUniqueID:fetch_or(null)
    -- RM210的产品唯一识别ID
    if system_name == 34041088 or system_name == 34040832 then
        return 'iRM'
    else
        return O.PRODUCT.MgmtSoftWareType:fetch() == MGMT_SOFTWARE_TYPE_EM and 'EMM' or
            O.BMC.SoftwareName:fetch_or(nil)
    end
end

local M = {}

function M.GetLogin()
    local accountLockoutDuration = 0
    local AuthFailMaxExtension = O.SecurityEnhance.AuthFailMaxExtension:fetch()
    if AuthFailMaxExtension == 0 then
        accountLockoutDuration = 0
    else
        accountLockoutDuration = O.SecurityEnhance.AuthFailLockTimeExtension:fetch_or('')
    end

    return {
        FQDN = GetLoginFQDN(),
        Copyright = O.Contact.Copyright:fetch_or(''),
        LDAPDomain = O.LDAPCommon.Enable:eq(1):next(function()
            local domains = {}
            C.LDAP():fold(function(obj, acc)
                if dflib.object_name(obj) ~= 'KRBServer' then
                    acc[#acc + 1] = obj.UserDomain:neq(''):fetch_or(nil)
                end
                return acc
            end, domains)
            return domains
        end):fetch_or(nil),
        QRCodeState = O.Contact.DocSupportFlag:fetch() == 1 and 'Show' or 'Hide',
        ProductName = O.PRODUCT.MgmtSoftWareType:fetch() == MGMT_SOFTWARE_TYPE_EM and O.PRODUCT.ProductName:fetch_or('') or 
            O.BMC.SystemName:fetch_or(''),
        LanguageSet = utils.split(utils.GetLanguageSet(true), ','),
        SecurityBanner = O.SecurityEnhance.BannerState:fetch() == 1 and O.SecurityEnhance.BannerContent:fetch() or
            nil,
        KerberosDomain = GetLoginKerberosDomain(),
        AccountLockoutDuration = accountLockoutDuration,
        SoftwareName = get_software_name(),
        SmsName = C.SMS[0].SmsName:fetch_or(nil)
    }
end

function M.Login(data, client_ip, session_id)
    if O.PRODUCT.MgmtSoftWareType:fetch() == MGMT_SOFTWARE_TYPE_EM and O.AsmObject.AsStatus:fetch() == ASM_STANDBY then
        return http.reply_bad_request('NotAllowedOnStandbyMM',
            'This is the standby management module. Please perform this operation on the active management module.')
    end
    if session_id and (#session_id > 1024 or #session_id == 0) and not string.match(session_id, "^[a-zA-Z0-9]+$") then
        return http.reply_bad_request("Invalid param")
    end

    local logtype = get_logtype(data.Type, data.Domain)

    if next(logtype) ~= nil then
        return bmc_login(data, client_ip or "127.0.0.1", logtype, session_id)
    else
        return http.reply_bad_request('InvalidField', 'Ensure Domain is valid.')
    end
end

local logout_ret_map = {
    [1] = 'SessionTimeout', -- 会话超时
    [2] = 'SessionKickout', -- 会话被踢出
    [3] = 'SessionRelogin', -- 会话异地登录
    [4] = 'SessionChanged' -- 用户名、权限、密码、使能状态信息被修改时,被自动踢出
}

function M.AuthUser(method, url, session_id, csrf_token)
    if url and #url > 1024 then
        return http.reply_bad_request("Invalid param")
    end

    -- 接口校验，RM210产品不支持的接口返回404
    local sys_name = get_system_name()
    local url_lower = string.lower(url)
    if sys_name == 'RMM' then
        for k, v in ipairs(G_RM_NOT_ALLOW_URL) do
            if string.lower(v) == url_lower or string.match(url_lower, 'workrecord') ~= nil
                or string.match(url_lower, 'fdm') ~= nil or string.match(url_lower, 'storage') ~= nil
                or string.match(url_lower, 'networkadapter') then
                return reply_not_found()
            end
        end
    else
        for k, v in ipairs(G_BMC_NOT_ALLOW_URL) do
            if string.lower(v) == url_lower or string.match(url_lower, 'assetinfo') ~= nil then
                return reply_not_found()
            end
        end
    end
    if not check_system_lockdown_allow(method, url) then
        return http.reply_bad_request('SystemLockdownForbid')
    end

    if session_id and (#session_id > 1024 or #session_id == 0) and not string.match(session_id, "^[a-zA-Z0-9]+$") then
        return http.reply_bad_request("Invalid param")
    end

    if csrf_token and (#csrf_token > 1024 or #csrf_token == 0) and not string.match(csrf_token, "^[a-zA-Z0-9]+$") then
        return http.reply_bad_request("Invalid param")
    end

    return C.Session[0]:next(function(session_obj)
        local session_hash = core.compute_checksum(defs.G_CHECKSUM_SHA256, session_id):sub(1, 16)
        local caller_info = {gstring('Redfish'), gint32(defs.REFRESH_ENABLE)}
        local input_list = {gstring(session_hash)}
        local session_rsp = session_obj:call('GetOnlineUser', caller_info, input_list)
        if session_rsp[1] and #session_rsp[1] > 0 then
            local session = cjson.decode(session_rsp[1])

            -- 判断是否要执行首次登陆白名单检查
            if get_security_enhanced_compatible_board_v3() == 0 and session.authenticateType == cfg.LOG_TYPE_LOCAL then
                local user_obj = C.User('UserName', session.loginname)
                if not user_obj then
                    logging:error('Invalid user name')
                    return http.reply_error(http.status.HTTP_UNAUTHORIZED, 'NoValidSession', 'invalid session.')
                else
                    local initial_state = user_obj.InitialState:fetch() == 1
                    local force_passwd_reset = user_obj.FirstLoginPolicy:fetch() == 2
                    if is_user_force_passwd_reset(initial_state, force_passwd_reset) then
                        local user_id = user_obj.UserId:fetch()
                        if not check_first_login_allow(method, url, session_hash, user_id) then
                            -- 首次登陆若访问其他非预期资源，则删除该会话，返回超时
                            logging:error('Illeagal visit , delete session')
                            utils.DeleteOldSession('127.0.0.1', session_id)
                            return http.reply_error(http.status.HTTP_UNAUTHORIZED, 'SessionTimeout', 'invalid session.')
                        end
                    end
                end
            end

            local token = tools.get_token_by_session(session.session_id)
            if method == 'GET' or csrf_token == token then
                return http.reply_ok({
                    Privilege = session.user_privilege,
                    UserName = session.loginname,
                    ClientIp = session.loginipaddress,
                    RoleId = session.user_roleid,
                    SessionId = session.session_id,
                    AuthType = session.authenticateType
                })
            else
                return http.reply_error(http.status.HTTP_UNAUTHORIZED, 'NoValidSession', 'invalid session.')
            end
        else
            local ok, rsp = utils.call_method(nil, session_obj, 'GetSessionLogoutType', function(err_id)
                if err_id == 0xd5 then
                    return http.reply_error(http.status.HTTP_UNAUTHORIZED, 'SessionTimeout', 'invalid session.')
                end
            end, {gstring(session_id)})
            if not ok then
                return rsp
            end
            return http.reply_error(http.status.HTTP_UNAUTHORIZED, logout_ret_map[rsp[1]], 'invalid session.')
        end
    end):fetch()
end

function M.KeepAlive(data, user)
    local alive_time = 0
    if data.Mode == 'Activate' then
        alive_time = O.Session.Timeout:fetch()
    elseif data.Mode == 'Deactivate' then
        alive_time = 10
    end

    return O.Session:next(function(obj)
        local input_list = {gstring(user.SessionId), gint32(alive_time)}
        local ok, _ = utils.call_method(user, obj, 'SessionHeartBeat', nil, input_list)
        if ok then
            return http.reply_ok()
        else
            return http.reply_bad_request()
        end
    end):fetch()
end

return M
