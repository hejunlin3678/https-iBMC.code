local dflib = require 'dflib'
local utils = require 'utils'
local http = require 'http'
local logging = require 'logging'
local core = require 'dflib.core'
local xmlhandler = require 'maintenance.xmlhandler'
local O = dflib.object
local flock = core.flock
local GVariant = core.GVariant
local get_file_accessible = core.get_file_accessible
local guint32 = GVariant.new_uint32
local gstring = GVariant.new_string
local OperateLog = utils.OperateLog
local call_method = utils.call_method
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_bad_request = http.reply_bad_request
local reply_internal_server_error = http.reply_internal_server_error
local xml2table = xmlhandler.xml2table
local table2xml = xmlhandler.table2xml

local MAX_WORKRECORD_NUM = 20
local FILE_MODE = 436 -- 436 => 0664
local APACHE_UID = 98
local APPS_USER_GID = 103
local RECORD_FILE = '/data/var/log/pme/record.xml'

local function proxy_create_file(ctx, dest_file, create_flag, user_id, group_id, file_mode)
    return O.PrivilegeAgent:next(function(obj)
        local inputlist = {
            gstring(dest_file), gstring(create_flag), guint32(file_mode), guint32(user_id), guint32(group_id)
        }
        local ok, ret = call_method(ctx, obj, 'CreateFile', nil, inputlist)
        return ok and true or ret
    end):fetch()
end

local function read_file(name)
    local file, _ = io.open(name, 'rb')
    if file == nil then
        logging:error('Unable to read record file')
        return
    end

    local data = file:read('*a')
    file:close()

    if data == nil then
        logging:error('Failed to read record file')
        return
    end

    return data
end

local function save_file(filename, data)
    local file, _ = io.open(filename, 'wb')
    if file == nil then
        logging:error('Unable to write record file')
        return
    end

    local ok, ret = flock(file, "exclusive")
    if not ok then
        file:close()
        logging:error("lock work record file failed: %s", ret)
        return
    end

    file:write(data)
    flock(file, "unlock")
    file:close()
end

local function get_file_length(file)
    if not get_file_accessible(file) then
        return true, 0
    end
    return true, core.get_file_length(file)
end

local function get_records(user)
    local xml = read_file(RECORD_FILE)
    local ok = pcall(function()
                        xml2table(xml)
                    end)
    if not ok then
        O.PrivilegeAgent:call('DeleteFile', nil, {gstring(RECORD_FILE)})
    end

    local ok, file_size = get_file_length(RECORD_FILE)
    if not ok then
        return false
    end

    if file_size <= 0 then
        logging:info('record.xml is not exist or empty! create file')
        local work_record = {recordId = 0, recordCount = 0, records = {}}
        local xml = table2xml(work_record, 'work_record')
        proxy_create_file(user, RECORD_FILE, 'w+', APACHE_UID, APPS_USER_GID, FILE_MODE)
        save_file(RECORD_FILE, xml)
        return true, work_record
    end

    local xml = read_file(RECORD_FILE)
    local root = xml2table(xml)
    local work_record = root.work_record

    if work_record.records == nil or work_record.records.recordInfo == nil then
        return true, work_record
    end
    -- 单个数据的xml不会解析成table，转换成table
    if #work_record.records.recordInfo == 0 then
        work_record.records.recordInfo = {work_record.records.recordInfo}
    end
    if type(work_record.records.recordInfo) ~= 'table' then
        logging:error('recordinfo is not table')
        work_record.records.recordInfo = nil
    end
    return true, work_record
end

local function get_next_availabled_id(records)
    if records == nil or records.recordInfo == nil then
        return 1
    end

    records = records.recordInfo
    local record_id = {}
    for _, record in ipairs(records) do
        local id = tonumber(record.id)
        if id then
            record_id[id] = 1
        end
    end

    for i = 1, MAX_WORKRECORD_NUM do
        if record_id[i] == nil then
            logging:info('next_availabled_id = %d', i)
            return i
        end
    end
end

local function get_record_collection(records)
    if records.recordInfo == nil then
        return {}
    end

    local result = {}
    for i, record in ipairs(records.recordInfo) do
        result[i] = {
            ID = record.id,
            UserName = record.user,
            IPAddress = record.ip,
            Time = record.mod_time,
            Content = record.detail_info
        }
    end
    return result
