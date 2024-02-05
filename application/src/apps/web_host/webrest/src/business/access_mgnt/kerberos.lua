local utils = require 'utils'
local dflib = require 'dflib'
local c = require 'dflib.core'
local http = require 'http'
local bit = require 'bit'
local logging = require 'logging'
local portal = require "portal"
local C = dflib.class
local O = dflib.object
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local gint32 = GVariant.new_int32
local guint32 = GVariant.new_uint32
local gstring = GVariant.new_string
local new_byte_string = GVariant.new_byte_string
local call_method = utils.call_method
local modify_file_mode = utils.modify_file_mode
local check_file_exists = utils.check_file_exists
local real_file_path = utils.real_file_path
local is_tmp_web_path = utils.is_tmp_web_path
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_bad_request = http.reply_bad_request
local reply_ok = http.reply_ok
local safe_call = http.safe_call

-- 登录类型定义
local LOG_TYPE_KRB_SERVER = 13

local USER_LOGIN_INTERFACE_VALUE_MASK = 0x81 -- 本地用户目前只支持Web和Redfish接口
local RFPROP_LOGIN_INTERFACE_ARRAY_LEN = 8
local RFPROP_LOGIN_INTERFACE_WEB_VALUE = 1
local RFPROP_LOGIN_INTERFACE_SSH_VALUE = 8
local RFPROP_LOGIN_INTERFACE_REDFISH_VALUE = 128

local RFERR_INSUFFICIENT_PRIVILEGE = 403
local RFERR_WRONG_PARAM = 400
local PROPERTY_LDAP_RET_ERROR = -6002000
local PROPERTY_LDAP_LENGTH_ERROR = -6002002
local PROPERTY_LDAP_NO_MATCH_ERROR = 4
local PROPERTY_LDAP_FOLDER_MISMATCH = 5
local IMPORT_CRL_ERR_FORMAT_ERR = 0x100 -- 格式错误，无法解析出CRL对象
local IMPORT_CRL_ERR_ISSUER_VERIFY_FAIL = 0x101 -- 进行CRL发行者签名校验失败
local IMPORT_CRL_ERR_ROOT_CERT_NOT_IMPORT = 0x102 -- 对应根证书未导入的错误
local RFPROP_KRB_KEYTABLE_UPLOAD_FAILED = 2
local RFPROP_KRB_KEYTABLE_FORMAT_ERROR = 3

local PROPERTY_LDAP_SERVICE_DTR_LENTH = 255
local MAX_GROUP_COUNT = 5

local MSG_INVALID_FILE = "An error occurred during the firmware upload process. Details: invalid upload file."

local errMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request('PropertyModificationNeedPrivilege'),
    [RFERR_WRONG_PARAM] = reply_bad_request('ResourceMissingAtURI'),
    [PROPERTY_LDAP_RET_ERROR] = reply_bad_request('PropertyValueFormatError'),
    [PROPERTY_LDAP_LENGTH_ERROR] = reply_bad_request('PropertyValueExceedsMaxLength'),
    [PROPERTY_LDAP_NO_MATCH_ERROR] = reply_bad_request('BootProtocolNotFoundInPFs'),
    [PROPERTY_LDAP_FOLDER_MISMATCH] = reply_bad_request('PropertyValueMismatch'),
    [IMPORT_CRL_ERR_FORMAT_ERR] = reply_bad_request('CrlFileFormatError'),
    [IMPORT_CRL_ERR_ISSUER_VERIFY_FAIL] = reply_bad_request('RootCertificateMismatch'),
    [IMPORT_CRL_ERR_ROOT_CERT_NOT_IMPORT] = reply_bad_request('RootCertificateNotImported'),
    [RFPROP_KRB_KEYTABLE_UPLOAD_FAILED] = reply_bad_request('KRBKeytabUploadFail'),
    [RFPROP_KRB_KEYTABLE_FORMAT_ERROR] = reply_bad_request('KRBKeytabUploadFail')
}

local encryptFileDataErrMap = {
    [-1] = reply_bad_request('KRBKeytabUploadFail')
}

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

