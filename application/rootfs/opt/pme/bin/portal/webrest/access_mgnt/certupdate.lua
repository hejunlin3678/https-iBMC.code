local http = require 'http'
local cjson = require 'cjson'
local dflib = require 'dflib'
local utils = require 'utils'
local c = require "dflib.core"
local cfg = require 'config'
local null = cjson.null
local C = dflib.class
local O = dflib.object
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local guint32 = GVariant.new_uint32
local gstring = GVariant.new_string
local call_method = utils.call_method
local logging = require 'logging'
local reply_ok = http.reply_ok
local rf_string_check = utils.rf_string_check

local reply_bad_request = http.reply_bad_request
local real_file_path = utils.real_file_path
local is_tmp_web_path = utils.is_tmp_web_path

-- Web服务模块错误码定义
local RF_ERROR = -1
local SSL_WEAK_CA_MD = 0x00020010        -- 加密方式太弱--MD5
local SSL_WEAK_CA_SHA1 = 0x00020011      -- 加密方式太弱--SHA1

-- 证书导入错误码定义
local ERR_ID_BMC_GLOBAL = 0x7FFF0000
local ERR_ID_BMC_GLOBAL_MA = 0x0100
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
    VOS_ERR_MA_CERT_EXPIRE = 0x7FFF010A, -- 证书过期
    VOS_ERR_MA_FILE_PATH_INVALILD = 0x7FFF010B, -- 证书文件路径无效
    VOS_ERR_MA_FILE_LEN_EXCEED = 0x7FFF010C,  -- 证书文件大小过大
    VOS_ERR_MA_PWD_LEN_INVALID = 0x7FFF010D   -- 证书密码长度不合法
};

local CACertErrMap = {
    [RF_ERROR] = reply_bad_request("CertImportFailed"),
    [VOS_ERR_MA_E.VOS_ERR_MA_REACH_MAX] = reply_bad_request("CertQtyExceedLimit"),
    [VOS_ERR_MA_E.VOS_ERR_MA_CERT_EXSIT] = reply_bad_request("CertificateAlreadyExists"),
    [VOS_ERR_MA_E.VOS_ERR_MA_FORMAT_ERR] = reply_bad_request("CACertificateFormatError"),
    [VOS_ERR_MA_E.VOS_ERR_MA_NO_ISSUER] = reply_bad_request("CertIssuerMismatch"),
    [VOS_ERR_MA_E.VOS_ERR_MA_BASIC_CONSTRAINTS_ERR] = reply_bad_request("CACertificateLackProperties"),
    [VOS_ERR_MA_E.VOS_ERR_MA_CERT_NOT_EXIST] = reply_bad_request("RootCANotExists"),
    [VOS_ERR_MA_E.VOS_ERR_MA_KEY_USAGE_CERT_SIGN_ERR] = reply_bad_request("RootCertLackProperties"),
    [VOS_ERR_MA_E.VOS_ERR_MA_CERT_NO_MATCH_DEVICE] = reply_bad_request("CertificateNotMatchDevice"),
    [VOS_ERR_MA_E.VOS_ERR_MA_CERT_EXPIRE] = reply_bad_request('CertificateExpired'),
    [VOS_ERR_MA_E.VOS_ERR_MA_FILE_PATH_INVALILD] = reply_bad_request("CertImportFailed"),
    [VOS_ERR_MA_E.VOS_ERR_MA_FILE_LEN_EXCEED] = reply_bad_request("CertImportFailed")
}

local IMPORT_CRL_ERROR_CODE = {
    IMPORT_CRL_ERR_FORMAT_ERR = 0x100,   -- 格式错误，无法解析出CRL对象
    IMPORT_CRL_ERR_ISSUER_VERIFY_FAIL = 0x101,   -- 进行CRL发行者签名校验失败
    IMPORT_CRL_ERR_ROOT_CERT_NOT_IMPORT = 0x102, -- 对应根证书未导入的错误
    IMPORT_CRL_ERR_ISSUING_DATE_INVALID = 0x103, -- 对应CRL发行日期无效
    IMPORT_CRL_ERR_NOT_SUPPORT_CRL_SIGN = 0x104  -- 根证书不支持吊销列表签名
}

