local cjson = require 'cjson'
local dflib = require 'dflib'
local utils = require 'utils'
local c = require "dflib.core"
local logging = require 'logging'
local bit = require 'bit'
local ffi = require 'ffi'
local cfg = require 'config'
local http = require 'http'
local class_name = dflib.class_name
local O = dflib.object
local C = dflib.class
local object_name = dflib.object_name
local get_position = dflib.get_position
local null = cjson.null
local rf_string_check = utils.rf_string_check
local dal_clear_string_blank = utils.dal_clear_string_blank
local GetChassisId = utils.GetChassisId
local GetChassisObj = utils.GetChassisObj
local dal_rf_get_board_type = utils.dal_rf_get_board_type
local redfish_get_board_class_name = utils.redfish_get_board_class_name
local dal_get_specific_object_position = utils.dal_get_specific_object_position
local CDTE = cfg.COMPONENT_DEVICE_TYPE_E
local BOARD_MM = cfg.BOARD_MM
local lower = string.lower
local get_system_name = utils.get_system_name
local add_component_uid = utils.add_component_uid
local get_chassis_disk_backplane = utils.get_chassis_disk_backplane
local get_serial_number = utils.get_serial_number
local PRODUCT_ID_TIANCHI_SERVER = cfg.PRODUCT_ID_TIANCHI_SERVER
local dal_match_product_id = utils.dal_match_product_id
local reply_bad_request = http.reply_bad_request
local reply_not_found = http.reply_not_found
local reply_ok = http.reply_ok
local GVariant = c.GVariant
local gbyte = GVariant.new_byte

local M = {}

local MAX_RECURSIVE_DEPTH = 10

local ENABLE = 1
local INVALID_DATA_BYTE = 0xFF
local M2_1_PRESENCE_MASK = 0x01
local M2_2_PRESENCE_MASK = 0x02

-- PCIeCard中FunctionClass取值定义
local PCIECARD_FUNCTION_UNKNOWN = 0
local PCIECARD_FUNCTION_RAID = 1
local PCIECARD_FUNCTION_NETCARD = 2
local PCIECARD_FUNCTION_GPU = 3
local PCIECARD_FUNCTION_STORAGE = 4
local PCIECARD_FUNCTION_SDI = 5
local PCIECARD_FUNCTION_ACCELERATE = 6
local PCIECARD_FUNCTION_PCIE_RISER = 7
local PCIECARD_FUNCTION_FPGA = 8
local PCIECARD_FUNCTION_NPU = 9
local PCIECARD_FUNCTION_STORAGE_DPU = 10

local CLASS_PCIE_CARD = 'PcieCard'
local CLASS_PCIE_FPGA_CARD = 'PCIeFPGACard'
local CLASS_PCIE_SDI_CARD = 'PCIeSDICard'
local CLASS_PCIESSDCARD_NAME = 'PCIeSSDCard'
local CLASS_ACCLERATE_CARD = 'AccelerateCard'
local CLASS_M2TRANSFORMCARD = 'M2TransformCard'
local CLASS_OCP_CARD = 'OCPCard'
local CLASS_STORAGE_DPU_CARD = 'PCIeStorageDPUCard'

local RFPROP_FUNCTIONTYPE_RAIDCARD = 'RAID Card'
local RFPROP_FUNCTIONTYPE_NETCARD = 'Net Card'
local RFPROP_FUNCTIONTYPE_GPUCARD = 'GPU Card'
local RFPROP_FUNCTIONTYPE_STORAGE = 'Storage Card'
local RFPROP_FUNCTIONTYPE_SDI = 'SDI Card'
local RFPROP_FUNCTIONTYPE_ACCELERATE = 'Accelerate Card'
local RFPROP_FUNCTIONTYPE_PCIERISER = 'PCIe Riser Card'
local RFPROP_FUNCTIONTYPE_FPGA = 'FPGA Card'
local RFPROP_FUNCTIONTYPE_NPUCARD = 'NPU Card'
local RFPROP_FUNCTIONTYPE_STORAGE_DPUCARD = 'Storage DPU Card'

local CRITICAL_STRING = 'Critical'
local OK_STRING = 'OK'

-- PCIe资源ID归属定义
local RESOURCE_ID_PCIE_DEVICE_BASE = 0x80 -- PCIe资源归属于其它PCIe设备时, 资源ID的基数, 实际的资源ID=基数 + PCIe设备FRU ID
-- PCIe资源归属于其它PCIe设备时, 资源ID的最大值, 实际的资源ID=基数 + PCIe设备FRU ID
local RESOURCE_ID_PCIE_DEVICE_MAX = 0xC0
local RESOURCE_ID_CPU1_AND_CPU2 = 0xFD -- PCIe资源归属于CPU1和CPU2
local RESOURCE_ID_CPU1_TO_CPU4 = 0xFC -- PCIe资源归属于所有CPU
local RESOURCE_ID_PCH = 0xFE -- PCIe资源归属于PCH
local RESOURCE_ID_PCIE_SWTICH = 0xFF -- PCIe资源归属于PCIe Switch, 可动态切换, 不固定

local DATA_ERR = -1 -- IP数据尚未更新
local BOARDS_ERR_MAP = {
    [DATA_ERR] = reply_bad_request('DataIsNotReady', 'IP data is not ready')
}

local SDI_CARD_NOT_SUPPORT_NMI = -2
local SDI_CARD_ERR_MAP = {
    [SDI_CARD_NOT_SUPPORT_NMI] = reply_bad_request('SdiCardNotSupport')
}

local DEVICE_TYPE = {
    HDD_BACKPLANE = 5,
    RAID_CARD = 6,
    MEZZ_CARD = 11,
    MEMORY_BOARD = 14,
    PCIE_RISER = 15,
    CHASSIS_BACKPLANE = 22,
    FAN_BACKPLANE = 24,
    PSU_BACKPLANE = 25,
    SD_CARD = 33,
    IO_BOARD = 35,
    CPU_BOARD = 36,
    BOARD_PCIE_ADAPTER = 38,
    EXPAND_BOARD = 45,
    EXPANDER = 50,
    GPU_BOARD = 64,
    PASS_THROUGH_CARD = 67,
    HORIZ_CONN_BRD = 76,
    LEAK_DET_CARD = 84,
    AR_CARD = 90,
    CONVERGE_BOARD = 91,
    EXPANSION_BOARD = 93
}

local G_BOARDS_PROPERTY = {
    {DEVICE_TYPE.HDD_BACKPLANE, 'DiskBackplane', 'HddBackCardNum'}, -- 硬盘背板
    {DEVICE_TYPE.PCIE_RISER, 'PCIeRiserCard', 'RiserCardNum'}, -- Riser卡
    {DEVICE_TYPE.RAID_CARD, 'RAIDCard', 'RaidCardNum'}, -- Raid卡
    {DEVICE_TYPE.EXPAND_BOARD, 'M2TransformCard', 'M2AdpterCardNum'}, -- M.2转接卡
    {DEVICE_TYPE.PASS_THROUGH_CARD, 'PassThroughCard', null}, -- 直通卡，没有最大个数
    {DEVICE_TYPE.MEZZ_CARD, 'MezzCard', 'MezzCardNum'}, -- MEZZ卡
    {DEVICE_TYPE.BOARD_PCIE_ADAPTER, 'PCIeTransformCard', null}, -- PCIE转接卡，没有最大个数
    {DEVICE_TYPE.GPU_BOARD, 'GPUBoard', 'GPUBoardNum'}, -- GPU板
    {DEVICE_TYPE.FAN_BACKPLANE, 'FanBackplane', null}, -- 风扇背板，没有最大个数
    {DEVICE_TYPE.HORIZ_CONN_BRD, 'HorizontalConnectionBoard', 'HorizConnBoardNum'}, -- 水平转接板
    {DEVICE_TYPE.CPU_BOARD, 'CPUBoard', 'CpuBoardNum'}, -- 处理器板
    {DEVICE_TYPE.MEMORY_BOARD, 'MemoryBoard', 'MemoryCardNum'}, -- 内存板
    {DEVICE_TYPE.IO_BOARD, 'IOBoard', 'IOCardNum'}, -- IO板
    {DEVICE_TYPE.CHASSIS_BACKPLANE, 'ChassisBackplane', null}, -- 机框背板，没有最大个数
    {DEVICE_TYPE.SD_CARD, 'SDCard', 'SDCardNum'}, -- SD卡
    {DEVICE_TYPE.EXPANDER, 'ExpandBoard', 'ExpBoardNum'},
    {DEVICE_TYPE.LEAK_DET_CARD, 'LeakageDetectionCard', 'LeakDetCardNum'},
    {DEVICE_TYPE.CONVERGE_BOARD, 'ConvergeBoard', null} -- 汇聚板，没有最大个数
}

local G_TIANCHI_BOARDS_PROPERTY = {
    {DEVICE_TYPE.HDD_BACKPLANE, 'DiskBackplane', 'HddBackCardNum'}, -- 硬盘背板
    {DEVICE_TYPE.PCIE_RISER, 'PCIeRiserCard', 'RiserCardNum'}, -- Riser卡
    {DEVICE_TYPE.RAID_CARD, 'RAIDCard', 'RaidCardNum'}, -- Raid卡
    {DEVICE_TYPE.EXPAND_BOARD, 'M2TransformCard', 'M2AdpterCardNum'}, -- M.2转接卡
    {DEVICE_TYPE.PASS_THROUGH_CARD, 'PassThroughCard', null}, -- 直通卡，没有最大个数
    {DEVICE_TYPE.MEZZ_CARD, 'MezzCard', 'MezzCardNum'}, -- MEZZ卡
    {DEVICE_TYPE.BOARD_PCIE_ADAPTER, 'PCIeTransformCard', null}, -- PCIE转接卡，没有最大个数
    {DEVICE_TYPE.GPU_BOARD, 'GPUBoard', 'GPUBoardNum'}, -- GPU板
    {DEVICE_TYPE.FAN_BACKPLANE, 'FanBackplane', null}, -- 风扇背板，没有最大个数
    {DEVICE_TYPE.HORIZ_CONN_BRD, 'HorizontalConnectionBoard', 'HorizConnBoardNum'}, -- 水平转接板
    {DEVICE_TYPE.CPU_BOARD, 'CPUBoard', 'CpuBoardNum'}, -- 处理器板
    {DEVICE_TYPE.MEMORY_BOARD, 'MemoryBoard', 'MemoryCardNum'}, -- 内存板
    {DEVICE_TYPE.IO_BOARD, 'IOBoard', 'IOCardNum'}, -- IO板
    {DEVICE_TYPE.CHASSIS_BACKPLANE, 'ChassisBackplane', null}, -- 机框背板，没有最大个数
    {DEVICE_TYPE.SD_CARD, 'SDCard', 'SDCardNum'}, -- SD卡
    {DEVICE_TYPE.EXPANDER, 'ExpandBoard', 'ExpBoardNum'},
    {DEVICE_TYPE.LEAK_DET_CARD, 'LeakageDetectionCard', 'LeakDetCardNum'},
    {DEVICE_TYPE.EXPANSION_BOARD, 'ExpandBoard', 'ExpBoardNum'},
    {DEVICE_TYPE.AR_CARD, 'ARCard', 'ARCardNum'} -- AR卡
}