local function GroupUserRoleIdToStr(id)
    local privArrLdap = {}
    privArrLdap[2] = 'Common User'
    privArrLdap[3] = 'Operator'
    privArrLdap[4] = 'Administrator'
    privArrLdap[5] = 'Custom Role 1'
    privArrLdap[6] = 'Custom Role 2'
    privArrLdap[7] = 'Custom Role 3'
    privArrLdap[8] = 'Custom Role 4'
    privArrLdap[15] = 'No Access'

    if 2 <= id and id <= 8 then
        return privArrLdap[id]
    else
        return privArrLdap[8]
    end
end

local function GroupUserRoleIdToInt(userRoleStr)
    local privArrLdap = {}
    privArrLdap[2] = 'Common User'
    privArrLdap[3] = 'Operator'
    privArrLdap[4] = 'Administrator'
    privArrLdap[5] = 'Custom Role 1'
    privArrLdap[6] = 'Custom Role 2'
    privArrLdap[7] = 'Custom Role 3'
    privArrLdap[8] = 'Custom Role 4'
    privArrLdap[15] = 'No Access'

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
        return 'Web'
    elseif value == RFPROP_LOGIN_INTERFACE_SSH_VALUE then
        return 'SSH'
    elseif value == RFPROP_LOGIN_INTERFACE_REDFISH_VALUE then
        return 'Redfish'
    end
end

