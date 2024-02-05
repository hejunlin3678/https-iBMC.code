local utils = require 'utils'
local defs = require 'define'
local cjson = require 'cjson'
local download = require 'download'
local dflib = require "dflib"
local c = require 'dflib.core'
local http = require 'http'
local cfg = require 'config'
local logging = require "logging"
local reply_ok = http.reply_ok
local null = cjson.null
local format = string.format
local GetFileLines = utils.GetFileLines
local file_exists = utils.file_exists
local copy_file = utils.copy_file
local proxy_copy_file = utils.proxy_copy_file
local DFL_OK = dflib.DFL_OK

local TMP_PATH_NO_SLASH = '/tmp'
local TMP_OPERATELOG_BAK_PATH = '/tmp/md_so_operate_log'

local OPERATE_LOG_PATH = '/var/log/pme/operate_log'
local OPERATE_LOG_BAK_PATH = '/var/log/pme/operate_log.tar.gz'
local RUN_LOG_PATH = '/var/log/pme/strategy_log'
local SECURITY_LOG_PATH = '/var/log/pme/security_log'
local SECURITY_LOG_BAK_PATH = '/var/log/pme/security_log.1'

local JSON_SCHEMA_MEMBERS_MAX = 32

-- 串口构造不可见字符会导致安全日志解析失败 --
local function FilterChars(str)
    local tab = {}
    for count = 1, #str do
        local cha = string.byte(str, count)
        if not cha then break end
        if (cha >=32 and cha <=126) then --ASCII码32到126为可见字符--
            table.insert(tab, string.char(cha))
        end
    end
    return table.concat(tab)
end

local parseOperationLog = function(line, index)
    if not line then
        return
    end
    local date, time, interf, userAddress, desp = line:match(
        '([0-9%-]*) ([0-9:]*) ([^,]*),([^,]*),[^,]*,([^\r\n]*)$')
    if not date then
        return
    end
    local fullTime
    if date:len() ~= 0 or time:len() ~= 0 then
        fullTime = date .. ' ' .. time
    end
    local user = ''
    local address = ''
    if userAddress:find('@') then
        local pos = userAddress:len() - userAddress:reverse():find('@') + 1
        user = userAddress:sub(1, pos - 1)
        address = userAddress:sub(pos + 1)
    end
    if desp:len() == 0 then
        return {ID = index, Time = fullTime, Interface = null, UserName = null, IPAdress = null, Content = null}
    else
        return {
            ID = index,
            Time = fullTime,
            Interface = interf,
            UserName = user,
            IPAdress = address,
            Content = FilterChars(desp)
        }
    end
end

local parseRunLog = function(line, index)
    if not line then
        return
    end
    local date, time, level, space, desp = line:match('([0-9%-]*) ([0-9:]*) ([A-Za-z]*)([ ]*): ([^\r\n]*)$')
    if not date then
        return
    end
    local fullTime
    if date:len() ~= 0 or time:len() ~= 0 then
        fullTime = date .. ' ' .. time
    end
    if desp:len() == 0 then
        return {ID = index, Time = fullTime, Level = null, Content = null}
    else
        return {ID = index, Time = fullTime, Level = level, Content = desp}
    end
end

local parseSecurityLog = function(line, index)
    if not line then
        return
    end
    local date, time, host, interf, desp = line:match('([0-9%-]*)T([0-9:]*)[^ ]* ([^ ]*) ([^: ]*): ([^\r\n]*)$')
    if not date then
        return
    end
    local fullTime
    if date:len() ~= 0 or time:len() ~= 0 then
        fullTime = date .. ' ' .. time
    end
    if desp:len() == 0 then
        return {ID = index, Time = fullTime, Interface = null, Host = null, Content = null}
    else
        return {ID = index, Time = fullTime, Interface = interf, Host = host, Content = FilterChars(desp)}
    end
end

local function GetLog(skip, top, path, functionParseLog)
    if top >= JSON_SCHEMA_MEMBERS_MAX then
        top = JSON_SCHEMA_MEMBERS_MAX
    end
    local file = io.open(path, 'r')
    if not file then
        error('open file failed')
    end

    local logs = {}
    local index = 1
    for line in file:read('a'):gmatch('([^\r\n]+)') do
        if index > skip and index <= skip + top then
            logs[#logs + 1] = functionParseLog(line, index)
        end
        index = index + 1
    end
    file:close()

    return {TotalCount = index - 1, Logs = logs}
end

local M = {}

function M.GetOperationLog(data)
    return GetLog(data.Skip, data.Top, OPERATE_LOG_PATH, parseOperationLog)
end

function M.DownloadOperationLog(user)
    local collect_cb = function(filePath)
        if file_exists(OPERATE_LOG_BAK_PATH) then
            local result1 = c.system_s('/bin/tar', '-zxf', OPERATE_LOG_BAK_PATH, '-C', TMP_PATH_NO_SLASH)
            if result1 == DFL_OK then
                local cmd2 = format('/bin/cat %s %s > %s', TMP_OPERATELOG_BAK_PATH, OPERATE_LOG_PATH, filePath)
                local ret = c.system_s('/bin/sh', '-c', cmd2)
                c.rm_filepath(TMP_OPERATELOG_BAK_PATH)
                assert(ret == DFL_OK, 'cat operation log failed')
            else
                logging:error('Decompress operation log package failed')
                copy_file(filePath, OPERATE_LOG_PATH)
            end
        else
            copy_file(filePath, OPERATE_LOG_PATH)
        end

        return reply_ok()
    end

    local progress_cb = function(filePath)
        if file_exists(filePath) then
            return 100
        end
        return 0
    end

    return download(user, 'operate.log', 'operate log', defs.DOWNLOAD_OPERATE_LOG, collect_cb, progress_cb)
end

function M.GetRunLog(data)
    return GetLog(data.Skip, data.Top, RUN_LOG_PATH, parseRunLog)
end

function M.DownloadRunLog(user)
    local collect_cb = function(filePath)
        if not proxy_copy_file(user, RUN_LOG_PATH, filePath, cfg.REDFISH_USER_UID, cfg.APPS_USER_GID, 0) then
            error('copy run log failed')
        end
        return reply_ok()
    end

    local progress_cb = function(filePath)
        if file_exists(filePath) then
            return 100
        end
        return 0
    end

    return download(user, 'runlog.log', 'run log', defs.DOWNLOAD_RUN_LOG, collect_cb, progress_cb)
end

function M.GetSecurityLog(data)
    return GetLog(data.Skip, data.Top, SECURITY_LOG_PATH, parseSecurityLog)
end

function M.DownloadSecurityLog(user)
    local collect_cb = function(filePath)
        if file_exists(SECURITY_LOG_BAK_PATH) then
            local cmd2 = format('/bin/cat %s %s > %s', SECURITY_LOG_BAK_PATH, SECURITY_LOG_PATH, filePath)
            local ret = c.system_s('/bin/sh', '-c', cmd2)
            c.rm_filepath(TMP_OPERATELOG_BAK_PATH)
            assert(ret == DFL_OK, 'cat security log failed')
        else
            copy_file(filePath, SECURITY_LOG_PATH)
        end

        return reply_ok()
    end

    local progress_cb = function(filePath)
        if file_exists(filePath) then
            return 100
        end
        return 0
    end

    return download(user, 'security.log', 'security log', defs.DOWNLOAD_SECURITY_LOG, collect_cb, progress_cb)
end

return M
