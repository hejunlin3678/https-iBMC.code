local dflib = require 'dflib'
local cfg = require 'config'
local bit = require 'bit'
local c = require 'dflib.core'
local cjson = require 'cjson'
local logging = require 'logging'
local tasks = require 'tasks'
local http = require 'http'
local portal = require "portal"
local bit_and = bit.band
local bit_rshift = bit.rshift
local C = dflib.class
local null = cjson.null
local t_insert = table.insert
local t_move = table.move
local GVariant = c.GVariant
local g_string = GVariant.new_string
local gint32 = GVariant.new_int32
local guint32 = GVariant.new_uint32
local is_gvariant = GVariant.is_gvariant
local get_position = dflib.get_position
local class_name = dflib.class_name
local object_name = dflib.object_name
local reply_bad_request = http.reply_bad_request
local reply_internal_server_error = http.reply_internal_server_error
local is_http_reply = http.is_reply
local DFL_OK = dflib.DFL_OK
local RFERR_SUCCESS = dflib.RFERR_SUCCESS
local O = dflib.object
local string_format = string.format
local gstring = GVariant.new_string
local gbyte = GVariant.new_byte
local realpath = c.realpath
local get_file_length = c.get_file_length
local get_file_accessible = c.get_file_accessible
local copy_file = c.copy_file
local is_dir = c.is_dir
local string_lower = string.lower

local MAX_FILEPATH_LENGTH = 256

local FILE_MODE = bit.bor(0400, 0200)

local CHASSIS_TYPE_MASK = cfg.CHASSIS_TYPE_MASK
local CHASSIS_TYPE_SWITCH = cfg.CHASSIS_TYPE_SWITCH
local CHASSIS_TYPE_BLADE = cfg.CHASSIS_TYPE_BLADE
local CHASSIS_TYPE_X_SERIAL = cfg.CHASSIS_TYPE_X_SERIAL
local CHASSIS_TYPE_MM = cfg.CHASSIS_TYPE_MM
local CHASSIS_TYPE_RACK = cfg.CHASSIS_TYPE_RACK

local MGMT_SOFTWARE_TYPE_EM = cfg.MGMT_SOFTWARE_TYPE_EM
local MGMT_SOFTWARE_TYPE_EM_X86_BMC = cfg.MGMT_SOFTWARE_TYPE_EM_X86_BMC
local MGMT_SOFTWARE_TYPE_RM = cfg.MGMT_SOFTWARE_TYPE_RM

local BOARD_SWITCH = cfg.BOARD_SWITCH
local BOARD_BLADE = cfg.BOARD_BLADE
local BOARD_MM = cfg.BOARD_MM
local BOARD_RM = cfg.BOARD_RM
local BOARD_HMM = cfg.BOARD_HMM
local BOARD_RACK = cfg.BOARD_RACK
local BOARD_OTHER = cfg.BOARD_OTHER

local RFPROP_MANAGER_MANAGER_IPV4_MODE0 = cfg.RFPROP_MANAGER_MANAGER_IPV4_MODE0
local RFPROP_MANAGER_MANAGER_IPV4_MODE1 = cfg.RFPROP_MANAGER_MANAGER_IPV4_MODE1
local RFPROP_MANAGER_MANAGER_IPV4_MODE2 = cfg.RFPROP_MANAGER_MANAGER_IPV4_MODE2

local LOGENTRY_STATUS_INFORMATIONAL_CODE = cfg.LOGENTRY_STATUS_INFORMATIONAL_CODE
local LOGENTRY_STATUS_MINOR_CODE = cfg.LOGENTRY_STATUS_MINOR_CODE
local LOGENTRY_STATUS_MAJOR_CODE = cfg.LOGENTRY_STATUS_MAJOR_CODE
local LOGENTRY_STATUS_CRITICAL_CODE = cfg.LOGENTRY_STATUS_CRITICAL_CODE

local LOGENTRY_SEVERITY_OK = cfg.LOGENTRY_SEVERITY_OK
local LOGENTRY_SEVERITY_WARNING = cfg.LOGENTRY_SEVERITY_WARNING
local LOGENTRY_SEVERITY_CRITICAL = cfg.LOGENTRY_SEVERITY_CRITICAL

local CRITICAL_STRING = cfg.CRITICAL_STRING
local MINOR_STRING = cfg.MINOR_STRING
local MAJOR_STRING = cfg.MAJOR_STRING
local INFORMATIONAL_STRING = cfg.INFORMATIONAL_STRING

local CDTE = cfg.COMPONENT_DEVICE_TYPE_E

local PRODUCT_ID_ATLAS = cfg.PRODUCT_ID_ATLAS
local PRODUCT_ID_XSERIAL_SERVER = cfg.PRODUCT_ID_XSERIAL_SERVER
local COMPONENT_TYPE_BACKPLANE_BOARD = 22
local ASSET_MGMT_SVC_ON = 1

local BOARD_MM_HIGH = 5
local BOARD_RM_HIGH = 6
local BOARD_MM_LOW = 0

local FN_TYPE_INNER_SHELF = cfg.FN_TYPE_INNER_SHELF

local board_chassis_info = {
    {CDTE.COMPONENT_TYPE_IO_BOARD, 'IOCardNum', 'IOM', 'Module'},
    {CDTE.COMPONENT_TYPE_GPU_BOARD, 'GPUBoardNum', 'Drawer', 'Drawer'},
    {CDTE.COMPONENT_TYPE_CHASSIS, 'NodeNum', 'Blade', 'Blade'},
    {COMPONENT_TYPE_BACKPLANE_BOARD, nil, 'Enc', 'Enclosure'}, {CDTE.COMPONENT_TYPE_HMM, 'MMNum', 'SMM', 'Module'},
    {CDTE.COMPONENT_TYPE_UNIT, 'UnitNum', 'U', 'Module'}
}

local CLASS_HDDBACKPLANE_NAME = "HDDBackplane"
local CLASS_RAIDCARD_NAME = "Raid"
local CLASS_PCIECARD_NAME = "PcieCard" -- PCIE标卡类
local CLASS_MEZZ = "MezzCard"
local CLASS_NETCARD_NAME = "NetCard"
local CLASS_MEMBOARD_NAME = "MemoryBoard"
local CLASS_RISERPCIECARD_NAME = "RiserPcieCard" -- riser卡
local CLASS_MAINBOARD_NAME = "MainBoard"
local CLASS_FANBOARD_NAME = "FANBoard"
local CLASS_CARD_NAME = "Card"
local CLASS_IOBOARD_NAME = "IOBoard"
local CLASS_CPUBOARD_NAME = "CpuBoard"
local CLASS_M2TRANSFORMCARD = "M2TransformCard"
local RFPROP_CLASS_LEDBOARD = "LedBoard"
local RFPROP_TYPE_TWIN_NODE_BACKPLANE = "TwinNodeBackplane"
local RFPROP_TYPE_BOARD_PCIE_ADAPTER = "PCIeTransformCard"
local CLASS_GPUBOARD_NAME = "GPUBoard"
local RFPROP_TYPE_PASS_THROUGH_CARD = "PassThroughCard"
local CLASS_HORIZ_CONN_BRD = "HorizConnectionBoard"
local CLASS_PERIPHERAL_CARD_NAME = "PeripheralCard"
local CLASS_ASSET_LOCATE_BOARD = "AssetLocateBoard"

