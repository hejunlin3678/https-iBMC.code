local cjson = require 'cjson'
local utils = require "utils"
local dflib = require "dflib"
local c = require "dflib.core"
local http = require "http"
local logging = require "logging"
local portal = require "portal"
local C = dflib.class
local O = dflib.object
local json_decode = cjson.decode
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local guint32 = GVariant.new_uint32
local gint32 = GVariant.new_int32
local gstring = GVariant.new_string
local call_method = utils.call_method
local reply_bad_request = http.reply_bad_request
local reply_ok = http.reply_ok
local real_file_path = utils.real_file_path
local is_tmp_web_path = utils.is_tmp_web_path
local check_file_exists = utils.check_file_exists
 
local RF_OK = 0
local RF_ERR = -1
local RFINVALID_PARAM = -2
local RFEXPIRED_CERT = -3
local RFBIOS_STATE_UNSUPPORT = -4
local RFPEM_FORMAT_ERR = -5
local RFSTRING_TOO_LONG = -6
local RFIMPORT_LIMIT = -7
 
local requestErrMap = {
    [RF_ERR] = reply_bad_request("NotSupported"),
    [RFINVALID_PARAM] = reply_bad_request("InvalidParameter"),
    [RFEXPIRED_CERT] = reply_bad_request("ExpiredCertificate"),
    [RFBIOS_STATE_UNSUPPORT] = reply_bad_request("BiosIsStarting"),
    [RFPEM_FORMAT_ERR] = reply_bad_request("FormatIncorrect"),
    [RFSTRING_TOO_LONG] = reply_bad_request("StringParameterIsTooLong"),
    [RFIMPORT_LIMIT] = reply_bad_request("ImportLimitExceeded")
}
 
local M = {}

function  readText(filePath)
    local openFile = io.open(filePath, "r")
    if not openFile then
        error('open file failed')
    end
    local text = {}
    local index = 1
    for line in openFile:lines() do
        text[index] = line..'\n'
        index = index + 1
    end
    io.close(openFile)
    return table.concat(text)
end

function M.ImportBootCertActionInfo(formdata)
    local user = formdata.user
    local filePath = utils.GetFormDataFilePath(formdata)

    if utils.CheckFormData(formdata) == false then
        logging:error('Failed to import client certificate')
        if filePath then
            c.rm_filepath(filePath)
        end
        return reply_bad_request('InvalidParam')
    end

    if not filePath then
        return reply_bad_request("InvalidFile")
    end
    local cert_string = readText(filePath)
    c.rm_filepath(filePath)

    local result = C.Bios[0]:next(function(biosObj)
        local input = {gstring("Boot"), gstring("cert"), gstring(cert_string), gstring("PEM")}
        local ok, ret = call_method(user, biosObj, "ImportCert", requestErrMap, input)
        if not ok then
            logging:error(" ImportCert error ")
            return ret
        end
    end):fetch()
    if result then
        return result
    end
 
    return reply_ok()
end

function M.ImportBootCrlActionInfo(formdata)
    local user = formdata.user
    local filePath = utils.GetFormDataFilePath(formdata)

    if utils.CheckFormData(formdata) == false then
        logging:error('Failed to import client certificate')
        if filePath then
            c.rm_filepath(filePath)
        end
        return reply_bad_request('InvalidParam')
    end

    if not filePath then
        return reply_bad_request("InvalidFile")
    end
    local cert_string = readText(filePath)
    c.rm_filepath(filePath)

    local result = C.Bios[0]:next(function(biosObj)
        local input = {gstring("Boot"), gstring("crl"), gstring(cert_string), gstring("PEM")}
        local ok, ret = call_method(user, biosObj, "ImportCert", requestErrMap, input)
        if not ok then
            logging:error(" ImportCrl error ")
            return ret
        end
    end):fetch()
    if result then
        return result
    end

    return reply_ok()
end

function M.ImportSecureBootCertActionInfo(formdata)
    local user = formdata.user
    local database = formdata.fields.type
    local filePath = utils.GetFormDataFilePath(formdata)

    if database == nil then
        if filePath then
            c.rm_filepath(filePath)
        end
        return reply_bad_request('InvalidParam')
    end

    if utils.CheckFormData(formdata) == false then
        logging:error('Failed to import client certificate')
        if filePath then
            c.rm_filepath(filePath)
        end
        return reply_bad_request('InvalidParam')
    end

    if not filePath then
        return reply_bad_request("InvalidFile")
    end

    local cert_string = readText(filePath)
    c.rm_filepath(filePath)
    local result = C.Bios[0]:next(function(biosObj)
        local input = {gstring("SecureBoot"), gstring(database), gstring(cert_string), gstring("PEM")}
        local ok, ret = call_method(user, biosObj, "ImportCert", requestErrMap, input)
        if not ok then
            logging:error(" ImportCert Secure Boot Cert error ")
            return ret
        end
    end):fetch()
    if result then
        return result
    end

    return reply_ok()
end

function M.GetBootCertificates(user)
    local ok = false
    local ret
    local result = C.Bios[0]:next(function(biosObj)
        local input = {}
        ok, ret = call_method(user, biosObj, "GetHttpsCertificates", requestErrMap, input)
        return ret
    end):fetch()

    if not ok then
        logging:error("Get https boot cert failed.")
        return result
    end

    local body = {}
    body['HttpsCert'] = json_decode(result[1])
    return reply_ok(body)
end

function M.GetBiosSecureBoot(user)
    local ok = false
    local ret
    local result = C.Bios[0]:next(function(biosObj)
        local input = {}
        ok, ret = call_method(user, biosObj, "GetSecureBoot", requestErrMap, input)
        return ret
    end):fetch()

    if not ok then
        logging:error("Get secure boot failed.")
        return result
    end

    local body = {}
    local val = result[1]
    if val == 1 then
        body['SecureBoot'] = "Enabled"
    elseif val == 0 then
        body['SecureBoot'] = "Disabled"
    end
    return reply_ok(body)
end

function M.SetBiosSecureBoot(data, user)
    local SecBoot = data.SecureBoot
    if not SecBoot then
        return reply_bad_request('InvalidParam', 'The parameter SecureBoot is missing.')
    end

    local secureboot = tonumber(SecBoot)
    if secureboot ~= 0 and secureboot ~= 1 then
        return reply_bad_request('InvalidParam', 'SecureBoot can only be 1 or 0.')
    end

    local ok, result = C.Bios[0]:next(function(biosObj)
        local input = {gbyte(secureboot)}
        local errCode, ret = call_method(user, biosObj, "SetSecureBoot", requestErrMap, input)
        return errCode, ret
    end):fetch()

    if not ok then
        logging:error("Set secure boot failed.")
        return result
    end

    return reply_ok()
end

return M