ffi.cdef [[
    #pragma pack(1)
    typedef struct tag_pfport_info {
        short int pf;
        short int physical_port;
        char mac_addr[65];
    } PFPORT_INFO_S;
]]

local function working_state_to_str(work_state)
    local bbu_status_str = {
        'Offline', 'Init', 'Idle', 'Charging', 'FullCharged', 'Discharging', 'OnlineTest', 'Protect', 'Shutdown',
        'Sleep'
    }
    return bbu_status_str[work_state + 1] or 'Undefined'
end

local function m2_device1_presence(presence)
    return bit.band(presence, M2_1_PRESENCE_MASK) ~= 0
end

local function m2_device2_presence(presence)
    return bit.band(presence, M2_2_PRESENCE_MASK) ~= 0
end

local function selftest_result_to_str(selftest_result)
    if selftest_result == INVALID_DATA_BYTE then
        return null
    elseif selftest_result == ENABLE then
        return OK_STRING
    else
        return CRITICAL_STRING
    end
end

local function check_str_effective(str)
    if str == 'N/A' or str == 'UnKnown' or str == 'NA' or str == '' or str == nil then
        logging:error('check str effective fail.')
        return null
    else
        return str
    end
end

local function get_uint16_hex(val)
    if val == 0 or val == 0xFFFF then
        return null
    end
    return string.format('0x%04x', val)
end

local function get_uint16_hex_with_venderid(venderid, val)
    if venderid == 0 and val == 0 or val == 0xFFFF or not val then
        return null
    end
    return string.format('0x%04x', val)
end

local function IntToBool(value)
    if value == 0 then
        return false
    else
        return true
    end
end

local function get_pciecard_component_handle(obj)
    return O[obj.RefComponent:fetch_or()]
end

local function get_pciecard_logic_slot(obj)
    if obj.LogicSlot:fetch_or() ~= nil and obj.LogicSlot:fetch_or() ~= null and obj.LogicSlot:fetch_or() ~= 0xff then
        return obj.LogicSlot:fetch_or()
    else
        return obj.PcieCardSlot:fetch_or()
    end
end

local function get_one_refproperty_object_handle(obj, className)
    if className == 'HDDBackplane' or className == 'HorizConnectionBoard' then
        return dal_get_specific_object_position(obj, className)
    end
    return obj:ref('DeviceNum'):next(function(refObj)
        return C(className):fold(function(oo)
            if className == 'IOBoard' and object_name(oo) == refObj.obj_name then
                return oo, false
            end
            local match = oo:ref(refObj.prop_name):next(function(refrefObj)
                return refrefObj.obj_name == refObj.obj_name
            end):fetch_or()
            if match then
                return oo, false
            end
        end):fetch_or()
    end):neq(nil):catch(function()
        logging:error('get_one_refproperty_object_handle: obj = %s, className = %s', object_name(obj),
            className)
    end)
end

local function get_chassis_board_firmwareversion(obj, className)
    if not className and className ~= 'AssetLocateBoard' and className ~= 'IOBoard' then
        return
    end
    return get_one_refproperty_object_handle(obj, className):next(
        function(refObj)
            return refObj.FirmwareVersion:next(rf_string_check):fetch()
        end):fetch_or()
end

local function get_pciecard_cardtype_handle(obj)
    local slave_name = obj.SlaveCard:fetch_or()
    if slave_name == 'M2TransformCard' then
        logging:error('M2TransformCard is fail.')
        return
    end

    local functionclass = obj.FunctionClass:fetch_or()

    if functionclass == nil or slave_name == nil then
        return
    end
    if functionclass == PCIECARD_FUNCTION_RAID then
        local component_handle = get_pciecard_component_handle(obj)
        local com_devicename = component_handle.DeviceName:fetch_or()
        return C.RaidController('ComponentName', com_devicename), PCIECARD_FUNCTION_RAID
    elseif functionclass == PCIECARD_FUNCTION_NETCARD then
        return dal_get_specific_object_position(obj, 'NetCard'):fetch_or(), PCIECARD_FUNCTION_NETCARD
    elseif functionclass == PCIECARD_FUNCTION_ACCELERATE then
        return dal_get_specific_object_position(obj, 'AccelerateCard'):fetch_or(), PCIECARD_FUNCTION_ACCELERATE
    elseif functionclass == PCIECARD_FUNCTION_GPU then
        return dal_get_specific_object_position(obj, 'GPUCard'):fetch_or(), PCIECARD_FUNCTION_GPU
    elseif functionclass == PCIECARD_FUNCTION_FPGA then
        return dal_get_specific_object_position(obj, 'PCIeFPGACard'):fetch_or(), PCIECARD_FUNCTION_FPGA
    elseif functionclass == PCIECARD_FUNCTION_NPU then
        return dal_get_specific_object_position(obj, 'PCIeFPGACard'):fetch_or(), PCIECARD_FUNCTION_NPU
    else
        local pcie_ref_info = obj:ref('PcieCardSlot'):fetch_or()
        local slave_obj = C[slave_name]():fold(function(slave_obj)
            local slave_ref_info = slave_obj:ref('Slot'):fetch_or()
            if pcie_ref_info.obj_name == slave_ref_info.obj_name then
                return slave_obj, false
            end
        end):fetch_or()
        return slave_obj, functionclass
    end
end

local function get_pcie_slave_card_object_handle(obj)
    local slave_card = obj.SlaveCard:fetch_or()
    if slave_card and #slave_card == 0 then
        return obj
    end

    local slot_id = obj.PcieCardSlot:fetch_or()
    local slave_card_obj_handle = C[slave_card]('Slot', slot_id):next():fetch_or(nil)
    return slave_card_obj_handle
end

local function get_pciecard_extend_version_info(obj, request_prop)
    local firmware = obj.Firmware:fetch_or()
    if firmware and #firmware == 0 then
        return false, null
    end
    local firmware_obj_handle = O[firmware]
    if firmware_obj_handle == nil then
        return false, null
    end
    local classifications = firmware_obj_handle.Classifications:fetch_or()
    if classifications == 6 or classifications == 3 then -- FW_CLASSIFICATIONS_BACKPLANE_CPLD  FW_CLASSIFICATIONS_CPLD
        if 'CPLDVersion' ~= request_prop then
            return false, null
        end
    else
        if 'FirmwareVersion' ~= request_prop then
            return false, null
        end
    end
    local version = firmware_obj_handle.VersionString:fetch_or()
    if version and #version == 0 or version == 'N/A' then
        return false, null
    end
    local location = firmware_obj_handle.Location:fetch_or()
    if location and #location == 0 or location == 'N/A' then
        return true, version
    end

    return true, '(' .. location .. ')' .. version
end

local function get_pcie_card_firmwareversion(obj)
    local slave_card_obj_handle = get_pcie_slave_card_object_handle(obj)

    local obj_class_name = nil
    if slave_card_obj_handle ~= nil then
        obj_class_name = class_name(slave_card_obj_handle)
    end

    if obj_class_name == CLASS_PCIE_CARD then
        local ret, firmwareversion = get_pciecard_extend_version_info(obj, 'FirmwareVersion')
        if ret then
            return firmwareversion
        end
    end

    local shell_id
    local firmwareversion
    local obj_handle, card_type = get_pciecard_cardtype_handle(obj)
    if card_type == PCIECARD_FUNCTION_NETCARD or card_type == PCIECARD_FUNCTION_STORAGE or card_type ==
        PCIECARD_FUNCTION_RAID or card_type == PCIECARD_FUNCTION_ACCELERATE or card_type == PCIECARD_FUNCTION_GPU or
        card_type == PCIECARD_FUNCTION_FPGA or card_type == PCIECARD_FUNCTION_NPU then
        firmwareversion = obj_handle and obj_handle.FirmwareVersion:next(check_str_effective):fetch_or()
    end

    if card_type == PCIECARD_FUNCTION_FPGA or card_type == PCIECARD_FUNCTION_NPU then
        shell_id = obj.ShellID:fetch_or()
    end

    if firmwareversion == null or firmwareversion == nil then
        return firmwareversion
    elseif shell_id ~= nil and shell_id ~= 'N/A' then
        return firmwareversion .. ' (' .. shell_id .. ')'
    else
        return firmwareversion
    end
end

local function get_pcie_card_serialnumber(obj)
    local obj_handle, card_type = get_pciecard_cardtype_handle(obj)
    if card_type == PCIECARD_FUNCTION_STORAGE or card_type == PCIECARD_FUNCTION_GPU then
        return obj_handle and obj_handle.SerialNumber:next(check_str_effective):fetch_or() or null
    else
        local elabel_handle = dal_get_specific_object_position(obj, 'Elabel'):fetch_or()
        return elabel_handle and elabel_handle.BoardSerialNumber:next(check_str_effective):fetch_or() or null
    end
end

local function get_pcie_card_model(obj)
    local obj_handle, card_type = get_pciecard_cardtype_handle(obj)
    if card_type == PCIECARD_FUNCTION_STORAGE then
        return obj_handle and obj_handle.ModelNumber:next(check_str_effective):fetch_or() or null
    elseif card_type == PCIECARD_FUNCTION_NETCARD or card_type == PCIECARD_FUNCTION_GPU then
        return obj_handle and obj_handle.Model:next(check_str_effective):fetch_or() or null
    end
