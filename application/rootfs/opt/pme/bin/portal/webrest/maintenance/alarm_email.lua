local dflib = require "dflib"
local c = require "dflib.core"
local http = require "http"
local utils = require "utils"
local bit = require "bit"
local cjson = require 'cjson'
local reply_ok = http.reply_ok
local reply_ok_encode_json = http.reply_ok_encode_json
local safe_call = http.safe_call
local reply_bad_request = http.reply_bad_request
local C = dflib.class
local O = dflib.object
local rf_string_check = utils.rf_string_check
local call_method = utils.call_method
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local gstring = GVariant.new_string
local gint32 = GVariant.new_int32
local band = bit.band
local null = cjson.null
local logging = require "logging"
local OperateLog = utils.OperateLog
local RF_ERROR = -1

local SMTP_ERR_ACCOUNT = -1001 -- 发件人账户不存在
local SMTP_ERR_CONNECT = -1002 -- 连接服务器失败
local SMTP_ERR_QUIT = -1003 -- 邮件服务器响应失败
local SMTP_ERR_ADDRESS = -1004 -- 发件人地址错误
local SMTP_ERR_NAME_PASSWORD = -1005 -- 发件人用户名或密码错误
local SMTP_ERR_SEND_MSG = -1006 -- 邮件发送失败
local SMTP_ERR_RECEIVE_ADDRESS = -1007 -- 告警邮件接收地址错误
local SMTP_ERR_REQUIRED_AUTH = -1008 -- 发件人身份认证失败或服务器不支持匿名
local SMTP_ERR_ADDRESS_DISABLED = -1010 -- 告警邮件接收地址未使能
local smtpErrMap = {
    [SMTP_ERR_ACCOUNT] = reply_bad_request("IncorrectSenderInfo"),
    [SMTP_ERR_NAME_PASSWORD] = reply_bad_request("IncorrectSenderInfo"),
    [SMTP_ERR_ADDRESS] = reply_bad_request("IncorrectSenderInfo"),

    [SMTP_ERR_REQUIRED_AUTH] = reply_bad_request("SmtpConnectionFailed"),
    [SMTP_ERR_CONNECT] = reply_bad_request("SmtpConnectionFailed"),
    [SMTP_ERR_QUIT] = reply_bad_request("SmtpConnectionFailed"),

    [RF_ERROR] = reply_bad_request("SendingEmailFailed"),
    [SMTP_ERR_SEND_MSG] = reply_bad_request("SendingEmailFailed"),
    [SMTP_ERR_RECEIVE_ADDRESS] = reply_bad_request("SendingEmailFailed"),

    [SMTP_ERR_ADDRESS_DISABLED] = reply_bad_request("FeatureDisabledAndNotSupportOperation")
}

local SSL_UNKNOWN_ERROR = 0x00020000 -- 未知错误
local SSL_INVALID_PARAMETER = 0x00020001 -- 无效参数
local SSL_SENSOR_NOT_EXIST = 0x00020003 -- 此传感器不存在
local SSL_INVALID_DATA = 0x00020004 -- 无效数据
local SSL_SCANNING_DISABLE = 0x00020005 -- 禁止扫描
local SSL_SDR_INITIAL_FAILED = 0x00020006 -- SDR初始化失败
local SSL_INITIAL_FAILED = 0x00020007 -- 初始化失败
local SSL_IPMI_MESSAGE_SEND_FAILED = 0x00020008 -- 消息请求发送失败
local SSL_INVALID_USERNAME = 0x00020009 -- 无效的用户名
local SSL_USER_NOT_EXIST = 0x0002000A -- 该用户不存在
local SSL_FUNCTION_NOT_SUPPORT = 0x0002000B -- 功能不支持
local SSL_DEVICE_NOT_PRESENT = 0x0002000C -- 设备不在位
local SSL_INTERNAL_ERROR = 0x0002000D -- 内部错误
local SSL_CMD_RESPONSE_TIMEOUT = 0x0002000E -- 命令响应超时
local SSL_NO_DATA = 0x0002000F -- 无数据,数据为空
local SSL_WEAK_CA_MD = 0x00020010 -- 加密方式太弱
local SSL_CACERT_OVERDUED = 8 -- 证书过期

