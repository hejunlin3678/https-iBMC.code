local cfg = require 'config'
local bit = require 'bit'
local utils = require 'utils'
local dflib = require 'dflib'
local c = require 'dflib.core'
local cjson = require 'cjson'
local http = require 'http'
local ffi = require 'ffi'
local tasks = require 'tasks'
local logging = require 'logging'
local bit_and = bit.band
local bit_or = bit.bor
local C = dflib.class
local DFL_OK = dflib.DFL_OK
local null = cjson.null
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local guint32 = GVariant.new_uint32
local gstring = GVariant.new_string
local call_method = utils.call_method
local safe_call = http.safe_call
local reply_internal_server_error = http.reply_internal_server_error
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_ok = http.reply_ok
local reply_error = http.reply_error
local reply_bad_request = http.reply_bad_request
local reply_not_found = http.reply_not_found
local reply_forbidden = http.reply_forbidden
local HTTP_BAD_REQUEST = http.status.HTTP_BAD_REQUEST
local HTTP_FORBIDDEN = http.status.HTTP_FORBIDDEN
local BoolToValue = utils.BoolToValue
local rf_string_check = utils.rf_string_check
local HasUserMgntPrivilege = utils.HasUserMgntPrivilege
local USER_ROLE_ADMIN = cfg.USER_ROLE_ADMIN
local USER_ROLE_OPERATOR = cfg.USER_ROLE_OPERATOR
local USER_ROLE_COMMONUSER = cfg.USER_ROLE_COMMONUSER
local USER_ROLE_CUSTOMROLE1 = cfg.USER_ROLE_CUSTOMROLE1
local USER_ROLE_CUSTOMROLE2 = cfg.USER_ROLE_CUSTOMROLE2
local USER_ROLE_CUSTOMROLE3 = cfg.USER_ROLE_CUSTOMROLE3
local USER_ROLE_CUSTOMROLE4 = cfg.USER_ROLE_CUSTOMROLE4
local USER_ROLE_NOACCESS = cfg.USER_ROLE_NOACCESS
local UUSER_ROLE_USER = cfg.UUSER_ROLE_USER
local UUSER_ROLE_OPER = cfg.UUSER_ROLE_OPER
local UUSER_ROLE_ADMIN = cfg.UUSER_ROLE_ADMIN
local UUSER_ROLE_CUSt1 = cfg.UUSER_ROLE_CUSt1
local UUSER_ROLE_CUSt2 = cfg.UUSER_ROLE_CUSt2
local UUSER_ROLE_CUSt3 = cfg.UUSER_ROLE_CUSt3
local UUSER_ROLE_CUSt4 = cfg.UUSER_ROLE_CUSt4
local UUSER_ROLE_NOACCESS = cfg.UUSER_ROLE_NOACCESS
local get_security_enhanced_compatible_board_v3 = dflib.get_security_enhanced_compatible_board_v3
local UIP_USER_COMMON = 2
local UIP_USER_OPERATOR = 3
local UIP_USER_ADMIN = 4
local UIP_USER_CUSTOMROLE1 = 5
local UIP_USER_CUSTOMROLE2 = 6
local UIP_USER_CUSTOMROLE3 = 7
local UIP_USER_CUSTOMROLE4 = 8

local LOG_TYPE_LOCAL = 0
local RET_OK = 0
local RF_ENABLE = 1
local USER_LOGIN_INTERFACE_VALUE_MASK = 0xDF
local USER_LOGIN_INTERFACE_VALUE_MASK_FOR_PANGEA = 0xDD
local RFPROP_LOGIN_INTERFACE_ARRAY_LEN = 8
local URI_FORMAT_LOGRULE = '/UI/Rest/AccessMgnt/LoginRule/%d'
local LOGRULE_1 = 1
local LOGRULE_2 = 2
local LOGRULE_3 = 3
local USER_MIN_USER_NUMBER = 2
local USER_MAX_USER_NUMBER = 17
local PWSWORDMAXLENGH = 20
local OPERATION_ENABLE_USER = 1
local OPERATION_SET_PASSWD = 2
local DISABLED = 0
local ENABLED = 1
local PRODUCT_VERSION_V5 = 5
local RFERR_INSUFFICIENT_PRIVILEGE = bit_or(0x000000, 403)
local USER_PUBLIC_KEY_TYPE_DELETE = 0
local USER_PUBLIC_KEY_TYPE_FILE = 1
local HTTP_INTERNAL_SERVER_ERROR = -1
local USER_PROCESS = 7
local MAX_TRY_COUNT = 10
local SSH_PUBLIC_KEY_MAX_LEN = 2048
local TMP_SSH_PUBLIC_KEY_PUB_FILE = '/tmp/client_tmp.pub'
local RFACTION_FORMAT_TEXT = 'Text'
local RFACTION_FORMAT_FILE = 'URI'
local S_IRUSR = 256
local S_IWUSR = 128
local ACTION_IMPORT_MUTAU_CERT = '/UI/Rest/AccessMgnt/TwoFactorAuthentication/ImportClientCertificate'
local ACTION_INFO_IMPORT_MUTAU_CERT =
    '/UI/Rest/AccessMgnt/TwoFactorAuthentication/ImportMutualAuthClientCertActionInfo'
local ACTION_DELETE_MUTAU_CERT = '/UI/Rest/AccessMgnt/TwoFactorAuthentication/DeleteCertificate'
local ACTION_INFO_DELETE_MUTAU_CERT =
    '/UI/Rest/AccessMgnt/TwoFactorAuthentication/DeleteMutualAuthClientCertActionInfo'
local ACTION_IMPORT_SSH_PUBLIC_KEY = '/UI/Rest/AccessMgnt/Accounts/%d/ImportSSHPublicKey'
local ACTION_INFO_IMPORT_SSH_PUBLIC_KEY = '/UI/Rest/AccessMgnt/Accounts/%d/ImportSSHPublicKeyActionInfo'
local ACTION_DELETE_SSH_PUBLIC_KEY = '/UI/Rest/AccessMgnt/Accounts/%d/DeleteSSHPublicKey'
local ACTION_INFO_DELETE_SSH_PUBLIC_KEY = '/UI/Rest/AccessMgnt/Accounts/%d/DeleteSSHPublicKeyActionInfo'
local RFACTION_MUTAU_CERT_IMPORT_CLI_CERT = 'Account.ImportMutualAuthClientCert'
local RFACTION_MUTAU_CERT_DELETE_CLI_CERT = 'Account.DeleteMutualAuthClientCert'
local RFACTION_IMPORT_SSH_PUBLICKEY = 'Account.ImportSSHPublicKey'
local RFACTION_DELETE_SSH_PUBLICKEY = 'Account.DeleteSSHPublicKey'
local RFPROP_TARGET = 'target'
local RFPROP_ACTION_INFO = '@Redfish.ActionInfo'
local RFPROP_USER_SSHLOGIN = 'SSHLogin'
local RFPROP_USER_EMERGENCYUSER = 'EmergencyUser'
local RFPROP_USER_TRAPV3USER = 'TrapV3User'

local RFPROP_LOGIN_INTERFACE_WEB = 'Web'
local RFPROP_LOGIN_INTERFACE_SNMP = 'SNMP'
local RFPROP_LOGIN_INTERFACE_IPMI = 'IPMI'
local RFPROP_LOGIN_INTERFACE_SSH = 'SSH'
local RFPROP_LOGIN_INTERFACE_SFTP = 'SFTP'
local RFPROP_LOGIN_INTERFACE_LOCAL = 'Local'
local RFPROP_LOGIN_INTERFACE_REDFISH = 'Redfish'
local RFPROP_LOGIN_INTERFACE_IPMI_VALUE = 4

