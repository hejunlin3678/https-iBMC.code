local cfg = require "config"
local utils = require "utils"
local defs = require 'define'
local dflib = require "dflib"
local c = require "dflib.core"
local tools = require 'tools'
local http = require "http"
local download = require "download"
local logging = require "logging"
local C = dflib.class
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local guint16 = GVariant.new_uint16
local gint32 = GVariant.new_int32
local gstring = GVariant.new_string
local call_method = utils.call_method
local HasSecurityMgntPrivilege = utils.HasSecurityMgntPrivilege
local HasBasicSettingPrivilege = utils.HasBasicSettingPrivilege
local reply_ok = http.reply_ok
local reply_internal_server_error = http.reply_internal_server_error
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_bad_request = http.reply_bad_request
local dal_set_file_owner = utils.dal_set_file_owner
local dal_set_file_mode = utils.dal_set_file_mode

-- web服务模块错误码定义
local RF_ERROR = -1
local RFERR_INSUFFICIENT_PRIVILEGE = 403
local COMP_CODE_INVALID_CMD = 0xC1
local COMP_CODE_OUTOF_RANGE = 0xC9

-- 导入失败类型
local IMPORT_OK = 0
local IMPORT_FAIL = 1
local FILE_MISMATCH = 3
local CUSTOM_CERT_IMPORT_FAIL = 4
local CA_MD_TOO_WEAK = 7
local SSL_CACERT_OVERDUED = 8
local CA_SHA1_TOO_WEAK = 9

local CSR_GEN_READY_OR_OK = 0
local CSR_GENERATING = 1

local portErrMap = {
    [COMP_CODE_INVALID_CMD] = reply_bad_request("PropertyModificationNotSupported",
        "The property modification failed because it is not supported."),
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("PropertyModificationNeedPrivilege",
        "The property cannot be modified because of insufficient permission."),
    [COMP_CODE_OUTOF_RANGE] = reply_bad_request("PortIdModificationFailed",
        "The value for the property is not in the range of 1 to 65535 or already exists."),
    [RF_ERROR] = reply_bad_request("PortIdModificationFailed",
        "The value for the property is not in the range of 1 to 65535 or already exists.")
}

local stateErrMap = {
    [COMP_CODE_INVALID_CMD] = reply_bad_request("PropertyModificationNotSupported",
        "The property modification failed because it is not supported."),
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("PropertyModificationNeedPrivilege",
        "The property cannot be modified because of insufficient permission."),
    [COMP_CODE_OUTOF_RANGE] = reply_bad_request("PropertyValueNotInList",
        "The value for the property is not in the list of acceptable values."),
    [RF_ERROR] = reply_bad_request("PropertyValueNotInList",
        "The value for the property is not in the list of acceptable values.")
}

local webSessionErrMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("PropertyModificationNeedPrivilege",
        "The property cannot be modified because of insufficient permission."),
    [IMPORT_FAIL] = reply_bad_request("CertImportFailed", "Failed to import the certificate."),
    [FILE_MISMATCH] = reply_bad_request("CertMismatch",
        "Failed to import the certificate because the certificate verification failed."),
    [CUSTOM_CERT_IMPORT_FAIL] = reply_bad_request("PropertyModificationNeedPrivilege",
        "Failed to import the encrypted certificate."),
    [CA_MD_TOO_WEAK] = reply_bad_request("EncryptionAlgorithmIsWeak",
        "Failed to import the certificate because the MD5 signature algorithm is not supported."),
    [CA_SHA1_TOO_WEAK] = reply_bad_request("EncryptionAlgorithmSHA1IsWeak",
        "Failed to import the certificate because the SHA1 signature algorithm is not supported."),
    [SSL_CACERT_OVERDUED] = reply_bad_request("CertificateExpired", "The certificate has expired.")
}

