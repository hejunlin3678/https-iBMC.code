local utils = require "utils"
local dflib = require "dflib"
local c = require "dflib.core"
local cjson = require "cjson"
local http = require "http"
local logging = require "logging"
local C = dflib.class
local O = dflib.object
local null = cjson.null
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local guint32 = GVariant.new_uint32
local gstring = GVariant.new_string
local call_method = utils.call_method
local HasUserMgntPrivilege = utils.HasUserMgntPrivilege
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_bad_request = http.reply_bad_request
local reply_forbidden = http.reply_forbidden
local reply_not_found = http.reply_not_found
local reply_ok = http.reply_ok
local safe_call = http.safe_call

local DISABLE = 0

local ROOT_CERT_TYPE = 0
local CLIENT_CERT_TYPE = 1

local PME_SERVICE_SUPPORT = 1 -- PME支持该服务

local RFERR_INSUFFICIENT_PRIVILEGE = 403
local RFERR_NO_RESOURCE = 404
local MUTUAL_AUTH_STATE_ERR = 1

local ERR_ID_BMC_GLOBAL = 0x7FFF0000
local ERR_ID_BMC_GLOBAL_MA = 0x0100

local MIN_USER_ID = 2 -- 最大IPMI用户ID
local MAX_USER_ID = 17 -- 最小IPMI用户ID

local CRL_FILE_NOT_EXIST = 1 -- crl文件不存在

local VOS_ERR_MA_E = {
    VOS_ERR_MA_IDEL = ERR_ID_BMC_GLOBAL + ERR_ID_BMC_GLOBAL_MA, -- 0x7FFF0100
    VOS_ERR_MA_REACH_MAX = 0x7FFF0101, -- 证书达到上限
    VOS_ERR_MA_CERT_EXSIT = 0x7FFF0102, -- 证书已经存在
    VOS_ERR_MA_FORMAT_ERR = 0x7FFF0103, -- 证书格式错误
    VOS_ERR_MA_NO_ISSUER = 0x7FFF0104, -- 未找到签发者
    VOS_ERR_MA_BASIC_CONSTRAINTS_ERR = 0x7FFF0105, -- 证书基本约束不满足
    VOS_ERR_MA_CERT_ID_OUT_OF_RANGE = 0x7FFF0106,  -- 证书ID超限
    VOS_ERR_MA_CERT_NOT_EXIST = 0x7FFF0107, -- 指定证书不存在
    VOS_ERR_MA_KEY_USAGE_CERT_SIGN_ERR = 0x7FFF0108, -- 证书密钥用法错误
    VOS_ERR_MA_CERT_NO_MATCH_DEVICE = 0x7FFF0109, -- 证书与设备不匹配
    VOS_ERR_MA_CERT_EXPIRE = 0x7FFF010A -- 证书过期
};

-- 导入CRL远程方法的错误返回码
local IMPORT_CRL_ERRORCODE_E = {
    IMPORT_CRL_ERR_FORMAT_ERR = 0x100, -- 格式错误，无法解析出CRL对象
    IMPORT_CRL_ERR_ISSUER_VERIFY_FAIL = 0x101, -- 进行CRL发行者签名校验失败
    IMPORT_CRL_ERR_ROOT_CERT_NOT_IMPORT = 0x102, -- 对应根证书未导入的错误
    IMPORT_CRL_ERR_ISSUING_DATE_INVALID = 0x103,
    IMPORT_CRL_ERR_NOT_SUPPORT_CRL_SIGN = 0x104
};

local ENABLE_MUTUAL_AUTHENTICATIONS_ERR_WITH_TLS = -4 -- TLS版本不满足
local RET_ERR = -1 -- 远程rpc方法调用返回-1时报错导入证书失败

local errMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("PropertyModificationNeedPrivilege"),
    [RFERR_NO_RESOURCE] = reply_bad_request("ResourceMissingAtURI"),
    [MUTUAL_AUTH_STATE_ERR] = reply_bad_request("RootCertificateInUse"),
    [VOS_ERR_MA_E.VOS_ERR_MA_REACH_MAX] = reply_bad_request("CertificateQtyReachLimit"),
    [VOS_ERR_MA_E.VOS_ERR_MA_CERT_EXSIT] = reply_bad_request("RootCertificateAlreadyExists"),
    [VOS_ERR_MA_E.VOS_ERR_MA_FORMAT_ERR] = reply_bad_request("CertificateFormatError"),
    [VOS_ERR_MA_E.VOS_ERR_MA_BASIC_CONSTRAINTS_ERR] = reply_bad_request("RootCertLackProperties"),
    [VOS_ERR_MA_E.VOS_ERR_MA_NO_ISSUER] = reply_bad_request('FailedEnableTwoFactorCertification'),
    [VOS_ERR_MA_E.VOS_ERR_MA_CERT_NOT_EXIST] = reply_bad_request("RootCANotExists"),
    [VOS_ERR_MA_E.VOS_ERR_MA_KEY_USAGE_CERT_SIGN_ERR] = reply_bad_request("RootCertLackProperties"),
    [VOS_ERR_MA_E.VOS_ERR_MA_CERT_NO_MATCH_DEVICE] = reply_bad_request("CertificateNotMatchDevice"),
    [VOS_ERR_MA_E.VOS_ERR_MA_CERT_EXPIRE] = reply_bad_request('CertificateExpired'),
    [ENABLE_MUTUAL_AUTHENTICATIONS_ERR_WITH_TLS] = reply_bad_request('TwoFactorCertificationFailedWithTLS'),
    [RET_ERR] = reply_bad_request("CertMismatch")
}

local clientCertErrMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("PropertyModificationNeedPrivilege"),
    [RFERR_NO_RESOURCE] = reply_bad_request("ResourceMissingAtURI"),
    [MUTUAL_AUTH_STATE_ERR] = reply_bad_request("RootCertificateInUse"),
    [VOS_ERR_MA_E.VOS_ERR_MA_REACH_MAX] = reply_bad_request("ClientCertQtyExceedLimit"),
    [VOS_ERR_MA_E.VOS_ERR_MA_CERT_EXSIT] = reply_bad_request("ClientCertificateAlreadyExists"),
    [VOS_ERR_MA_E.VOS_ERR_MA_FORMAT_ERR] = reply_bad_request("CertificateFormatError"),
    [VOS_ERR_MA_E.VOS_ERR_MA_BASIC_CONSTRAINTS_ERR] = reply_bad_request("ClientCertLackProperties"),
    [VOS_ERR_MA_E.VOS_ERR_MA_NO_ISSUER] = reply_bad_request("ClientCertIssuerMismatch"),
    [VOS_ERR_MA_E.VOS_ERR_MA_CERT_NOT_EXIST] = reply_bad_request("RootCANotExists"),
    [VOS_ERR_MA_E.VOS_ERR_MA_KEY_USAGE_CERT_SIGN_ERR] = reply_bad_request("RootCertLackProperties"),
    [VOS_ERR_MA_E.VOS_ERR_MA_CERT_NO_MATCH_DEVICE] = reply_bad_request("CertificateNotMatchDevice"),
    [VOS_ERR_MA_E.VOS_ERR_MA_CERT_EXPIRE] = reply_bad_request('CertificateExpired'),
    [RET_ERR] = reply_bad_request("CertMismatch")
}

local importCRLErrMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("InsufficientPrivilege"),
    [IMPORT_CRL_ERRORCODE_E.IMPORT_CRL_ERR_FORMAT_ERR] = reply_bad_request("CrlFileFormatError"),
    [IMPORT_CRL_ERRORCODE_E.IMPORT_CRL_ERR_ISSUER_VERIFY_FAIL] = reply_bad_request("RootCertificateMismatch"),
    [IMPORT_CRL_ERRORCODE_E.IMPORT_CRL_ERR_ROOT_CERT_NOT_IMPORT] = reply_bad_request("RootCertificateNotImported"),
    [IMPORT_CRL_ERRORCODE_E.IMPORT_CRL_ERR_ISSUING_DATE_INVALID] = reply_bad_request("CrlIssuingDateInvalid"),
    [IMPORT_CRL_ERRORCODE_E.IMPORT_CRL_ERR_NOT_SUPPORT_CRL_SIGN] = reply_bad_request("CANotSupportCrlSignature"),
    [RET_ERR] = reply_bad_request("CertMismatch")
}