local LOGIN_INTERFACES = {
    RFPROP_LOGIN_INTERFACE_WEB, RFPROP_LOGIN_INTERFACE_SNMP, RFPROP_LOGIN_INTERFACE_IPMI,
    RFPROP_LOGIN_INTERFACE_SSH, RFPROP_LOGIN_INTERFACE_SFTP, nil, RFPROP_LOGIN_INTERFACE_LOCAL,
    RFPROP_LOGIN_INTERFACE_REDFISH
}

local LOGIN_INTERFACE_VALUE_MAP = {
    [RFPROP_LOGIN_INTERFACE_WEB] = 1,
    [RFPROP_LOGIN_INTERFACE_SNMP] = 2,
    [RFPROP_LOGIN_INTERFACE_IPMI] = RFPROP_LOGIN_INTERFACE_IPMI_VALUE,
    [RFPROP_LOGIN_INTERFACE_SSH] = 8,
    [RFPROP_LOGIN_INTERFACE_SFTP] = 16,
    [RFPROP_LOGIN_INTERFACE_LOCAL] = 64,
    [RFPROP_LOGIN_INTERFACE_REDFISH] = 128
}

local SNMPAuthProtocol = {
    USE_MD5 = 0,
    USE_SHA = 1,
    USE_SHA1 = 2, -- 鉴权算法统一使用SHA，USE_SHA1等价于USE_SHA，向前兼容
    USE_SHA256 = 3,
    USE_SHA384 = 4,
    USE_SHA512 = 5,
    SNMP_AUTH_PROTOCOL_BUT = 6
}

local SNMP_AUTHPRO_MD5 = 'MD5'
local SNMP_AUTHPRO_SHA = 'SHA'
local SNMP_AUTHPRO_SHA1 = 'SHA1'
local SNMP_AUTHPRO_SHA256 = 'SHA256'
local SNMP_AUTHPRO_SHA384 = 'SHA384'
local SNMP_AUTHPRO_SHA512 = 'SHA512'

local SNMP_PRIVPRO_DES = 'DES'
local SNMP_PRIVPRO_AES = 'AES'
local SNMP_PRIVPRO_AES256 = 'AES256'
local SNMP_PRIVPRO_VAL_DES = 1
local SNMP_PRIVPRO_VAL_AES = 2
local SNMP_PRIVPRO_VAL_AES256 = 3

local Permit_ROLE_MAP = {Rule1 = 1, Rule2 = 2, Rule3 = 4}

local PRIVILEGE_INFO = {
    {USER_ROLE_ADMIN, UIP_USER_ADMIN, UIP_USER_ADMIN}, {USER_ROLE_OPERATOR, UIP_USER_OPERATOR, UIP_USER_OPERATOR},
    {USER_ROLE_COMMONUSER, UIP_USER_COMMON, UIP_USER_COMMON},
    {USER_ROLE_CUSTOMROLE1, UIP_USER_OPERATOR, UIP_USER_CUSTOMROLE1},
    {USER_ROLE_CUSTOMROLE2, UIP_USER_OPERATOR, UIP_USER_CUSTOMROLE2},
    {USER_ROLE_CUSTOMROLE3, UIP_USER_OPERATOR, UIP_USER_CUSTOMROLE3},
    {USER_ROLE_CUSTOMROLE4, UIP_USER_OPERATOR, UIP_USER_CUSTOMROLE4},
    {USER_ROLE_NOACCESS, UUSER_ROLE_NOACCESS, UUSER_ROLE_NOACCESS}
}

local ROLEID_ROLE_MAP = {
    [UUSER_ROLE_USER] = USER_ROLE_COMMONUSER,
    [UUSER_ROLE_OPER] = USER_ROLE_OPERATOR,
    [UUSER_ROLE_ADMIN] = USER_ROLE_ADMIN,
    [UUSER_ROLE_CUSt1] = USER_ROLE_CUSTOMROLE1,
    [UUSER_ROLE_CUSt2] = USER_ROLE_CUSTOMROLE2,
    [UUSER_ROLE_CUSt3] = USER_ROLE_CUSTOMROLE3,
    [UUSER_ROLE_CUSt4] = USER_ROLE_CUSTOMROLE4,
    [UUSER_ROLE_NOACCESS] = USER_ROLE_NOACCESS
}

local USER_NODE_BUSY = 0xC0
local USER_DATA_LENGTH_INVALID = 0xC7
local COMP_CODE_INVALID_FIELD = 0xCC
local USER_INVALID_DATA_FIELD = 0xCC
local USER_UNSUPPORT = 0xD5
local USER_DELETE_TRAPV3_USER_UNSUPPORT = 0xD7
local USER_DELETE_EXCLUDE_USER_UNSURPPORT = 0xD9
local USER_NAME_EXIST = 0x80
local USER_DONT_EXIST = 0x81
local USER_PASS_COMPLEXITY_FAIL = 0x84
local USER_TIME_LIMIT_UNREASONABLE = 0x91
local USER_PUBLIC_KEY_FORMAT_ERROR = 0x92
local USER_CANNT_SET_SAME_PASSWORD = 0x93
local USER_SET_PASSWORD_EMPTY = 0x94
local USER_SET_PASSWORD_TOO_WEAK = 0x95
local UUSER_USERNAME_INVALID = -121
local UUSER_USERPASS_TOO_LONG = -182
local UUSER_USERPASS_EMPTY = -183
local UUSER_LINUX_DEFAULT_USER = -184
local USER_PRIVILEGE_WRONG = 9000
local USER_NAME_RESTRICTED = 9001
local USER_PRIV_RESTRICTED = 9003
local EXCLUDE_USER_WRONG = 9005
local USER_LAST_ADMIN_ERR = 9006

local ADD_ACCOUNT_ERR_MAP = {
    [UUSER_USERNAME_INVALID] = reply_bad_request('InvalidUserName', 'Invalid user name'),
    [USER_NAME_EXIST] = reply_bad_request('ResourceAlreadyExists', 'The user already exists'),
    [USER_NAME_RESTRICTED] = reply_bad_request('UserNameIsRestricted', 'The user name is restricted'),
    [UUSER_USERPASS_TOO_LONG] = reply_bad_request('InvalidPasswordLength', 'Invalid password length'),
    [UUSER_USERPASS_EMPTY] = reply_bad_request('InvalidPassword', 'Invalid password'),
    [USER_SET_PASSWORD_EMPTY] = reply_bad_request('InvalidPassword', 'Invalid password'),
    [USER_PASS_COMPLEXITY_FAIL] = reply_bad_request('PasswordComplexityCheckFail',
        'The password does not meet the password complexity requirements'),
    [USER_SET_PASSWORD_TOO_WEAK] = reply_bad_request('PasswordInWeakPWDDict',
        'The password cannot be in the weak password dictionary'),
    [USER_INVALID_DATA_FIELD] = reply_bad_request('InvalidUserName', 'Invalid user name'),
    [USER_PRIVILEGE_WRONG] = reply_bad_request('RoleIdIsRestricted', 'The role id is restricted'),
    [UUSER_LINUX_DEFAULT_USER] = reply_bad_request('ConflictWithLinuxDefaultUser',
        'Conflict with Linux default user'),
    [HTTP_BAD_REQUEST] = reply_bad_request('PropertyValueNotInList', 'The property does not exist')
}