end

local function get_pcie_card_function_type(obj)
    local result = {}
    result[PCIECARD_FUNCTION_UNKNOWN] = null
    result[PCIECARD_FUNCTION_RAID] = RFPROP_FUNCTIONTYPE_RAIDCARD
    result[PCIECARD_FUNCTION_NETCARD] = RFPROP_FUNCTIONTYPE_NETCARD
    result[PCIECARD_FUNCTION_GPU] = RFPROP_FUNCTIONTYPE_GPUCARD
    result[PCIECARD_FUNCTION_STORAGE] = RFPROP_FUNCTIONTYPE_STORAGE
    result[PCIECARD_FUNCTION_SDI] = RFPROP_FUNCTIONTYPE_SDI
    result[PCIECARD_FUNCTION_ACCELERATE] = RFPROP_FUNCTIONTYPE_ACCELERATE
    result[PCIECARD_FUNCTION_PCIE_RISER] = RFPROP_FUNCTIONTYPE_PCIERISER
    result[PCIECARD_FUNCTION_FPGA] = RFPROP_FUNCTIONTYPE_FPGA
    result[PCIECARD_FUNCTION_NPU] = RFPROP_FUNCTIONTYPE_NPUCARD
    result[PCIECARD_FUNCTION_STORAGE_DPU] = RFPROP_FUNCTIONTYPE_STORAGE_DPUCARD
    local functype = obj.FunctionClass:fetch_or()

    if result[functype] == nil then
        logging:error('get function class value is error.')
        return null
    end

    return result[functype]
end

local function get_pciecard_extend_info(obj)
    local result = {}
    local ret, cpldVersion = get_pciecard_extend_version_info(obj, 'CPLDVersion')
    result['ProductName'] = obj.Name:next(check_str_effective):fetch_or(null)
    result['BoardId'] = obj.BoardId:next(get_uint16_hex):fetch_or(null)
    result['PCBVersion'] = obj.PcbVer:next(check_str_effective):fetch_or(null)
    if result['PCBVersion'] == null then
        result['PCBVersion'] = C.Fru('FruId', obj:ref_obj('RefComponent'):fetch_or().FruId:fetch_or()).PcbVersion:next(check_str_effective):fetch_or(null)
    end
    result['CPLDVersion'] = cpldVersion

    return result
end

local function get_fpgacard_extend_info(obj)
    local result = {}
    result['ProductName'] = obj.Name:next(check_str_effective):fetch_or(null)
    result['BoardId'] = obj.BoardId:next(get_uint16_hex):fetch_or(null)
    result['PCBVersion'] = obj.PcbVer:next(check_str_effective):fetch_or(null)

    return result
end