local deleteCRLErrMap = {
    [CRL_FILE_NOT_EXIST] = reply_not_found("CRLFileNotExist")
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

local function UserRoleIdToStr(id)
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
        return privArrLdap[15]
    end
end

local function GetRootCert()
    local rootCert = C.MutualAuthenticationRootCert():fold(
        function(obj, acc)
            if #(obj.SerialNumber:fetch()) == 0 then
                return acc
            end
            acc[#acc + 1] = {
                ID = obj.CertId:fetch(),
                IssueBy = obj.IssueBy:fetch(),
                IssueTo = obj.IssueTo:fetch(),
                ValidFrom = obj.ValidFrom:fetch(),
                ValidTo = obj.ValidTo:fetch(),
                SerialNumber = obj.SerialNumber:fetch(),
                CrlState = obj.CRLFilePath:next(function(state)
                    if state == "" then
                        return false
                    else
                        return true
                    end
                end):fetch(),
                CrlValidFrom = obj.CRLStartTime:fetch(),
                CrlValidTo = obj.CRLExpireTime:fetch()
            }
            return acc
        end, {}):fetch()
    return rootCert
end

local function GetClentCert(user)
    local clientCert = C.User():fold(function(userObj, acc)
        if #(userObj.UserName:fetch()) == 0 then
            return acc
        end
        local obj = C.MutualAuthenticationCliCert("hashId", userObj.UserId:fetch())
        acc[#acc + 1] = {
            UserID = userObj.UserId:fetch(),
            UserName = userObj.UserName:fetch(),
            RoleID = userObj.UserRoleId:next(UserRoleIdToStr):fetch(),
            RootCertUploadedState = obj.IssuerId:next(function(issuerId)
                if issuerId > 0 then
                    return true
                else
                    return false
                end
            end):fetch(),
            RevokedState = obj.RevokedState:next(IntToBool):fetch(),
            RevokedDate = obj.RevokedDate:next(function(date)
                if date == nil or #date == 0 or date == "N/A" or date == "Unknown" then
                    return null
                else
                    return date
                end
            end):fetch(),
            FingerPrint = obj.FingerPrint:fetch(),
            SerialNumber = obj.SerialNumber:fetch()
        }
        if obj.SigAlgorithm:fetch() == "" then
            call_method(user, obj, "ReInitClientCertificate", errMap, gbyte(userObj.UserId:fetch()),
                guint32(obj.hashValue:fetch()))
        end
        return acc
    end, {}):fetch()
    return clientCert
end

local function DeleteRootCertificate(data, user)
    if O.PRODUCT.MutualAuthenticationEnable:fetch() == 0 then
        return reply_bad_request("2FANotSupported")
    end

    return C.MutualAuthenticationRootCert("CertId", data.ID):next(
        function(certObj)
            if certObj.SerialNumber:fetch() == "" then
                return reply_bad_request("RootCertificateNotExist")
            end

            local input = gbyte(data.ID)
            local ok, ret = call_method(user, certObj, "DeleteRootCertificate", errMap, input)
            if not ok then
                return ret
            end
        end):fetch()
end

local function DeleteClientCertificate(data, user)
    if O.PRODUCT.MutualAuthenticationEnable:fetch() == 0 then
        return reply_bad_request("ClientCertDeleteNotSupported")
    end
    local userObj = C.User("UserId", data.ID)
    if userObj.UserName:fetch() == "" then
        return reply_bad_request("ResourceMissingAtURI")
    end

    return C.MutualAuthenticationCliCert("hashId", data.ID):next(
        function(certObj)
            if certObj.SerialNumber:fetch() == "" then
                return reply_bad_request("ClientCertificateNotExist")
            end

            local input = gbyte(data.ID)
            local ok, ret = call_method(user, certObj, "DeleteClientCertificate", errMap, input)
            if not ok then
                return ret
            end
        end):fetch()
end

local M = {}

function M.GetTwoFactorAuthentication(user)
    logging:debug(" GetTwoFactorAuthentication ")
    local rootCertArray = GetRootCert()
    local clientCertArray = GetClentCert(user)
    local rootCertcnt = #rootCertArray
    local clientCertcnt = #clientCertArray
    local twoFactorAuthObj = O.MutualAuthentication
    return {
        TwoFactorAuthenticationEnabled = twoFactorAuthObj.MutualAuthenticationState:next(IntToBool):fetch(),
        OCSPEnabled = twoFactorAuthObj.MutualAuthenticationOCSP:next(IntToBool):fetch(),
        CRLEnabled = twoFactorAuthObj.MutualAuthenticationCRL:next(IntToBool):fetch(),
        RootCertificateMembers = rootCertArray,
        RootCertificateCount = rootCertcnt,
        ClientCertificateMembers = clientCertArray,
        ClientCertificateCount = clientCertcnt
    }
end

function M.SetTwoFactorAuthentication(user, data)
    logging:debug(" SetTwoFactorAuthentication ")
    if C.SecurityEnhance[0].SystemLockDownStatus:fetch() ~= DISABLE and
        C.PMEServiceConfig[0].SystemLockDownSupport:fetch() == PME_SERVICE_SUPPORT then
        return reply_bad_request("SystemLockdownForbid")
    end

    if O.PRODUCT.MutualAuthenticationEnable:fetch() == 0 then
        return reply_bad_request("2FANotSupported")
    end

    local result = reply_ok()
    result:join(safe_call(function()
        return O.MutualAuthentication:next(function(obj)
            if data.TwoFactorAuthenticationEnabled ~= nil then
                local input = gbyte(BoolToInt(data.TwoFactorAuthenticationEnabled))
                local ok, ret = call_method(user, obj, "SetMutualAuthenticationState", errMap, input)
                if not ok then
                    return ret
                end
            end

            if data.OCSPEnabled ~= nil then
                local input = gbyte(BoolToInt(data.OCSPEnabled))
                local ok, ret = call_method(user, obj, "SetMutualAuthenticationOCSP", errMap, input)
                if not ok then
                    return ret
                end
            end

            if data.CRLEnabled ~= nil then
                local input = gbyte(BoolToInt(data.CRLEnabled))
                local ok, ret = call_method(user, obj, "SetMutualAuthenticationCRL", errMap, input)
                if not ok then
                    return ret
                end
            end
        end):fetch()
    end))

    if result:isOk() then
        return reply_ok_encode_json(M.GetTwoFactorAuthentication(user))
    end
    result:appendErrorData(M.GetTwoFactorAuthentication(user))
    return result
end

function M.ImportClentCertificate(formdata)
    local userID = tonumber(formdata.fields.UserID)
    local filePath = utils.GetFormDataFilePath(formdata)

    if utils.CheckFormData(formdata) == false then
        logging:error('Failed to import clent certificate')
        if filePath then
            c.rm_filepath(filePath)
        end
        return reply_bad_request('InvalidParam')
    end

    if not filePath then
        return reply_bad_request("InvalidFile")
    end
    if not userID then
        c.rm_filepath(filePath)
        return reply_bad_request("InvalidParam")
    end
    if userID < MIN_USER_ID or userID > MAX_USER_ID then
        c.rm_filepath(filePath)
        return reply_bad_request("ResourceMissingAtURI")
    end

    local userObj = C.User("UserId", userID)
    local user = formdata.user
    if userObj.UserName:fetch() == "" then
        c.rm_filepath(filePath)
        return reply_bad_request("ResourceMissingAtURI")
    end

    if O.PRODUCT.MutualAuthenticationEnable:fetch() == 0 then
        c.rm_filepath(filePath)
        return reply_bad_request("ClientCertImportNotSupported")
    end

    local result = C.MutualAuthenticationCliCert("hashId", userID):next(
        function(certObj)
            local ok, ret = call_method(user, certObj, "ImportClientCertificate", clientCertErrMap, gbyte(userID),
                gstring(filePath))
            if not ok then
                logging:error(" ImportClientCert error ")
                return ret
            end
        end):fetch()
    if result then
        return result
    end

    return reply_ok()
end

function M.ImportRootCertificate(formdata)
    local rootCertObj
    local filePath = utils.GetFormDataFilePath(formdata)

    if utils.CheckFormData(formdata) == false then
        logging:error('Failed to import root certificate')
        if filePath then
            c.rm_filepath(filePath)
        end
        return reply_bad_request('InvalidParam')
    end
    if not filePath then
        return reply_bad_request("InvalidFile")
    end

    local certId = C.MutualAuthenticationRootCert():fold(function(certObj)
        if certObj.SerialNumber:fetch() == "" then
            rootCertObj = certObj
            return certObj.CertId:fetch(), false
        end
    end):fetch()
    local user = formdata.user
    if certId == nil then
        c.rm_filepath(filePath)
        return reply_bad_request("CertificateQtyReachLimit")
    end

    local ok, ret = call_method(user, rootCertObj, "ImportRootCertificate", errMap, gbyte(certId),
        gstring(filePath))
    if not ok then
        logging:error(" ImportRootCertificate error ")
        return ret
    end

    return reply_ok()
end

function M.DeleteCertificate(user, data)
    logging:debug(" DeleteCertificate CertificateType:%d, CertID:%d", data.CertificateType, data.ID)
    local ret
    if data.CertificateType == ROOT_CERT_TYPE then
        ret = DeleteRootCertificate(data, user)
    elseif data.CertificateType == CLIENT_CERT_TYPE then
        ret = DeleteClientCertificate(data, user)
    else
        return reply_bad_request()
    end
    if ret then
        return ret
    end

    logging:info(" DeleteCertificate sucess")
    return reply_ok()
end

function M.ImportCrlCertificate(formdata)
    local certID = tonumber(formdata.fields.CertID)
    local user = formdata.user
    local filePath = utils.GetFormDataFilePath(formdata)

    if utils.CheckFormData(formdata) == false then
        if filePath then
            c.rm_filepath(filePath)
        end
        logging:error('Failed to crl certificate')
        return reply_bad_request('InvalidParam')
    end

    if not filePath then
        return reply_bad_request("InvalidFile")
    end

    if not certID then
        c.rm_filepath(filePath)
        return reply_bad_request("InvalidParam")
    end

    local result = C.MutualAuthenticationRootCert("CertId", certID):next(
        function(certObj)
            if certObj.SerialNumber:fetch() == "" then
                return reply_bad_request("RootCertificateIdInvalid")
            end

            local ok, ret = call_method(user, certObj, "ImportCRL", importCRLErrMap, gbyte(certID), gstring(filePath))
            if not ok then
                logging:error(" ImportRootCert error ")
                return ret
            end
        end):fetch()
    if result then
        return result
    end

    return reply_ok()
end

function M.DeleteCrlCertificate(certID, user)
    if certID < 1 or certID > 16 then
        return reply_not_found('ResourceMissingAtURI')
    end
    if not HasUserMgntPrivilege(user) then
        return reply_forbidden("PropertyModificationNeedPrivilege")
    end

    local result = C.MutualAuthenticationRootCert("CertId", certID):next(
        function(certObj)
            if certObj.SerialNumber:fetch() == "" then
                return reply_bad_request("RootCertificateIdInvalid")
            end

            local ok, ret = call_method(user, certObj, "DeleteCRL", deleteCRLErrMap, gbyte(certID))
            if not ok then
                logging:error(" DeleteRootCert error ")
                return ret
            end
        end):fetch()
    if result then
        return result
    end

    return reply_ok()
end

return M