local CrlErrMap = {
    [RF_ERROR] = reply_bad_request("InternalError"),
    [IMPORT_CRL_ERROR_CODE.IMPORT_CRL_ERR_FORMAT_ERR] = reply_bad_request("CrlFileFormatError"),
    [IMPORT_CRL_ERROR_CODE.IMPORT_CRL_ERR_ISSUER_VERIFY_FAIL] = reply_bad_request("RootCertificateMismatch"),
    [IMPORT_CRL_ERROR_CODE.IMPORT_CRL_ERR_ROOT_CERT_NOT_IMPORT] = reply_bad_request("RootCertificateNotImported"),
    [IMPORT_CRL_ERROR_CODE.IMPORT_CRL_ERR_ISSUING_DATE_INVALID] = reply_bad_request("CrlIssuingDateInvalid"),
    [IMPORT_CRL_ERROR_CODE.IMPORT_CRL_ERR_NOT_SUPPORT_CRL_SIGN] = reply_bad_request("CANotSupportCrlSignature"),
    [VOS_ERR_MA_E.VOS_ERR_MA_FILE_PATH_INVALILD] = reply_bad_request("CrlImportFailed"),
    [VOS_ERR_MA_E.VOS_ERR_MA_FILE_LEN_EXCEED] = reply_bad_request("CrlImportFailed")
}

local DELETE_CRL_NOT_EXISTED_ERR = 0x7FFF0108 -- CRL文件不存在
local deleteCrlErrMap = {
    [DELETE_CRL_NOT_EXISTED_ERR] = reply_bad_request("CrlNotExisted")
} 

local ClientCertErrMap = {
    [RF_ERROR] = reply_bad_request("EncryptedCertImportFailed"),
    [VOS_ERR_MA_E.VOS_ERR_MA_FORMAT_ERR] = reply_bad_request("CertificateFormatError"),
    [VOS_ERR_MA_E.VOS_ERR_MA_CERT_EXPIRE] = reply_bad_request('CertificateExpired'),
    [VOS_ERR_MA_E.VOS_ERR_MA_FILE_PATH_INVALILD] = reply_bad_request("EncryptedCertImportFailed"),
    [VOS_ERR_MA_E.VOS_ERR_MA_FILE_LEN_EXCEED] = reply_bad_request("EncryptedCertImportFailed"),
    [VOS_ERR_MA_E.VOS_ERR_MA_PWD_LEN_INVALID] = reply_bad_request("InvalidPasswordLength"),
    [SSL_WEAK_CA_MD] = reply_bad_request("EncryptionAlgorithmIsWeak"),
    [SSL_WEAK_CA_SHA1] = reply_bad_request("EncryptionAlgorithmIsWeak")
}

local M = {}

local WRONG_CASERVER_ADDRESS_FORMAT = -2
local errMap = {
    [WRONG_CASERVER_ADDRESS_FORMAT] = reply_bad_request("CAServerAddrWrongFormat")
}

local WRONG_CERT_UPDATE_CONFIG = -1
local errMapUpdate = {
    [WRONG_CERT_UPDATE_CONFIG] = reply_bad_request("UpdateCertFailed")
}

local function SetCMPCfgCAServerAddr(data, user)
    if data.CMPConfig.CAServerAddr ~= nil then
        local ok, ret = call_method(user, C.CMPConfig[0]:next():fetch(), "SetCMPCfgCAServerAddr", errMap,
            gstring(data.CMPConfig.CAServerAddr))
        if not ok then
            return ret
        end
    end
    return
end

local function SetCMPCfgCAServerPort(data, user)
    if data.CMPConfig.CAServerPort ~= nil then
        local ok, ret = call_method(user, C.CMPConfig[0]:next():fetch(), "SetCMPCfgCAServerPort", errMap,
            guint32(data.CMPConfig.CAServerPort))
        if not ok then
            return ret
        end
    end
    return
end

local function SetCMPCfgCAServerCMPPath(data, user)
    if data.CMPConfig.CAServerCMPPath ~= nil then
        local ok, ret = call_method(user, C.CMPConfig[0]:next():fetch(), "SetCMPCfgCAServerCMPPath", errMap,
            gstring(data.CMPConfig.CAServerCMPPath))
        if not ok then
            return ret
        end
    end
    return
end

local function SetCMPCfgTLSAuthType(data, user)
    local auth_type
    if data.CMPConfig.TLSAuthType ~= nil then
        if data.CMPConfig.TLSAuthType == "one-way" then
            auth_type = 1
        else
            auth_type = 2
        end

        local ok, ret = call_method(user, C.CMPConfig[0]:next():fetch(), "SetCMPCfgTLSAuthType", errMap,
            gbyte(auth_type))
        if not ok then
            return ret
        end
    end
    return
end