local DEVICETYPE_2_CLASSNAME = {
    [CDTE.COMPONENT_TYPE_HDD_BACKPLANE] = CLASS_HDDBACKPLANE_NAME,
    [CDTE.COMPONENT_TYPE_RAID_CARD] = CLASS_RAIDCARD_NAME,
    [CDTE.COMPONENT_TYPE_PCIE_CARD] = CLASS_PCIECARD_NAME,
    [CDTE.COMPONENT_TYPE_MEZZ] = CLASS_MEZZ,
    [CDTE.COMPONENT_TYPE_NIC_CARD] = CLASS_NETCARD_NAME,
    [CDTE.COMPONENT_TYPE_MEMORY_RISER] = CLASS_MEMBOARD_NAME,
    [CDTE.COMPONENT_TYPE_PCIE_RISER] = CLASS_RISERPCIECARD_NAME,
    [CDTE.COMPONENT_TYPE_MAINBOARD] = CLASS_MAINBOARD_NAME,
    [CDTE.COMPONENT_TYPE_FAN_BACKPLANE] = CLASS_FANBOARD_NAME,
    [CDTE.COMPONENT_TYPE_SWITCH_MEZZ] = CLASS_CARD_NAME, -- 交换扣卡属Card类管理
    [CDTE.COMPONENT_TYPE_IO_BOARD] = CLASS_IOBOARD_NAME,
    [CDTE.COMPONENT_TYPE_BOARD_CPU] = CLASS_CPUBOARD_NAME,
    [CDTE.COMPONENT_TYPE_EXPAND_BOARD] = CLASS_M2TRANSFORMCARD, -- m.2转接卡
    [CDTE.COMPONENT_TYPE_LED_BOARD] = RFPROP_CLASS_LEDBOARD,
    [CDTE.COMPONENT_TYPE_PIC_CARD] = CLASS_CARD_NAME,
    [CDTE.COMPONENT_TYPE_TWIN_NODE_BACKPLANE] = RFPROP_TYPE_TWIN_NODE_BACKPLANE,
    [CDTE.COMPONENT_TYPE_BOARD_PCIE_ADAPTER] = RFPROP_TYPE_BOARD_PCIE_ADAPTER,
    [CDTE.COMPONENT_TYPE_GPU_BOARD] = CLASS_GPUBOARD_NAME,
    [CDTE.COMPONENT_TYPE_PASS_THROUGH_CARD] = RFPROP_TYPE_PASS_THROUGH_CARD,
    [CDTE.COMPONENT_TYPE_PSU_BACKPLANE] = CLASS_CARD_NAME,
    [CDTE.COMPONENT_TYPE_HORIZ_CONN_BRD] = CLASS_HORIZ_CONN_BRD,
    [CDTE.COMPONENT_TYPE_CHASSIS_BACKPLANE] = CLASS_PERIPHERAL_CARD_NAME,
    [CDTE.COMPONENT_TYPE_ASSET_LOCATE_BRD] = CLASS_ASSET_LOCATE_BOARD
}

local utils = {}

local function map_board_type(mgmt_software_type)
    local board_type
    local chassis_type = bit_and(mgmt_software_type, CHASSIS_TYPE_MASK)
    if CHASSIS_TYPE_SWITCH == chassis_type then
        board_type = BOARD_SWITCH
    elseif CHASSIS_TYPE_BLADE == chassis_type or CHASSIS_TYPE_X_SERIAL == chassis_type then
        board_type = BOARD_BLADE
    elseif (CHASSIS_TYPE_MM == chassis_type) then
        board_type = BOARD_MM
    elseif (CHASSIS_TYPE_RACK == chassis_type) then
        board_type = BOARD_RACK
    else
        board_type = BOARD_OTHER
    end

    if MGMT_SOFTWARE_TYPE_EM == mgmt_software_type then
        board_type = BOARD_HMM
    elseif MGMT_SOFTWARE_TYPE_EM_X86_BMC == mgmt_software_type then
        board_type = BOARD_BLADE
    elseif MGMT_SOFTWARE_TYPE_RM == mgmt_software_type then
        board_type = BOARD_RM
    end
    return board_type
end

local function format_bold_slot(board_type, slot_id)
    -- 前插板Blade+slot_id,后插板Swi+slot_id
    if board_type == BOARD_SWITCH then
        return string.format('Swi%d', slot_id - 32)
    elseif board_type == BOARD_BLADE then
        return string.format('Blade%d', slot_id)
    elseif board_type == BOARD_MM then
        return string.format('SMM%d', slot_id)
    elseif board_type == BOARD_HMM then
        return string.format('HMM%d', slot_id)
    elseif board_type == BOARD_RM then
        return 'Rack'
    else
        return '1'
    end
end

local function dal_rf_get_board_type(product)
    local product = product or C.PRODUCT[0]
    return product.MgmtSoftWareType:next(function(mgmt_software_type)
        return map_board_type(mgmt_software_type)
    end)
end

local function dal_rf_get_board_slot(board_type, board)
    local board = board or C.BOARD[0]
    return board.SlotId:next(function(slot_id)
        return format_bold_slot(board_type, slot_id)
    end)
end

utils.dal_rf_get_board_type = dal_rf_get_board_type
utils.dal_rf_get_board_slot = dal_rf_get_board_slot

function utils.get_property_and_severity(health)
    if not health then
        return nil, nil
    elseif health == LOGENTRY_STATUS_INFORMATIONAL_CODE then
        return LOGENTRY_SEVERITY_OK, INFORMATIONAL_STRING
    elseif health == LOGENTRY_STATUS_MINOR_CODE then
        return LOGENTRY_SEVERITY_WARNING, MINOR_STRING
    elseif health == LOGENTRY_STATUS_MAJOR_CODE then
        return LOGENTRY_SEVERITY_WARNING, MAJOR_STRING
    elseif health == LOGENTRY_STATUS_CRITICAL_CODE then
        return LOGENTRY_SEVERITY_CRITICAL, CRITICAL_STRING
    else
        return null, null
    end
end

function utils.get_ip_address(IPMode, IPv4Address)
    if IPMode == RFPROP_MANAGER_MANAGER_IPV4_MODE0 or IPMode == RFPROP_MANAGER_MANAGER_IPV4_MODE1 then
        return {Mode = 'Static', Address = IPv4Address}
    elseif IPMode == RFPROP_MANAGER_MANAGER_IPV4_MODE2 then
        return {Mode = 'DHCP', Address = nil}
    end
end

