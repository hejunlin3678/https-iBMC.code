local utils = require "utils"
local dflib = require "dflib"
local c = require "dflib.core"
local http = require "http"
local bit = require "bit"
local logging = require "logging"
local math = require "math"
local C = dflib.class
local O = dflib.object
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local gint32 = GVariant.new_int32
local guint32 = GVariant.new_uint32
local gstring = GVariant.new_string
local call_method = utils.call_method
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_bad_request = http.reply_bad_request
local reply_not_found = http.reply_not_found
local reply_ok = http.reply_ok
local safe_call = http.safe_call
local cfg = require "config"

local CERT_CHAIN_LDAP_MAX = 10
local LDAP_GROUP_MAX = 5

local CERT_TYPE_RESERVED = 0
local CERT_TYPE_SERVER = 1
local CERT_TYPE_INTERMEDIATE = 2
local CERT_TYPE_ROOT = 3

local REAUTH_ERROR_INFO = "Failed to %s ldap controller %s, because the current user password is incorrect or the account is locked"
local REAUTH_FAIL_REQ_INFO = 'Authorization failed because the user name or password is incorrect or your account is locked.'
-- 登录类型定义
local LOG_TYPE_KRB_SERVER = 13

local USER_LOGIN_INTERFACE_VALUE_MASK = 0xDF -- 本地用户目前只有7个接口,修改为8个接口(redfish)
local RFPROP_LOGIN_INTERFACE_ARRAY_LEN = 8
local RFPROP_LOGIN_INTERFACE_WEB_VALUE = 1
local RFPROP_LOGIN_INTERFACE_SSH_VALUE = 8
local RFPROP_LOGIN_INTERFACE_REDFISH_VALUE = 128

local PROPERTY_LDAP_CACERT_UPLOAD_SUCCESS = 0
local PROPERTY_LDAP_CACERT_UPLOAD_FAILED = 2
local PROPERTY_LDAP_CACERT_TOO_BIG = 1
local PROPERTY_LDAP_CACERT_FORMAT_FAILED = 3
local LDAP_CACERT_OVERDUED = 4

local RFERR_INSUFFICIENT_PRIVILEGE = 403
local RFERR_WRONG_PARAM = 400
local PROPERTY_LDAP_RET_ERROR = -6002000
local PROPERTY_LDAP_LENGTH_ERROR = -6002002
local PROPERTY_LDAP_NO_MATCH_ERROR = 4
local PROPERTY_LDAP_FOLDER_MISMATCH = 5
local IMPORT_CRL_ERR_FORMAT_ERR = 0x100 -- 格式错误，无法解析出CRL对象
local IMPORT_CRL_ERR_ISSUER_VERIFY_FAIL = 0x101 -- 进行CRL发行者签名校验失败
local IMPORT_CRL_ERR_ROOT_CERT_NOT_IMPORT = 0x102 -- 对应根证书未导入的错误
local IMPORT_CRL_ERR_ISSUING_DATE_INVALID = 0x103 -- 证书时间信息错误
local IMPORT_CRL_ERR_NOT_SUPPORT_CRL_SIGN = 0x104 --证书不支持吊销列表签名
local DELETE_CRL_ERR_CRL_FILE_NOT_EXIST = 1 -- 要删除的crl文件不存在

local PROPERTY_LDAP_SERVICE_DTR_LENTH = 255
local USER_PASSWORD_MAX_LEN = 20
local MAX_GROUP_COUNT = 5

local MAX_CRL_FILE_LEN = 100 * 1024 -- CRL文件最大大小100k 

local errMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("PropertyModificationNeedPrivilege"),
    [RFERR_WRONG_PARAM] = reply_bad_request("ResourceMissingAtURI"),
    [PROPERTY_LDAP_RET_ERROR] = reply_bad_request("PropertyValueFormatError"),
    [PROPERTY_LDAP_LENGTH_ERROR] = reply_bad_request("PropertyValueExceedsMaxLength"),
    [PROPERTY_LDAP_NO_MATCH_ERROR] = reply_bad_request("BootProtocolNotFoundInPFs"),
    [PROPERTY_LDAP_FOLDER_MISMATCH] = reply_bad_request("PropertyValueMismatch"),
    [IMPORT_CRL_ERR_FORMAT_ERR] = reply_bad_request("CrlFileFormatError"),
    [IMPORT_CRL_ERR_ISSUER_VERIFY_FAIL] = reply_bad_request("RootCertificateMismatch"),
    [IMPORT_CRL_ERR_ROOT_CERT_NOT_IMPORT] = reply_bad_request("RootCertificateNotImported"),
    [IMPORT_CRL_ERR_ISSUING_DATE_INVALID] = reply_bad_request("CrlIssuingDateInvalid"),
    [IMPORT_CRL_ERR_NOT_SUPPORT_CRL_SIGN] = reply_bad_request("CANotSupportCrlSignature"),
    [DELETE_CRL_ERR_CRL_FILE_NOT_EXIST] = reply_not_found("CRLFileNotExist")
}

local function CertLevelToStr(value)
    if value == 2 then
        return "Demand"
    else
        return "Allow"
    end
end

local function CertLevelToInt(value)
    if value == "Demand" then
        return 2
    else
        return 3
    end
end

local function IntToBool(value)
    if value == 0 then
        return false
    else
        return true
    end
end

local function BoolToInt(value)
    if value == false then
        return 0
    else
        return 1
    end
end

local function GetUserDomain(obj, user)
    local ok, result = call_method(user, obj, "LdapComposeUserDomain", errMap)
    if ok then
        return result[1]
    end
end

local function GetGroupDomain(obj, user)
    local ok, result = call_method(user, obj, "LdapComposeGroupDomain", errMap)
    if ok then
        return result[1]
    end
end

local function GroupUserRoleIdToStr(id)
    local privArrLdap = {}
    privArrLdap[2] = "Common User"
    privArrLdap[3] = "Operator"
    privArrLdap[4] = "Administrator"
    privArrLdap[5] = "Custom Role 1"
    privArrLdap[6] = "Custom Role 2"
    privArrLdap[7] = "Custom Role 3"
    privArrLdap[8] = "Custom Role 4"
    privArrLdap[15] = "No Access"

    if 2 <= id and id <= 8 then
        return privArrLdap[id]
    else
        return privArrLdap[8]
    end
end

local function GroupUserRoleIdToInt(userRoleStr)
    local privArrLdap = {}
    privArrLdap[2] = "Common User"
    privArrLdap[3] = "Operator"
    privArrLdap[4] = "Administrator"
    privArrLdap[5] = "Custom Role 1"
    privArrLdap[6] = "Custom Role 2"
    privArrLdap[7] = "Custom Role 3"
    privArrLdap[8] = "Custom Role 4"
    privArrLdap[15] = "No Access"

    for i = 2, 8, 1 do
        if privArrLdap[i] == userRoleStr then
            return i
        end
    end
    if privArrLdap[15] == userRoleStr then
        return 15
    end
end

local function LoginInterfaceIntToStr(value)
    if value == RFPROP_LOGIN_INTERFACE_WEB_VALUE then
        return "Web"
    elseif value == RFPROP_LOGIN_INTERFACE_SSH_VALUE then
        return "SSH"
    elseif value == RFPROP_LOGIN_INTERFACE_REDFISH_VALUE then
        return "Redfish"
    end
end

local function LoginInterfaceIntToInt(value)
    if value == "Web" then
        return RFPROP_LOGIN_INTERFACE_WEB_VALUE
    elseif value == "SSH" then
        return RFPROP_LOGIN_INTERFACE_SSH_VALUE
    elseif value == "Redfish" then
        return RFPROP_LOGIN_INTERFACE_REDFISH_VALUE
    end