local UPDATE_ACCOUNT_ERR_MAP = {
    [USER_NAME_EXIST] = reply_bad_request('ResourceAlreadyExists', 'The user already exists'),
    [USER_NAME_RESTRICTED] = reply_bad_request('UserNameIsRestricted', 'The user name is restricted'),
    [UUSER_USERNAME_INVALID] = reply_bad_request('InvalidUserName', 'Invalid user name'),
    [USER_INVALID_DATA_FIELD] = reply_bad_request('InvalidUserName', 'Invalid user name'),
    [UUSER_LINUX_DEFAULT_USER] = reply_bad_request('ConflictWithLinuxDefaultUser',
        'Conflict with Linux default user'),
    [USER_TIME_LIMIT_UNREASONABLE] = reply_bad_request('DuringMinimumPasswordAge',
        'The password is in the minimum password validity period and cannot be changed'),
    [UUSER_USERPASS_TOO_LONG] = reply_bad_request('PropertyValueExceedsMaxLength', 'The password is too long'),
    [USER_CANNT_SET_SAME_PASSWORD] = reply_bad_request('InvalidPasswordSameWithHistory',
        'The historical passwords in the restricted range cannot be used'),
    [UUSER_USERPASS_EMPTY] = reply_bad_request('InvalidPassword', 'Invalid password'),
    [USER_SET_PASSWORD_EMPTY] = reply_bad_request('InvalidPassword', 'Invalid password'),
    [COMP_CODE_INVALID_FIELD] = reply_bad_request('InvalidPassword', 'Invalid password'),
    [USER_PASS_COMPLEXITY_FAIL] = reply_bad_request('PasswordComplexityCheckFail',
        'The password does not meet the password complexity requirements'),
    [USER_SET_PASSWORD_TOO_WEAK] = reply_bad_request('PasswordInWeakPWDDict',
        'The password cannot be in the weak password dictionary'),
    [USER_PRIV_RESTRICTED] = reply_bad_request('RoleIdIsRestricted', 'The role id is restricted'),
    [EXCLUDE_USER_WRONG] = reply_bad_request('EmergencyLoginUser', 'Emergency login user'),
    [HTTP_INTERNAL_SERVER_ERROR] = reply_internal_server_error(),
    [USER_UNSUPPORT] = reply_bad_request('EmergencyLoginUser', 'Emergency login user'),
    [USER_LAST_ADMIN_ERR] = reply_bad_request('CannotDisableLastAdministrator', 'Cannot disable last administrator'),
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request('PropertyModificationNeedPrivilege',
        'Property Modification needs privilege'),
    [USER_DELETE_TRAPV3_USER_UNSUPPORT] = reply_bad_request('TrapV3loginUser',
        'When trap is enabled, the SNMP v3 trap user names cannot be changed.')
}

local UPDATE_SNMP_PRIVACY_PASSWORD_ERR_MAP = {
    [USER_INVALID_DATA_FIELD] = reply_bad_request('InvalidUserName', 'Invalid user name'),
    [USER_DONT_EXIST] = reply_bad_request('InvalidUserName', 'Invalid user name'),
    [USER_DATA_LENGTH_INVALID] = reply_bad_request('InvalidPassword', 'Invalid snmp privacy password password'),
    [USER_SET_PASSWORD_TOO_WEAK] = reply_bad_request('PasswordInWeakPWDDict',
        'The snmp privacy password cannot be in the weak password dictionary'),
    [USER_SET_PASSWORD_EMPTY] = reply_bad_request('InvalidPassword', 'Invalid snmp privacy password'),
    [USER_PASS_COMPLEXITY_FAIL] = reply_bad_request('PasswordComplexityCheckFail',
        'The snmp privacy password does not meet the password complexity requirements'),
    [USER_NODE_BUSY] = reply_bad_request('OperationFailed', 'Update snmp privacy password failed')
}

local DELETE_ACCOUNT_ERR_MAP = {
    [USER_DELETE_TRAPV3_USER_UNSUPPORT] = reply_bad_request('AccountForbidRemoved',
        'Emergency login user or SNMPtrapv3 user'),
    [USER_DELETE_EXCLUDE_USER_UNSURPPORT] = reply_bad_request('AccountForbidRemoved',
        'Emergency login user or SNMPtrapv3 user'),
    [USER_LAST_ADMIN_ERR] = reply_bad_request('CannotDeleteLastAdministrator', 'Cannot delete last administrator')
}

local DELETE_SSH_PUBLIC_KEY_ERR_MAP = {[HTTP_INTERNAL_SERVER_ERROR] = reply_internal_server_error()}

ffi.cdef [[
    int chmod(const char *path, unsigned int mode);
]]

-- Parse the value of Logininterface to the corresponding binary value, convert each bit in the binary 
-- value to a corresponding character string, and assign the space to which *o_result_jso points
-- Web :  1  SNMP : 2  IPMI :  4  SSH:   8  SFTP: 16  Local: 64  Redfish : 128
local function GetLoginInterface(loginInterface)
    local mask = USER_LOGIN_INTERFACE_VALUE_MASK
    if C.PRODUCT[0].ProductId:fetch() == 0x0d and
        C.BOARD[0].BoardId:fetch() ~= 0x9c and -- CST1020V6的9c板子不进入该分支
        C.BOARD[0].BoardId:fetch() ~= 0x9b and -- CST1020V6的9b板子不进入该分支
        C.BOARD[0].BoardId:fetch() ~= 0x9f and -- CST1220V6的板子不进入该分支
        C.BOARD[0].BoardId:fetch() ~= 0xa6 then -- CST0220V6的板子不进入该分支
        mask = USER_LOGIN_INTERFACE_VALUE_MASK_FOR_PANGEA
    end
    loginInterface = bit_and(loginInterface, mask)
    local loginInterfaceList = {}
    local base = 1
    local loginInterfaces = {}
    for i = 1, RFPROP_LOGIN_INTERFACE_ARRAY_LEN do
        loginInterfaceList[i] = bit_and(loginInterface, base)
        base = base * 2
        if loginInterfaceList[i] ~= 0 and i ~= 6 then
            table.insert(loginInterfaces, LOGIN_INTERFACES[i])
        end
    end
    return loginInterfaces
end

local function GetLoginRule(permitRuleIds)
    local loginRules = {}
    if bit_and(permitRuleIds, 0x01) ~= 0 then
        table.insert(loginRules, string.format(URI_FORMAT_LOGRULE, LOGRULE_1))
    end
    if bit_and(permitRuleIds, 0x02) ~= 0 then
        table.insert(loginRules, string.format(URI_FORMAT_LOGRULE, LOGRULE_2))
    end
    if bit_and(permitRuleIds, 0x04) ~= 0 then
        table.insert(loginRules, string.format(URI_FORMAT_LOGRULE, LOGRULE_3))
    end
    return loginRules
end

local function GetAvaliableUserId(userId, operateUser)
    if not userId then
        for i = USER_MIN_USER_NUMBER, USER_MAX_USER_NUMBER do
            local user = C.User('UserId', i):next():fetch_or()
            if not user or not user.UserName:fetch() then
                return false, reply_internal_server_error()
            elseif user.UserName:fetch():len() == 0 then
                return true, i
            end
        end
        return false,
            reply_bad_request('CreateUserNumLimit', 'The number of created users reaches the upper limit')
    end
    local user = C.User('UserId', userId):next():fetch_or()
    if not user or not user.UserName:fetch() then
        return false, reply_internal_server_error()
    end
    if user.UserName:fetch():len() ~= 0 then
        utils.OperateLog(operateUser, 'Add user%d\'s id (%d) failed', userId, userId)
        return false, reply_bad_request('UserIdExistsed', 'The user Id already exists')
    end
    return true, userId
end

local function CaculateLoginInterface(loginInterface)
    local newLoginInterface = 0
    if not loginInterface then
        return newLoginInterface
    end
    for i = 1, #loginInterface do
        newLoginInterface = newLoginInterface + LOGIN_INTERFACE_VALUE_MAP[loginInterface[i]]
    end
    newLoginInterface = bit_and(newLoginInterface, USER_LOGIN_INTERFACE_VALUE_MASK)
    return newLoginInterface
end

