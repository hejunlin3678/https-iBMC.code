local cjson = require "cjson"
local logging = require "logging"
local json_encode = cjson.encode

local http = {}

local status = {}
status.HTTP_CONTINUE = 100
status.HTTP_SWITCHING_PROTOCOLS = 101
status.HTTP_OK = 200
status.HTTP_CREATED = 201
status.HTTP_ACCEPTED = 202
status.HTTP_NO_CONTENT = 204
status.HTTP_PARTIAL_CONTENT = 206
status.HTTP_SPECIAL_RESPONSE = 300
status.HTTP_MOVED_PERMANENTLY = 301
status.HTTP_MOVED_TEMPORARILY = 302
status.HTTP_SEE_OTHER = 303
status.HTTP_NOT_MODIFIED = 304
status.HTTP_TEMPORARY_REDIRECT = 307
status.HTTP_BAD_REQUEST = 400
status.HTTP_UNAUTHORIZED = 401
status.HTTP_PAYMENT_REQUIRED = 402
status.HTTP_FORBIDDEN = 403
status.HTTP_NOT_FOUND = 404
status.HTTP_NOT_ALLOWED = 405
status.HTTP_NOT_ACCEPTABLE = 406
status.HTTP_REQUEST_TIMEOUT = 408
status.HTTP_CONFLICT = 409
status.HTTP_GONE = 410
status.HTTP_UPGRADE_REQUIRED = 426
status.HTTP_TOO_MANY_REQUESTS = 429
status.HTTP_CLOSE = 444
status.HTTP_ILLEGAL = 451
status.HTTP_INTERNAL_SERVER_ERROR = 500
status.HTTP_METHOD_NOT_IMPLEMENTED = 501
status.HTTP_BAD_GATEWAY = 502
status.HTTP_SERVICE_UNAVAILABLE = 503
status.HTTP_GATEWAY_TIMEOUT = 504
status.HTTP_VERSION_NOT_SUPPORTED = 505
status.HTTP_INSUFFICIENT_STORAGE = 507

http.status = status

local mt_reply = {}
mt_reply.__index = mt_reply

local default_header = {["content-type"] = "application/json;charset=utf-8"}

local HTTP_OK = status.HTTP_OK
local HTTP_ACCEPTED = status.HTTP_ACCEPTED
local HTTP_BAD_REQUEST = status.HTTP_BAD_REQUEST
local HTTP_UNAUTHORIZED = status.HTTP_UNAUTHORIZED
local HTTP_INTERNAL_SERVER_ERROR = status.HTTP_INTERNAL_SERVER_ERROR
local HTTP_NOT_FOUND = status.HTTP_NOT_FOUND
local HTTP_FORBIDDEN = status.HTTP_FORBIDDEN
local HTTP_METHOD_NOT_IMPLEMENTED = status.HTTP_METHOD_NOT_IMPLEMENTED

local function is_reply(v)
    return getmetatable(v) == mt_reply
end

function mt_reply:status()
    return self[1]
end

function mt_reply:isOk()
    return self[1] == HTTP_OK
end

function mt_reply:body()
    return self[2]
end

function mt_reply:__tostring()
    return json_encode(self[2])
end

function mt_reply:setBody(body)
    self[2] = body
end

function mt_reply:setStatus(status)
    self[1] = status
end

function mt_reply:headers()
    return self[3]
end

function mt_reply:addHeader(name, value)
    local h = self[3]
    if not h then
        self[3] = {[name] = value}
    else
        h[name] = value
    end
end

function mt_reply:addHeaders(headers)
    if not headers then
        return
    end

    local h = self[3]
    if not h then
        self[3] = headers
    else
        for k, v in pairs(headers) do
            h[k] = v
        end
    end
end