end

local function GetLoginInterface(loginInterface)
    local array = {}
    loginInterface = bit.band(loginInterface, USER_LOGIN_INTERFACE_VALUE_MASK)

    local base = 1
    for _ = 1, RFPROP_LOGIN_INTERFACE_ARRAY_LEN, 1 do
        local index = bit.band(loginInterface, base)
        base = 2 * base
        if index ~= 0 then
            array[#array + 1] = LoginInterfaceIntToStr(index)
        end
    end
    return array
end

local function LoginRuleToStr(loginRule)
    local array = {}
    local base = 1
    for i = 1, 3, 1 do
        local index = bit.band(loginRule, base)
        base = 2 * base
        if index ~= 0 then
            array[#array + 1] = "Rule" .. i
        end
    end
    return array
end

local function LoginRuleToInt(loginRule)
    local result = 0
    for i = 1, #loginRule, 1 do
        local base = 1
        for num = 1, 3, 1 do
            local ruleStr = "Rule" .. num
            if loginRule[i] == ruleStr then
                result = result + base
            end
            base = 2 * base
        end
    end
    return result
end

local function DelGroup(groupHandle, user)
    local ok, result = call_method(user, groupHandle, "DelGroup", errMap)
    if not ok then
        return result
    end
end

local function SetGroupName(name, groupHandle, user)
    if #name > PROPERTY_LDAP_SERVICE_DTR_LENTH then
        return reply_bad_request("PropertyValueExceedsMaxLength")
    end
    local ok, result = call_method(user, groupHandle, "SetGroupName", errMap, gstring(name))
    if not ok then
        return result
    end
end

local function SetGroupFolder(folder, groupHandle, user)
    if #folder > PROPERTY_LDAP_SERVICE_DTR_LENTH then
        return reply_bad_request("PropertyValueExceedsMaxLength")
    end
    local ok, result = call_method(user, groupHandle, "SetGroupFolder", errMap, gstring(folder))
    if not ok then
        return result
    end
end

local function SetGroupDomain(domain, groupHandle, user)
    local ok, result = call_method(user, groupHandle, "SetGroupInfo", errMap, gstring(domain))
    if not ok then
        return result
    end
end

local function SetGroupUserRole(userRole, groupHandle, user)
    local userRoleInt = GroupUserRoleIdToInt(userRole)
    local _, _ = call_method(user, groupHandle, "SetGroupPrivilege", errMap, gbyte(userRoleInt))
    local ok, result = call_method(user, groupHandle, "SetGroupUserRoleId", errMap, gbyte(userRoleInt))
    if not ok then
        return result
    end
end

local function SetGroupLoginRule(loginRule, groupHandle, user)
    if #loginRule > 3 then
        return reply_bad_request("PropertyValueExceedsMaxLength")
    end
    local loginRuleInt = LoginRuleToInt(loginRule)
    local ok, result = call_method(user, groupHandle, "SetLdapPermitRuleIds", errMap, gbyte(loginRuleInt))
    if not ok then
        return result
    end
end

local function SetGroupLoginInterface(loginInterface, groupHandle, user)
    if #loginInterface > 3 then
        return reply_bad_request("PropertyValueExceedsMaxLength")
    end
    local loginInterfaceInt = 0
    for i = 1, #loginInterface, 1 do
        loginInterfaceInt = bit.bor(loginInterfaceInt, LoginInterfaceIntToInt(loginInterface[i]))
    end
    local ok, result = call_method(user, groupHandle, "SetGroupLoginInterface", errMap, guint32(loginInterfaceInt))
    if not ok then
        return result
    end
end

local function SetGroup(controllerId, group, user)
    logging:debug("controllerId:%d, group count:%d", controllerId, #group)
    local setGroupId = -1
    local groupCnt = #group
    if groupCnt > MAX_GROUP_COUNT then
        return reply_bad_request("PropertyMemberQtyExceedLimit")
    end

    --   跳过group为{}的情况
    for groupId = 0, groupCnt - 1, 1 do
        if next(group[groupId + 1]) ~= nil then
            setGroupId = groupId
            break
        end
    end

    if setGroupId == -1 then
        return reply_bad_request("ArrayPropertyInvalidItem")
    end

    logging:debug("setGroupId:%d, group count:%d", setGroupId, #group)
    for groupId = setGroupId, groupCnt - 1, 1 do
        local isFind = false
        local objGroup
        local ret
        C.LDAPGroup():fold(function(obj)
            local groupControllerId = obj.LDAPServerID:fetch()
            if bit.band(groupControllerId, 0x0f) < LOG_TYPE_KRB_SERVER and (groupControllerId - controllerId) % 16 ==
                0 and obj.GroupID:fetch() == groupId then
                isFind = true
                objGroup = obj
                return false
            end
        end)

        if isFind == false then
            if controllerId ~= nil then
                return reply_bad_request("MismatchedGroupAndController")
            end
            return reply_bad_request("InternalError")
        end

        if group[groupId + 1].Name == "" then
            ret = DelGroup(objGroup, user)
            if ret then
                return ret
            end
        else
            if group[groupId + 1].Name then
                ret = SetGroupName(group[groupId + 1].Name, objGroup, user)
                if ret then
                    return ret
                end
            end

            if group[groupId + 1].Folder ~= nil then
                ret = SetGroupFolder(group[groupId + 1].Folder, objGroup, user)
                if ret then
                    return ret
                end
            end

            if group[groupId + 1].GroupDomain ~= nil then
                ret = SetGroupDomain(group[groupId + 1].GroupDomain, objGroup, user)
                if ret then
                    return ret
                end
            end

            if group[groupId + 1].RoleID ~= nil then
                ret = SetGroupUserRole(group[groupId + 1].RoleID, objGroup, user)
                if ret then
                    return ret
                end
            end

            if group[groupId + 1].LoginRule ~= nil then
                ret = SetGroupLoginRule(group[groupId + 1].LoginRule, objGroup, user)
                if ret then
                    return ret
                end
            end

            if group[groupId + 1].LoginInterface ~= nil then
                ret = SetGroupLoginInterface(group[groupId + 1].LoginInterface, objGroup, user)
                if ret then
                    return ret
                end
            end
        end
    end
end

local function ParseImportCertificateResultCode(user, code)
    if code == PROPERTY_LDAP_CACERT_UPLOAD_SUCCESS then
        return reply_ok()
    elseif code == PROPERTY_LDAP_CACERT_TOO_BIG then
        return reply_bad_request("CertImportFailed")
    elseif code == PROPERTY_LDAP_CACERT_UPLOAD_FAILED then
        return reply_bad_request("CertImportFailed")
    elseif code == RFERR_INSUFFICIENT_PRIVILEGE then
        return reply_bad_request("InsufficientPrivilege")
    elseif code == LDAP_CACERT_OVERDUED then
        return reply_bad_request("CertificateExpired")
    elseif code == PROPERTY_LDAP_CACERT_FORMAT_FAILED then
        return reply_bad_request("CertificateFormatError")
    else
        return reply_bad_request("InternalError")
    end
end

local function GetCerts(controllerId)
    local certArray = {}
    certArray[CERT_TYPE_INTERMEDIATE] = {}
    C.LDAPCert():fold(function(obj)
        if (math.floor((obj.CertId:fetch() - 1) / CERT_CHAIN_LDAP_MAX + 1) == controllerId) and
            obj.CertType:fetch() == 1 then
            certArray[CERT_TYPE_SERVER] = {
                Issuer = obj.IssuerInfo:fetch(),
                Subject = obj.SubjectInfo:fetch(),
                ValidNotBefore = obj.StartTime:fetch(),
                ValidNotAfter = obj.Expiration:fetch(),
                SerialNumber = obj.SerialNumber:fetch()
            }
        end

        if math.floor((obj.CertId:fetch() - 1) / CERT_CHAIN_LDAP_MAX + 1) == controllerId and obj.CertType:fetch() ==
            2 then
            certArray[CERT_TYPE_INTERMEDIATE][#certArray[CERT_TYPE_INTERMEDIATE] + 1] =
                {
                    Issuer = obj.IssuerInfo:fetch(),
                    Subject = obj.SubjectInfo:fetch(),
                    ValidNotBefore = obj.StartTime:fetch(),
                    ValidNotAfter = obj.Expiration:fetch(),
                    SerialNumber = obj.SerialNumber:fetch()
                }
        end

        if math.floor((obj.CertId:fetch() - 1) / CERT_CHAIN_LDAP_MAX + 1) == controllerId and obj.CertType:fetch() ==
            3 then
            certArray[CERT_TYPE_ROOT] = {
                Issuer = obj.IssuerInfo:fetch(),
                Subject = obj.SubjectInfo:fetch(),
                ValidNotBefore = obj.StartTime:fetch(),
                ValidNotAfter = obj.Expiration:fetch(),
                SerialNumber = obj.SerialNumber:fetch()
            }
        end
    end):fetch()

    return certArray
end

local function get_ldap_domain_and_folder(objLDAP, user)
    local domain = ""
    local userDomain = utils.split(GetUserDomain(objLDAP, user), ",DC=")
    if #userDomain > 1 then
        domain = userDomain[2]
    end
    for i = 3, #userDomain, 1 do
        domain = domain .. "." .. userDomain[i]
    end

    return domain, userDomain[1]
end

local M = {}

function M.GetLDAP(id, user)
    logging:debug(" GetLDAP id:%d", id)
    local ldapGoupCnt = 0
    local ldapControllerCnt = 0
    local objLDAP
    C.LDAP():fold(function(obj)
        if obj.ID:fetch() < 7 then
            ldapControllerCnt = ldapControllerCnt + 1
        end
        if obj.ID:fetch() == id then
            objLDAP = obj
        end
    end):fetch()

    if objLDAP == nil then
        return {}
    end
    local _ = C.LDAPCert():fold(function(obj)
        if (math.floor((obj.CertId:fetch() - 1) / CERT_CHAIN_LDAP_MAX + 1) == id) and obj.CertType:fetch() ~=
            CERT_TYPE_RESERVED then
            return obj, false
        end
    end):fetch()

    local certArray = GetCerts(id)
    local domain, folder = get_ldap_domain_and_folder(objLDAP, user)

    return {
        ControllerCnt = ldapControllerCnt,
        Controller = {
            IPAddress = objLDAP.HostAddr:fetch(),
            Port = objLDAP.Port:fetch(),
            Domain = domain,
            Folder = folder,
            BindDN = objLDAP.BindDN:fetch()
        },
        Certificate = {
            VerificationEnabled = objLDAP.CertStatus:next(IntToBool):fetch(),
            VerificationLevel = objLDAP.CertificateVerificationLevel:next(CertLevelToStr):fetch(),
            CrlVerification = objLDAP.CRLVerificationEnabled:next(IntToBool):fetch(),
            CRLValidNotBefore = objLDAP.CRLStartTime:fetch(),
            CRLValidNotAfter = objLDAP.CRLExpireTime:fetch(),
            ServerCert = certArray[CERT_TYPE_SERVER],
            IntermediateCert = certArray[CERT_TYPE_INTERMEDIATE],
            RootCert = certArray[CERT_TYPE_ROOT]
        },
        LdapGroup = {
            Members = C.LDAPGroup():fold(function(obj, acc)
                if bit.band(obj.LDAPServerID:fetch(), 0x0f) < LOG_TYPE_KRB_SERVER and
                    (obj.LDAPServerID:fetch() - id) % 16 == 0 then
                    acc[#acc + 1] = {
                        ID = obj.GroupID:fetch(),
                        Name = obj.GroupName:fetch(),
                        Folder = obj.GroupFolder:fetch(),
                        Domain = GetGroupDomain(obj, user),
                        RoleID = obj.GroupUserRoleId:next(GroupUserRoleIdToStr):fetch(),
                        LoginRule = obj.GroupPermitRuleIds:next(LoginRuleToStr):fetch(),
                        LoginInterface = obj.GroupLoginInterface:next(GetLoginInterface):fetch()
                    }
                    ldapGoupCnt = ldapGoupCnt + 1
                end
                if ldapGoupCnt == LDAP_GROUP_MAX then
                    return acc, false
                end
                return acc
            end, {}):fetch(),
            ldapGoupCnt = ldapGoupCnt
        }
    }
end

function M.GetLDAPState(_, _)
    logging:debug(" GetLDAPState ")
    return {Enabled = O.LDAPCommon.Enable:next(IntToBool):fetch()}
end

function M.SetLDAP(id, data, user)
    logging:debug(" SetLDAP id:%d", id)
    local reauth = utils.ReAuthUser(user, data.ReauthKey)
    if reauth then
        utils.OperateLog(user, 'Failed to set ldap controller, because the current user password is incorrect or the account is locked')
        return reauth
    end
    local result = reply_ok()
    local objController
    objController = C.LDAP():fold(function(obj)
        if id < LOG_TYPE_KRB_SERVER and obj.ID:fetch() == id then
            return obj, false
        end
    end):fetch_or(nil)

    if objController == nil then
        return reply_bad_request("InvalidIndex")
    end

    result:join(safe_call(function()
        if data.Controller and data.Controller.IPAddress then
            local input = gstring(data.Controller.IPAddress)
            local ok, ret = call_method(user, objController, "SetHostAddr", errMap, input)
            if not ok then
                return ret
            end
        end

        if data.Controller and data.Controller.Port then
            local input = gint32(data.Controller.Port)
            local ok, ret = call_method(user, objController, "SetLdapPort", errMap, input)
            if not ok then
                return ret
            end
        end

        if data.Controller and (data.Controller.Folder or data.Controller.Domain) then
            local oldDomain, oldFolder = get_ldap_domain_and_folder(objController, user)
            local userDomain = data.Controller.Folder or oldFolder
            local newDomain = data.Controller.Domain or oldDomain
            local domains = {}
            for match in (newDomain .. "."):gmatch('(.-)' .. "%.") do
                table.insert(domains, match)
            end
            for i = 1, #domains, 1 do
                userDomain = userDomain .. ",DC=" .. domains[i]
            end
            local input = gstring(userDomain)
            local ok, ret = call_method(user, objController, "LdapSplitUserDomain", errMap, input)
            if not ok then
                return ret
            end
        end

        if data.Controller and data.Controller.BindDN then
            if #(data.Controller.BindDN) > PROPERTY_LDAP_SERVICE_DTR_LENTH then
                return reply_bad_request("PropertyValueExceedsMaxLength")
            end
            local input = gstring(data.Controller.BindDN)
            local ok, ret = call_method(user, objController, "SetBindDN", errMap, input)
            if not ok then
                return ret
            end
        end

        if data.Controller and data.Controller.BindPwd then
            if #(data.Controller.BindPwd) > USER_PASSWORD_MAX_LEN then
                return reply_bad_request("PropertyValueExceedsMaxLength")
            end
            local input = gstring(data.Controller.BindPwd)
            local ok, ret = call_method(user, objController, "SetBindDNPsw", errMap, input)
            if not ok then
                return ret
            end
        end

        -- UserFolder

        if data.Certificate and data.Certificate.VerificationEnabled ~= nil then
            local input = gbyte(BoolToInt(data.Certificate.VerificationEnabled))
            local ok, ret = call_method(user, objController, "SetCertStatus", errMap, input)
            if not ok then
                return ret
            end
        end

        if data.Certificate and data.Certificate.VerificationLevel then
            local input = gbyte(CertLevelToInt(data.Certificate.VerificationLevel))
            local ok, ret = call_method(user, objController, "SetCertificateVerificationLevel", errMap, input)
            if not ok then
                return ret
            end
        end

        if data.LdapGroup then
            local ret = SetGroup(id, data.LdapGroup, user)
            if ret then
                return ret
            end
        end
    end))

    if result:isOk() then
        return reply_ok_encode_json(M.GetLDAP(id, user))
    end
    result:appendErrorData(M.GetLDAP(id, user))
    return result
end

function M.SetLDAPState(data, user)
    logging:debug(" SetLDAPState ")
    local result = reply_ok()

    result:join(safe_call(function()
        if data.Enabled ~= nil then
            local input = gbyte(BoolToInt(data.Enabled))
            local ok, ret = call_method(user, O.LDAPCommon, "SetLDAPEnable", errMap, input)
            if not ok then
                return ret
            end
        end
    end))

    if result:isOk() then
        return reply_ok_encode_json(M.GetLDAPState())
    end
    result:appendErrorData(M.GetLDAPState())
    return result
end

function M.ImportLDAPCertificate(id, formdata)
    local user = formdata.user
    local filePath = utils.GetFormDataFilePath(formdata)
    if utils.CheckFormData(formdata) == false then
        utils.OperateLog(user, 'Failed to import ldap controller certificate')
        if filePath then
            c.rm_filepath(filePath)
        end
        return reply_bad_request('InvalidParam')
    end

    -- 双因素、ldap、krb和sso登录无需二次认证
    if O.MutualAuthentication.MutualAuthenticationState:fetch_or(0) ~= 1 and user and user.AuthType ~= cfg.LOG_TYPE_LDAP_KRB_SSO then
        if not formdata.fields or not formdata.fields.ReauthKey or #formdata.fields.ReauthKey > 128 then
            if filePath then
                c.rm_filepath(filePath)
            end
            utils.OperateLog(user, REAUTH_ERROR_INFO, "import", "certificate")
            return http.reply_error(http.status.HTTP_UNAUTHORIZED, 'ReauthFailed', REAUTH_FAIL_REQ_INFO)
        end

        if not formdata.fields.ReauthKey:match('^[a-zA-Z0-9+/=]+$') then
            if filePath then
                c.rm_filepath(filePath)
            end
            utils.OperateLog(user, REAUTH_ERROR_INFO, "import", "certificate")
            return http.reply_error(http.status.HTTP_UNAUTHORIZED, 'ReauthFailed', REAUTH_FAIL_REQ_INFO)
        end

        local reauth = utils.ReAuthUser(user, formdata.fields.ReauthKey)
        if reauth then
            if filePath then
                c.rm_filepath(filePath)
            end
            utils.OperateLog(user, REAUTH_ERROR_INFO, "import", "certificate")
            return reauth
        end
    end

    if not filePath then
        utils.OperateLog(user, 'Import ldap controller certificate failed')
        return reply_bad_request("InvalidFile")
    end

    local objLDAP
    C.LDAP():fold(function(obj)
        if obj.ID:fetch() == id then
            objLDAP = obj
        end
    end):fetch()

    local fileLength = utils.file_length(filePath)
    local ok, ret = call_method(user, objLDAP, "LDAPCertUpload", errMap, gint32(fileLength), gbyte(id),
        gstring(filePath))
    if not ok then
        c.rm_filepath(filePath)
        return ret
    end

    return ParseImportCertificateResultCode(user, ret[1])
end

function M.ImportCrlVerification(id, formdata)
    local user = formdata.user
    local filePath = utils.GetFormDataFilePath(formdata)
    if utils.CheckFormData(formdata) == false then
        utils.OperateLog(user, 'Failed to import ldap controller crl')
        if filePath then
            c.rm_filepath(filePath)
        end
        return reply_bad_request('InvalidParam')
    end

    -- 双因素、ldap、krb和sso登录无需二次认证
    if O.MutualAuthentication.MutualAuthenticationState:fetch_or(0) ~= 1 and user and user.AuthType ~= cfg.LOG_TYPE_LDAP_KRB_SSO then
        if not formdata.fields or not formdata.fields.ReauthKey or #formdata.fields.ReauthKey > 128 then
            if filePath then
                c.rm_filepath(filePath)
            end
            utils.OperateLog(user, REAUTH_ERROR_INFO, "import", "crl")
            return http.reply_error(http.status.HTTP_UNAUTHORIZED, 'ReauthFailed', REAUTH_FAIL_REQ_INFO)
        end

        if not formdata.fields.ReauthKey:match('^[a-zA-Z0-9+/=]+$') then
            if filePath then
                c.rm_filepath(filePath)
            end
            utils.OperateLog(user, REAUTH_ERROR_INFO, "import", "crl")
            return http.reply_error(http.status.HTTP_UNAUTHORIZED, 'ReauthFailed', REAUTH_FAIL_REQ_INFO)
        end

        local reauth = utils.ReAuthUser(user, formdata.fields.ReauthKey)
        if reauth then
            if filePath then
                c.rm_filepath(filePath)
            end
            utils.OperateLog(user, REAUTH_ERROR_INFO, "import", "crl")
            return reauth
        end
    end

    if not filePath then
        utils.OperateLog(user, 'Import ldap controller crl failed')
        return reply_bad_request("Invalidile")
    end

    local objLDAP
    C.LDAP():fold(function(obj)
        if obj.ID:fetch() == id then
            objLDAP = obj
        end
    end):fetch()

    local ok, ret = call_method(user, objLDAP, "ImportCRL", errMap, gbyte(id), gstring(filePath))
    if not ok then
        c.rm_filepath(filePath)
        return ret
    end

    return reply_ok()
end

function M.DeleteCrlVerification(id, data, user)
    if id < 1 or id > 6 then
        return reply_not_found('ResourceMissingAtURI')
    end
    -- 双因素、ldap、krb和sso登录无需二次认证
    if O.MutualAuthentication.MutualAuthenticationState:fetch_or(0) ~= 1 and user and user.AuthType ~= cfg.LOG_TYPE_LDAP_KRB_SSO then
        if not data or not data.ReauthKey or #data.ReauthKey > 128 then
            utils.OperateLog(user, REAUTH_ERROR_INFO, "Delete", "crl")
            return http.reply_error(http.status.HTTP_UNAUTHORIZED, 'ReauthFailed', REAUTH_FAIL_REQ_INFO)
        end

        if not data.ReauthKey:match('^[a-zA-Z0-9+/=]+$') then
            utils.OperateLog(user, REAUTH_ERROR_INFO, "Delete", "crl")
            return http.reply_error(http.status.HTTP_UNAUTHORIZED, 'ReauthFailed', REAUTH_FAIL_REQ_INFO)
        end

        local reauth = utils.ReAuthUser(user, data.ReauthKey)
        if reauth then
            utils.OperateLog(user, REAUTH_ERROR_INFO, "Delete", "crl")
            return reauth
        end
    end

    local objLDAP
    C.LDAP():fold(function(obj)
        if obj.ID:fetch() == id then
            objLDAP = obj
        end
    end):fetch()
    local ok, ret = call_method(user, objLDAP, "DeleteCRL", errMap, gbyte(id))
    if not ok then
        utils.OperateLog(user, 'Delete ldap controller crl failed')
        return ret
    end
    return reply_ok()
end
return M