function utils.fix_v3_guid(guid)
    local tmp = {}
    t_move(guid, 13, 16, #tmp + 1, tmp)
    t_move(guid, 11, 12, #tmp + 1, tmp)
    t_move(guid, 9, 10, #tmp + 1, tmp)
    for i = 8, 1, -1 do
        t_insert(tmp, guid[i])
    end
    return tmp
end

local ROLEID_TO_STR = {
    [cfg.UUSER_ROLE_USER] = cfg.USER_ROLE_COMMONUSER,
    [cfg.UUSER_ROLE_OPER] = cfg.USER_ROLE_OPERATOR,
    [cfg.UUSER_ROLE_ADMIN] = cfg.USER_ROLE_ADMIN,
    [cfg.UUSER_ROLE_CUSt1] = cfg.USER_ROLE_CUSTOMROLE1,
    [cfg.UUSER_ROLE_CUSt2] = cfg.USER_ROLE_CUSTOMROLE2,
    [cfg.UUSER_ROLE_CUSt3] = cfg.USER_ROLE_CUSTOMROLE3,
    [cfg.UUSER_ROLE_CUSt4] = cfg.USER_ROLE_CUSTOMROLE4,
    [cfg.UUSER_ROLE_NOACCESS] = cfg.USER_ROLE_NOACCESS
}
function utils.roleid_to_str(roleId)
    return ROLEID_TO_STR[roleId] or 'Unknown'
end

local USER_PARAMETER_OUT_OF_RANGE = 0xC9
local RFERR_INSUFFICIENT_PRIVILEGE = 403
local RFERR_WRONG_PARAM = 400
local RFERR_NO_RESOURCE = 404
local RFERR_GENERAL = 500
local commonErrMap = {
    [USER_PARAMETER_OUT_OF_RANGE] = reply_bad_request('PropertyValueNotInList'),
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request('InsufficientPrivilege'),
    [RFERR_WRONG_PARAM] = reply_bad_request('WrongParam'),
    [RFERR_NO_RESOURCE] = reply_bad_request('ResourceMissingAtURI'),
    [RFERR_GENERAL] = reply_bad_request('GeneralError')
}

local function mapCallError(err, errMap)
    if not err or not errMap then
        return
    end

    local errId = (type(err) ~= 'string') and err or tonumber(err:match('.-(-?%d+)$', -10))
    if not errId then
        return
    end

    local errReply = nil
    local ty = type(errMap)
    if ty == 'table' then
        errReply = errMap[errId]
    elseif ty == 'function' then
        errReply = errMap(errId)
    end
    if errReply then
        return errReply
    end

    return commonErrMap[errId]
end

local gWebUiFlag = g_string('WEB')
local gRedfishFlag = g_string('Redfish')

utils.gWebUiFlag = gWebUiFlag

local function call_method(callerInfo, obj, methodName, errMap, inputs, ...)
    if inputs and is_gvariant(inputs) then
        inputs = {inputs, ...}
    end
    local ok, ret = pcall(obj.call, obj, methodName, callerInfo, inputs)
    if not ok then
        local newErr = mapCallError(ret, errMap)
        if newErr then
            -- 找到错误映射
            if not is_http_reply(newErr) then
                newErr = reply_bad_request(nil, tostring(newErr))
            else
                newErr = newErr:clone()
            end
            return false, newErr
        end

        -- 没有找到错误映射,抛出异常由框架统一处理
        error(ret)
    end
    return true, ret
end

local function call_method_async(callerInfo, obj, methodName, errMap, inputs, ...)
    if inputs and is_gvariant(inputs) then
        inputs = {inputs, ...}
    end
    local ok, work = pcall(obj.async_call, obj, methodName, callerInfo, inputs)
    if not ok then
        return false, reply_internal_server_error(nil, work)
    end
    if not work then
        return false, reply_bad_request()
    end
    while true do
        tasks.sleep(100)
        local complete, ok, ret = work:query()
        if complete then
            work:unref()
            if ok ~= DFL_OK and ok ~= RFERR_SUCCESS then
                local newErr = mapCallError(ok, errMap)
                if newErr then
                    -- 找到错误映射
                    if not is_http_reply(newErr) then
                        newErr = reply_bad_request(nil, tostring(newErr))
                    else
                        newErr = newErr:clone()
                    end
                    return false, newErr
                end

                -- 没有找到错误映射,抛出异常由框架统一处理
                error(string.format('call object method (%s.%s) failed: %d', dflib.class_name(obj), methodName, ok))
            end
            return true, ret
        end
    end
end

local function call_remote_method2_async(callerInfo, ip, port, obj, methodName, errMap, inputs, ...)
    if inputs and is_gvariant(inputs) then
        inputs = {inputs, ...}
    end
    local ok, work = pcall(obj.remote_async_call, ip, port, obj, methodName, callerInfo, inputs)
    if not ok then
        return false, reply_internal_server_error(nil, work)
    end
    if not work then
        return false, reply_bad_request()
    end
    while true do
        tasks.sleep(100)
        local complete, ok, ret = work:query()
        if complete then
            work:unref()
            if ok ~= DFL_OK and ok ~= RFERR_SUCCESS then
                local newErr = mapCallError(ok, errMap)
                if newErr then
                    -- 找到错误映射
                    if not is_http_reply(newErr) then
                        newErr = reply_bad_request(nil, tostring(newErr))
                    else
                        newErr = newErr:clone()
                    end
                    return false, newErr
                end

                -- 没有找到错误映射,抛出异常由框架统一处理
                error(string.format('call object method (%s.%s) failed: %d', dflib.class_name(obj), methodName, ok))
            end
            return true, ret
        end
    end
end

local function webui_call_method(user, obj, methodName, errMap, inputs, ...)
    local callerInfo = nil
    if user then
        callerInfo = user.callerInfo or {gWebUiFlag, g_string(user.UserName), g_string(user.ClientIp)}
    end
    return call_method_async(callerInfo, obj, methodName, errMap, inputs, ...)
end

local function webui_remote_call_method2(user, obj, methodName, errMap, inputs, ...)
    local callerInfo = nil
    if user then
        callerInfo = user.callerInfo or {gWebUiFlag, g_string(user.UserName), g_string(user.ClientIp)}
    end
    if O.AsmObject.AsStatus:fetch() == 0 then
        return call_method_async(callerInfo, obj, methodName, errMap, inputs, ...)
    else
        local ip, port = utils.GetRemoteIpAddrAndPort()
        return call_remote_method2_async(callerInfo, ip, port, obj, methodName, errMap, inputs, ...)
    end
end

local function redfish_call_method(user, obj, methodName, errMap, inputs, ...)
    local callerInfo = nil
    if user then
        callerInfo = user.callerInfo or {gRedfishFlag, g_string(user.UserName), g_string(user.ClientIp)}
    end
    return call_method_async(callerInfo, obj, methodName, errMap, inputs, ...)
end

-- call_method 也使用异步方法吧
utils.call_method = webui_call_method
utils.call_method_async = webui_call_method
-- 共享器件远程调用主控方法
utils.call_remote_method2_async = webui_remote_call_method2

-- 共享器件远程调用主控方法
utils.call_remote_method2_async = webui_remote_call_method2

utils.redfish_call_method = redfish_call_method
utils.redfish_call_method_async = redfish_call_method

local OPT_PME_PRAM_FOLDER = '/opt/pme/pram/'
local function makeHistoryFilePath(fileName)
    -- 限定文件名中不允许出现 . / \ 字符
    local name, _ext = fileName:match('^([^./\\]+)%.?([^./\\]*)$')
    if not name then
        error('invalid history file name')
    end
    return OPT_PME_PRAM_FOLDER .. fileName
end

function utils.read_history(fileName, cb)
    local path = makeHistoryFilePath(fileName)
    local file, _ = io.open(path, 'r')
    if not file then
        error('open history file failed')
    end

    local ok, datas = pcall(function()
        local datas = {}
        local skipFirstLine = true
        for line in file:lines() do
            if skipFirstLine then
                skipFirstLine = false
            else
                local fields = {}
                line:gsub('([^#]*)#?', function(c)
                    fields[#fields + 1] = c
                end)
                if cb then
                    datas[#datas + 1] = cb(fields)
                else
                    datas[#datas + 1] = fields
                end
            end
        end
        return datas
    end)
    file:close()
    if not ok then
        error(datas)
    end
    return datas
end

function utils.rf_string_check(data)
    if not data or #data == 0 or data == 'N/A' or data == 'Unknown' or data == 'NA' or data == 'null' or data ==
        'Undefined' then
        return null
    end
    return data
end

function utils.OnOffToStr(v)
    return v == 0 and 'Off' or 'On'
end

function utils.OnOffStrToValue(v)
    if v == 'Off' then
        return 0
    elseif v == 'On' then
        return 1
    end
end

local SUBJECT_TYPE_UNKNOWN = 'UNKNOWN'
local m_event_subject_type = {
    'CPU',
    'Memory',
    'Disk',
    'PSU',
    'Fan',
    'Disk Backplane',
    'RAID Card',
    SUBJECT_TYPE_UNKNOWN,
    'PCIe Card',
    'AMC',
    'HBA',
    'Mezz Card',
    SUBJECT_TYPE_UNKNOWN,
    'NIC',
    'Memory Board',
    'PCIe Riser',
    'Mainboard',
    'LCD',
    'Chassis',
    'NCM',
    'Switch Module',
    'Storage Board',
    'Chassis Backplane',
    'HMM/CMC',
    'Fan Backplane',
    'PSU Backplane',
    'BMC',
    'MM/MMC',
    'Twin Node Backplane',
    'Base Plane',
    'Fabric Plane',
    'Switch Mezz',
    'LED',
    'SD Card',
    'Security Module',
    'I/O Board',
    'CPU Board',
    'RMC',
    'PCIe Adapter',
    'PCH',
    'Cable',
    'Port',
    'LSW',
    'PHY',
    'System',
    'M.2 Transfer Card',
    'LED Board',
    'LPM',
    'PIC Card',
    'Button',
    'Expander',
    'CPI',
    'ACM',
    'CIM',
    'PFM',
    'KPAR',
    'JC',
    'SCM',
    'Minisas HD channel',
    'SATA DOM channel',
    'GE channel',
    'XGE channel',
    'PCIe Switch',
    'Interface Device',
    'xPU Carrier Board',
    'Disk BaseBoard',
    'VGA Interface Card',
    'Pass-Through Card',
    'Logical Driver',
    'PCIe Retimer',
    'PCIe Repeater',
    'SAS',
    'Memory Channel',
    'BMA',
    'LOM',
    'Signal Adapter Board',
    'Horizontal Connection Board',
    'Node',
    'Asset Locate Board',
    'Unit',
    'RMM',
    'Rack',
    'BBU',
    'OCP Card',
    'Leakage Detection Card',
    'MESH Card',
    'NPU',
    'CIC Card',
    "Expansion Module",
    "Fan Module",
    "AR Card",
    SUBJECT_TYPE_UNKNOWN,
    'SoC Board',
    "Expand Board"
}

function utils.get_event_subtype()
    return C.Component():fold(function(obj, acc)
        local deviceType = obj.DeviceType:fetch_or(false)
        if deviceType then
            local typeStr = m_event_subject_type[deviceType + 1]
            if typeStr and typeStr ~= SUBJECT_TYPE_UNKNOWN then
                acc[deviceType] = typeStr
            end
        end
        return acc
    end, {}):fetch()
end

function utils.BoolToValue(v)
    if v == true then
        return 1
    elseif v == false then
        return 0
    end
end

function utils.ValueToBool(v)
    if v == 1 then
        return true
    else
        return false
    end
end

function utils.ValueToBoolNoDefault(v)
    if v == 1 then
        return true
    elseif v == 0 then
        return false
    else
        return null
    end
end

function utils.GetFileLines(path)
    local count = 0
    local file = io.open(path, 'r')
    if (not file) then
        error('open file failed')
    end
    for _ in file:lines() do
        count = count + 1
    end
    file:close()
    return count
end

local function file_exists(filename)
    return get_file_accessible(filename)
end

local function file_length(filename)
    return get_file_length(filename)
end

utils.file_exists = file_exists
utils.file_length = file_length
utils.copy_file = function(desfilepath, sourcepath)
    if desfilepath:match('/$') then
        local name = sourcepath:match("[^/\\]+$") or ''
        desfilepath = desfilepath .. name
    end
    return copy_file(desfilepath, sourcepath) == DFL_OK
end

function utils.dal_set_file_owner(path, uid, gid)
    return O.PrivilegeAgent:next(function(obj)
        local inputList = {gstring(path), guint32(uid), guint32(gid)}
        local ok, _ = call_method(nil, obj, 'ModifyFileOwn', nil, inputList)
        return ok and true or false
    end):fetch_or(false)
end

function utils.dal_set_file_mode(path, mode)
    return O.PrivilegeAgent:next(function(obj)
        local inputList = {gstring(path), guint32(mode)}
        local ok, _ = call_method(nil, obj, 'ModifyFileMode', nil, inputList)
        return ok and true or false
    end):fetch_or(false)
end

function utils.proxy_copy_file(ctx, srcFile, destFile, userId, groupId, deleteFlag)
    return O.PrivilegeAgent:next(function(obj)
        local inputList = {
            gstring(srcFile), gstring(destFile), guint32(userId), guint32(groupId), guint32(deleteFlag)
        }
        local ok, _ = call_method(ctx, obj, 'CopyFile', nil, inputList)
        return ok and true or false
    end):fetch_or(false)
end

local function split(str, delimiter)
    if str == nil or str == '' or delimiter == nil then
        return nil
    end

    local result = {}
    for match in (str .. delimiter):gmatch('(.-)' .. delimiter) do
        table.insert(result, match)
    end
    return result
end

utils.split = split

local USERROLE_READONLY = 0x01
local USERROLE_DIAGNOSEMGNT = 0x02
local USERROLE_SECURITYMGNT = 0x04
local USERROLE_BASICSETTING = 0x08
local USERROLE_USERMGNT = 0x10
local USERROLE_POWERMGNT = 0x20
local USERROLE_VMMMGNT = 0x40
local USERROLE_KVMMGNT = 0x80

local function HasPrivilege(ctx, priv)
    return bit.band(ctx.Privilege or 0, priv) ~= 0
end

function utils.HasReadonlyPrivilege(ctx)
    return HasPrivilege(ctx, USERROLE_READONLY)
end

function utils.HasDiagnoseMgntPrivilege(ctx)
    return HasPrivilege(ctx, USERROLE_DIAGNOSEMGNT)
end

function utils.HasUserMgntPrivilege(ctx)
    return HasPrivilege(ctx, USERROLE_USERMGNT)
end

function utils.HasSecurityMgntPrivilege(ctx)
    return HasPrivilege(ctx, USERROLE_SECURITYMGNT)
end

function utils.HasBasicSettingPrivilege(ctx)
    return HasPrivilege(ctx, USERROLE_BASICSETTING)
end

function utils.HasPowerMgntPrivilege(ctx)
    return HasPrivilege(ctx, USERROLE_POWERMGNT)
end

function utils.HasVmmMgntPrivilege(ctx)
    return HasPrivilege(ctx, USERROLE_VMMMGNT)
end

function utils.HasKvmMgntPrivilege(ctx)
    return HasPrivilege(ctx, USERROLE_KVMMGNT)
end

function utils.OperateLog(user, fmt, ...)
    local msg
    if select('#', ...) > 0 then
        msg = string_format(fmt, ...)
    else
        msg = fmt
    end
    O.log_0:next(function(obj)
        webui_call_method(user, obj, 'OperateLogProxy', nil, {gstring('portal'), gstring(msg)})
    end):fetch()
end

function utils.RedfishOperateLog(user, fmt, ...)
    local msg
    if select('#', ...) > 0 then
        msg = string_format(fmt, ...)
    else
        msg = fmt
    end
    O.log_0:next(function(obj)
        redfish_call_method(user, obj, 'OperateLogProxy', nil, {gstring('portal'), gstring(msg)})
    end):fetch()
end

function utils.dal_trim_string(data)
    if not data or data == null then
        return null
    end
    return data:match('^%s*(.-)%s*$')
end

function utils.dal_clear_string_blank(data)
    if not data or data == null then
        return null
    end
    return data:gsub(' ', '')
end

function utils.redfish_strip_to_int(data)
    if data and data ~= null then
        local numStr = data:match('^([%s%d]+)[^%d]*$')
        numStr = numStr and numStr:gsub('%s', '')
        return numStr and tonumber(numStr)
    end
end

function utils.IntToHexStr(data)
    return data and string_format('0x%04x', data)
end

local function real_file_path(path)
    if not path then
        return false
    end
    local len = #path
    if len > MAX_FILEPATH_LENGTH or len == 0 then
        return false
    end
    if path:byte(len) == 47 then
        return false
    end
    local dir, file = path:match('(.+/)([^/]+)$')
    if not file then
        return false
    end
    if not dir then
        return false
    end
    if file == '.' or file == '..' then
        return false
    end
    local realPath = realpath(dir)
    if not realPath then
        return false
    end
    local realPathLen = #realPath
    if realPathLen > MAX_FILEPATH_LENGTH or realPathLen == 0 then
        return false
    end
    if realPath:byte(realPathLen) ~= 47 then
        realPath = realPath .. '/'
    end

    return realPath
end

local function is_tmp_path(path)
    return path:match('^/tmp/') ~= nil
end

local function is_tmp_web_path(path)
    return path:match('^/tmp/web') ~= nil
end

utils.real_file_path = real_file_path
utils.is_tmp_path = is_tmp_path
utils.is_tmp_web_path = is_tmp_web_path

function utils.rf_check_file_path(path)
    local realPath = real_file_path(path)
    if not realPath or not is_tmp_path(realPath) or not file_exists(realPath) then
        return false
    end

    return true
end

local function HasRoleId(user, roleId)
    for v in user.RoleId:gmatch('(%d+)') do
        if tonumber(v) == roleId then
            return true
        end
    end
end

utils.HasRoleId = HasRoleId

local function dal_get_uid_and_gid(userName)
    return C.User[0]:next(function(obj)
        local ok, ret = redfish_call_method(nil, obj, 'GetUidGidByUsername', nil, {gstring(userName)})
        return ok and ret
    end):fetch_or()
end

local function get_hashtable_file_owner(path)
    return O.BMC:next(function(obj)
        local ok, ret = redfish_call_method(nil, obj, 'GetFileOwner', nil, {gstring(path)})
        return ok and ret[1]
    end):fetch_or()
end

local APACHE_UID = 98
local REDFISH_USER_UID = 102
local UUSER_ROLE_ADMIN = cfg.UUSER_ROLE_ADMIN
local LOG_TYPE_LOCAL = 0

local function dal_check_file_owner(userName, path, islocal)
    local uid = c.file_stat(path).st_uid
    if islocal then
        local ret = dal_get_uid_and_gid(userName)
        return ret and uid == ret[1]
    end
    return uid == APACHE_UID or uid == REDFISH_USER_UID and get_hashtable_file_owner(path) == userName
end

local function dal_check_file_opt_user(user, path)
    return HasRoleId(user, UUSER_ROLE_ADMIN) or
               dal_check_file_owner(user.UserName, path, user.AuthType == LOG_TYPE_LOCAL)
end

utils.dal_check_file_owner = dal_check_file_owner
utils.dal_check_file_opt_user = dal_check_file_opt_user

local function FilterLanguageSet(all_language_set, filter)
    local language_set = ""
    local language_support = O.BMC.LanguageSetSupport:fetch_or()
    local language_set_arr = split(all_language_set, ',')

    for _, language in ipairs(language_set_arr) do
        if not string.find(language_set, language) then
            if not filter or string.find(language_support, language) then
                if #language_set == 0 then
                    language_set = language
                else
                    language_set = language_set .. ',' .. language
                end
            end
        end
    end
    return language_set
end

function utils.GetLanguageSet(filter)
    return C.BMC[0]:next(function(obj)
        local all_language_set
        local language_set, language_extend = obj.LanguageSet:fetch_or(), obj.LanguageSetExtend:fetch_or()
        if language_set and language_extend then
            all_language_set = language_set .. ',' .. language_extend
        else
            all_language_set = language_set or language_extend or ''
        end
        return FilterLanguageSet(all_language_set, filter)
    end):fetch()
end

local G_CHECKSUM_SHA256 = 2
local REFRESH_ENABLE = 1
function utils.DeleteOldSession(ipaddr, old_session_id)
    C.Session[0]:next(function(session_obj)
        local old_session_hash = c.compute_checksum(G_CHECKSUM_SHA256, old_session_id):sub(1, 16)
        local caller_info = {gstring('Redfish'), gint32(REFRESH_ENABLE)}
        local old_session_rsp = session_obj:call('GetOnlineUser', caller_info, {gstring(old_session_hash)})
        if old_session_rsp[1] and #old_session_rsp[1] > 0 then
            local old_session = cjson.decode(old_session_rsp[1])
            local caller_type = (old_session.session_type == 'GUI') and 'WEB' or 'Redfish'
            caller_info = {gstring(caller_type), gstring(old_session.loginname), gstring(ipaddr)}
            session_obj:call('DelSession', caller_info, {gstring(old_session_hash)})
        end
    end)
end

function utils.ReAuthUser(user, pwd)
    if O.MutualAuthentication.MutualAuthenticationState:eq(1):fetch_or(false) then
        return
    end
    if user and user.AuthType == cfg.LOG_TYPE_LDAP_KRB_SSO then
        return
    end
    if not pwd or #pwd == 0 then
        return http.reply_error(http.status.HTTP_UNAUTHORIZED, 'ReauthFailed',
        'Authorization failed because the user name or password is incorrect or your account is locked.')
    end

    local ret, decodePwd = portal.base64_decode(pwd)
    if not ret then
        return http.reply_error(http.status.HTTP_UNAUTHORIZED, 'ReauthFailed',
        'Authorization failed because the user name or password is incorrect or your account is locked.')
    end

    local ok, password = pcall(function ()
        return gstring(decodePwd)
    end)
    if not ok then
        return http.reply_error(http.status.HTTP_UNAUTHORIZED, 'ReauthFailed',
        'Authorization failed because the user name or password is incorrect or your account is locked.')
    end
    local login_type = gbyte(user.AuthType)
    local ipaddr = gstring(user.ClientIp)
    local caller_info = {gstring('Redfish-WebUI'), gstring(user.UserName), ipaddr}
    local user_nam
    if user.AuthType == cfg.LOG_TYPE_LOCAL then
        user_nam = user.UserName
    elseif user.AuthType == cfg.LOG_TYPE_KRB_SERVER then
        local i = user.UserName:find('%(KRB%)')
        user_nam = (i and user.UserName:sub(1, i - 1) or user.UserName)
    else
        local i = user.UserName:reverse():find('@')
        user_nam = (i and user.UserName:sub(1, user.UserName:len() - i) or user.UserName)
    end
    local input_list = {gstring(user_nam), password, login_type, ipaddr}

    return C.User[0]:next(function(obj)
        local auth_rsp = obj:call('AuthUser', caller_info, input_list)
        if auth_rsp[1] ~= DFL_OK then
            return http.reply_error(http.status.HTTP_UNAUTHORIZED, 'ReauthFailed',
                'Authorization failed because the user name or password is incorrect or your account is locked.')
        end
    end):fetch()
end

function IsSupportNodeEnclosure(product_id, board_type)
    return
        (product_id == PRODUCT_ID_ATLAS or product_id == PRODUCT_ID_XSERIAL_SERVER) and board_type == BOARD_BLADE and
            C.Component('DeviceType', COMPONENT_TYPE_BACKPLANE_BOARD):next():fetch_or(false)
end

function GetNonBoardMMChassisId(chassis_id, board_type)
    local board_slot = dal_rf_get_board_slot(board_type):fetch()
    if board_slot:match('%d$') then
        return board_slot
    elseif not chassis_id then
        chassis_id = board_slot
    end
    if board_type == BOARD_RM and C.RackAssetMgmt[0].ServiceStatus:fetch() == ASSET_MGMT_SVC_ON then
        local id = C.UnitInfo():fold(function(obj)
            return 'U' .. obj.UNum:fetch(), false
        end):fetch_or()
        if id and id:match('%d$') then
            return id
        end
    end
    return chassis_id
end

function utils.GetChassisId()
    -- Obtain the first ID ending with a digit. If no ID is found, obtain the first ID.
    local board_type = dal_rf_get_board_type():fetch()
    local chassis_id
    if IsSupportNodeEnclosure(C.PRODUCT[0].ProductId:fetch(), board_type) then
        chassis_id = 'Enc'
    end
    if board_type ~= BOARD_MM then
        return GetNonBoardMMChassisId(chassis_id, board_type)
    end
    for _, v in ipairs(board_chassis_info) do
        if not v[2] then
            if v[3]:match('%d$') then
                return v[3]
            elseif not chassis_id then
                chassis_id = v[3]
            end
        else
            local id = C.ProductComponent[0]:next(function(obj)
                if obj[v[2]]:fetch() >= 1 then
                    return v[3] .. 1
                end
            end):fetch_or()
            if id then
                return id
            end
        end
    end
    return chassis_id
end

function IsComponentExceed(i, index_out)
    return C.ProductComponent[0]:next(function(obj)
        if obj[board_chassis_info[i][2]] < index_out then
            logging:error('current component id exceed, max index limit is %d, current index id is %d',
                obj[board_chassis_info[i][2]], index_out)
            return true
        end
        return false
    end):fetch_or(true)
end

local function GetChassisComponentInfoById(chassis_id)
    if not chassis_id then
        return
    end
    local index_out = 0
    for i = 1, #board_chassis_info do
        local index = string_lower(chassis_id):reverse():find(string_lower(board_chassis_info[i][3]:reverse()))
        if index then
            if not board_chassis_info[i][2] then
                if string_lower(chassis_id) ~= string_lower(board_chassis_info[i][3]) then
                    return
                end
            else
                index_out = tonumber(chassis_id:sub(#chassis_id - index + 2))
                if IsComponentExceed(i, index_out) then
                    return
                end
            end
            return board_chassis_info[i], index_out
        end
    end
    logging:error('can\'t find component info for %s', chassis_id)
end
utils.GetChassisComponentInfoById = GetChassisComponentInfoById

local function FindChassisComponentHandleWithId(chassis_id)
    local board_type = dal_rf_get_board_type():fetch()
    local board_slot = dal_rf_get_board_slot(board_type):fetch()
    if string_lower(board_slot) == string_lower(chassis_id) then
        return C.Component('DeviceType', CDTE.COMPONENT_TYPE_MAINBOARD):next():fetch_or()
    end
    local chassis_component_info, component_id = GetChassisComponentInfoById(chassis_id)
    if not chassis_component_info then
        return
    end
    return C.Component():fold(function(obj)
        if obj.DeviceType:fetch() == chassis_component_info[1] and
            (not chassis_component_info[2] or obj.DeviceNum:fetch() == component_id) then
            return obj, false
        end
    end):fetch_or()
end

function utils.GetChassisObj(chassis_id)
    if string_lower(chassis_id) == 'rack' then
        return C.RackAssetMgmt[0]:next():fetch_or()
    elseif chassis_id:match('^[uU][0-9]+$') then
        return C.UnitInfo('UNum', chassis_id:match('[uU](%d+)$')):next():fetch_or()
    end
    return FindChassisComponentHandleWithId(chassis_id)
end

function utils.get_system_name()
    -- 34041088为RM210产品唯一识别ID
    -- 增加RM211产品唯一识别ID 34040832
    if O.PRODUCT.ProductUniqueID:eq(34041088):fetch_or() or
        O.PRODUCT.ProductUniqueID:eq(34040832):fetch_or() then
        return 'RMM'
    elseif O.PRODUCT.ProductUniqueID:eq(34016513):fetch_or() then
        return 'DA122C'
    end
    return null
end

function check_in_array(array, val)
    for _, v in ipairs(array) do
        if v == val then
            return true
        end
    end
    return false
end

function utils.IsPangeaPacificSeries()
    if O.PRODUCT.ProductId:fetch_or(null) ~= 0xd then
        return false
    end

    local board_id = O.BOARD.BoardId:fetch_or(null)
    local board_id_array = {0x30, 0x17, 0x90, 0x93, 0x1b, 0xf3}
    if check_in_array(board_id_array, board_id) then
        return true
    end
    return false
end

local function get_customized_kunpeng_cpu_model(in_model)
    local out_model = in_model
    local arr = {}
    for w in string.gmatch(in_model, "%S+") do
        table.insert(arr,w)
    end
   
    if arr[1] == "Kunpeng" and #arr == 2 then -- Kunpeng 920-4826
        out_model = arr[1] .. arr[2]
        out_model = string.gsub(out_model, "-", "_", 1)
    elseif arr[1] == "HUAWEI" and #arr == 4 then -- HUAWEI Kunpeng 920 7260
        out_model = arr[2] .. arr[3] .. "_" .. arr[4]
    end
    return out_model -- Kunpeng920_4826
end

local function get_customized_intel_cpu_model(in_model)
    -- Intel(R) Xeon(R) Gold 5117 CPU @ 2.00GHz
    local out_model = in_model
    local arr = {}
    for w in string.gmatch(in_model, "%S+") do
        table.insert(arr,w)
    end
   
    if #arr <= 4 then
        return in_model
    end

    arr[2] = string.gsub(arr[2], "%(R%)", "", 1)
    return arr[2] .. '_' .. arr[3] .. '_' .. arr[4] -- Xeon_Gold_5117
end

local function format_cpu_model_by_cmcc(model)
    if string.find(model, "Kunpeng") then
        return get_customized_kunpeng_cpu_model(model)
    elseif string.find(model, "Intel") then
        return get_customized_intel_cpu_model(model)
    end
    return model
end

function utils.get_cpu_model(obj)
    local model = obj.Version:next(rf_string_check):fetch_or(null)
    if O.RedfishEventService.EventDetailCustomizedID:fetch_or(null) == cfg.ENABLE then
        return format_cpu_model_by_cmcc(model)
    end
    return model
end

-- file_field:"Contents"
-- fields:{"Contents":"/tmp/web/interface.xml","test":"/tmp/web/license.xml","Type":"123"}
-- 校验文件上传类接口入参，目前仅支持校验上传文件的field，其他非法输入暂不校验
function utils.CheckFormData(form_data, file_field)
    file_field = file_field and file_field or 'Content'

    local check_pass = true
    local fields = form_data.fields
    local headers = form_data.headers

    --校验form_data中file_field是否存在，且为FILE类型
    if not fields or not fields[file_field] or not headers or not headers[file_field] or headers[file_field].type ~= 'FILE'  then
        check_pass = false
    end

    --删除form_data中非file_field的文件类型fields所指向的文件
    for key, value in pairs(fields) do
        if headers and headers[key] and headers[key].type == 'FILE' and key ~= file_field then
            local real_path = realpath(value)
            if real_path and file_exists(real_path) then
                c.rm_filepath(real_path)
            end
        end
    end

    return check_pass
end

function utils.GetFormDataFilePath(form_data, field_name)
    field_name = field_name or 'Content'

    local fields = form_data.fields
    if not fields or not fields[field_name] then
        return nil
    end

    local headers = form_data.headers
    if not headers or not headers[field_name] or headers[field_name].type ~= 'FILE' then
        return nil
    end

    local file_path, file_name = string.match(fields[field_name], '(.-)([^\\/]+)$')
    if not file_path or not file_name then
        return nil
    end

    if file_path:find(".", 1, true) then
        return false
    end

    local real_path = realpath(fields[field_name])
    if not real_path or not file_exists(real_path) then
        return nil
    end

    return real_path
end

-- 根据某个对象查找同级position指定类的第一个对象接口
local function dal_get_specific_object_position(obj, cname)
    local position = get_position(obj)
    return C(cname):fold(function(oo)
        if position == get_position(oo) then
            return oo, false
        end
    end):neq(nil):catch(function()
        logging:error("dal_get_specific_object_position: cname = %s, obj = %s, position = %d", cname,
            object_name(obj), position)
        error("get position class failed")
    end)
end

utils.dal_get_specific_object_position = dal_get_specific_object_position

-- 获取DeviceType是0x26的部件对应的类名
local function _get_device_type_0x26_class_name(comObj)
    if dal_get_specific_object_position(comObj, "PCIeTransformCard"):fetch_or() then
        return "PCIeTransformCard"
    end

    if dal_get_specific_object_position(comObj, "RiserPcieCard"):fetch_or() then
        return "RiserPcieCard"
    end

    if dal_get_specific_object_position(comObj, "IOBoard"):fetch_or() then
        return "IOBoard"
    end
end

-- 获取电源背板对应的类名
local function _get_power_bp_class_name(comObj)
    if C.IOBoard("RefComponent", object_name(comObj)):next():fetch_or() then
        return "IOBoard"
    end

    if dal_get_specific_object_position(comObj, "Card"):fetch_or() then
        return "Card"
    end
end

-- 根据Component对象句柄获取对应的部件类
function utils.redfish_get_board_class_name(comObj)
    -- 判断是否是外围卡，外围卡没有Component对象
    if class_name(comObj) == "PeripheralCard" then
        return "PeripheralCard"
    end

    local classname
    local componentType = comObj.DeviceType:fetch()
    if componentType == CDTE.COMPONENT_TYPE_BOARD_PCIE_ADAPTER then
        --[[
            device type 是0x26时，类名可能有多个，任意一个匹配上都算成功
            *PCIeTransformCard
            *RiserPcieCard--/profile/x6800_x6000_server_01/14140129_94.xml等
            *IOBoard--------/application/resource/profile/8100_server_01/8100v5/14220292_81.xml
        ]] --
        classname = _get_device_type_0x26_class_name(comObj)
    elseif componentType == CDTE.COMPONENT_TYPE_PSU_BACKPLANE then
        --[[
            device type 是0x19时，类名可能有多个，任意一个匹配上都算成功
            *Card-----------/profile/atlas_01/14100075_66.xml
            *IOBoard--------/profile/8100_server_01/9008v5/14100249_62.xml
        ]] --
        classname = _get_power_bp_class_name(comObj)
    else
        classname = DEVICETYPE_2_CLASSNAME[componentType]
    end

    if classname then
        return classname
    end

    logging:error("redfish_get_board_class_name: Type = %s, obj = %s", componentType, object_name(comObj))
    error("get board class name failed")
end

-- 向返回接口中添加组件唯一标识
function utils.add_component_uid(result, component_uid)
    if not result or result == null or not component_uid or component_uid == null then
        return
    end

    local lower_uid = string.lower(component_uid)
    if lower_uid ~= "n/a" and lower_uid ~= "na" and lower_uid ~= "null" and
        lower_uid ~= "unknown" and lower_uid ~= "" then
        result["ComponentUniqueID"] = component_uid
    end
end

function utils.modify_file_mode(user, file_path)
    if user.AuthType == LOG_TYPE_LOCAL then
        -- 本地用户上传的文件设置其属主为上传文件的用户
        C.User[0]:next(function(obj)
            local ret = obj:call("GetUidGidByUsername", nil, {gstring(user.UserName)})
            local uid, gid = ret[1], ret[2]
            O.PrivilegeAgent:next(function(obj_priv)
                obj_priv:call("ModifyFileOwn", nil, {gstring(file_path), guint32(uid), guint32(gid)})
                obj_priv:call("ModifyFileMode", nil, {gstring(file_path), guint32(FILE_MODE)})
            end)
        end):fetch()
    else
        -- 域用户设置上传的文件, 在redfish模块的哈希表中记录文件路径与上传者
        O.BMC:next(function(obj)
            obj:call("RecordFileOwner", nil, {gstring(file_path), gstring(user.UserName)})
        end):fetch()

        O.PrivilegeAgent:next(function(obj)
            obj:call("ModifyFileMode", nil, {gstring(file_path), guint32(FILE_MODE)})
        end):fetch()
    end
end

-- 检查/tmp/web目录下是否存在同名文件
function utils.check_file_exists(user, target_file_path)
    if not file_exists(target_file_path) then
        return false
    end

    -- 如果tmp/web下存在同名的目录或者存在同名文件但是属主不是本用户，则不允许上传
    if is_dir(target_file_path) then
        return true
    end

    return not dal_check_file_owner(user.UserName, target_file_path, user.AuthType == LOG_TYPE_LOCAL)
end

-- 根据某个对象查找同级position指定类的对象列表
function utils.dal_get_object_list_position(obj, cname)
    local position = get_position(obj)
    return C(cname):fold(function(oo, acc)
        if position == get_position(oo) then
            acc[#acc + 1] = oo
        end
        return acc
    end, {}):next(function(array)
        assert(#array > 0)
        return array
    end):catch(function()
        logging:error('dal_get_object_list_position: obj = %s, cname = %s', object_name(obj), cname)
        error("get position class failed")
    end)
end

-- 判断产品是否指定的product id
function utils.dal_match_product_id(expect_id)
    local product_id = C.PRODUCT[0].ProductId:fetch()
    if expect_id == product_id then
        return true
    else
        return false
    end
end

local REAR_BACKPLANE_CONNECT = 'RearHddBackConnect'
local INNER_BACKPLANE_CONNECT = 'InnerHddBackConnect'
local FRONT_BACKPLANE_CONNECT = 'FrontHddBackConnect'

-- 获取硬盘背板的后缀字符
function utils.get_chassis_disk_backplane(component_obj)
    local pAnchorObj = dal_get_specific_object_position(component_obj, 'Anchor')
    local type = pAnchorObj:next(function(anchorObj)
        return anchorObj.Position
    end):catch(0):next(function(position)
        return C.Connector('Position', position)
    end):next(function(connectorObj)
        return connectorObj.Type
    end):fetch_or()
    if type == REAR_BACKPLANE_CONNECT then
        return 'Rear'
    elseif type == INNER_BACKPLANE_CONNECT then
        return 'Inner'
    elseif type == FRONT_BACKPLANE_CONNECT then
        return 'Front'
    end
    return ''
end

-- 获取component对象的关联的SN号
function utils.get_serial_number(component_obj)
    local sn = nil
    C.Fru():fold(function(fruobj)
        fruobj.FruId:eq(component_obj.FruId:fetch()):next(function()
            if fruobj.Elabel:fetch() ~= null then
                sn = O[fruobj.Elabel:fetch()].BoardSerialNumber:next(rf_string_check):fetch_or(null)
            end
        end):fetch_or()
    end):fetch_or()
    return sn
end

-- 获取component对象的关联的partnumber号
function utils.get_part_number(component_obj)
    local part_num = nil
    C.Fru():fold(function(fruobj)
        fruobj.FruId:eq(component_obj.FruId:fetch()):next(function()
            if fruobj.Elabel:fetch() ~= null then
                part_num = O[fruobj.Elabel:fetch()].BoardPartNumber:next(rf_string_check):fetch_or(null)
            end
        end):fetch_or()
    end):fetch_or()
    return part_num
end

function utils.IsMultiplaneSupport()
    local prop = C.EthFunction('Functiontype', FN_TYPE_INNER_SHELF).SupportMultiPlane:fetch_or()
    if prop ~= 1 then
        return false
    end
    return true
end
 
function utils.IsMgmtBoard()
    local softwareType = O.PRODUCT.MgmtSoftWareType:fetch_or()
    if softwareType == nil then
        return false
    end
    local highb = bit_rshift(softwareType, 4)  -- 高四位
    local lowb = bit_and(softwareType, 0x0f)  -- 低四位
    if highb == BOARD_MM_HIGH or highb == BOARD_RM_HIGH then
        if lowb == BOARD_MM_LOW then
            return true
        end
    end
    return false
end

function utils.dal_check_string_is_ascii(str)
    if not str and type(str) ~= 'string' then
        return false
    end
    for letter in str:gmatch '.' do
        if string.byte(letter) > 0x7e or string.byte(letter) < 0x20 then
            return false
        end
    end
    return true
end

function utils.GetRemoteIpAddrAndPort()
    local ethGroup = O.DataSyncManagementObject.BindEthGroup:fetch_or()
    local innerName = O(ethGroup).InnerNetWork:fetch_or()
    local ipvalue1, ipvalue2, ipvalue3, ipvalue4 = innerName:match("(%d+)%.(%d+)%.(%d+)%.(%d+)")
    local retip = ipvalue1..'.'..ipvalue2..'.'..ipvalue3..'.'..(128 + ipvalue4 + (O.AsmObject.RemoteSlotId:fetch_or(0)))
    logging:debug('remote ip address is: %s', retip)
    return retip, 51000
end

return utils