local function get_eth_pf_info(user, obj)
    local ok, ret = pcall(obj.call, obj, 'GetPfInfo', user, nil)
    if not ok then
        logging:error('dfl_call_class_method GetPfInfo failed.')
        return null
    end

    local pfportInfo = ret[1]
    local c_str = ffi.new('char[?]', #pfportInfo)
    for j = 1, #pfportInfo do
        ffi.fill(c_str + j - 1, 1, pfportInfo[j])
    end

    local result = {}
    for i = 0, 31 do
        local portInfo = ffi.cast('PFPORT_INFO_S *', c_str + i * 69)
        local pfvf, port, macAddr = portInfo.pf, portInfo.physical_port, ffi.string(portInfo.mac_addr)
        if pfvf ~= -1 then
            result[#result + 1] = {Port = port + 1, PfId = pfvf, PermanentMac = macAddr}
        end
    end
    return result
end

local function get_sdicard_extendcard_info(user, obj)
    local result = {}
    local slot_id = obj.Slot:fetch_or()
    local pcie_obj_handle = C.PcieCard('PcieCardSlot', slot_id):next():fetch_or()
    if slot_id == nil or pcie_obj_handle == nil then
        return null
    end

    local pcie_obj_name = object_name(pcie_obj_handle)
    C.ExtendNetCard():fold(function(oneObj)
        local basecard_obj_name = oneObj.BaseCard:fetch_or()
        if basecard_obj_name == pcie_obj_name then
            local extend_netcard_slot_id = oneObj.SlotId:fetch_or()
            local extend_pciecard_obj_handle = C.ExtendPcieCard('PcieCardSlot', extend_netcard_slot_id):next():fetch_or()
            result[#result + 1] = {
                ProductName = extend_pciecard_obj_handle.Name:fetch_or(null),
                BoardId = extend_pciecard_obj_handle.BoardId:next(get_uint16_hex):fetch_or(null),
                BoardName = extend_pciecard_obj_handle.BoardName:fetch_or(null),
                PCBVersion = extend_pciecard_obj_handle.PcbVer:fetch_or(null),
                DeviceId = extend_pciecard_obj_handle.DeviceId:next(get_uint16_hex):fetch_or(null),
                VendorId = extend_pciecard_obj_handle.VenderId:next(get_uint16_hex):fetch_or(null),
                SubsystemId = extend_pciecard_obj_handle.SubDeviceId:next(get_uint16_hex):fetch_or(null),
                Slot = extend_netcard_slot_id,
                SubsystemVendorId = extend_pciecard_obj_handle.SubVenderId:next(get_uint16_hex):fetch_or(null),
                Manufacturer = oneObj.Manufacture:fetch_or(null),
                ChipManufacturer = oneObj.ChipManufacturer:fetch_or(null),
                Description = oneObj.CardDesc:fetch_or(null),
                Model = oneObj.Model:fetch_or(null),
                PfMacInfo = get_eth_pf_info(user, extend_pciecard_obj_handle)
            }
        end
    end):fetch_or(null)
    return result
end

local function get_sdicard_extend_info(user, obj)
    local result = {}
    local PROPERTY_SDICARD_BOARDID_V3 = '0x00d0'
    local PROPERTY_SDICARD_BOARDID_V5 = '0x00d1'
    local PROPERTY_SDICARD_BOARDID_FD = '0x00fd'

    result['ProductName'] = obj.Name:next(check_str_effective):fetch_or()
    result['BoardId'] = obj.BoardId:next(get_uint16_hex):fetch_or()
    result['PCBVersion'] = obj.PcbVer:next(check_str_effective):fetch_or()

    local m2_presence = obj.M2Presence:fetch_or()
    if m2_presence == 1 then
        result['M2Device1Presence'] = true
        result['M2Device2Presence'] = false
    elseif m2_presence == 2 then
        result['M2Device1Presence'] = false
        result['M2Device2Presence'] = true
    elseif m2_presence == 3 then
        result['M2Device1Presence'] = true
        result['M2Device2Presence'] = true
    else
        result['M2Device1Presence'] = false
        result['M2Device2Presence'] = false
    end

    local SODIMMPresence = obj.SODIMMPresence:fetch_or()
    if SODIMMPresence == 1 then
        result['SODIMM00Presence'] = true
        result['SODIMM10Presence'] = false
    elseif SODIMMPresence == 2 then
        result['SODIMM00Presence'] = false
        result['SODIMM10Presence'] = true
    elseif SODIMMPresence == 3 then
        result['SODIMM00Presence'] = true
        result['SODIMM10Presence'] = true
    elseif SODIMMPresence == 0 then
        result['SODIMM00Presence'] = false
        result['SODIMM10Presence'] = false
    end

    local ip_val = obj.StorageIpAddr:fetch_or()
    if not ip_val then
        result['StorageManagementIP'] = null
    else
        result['StorageManagementIP'] = string.format('%d.%d.%d.%d', bit.band(ip_val, 0xFF),
        bit.band(bit.rshift(ip_val, 8), 0xFF), bit.band(bit.rshift(ip_val, 16), 0xFF),
        bit.band(bit.rshift(ip_val, 24), 0xFF))
    end

    local vlanid = obj.StorageIpVlan:fetch_or()
    if not vlanid or vlanid <= 0 or vlanid >= 0xFFF then
        result['StorageManagementVlanId'] = null
    else
        vlanid = bit.lshift(bit.band(vlanid, 0xFF), 8) + bit.band(bit.rshift(vlanid, 8), 0xFF)
        result['StorageManagementVlanId'] = vlanid
    end

    local boot_order = obj.BootOrder:fetch_or()
    if boot_order == 0 then
        result['BootOption'] = 'HDD Boot'
    elseif boot_order == 1 then
        result['BootOption'] = 'Base Board PXE Boot'
    elseif boot_order == 3 then
        result['BootOption'] = 'Buckle Card1 PXE Boot'
    elseif boot_order == 5 then
        result['BootOption'] = 'Buckle Card2 PXE Boot'
    elseif boot_order == 7 then
        result['BootOption'] = 'Buckle Card3 PXE Boot'
    elseif boot_order == 255 then
        result['BootOption'] = 'None'
    elseif not boot_order then
        result['BootOption'] = null
    else
        result['BootOption'] = 'HDD Boot'
    end

    result['ExtendCardInfo'] = get_sdicard_extendcard_info(user, obj)

    result.SDIPfMacInfo = {}
    local macAddrList = obj.MacAddr:fetch_or({})

    if #macAddrList== 0 or #macAddrList[1] == 0 then
        return result
    end

    for i = 1, #macAddrList do
        local macInfo = utils.split(macAddrList[i],';')
        result.SDIPfMacInfo[#result.SDIPfMacInfo + 1] = {
            Port = tonumber(macInfo[1]),
            PfId = tonumber(macInfo[2]),
            PermanentMac = tostring(macInfo[3])
        }
    end

    return result
end

local function get_sddcard_extend_info(obj)
    local result = {}
    result['ProductName'] = obj.Name:next(check_str_effective):fetch_or()
    result['RemainingWearRatePercent'] = obj.RemnantMediaWearout:fetch_or()

    return result
end

local function get_acceleratecard_extend_info(obj)
    local result = {}
    result['ProductName'] = obj.Name:next(check_str_effective):fetch_or()
    result['BoardId'] = obj.BoardId:next(get_uint16_hex):fetch_or()
    result['PCBVersion'] = obj.PcbVer:next(check_str_effective):fetch_or()
    result['BomId'] = obj.BomId:next(get_uint16_hex):fetch_or()

    return result
end

local function get_m2transformcard_extend_info(obj)
    local result = {}
    result['ProductName'] = obj.Name:next(check_str_effective):fetch_or()
    result['BoardId'] = obj.BoardId:next(get_uint16_hex):fetch_or()
    result['PCBVersion'] = obj.PcbVer:next(check_str_effective):fetch_or()
    result['SSD1Presence'] = obj.SSD1Presence:next(IntToBool):fetch_or()
    result['SSD2Presence'] = obj.SSD2Presence:next(IntToBool):fetch_or()

    return result
end

local function get_storage_dpucard_extend_info(user, obj)
    local result = {}
    result['ProductName'] = obj.Name:next(check_str_effective):fetch_or()
    result['PCBVersion'] = obj.PcbVer:next(check_str_effective):fetch_or()
    local link_status = obj.DpuEthLinkStatus:fetch_or()
    if link_status == 0 then
        result['StorageManagementPortStatus'] = 'LinkUp'
    elseif link_status == 1 then
        result['StorageManagementPortStatus'] = "LinkDown"
    else
        result['StorageManagementPortStatus'] = 'Unknown'
    end
    local ok, temp = utils.call_method(user, obj, 'GetStorageIpAddr', BOARDS_ERR_MAP, nil)
    if not ok then
        result['StorageManagementIP'] = "Unknown"
        result['StorageManagementIPv6'] = "Unknown"
        return result
    end

    local ipinfo = temp[1]
    local ret = ffi.new('char[?]', #ipinfo)
    for j = 1, #ipinfo do
        ffi.fill(ret + j - 1, 1, ipinfo[j])
    end
    result['StorageManagementIP'] = string.format('%d.%d.%d.%d/%d', ret[0], ret[1], ret[2], ret[3], ret[4])
    result['StorageManagementIPv6'] = string.format(
        "%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X/%d", 
        ret[5], ret[6], ret[7], ret[8], ret[9], ret[10], ret[11], ret[12], ret[13],
        ret[14], ret[15], ret[16], ret[17], ret[18], ret[19], ret[20], ret[21])

    return result
end

local function get_pcie_extend_info(user, obj)
    local slave_card_obj_handle = get_pcie_slave_card_object_handle(obj)
    local obj_class_name = nil
    if slave_card_obj_handle ~= nil then
        obj_class_name = class_name(slave_card_obj_handle)
    end
    if obj_class_name == CLASS_PCIE_CARD then
        return get_pciecard_extend_info(obj)
    elseif obj_class_name == CLASS_PCIE_FPGA_CARD then
        return get_fpgacard_extend_info(obj)
    elseif obj_class_name == CLASS_PCIE_SDI_CARD then
        return get_sdicard_extend_info(user, slave_card_obj_handle)
    elseif obj_class_name == CLASS_PCIESSDCARD_NAME then
        return get_sddcard_extend_info(slave_card_obj_handle)
    elseif obj_class_name == CLASS_ACCLERATE_CARD then
        return get_acceleratecard_extend_info(obj)
    elseif obj_class_name == CLASS_M2TRANSFORMCARD then
        return get_m2transformcard_extend_info(obj)
    elseif obj_class_name == CLASS_OCP_CARD then
        return get_pciecard_extend_info(obj)
    elseif obj_class_name == CLASS_STORAGE_DPU_CARD then
        return get_storage_dpucard_extend_info(user, slave_card_obj_handle)
    else
        return nil
    end
end

local function get_pciefunction_associatedresource(obj)
    local resource_num = obj.ResId:fetch_or()
    if resource_num == nil or resource_num == 0 then
        logging:error('resource ID is invalid')
        return null
    end
    if resource_num == RESOURCE_ID_PCIE_SWTICH then
        return 'PCIeSwitch'
    elseif resource_num == RESOURCE_ID_PCH then
        return 'PCH'
    elseif resource_num == RESOURCE_ID_CPU1_AND_CPU2 then
        return 'CPU1,CPU2'
    elseif resource_num == RESOURCE_ID_CPU1_TO_CPU4 then
        return 'CPU1,CPU2,CPU3,CPU4'
    elseif resource_num > RESOURCE_ID_PCIE_DEVICE_BASE and resource_num <= RESOURCE_ID_PCIE_DEVICE_MAX then
        return C.CLASS_COMPONENT('FruId', resource_num - RESOURCE_ID_PCIE_DEVICE_BASE).DeviceName:fetch_or()
    else
        return 'CPU' .. resource_num
    end
end

local function get_pcie_card_pciefunction(obj)
    local result = {}
    local venderid = obj.VenderId:fetch_or(0)
    result['VendorId'] = get_uint16_hex_with_venderid(venderid, obj.VenderId:fetch_or())
    result['DeviceId'] = get_uint16_hex_with_venderid(venderid, obj.DeviceId:fetch_or())
    result['SubsystemVendorId'] = get_uint16_hex_with_venderid(venderid, obj.SubVenderId:fetch_or())
    result['SubsystemId'] = get_uint16_hex_with_venderid(venderid, obj.SubDeviceId:fetch_or())
    if C.PMEServiceConfig[0].X86Enable ~= 0 then -- 没有x86的系统上的PCIe卡不支持以下信息查询
        result['AssociatedResource'] = get_pciefunction_associatedresource(obj)
        result['LinkWidth'] = obj.LinkWidth:fetch_or(null)
        result['LinkWidthAbility'] = obj.LinkWidthAbility:fetch_or(null)
        result['LinkSpeed'] = obj.LinkSpeed:fetch_or(null)
        result['LinkSpeedAbility'] = obj.LinkSpeedAbility:fetch_or(null)
    end

    return result
end

local function get_ocp_card_pciefunction(obj)
    local result = {}
    local venderid = obj.VenderId:fetch_or(0)
    result['VendorId'] = get_uint16_hex_with_venderid(venderid, obj.VenderId:fetch_or())
    result['DeviceId'] = get_uint16_hex_with_venderid(venderid, obj.DeviceId:fetch_or())
    result['SubsystemVendorId'] = get_uint16_hex_with_venderid(venderid, obj.SubVenderId:fetch_or())
    result['SubsystemId'] = get_uint16_hex_with_venderid(venderid, obj.SubDeviceId:fetch_or())
    if C.PMEServiceConfig[0].X86Enable ~= 0 then -- 没有x86的系统上的PCIe卡不支持以下信息查询
        result['AssociatedResource'] = get_pciefunction_associatedresource(obj)
    end

    return result
end

local function get_bbu()
    local result = nil
    local curnumber = 0
    local maxnumber = O.ProductComponent.BBUModuleNum:fetch_or(0)
    if maxnumber == 0xff then
        maxnumber = 0
    end
    C.BBUModule():fold(function()
        curnumber = curnumber + 1
    end):fetch_or()
    if curnumber ~= 0 or maxnumber ~= 0 then
        result = {
            MaxNumber = maxnumber,
            CurrentNumber = curnumber,
            DetailLink = '/UI/Rest/System/Boards/BBUModule'
        }
    end
    return result
end

local function get_security_module()
    local result = nil
    local curnumber = 0
    local maxnumber = O.ProductComponent.SecurityModuleNum:fetch_or(0)
    if maxnumber == 0xff then
        maxnumber = 0
    end
    C.SecurityModule():fold(function(obj)
        obj.Presence:eq(1):next(function()
            curnumber = curnumber + 1
        end):fetch_or()
    end):fetch_or()
    if curnumber ~= 0 or maxnumber ~= 0 then
        result = {
            MaxNumber = maxnumber,
            CurrentNumber = curnumber,
            DetailLink = '/UI/Rest/System/Boards/SecurityModule'
        }
    end
    return result
end

local function get_lcd()
    local result = nil
    local curnumber = 0
    C.Lcd():fold(function(obj)
        obj.Presence:eq(1):next(function()
            curnumber = curnumber + 1
        end):fetch_or()
    end):fetch_or()
    if curnumber ~= 0 then
        result = {MaxNumber = curnumber, CurrentNumber = curnumber, DetailLink = '/UI/Rest/System/Boards/LCD'}
    end
    return result
end

local function get_peripheral_firmware()
    local result = nil
    local curnumber = 0
    C.PeripheralFirmware():fold(function()
        curnumber = curnumber + 1
    end):fetch_or()
    if curnumber ~= 0 then
        result = {
            MaxNumber = curnumber,
            CurrentNumber = curnumber,
            DetailLink = '/UI/Rest/System/Boards/PeripheralFirmware'
        }
    end
    return result
end

local function get_sd_controller()
    local result = nil
    local curnumber = 0
    local maxnumber = O.ProductComponent.SDCardControllerNum:fetch_or(0)
    if maxnumber == 0xff then
        maxnumber = 0
    end
    C.RaidChip():fold(function(obj)
        obj.Presence:eq(1):next(function()
            curnumber = curnumber + 1
        end):fetch_or()
    end):fetch_or()
    if curnumber ~= 0 or maxnumber ~= 0 then
        result = {
            MaxNumber = maxnumber,
            CurrentNumber = curnumber,
            DetailLink = '/UI/Rest/System/Boards/SDCardController'
        }
    end
    return result
end

local function get_ocp_card() -- _get_chassis_related_pciedevices
    local result = nil
    local curnumber = 0
    local maxnumber = O.ProductComponent.OCPNum:fetch_or(0)
    if maxnumber == 0xff then
        maxnumber = 0
    end
    C.OCPCard():fold(function(obj)
        if class_name(obj) == 'OCPCard' then -- Type:OCPConnector
            obj.Presence:eq(1):next(function()
                curnumber = curnumber + 1
            end):fetch_or()
        end
    end):fetch_or()
    if curnumber ~= 0 or maxnumber ~= 0 then
        result = {MaxNumber = maxnumber, CurrentNumber = curnumber, DetailLink = '/UI/Rest/System/Boards/OCPCard'}
    end
    return result
end

-- 根据某个对象查找同级position指定类的所有对象接口
local function dal_get_object_list_position(obj, cname)
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
    end)
end

local function rf_get_object_location_handle(obj, recursiveDepth)
    recursiveDepth = recursiveDepth + 1
    if not object_name(obj) or recursiveDepth > MAX_RECURSIVE_DEPTH then
        return
    end
    local mainboardObj = C.Component('DeviceType', CDTE.COMPONENT_TYPE_MAINBOARD):next():fetch_or()
    return dal_get_object_list_position(obj, 'Component'):next(
        function(objList)
            for i = 1, #objList do
                local compDeviceType = objList[i].DeviceType:fetch()
                if compDeviceType == CDTE.COMPONENT_TYPE_MAINBOARD or
                    (dal_rf_get_board_type():fetch() == BOARD_MM and
                        (compDeviceType == CDTE.COMPONENT_TYPE_IO_BOARD or compDeviceType ==
                            CDTE.COMPONENT_TYPE_GPU_BOARD)) then
                    logging:debug('successfully found location obj [%s]', object_name(objList[i]))
                    return objList[i]
                end
            end
            return dal_get_specific_object_position(obj, 'Anchor'):next(
                function(anchorObj)
                    return C.Connector('Position', get_position(anchorObj)):fetch_or()
                end):next(function(connectorObj)
                return rf_get_object_location_handle(connectorObj, recursiveDepth)
            end):fetch_or(mainboardObj)
        end):fetch_or(mainboardObj)
end

local function pcie_connector_compare_func(connectorObj, locationObj)
    return connectorObj.Type:next(function(connectorTypeBuf)
        if connectorTypeBuf == 'PCIe' or connectorTypeBuf == 'PCIeConnector' or connectorTypeBuf == 'OCPConnector' or connectorTypeBuf == 'PCIeSDI' then
            return (rf_get_object_location_handle(connectorObj, 0).__handle == locationObj.__handle) and 0 or -1
        end
    end):fetch_or(-1)
end

local function is_child_position_handle(obj, comparedPos)
    local handlePos = get_position(obj)
    if handlePos < comparedPos then
        return false
    end
    local ret = false
    while handlePos ~= 0 do
        if comparedPos == handlePos then
            ret = true
            break
        end
        handlePos = bit.rshift(handlePos, 8)
    end
    if not ret and comparedPos ~= 0 then
        return false
    end
    return true
end

local function dal_get_container_specific_handle_list(containerObj, className, compareFunc)
    local comparedPos = get_position(containerObj)
    return C(className):fold(function(obj, acc)
        if is_child_position_handle(obj, comparedPos) then
            if not compareFunc or compareFunc(obj, containerObj) == 0 then
                acc[#acc + 1] = obj
            end
        end
        return acc
    end, {}):next(function(array)
        assert(#array > 0)
        return array
    end):catch(function()
        logging:error('dal_get_container_specific_handle_list: obj = %s, cname = %s', object_name(containerObj),
            className)
    end)
end

local function check_connector_mached(pcie_card_slot)
    local chassis_id = GetChassisId()
    if not chassis_id then
        return false
    end
    local chassisObj = GetChassisObj(chassis_id)
    return dal_get_container_specific_handle_list(chassisObj, 'Connector', pcie_connector_compare_func):next(
        function(connectorList)
            for i = 1, #connectorList do
                local type = connectorList[i].Type:fetch_or("")
                if connectorList[i].Slot:neq(255):fetch_or() == pcie_card_slot and connectorList[i].Present:fetch_or() == 1 and 
                    (lower(type) == lower('PCIe') or lower(type) == lower('PCIeConnector') or lower(type) == lower('PCIeSDI')) then
                    return true
                end
            end
            return false
        end):fetch_or()
end

local function get_pcie_card()
    local result = nil
    local curnumber = 0
    local maxnumber = O.ProductComponent.PcieCardNum:neq(0xff):fetch_or(0)
    C.PcieCard():fold(function(obj)
        if check_connector_mached(obj.PcieCardSlot:fetch_or()) then
            obj.Presence:eq(1):next(function()
                curnumber = curnumber + 1
            end):fetch_or()
        end
    end):fetch_or()
    if curnumber ~= 0 or maxnumber ~= 0 then
        result = {MaxNumber = maxnumber, CurrentNumber = curnumber, DetailLink = '/UI/Rest/System/Boards/PCIeCard'}
    end
    return result
end

local function get_cic_card()
    local result = nil
    local curnumber = 0
    local maxnumber = O.ProductComponent.CICCardNum:fetch_or(0)
    if maxnumber == 0xff then
        maxnumber = 0
    end
    C.CICCard():fold(function(obj)
        if class_name(obj) == 'CICCard' then -- Type:CICCard
            obj.Presence:eq(1):next(function()
                curnumber = curnumber + 1
            end):fetch_or()
        end
    end):fetch_or()
    if curnumber ~= 0 or maxnumber ~= 0 then
        result = {MaxNumber = maxnumber, CurrentNumber = curnumber, DetailLink = '/UI/Rest/System/Boards/CICCard'}
    end
    return result
end

local function get_ar_card()
    local result = nil
    local curnumber = 0
    local maxnumber = O.ProductComponent.ARCardNum:fetch_or(0)
    if maxnumber == 0xff then
        maxnumber = 0
    end
    C.ARCard():fold(function(obj)
        if class_name(obj) == 'ARCard' then -- Type:ARCard
            obj.Presence:eq(1):next(function()
                curnumber = curnumber + 1
            end)
        end
    end):fetch_or()
    if curnumber ~= 0 or maxnumber ~= 0 then
        result = {MaxNumber = maxnumber, CurrentNumber = curnumber, DetailLink = '/UI/Rest/System/Boards/ARCard'}
    end
    return result
end

function M.GetBoards()
    local result = {}
    local sys_name = get_system_name()
    local boards_property= {}
    if dal_match_product_id(PRODUCT_ID_TIANCHI_SERVER) == true then
        boards_property = G_TIANCHI_BOARDS_PROPERTY
    else 
        boards_property = G_BOARDS_PROPERTY
    end

    for _, v in ipairs(boards_property) do
        local curnumber = 0
        local maxnumber = nil
        if v[3] ~= null then
            maxnumber = O.ProductComponent[v[3]]:fetch_or(0)
        end

        C.Component():fold(function(obj)
            obj.Presence:eq(1):next(function()
                obj.FruId:neq(0xff):next(function()
                    if obj.DeviceType:fetch() == v[1] then
                        curnumber = curnumber + 1
                    elseif obj.DeviceType:fetch() == DEVICE_TYPE.PSU_BACKPLANE and v[1] == DEVICE_TYPE.IO_BOARD then
                        -- 电源背板归类为IO板
                        curnumber = curnumber + 1
                    end
                end):fetch_or()
            end):fetch_or()
        end):fetch_or()
        if maxnumber and maxnumber ~= 0xff then
            if maxnumber ~= 0 then
                result[v[2]] = {
                    MaxNumber = maxnumber >= curnumber and maxnumber or curnumber,
                    CurrentNumber = curnumber,
                    DetailLink = '/UI/Rest/System/Boards/' .. v[2]
                }
            else 
                if maxnumber == 0 and curnumber ~= 0 and sys_name == 'RMM' then
                    result[v[2]] = {
                        MaxNumber = null,
                        CurrentNumber = curnumber,
                        DetailLink = '/UI/Rest/System/Boards/' .. v[2]
                    }
                end
            end
        else
            if curnumber ~= 0 then
                result[v[2]] = {
                    MaxNumber = null,
                    CurrentNumber = curnumber,
                    DetailLink = '/UI/Rest/System/Boards/' .. v[2]
                }
            end
        end
    end
    if sys_name ~= 'RMM' then
        result['BBUModule'] = get_bbu()
        result['SecurityModule'] = get_security_module()
        result['LCD'] = get_lcd()
        result['PeripheralFirmware'] = get_peripheral_firmware()
        result['SDCardController'] = get_sd_controller()
        result['OCPCard'] = get_ocp_card()
        result['PCIeCard'] = get_pcie_card()
        result['CICCard'] = get_cic_card()
        result['ARCard'] = get_ar_card()
    end
    C.LiquidCoolingUnits():fold(function(obj)
        obj.Presence:eq(1):next(function()
            result['EcuBoard'] = {
                MaxNumber = null,
                CurrentNumber = 1,
                DetailLink = '/UI/Rest/System/Boards/EcuBoard'
            }
        end):fetch_or()
    end):fetch_or()

    return result
end

local function IntToHexStr(data)
    return data and string.format('0x%04x', data) or null
end

function M.GetExpandBoard()
    return C.Component():fold(function(obj, acc)
        if obj.Presence:fetch_or(0) ~= 1 or obj.FruId:fetch_or(0xff) == 0xff or 
            (obj.DeviceType:fetch() ~= DEVICE_TYPE.EXPANDER and obj.DeviceType:fetch() ~= DEVICE_TYPE.EXPANSION_BOARD) then
            return acc
        end
        C.ExpBoard():fold(function(expBoard)
            if not expBoard.RefComponent:eq(dflib.object_name(obj)):fetch_or() then
                return
            end
            acc[#acc + 1] = {
                BoardName = expBoard.Name:fetch(),
                Location = obj.Location:fetch() .. get_chassis_disk_backplane(obj),
                Manufacturer = obj.Manufacturer:neq('N/A'):fetch_or(null),
                CardNo = obj.DeviceNum:fetch(),
                Description = expBoard.Desc:fetch_or('ExpBoard'),
                PCBVersion = expBoard.PcbVer:next(rf_string_check):fetch_or(null),
                CPLDVersion = expBoard.LogicVer:next(rf_string_check):fetch_or(null),
                BoardID = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null),
                PartNumber = dal_get_specific_object_position(obj, 'ExpBoard'):next(
                    function(cardObj)
                        return cardObj.PartNum:fetch()
                    end):neq('Unknown'):neq('N/A'):fetch_or(null),
                SerialNumber = get_serial_number(obj) or null
            }
            add_component_uid(acc[#acc], obj.ComponentUniqueID:fetch_or(null))
        end)
        return acc
    end, {}):fetch_or({})
end

function M.GetDiskBackplane()
    local result = {}
    C.Component():fold(function(obj)
        obj.Presence:eq(1):next(function()
            obj.FruId:neq(0xff):next(function()
                if obj.DeviceType:fetch() == DEVICE_TYPE.HDD_BACKPLANE then
                    C.HDDBackplane():fold(function(backplane)
                        backplane.RefComponent:eq(dflib.object_name(obj)):next(
                            function()
                                result[#result + 1] = {
                                    BoardName = backplane.Name:fetch(),
                                    Location = obj.Location:fetch() .. get_chassis_disk_backplane(obj),
                                    Manufacturer = obj.Manufacturer:neq('N/A'):fetch_or(null),
                                    CardNo = obj.DeviceNum:fetch(),
                                    Description = backplane.Type:fetch(),
                                    PCBVersion = backplane.PcbVer:fetch(),
                                    CPLDVersion = backplane.LogicVer:fetch(),
                                    BoardID = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null),
                                    PartNumber = backplane.PartNum:neq('N/A'):fetch_or(null),
                                    SerialNumber = get_serial_number(obj) or null
                                }
                                add_component_uid(result[#result], obj.ComponentUniqueID:fetch_or(null))
                            end):fetch_or()
                    end):fetch_or()
                end
            end):fetch_or()
        end):fetch_or()
    end):fetch_or()
    return result
end

function M.GetLeakageDetectionCard()
    local result = {}
    C.LeakageDetectionCard():fold(function(obj)
        local boardId = obj.BoardId:fetch_or()
        result[#result + 1] = {
            BoardName = obj.BoardName:fetch_or(null),
            PCBVersion = obj.PcbVer:fetch_or(null),
            BoardId = boardId and string.format('0x%04x', boardId) or null,
            PartNumber = obj.BomCode:fetch_or(null)
        }
    end):fetch_or(null)

    return result
end

local function get_retimer_version(obj)
    local result = {}
    C.Retimer():fold(function(retimerobj)
        retimerobj.RefComponent:eq(dflib.object_name(obj)):next(function()
            result[#result + 1] = {
                Version = retimerobj.SoftVer:fetch_or(null),
                PositionId = retimerobj.ChipLocation:fetch_or(null)
            }
        end)
    end):fetch_or(null)
    return result
end

function M.GetPCIeRiserCard()
    local result = {}
    C.Component():fold(function(obj)
        obj.Presence:eq(1):next(function()
            obj.FruId:neq(0xff):next(function()
                if obj.DeviceType:fetch() == DEVICE_TYPE.PCIE_RISER then
                    local risercardobj = dal_get_specific_object_position(obj, 'RiserPcieCard')
                    risercardobj:next(function(risercard)
                        result[#result + 1] = {
                            BoardName = risercard.Name:fetch(),
                            Manufacturer = obj.Manufacturer:neq('N/A'):fetch_or(null),
                            CardNo = obj.DeviceNum:fetch(),
                            Description = risercard.Type:fetch(),
                            PCBVersion = risercard.PcbVer:fetch(),
                            BoardID = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null),
                            PartNumber = risercard.PartNum:neq('N/A'):fetch_or(null),
                            SerialNumber = get_serial_number(obj) or null,
                            Location = obj.Location:fetch(),
                            RetimerVersion = get_retimer_version(obj)
                        }
                        add_component_uid(result[#result], obj.ComponentUniqueID:fetch_or(null))
                    end):fetch_or()
                end
            end):fetch_or()
        end):fetch_or()
    end):fetch_or()
    return result
end

local PCIEFUNCTION_VALUE = {
    PCIESWITCH = {0xFF, 'PCIeSwitch'},
    PCH = {0xFE, 'PCH'},
    CPU1_AND_CPU2 = {0xFD, 'CPU1,CPU2'},
    ALL_CPU = {0xFC, 'CPU1,CPU2,CPU3,CPU4'}
}

local function get_associatedresource(obj)
    local resid = obj.ResId:fetch()
    local associated_resource = null

    if resid == 0 then
        return associated_resource
    elseif resid == PCIEFUNCTION_VALUE.PCIESWITCH[1] then
        associated_resource = PCIEFUNCTION_VALUE.PCIESWITCH[2]
    elseif resid == PCIEFUNCTION_VALUE.PCH[1] then
        associated_resource = PCIEFUNCTION_VALUE.PCH[2]
    elseif resid == PCIEFUNCTION_VALUE.CPU1_AND_CPU2[1] then
        associated_resource = PCIEFUNCTION_VALUE.CPU1_AND_CPU2[2]
    elseif resid == PCIEFUNCTION_VALUE.ALL_CPU[1] then
        associated_resource = PCIEFUNCTION_VALUE.ALL_CPU[2]
    else
        associated_resource = string.format('CPU%d', resid)
    end

    return associated_resource
end

function M.GetRAIDCard()
    local result = {}
    C.Component():fold(function(obj)
        obj.Presence:eq(1):next(function()
            obj.FruId:neq(0xff):next(function()
                if obj.DeviceType:fetch() == DEVICE_TYPE.RAID_CARD then
                    C.Raid():fold(function(raid)
                        raid.RefComponent:eq(dflib.object_name(obj)):next(
                            function()
                                local cpld = null
                                raid.Firmware:neq(''):next(
                                    function()
                                        cpld = raid:ref_obj('Firmware'):fetch().VersionString:fetch()
                                    end)
                                result[#result + 1] = {
                                    ProductName = raid.ProductName:fetch(),
                                    Location = obj.Location:fetch(),
                                    Manufacturer = obj.Manufacturer:neq('N/A'):fetch_or(null),
                                    CardNo = obj.DeviceNum:fetch(),
                                    Description = raid.Type:fetch(),
                                    PCBVersion = raid.PcbVer:fetch(),
                                    CPLDVersion = cpld,
                                    BoardID = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null),
                                    PartNumber = raid.PartNum:neq('N/A'):fetch_or(null),
                                    SerialNumber = get_serial_number(obj) or null,
                                    AssociatedResource = get_associatedresource(raid)
                                }
                            end):fetch_or()
                    end):fetch_or()
                end
            end):fetch_or()
        end):fetch_or()
    end):fetch_or()
    return result
end

function M.GetPassThroughCard()
    local result = {}
    C.Component():fold(function(obj)
        obj.Presence:eq(1):next(function()
            obj.FruId:neq(0xff):next(function()
                if obj.DeviceType:fetch() == DEVICE_TYPE.PASS_THROUGH_CARD then
                    C.PassThroughCard():fold(function(card_obj)
                        card_obj.RefComponent:eq(dflib.object_name(obj)):next(
                            function()
                                result[#result + 1] = {
                                    ProductName = card_obj.Name:fetch(),
                                    Manufacturer = obj.Manufacturer:neq('N/A'):fetch_or(null),
                                    CardNo = obj.DeviceNum:fetch(),
                                    Description = card_obj.Desc:fetch(),
                                    PCBVersion = card_obj.PcbVer:fetch(),
                                    BoardID = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null),
                                    PartNumber = card_obj.PartNum:neq('N/A'):fetch_or(null),
                                    SerialNumber = get_serial_number(obj) or null
                                }
                            end):fetch_or()
                    end):fetch_or()
                end
            end):fetch_or()
        end):fetch_or()
    end):fetch_or()
    return result
end

function M.GetMEZZCard()
    local result = {}
    C.Component():fold(function(obj)
        obj.Presence:eq(1):next(function()
            obj.FruId:neq(0xff):next(function()
                if obj.DeviceType:fetch_or() == DEVICE_TYPE.MEZZ_CARD then
                    C.MezzCard():fold(function(card_obj)
                        card_obj.RefComponent:eq(dflib.object_name(obj)):next(
                            function()
                                result[#result + 1] = {
                                    ProductName = card_obj.ProductName:next(rf_string_check):fetch_or(null),
                                    CardNo = obj.DeviceNum:next(rf_string_check):fetch_or(null),
                                    Description = card_obj.Desc:next(rf_string_check):fetch_or(null),
                                    PCBVersion = card_obj.PcbVer:next(rf_string_check):fetch_or(null),
                                    BoardID = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null),
                                    PartNumber = card_obj.PartNum:next(rf_string_check):fetch_or(null),
                                    SerialNumber = get_serial_number(obj) or null,
                                    AssociatedResource = get_associatedresource(card_obj),
                                    LinkWidth = card_obj.LinkWidth:next(rf_string_check):fetch_or(null),
                                    LinkSpeed = card_obj.LinkSpeed:next(rf_string_check):fetch_or(null),
                                    LinkWidthAbility = card_obj.LinkWidthAbility:next(rf_string_check):fetch_or(null),
                                    LinkSpeedAbility = card_obj.LinkSpeedAbility:next(rf_string_check):fetch_or(null)

                                }
                            end):fetch_or()
                    end):fetch_or()
                end
            end):fetch_or()
        end):fetch_or()
    end):fetch_or()
    return result
end

function M.GetPCIeTransformCard()
    local result = {}
    C.Component():fold(function(obj)
        obj.Presence:eq(1):next(function()
            obj.FruId:neq(0xff):next(function()
                if obj.DeviceType:fetch() == DEVICE_TYPE.BOARD_PCIE_ADAPTER then
                    local pcie_transform_card_obj = dal_get_specific_object_position(obj, 'PCIeTransformCard')
                    pcie_transform_card_obj:next(function(card_obj)
                        result[#result + 1] = {
                            BoardName = card_obj.Name:fetch(),
                            CardNo = obj.DeviceNum:fetch(),
                            Description = card_obj.Desc:fetch(),
                            PCBVersion = card_obj.PcbVer:fetch(),
                            BoardID = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null)
                        }
                    end):fetch_or()
                end
            end):fetch_or()
        end):fetch_or()
    end):fetch_or()
    return result
end

function M.GetGPUBoard()
    local result = {}
    C.Component():fold(function(obj)
        obj.Presence:eq(1):next(function()
            obj.FruId:neq(0xff):next(function()
                if obj.DeviceType:fetch() == DEVICE_TYPE.GPU_BOARD then
                    local gpu_board_obj = dal_get_specific_object_position(obj, 'GPUBoard')
                    gpu_board_obj:next(function(board_obj)
                        result[#result + 1] = {
                            BoardName = board_obj.BoardName:fetch(),
                            PCBVersion = board_obj.PcbVer:fetch(),
                            CPLDVersion = board_obj.LogicVer:fetch(),
                            BoardID = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null),
                            PositionId = 'U'..board_obj.LogicUnit:fetch(),
                            CardNo = obj.DeviceNum:fetch_or()
                        }
                    end):fetch_or()
                end
            end):fetch_or()
        end):fetch_or()
    end):fetch_or()
    return result
end

local function get_specific_peripheral_card_obj(obj)
    return dal_get_object_list_position(obj, 'PeripheralCard'):next(
        function(objList)
            local compType, deviceName = obj.DeviceType:fetch(), obj.DeviceName:fetch()
            for i = 1, #objList do
                if objList[i].Type:fetch() == compType and objList[i].BoardName:fetch() == deviceName then
                    return objList[i]
                end
            end
        end):neq(nil):catch(function()
        logging:error('get_specific_peripheral_card_obj: obj = %s', object_name(obj))
    end)
end

function M.GetFanBackplane()
    local result = {}
    C.Component():fold(function(obj)
        obj.Presence:eq(1):next(function()
            obj.FruId:neq(0xff):next(function()
                if obj.DeviceType:fetch() == DEVICE_TYPE.FAN_BACKPLANE then
                    local pcb_version = null
                    C.Fru():fold(function(fruobj)
                        fruobj.FruId:eq(obj.FruId:fetch()):next(
                            function()
                                pcb_version = fruobj.PcbVersion:fetch()
                            end)
                    end):fetch()

                    local fan_obj = dal_get_specific_object_position(obj, 'FANBoard'):fetch_or() or
                                        get_specific_peripheral_card_obj(obj):fetch_or()

                    result[#result + 1] = {
                        BoardName = fan_obj and fan_obj.Name:fetch_or(null) or null,
                        Location = obj.Location:fetch_or(null),
                        Manufacturer = obj.Manufacturer:neq('N/A'):fetch_or(null),
                        DeviceType = 'FANBoard',
                        PCBVersion = pcb_version,
                        BoardID = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null)
                    }
                    add_component_uid(result[#result], obj.ComponentUniqueID:fetch_or(null))
                end
            end):fetch_or()
        end):fetch_or()
    end):fetch_or()
    return result
end

function M.GetHorizontalConnectionBoard()
    local result = {}
    C.Component():fold(function(obj)
        obj.Presence:eq(1):next(function()
            obj.FruId:neq(0xff):next(function()
                if obj.DeviceType:fetch() == DEVICE_TYPE.HORIZ_CONN_BRD then
                    local connection_board_obj = dal_get_specific_object_position(obj, 'HorizConnectionBoard')
                    connection_board_obj:next(function(board_obj)

                        local pcb_version = null
                        C.Fru():fold(function(fruobj)
                            fruobj.FruId:eq(obj.FruId:fetch()):next(
                                function()
                                    pcb_version = fruobj.PcbVersion:fetch()
                                end):fetch_or()
                        end):fetch_or()

                        result[#result + 1] = {
                            BoardName = board_obj.Name:fetch(),
                            PCBVersion = pcb_version,
                            BoardID = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null)
                        }
                    end):fetch_or()
                end
            end):fetch_or()
        end):fetch_or()
    end):fetch_or()
    return result
end

function M.GetCPUBoard()
    local result = {}
    C.Component():fold(function(obj)
        obj.Presence:eq(1):next(function()
            obj.FruId:neq(0xff):next(function()
                if obj.DeviceType:fetch() == DEVICE_TYPE.CPU_BOARD then
                    local cpu_board_obj = dal_get_specific_object_position(obj, 'CpuBoard')
                    cpu_board_obj:next(function(board_obj)
                        result[#result + 1] = {
                            BoardName = board_obj.Name:fetch(),
                            Manufacturer = obj.Manufacturer:neq('N/A'):fetch_or(null),
                            CardNo = obj.DeviceNum:fetch(),
                            Description = board_obj.Type:fetch(),
                            PCBVersion = board_obj.PcbVer:fetch(),
                            CPLDVersion = board_obj.LogicVer:fetch(),
                            BoardID = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null),
                            PowerWatts = board_obj.Power:neq(0):fetch_or(null)
                        }
                        add_component_uid(result[#result], obj.ComponentUniqueID:fetch_or(null))
                    end):fetch_or()
                end
            end):fetch_or()
        end):fetch_or()
    end):fetch_or()
    return result
end

function M.GetMemoryBoard()
    local result = {}
    C.Component():fold(function(obj)
        obj.Presence:eq(1):next(function()
            obj.FruId:neq(0xff):next(function()
                if obj.DeviceType:fetch() == DEVICE_TYPE.MEMORY_BOARD then
                    local memory_board_obj = dal_get_specific_object_position(obj, 'MemoryBoard')
                    memory_board_obj:next(function(board_obj)
                        result[#result + 1] = {
                            BoardName = board_obj.Name:fetch(),
                            Manufacturer = obj.Manufacturer:neq('N/A'):fetch_or(null),
                            CardNo = obj.DeviceNum:fetch(),
                            Description = board_obj.Type:fetch(),
                            PCBVersion = board_obj.PcbVer:fetch(),
                            BoardID = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null)
                        }
                    end):fetch_or()
                end
            end):fetch_or()
        end):fetch_or()
    end):fetch_or()
    return result
end

local function GetPwrBkpBoardInfo(obj, property)
    local val = nil
    C.Fru():fold(function(fruobj)
        fruobj.FruId:eq(obj.FruId:fetch()):next(function()
            if fruobj.Elabel:fetch() ~= null then
                val = O[fruobj.Elabel:fetch()][property]:next(rf_string_check):fetch_or(null)
            end
        end)
    end)
    return val
end

function M.GetIOBoard()
    local result = {}
    local sys_name = get_system_name()
    C.Component():fold(function(obj)
        obj.Presence:eq(1):next(function()
            obj.FruId:neq(0xff):next(function()
                if sys_name ~= 'RMM' then
                    if obj.DeviceType:fetch() == DEVICE_TYPE.IO_BOARD or obj.DeviceType:fetch() ==
                        DEVICE_TYPE.PSU_BACKPLANE then
                        local io_board_obj = dal_get_specific_object_position(obj, 'IOBoard')
                        local className = redfish_get_board_class_name(obj)
                        io_board_obj:next(function(board_obj)
                            result[#result + 1] = {
                                BoardName = board_obj.Name:next(rf_string_check):fetch_or(null),
                                Location = obj.Location:next(dal_clear_string_blank):neq(''):fetch_or(null),
                                Manufacturer = obj.Manufacturer:next(rf_string_check):fetch_or(null),
                                CardNo = obj.DeviceNum:fetch_or(null),
                                Description = board_obj.Type:fetch_or('IOBoard'),
                                PCBVersion = board_obj.PcbVer:next(rf_string_check):fetch_or(null),
                                CPLDVersion = board_obj.LogicVer:next(rf_string_check):fetch_or(null),
                                BoardID = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null),
                                PartNumber = board_obj.PartNum:next(rf_string_check):fetch_or(null),
                                SerialNumber = get_serial_number(obj) or null,
                                PowerWatts = board_obj.Power:neq(0):fetch_or(null),
                                FirmwareVersion = get_chassis_board_firmwareversion(obj, className)
                            }
                        end)
                    end
                else
                    if obj.DeviceType:fetch() == DEVICE_TYPE.PSU_BACKPLANE then
                        result[#result + 1] = {
                            BoardName = GetPwrBkpBoardInfo(obj, 'ProductName'),
                            ProductName = GetPwrBkpBoardInfo(obj, 'ProductName'),
                            Location = obj.Location:next(rf_string_check):fetch_or(null),
                            Manufacturer = obj.Manufacturer:next(rf_string_check):fetch_or(null),
                            CardNo = obj.DeviceNum:fetch_or(null),
                            DeviceType = 'PowerBoard',
                            PCBVersion = null,
                            BoardID = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null),
                            PartNumber = GetPwrBkpBoardInfo(obj, 'ProductPartNumber'),
                            SerialNumber = GetPwrBkpBoardInfo(obj, 'ProductSerialNumber')
                        }
                    end
                end
            end)
        end)
    end)
    return result