local function CaculateLoginRule(loginRule)
    local newLoginRule = 0
    if not loginRule then
        return newLoginRule
    end
    for i = 1, #loginRule do
        newLoginRule = bit_or(newLoginRule, Permit_ROLE_MAP[loginRule[i]])
    end
    return newLoginRule
end

local function GetPrivilegeAndRoleId(role)
    for i = 1, #PRIVILEGE_INFO do
        if role == PRIVILEGE_INFO[i][1] then
            return PRIVILEGE_INFO[i][2], PRIVILEGE_INFO[i][3]
        end
    end
end

local function RoleIdToRole(roleId)
    return ROLEID_ROLE_MAP[roleId] or 'unknown'
end

local function CatchError(ok, ret, errMap, action)
    if not ok then
        return ret
    end
    if errMap[ret[1]] then
        return errMap[ret[1]]:clone()
    end
    if ret[1] ~= nil and ret[1] ~= RET_OK then
        return reply_bad_request('PropertyNotModified', 'Fail to ' .. action)
    end
end

local function SetEnabled(user, obj, userId, enabled)
    if enabled == nil then
        return
    end
    if not HasUserMgntPrivilege(user) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'Enabled'})
    end
    return safe_call(function()
        local inputList = {gbyte(userId), gstring(''), gbyte(PWSWORDMAXLENGH), gbyte(BoolToValue(enabled))}
        local ok, ret = call_method(user, obj, 'SetPasswd', nil, inputList)
        local catchError = CatchError(ok, ret, UPDATE_ACCOUNT_ERR_MAP, 'update user enabled status')
        if catchError then
            return catchError
        end
    end)
end

local function SetUserName(user, obj, userId, userName)
    if not userName then
        return
    end

    if #userName == 0 then
        return reply_bad_request('InvalidUserName', {'UserName'})
    end

    if not HasUserMgntPrivilege(user) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'UserName'})
    end

    return safe_call(function()
        local inputList = {gbyte(userId), gstring(userName)}
        local ok, ret = call_method(user, obj, 'SetUserName', nil, inputList)
        local catchError = CatchError(ok, ret, UPDATE_ACCOUNT_ERR_MAP, 'update user name')
        if catchError then
            return catchError
        end
        for i = 1, MAX_TRY_COUNT do
            -- After multiple tests, file_manage_app is synchronized in the second loop at most. Set this parameter to 10 to prevent exceptions
            tasks.sleep(200)
            ok = call_method(user, obj, 'CheckUserNameSyncState', nil, gstring(userName))
            if ok then
                logging:info('User name sync ok in loop %d', i)
                break
            end
            if i == MAX_TRY_COUNT then
                logging:error('User name sync is not done in 2 sec')
            end
        end
    end)
end

local function IsUserRolePrivEnable(userId, prop)
    return C.User('UserId', userId).UserRoleId:next(function(roleId)
        return C.UserRole('RoleId', roleId)[prop]:fetch_or() == 1
    end):fetch_or(false)
end

local function _rf_check_user_configure_priv(user, userId)
    return HasUserMgntPrivilege(user) or
               (user.AuthType == LOG_TYPE_LOCAL and C.User('UserName', user.UserName).UserId:fetch() == userId and
                   IsUserRolePrivEnable(userId, 'ConfigureSelf'))
end

local function SetPassword(user, obj, userId, userName, password)
    if not password then
        return
    end
    if not _rf_check_user_configure_priv(user, userId) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'Password'})
    end
    return safe_call(function()
        -- When complex password read check is enabled, the password cannot be the same as the user name or user name reverse write
        -- The password is set before the password. Therefore, the password must be different from the user name or the reverse of the user name
        if C.PasswdSetting[0].EnableStrongPassword:fetch() == RF_ENABLE and
            (password == userName or password == userName:reverse()) then
            return reply_bad_request('PasswordNotSatisfyComplexity',
                'The password does not meet the password complexity requirements')
        end
        local inputList = {gbyte(userId), gstring(password), gbyte(PWSWORDMAXLENGH), gbyte(OPERATION_SET_PASSWD)}
        local ok, ret = call_method(user, obj, 'SetPasswd', nil, inputList)
        return CatchError(ok, ret, UPDATE_ACCOUNT_ERR_MAP, 'update password')
    end)
end

local function SetPrivilege(user, obj, userId, role)
    if not role then
        return
    end
    if not HasUserMgntPrivilege(user) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'RoleID'})
    end
    return safe_call(function()
        local privilege, roleId = GetPrivilegeAndRoleId(role)
        if not privilege then
            return reply_bad_request('PropertyValueNotInList', 'Fail to get privilege and role id from role')
        end
        local inputList = {gbyte(userId), gbyte(privilege), gbyte(roleId)}
        local ok, ret = call_method(user, obj, 'SetPrivilege', nil, inputList)
        local catchError = CatchError(ok, ret, UPDATE_ACCOUNT_ERR_MAP, 'update privilege')
        if catchError then
            return catchError
        end
        ok, ret = call_method(user, obj, 'SetUserRoleId', UPDATE_ACCOUNT_ERR_MAP, gbyte(roleId))
        if not ok then
            return ret
        end
    end)
end

local function SetInsecurePromptEnabled(user, obj, userId, enabled)
    if enabled == nil then
        return
    end
    if not _rf_check_user_configure_priv(user, userId) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'InsecurePromptEnabled'})
    end
    return safe_call(function()
        local inputList = {gbyte(BoolToValue(enabled))}
        local ok, ret = call_method(user, obj, 'SetInitialState', nil, inputList)
        local catchError = CatchError(ok, ret, UPDATE_ACCOUNT_ERR_MAP, 'update user initial state')
        if catchError then
            return catchError
        end
    end)
end

local function SetUserFirstLoginPolicy(user, obj, userId, policy)
    if not policy then
        return
    end
    if not _rf_check_user_configure_priv(user, userId) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'FirstLoginPolicy'})
    end
    return safe_call(function()
        local inputList = {gbyte(policy)}
        local ok, ret = call_method(user, obj, 'SetUserFirstLoginPolicy', nil, inputList)
        local catchError = CatchError(ok, ret, UPDATE_ACCOUNT_ERR_MAP, 'update user first login policy')
        if catchError then
            return catchError
        end
    end)
end

local function SetLoginInterface(user, obj, loginInterface)
    if not loginInterface then
        return
    end
    if not HasUserMgntPrivilege(user) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'LoginInterface'})
    end
    return safe_call(function()
        local ok, ret = call_method(user, obj, 'SetUserLoginInterface', nil,
            guint32(CaculateLoginInterface(loginInterface)))
        if not ok then
            return ret
        end
    end)
end

local function SetLoginRole(user, obj, loginRule)
    if not loginRule then
        return
    end
    if not HasUserMgntPrivilege(user) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'LoginRule'})
    end
    return safe_call(function()
        local ok, ret = call_method(user, obj, 'SetUserPermitRuleIds', UPDATE_ACCOUNT_ERR_MAP,
            gbyte(CaculateLoginRule(loginRule)))
        if not ok then
            return ret
        elseif ret[1] ~= RET_OK then
            return reply_internal_server_error('SetUserPermitRuleIdsFailed', 'Fail to set user login rule')
        end
    end)
end

local function SetSnmpPrivacyPassword(user, obj, userId, snmpV3PrivPasswd)
    if not snmpV3PrivPasswd then
        return
    end

    if not _rf_check_user_configure_priv(user, userId) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'SNMPV3PrivPasswd'})
    end

    return safe_call(function()
        local productVersionNum = C.PRODUCT[0].ProductVersionNum:fetch()
        if not productVersionNum then
            return reply_internal_server_error()
        end
        if C.PMEServiceConfig[0].SnmpdEnable:fetch_or(DISABLED) == DISABLED or productVersionNum <
            PRODUCT_VERSION_V5 then
            return reply_bad_request('ActionNotSupported', 'Set snmp privacy password not supported')
        end
        local inputList = {gbyte(userId), gstring(snmpV3PrivPasswd)}
        local ok, ret = call_method(user, obj, 'SetSnmpPrivacyPassword', UPDATE_SNMP_PRIVACY_PASSWORD_ERR_MAP,
            inputList)
        if not ok then
            return ret
        end
    end)