local selLogSSLErrMap = {
    [SSL_UNKNOWN_ERROR] = reply_bad_request("SSL_UNKNOWN_ERROR"),
    [SSL_INVALID_PARAMETER] = reply_bad_request("SSL_INVALID_PARAMETER"),
    [SSL_SENSOR_NOT_EXIST] = reply_bad_request("SSL_SENSOR_NOT_EXIST"),
    [SSL_INVALID_DATA] = reply_bad_request("SSL_INVALID_DATA"),
    [SSL_SCANNING_DISABLE] = reply_bad_request("SSL_SCANNING_DISABLE"),
    [SSL_SDR_INITIAL_FAILED] = reply_bad_request("SSL_SDR_INITIAL_FAILED"),
    [SSL_INITIAL_FAILED] = reply_bad_request("SSL_INITIAL_FAILED"),
    [SSL_IPMI_MESSAGE_SEND_FAILED] = reply_bad_request("SSL_IPMI_MESSAGE_SEND_FAILED"),
    [SSL_INVALID_USERNAME] = reply_bad_request("SSL_INVALID_USERNAME"),
    [SSL_USER_NOT_EXIST] = reply_bad_request("SSL_USER_NOT_EXIST"),
    [SSL_FUNCTION_NOT_SUPPORT] = reply_bad_request("SSL_FUNCTION_NOT_SUPPORT"),
    [SSL_DEVICE_NOT_PRESENT] = reply_bad_request("SSL_DEVICE_NOT_PRESENT"),
    [SSL_INTERNAL_ERROR] = reply_bad_request("SSL_INTERNAL_ERROR"),
    [SSL_CMD_RESPONSE_TIMEOUT] = reply_bad_request("SSL_CMD_RESPONSE_TIMEOUT"),
    [SSL_NO_DATA] = reply_bad_request("SSL_NO_DATA"),
    [SSL_WEAK_CA_MD] = reply_bad_request("EncryptionAlgorithmIsWeak")
}

local function selLogSSLErrMapFun()
    local errCode = "CertImportFailed" or "EncryptedCertImportFailed"
    return function(errId)
        if errId == RF_ERROR or errId == SSL_INVALID_DATA or errId == SSL_NO_DATA or errId == SSL_INTERNAL_ERROR or
            errId == SSL_INVALID_PARAMETER then
            return reply_bad_request(errCode)
        elseif errId == SSL_WEAK_CA_MD then
            return reply_bad_request("EncryptionAlgorithmIsWeak")
        elseif errId == SSL_CACERT_OVERDUED then
            return reply_bad_request("CertificateExpired")
        end
    end
end

local RF_BRDSN_STR = "BoardSN"
local RF_PRODUCT_ASSET_TAG_STR = "ProductAssetTag"
local RF_HOST_NAME_STR = "HostName"

local MGMT_SOFTWARE_TYPE_EM = 0x50 -- SMM集中管理BMC

local alarm_severities_arr = {None = 0x0, Normal = 0xf, Minor = 0xe, Major = 0xc, Critical = 0x8}
local function redfish_alarm_severities_change(data)
    if band(data, 0x01) == 1 then
        return "Normal"
    elseif band(data, 0x02) == 2 then
        return "Minor"
    elseif band(data, 0x04) == 4 then
        return "Major"
    elseif band(data, 0x08) == 8 then
        return "Critical"
    elseif data == 0 then
        return "None"
    end
end

local function SetEnabled(user, obj, Enabled)
    if Enabled == nil then
        return
    end

    return safe_call(function()
        local v = Enabled and 1 or 0
        local ok, ret = call_method(user, obj, "SetSmtpEnable", nil, gbyte(v))
        if not ok then
            return ret
        end
    end)
end