end

function M.GetChassisBackplane()
    local result = {}
    C.Component():fold(function(obj)
        obj.Presence:eq(1):next(function()
            obj.FruId:neq(0xff):next(function()
                if obj.DeviceType:fetch() == DEVICE_TYPE.CHASSIS_BACKPLANE then
                    local chassis_board_obj = dal_get_specific_object_position(obj, 'PeripheralCard')
                    chassis_board_obj:next(function(board_obj)
                        result[#result + 1] = {
                            BoardName = board_obj.Name:fetch(),
                            Location = obj.Location:fetch(),
                            Manufacturer = obj.Manufacturer:neq('N/A'):fetch_or(null),
                            PCBVersion = board_obj.PcbVer:fetch(),
                            BoardID = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null)
                        }
                    end):fetch_or()
                end
            end):fetch_or()
        end):fetch_or()
    end):fetch_or()
    return result
end

function M.GetM2TransformCard()
    local result = {}
    C.Component():fold(function(obj)
        obj.Presence:eq(1):next(function()
            obj.FruId:neq(0xff):next(function()
                if obj.DeviceType:fetch() == DEVICE_TYPE.EXPAND_BOARD then
                    local m2_card_obj = dal_get_specific_object_position(obj, 'M2TransformCard')
                    m2_card_obj:next(function(card_obj)
                        result[#result + 1] = {
                            BoardName = card_obj.BoardName:fetch_or(),
                            CardNo = obj.DeviceNum:fetch(),
                            Description = card_obj.Desc:fetch(),
                            PCBVersion = card_obj.PcbVer:fetch(),
                            BoardID = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null),
                            PartNumber = card_obj.PartNum:neq('N/A'):fetch_or(null),
                            SerialNumber = get_serial_number(obj) or null
                        }
                    end):fetch_or()
                end
            end):fetch_or()
        end):fetch_or()
    end):fetch_or()
    return result
