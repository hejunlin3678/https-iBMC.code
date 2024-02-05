local cjson = require 'cjson'
local dflib = require 'dflib'
local c = require 'dflib.core'
local http = require 'http'
local utils = require 'utils'
local reboot = require "reboot"
local rutils = require 'redfish_utils'
local logging = require 'logging'
local sessionservice = require 'sessionservice'
local cfg = require 'config'
local O = dflib.object
local GVariant = c.GVariant
local gstring = GVariant.new_string
local gint32 = GVariant.new_int32
local reply = http.reply
local reply_internal_server_error = http.reply_internal_server_error
local reply_bad_request = http.reply_bad_request
local status = http.status
local HTTP_BAD_REQUEST = status.HTTP_BAD_REQUEST
local json_decode = cjson.decode
local json_encode = cjson.encode
local redfish_call_method = utils.redfish_call_method
local lower = string.lower
local tsort = table.sort
local sformat = string.format
local create_message = rutils.create_message
local real_file_path = utils.real_file_path
local file_exists = utils.file_exists
local dal_check_file_opt_user = utils.dal_check_file_opt_user
local dal_set_file_owner = utils.dal_set_file_owner
local dal_set_file_mode = utils.dal_set_file_mode
local RedfishOperateLog = utils.RedfishOperateLog

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

local function decode_body(body)
    local body_ok, result = pcall(json_decode, body)
    if not body_ok then
        local GeneralError = create_message('GeneralError')
        local res = cjson.encode({
            error = {
                code = GeneralError.MessageId,
                message = GeneralError.Message,
                ['@Message.ExtendedInfo'] = {create_message('MalformedJSON')}
            }
        })
        logging:error('respone body: %s', res)
        return false, reply(HTTP_BAD_REQUEST, res)
    end
    return true, result
end

local function process_download_file(downloadFile, user)
    local target_name = nil
    local need_clean = false
    local file_path
    if downloadFile:match('Tag') then
        target_name = 'Tag.csv'
        file_path = sformat('/dev/shm/%s.csv', downloadFile)
    elseif downloadFile:match('Metric') then
        target_name = 'Metric.csv'
        file_path = sformat('/dev/shm/%s.csv', downloadFile)
    elseif downloadFile:match('DataPoint') then
        target_name = 'Datapoint.csv'
        file_path = sformat('/dev/shm/%s.csv', downloadFile)
    else
        need_clean = true
        file_path = downloadFile
        target_name = downloadFile:match('([^\\/]+)$')
    end

    local realPath = real_file_path(file_path)
    if not realPath then
        logging:error('invalid download file path')
        return false, reply_internal_server_error('InternalError')
    end

    if not file_exists(file_path) then
        return false, reply_bad_request('FileNotExist')
    end

    if not user or not dal_check_file_opt_user(user, file_path) then
        local generalError = create_message('GeneralError')
        local res = cjson.encode({
            error = {
                code = generalError.MessageId,
                message = generalError.Message,
                ['@Message.ExtendedInfo'] = {create_message('NoPrivilegeToOperateSpecifiedFile')}
            }
        })
        return false, reply(HTTP_BAD_REQUEST, res)
    end

    if not dal_set_file_owner(file_path, cfg.REDFISH_USER_UID, cfg.APPS_USER_GID) then
        logging:error('chown blackbox failed')
        return false, reply_internal_server_error('InternalError')
    end

    if not dal_set_file_mode(file_path, cfg.FILE_MODE_600) then
        logging:error('chmod blackbox failed')
        return false, reply_internal_server_error('InternalError')
    end

    return true, {need_clean = need_clean, file_path = file_path}, target_name
end

local WAIT_RESET_TIMEOUT = 3000
local CANCEL_RESET_TIMEOUT = 500
local function is_reboot_url(path)
    return path:find('actions/manager.reset', 1, true) or path:find('actions/oem/huawei/manager.rollback', 1, true)
end

