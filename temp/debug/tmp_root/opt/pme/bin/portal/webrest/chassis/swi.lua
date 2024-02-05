local dflib = require 'dflib'
local c = require 'dflib.core'
local utils = require 'utils'
local http = require 'http'
local logging = require 'logging'
local cfg = require 'config'
local C = dflib.class
local reply_bad_request = http.reply_bad_request
local call_method = utils.call_method
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local reply_ok = http.reply_ok
local gstring = GVariant.new_string
local dal_trim_string = utils.dal_trim_string
local file_list = c.file_list
local file_exists = utils.file_exists
local unpack = unpack or table.unpack
local dal_set_file_owner = utils.dal_set_file_owner

local SWI_SLAVEADDR_BASE = 0xC0
local CFG_FILE_PATH = '/data/opt/pme/conf/bladecfg'
local SWITCH_CFG_FILE_BAK_PATH = 'bakcfg'
local BLADE_TYPE_SWI = 0xC0
local SWI_BASE_STR = 'Base'
local SWI_FABRIC_STR = 'Fabric'

local function get_swi_id_from_model_str(swi_model)
    local model_info = {
        ['CX910'] = 1,
        ['CX911'] = 1,
        ['CX912'] = 1,
        ['CX915'] = 6,
        ['CX916'] = 5,
        ['CX916L'] = 5,
        ['CX310'] = 2,
        ['CX311'] = 34,
        ['CX312'] = 18,
        ['CX110'] = 3,
        ['CX111'] = 6,
        ['CX210'] = 1,
        ['CX220'] = 1,
        ['CX710'] = 8,
        ['CX920'] = 11,
        ['CX320'] = 13,
        ['CX930'] = 15,
    }
    return model_info[swi_model]
end

local function get_swi_cfg_info(swi_cfg_list, cfg_file_path, backup_time, swi_info)
    local cfg_file_time_path = cfg_file_path
    if backup_time ~= 'current' then
        cfg_file_time_path = string.format('%s/%s', cfg_file_path, backup_time)
    end
    for _, file_name in ipairs(file_list(cfg_file_time_path)) do
        local name_array = utils.split(file_name, '_')
        local ck_swi_id = tonumber(name_array[1])
        local ck_plane_id = tonumber(name_array[2])
        if swi_info.id == ck_swi_id then
            swi_cfg_list[#swi_cfg_list + 1] = {
                Slot = swi_info.slot,
                BoardType = swi_info.model,
                PlaneType = (ck_plane_id == 0 and SWI_BASE_STR) or SWI_FABRIC_STR,
                FlieName = file_name,
                BackupTime = backup_time,
            }
        end
    end
    return swi_cfg_list
end

local M = {}
function M.GetSwitchFileList()
    local switchFile = {}
    for slot = 1, 4 do
        local swi = {}
        local slaveAddr = SWI_SLAVEADDR_BASE + slot * 2
        swi.slot = slot
        C.IpmbEthBlade('SlaveAddr', slaveAddr):next(function(obj)
            if obj.Presence:fetch() == 0 then
               return switchFile
            end
            local ok, ret = call_method(nil, obj, 'GetBladeInfo', nil, {gbyte(slaveAddr)})
            if not ok then
                return ret
            end
            ret[1][#ret[1]] = nil
            swi.model = dal_trim_string(string.char(unpack(ret[1])))
            ok, ret = call_method(nil, obj, 'DeleteInvalidSwiCfgFile', nil, {gstring(swi.model)})
            if not ok then
                return ret
            end
            local swiName = dflib.object_name(obj)
            swi.id = get_swi_id_from_model_str(swi.model)
            local cfgFilePath = string.format('%s/%s/%s', CFG_FILE_PATH, swiName, SWITCH_CFG_FILE_BAK_PATH)
            for _, fileName in ipairs(file_list(cfgFilePath)) do
                switchFile = get_swi_cfg_info(switchFile, cfgFilePath, fileName, swi)
            end
            cfgFilePath = string.format('%s/%s', CFG_FILE_PATH, swiName)
            switchFile = get_swi_cfg_info(switchFile, cfgFilePath, 'current', swi)
        end)
    end
    return switchFile
end



local function check_slot_id(id)
    return C.IpmbEthBlade('Id', id):next(function(obj)
        if obj.BladeType:fetch() == BLADE_TYPE_SWI then
            return true
        else
            return false
        end
    end):fetch_or(false)
end

local function check_swi_model(model)
    if get_swi_id_from_model_str(model) then
        return true
    end
    return false
end

local function check_action_manager_parameter(data, action_type)
    local slot_id = 32 + data.SlotId
    if not check_slot_id(slot_id) then
        logging:error('check the slot id  %d failed', data.SlotId)
        return reply_bad_request('PropertyValueError', 'Incorrect value of property SlotId')
    end

    if not check_swi_model(data.SwitchModel) then
        logging:error('check the SwitchModel failed')
        return reply_bad_request('PropertyValueError', 'Incorrect value of property SwitchModel')
    end

    if action_type == true and data.BackupTime == 'current' then
        return reply_ok()
    end

    if not string.match(data.BackupTime, '^(%d%d%d%d%-%d%d%-%d%d)$') then
        logging:error('invalid BackupTime')
        return reply_bad_request('PropertyValueError', 'Incorrect value of property BackupTime')
    end
    local swi_name = C.IpmbEthBlade('Id', slot_id):next(function(obj)
        return dflib.object_name(obj)
    end):fetch()
    local dir = string.format('%s/%s/%s/', CFG_FILE_PATH, swi_name, SWITCH_CFG_FILE_BAK_PATH)
    for _, file_name in ipairs(file_list(dir)) do
        if file_name == data.BackupTime then
            return reply_ok()
        end
    end
    return reply_bad_request('PropertyValueError', 'Incorrect value of property BackupTime')
end

function M.GetSwitchFile(data, user)
    local ok, ret
    ret = check_action_manager_parameter(data, true)
    if not ret:isOk() then
        return ret
    end
    local slotId = 32 + data.SlotId
    return C.IpmbEthBlade('Id', slotId):next(function(obj)
        local input = {gstring(data.SwitchModel), gbyte(data.FruId), gstring(data.BackupTime)}
        ok, ret = call_method(user, obj, 'ParseSwiCfgFile', nil, input)
        if not ok then
            return ret
        end
        local cfgFilepath = ret[1]
        if not file_exists(cfgFilepath) then
            return reply_bad_request('FileNotExist', 'The file does not exist.')
        end
        dal_set_file_owner(cfgFilepath, cfg.APACHE_UID, cfg.APACHE_GID)
        local file = io.open(cfgFilepath, 'r')
        local body = file:read('*all')
        file:close()
        local header = {['content-type'] = "application/octet-stream"}
        return reply_ok(body, header)
    end):fetch()
end


function M.RestoreSwiProfile(data, user)
    local ok, ret
    ret = check_action_manager_parameter(data, false)
    if not ret:isOk() then
        logging:error('check_action_manager_parameter failed. ret = %d', ret)
        return ret
    end
    local slotId = 32 + data.SlotId
    return C.IpmbEthBlade('Id', slotId):next(function(obj)
        local input = {gstring(data.SwitchModel), gbyte(data.FruId), gstring(data.BackupTime)}
        ok, ret = call_method(user, obj, 'RestoreSwiCfgFile', nil, input)
        if not ok then
            return ret
        end
        return reply_ok()
    end):fetch()
end

return M