end

function M.GetBBUModule()
    local result = {}
    C.BBUModule():fold(function(obj)
        result[#result + 1] = {
            Id = obj.Id:fetch(),
            BoardName = obj.BoardName:fetch(),
            FirmwareVersion = obj.FirmwareVersion:fetch(),
            WorkingState = obj.WorkStatus:next(working_state_to_str):fetch(),
            M2Device1Presence = obj.M2Presence:next(m2_device1_presence):fetch(),
            M2Device2Presence = obj.M2Presence:next(m2_device2_presence):fetch(),
            Battery = {
                RemainingPowerMilliWatts = obj.RemainCap:fetch(),
                Model = obj.BattModel:fetch(),
                SerialNumber = obj.BattSN:fetch(),
                Manufacturer = obj.BattManu:fetch()
            }
        }
    end):fetch_or()
    return result
end

function M.GetARCard()
    local result = {}
    C.Component():fold(function(obj)
        obj.Presence:eq(1):next(function()
            obj.FruId:neq(0xff):next(function()
                if obj.DeviceType:fetch() == DEVICE_TYPE.AR_CARD then
                    local pcb_version = null
                    C.Fru():fold(function(fruobj)
                        fruobj.FruId:eq(obj.FruId:fetch()):next(function()
                            pcb_version = fruobj.PcbVersion:fetch()
                        end)
                    end)
                    local arcard_obj = dal_get_specific_object_position(obj, 'ARCard'):fetch_or()
                    arcard_obj:next(function(card_obj)
                        result[#result + 1] = {
                            BoardName = card_obj.Desc:fetch(),
                            PCBVersion = pcb_version,
                            BoardID = obj.BoardId:neq(0):fetch_or(null),
                            SlotID = card_obj.Slot:fetch(),
                            DeviceType = 'ARCard',
                            Location = obj.Location:fetch(),
                            Manufacturer = obj.Manufacturer:neq('N/A'):fetch_or(null)
                        }
                    end)
                end
            end)
        end)
    end)
    return result