local function SetServerAddress(user, obj, data)
    if data.ServerAddress == nil then
        return
    end

    if obj.AnonymousMode:fetch() == 1 then
        if data.EmailInfo == nil or data.EmailInfo.SenderPassword == nil then
            return reply_bad_request("ModifyFailedMissingReqProperties", "Modify ServerAddress failed because SenderPassword was missing from the request")
        end
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetSmtpServer", nil, gstring(data.ServerAddress))
        if not ok then
            return ret
        end
    end)
end

local function SetServerPort(user, obj, ServerPort)
    if ServerPort == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetServerPort", nil, gint32(ServerPort))
        if not ok then
            return ret
        end
    end)
end

local function SetTLSEnabled(user, obj, TLSEnabled)
    if TLSEnabled == nil then
        return
    end

    return safe_call(function()
        local v = TLSEnabled and 1 or 0
        local ok, ret = call_method(user, obj, "SetTlsSendMode", nil, gbyte(v))
        if not ok then
            return ret
        end
    end)
end

local function SetCertVerifyEnabled(user, obj, CertVerifyEnabled)
    if CertVerifyEnabled == nil then
        return
    end

    return safe_call(function()
        local v = CertVerifyEnabled and 1 or 0
        local ok, ret = call_method(user, obj, "SetCertVerificationEnabled", nil, gbyte(v))
        if not ok then
            return ret
        end
    end)
end

local function SetAnonymousLoginEnabled(user, obj, EmailInfo)
    if EmailInfo.AnonymousLoginEnabled == nil then
        return
    end

    if obj.AnonymousMode:fetch() == 0 then
        if EmailInfo.SenderPassword == nil then
            return reply_bad_request("ModifyFailedMissingReqProperties", "Modify AnonymousMode failed because SenderPassword was missing from the request")
        end
    end

    return safe_call(function()
        local v = (not EmailInfo.AnonymousLoginEnabled) and 1 or 0
        local ok, ret = call_method(user, obj, "SetAnonymousMode", nil, gbyte(v))
        if not ok then
            return ret
        end
    end)
end

local function SetSenderUserName(user, obj, EmailInfo)
    if EmailInfo.SenderUserName == nil then
        return
    end

    if obj.AnonymousMode:fetch() == 1 then
        if EmailInfo.SenderPassword == nil then
            return reply_bad_request("ModifyFailedMissingReqProperties", "Modify SenderUserName failed because SenderPassword was missing from the request")
        end
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetLoginName", nil, gstring(EmailInfo.SenderUserName))
        if not ok then
            return ret
        end
    end)
end

local function SetSenderPassword(user, obj, SenderPassword)
    if SenderPassword == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetLoginPasswd", nil, gstring(SenderPassword))
        if not ok then
            return ret
        end
    end)
end

local function SetSenderAddress(user, obj, SenderAddress)
    if SenderAddress == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetSenderName", nil, gstring(SenderAddress))
        if not ok then
            return ret
        end
    end)
end

local function SetEmailSubject(user, obj, EmailSubject)
    if EmailSubject == nil then
        return
    end

    if EmailSubject then
        return safe_call(function()
            local ok, ret = call_method(user, obj, "SetTempletTopic", nil, gstring(EmailSubject))
            if not ok then
                return ret
            end
        end)
    end
end

local function SetTempletIpaddr(user, obj, HostName)
    if HostName == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetTempletIpaddr", nil, gbyte(HostName))
        if not ok then
            return ret
        end
    end)
end

local function SetTempletBoardSn(user, obj, BoardSN)
    if BoardSN == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetTempletBoardSn", nil, gbyte(BoardSN))
        if not ok then
            return ret
        end
    end)
end

local function SetTempletAsset(user, obj, ProductAssetTag)
    if ProductAssetTag == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetTempletAsset", nil, gbyte(ProductAssetTag))
        if not ok then
            return ret
        end
    end)
end