end

local function CheckUserPrivilege(user, obj)
    if HasUserMgntPrivilege(user) then
        return true
    end
    
    local userName = obj.UserName:fetch()
    if user.UserName ~= userName then
        return false
    end

    local roleId = obj.UserRoleId:fetch()
    local configure_self_priv = C.UserRole('RoleId', roleId).ConfigureSelf:fetch_or(DISABLED)

    return configure_self_priv == ENABLED
end

local snmp_encryption_to_protocol = {
    [SNMP_AUTHPRO_MD5] = SNMPAuthProtocol.USE_MD5,
    [SNMP_AUTHPRO_SHA] = SNMPAuthProtocol.USE_SHA,
    [SNMP_AUTHPRO_SHA1] = SNMPAuthProtocol.USE_SHA1,
    [SNMP_AUTHPRO_SHA256] = SNMPAuthProtocol.USE_SHA256,
    [SNMP_AUTHPRO_SHA384] = SNMPAuthProtocol.USE_SHA384,
    [SNMP_AUTHPRO_SHA512] = SNMPAuthProtocol.USE_SHA512
}

local function SetSnmpAuthProtocol(user, obj, userId, data)
    if not data.SnmpV3AuthProtocol then -- SnmpV3PrivProtocol
        return
    end

    if not CheckUserPrivilege(user, obj) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'SnmpV3AuthProtocol'})
    end

    local authProtocolInt = snmp_encryption_to_protocol[data.SnmpV3AuthProtocol]
    local snmpLoginPasswordStr
    local snmpPrivPasswordStr
    if not authProtocolInt then
        logging:error('the snmp authentication algorithm does not support, snmpV3AuthProtocol:%s',
            data.SnmpV3AuthProtocol)
        return reply_bad_request('PropertyItemNotInList')
    end

    if authProtocolInt < SNMPAuthProtocol.USE_SHA256 then
        local privProtocolInt
        if data.SnmpV3PrivProtocol ~= nil then
            if data.SnmpV3PrivProtocol == SNMP_PRIVPRO_DES then
                privProtocolInt = SNMP_PRIVPRO_VAL_DES
            elseif data.SnmpV3PrivProtocol == SNMP_PRIVPRO_AES then
                privProtocolInt = SNMP_PRIVPRO_VAL_AES
            elseif data.SnmpV3PrivProtocol == SNMP_PRIVPRO_AES256 then
                logging:error('privacy protocol is AES256, auth protocol should be SHA256/SHA384/SHA512')
                return reply_bad_request('PrivProtocolAes256NeedMatch')
            else
                logging:error('invalid property value :%s', data.SnmpV3PrivProtocol)
                return reply_bad_request('PropertyItemNotInList')
            end

            local inputList = {gbyte(userId), gbyte(privProtocolInt)}
            local ok, ret = call_method(user, obj, 'SetSnmpPrivacyProtocol', nil, inputList)
            if not ok then
                logging:error('set snmpv3 user priv protocol failed, ret:%d', ret[1])
                return reply_bad_request('InternalError')
            end
        else
            privProtocolInt = obj.SnmpPrivProtocol:fetch()
            if privProtocolInt >= SNMP_PRIVPRO_VAL_AES256 then
                logging:error('privacy protocol is AES256, auth protocol should be SHA256/SHA384/SHA512')
                return reply_bad_request('PrivProtocolAes256NeedMatch')
            end
        end
    end

    snmpLoginPasswordStr = data.Password
    snmpPrivPasswordStr = data.SNMPV3PrivPasswd
    return safe_call(function()
        local inputList = {
            gbyte(userId), gbyte(authProtocolInt), gstring(snmpLoginPasswordStr), gstring(snmpPrivPasswordStr)
        }
        local ok, ret = call_method(user, obj, 'SetSnmpAuthProtocol', nil, inputList)
        if not ok then
            logging:error('set snmpv3 user auth protocol failed, ret:%d', ret[1])
            return reply_bad_request('InternalError')
        end
        if ret[2] ~= RET_OK or ret[3] ~= RET_OK then
            return reply_bad_request('InternalError')
        end
    end)
end

local function SetSnmpPrivacyProtocol(user, obj, userId, data)
    -- SnmpV3AuthProtocol不为空时，避免在set_account_huawei中再次调用set方法设置加密密码
    if not data.SnmpV3PrivProtocol or data.SnmpV3AuthProtocol == SNMP_AUTHPRO_MD5 or data.SnmpV3AuthProtocol ==
        SNMP_AUTHPRO_SHA or data.SnmpV3AuthProtocol == SNMP_AUTHPRO_SHA1 then
        return
    end

    if not CheckUserPrivilege(user, obj) then
        return reply_bad_request('PropertyModificationNeedPrivilege', {'SnmpV3PrivProtocol'})
    end

    return safe_call(function()
        local privProtocolInt
        local authProtocol = obj.SnmpAuthProtocol:fetch()
        if data.SnmpV3PrivProtocol == SNMP_PRIVPRO_DES then
            privProtocolInt = SNMP_PRIVPRO_VAL_DES
        elseif data.SnmpV3PrivProtocol == SNMP_PRIVPRO_AES then
            privProtocolInt = SNMP_PRIVPRO_VAL_AES
        elseif data.SnmpV3PrivProtocol == SNMP_PRIVPRO_AES256 then
            if authProtocol > SNMPAuthProtocol.USE_SHA512 then
                return reply_bad_request('InternalError')
            end
            if authProtocol < SNMPAuthProtocol.USE_SHA256 then
                return reply_bad_request('PrivProtocolAes256NeedMatch')
            end
            privProtocolInt = SNMP_PRIVPRO_VAL_AES256
        else
            logging:error('invalid property value :%s', data.SnmpV3PrivProtocol)
            return reply_bad_request('InternalError')
        end

        local inputList = {gbyte(userId), gbyte(privProtocolInt)}
        local ok, ret = call_method(user, obj, 'SetSnmpPrivacyProtocol', nil, inputList)
        if not ok then
            logging:error('set snmpv3 user priv protocol failed, ret:%d', ret[1])
            return reply_bad_request('InternalError')
        end
    end)
end

local function GetDelDisableReason(userId)
    local uId = C.SecurityEnhance[0].ExcludeUser:fetch_or()
    local version = C.TrapConfig[0].TrapVersion:fetch()
    local trapv3Userid = C.TrapConfig[0].Trapv3Userid:fetch()
    if uId and uId == userId then
        -- Check whether the user is a security enhancement user
        return false, RFPROP_USER_EMERGENCYUSER
    elseif version == 3 and trapv3Userid == userId then
        -- Check whether the user is an IPv3-enabled user.
        return false, RFPROP_USER_TRAPV3USER
    end
    return true, null
end