local function LoginInterfaceIntToInt(value)
    if value == 'Web' then
        return RFPROP_LOGIN_INTERFACE_WEB_VALUE
    elseif value == 'SSH' then
        return RFPROP_LOGIN_INTERFACE_SSH_VALUE
    elseif value == 'Redfish' then
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
            array[#array + 1] = 'Rule' .. i
        end
    end
    return array
end

local function LoginRuleToInt(loginRule)
    local result = 0
    for i = 1, #loginRule, 1 do
        local base = 1
        for num = 1, 3, 1 do
            local ruleStr = 'Rule' .. num
            if loginRule[i] == ruleStr then
                result = result + base
            end
            base = 2 * base
        end
    end
    return result
end

local function DelGroup(groupHandle, user)
    local ok, result = call_method(user, groupHandle, 'DelGroup', errMap)
    if not ok then
        return result
    end
end

local function SetGroupName(name, groupHandle, user)
    if #name > PROPERTY_LDAP_SERVICE_DTR_LENTH then
        return reply_bad_request('PropertyValueExceedsMaxLength')
    end
    local ok, result = call_method(user, groupHandle, 'SetGroupName', errMap, gstring(name))
    if not ok then
        return result
    end
end

local function SetGroupSID(SID, groupHandle, user)
    if #SID <= 0 then
        return reply_bad_request('PropertyValueNotInList')
    end
    local ok, result = call_method(user, groupHandle, 'SetGroupSID', errMap, gstring(SID))
    if not ok then
        return result
    end
end

local function SetGroupFolder(folder, groupHandle, user)
    if #folder > PROPERTY_LDAP_SERVICE_DTR_LENTH then
        return reply_bad_request('PropertyValueExceedsMaxLength')
    end
    local ok, result = call_method(user, groupHandle, 'SetGroupFolder', errMap, gstring(folder))
    if not ok then
        return result
    end
end

local function SetGroupUserRole(userRole, groupHandle, user)
    local userRoleInt = GroupUserRoleIdToInt(userRole)
    local _, _ = call_method(user, groupHandle, 'SetGroupPrivilege', errMap, gbyte(userRoleInt))
    local ok, result = call_method(user, groupHandle, 'SetGroupUserRoleId', errMap, gbyte(userRoleInt))
    if not ok then
        return result
    end
end

local function SetGroupLoginRule(loginRule, groupHandle, user)
    if #loginRule > 3 then
        return reply_bad_request('PropertyValueExceedsMaxLength')
    end
    local loginRuleInt = LoginRuleToInt(loginRule)
    local ok, result = call_method(user, groupHandle, 'SetLdapPermitRuleIds', errMap, gbyte(loginRuleInt))
    if not ok then
        return result
    end
end

local function SetGroupLoginInterface(loginInterface, groupHandle, user)
    if #loginInterface > 3 then
        return reply_bad_request('PropertyValueExceedsMaxLength')
    end
    local loginInterfaceInt = 0
    for i = 1, #loginInterface, 1 do
        loginInterfaceInt = bit.bor(loginInterfaceInt, LoginInterfaceIntToInt(loginInterface[i]))
    end
    local ok, result = call_method(user, groupHandle, 'SetGroupLoginInterface', errMap, guint32(loginInterfaceInt))
    if not ok then
        return result
    end
end

local function SetGroup(group, user)
    logging:debug(' SetGroup: group count:%d', #group)
    local setGroupId = -1
    local groupCnt = #group
    if groupCnt > MAX_GROUP_COUNT then
        return reply_bad_request('PropertyMemberQtyExceedLimit')
    end

    --   跳过group为{}的情况
    for groupId = 0, groupCnt - 1, 1 do
        if next(group[groupId + 1]) ~= nil then
            setGroupId = groupId
            break
        end
    end

    if setGroupId == -1 then
        return reply_bad_request('ArrayPropertyInvalidItem')
    end

    logging:debug('setGroupId:%d, group count:%d', setGroupId, #group)
    for groupId = setGroupId, groupCnt - 1, 1 do
        local isFind = false
        local objGroup
        local ret
        C.LDAPGroup():fold(function(obj)
            local groupControllerId = obj.LDAPServerID:fetch()
            if bit.band(groupControllerId, 0x0f) == LOG_TYPE_KRB_SERVER and obj.GroupID:fetch() == groupId then
                isFind = true
                objGroup = obj
                return false
            end
        end)

        if isFind == false then
            return reply_bad_request('MismatchedGroupAndController')
        end

        if group[groupId + 1].DelGroupFlag == true then
            ret = DelGroup(objGroup, user)
            if ret then
                return ret
            end
        else
            if group[groupId + 1].Name ~= nil then
                if group[groupId + 1].Name:match('^%s*(.-)%s*$') == "" then
                    return reply_bad_request("PropertyValueNotInList")
                end
                ret = SetGroupName(group[groupId + 1].Name:match('^%s*(.-)%s*$'), objGroup, user)
                if ret then
                    return ret
                end
            end

            if group[groupId + 1].SID ~= nil then
                if group[groupId + 1].SID:find(' ') then
                    return reply_bad_request("PropertyValueFormatError")
                end
                ret = SetGroupSID(group[groupId + 1].SID:match('^%s*(.-)%s*$'), objGroup, user)
                if ret then
                    return ret
                end
            end

            if group[groupId + 1].Domain ~= nil then
                ret = SetGroupFolder(group[groupId + 1].Domain:match('^%s*(.-)%s*$'), objGroup, user)
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

local M = {}

function M.GetKerberos(_)
    local kbrGoupCnt = 0
    local objKerbers
    C.LDAP():fold(function(obj)
        if obj.ID:fetch() == LOG_TYPE_KRB_SERVER then
            objKerbers = obj
        end
    end):fetch()
    if objKerbers == nil then
        return reply_bad_request('InternalError')
    end

    return {
        Enabled = O.KRBCommon.Enable:next(IntToBool):fetch(),
        IPAddress = objKerbers.HostAddr:fetch(),
        Port = objKerbers.Port:fetch(),
        Domain = objKerbers.UserDomain:fetch(),
        Group = {
            Members = C.LDAPGroup():fold(function(obj, acc)
                if bit.band(obj.LDAPServerID:fetch(), 0x0f) == LOG_TYPE_KRB_SERVER then
                    acc[#acc + 1] = {
                        ID = obj.GroupID:fetch(),
                        Name = obj.GroupName:fetch(),
                        SID = obj.SID:fetch(),
                        Domain = obj.GroupFolder:fetch(),
                        RoleID = obj.GroupUserRoleId:next(GroupUserRoleIdToStr):fetch(),
                        LoginRule = obj.GroupPermitRuleIds:next(LoginRuleToStr):fetch(),
                        LoginInterface = obj.GroupLoginInterface:next(GetLoginInterface):fetch()
                    }
                    kbrGoupCnt = kbrGoupCnt + 1
                end
                return acc
            end, {}):fetch(),
            Count = kbrGoupCnt
        }
    }
end

function M.SetKerberos(data, user)
    local reauth = utils.ReAuthUser(user, data.ReauthKey)
    if reauth then
        utils.OperateLog(user,
            'Failed to set kerberos controller, because the current user password is incorrect or the account is locked')
        return reauth
    end
    local result = reply_ok()
    local objKerbers
    C.LDAP():fold(function(obj)
        if obj.ID:fetch() == LOG_TYPE_KRB_SERVER then
            objKerbers = obj
        end
    end):fetch()
    if objKerbers == nil then
        return reply_bad_request('InternalError')
    end

    result:join(safe_call(function()
        if data.IPAddress then
            if #data.IPAddress > PROPERTY_LDAP_SERVICE_DTR_LENTH then
                return reply_bad_request('PropertyValueExceedsMaxLength')
            end
            local input = gstring(data.IPAddress)
            local ok, ret = call_method(user, objKerbers, 'SetHostAddr', errMap, input)
            if not ok then
                return ret
            end
        end

        if data.Port then
            local input = gint32(data.Port)
            local ok, ret = call_method(user, objKerbers, 'SetLdapPort', errMap, input)
            if not ok then
                return ret
            end
        end

        if data.Domain then
            if #data.Domain > PROPERTY_LDAP_SERVICE_DTR_LENTH then
                return reply_bad_request('PropertyValueExceedsMaxLength')
            end
            local input = gstring(data.Domain)
            local ok, ret = call_method(user, objKerbers, 'SetUserDomain', errMap, input)
            if not ok then
                return ret
            end
        end

        if data.Group then
            local ret = SetGroup(data.Group, user)
            if ret then
                return ret
            end
        end
    end))

    if result:isOk() then
        return reply_ok_encode_json(M.GetKerberos(user))
    end
    result:appendErrorData(M.GetKerberos(user))
    return result
end

function M.SetKerberosState(data, user)
    local result = reply_ok()
    result:join(safe_call(function()
        if data.Enabled ~= nil then
            local input = gbyte(BoolToInt(data.Enabled))
            local ok, ret = call_method(user, O.KRBCommon, 'SetKRBEnable', errMap, input)
            if not ok then
                return ret
            end
        end
    end))

    if result:isOk() then
        return reply_ok_encode_json(M.GetKerberos(user))
    end
    result:appendErrorData(M.GetKerberos(user))
    return result
end

-- formdata:user
--          headers:Type-TEXT
--                  Content-FILE_CONTENT
--          fields:Type-URI
--                 ReauthKey-二次认证密码的base64编码
--                 Content:file_name-文件名称
--                         data-文件内容
function M.ImportKeyTab(formdata)
    local user = formdata.user
    local reauth = utils.ReAuthUser(user, formdata.fields.ReauthKey)
    if reauth then
        utils.OperateLog(user,
            'Failed to import kerberos key table, because the current user password is incorrect or the account is locked')
        return reauth
    end

    local objKerbers
    local filename = formdata.fields.Content.file_name

    C.LDAP():fold(function(obj)
        if obj.ID:fetch() == LOG_TYPE_KRB_SERVER then
            objKerbers = obj
        end
    end):fetch()
    if objKerbers == nil then
        utils.OperateLog(user, 'Upload file(%s) failed', filename)
        return reply_bad_request('InternalError')
    end

    local ret, content = portal.base64_decode(formdata.fields.Content.data)
    if not ret then
        utils.OperateLog(user, 'Upload file(%s) failed', filename)
        return reply_bad_request('InternalError')
    end

    local filePath = "/tmp/web/" .. filename
    local realPath = real_file_path(filePath)
    if not realPath or not is_tmp_web_path(realPath) then
        utils.OperateLog(user, 'Upload file(%s) failed', filename)
        return reply_bad_request('FirmwareUploadError', MSG_INVALID_FILE)
    end

    if check_file_exists(user, filePath) then
        utils.OperateLog(user, 'Upload file(%s) failed', filename)
        return reply_bad_request("SpecifiedFileExist")
    end
    O.PrivilegeAgent:call('DeleteFile', nil, {gstring(filePath)})

    local ok, result = call_method(user, O.UploadSensitiveFile, "EncryptFileData", encryptFileDataErrMap, new_byte_string(content) , gstring(filePath))
    if not ok then
        logging:error("Encrypt kerberos key table failed.")
        utils.OperateLog(user, 'Upload file(%s) failed', filename)
        return result
    end
    modify_file_mode(user, filePath)

    utils.OperateLog(user, 'Upload file(%s) successfully', filename)

    ok, result = call_method(user, objKerbers, 'KRBKeytabUpload', errMap, gstring(filePath))
    if not ok then
        logging:error("Import kerberos key table failed.")
        c.rm_filepath(filePath)
        return result
    end

    return reply_ok()
end

return M
