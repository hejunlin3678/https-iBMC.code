local type = type
local select = select
local sformat = string.format
local srep = string.rep
local tinsert = table.insert
local tconcat = table.concat
local tostring = tostring

local logging_str = {"ERROR", "WARN", "INFO", "DEBUG", "TRACE"}

local ERROR, WARN, INFO, DEBUG, TRACE = 1, 2, 3, 4, 5

local function format_message(module, message)
    if module then
        return sformat("[%s] %s", module, message)
    else
        return message
    end
end

local function table_tostring(tt, indent, done)
    done = done or {}
    indent = indent or 0
    if type(tt) == "table" then
        local sb = {}
        for key, value in pairs(tt) do
            tinsert(sb, srep(" ", indent))
            if type(value) == "table" and not done[value] then
                done[value] = true
                tinsert(sb, key .. " = {\n");
                tinsert(sb, table_tostring(value, indent + 2, done))
                tinsert(sb, srep(" ", indent))
                tinsert(sb, "}\n");
            elseif "number" == type(key) then
                tinsert(sb, sformat("\"%s\"\n", tostring(value)))
            else
                tinsert(sb, sformat("%s = \"%s\"\n", tostring(key), tostring(value)))
            end
        end
        return tconcat(sb)
    else
        return tt .. "\n"
    end
end

local function _tostring(val)
    local t = type(val)
    if t == "table" then
        return table_tostring(val)
    elseif t == "string" then
        return val
    else
        return tostring(val)
    end
end

local function FORMAT_LOG_MSG(self, fmt, ...)
    local t = type(fmt)
    if t == 'string' then
        if select('#', ...) > 0 then
            local status, msg = pcall(sformat, fmt, ...)
            if status then
                return format_message(self.module, msg)
            else
                return format_message(self.module, "Formatting log message failed: " .. msg)
            end
        else
            return format_message(self.module, fmt)
        end
    elseif t == "table" then
        return format_message(self.module, table_tostring(fmt))
    else
        return format_message(self.module, tostring(fmt))
    end
end

local function LOG_MSG(self, level, fmt, ...)
    self.print(level, FORMAT_LOG_MSG(self, fmt, ...))
end

local function default_print(level, msg)
    print(sformat("%s %s %s", os.date("%Y-%m-%d %X"), logging_str[level], msg))
end

local function new_logger(module)
    local logger = {}
    logger.module = module
    logger.print = default_print
    logger.level = INFO

    logger.setLevel = function(self, level)
        if type(level) == "function" then
            self.getLevel = level
            return
        end

        assert(level <= #logging_str, sformat("Logger: invalid log level: %d", level))
        if self.level ~= level then
            self:warn("Logger: changing loglevel from %s to %s", logging_str[self.level], logging_str[level])
            self.level = level
        end
    end

    logger.getLevel = function(self)
        return self.level
    end

    logger.setPrint = function(self, print)
        local old = self.print
        self.print = print
        return old
    end

    logger.log = function(self, level, ...)
        if level <= self:getLevel() then
            return LOG_MSG(self, level, ...)
        end
    end

    logger.debug = function(self, ...)
        if DEBUG <= self:getLevel() then
            return LOG_MSG(self, DEBUG, ...)
        end
    end

    logger.info = function(self, ...)
        if INFO <= self:getLevel() then
            return LOG_MSG(self, INFO, ...)
        end
    end

    logger.warn = function(self, ...)
        if WARN <= self:getLevel() then
            return LOG_MSG(self, WARN, ...)
        end
    end

    logger.error = function(self, ...)
        if ERROR <= self:getLevel() then
            return LOG_MSG(self, ERROR, ...)
        end
    end

    logger.trace = function(self, ...)
        if TRACE <= self:getLevel() then
            return LOG_MSG(self, TRACE, ...)
        end
    end

    return logger
end

local logging = new_logger()
logging.DEBUG = DEBUG
logging.INFO = INFO
logging.WARN = WARN
logging.ERROR = ERROR
logging.TRACE = TRACE
logging.tostring = _tostring
logging.new = new_logger
return logging