local importCustomCertErrMap = {
    [IMPORT_OK] = reply_bad_request("CertImportOK"),
    [IMPORT_FAIL] = reply_bad_request("EncryptedCertImportFailed", "Failed to import the encrypted certificate."),
    [FILE_MISMATCH] = reply_bad_request("EncryptedCertImportFailed", 
        "Failed to import the certificate because the certificate verification failed."),
    [CUSTOM_CERT_IMPORT_FAIL] = reply_bad_request("EncryptedCertImportFailed",
        "Failed to import the encrypted certificate."),
    [CA_MD_TOO_WEAK] = reply_bad_request("EncryptionAlgorithmIsWeak", 
        "Failed to import the certificate because the MD5 signature algorithm is not supported."),
    [CA_SHA1_TOO_WEAK] = reply_bad_request("EncryptionAlgorithmSHA1IsWeak",
        "Failed to import the certificate because the SHA1 signature algorithm is not supported."),
    [SSL_CACERT_OVERDUED] = reply_bad_request("CertificateExpired", 
        "The certificate has expired.")
}

local importServerCertErrMap = {
    [IMPORT_OK] = reply_bad_request("CertImportOK"),
    [RF_ERROR] = reply_bad_request("CertImportFailed", "Failed to import the certificate.")
}


local M = {}

function M.GetWEBService()
    return {
        HTTP = {
            Port = C.PortConfig("ServiceID", cfg.SERVICE_CONFIGURATION_WEB_HTTP).Port:next(
                function(array)
                    return array[1]
                end):fetch(),
            Enabled = C.PortConfig("ServiceID", cfg.SERVICE_CONFIGURATION_WEB_HTTP).State:eq(1):next(
                function()
                    return true
                end):fetch_or(false)
        },
        HTTPS = {
            Port = C.PortConfig("ServiceID", cfg.SERVICE_CONFIGURATION_WEB_HTTPS).Port:next(
                function(array)
                    return array[1]
                end):fetch(),
            Enabled = C.PortConfig("ServiceID", cfg.SERVICE_CONFIGURATION_WEB_HTTPS).State:eq(1):next(
                function()
                    return true
                end):fetch_or(false)
        },
        WebSession = {
            TimeoutMinutes = C.Session[0].Timeout:fetch_or(10) / 60,
            Mode = C.Session[0]:next(function(obj)
                return obj.Mode:fetch_or(0)
            end):eq(0):next(function()
                return "Shared"
            end):fetch_or("Private")
        },

        ServerCert = C.SSL():fold(function(obj)
            if obj.CertType:fetch() == 1 then
                return {
                    Issuer = obj.IssuerInfo:fetch(),
                    Subject = obj.SubjectInfo:fetch(),
                    ValidNotBefore = obj.StartTime:fetch(),
                    ValidNotAfter = obj.Expiration:fetch(),
                    SerialNumber = obj.SerialNumber:fetch()
                }, false
            end
        end):fetch(),

        IntermediateCert = C.SSL():fold(function(obj, acc)
            if obj.CertType:fetch() == 2 then
                acc[#acc + 1] = {
                    Issuer = obj.IssuerInfo:fetch(),
                    Subject = obj.SubjectInfo:fetch(),
                    ValidNotBefore = obj.StartTime:fetch(),
                    ValidNotAfter = obj.Expiration:fetch(),
                    SerialNumber = obj.SerialNumber:fetch()
                }
            end
            return acc
        end, {}):fetch(),

        RootCert = C.SSL():fold(function(obj)
            if obj.CertType:fetch() == 3 then
                return {
                    Issuer = obj.IssuerInfo:fetch(),
                    Subject = obj.SubjectInfo:fetch(),
                    ValidNotBefore = obj.StartTime:fetch(),
                    ValidNotAfter = obj.Expiration:fetch(),
                    SerialNumber = obj.SerialNumber:fetch()
                }, false
            end
        end):fetch()
    }
end