local function GetMutualAuthClientCert(id)
    if C.PRODUCT[0].MutualAuthenticationEnable:fetch_or() ~= 1 then
        return null
    end
    local mutualAuthenticationCliCert = C.MutualAuthenticationCliCert('hashId', id)
    local issueBy = rf_string_check(mutualAuthenticationCliCert.IssueBy:fetch())
    local issueTo = rf_string_check(mutualAuthenticationCliCert.IssueTo:fetch())
    local validFrom = rf_string_check(mutualAuthenticationCliCert.ValidFrom:fetch())
    local validTo = rf_string_check(mutualAuthenticationCliCert.ValidTo:fetch())
    local serialNumber = rf_string_check(mutualAuthenticationCliCert.SerialNumber:fetch())
    local issuerId = mutualAuthenticationCliCert.IssuerId:fetch()
    local fingerPrint = rf_string_check(mutualAuthenticationCliCert.FingerPrint:fetch())
    local sigAlgorithm = mutualAuthenticationCliCert.SigAlgorithm:fetch_or(null)
    if issueBy == null or issueTo == null or validFrom == null or validTo == null or serialNumber == null or
        fingerPrint == null then
        return
    end
    if sigAlgorithm == '' then
        -- The certificate is not initialized and needs to be initialized again
        local hashValue = mutualAuthenticationCliCert.hashValue:fetch()
        local inputList = {gbyte(id), guint32(hashValue)}
        local ok = call_method(nil, mutualAuthenticationCliCert, 'ReInitClientCertificate', nil, inputList)
        if ok then
            sigAlgorithm = mutualAuthenticationCliCert.SigAlgorithm:fetch_or(null)
        end
    end
    local keyUsage = mutualAuthenticationCliCert.KeyUsage:fetch_or(null)
    local keyLength = mutualAuthenticationCliCert.KeyLength:fetch_or(null)
    local rootCertUploadedState = issuerId > 0
    -- Obtains the certificate revocation status
    local revokedState = mutualAuthenticationCliCert.RevokedState:fetch() == ENABLED
    local revokedDate = rf_string_check(mutualAuthenticationCliCert.RevokedDate:fetch_or())
    return {
        IssueBy = issueBy,
        IssueTo = issueTo,
        ValidFrom = validFrom,
        ValidTo = validTo,
        SerialNumber = serialNumber,
        RootCertUploadedState = rootCertUploadedState,
        FingerPrint = fingerPrint,
        SignatureAlgorithm = sigAlgorithm,
        KeyUsage = keyUsage,
        PublicKeyLengthBits = keyLength,
        RevokedState = revokedState,
        RevokedDate = revokedDate
    }
end

local function GetActions(userId)
    return {
        [RFACTION_MUTAU_CERT_IMPORT_CLI_CERT] = {
            [RFPROP_TARGET] = ACTION_IMPORT_MUTAU_CERT,
            [RFPROP_ACTION_INFO] = ACTION_INFO_IMPORT_MUTAU_CERT
        },
        [RFACTION_MUTAU_CERT_DELETE_CLI_CERT] = {
            [RFPROP_TARGET] = ACTION_DELETE_MUTAU_CERT,
            [RFPROP_ACTION_INFO] = ACTION_INFO_DELETE_MUTAU_CERT
        },
        [RFACTION_IMPORT_SSH_PUBLICKEY] = {
            [RFPROP_TARGET] = string.format(ACTION_IMPORT_SSH_PUBLIC_KEY, userId),
            [RFPROP_ACTION_INFO] = string.format(ACTION_INFO_IMPORT_SSH_PUBLIC_KEY, userId)
        },
        [RFACTION_DELETE_SSH_PUBLICKEY] = {
            [RFPROP_TARGET] = string.format(ACTION_DELETE_SSH_PUBLIC_KEY, userId),
            [RFPROP_ACTION_INFO] = string.format(ACTION_INFO_DELETE_SSH_PUBLIC_KEY, userId)
        }
    }
end

local snmp_protocol_to_encryption = {
    [SNMPAuthProtocol.USE_MD5] = SNMP_AUTHPRO_MD5,
    [SNMPAuthProtocol.USE_SHA] = SNMP_AUTHPRO_SHA,
    [SNMPAuthProtocol.USE_SHA1] = SNMP_AUTHPRO_SHA1,
    [SNMPAuthProtocol.USE_SHA256] = SNMP_AUTHPRO_SHA256,
    [SNMPAuthProtocol.USE_SHA384] = SNMP_AUTHPRO_SHA384,
    [SNMPAuthProtocol.USE_SHA512] = SNMP_AUTHPRO_SHA512
}

local function GetSnmpV3AuthProtocol(obj)
    return obj.LoginInterface:next(function(loginInterface)
        if bit.band(loginInterface, 2) == 0 then
            return null
        end
        return snmp_protocol_to_encryption[obj.SnmpAuthProtocol:fetch()] or null
    end):fetch_or(null)
end

local function GetSnmpV3PrivProtocol(obj)
    local privprotocol = obj.SnmpPrivProtocol:fetch()
    if privprotocol == SNMP_PRIVPRO_VAL_DES then
        return SNMP_PRIVPRO_DES
    elseif privprotocol == SNMP_PRIVPRO_VAL_AES then
        return SNMP_PRIVPRO_AES
    elseif privprotocol == SNMP_PRIVPRO_VAL_AES256 then
        return SNMP_PRIVPRO_AES256
    else
        logging:error('the snmp authentication algorithm does not support %d .', privprotocol)
        return null
    end
end

local M = {}

function M.GetAccounts(user)
    local count = 0
    local members = C.User():fold(function(obj, acc)
        local userId = obj.UserId:fetch()
        local userName = obj.UserName:fetch()
        if not userName or userName:len() == 0 then
            return acc
        end
        if not HasUserMgntPrivilege(user) and (user.AuthType ~= 0 or user.UserName ~= userName) then
            return acc
        end
        local deleteable, delDisableReason = GetDelDisableReason(userId)
        -- 落地首次登陆强制改密码的需求才呈现该属性
        local first_login_plcy
        if get_security_enhanced_compatible_board_v3() == 0 then
            first_login_plcy = obj.FirstLoginPolicy:fetch()
        end
        table.insert(acc, {
            ID = userId,
            Enabled = obj.IsUserEnable:fetch() == 1,
            UserName = userName,
            RoleID = obj.UserRoleId:next(RoleIdToRole):fetch_or(null),
            LoginInterface = obj.LoginInterface:next(GetLoginInterface):fetch_or(null),
            LoginRule = obj.PermitRuleIds:next(GetLoginRule):fetch_or(null),
            PasswordValidityDays = obj.PwdValidDays:neq(0xffff):fetch_or(null),
            SSHPublicKeyHash = rf_string_check(obj.PublickeyHash:fetch()),
            InsecurePromptEnabled = obj.InitialState:fetch() == 1,
            FirstLoginPolicy = first_login_plcy,
            SNMPEncryptPwdInit = obj.SnmpPrivacyPwdInitialState:fetch() == 1,
            MutualAuthClientCert = GetMutualAuthClientCert(userId),
            Actions = GetActions(userId),
            Deleteable = deleteable,
            DelDisableReason = delDisableReason,
            SnmpV3AuthProtocol = GetSnmpV3AuthProtocol(obj),
            SnmpV3PrivProtocol = GetSnmpV3PrivProtocol(obj)
        })
        count = count + 1
        return acc
    end, {})
    return {Count = count, Members = members[5]}
end

local function PreventFromAddWrongUser(user, userId)
    return C.User('UserId', userId):next(function(obj)
        local inputList = {gbyte(userId), gstring('')}
        local ok, ret = call_method(user, obj, 'SetUserName', nil, inputList)
        return CatchError(ok, ret, ADD_ACCOUNT_ERR_MAP, 'prevent from adding the wrong user')
    end):fetch()
end

