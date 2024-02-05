local utils = require 'utils'
local call_method = utils.call_method
local cjson = require 'cjson'
local dflib = require 'dflib'
local c = require 'dflib.core'
local http = require 'http'
local rutils = require 'redfish_utils'
local C = dflib.class
local O = dflib.object
local GVariant = c.GVariant
local realpath = c.realpath
local get_file_accessible = c.get_file_accessible
local gstring = GVariant.new_string
local gint32 = GVariant.new_int32
local reply = http.reply
local status = http.status
local HTTP_NOT_FOUND = status.HTTP_NOT_FOUND
local json_decode = cjson.decode
local json_encode = cjson.encode
local format = string.format
local lower = string.lower
local tsort = table.sort
local DFL_OK = dflib.DFL_OK

local G_CHECKSUM_SHA256 = 2
local PRODUCT_VERSION_V5 = 5
local MAX_FILEPATH_LENGTH = 256
local LOG_FILE_PATH = '/dev/shm/redfish'
local ERROR_FORMAT =
    '{"error":{"code":"Base.1.0.GeneralError","message":"A general error has occurred. See ExtendedInfo for more information.","@Message.ExtendedInfo":[{ "@odata.type":"#Message.v1_0_0.Message","MessageId":"%s","RelatedProperties":[],"Message":"%s","MessageArgs":%s,"Severity":"%s","Resolution":"%s"}]}}'

local M = {}

--  When the UI is refreshed after the language is switched, SessionId in the cookie header is moved forward,
--  causing the session to become invalid.
local function ProcessCookie(val)
    if not val:match(';%s*$') then
        val = val .. ';'
    end
    return val:gsub('^(.*)(SessionId=%w+);(.*)$', '%1%3%2')
end

local function rfind(s, ch, i, j)
    while i > j do
        local t = s:sub(i, i)
        if t == ch then
            return i
        end
        i = i - 1
    end
end