function mt_reply:appendErrorData(extData)
    if extData then
        if self[1] ~= HTTP_OK then
            -- body.error 字段已经存放了错误信息, 为防止冲突
            -- 新增加 body.data 字段来存附加数据
            local body = self[2]
            if not body.data then
                body.data = extData
            else
                if type(extData) == "table" then
                    for k, v in pairs(extData) do
                        body.data[k] = v
                    end
                else
                    body.data[#body.data + 1] = extData
                end
            end
        end
    end
end

function mt_reply:fixErrorStatus()
    local body = self:body()
    if body and type(body) == "table" and body.error then
        local err = body.error
        if self[1] ~= HTTP_OK then
            -- 只要不是全错就当做 ok
            if self.joinCount and #err ~= self.joinCount then
                self[1] = HTTP_OK
            end
        end

        if #err == 0 then
            body.error = {err}
        end
    end
    return self
end

function mt_reply:join(reply)
    if not reply then
        return
    end

    if not self.joinCount then
        self.joinCount = 1
    else
        self.joinCount = self.joinCount + 1
    end

    assert(is_reply(reply))
    local status = self:status()
    if not reply:isOk() then -- 只组合错误响应
        if status == HTTP_OK then
            local oldBody = self:body()
            self:setStatus(reply:status())
            self:setBody(reply:body())
            self:appendErrorData(oldBody)
        else
            -- 如果自己是通用错误就用新的错误替代,否则维持错误不变
            if status == HTTP_BAD_REQUEST or status == HTTP_INTERNAL_SERVER_ERROR then
                self:setStatus(reply:status())
            end
            -- 组合错误消息体
            local body = self:body()
            local error = body.error or {}
            local newError = reply:body().error or {}
            for _, v in ipairs(newError) do
                error[#error + 1] = v
            end
            body.error = error
        end
    end
    self:addHeaders(reply:headers())
end

function mt_reply:clone()
    local newHeader = nil
    local newBody = nil
    local body = self[2]
    if body then
        newBody = {}
        for k, v in pairs(body) do
            newBody[k] = v
        end

        if body.error then
            local newError = {}
            for _, v in ipairs(body.error) do
                newError[#newError + 1] = v
            end
            newBody.error = newError
        end
    end

    local header = self[3]
    if header then
        newHeader = {}
        for k, v in pairs(header) do
            newHeader[k] = v
        end
    end
    return setmetatable({self[1], newBody, newHeader}, mt_reply)
end

local function new_error_message(code, message, props)
    if type(message) == "string" and message:find("%.lua:%d+:") then
        logging:error("%s", message)
        message = nil
    end
    return {error = {{code = code, message = message, properties = props}}}
end

local function has_header(header, lower_name)
    for k, _ in pairs(header) do
        if k:lower() == lower_name then
            return true
        end
    end
    return false
end

local function reply(status, body, header)
    if not header then
        header = default_header
    elseif not has_header(header, 'content-type') then
        header["content-type"] = "application/json;charset=utf-8"
    end
    return setmetatable({status, body, header}, mt_reply)
end

function http.reply_ok_encode_json(body, header)
    return reply(HTTP_OK, body and json_encode(body) or "", header)
end

function http.reply_ok(body, header)
    return reply(HTTP_OK, body, header)
end

function http.reply_accepted(body, header)
    return reply(HTTP_ACCEPTED, body, header)
end

function http.reply_error(status, code, msg, props)
    return reply(status, new_error_message(code or "HTTP_BAD_REQUEST", msg, props))
end

function http.reply_bad_request(code, msg, props)
    return reply(HTTP_BAD_REQUEST, new_error_message(code or "HTTP_BAD_REQUEST", msg, props))
end

function http.reply_unauthorized(code, msg, props)
    return reply(HTTP_UNAUTHORIZED, new_error_message(code or "HTTP_UNAUTHORIZED", msg, props))
end

function http.reply_internal_server_error(code, msg, props)
    return reply(HTTP_INTERNAL_SERVER_ERROR, new_error_message(code or "HTTP_INTERNAL_SERVER_ERROR", msg, props))
end

function http.reply_not_found(code, msg, props)
    return reply(HTTP_NOT_FOUND, new_error_message(code or "HTTP_NOT_FOUND", msg, props))
end

function http.reply_forbidden(code, msg, props)
    return reply(HTTP_FORBIDDEN, new_error_message(code or "HTTP_FORBIDDEN", msg, props))
end

function http.reply_method_not_implemented(code, msg, props)
    return reply(HTTP_METHOD_NOT_IMPLEMENTED, new_error_message(code or "HTTP_METHOD_NOT_IMPLEMENTED", msg, props))
end

http.reply = reply
http.is_reply = is_reply

http.safe_call = function(cb)
    local ok, ret = pcall(cb)
    if not ok then
        logging:error("%s", ret)
        return reply(HTTP_BAD_REQUEST, new_error_message("HTTP_BAD_REQUEST", ret))
    end
    if not ret then
        return http.reply_ok()
    end
    return ret
end

http.new_error_message = new_error_message
http.default_header = default_header

return http