local function SetCMPCfgAutoUpdateEnabled(data, user)
    local auto_update_enabled
    if data.CMPConfig.AutoUpdateEnabled ~= nil then
        if data.CMPConfig.AutoUpdateEnabled == true then
            auto_update_enabled = 1
        else
            auto_update_enabled = 0
        end

        local ok, ret = call_method(user, C.CMPConfig[0]:next():fetch(), "SetCMPCfgAutoUpdateEnabled", errMap,
            gbyte(auto_update_enabled))
        if not ok then
            return ret
        end
    end
    return
end

local function SetCMPCfgSubjectInfo(data, user)
    if data.CMPConfig.SubjectInfo ~= nil then
        local obj_cmpconfig = O.CMPConfig

        local state = data.CMPConfig.SubjectInfo.State or obj_cmpconfig.State:fetch_or()
        local location = data.CMPConfig.SubjectInfo.Location or obj_cmpconfig.Location:fetch_or()
        local orgname = data.CMPConfig.SubjectInfo.OrgName or obj_cmpconfig.OrgName:fetch_or()
        local orgunit = data.CMPConfig.SubjectInfo.OrgUnit or obj_cmpconfig.OrgUnit:fetch_or()
        local internalname = data.CMPConfig.SubjectInfo.InternalName or obj_cmpconfig.InternalName:fetch_or()
        local email = data.CMPConfig.SubjectInfo.Email or obj_cmpconfig.Email:fetch_or()

        local ok, ret = call_method(user, C.CMPConfig[0]:next():fetch(), "SetCMPCfgSubjectInfo", errMap,
            gstring(data.CMPConfig.SubjectInfo.Country),
            gstring(state),
            gstring(location),
            gstring(orgname),
            gstring(orgunit),
            gstring(data.CMPConfig.SubjectInfo.CommonName),
            gstring(internalname),
            gstring(email))
        if not ok then
            return ret
        end
    end
    return
end

local function GetCACertInfo()
    local ca_cert = O.CAServerCACert
    local crl_path = ca_cert.CRLFilePath:fetch()
    local is_import_crl = string.len(crl_path) ~= 0

    return {
        ServerCert = {
            IssueBy = ca_cert.IssueBy:fetch(),
            IssueTo = ca_cert.IssueTo:fetch(),
            ValidFrom = ca_cert.ValidFrom:fetch(),
            ValidTo = ca_cert.ValidTo:fetch(),
            SerialNumber = ca_cert.SerialNumber:fetch(),
            IsImportCrl = is_import_crl,
            CrlValidFrom = ca_cert.CRLStartTime:fetch(),
            CrlValidTo = ca_cert.CRLExpireTime:fetch()
        }
    }
end

local function GetClientCertInfo()
    local client_cert = O.CAServerClientCert
    return {
        ClientCert = {
            IssueBy = client_cert.IssueBy:fetch(),
            IssueTo = client_cert.IssueTo:fetch(),
            ValidFrom = client_cert.ValidFrom:fetch(),
            ValidTo = client_cert.ValidTo:fetch(),
            SerialNumber = client_cert.SerialNumber:fetch()
        }
    }
end

function M.GetCMPConfig(user)
    logging:debug("GetCMPConfig...")
    local obj_cmpconfig = O.CMPConfig
    local auth_type = obj_cmpconfig.TLSAuthType:fetch_or()
    local tls_authtype
    if auth_type == 1 then
        tls_authtype = "one-way"
    else
        tls_authtype = "two-way"
    end

    local update_state = obj_cmpconfig.AutoUpdateEnabled:fetch_or()
    local auto_update_enabled
    if (update_state == 0) then
        auto_update_enabled = false
    else
        auto_update_enabled = true
    end

    return {
        CACertChainInfo = GetCACertInfo(),
        ClientCertChainInfo = GetClientCertInfo(),
        CMPConfig = {
            CAServerAddr = obj_cmpconfig.CAServerAddr:fetch_or(),
            CAServerPort = obj_cmpconfig.CAServerPort:fetch_or(),
            CAServerCMPPath = obj_cmpconfig.CAServerCMPPath:fetch_or(),
            TLSAuthType = tls_authtype,
            AutoUpdateEnabled = auto_update_enabled,
            SubjectInfo = {
                Country = rf_string_check(obj_cmpconfig.Country:fetch_or()),
                State = rf_string_check(obj_cmpconfig.State:fetch_or()),
                Location = rf_string_check(obj_cmpconfig.Location:fetch_or()),
                OrgName = rf_string_check(obj_cmpconfig.OrgName:fetch_or()),
                OrgUnit = rf_string_check(obj_cmpconfig.OrgUnit:fetch_or()),
                CommonName = rf_string_check(obj_cmpconfig.CommonName:fetch_or()),
                InternalName = rf_string_check(obj_cmpconfig.InternalName:fetch_or()),
                Email = rf_string_check(obj_cmpconfig.Email:fetch_or())
            }
        }
    }
