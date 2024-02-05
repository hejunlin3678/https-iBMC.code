local logging = require "logging"
local http = require "http"

local new_error_message = http.new_error_message
local default_header = http.default_header
local HTTP_INTERNAL_SERVER_ERROR = http.status.HTTP_INTERNAL_SERVER_ERROR

return function(server, h)
    return function(waker, ...)
        local reply = nil
        -- fork 协程处理 http 请求
        server:fork(function(...)
            local ok, result = pcall(h, ...)
            if not ok then
                local body = new_error_message("HTTP_INTERNAL_SERVER_ERROR", result)
                reply = {HTTP_INTERNAL_SERVER_ERROR, body, default_header}
            else
                reply = result
            end
            waker:wake()
        end, ...)
        return false, function()
            return reply
        end
    end
end