function M.ProxyRedfish(param)
    if param.Path then
        -- filter tail slash
        param.Path = param.Path:gsub('(/+)$', '')
    end

    local lPath = lower(param.Path)
    if param.Body and #param.Body > 0 then
        local body_ok, body = decode_body(param.Body)
        if not body_ok then
            return body
        end

        if (lPath:match('^/redfish/v1/managers/[^/]+/kvmservice/actions/kvmservice.exportkvmstartupfile$') or
            lPath:match(
                '^/redfish/v1/managers/[^/]+/diagnosticservice/actions/diagnosticservice.exportvideoplaybackstartupfile$')) then
            if not param.Body:match('^%s*{%s*}%s*$') then
                param.Body = param.Body:gsub('}%s*$',
                    sformat(',"HostName":"%s","ServerName":"%s","Language":"en"}', O.EthGroup0.IpAddr:fetch(),
                        O.EthGroup0.IpAddr:fetch()))
            else
                param.Body = sformat('{"HostName":"%s","ServerName":"%s","Language":"en"}',
                    O.EthGroup0.IpAddr:fetch(), O.EthGroup0.IpAddr:fetch())
            end
        end
    end

    local headers = {}
    for k, v in pairs(param.Headers) do
        headers[#headers + 1] = {[k] = (lower(k) == 'cookie') and ProcessCookie(v) or v}
    end
    local uri = param.Path
    if param.Query and #param.Query > 0 then
        uri = uri .. '?' .. param.Query
    end
    local requestObj = json_encode({
        RequestMethod = param.Method,
        RelativeUri = rutils.replace_manufacture_for_uri(uri, O.BMC.CustomManufacturer:fetch_or("")),
        RequestIp = param.Ip,
        RequestHeaders = headers,
        RequestBody = rutils.replace_manufacture_for_req(param.Body, O.BMC.CustomManufacturer:fetch_or("")),
        HostPort = param.Port
    })
    return O.Redfish:next(function(obj)
        local reboot_hook = is_reboot_url(lPath) and reboot.hook(WAIT_RESET_TIMEOUT)
        local ok, ret = redfish_call_method(nil, obj, 'ProcessHttpRequest', nil, {gstring(requestObj), gint32(0)})
        if reboot_hook then
            reboot_hook.cancel(CANCEL_RESET_TIMEOUT)
        end
        if not ok then
            if ret and http.is_reply(ret) and ret:status() == HTTP_BAD_REQUEST then
                local GeneralError = create_message('GeneralError')
                local body = cjson.encode({
                    error = {
                        code = GeneralError.MessageId,
                        message = GeneralError.Message,
                        ['@Message.ExtendedInfo'] = {create_message('TaskLimitExceeded')}
                    }
                })
                return reply(HTTP_BAD_REQUEST, body)
            end
            return reply_internal_server_error(nil, ret)
        end

        if #ret < 3 then
            process_ret(ret)
        end

        local responseBody = rutils.replace_manufacture_for_rsp(ret[1], O.BMC.CustomManufacturer:fetch_or(""))
        local responseHeaders = json_decode(ret[2])
        local responseStatusCode = ret[3]

        local rspHeaders = {}
        local isDownloadFile = false
        local downloadFile = nil
        for _, header in ipairs(responseHeaders) do
            for k, v in pairs(header) do
                local key = lower(k)
                if key == 'content-disposition' then
                    downloadFile = v
                elseif key == 'content-type' and v == 'application/octet-stream' then
                    isDownloadFile = true
                    rspHeaders[k] = v
                elseif key ~= 'content-length' then
                    rspHeaders[k] = v
                end
            end
        end

        if isDownloadFile then
            local result = sessionservice.AuthToken(param.AuthInfo)
            if not result:isOk() then
                return result
            end
            local user = result:body()
            if downloadFile then
                local ok1, res, name = process_download_file(downloadFile, user)
                if not ok1 then
                    RedfishOperateLog(user, 'GeneralDownload %s failed', downloadFile)
                    return res
                end
                rspHeaders['Content-Disposition'] = 'attachment; filename=' .. name
                rspHeaders['is_download'] = "true"
                responseBody = json_encode(res)
                RedfishOperateLog(user, 'GeneralDownload %s to local successfully', name)
            else
                if not responseBody or #responseBody == 0 then
                    logging:error('download file from responseBody falied, body is empty')
                    RedfishOperateLog(user, 'GeneralDownload failed')
                    return reply_internal_server_error('InternalError')
                end
                local body = json_decode(responseBody)
                if not body.file then
                    logging:error('download file from responseBody falied, body not file field')
                    RedfishOperateLog(user, 'GeneralDownload failed')
                    return reply_internal_server_error('InternalError')
                end

                local filename = body.file:match('([^/]+)$')
                rspHeaders['Content-Disposition'] = sformat('attachment; filename=%s', filename)
                responseBody = body.buffer or ''
                RedfishOperateLog(user, 'GeneralDownload %s to local successfully', filename)
            end
        end

        local code = 200
        if responseStatusCode then
            code = tonumber(responseStatusCode:match('%d+')) or 500
        end

        return reply(code, responseBody, rspHeaders)
    end):fetch()
end

return M