function M.AddAccount(data, user)
    local reauth = utils.ReAuthUser(user, data.ReauthKey)
    if reauth then
        utils.OperateLog(user,
            'Failed to add user, because the current user password is incorrect or the account is locked')
        return reauth
    end

    local isFind, val = GetAvaliableUserId(data.ID, user)
    if not isFind then
        return val
    end
    local userId = val
    local userName = data.UserName
    if not userName or userName:len() == 0 then
        return reply_bad_request('InvalidUserName', 'Invalid user name')
    end
    -- Limitation for hire-purchase privileges borrowed from the Web
    local privilege, roleId = GetPrivilegeAndRoleId(data.RoleID)
    if not privilege then
        return reply_bad_request('PropertyValueNotInList', 'Fail to get privilege and role id from role')
    end
    if userId == 2 and roleId ~= UIP_USER_ADMIN then
        for i = USER_MIN_USER_NUMBER + 1, USER_MAX_USER_NUMBER do
            local newUser = C.User('UserId', i)
            local newUserName = newUser.UserName:fetch()
            if not newUser or not newUserName then
                return reply_internal_server_error()
            elseif newUserName:len() == 0 then
                userId = i
                break
            end
        end
        if userId == 2 then
            return reply_bad_request('CreateUserNumLimit', 'The number of created users reaches the upper limit')
        end
    end
    local inputList, ok, ret, catchError
    local err = C.User('UserId', userId):next(function(obj)
        -- Step 1: Invoke the method to set the user name.
        inputList = {gbyte(userId), gstring(userName), gstring('')}
        ok, ret = call_method(user, obj, 'SetUserName', nil, inputList)
        catchError = CatchError(ok, ret, ADD_ACCOUNT_ERR_MAP, 'set the user name')
        if catchError then
            return catchError
        end

        -- Step 2: Invoke the method to set the password
        inputList = {gbyte(userId), gstring(data.Password), gbyte(PWSWORDMAXLENGH), gbyte(OPERATION_SET_PASSWD)}
        ok, ret = call_method(user, obj, 'SetPasswd', nil, inputList)
        if ret[1] == COMP_CODE_INVALID_FIELD then
            ret[1] = UUSER_USERPASS_EMPTY
        end
        catchError = CatchError(ok, ret, ADD_ACCOUNT_ERR_MAP, 'set the password')
        if catchError then
            return catchError
        end

        -- Step 3: Invoke the method to enable the user
        inputList = {gbyte(userId), gstring(data.Password), gbyte(PWSWORDMAXLENGH), gbyte(OPERATION_ENABLE_USER)}
        ok, ret = call_method(user, obj, 'SetPasswd', nil, inputList)
        catchError = CatchError(ok, ret, ADD_ACCOUNT_ERR_MAP, 'enable the user')
        if catchError then
            return catchError
        end

        -- Step 4 Invoke the method to set user rights
        inputList = {gbyte(userId), gbyte(privilege), gbyte(roleId)}
        ok, ret = call_method(user, obj, 'SetPrivilege', nil, inputList)
        catchError = CatchError(ok, ret, ADD_ACCOUNT_ERR_MAP, 'set privilege')
        if catchError then
            return catchError
        end

        -- Step 5 Invoke the method to set the user role
        ok, ret = call_method(user, obj, 'SetUserRoleId', ADD_ACCOUNT_ERR_MAP, gbyte(roleId))
        if not ok then
            return ret
        end

        -- 只有落地首次登陆强制改密码的需求才需要设置首次登陆策略
        if get_security_enhanced_compatible_board_v3() == 0 then
            -- Step 6 Invoke the method to set the user first login policy
            if not data.FirstLoginPolicy then
                return reply_bad_request('PropertyMissing', 'The property FirstLoginPolicy is a required property and must be included in the request.')
            end
            inputList = {gbyte(data.FirstLoginPolicy)}
            ok, ret = call_method(user, obj, 'SetUserFirstLoginPolicy', nil, inputList)
            catchError = CatchError(ok, ret, ADD_ACCOUNT_ERR_MAP, 'set first login policy')
            if catchError then
                return catchError
            end
        end

        -- Step 7 Invoke the method to set the user login interface
        ok, ret = call_method(user, obj, 'SetUserLoginInterface', nil,
            guint32(CaculateLoginInterface(data.LoginInterface)))
        if not ok then
            return ret
        end

        -- Step 8 Invoke the method to set the user permit rule id
        ok, ret = call_method(user, obj, 'SetUserPermitRuleIds', nil, gbyte(CaculateLoginRule(data.LoginRule)))
        if not ok then
            return ret
        elseif ret[1] ~= RET_OK then
            return reply_internal_server_error('SetUserPermitRuleIdsFailed', 'Fail to set user login rule')
        end
    end):catch(function(err)
        return reply_internal_server_error('AddAccountError', err)
    end):fetch()
    if err then
        PreventFromAddWrongUser(user, userId)
        return err
    end
    return reply_ok_encode_json(M.GetAccounts(user))
end

local function IpmiPermissionAdded(obj, data)
    local oldLoginInterfaceCode = obj.LoginInterface:fetch()
    if oldLoginInterfaceCode == nil then
        return false
    end
    oldLoginInterfaceCode = bit.band(oldLoginInterfaceCode, USER_LOGIN_INTERFACE_VALUE_MASK)
    logging:info('oldLoginInterfaceCode = %d', oldLoginInterfaceCode)
    local newLoginInterface = data.LoginInterface
    if newLoginInterface == nil then
        return false
    end
    local newLoginInterfaceCode = CaculateLoginInterface(newLoginInterface)
    logging:info('newLoginInterfaceCode = %d', newLoginInterfaceCode)
    return bit.band(oldLoginInterfaceCode, RFPROP_LOGIN_INTERFACE_IPMI_VALUE) == 0 and
               bit.band(newLoginInterfaceCode, RFPROP_LOGIN_INTERFACE_IPMI_VALUE) ~= 0
end

local function SnmpAuthProtocalModified(data)
    return data.SnmpV3AuthProtocol ~= nil
end

function M.UpdateAccount(userId, data, user)
    local reauth = utils.ReAuthUser(user, data.ReauthKey)
    if reauth then
        utils.OperateLog(user,
            'Failed to set user information, because the current user password is incorrect or the account is locked')
        return reauth
    end

    if userId < USER_MIN_USER_NUMBER or userId > USER_MAX_USER_NUMBER then
        return reply_not_found()
    end
    -- 是否有用户名校验，对用户名空的uri过滤处理
    if C.User('UserId', userId).UserName:fetch_or() == "" then
        return reply_not_found()
    end
    return C.User('UserId', userId):next(function(obj)
        local result = reply_ok()
        result:join(SetEnabled(user, obj, userId, data.Enabled))
        result:join(SetPrivilege(user, obj, userId, data.RoleID))
        result:join(SetInsecurePromptEnabled(user, obj, userId, data.InsecurePromptEnabled))
        -- 只有落地首次登陆强制改密码的需求才调用该方法
        if get_security_enhanced_compatible_board_v3() == 0 then
            result:join(SetUserFirstLoginPolicy(user, obj, userId, data.FirstLoginPolicy))
        end
        if IpmiPermissionAdded(obj, data) and not data.Password then
            return reply_bad_request('AccountMustResetPassword', {'After adding the IPMI, you must reset the password'})
        end
        if SnmpAuthProtocalModified(data) and (not data.Password or not data.SNMPV3PrivPasswd) then
            return reply_bad_request('AccountMustResetPassword', {'After Modifying the SNMP Auth Protocol, you must reset the password and SNMP privacy password'})
        end
        -- SetSnmpAuthProtocol方法中功能不单一，也会存在密码修改的步骤。
        -- 若执行SetSnmpAuthProtocol则不需要再执行SetPassword方法，否则会重复设置密码，影响历史密码校验
        if data.Password and not SnmpAuthProtocalModified(data) then
            local ret = SetPassword(user, obj, userId, obj.UserName:fetch(), data.Password)
            result:join(ret)
            if not ret:isOk() then
                result:appendErrorData(M.GetAccounts(user))
                return result
            end
        end
        result:join(SetLoginInterface(user, obj, data.LoginInterface))
        result:join(SetLoginRole(user, obj, data.LoginRule))
        result:join(SetSnmpPrivacyPassword(user, obj, userId, data.SNMPV3PrivPasswd))
        result:join(SetSnmpAuthProtocol(user, obj, userId, data))
        result:join(SetSnmpPrivacyProtocol(user, obj, userId, data))
        result:join(SetUserName(user, obj, userId, data.UserName))
        if result:isOk() then
            return reply_ok_encode_json(M.GetAccounts(user))
        end
        result:appendErrorData(M.GetAccounts(user))
        return result
    end):fetch()