function M.SetWEBService(data, user)
    local ret
    -- HTTP
    if data.HTTP then
        ret = C.PortConfig("ServiceID", cfg.SERVICE_CONFIGURATION_WEB_HTTP):next(
            function(obj)
                if not HasSecurityMgntPrivilege(user) then
                    return reply_bad_request("PropertyModificationNeedPrivilege", {"HTTP"})
                end
                if data.HTTP.Port then
                    local ok, err = call_method(user, obj, "SetPort", portErrMap, guint16(data.HTTP.Port))
                    if not ok then
                        return err
                    end
                end
                if data.HTTP.Enabled == true then
                    local ok, err = call_method(user, obj, "SetState", stateErrMap, gbyte(1))
                    if not ok then
                        return err
                    end
                elseif data.HTTP.Enabled == false then
                    local ok, err = call_method(user, obj, "SetState", stateErrMap, gbyte(0))
                    if not ok then
                        return err
                    end
                end
            end):catch(function(err)
            return reply_internal_server_error("SetWEBService HTTP", err)
        end):fetch()
        if ret then
            ret:appendErrorData(M.GetWEBService())
            return ret
        end
    end

    -- HTTPS
    if data.HTTPS then
        ret = C.PortConfig("ServiceID", cfg.SERVICE_CONFIGURATION_WEB_HTTPS):next(
            function(obj)
                if not HasSecurityMgntPrivilege(user) then
                    return reply_bad_request("PropertyModificationNeedPrivilege", {"HTTPS"})
                end
                if data.HTTPS.Port then
                    local ok, err = call_method(user, obj, "SetPort", portErrMap, guint16(data.HTTPS.Port))
                    if not ok then
                        return err
                    end
                end
                if data.HTTPS.Enabled == true then
                    local ok, err = call_method(user, obj, "SetState", stateErrMap, gbyte(1))
                    if not ok then
                        return err
                    end
                elseif data.HTTPS.Enabled == false then
                    local ok, err = call_method(user, obj, "SetState", stateErrMap, gbyte(0))
                    if not ok then
                        return err
                    end
                end
            end):catch(function(err)
            return reply_internal_server_error("SetWEBService HTTPS", err)
        end):fetch()
        if ret then
            ret:appendErrorData(M.GetWEBService())
            return ret
        end
    end

    -- WebSession TimeoutMinutes SessionMode
    if data.WebSession then
        ret = C.Session[0]:next(function(obj)
            if not HasBasicSettingPrivilege(user) then
                return reply_bad_request("PropertyModificationNeedPrivilege", {"WebSession"})
            end
            if data.WebSession.TimeoutMinutes then
                local ok, err = call_method(user, obj, "SetSessionTimeout", webSessionErrMap,
                    gint32(data.WebSession.TimeoutMinutes * 60))
                if not ok then
                    return err
                end
            end
            if data.WebSession.Mode == "Shared" then
                local ok, err = call_method(user, obj, "SetSessionMode", webSessionErrMap, gbyte(0))
                if not ok then
                    return err
                end
            elseif data.WebSession.Mode == "Private" then
                local ok, err = call_method(user, obj, "SetSessionMode", webSessionErrMap, gbyte(1))
                if not ok then
                    return err
                end
            end
        end):catch(function(err)
            return reply_internal_server_error("SetWEBService WebSession", err)
        end):fetch()
        if ret then
            ret:appendErrorData(M.GetWEBService())
            return ret
        end
    end

    return reply_ok_encode_json(M.GetWEBService())
end

local function getUserPropStrVal(user_prop_val)
    if user_prop_val == nil then
        return ""
    end
    return user_prop_val
end