local function GetEmailSubjects(EmailSubjectContains)
    local HostName = 0
    local BoardSN = 0
    local ProductAssetTag = 0
    for _, v in ipairs(EmailSubjectContains) do
        if v == RF_HOST_NAME_STR then
            HostName = 1
        elseif v == RF_BRDSN_STR then
            BoardSN = 1
        elseif v == RF_PRODUCT_ASSET_TAG_STR then
            ProductAssetTag = 1
        end
    end
    return HostName, BoardSN, ProductAssetTag
end

local function SetAlarmSeverity(user, obj, AlarmSeverity)
    if AlarmSeverity == nil then
        return
    end

    return safe_call(function()
        local v = alarm_severities_arr[AlarmSeverity]
        local ok, ret = call_method(user, obj, "SetSendSeverity", nil, gbyte(v))
        if not ok then
            return ret
        end
    end)
end

local function SetItemEnable(user, obj, Enabled)
    if Enabled == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetItemEnable", nil, gbyte(Enabled and 1 or 0))
        if not ok then
            return ret
        end
    end)
end

local function SetEmailName(user, obj, EmailAddress)
    if EmailAddress == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetEmailName", nil, gstring(EmailAddress))
        if not ok then
            return ret
        end
    end)
end

local function SetEmailDesc(user, obj, Description)
    if Description == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetEmailDesc", nil, gstring(Description))
        if not ok then
            return ret
        end
    end)
end

local function SetSmtpItemCfg(user, obj, result, v)
    if not v then
        return
    end

    result:join(SetItemEnable(user, obj, v.Enabled))
    result:join(SetEmailName(user, obj, v.EmailAddress))
    result:join(SetEmailDesc(user, obj, v.Description))
end

local function GetRecipientList()
    return C.SmtpItemCfg():fold(function(obj, acc)
        acc[#acc + 1] = {
            ID = obj.IndexNum:fetch(),
            Enabled = obj.ItemEnable:fetch() == 1,
            EmailAddress = obj.EmailName:neq(""):fetch_or(null),
            Description = obj.EmailDesc:fetch()
        }
        return acc
    end, {}):fetch()
end