end

function M.DeleteAccount(userId, data, user)
    local reauth = utils.ReAuthUser(user, data.ReauthKey)
    if reauth then
        utils.OperateLog(user,
            'Failed to delete user, because the current user password is incorrect or the account is locked')
        return reauth
    end
    if userId < USER_MIN_USER_NUMBER or userId > USER_MAX_USER_NUMBER then
        return reply_not_found()
    end
    if not HasUserMgntPrivilege(user) then
        return reply_forbidden('InsufficientPrivilege')
    end
    local deletedUser = C.User('UserId', userId)
    local userName = deletedUser.UserName:fetch()
    if not userName or userName:len() == 0 then
        return reply_not_found()
    end
    return deletedUser:next(function(obj)
        local result = reply_ok()
        result:join(safe_call(function()
            local inputList = {gbyte(userId), gstring('')}
            local ok, ret = call_method(user, obj, 'SetUserName', nil, inputList)
            local catchError = CatchError(ok, ret, DELETE_ACCOUNT_ERR_MAP)
            if catchError then
                return catchError
            end
        end))
        if result:isOk() then
            return reply_ok_encode_json(M.GetAccounts(user))
        end
        result:appendErrorData(M.GetAccounts(user))
        return result
    end):fetch()
end

function M.DeleteSSHPublicKey(userId, data, user)
    local reauth = utils.ReAuthUser(user, data.ReauthKey)
    if reauth then
        utils.OperateLog(user,
            'Failed to delete ssh public key, because the current user password is incorrect or the account is locked')
        return reauth
    end

    if not _rf_check_user_configure_priv(user, userId) then
        return reply_forbidden('InsufficientPrivilege')
    end
    if userId < USER_MIN_USER_NUMBER or userId > USER_MAX_USER_NUMBER then
        return reply_not_found()
    end
    local deletedUser = C.User('UserId', userId)
    local userName = deletedUser.UserName:fetch()
    if not userName or userName:len() == 0 then
        return reply_internal_server_error()
    end
    -- Public key hash
    local pkhString = deletedUser.PublickeyHash:fetch()
    if not pkhString then
        return reply_internal_server_error()
    elseif pkhString == '' then
        return reply_bad_request('PublicKeyNotExist', 'The public key does not exist')
    end
    local publicKeyTempPath = ''
    return deletedUser:next(function(obj)
        local inputList = {gbyte(userId), gbyte(USER_PUBLIC_KEY_TYPE_DELETE), gstring(publicKeyTempPath)}
        local ok, ret = call_method(user, obj, 'SetPublicKey', DELETE_SSH_PUBLIC_KEY_ERR_MAP, inputList)
        if not ok then
            return ret
        end
        return reply_ok()
    end):fetch()
end

function M.ImportSSHPublicKey(userId, formdata)
    local user = formdata.user
    local filePath
    local fields = formdata.fields
    local type = fields.Type
    local content = fields.Content
    local fileState = true
    if type == RFACTION_FORMAT_TEXT then
        filePath = TMP_SSH_PUBLIC_KEY_PUB_FILE
    else
        filePath = utils.GetFormDataFilePath(formdata)
        fileState = utils.CheckFormData(formdata)
    end
    local ok, ret = pcall(function()
        local reauth = utils.ReAuthUser(user, formdata.fields.ReauthKey)
        if reauth then
            utils.OperateLog(user,
                'Failed to import ssh public key, because the current user password is incorrect or the account is locked')
            return reauth
        end
        if userId < USER_MIN_USER_NUMBER or userId > USER_MAX_USER_NUMBER then
            utils.OperateLog(user, 'Failed to import ssh public key')
            logging:error("ImportSSHPublicKey: user id %d is out of range", userId);
            return reply_not_found()
        end
        if not _rf_check_user_configure_priv(user, userId) then
            utils.OperateLog(user, 'Failed to import ssh public key')
            return reply_forbidden('InsufficientPrivilege')
        end
        local userName = C.User('UserId', userId).UserName:fetch()
        if not userName or userName:len() == 0 then
            utils.OperateLog(user, 'Failed to import ssh public key')
            logging:error("ImportSSHPublicKey: user name of user id %d is invalid", userId);
            return reply_not_found()
        end
        if not type or not content then
            utils.OperateLog(user, 'Failed to import ssh public key')
            logging:error("ImportSSHPublicKey: import type or content is invalid");
            return reply_bad_request('MissingRequiredField')
        end
        if type == RFACTION_FORMAT_TEXT then
            if content:len() > SSH_PUBLIC_KEY_MAX_LEN then
                utils.OperateLog(user, 'Failed to import ssh public key')
                logging:error("ImportSSHPublicKey: import public key length is out of range.");
                return reply_bad_request('PublicKeyImportFailed', 'Public key is too long')
            end
            if c.is_symbol_link(filePath) then
                utils.OperateLog(user, 'Failed to import ssh public key')
                logging:error("ImportSSHPublicKey: import public key is a symbol link.");
                return reply_bad_request('PublicKeyImportFailed', 'File TMP_SSH_PUBLIC_KEY_PUB_FILE is a symbol link.')
            end
            local file, _ = io.open(filePath, 'w+')
            if not file then
                error("cannot open key file")
            end
            file:write(content)
            file:close()
            ffi.C.chmod(filePath, bit_or(S_IRUSR, S_IWUSR))
        elseif type == RFACTION_FORMAT_FILE then
            if fileState == false then
                utils.OperateLog(user, 'Failed to import ssh public key')
                logging:error("ImportSSHPublicKey: import public key file state is invalid.");
                return reply_bad_request('InvalidParam')
            end
            if not filePath then
                utils.OperateLog(user, 'Failed to import ssh public key')
                logging:error("ImportSSHPublicKey: import public key file path is invalid.");
                return reply_bad_request("InvalidFile")
            end
        else
            utils.OperateLog(user, 'Failed to import ssh public key')
            logging:error("ImportSSHPublicKey: import public key type is invalid.");
            return reply_bad_request('PublicKeyTypeError', 'The public key type is wrong')
        end
        return C.User('UserId', userId):next(function(obj)
            local inputList = {gbyte(userId), gbyte(USER_PUBLIC_KEY_TYPE_FILE), gstring(filePath)}
            local ok, ret = call_method(user, obj, 'SetPublicKey', nil, inputList)
            if not ok then
                utils.OperateLog(user, 'Failed to import ssh public key')
                logging:error("ImportSSHPublicKey: call SetPublicKey method failed.");
                return reply_internal_server_error('InternalError')
            end
            if ret[1] == USER_PUBLIC_KEY_FORMAT_ERROR then
                utils.OperateLog(user, 'Failed to import ssh public key')
                logging:error("ImportSSHPublicKey: call SetPublicKey method return key format error.");
                return reply_bad_request('PublicKeyImportError')
            elseif ret[1] ~= DFL_OK then
                utils.OperateLog(user, 'Failed to import ssh public key')
                logging:error("ImportSSHPublicKey: call SetPublicKey method return other error.");
                return reply_internal_server_error('InternalError')
            end
        end):fetch()
    end)
    if filePath and utils.file_exists(filePath) then
        c.rm_filepath(filePath)
    end
    if not ok or ret then
        return ret or reply_internal_server_error('InternalError')
    end
    return reply_ok_encode_json()
end

return M