end

local M = {}

function M.GetWorkRecord(user)
    logging:info(' get_diagnostic_workrecord... ')
    local ok, work_record = get_records(user)
    if not ok then
        return reply_internal_server_error('InternalError')
    end

    local record_list = get_record_collection(work_record.records)
    return {TotalNumber = #record_list, List = record_list}
end

function M.AddWorkRecord(user, data)
    logging:info('act_add_record... ')

    local details = data.Content
    local ok, work_record = get_records(user)
    if not ok then
        OperateLog(user, 'Add Work Record failed')
        return reply_internal_server_error('InternalError')
    end

    if tonumber(work_record.recordCount) >= MAX_WORKRECORD_NUM then
        OperateLog(user, 'Add Work Record failed')
        return reply_bad_request('RecordExceedsMaxNum')
    end

    local next_id = get_next_availabled_id(work_record.records)
    work_record.recordId = next_id
    work_record.recordCount = work_record.recordCount + 1
    local new_record = {
        id = next_id,
        user = user.UserName,
        ip = user.ClientIp,
        mod_time = os.date('%Y/%m/%d %H:%M:%S'),
        detail_info = details
    }

    if work_record.records.recordInfo == nil then
        work_record.records = {}
    end
    local table_temp = work_record.records.recordInfo or {}
    table_temp[#table_temp + 1] = new_record
    work_record.records.recordInfo = table_temp

    local xml = table2xml(work_record, 'work_record')
    save_file(RECORD_FILE, xml)

    OperateLog(user, 'Add Work Record ' .. next_id .. ' successfully')
    return reply_ok_encode_json(M.GetWorkRecord())
end

function M.DeleteWorkRecord(id, user)
    logging:info('act_del_record... ')

    local ok, work_record = get_records(user)
    if not ok then
        OperateLog(user, 'Delete Work Record ' .. id .. ' failed')
        return reply_internal_server_error('InternalError')
    end

    if work_record.records == nil then
        OperateLog(user, 'Delete Work Record ' .. id .. ' failed')
        return reply_bad_request('InvalidRecordId')
    end
    if work_record.records.recordInfo == nil then
        OperateLog(user, 'Delete Work Record ' .. id .. ' failed')
        return reply_bad_request('InvalidRecordId')
    end

    local table_temp = work_record.records.recordInfo
    for i, record in ipairs(table_temp) do
        if record.id == tostring(id) then
            table.remove(table_temp, i)
            work_record.recordCount = work_record.recordCount - 1
            if #table_temp == 0 then
                work_record.records.recordInfo = nil
            end
            local xml = table2xml(work_record, 'work_record')
            save_file(RECORD_FILE, xml)
            OperateLog(user, 'Delete Work Record ' .. id .. ' successfully')
            return reply_ok_encode_json(M.GetWorkRecord())
        end
    end

    OperateLog(user, 'Delete Work Record ' .. id .. ' failed')
    return reply_bad_request('InvalidRecordId')
end

function M.UpdateWorkRecord(user, data)
    logging:info('act_modify_record... ')

    local id = data.ID
    local details = data.Content
    local ok, work_record = get_records(user)
    if not ok then
        OperateLog(user, 'Edit Work Record ' .. id .. ' failed')
        return reply_internal_server_error('InternalError')
    end

    if work_record.records == nil then
        OperateLog(user, 'Edit Work Record ' .. id .. ' failed')
        return reply_bad_request('InvalidRecordId')
    end
    if work_record.records.recordInfo == nil then
        OperateLog(user, 'Edit Work Record ' .. id .. ' failed')
        return reply_bad_request('InvalidRecordId')
    end

    local table_temp = work_record.records.recordInfo
    for _, record in ipairs(table_temp) do
        if record.id == id then
            record.user = user.UserName
            record.ip = user.ClientIp
            record.mod_time = os.date('%Y/%m/%d %H:%M:%S')
            record.detail_info = details
            local xml = table2xml(work_record, 'work_record')
            save_file(RECORD_FILE, xml)
            OperateLog(user, 'Edit Work Record ' .. id .. ' successfully')
            return reply_ok_encode_json(M.GetWorkRecord())
        end
    end

    OperateLog(user, 'Edit Work Record ' .. id .. ' failed')
    return reply_bad_request('InvalidRecordId')
end

return M