local function GetEmailSubjectContains(obj)
    local EmailSubjectContains = {}
    local software_type = O.PRODUCT.MgmtSoftWareType:fetch()
    if software_type ~= MGMT_SOFTWARE_TYPE_EM then
        obj.TempletIpaddr:eq(1):next(function()
            EmailSubjectContains[#EmailSubjectContains + 1] = RF_HOST_NAME_STR
        end):catch()
        obj.TempletBoardSn:eq(1):next(function()
            EmailSubjectContains[#EmailSubjectContains + 1] = RF_BRDSN_STR
        end):catch()
        obj.TempletAsset:eq(1):next(function()
            EmailSubjectContains[#EmailSubjectContains + 1] = RF_PRODUCT_ASSET_TAG_STR
        end):catch()
    end
    return EmailSubjectContains
end

local M = {}

local function get_cert_info_from_output(output_list)
    return {
        Issuer = output_list[1],
        Subject = output_list[2],
        ValidNotBefore = output_list[3],
        ValidNotAfter = output_list[4],
        SerialNumber = output_list[5]
    }
end

local function GetRootCertificate(user, obj)
    local ok, ret = call_method(user, obj, "GetCertInfo", selLogSSLErrMap)
    if not ok then
        logging:error("get RootCertificate failed: %s", ret)
        return
    end

    local RootCertificate = get_cert_info_from_output(ret)
    return RootCertificate
end

function M.GetEmailNotification(user)
    return C.SmtpConfig[0]:next(function(obj)
        local TLSEnabled = obj.TlsSendMode:fetch() == 1
        local RootCertificate = nil
        local CertVerificationEnabled = nil
        if TLSEnabled == true then
            CertVerificationEnabled = obj.CertVerificationEnabled:fetch() == 1
            RootCertificate = GetRootCertificate(user, obj)
        end

        return {
            Enabled = obj.SmtpEnable:fetch() == 1,
            ServerAddress = rf_string_check(obj.SmtpServer:fetch()),
            ServerPort = obj.ServerPort:fetch(),
            TLSEnabled = TLSEnabled,
            CertVerificationEnabled = CertVerificationEnabled,
            RootCertificate = RootCertificate,
            EmailInfo = {
                AnonymousLoginEnabled = not (obj.AnonymousMode:fetch() == 1),
                SenderUserName = rf_string_check(obj.LoginName:fetch()),
                SenderAddress = rf_string_check(obj.SenderName:fetch()),
                EmailSubject = rf_string_check(obj.TempletTopic:fetch()),
                EmailSubjectContains = GetEmailSubjectContains(obj),
                AlarmSeverity = redfish_alarm_severities_change(obj.SendSeverity:fetch())
            },
            RecipientList = GetRecipientList()
        }
    end):fetch()
end

function M.UpdateEmailNotification(data, user)
    return C.SmtpConfig[0]:next(function(obj)
        local result = reply_ok()

        result:join(SetEnabled(user, obj, data.Enabled))
        result:join(SetServerPort(user, obj, data.ServerPort))
        result:join(SetTLSEnabled(user, obj, data.TLSEnabled))
        result:join(SetCertVerifyEnabled(user, obj, data.CertVerificationEnabled))
        if data.EmailInfo then
            local Em = data.EmailInfo
            result:join(SetAnonymousLoginEnabled(user, obj, Em))
            result:join(SetSenderUserName(user, obj, Em))
            result:join(SetSenderPassword(user, obj, Em.SenderPassword))
            result:join(SetSenderAddress(user, obj, Em.SenderAddress))
            result:join(SetEmailSubject(user, obj, Em.EmailSubject))
            if Em.EmailSubjectContains then
                local HostName, BoardSN, ProductAssetTag = GetEmailSubjects(Em.EmailSubjectContains)
                result:join(SetTempletIpaddr(user, obj, HostName))
                result:join(SetTempletBoardSn(user, obj, BoardSN))
                result:join(SetTempletAsset(user, obj, ProductAssetTag))
            end
            result:join(SetAlarmSeverity(user, obj, Em.AlarmSeverity))
        end
        result:join(SetServerAddress(user, obj, data))

        if data.RecipientList then
            local tmp = {}
            for _, v in ipairs(data.RecipientList) do
                tmp[v.ID] = v
            end
            C.SmtpItemCfg():fold(function(smtpItemCfgObj, id)
                SetSmtpItemCfg(user, smtpItemCfgObj, result, tmp[id])
                return id + 1
            end, 0):fetch()
        end

        if result:isOk() then
            return reply_ok_encode_json(M.GetEmailNotification(user))
        end
        result:appendErrorData(M.GetEmailNotification(user))
        return result
    end):fetch()
end

function M.TestEmailNotification(data, user)
    local count = dflib.object_count("SmtpItemCfg")
    if data.ID >= count or data.ID < 0 then
        OperateLog(user, "Test smtp send email address failed")
        return reply_bad_request(nil, "invalid smtp id")
    end

    return C.SmtpConfig[0]:next(function(obj)
        local ok, ret = call_method(user, obj, "SendTest", smtpErrMap, gbyte(data.ID))
        if not ok then
            return ret
        end
        return reply_ok()
    end):fetch()
end

function M.ImportEmailRootCertificate(data)
    local user = data.user
    if utils.CheckFormData(data) == false then
        logging:error('Failed to import email root certificate')
        OperateLog(user, "Import smtp server root certificate failed")
        return reply_bad_request('InvalidParam')
    end
    local filePath = utils.GetFormDataFilePath(data)
    if not filePath then
        OperateLog(user, "Import smtp server root certificate failed")
        return reply_bad_request("InvalidFile")
    end

    return C.SmtpConfig[0]:next(function(obj)
        local input = {gstring(filePath)}
        local errMap = selLogSSLErrMapFun()
        local ok, ret = call_method(user, obj, "ImportCert", errMap, input)
        if not ok then
            return ret
        end
        return reply_ok()
    end):fetch()
end

return M