function M.ExportCSR(data, user)
    logging:debug("ExportCSR begin：%s, %s, %s, %s, %s, %s", data.Country, data.State, data.City, data.OrgName,
        data.OrgUnit, data.CommonName)
    return C.SSL[0]:next(function(obj)
        local generate_cb = function(filePath)
            local ok, ret = utils.call_method_async(user, obj, "GenerateCSR", webSessionErrMap,
                gstring(data.Country),
                gstring(getUserPropStrVal(data.State)),
                gstring(getUserPropStrVal(data.City)),
                gstring(getUserPropStrVal(data.OrgName)),
                gstring(getUserPropStrVal(data.OrgUnit)),
                gstring(data.CommonName))
            if not ok then
                logging:error("generate_cb over")
                return ret
            end
            return reply_ok()
        end

        local progress_cb = function(filePath)
            return C.SSL[0]:next(function(obj)
                local ok, ret = call_method(user, obj, "GetCsrStatus", webSessionErrMap)
                if not ok then
                    logging:error("progress_cb GetCsrStatus failed")
                    return 0
                end
                local csrPath = filePath .. ".csr"
                logging:debug("progress_cb GetCsrStatus ret = %d, csrPath = %s", ret[1], csrPath)
                if ret[1] == CSR_GEN_READY_OR_OK then
                    ok, _ = call_method(user, obj, "ExportCSR", webSessionErrMap, gstring(csrPath))
                    if not ok then
                        logging:error("progress_cb ExportCSR failed")
                        return 0
                    end
                    dal_set_file_owner(csrPath, cfg.REDFISH_USER_UID, cfg.APPS_USER_GID)
                    dal_set_file_mode(csrPath, cfg.FILE_MODE_600)
                    tools.movefile(csrPath, filePath)
                    return 100
                elseif ret[1] == CSR_GENERATING then
                    return 50
                else
                    logging:error("progress_cb GetCsrStatus CSRGenFailed")
                    return 0
                end
            end):fetch()
        end

        return download(user, 'server.csr', 'csr', defs.DOWNLOAD_CSR, generate_cb, progress_cb)
    end):catch(function(err)
        return reply_bad_request("ActionNotSupported", err)
    end):fetch()
end

local function ParseCertImportResultCode(code)
    if code == IMPORT_OK then
        return reply_ok()
    elseif code == IMPORT_FAIL then
        return reply_bad_request("CertImportFailed", "Failed to import the certificate.")
    elseif code == FILE_MISMATCH then
        return reply_bad_request("CertMismatch",
            "Failed to import the certificate because the certificate verification failed.")
    elseif code == CUSTOM_CERT_IMPORT_FAIL then
        return reply_bad_request("EncryptedCertImportFailed", "Failed to import the encrypted certificate.")
    elseif code == CA_MD_TOO_WEAK then
        return reply_bad_request("EncryptionAlgorithmIsWeak",
            "Failed to import the certificate because the MD5 signature algorithm is not supported.")
    elseif code == CA_SHA1_TOO_WEAK then
        return reply_bad_request("EncryptionAlgorithmSHA1IsWeak",
            "Failed to import the certificate because the SHA1 signature algorithm is not supported.")
    elseif code == SSL_CACERT_OVERDUED then
        return reply_bad_request("CertificateExpired", "The certificate has expired.")
    else
        return reply_bad_request("CertImportFailed", "Failed to import the certificate.")
    end
end

local function ImportServerCert(filePath, user)
    local result
    result = C.SSL[0]:next(function(obj)
        local fileLength = utils.file_length(filePath)
        local ok, ret = call_method(user, obj, "ImportPublicKey", importServerCertErrMap, gint32(fileLength), gstring(filePath))
        if not ok then
            logging:error(" ImportServerCert error ")
            return ret
        end
        return ParseCertImportResultCode(ret[1])
    end):fetch()
    if result then
        return result
    end
    return reply_ok_encode_json()
end

local function ImportCustomCert(filePath, password, user)
    local result = C.SSL[0]:next(function(obj)
        local fileLength = utils.file_length(filePath)
        local ok, ret = call_method(user, obj, "ImportCustomCert", importCustomCertErrMap, gint32(fileLength),
            gstring(password or ""), gstring(filePath))
        if not ok then
            logging:error("ImportCustomCert error")
            return ret
        end
    end):fetch_or(reply_bad_request("EncryptedCertImportFailed"))

    if result then
        return result
    end
    return reply_ok_encode_json()
end

function M.ImportCertificate(formdata)
    local fields = formdata.fields
    local user = formdata.user
    if utils.CheckFormData(formdata) == false then
        logging:error('Failed to import certificate')
        return reply_bad_request('InvalidParam')
    end
    local filePath = utils.GetFormDataFilePath(formdata)
    if not filePath then
        return reply_bad_request("InvalidFile")
    end

    local ret
    if fields.CertificateType == "Server" then
        ret = ImportServerCert(filePath, user)
    elseif fields.CertificateType == "Custom" then
        ret = ImportCustomCert(filePath, fields.Password, user)
    end

    c.rm_filepath(filePath)
    return ret
end

return M