end

function M.SetCMPConfig(data, user)
    logging:debug("SetCMPConfig...")
    local result = reply_ok()

    result:join(SetCMPCfgCAServerAddr(data, user))
    result:join(SetCMPCfgCAServerPort(data, user))
    result:join(SetCMPCfgCAServerCMPPath(data, user))
    result:join(SetCMPCfgTLSAuthType(data, user))
    result:join(SetCMPCfgAutoUpdateEnabled(data, user))
    result:join(SetCMPCfgSubjectInfo(data, user))

    if result:isOk() == false then
        result:appendErrorData(M.GetCMPConfig(user))
        return result
    end

    return M.GetCMPConfig(user)
end

function M.UpdateCertFromCA(user)
    logging:debug("act cert update...")
    local obj_cmpconfig = O.CMPConfig
    local ok, err = call_method(user, obj_cmpconfig, "UpdateCertFromCA", errMapUpdate)
    if not ok then
        return err
    end
    return reply_ok()
end

function M.ImportCACert(formdata)
    local user = formdata.user
    local filePath = utils.GetFormDataFilePath(formdata)

    if utils.CheckFormData(formdata) == false then
        logging:error('the formdata is invalid.')
        return reply_bad_request('InvalidParam')
    end

    if not filePath then
        logging:error('the file path is invalid.')
        return reply_bad_request('InvalidFile')
    end

    local realPath = real_file_path(filePath)
    if not realPath or not is_tmp_web_path(realPath) then
        logging:error('the file path is invalid.')
        return reply_bad_request('InvalidFile')
    end

    local result
    result = C.CAServerCACert[0]:next(
        function(obj)
            local ok, ret = call_method(user, obj, 'ImportCA', CACertErrMap, gstring(filePath))
            if not ok then
                logging:error('Import CA Certificate error')
                return ret
            end
            return reply_ok()
        end
    ):fetch()

    c.rm_filepath(filePath)

    return result
end

function M.ImportCRL(formdata)
    local user = formdata.user
    local filePath = utils.GetFormDataFilePath(formdata)

    if utils.CheckFormData(formdata) == false then
        logging:error('the formdata is invalid.')
        return reply_bad_request('InvalidParam')
    end

    if not filePath then
        logging:error('the file path is invalid.')
        return reply_bad_request('InvalidFile')
    end

    local realPath = real_file_path(filePath)
    if not realPath or not is_tmp_web_path(realPath) then
        logging:error('the file path is invalid.')
        return reply_bad_request('InvalidFile')
    end

    local result
    result = C.CAServerCACert[0]:next(
        function(obj)
            local ok, ret = call_method(user, obj, 'ImportCRL', CrlErrMap, gstring(filePath))
            if not ok then
                logging:error('Import CRL error.')
                return ret
            end
            return reply_ok()
        end
    ):fetch()

    c.rm_filepath(filePath)

    return result
end

function M.DeleteCRL(user)
    return C.CAServerCACert[0]:next(function(obj)
        local ok, ret = call_method(user, obj, "DeleteCRL", deleteCrlErrMap)
        if not ok then
            logging:error('Delete CA Crl failed.')
            return ret
        end
        return reply_ok()
    end):fetch()
end
 
function M.ImportClientCert(formdata)
    local user = formdata.user
    local filePath = utils.GetFormDataFilePath(formdata)
    local password = formdata.fields.Password

    if utils.CheckFormData(formdata) == false then
        logging:error('the formdata is invalid.')
        return reply_bad_request('InvalidParam')
    end

    if not filePath then
        logging:error('the file path is invalid.')
        return reply_bad_request('InvalidFile')
    end

    local realPath = real_file_path(filePath)
    if not realPath or not is_tmp_web_path(realPath) then
        logging:error('the file path is invalid.')
        return reply_bad_request('InvalidFile')
    end

    if not password then  -- 未输入密码，表示空密码
        password = ""
    end

    if password and string.len(password) > 32 then
        logging:error('the password is too long.')
        c.rm_filepath(filePath)
        return reply_bad_request('InvalidPasswordLength')
    end

    local result
    result = C.CAServerClientCert[0]:next(
        function(obj)
            local ok, ret = call_method(user, obj, 'ImportClientCert', ClientCertErrMap, gstring(filePath),
                gstring(password))
            if not ok then
                logging:error('Import client certificate error.')
                return ret
            end
            return reply_ok()
        end
    ):fetch()

    c.rm_filepath(filePath)

    return result
end

return M