local function process_ret(ret)
    local s = ret[1]

    local result = {}

    local names = {{'ResponseStatusCode'}, {'ResponseHeaders'}, {'ResponseBody'}}
    for _, v in ipairs(names) do
        v[2] = s:find(v[1])
    end
    tsort(names, function(a, b)
        return a[2] < b[2]
    end)

    for i, v in ipairs(names) do
        local spos = s:find(':', v[2]) + 1
        local epos
        if i ~= #names then
            epos = rfind(s, ',', names[i + 1][2], v[2])
        else
            epos = rfind(s, '}', #s, v[2])
        end
        result[v[1]] = s:sub(spos, epos - 1)
    end

    ret[1] = result.ResponseBody
    ret[2] = result.ResponseHeaders
    ret[3] = result.ResponseStatusCode
end

local function check_pd_log_support()
    return O.PRODUCT.ProductVersionNum:fetch_or(0) >= PRODUCT_VERSION_V5 and
               C.StorageDiagnose[0].PDLogEnable:eq(1):fetch_or(false)
end

local function actions_log_service_save_logdata(path, logData)
    if #logData == 0 or #logData % 2 ~= 0 then
        return false
    end
    if not get_file_accessible(LOG_FILE_PATH) then
        if c.system_s('/bin/mkdir', '-p', LOG_FILE_PATH) ~= DFL_OK or
            c.system_s('/bin/chmod', '755', LOG_FILE_PATH) ~= DFL_OK then
            return false
        end
    end
    local file = io.open(path, 'wb')
    if not file then
        return false
    end
    for ch in logData:gmatch('(..)') do
        file:write(string.char(tonumber(ch, 16)))
    end
    file:close()
    c.system_s('/bin/chmod', '644', path)
    return true
end

local function is_inner_session(session_id)
    if session_id == nil then
        return false
    end
    local ok, ret = call_method(nil, O.Session, "IsRedfishInnerSession", nil, gstring(session_id))
    if not ok then
        return false
    end

    return true
end

local function actions_log_service_request_handler(param)
    if not param.Body or not check_pd_log_support() then
        return
    end
    local requestBody = json_decode(param.Body)
    local logType = requestBody.Type
    if not logType or type(logType) ~= 'string' or lower(logType) ~= lower('SMART') then
        return
    end
    local suffix = requestBody.FileSuffix
    if not suffix or type(suffix) ~= 'string' or suffix:match('/') then
        return
    end
    local path = '/dev/shm/redfish/drivelog.' .. suffix
    if get_file_accessible(path) then
        path = '/dev/shm/redfish/drivelog.1.' .. suffix
    end
    local logData = requestBody.LogData
    if not logData or type(logData) ~= 'string' then
        return
    end
    if not actions_log_service_save_logdata(path, logData) then
        return
    end
    requestBody.LogData = path
    param.Body = json_encode(requestBody)
    return path
end

local function log_check_file_path(path)
    if not path or #path > MAX_FILEPATH_LENGTH then
        return false
    end
    return realpath(path:match('^(.+)/[^/]+$')) == LOG_FILE_PATH
end

local function ReplyNotFoundError(path)
    local id = 'Base.1.0.ResourceMissingAtURI'
    local message = format('The resource at the URI %s was not found.', path)
    local messageArgs = json_encode({path})
    local severity = 'Critical'
    local resolution = 'Place a valid resource at the URI or correct the URI and resubmit the request.'
    local body = format(ERROR_FORMAT, id, message, messageArgs, severity, resolution)
    return reply(HTTP_NOT_FOUND, body)
end

function M.ProxySms(param)
    local headers = {}
    local session_id
    for k, v in pairs(param.Headers) do
        if lower(k) == lower('X-Auth-Token') then
            headers[#headers + 1] = {['X-Auth-Token'] = v}
            session_id = c.compute_checksum(G_CHECKSUM_SHA256, v):sub(1, 16)
        else
            headers[#headers + 1] = {[k] = (lower(k) == 'cookie') and ProcessCookie(v) or v}
        end
    end

    local obj, method, logPath
    if lower(param.Path) == '/redfish/v1/eventclient/sms0' then
        obj, method = O.sms0, 'HandleRedfishEvent'
    elseif lower(param.Path):match('^/redfish/v1/sms/') then
        obj, method = O.Redfish, 'RedirectHttpRequest'
    else
        if lower(param.Path):match(
            '^/redfish/v1/systems/[^/]+/logservices/hostlog/actions/oem/[^/]+/logservice.push$') then
            -- 硬盘日志收集特殊处理
            if is_inner_session(session_id) == true then
                logPath = actions_log_service_request_handler(param)
            else
                return ReplyNotFoundError(param.Path)
            end
        end
        obj, method = O.Redfish, 'ProcessHttpRequest'
    end

    local requestObj = json_encode({
        RequestMethod = param.Method,
        RelativeUri = rutils.replace_manufacture_for_uri(param.Path, O.BMC.CustomManufacturer:fetch_or("")),
        RequestIp = param.Ip,
        RequestHeaders = headers,
        RequestBody = rutils.replace_manufacture_for_req(param.Body, O.BMC.CustomManufacturer:fetch_or("")),
        HostPort = param.Port
    })

    local errMap = {
        [-1] = http.reply_bad_request('GeneralError'),
        [1] = http.reply_bad_request('GeneralError'),
    }
    local ok, ret = utils.redfish_call_method(nil, obj, method, errMap, {gstring(requestObj), gint32(0)})
    if not ok then
        if logPath and log_check_file_path(logPath) and get_file_accessible(logPath) then
            c.rm_filepath(logPath)
        end
        return ReplyNotFoundError(param.Path)
    end

    if #ret < 3 then
        process_ret(ret)
    end

    local responseBody = rutils.replace_manufacture_for_rsp(ret[1], O.BMC.CustomManufacturer:fetch_or(""))
    local responseHeaders = (ret[2] ~= '') and json_decode(ret[2]) or {}
    local responseStatusCode = ret[3]

    local rspHeaders = {}
    for _, header in ipairs(responseHeaders) do
        for k, v in pairs(header) do
            if lower(k) ~= 'content-length' then
                rspHeaders[k] = v
            end
        end
    end
    local code = 200
    if responseStatusCode then
        code = tonumber(responseStatusCode:match('%d+')) or 500
    end
    return reply(code, responseBody, rspHeaders)
end

return M