end

function M.GetSecurityModule()
    local result = {}
    C.SecurityModule():fold(function(obj)
        obj.Presence:eq(1):next(function()
            result[#result + 1] = {
                SelfTestResult = obj.SelfTestResult:next(selftest_result_to_str):fetch(),
                ProtocolType = obj.SpecificationType:fetch_or(null),
                ProtocolVersion = obj.SpecificationVersion:fetch_or(null),
                Manufacturer = obj.ManufacturerName:fetch_or(null),
                FirmwareVersion = obj.ManufacturerVersion:next(check_str_effective):fetch()
            }
        end)
    end):fetch()
    return result
end

function M.GetLCD()
    local result = {}
    C.Lcd():fold(function(obj)
        obj.Presence:eq(1):next(function()
            result[#result + 1] = {Version = obj.LcdVer:fetch()}
        end)
    end):fetch()
    return result
end

function M.GetPeripheralFirmware()
    local result = {}

    C.PeripheralFirmware():fold(function(obj)
        local name = 'Peripheral' .. obj.Name:fetch()
        result[#result + 1] = {
            Name = string.gsub(name, '%s+', ''),
            Manufacturer = obj.Manufacturer:next(check_str_effective):fetch_or(null),
            Version = string.gsub(obj.VersionString:fetch(), '%s+', '')
        }
    end):fetch()
    return result
end

function M.GetSDCardController()
    local result = {}
    C.RaidChip():fold(function(obj)
        obj.Presence:eq(1):next(function()
            result[#result + 1] = {Manufacturer = obj.Name:fetch(), FirmwareVersion = obj.Version:fetch()}
        end)
    end):fetch()
    return result
end

function M.GetOCPCard()
    local result = {}
    C.OCPCard():fold(function(obj)
        if class_name(obj) == 'OCPCard' then -- Type:PCIe || PCIeConnector
            obj.Presence:eq(1):next(function()
                local component_handle = get_pciecard_component_handle(obj)
                local response = {}
                response['Id'] = 'PCIeCard' .. get_pciecard_logic_slot(obj)
                response['Description'] = obj.CardDesc:next(check_str_effective):fetch()
                response['Manufacturer'] = obj.Manufacturer:next(check_str_effective):fetch()
                response['Position'] = component_handle.Location:fetch()
                response['ProductName'] = obj.Name:next(check_str_effective):fetch()
                response['OCPFunctionDate'] = get_ocp_card_pciefunction(obj)

                result[#result + 1] = response
            end):fetch_or()
        end
    end):fetch()
    return result
end

local function get_npu_memory_capacity(obj)
    local count = 0;
    dal_get_object_list_position(obj, 'NPU'):next(
        function(objList)
            for i = 1, #objList do
                local compMemoryCapacity = objList[i].MemoryCapacityKiB:fetch()
                if compMemoryCapacity ~= nil then
                    count = count + compMemoryCapacity
                end
            end
        end):neq(nil):catch(function()
        logging:error('dal_get_object_list_position: obj = %s', object_name(obj))
    end)
    count = math.modf(count / 1024)
    if count == 0 then
        return nil
    else
        return count
    end
end

function M.GetPCIeCard(user)
    local result = {}
    C.PcieCard():fold(function(obj)
        if check_connector_mached(obj.PcieCardSlot:fetch_or()) then
            obj.Presence:eq(1):next(function()
                local component_handle = get_pciecard_component_handle(obj)
                local response = {}
                response['Id'] = 'PCIeCard' .. get_pciecard_logic_slot(obj)
                response['Description'] = obj.CardDesc:next(check_str_effective):fetch()
                response['Manufacturer'] = obj.Manufacturer:next(check_str_effective):fetch()
                response['FirmwareVersion'] = get_pcie_card_firmwareversion(obj)
                response['SerialNumber'] = get_pcie_card_serialnumber(obj)
                response['PartNumber'] = obj.PartNum:next(check_str_effective):fetch()
                response['Model'] = get_pcie_card_model(obj)
                response['Position'] = component_handle.Location:fetch()
                response['FunctionType'] = get_pcie_card_function_type(obj)
                response['ExtendInfo'] = get_pcie_extend_info(user, obj)
                local card_type = obj.FunctionClass:fetch()
                if card_type == PCIECARD_FUNCTION_FPGA or card_type == PCIECARD_FUNCTION_NPU or card_type == PCIECARD_FUNCTION_SDI then
                    local slavecard = obj.SlaveCard:fetch()
                    local slave_handle = dal_get_specific_object_position(obj, slavecard):fetch_or()
                    if slave_handle and slave_handle ~= "" then
                        local mcu_supported = slave_handle.McuSupported:fetch()
                        if mcu_supported == 1 then -- PCIE卡支持MCU的管理
                            local SENSOR_INVALID_READING = 0x8000
                            local SENSOR_NA_READING = 0x4000
                            local power = slave_handle.Power:fetch()
                            if bit.band(power, SENSOR_NA_READING) == SENSOR_NA_READING or
                                bit.band(power, SENSOR_INVALID_READING) == SENSOR_INVALID_READING then
                                response['Power'] = null
                            else
                                response['Power'] = slave_handle.PowerDouble:fetch()
                            end
                            response['McuFirmwareVersion'] = slave_handle.McuVersion:fetch_or(null)
                            response['MemoryCapacityMB'] = get_npu_memory_capacity(obj)
                        elseif card_type ~= PCIECARD_FUNCTION_SDI then
                            -- to do Type  redfish_provider_chassis_pciedevices.c 1445
                            response['Power'] = slave_handle.Power:fetch()
                        end
                    end
                end
                response['PCIeFunctionDate'] = get_pcie_card_pciefunction(obj)

                result[#result + 1] = response
            end):fetch_or()
        end
    end):fetch_or()
    return result
end

function M.GetCICCard(user)
    local result = {}
    C.CICCard():fold(function(obj)
        if class_name(obj) == 'CICCard' then -- Type:CICCard
            obj.Presence:eq(1):next(function()
                local response = {}
                response['BoardId'] = string.format('0x%04x', obj.BoardId:fetch())
                response['Description'] = obj.CardDesc:next(check_str_effective):fetch()
                response['Manufacturer'] = obj.Manufacture:next(check_str_effective):fetch()
                response['BoardName'] = obj.BoardName:next(check_str_effective):fetch()
                response['SerialNumber'] = obj.SerialNumber:next(check_str_effective):fetch()
                response['PartNum'] = obj.PartNum:next(check_str_effective):fetch()
                response['Presence'] = obj.Presence:fetch()
                response['PcbVer'] = obj.PcbVer:fetch()
                response['M2Device1Presence'] = obj.M2Device1Presence:fetch()
                response['M2Device2Presence'] = obj.M2Device2Presence:fetch()

                result[#result + 1] = response
            end):fetch_or()
        end
    end):fetch_or()
    return result
end

function M.GetConvergeBoard(user)
    local result = {}
    local exist = false
    C.ConvergeBoard():fold(function(obj)
        result[#result + 1] = {
            BoardName = obj.Name:next(check_str_effective):fetch_or(null),
            Manufacturer = obj.Manufacturer:next(check_str_effective):fetch_or(null),
            PCBVersion = obj.PcbVer:next(check_str_effective):fetch_or(null),
            CPLDVersion = obj.CpldVersion:next(check_str_effective):fetch_or(null),
            BoardID = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null),
            PartNumber = obj.PartNumber:next(check_str_effective):fetch_or(null),
            SerialNumber = obj.SerialNumber:next(check_str_effective):fetch_or(null)
        }
        exist = true
    end):fetch_or()
    if not exist then
        return reply_not_found()
    end
    return result
end
function M.GetEcuBoard()
    local result = {}
    C.LiquidCoolingUnits():fold(function(obj)
        obj.Presence:eq(1):next(function()
            result[#result + 1] = {
                Name = 'ECU',
                Manufacturer = null,
                SerialNumber = null
            }
        end):fetch_or()
    end):fetch_or()
    return result
end

function M.SetSdiCardNMIMethod(user, data)
    local ok, err = utils.call_method(user, C.PCIeSDICard[0]:next():fetch(), "SetSDICardNMI", SDI_CARD_ERR_MAP,
    gbyte(data.SlotId))
    if not ok then
        logging:error("set SdiCard NMI failed...")
        return err
    end
    return reply_ok()
end

return M
