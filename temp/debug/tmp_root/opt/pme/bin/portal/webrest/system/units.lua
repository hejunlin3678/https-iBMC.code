local cjson = require 'cjson'
local dflib = require 'dflib'
local utils = require 'utils'
local logging = require 'logging'
local bit = require 'bit'
local ffi = require 'ffi'
local cfg = require 'config'
local class_name = dflib.class_name
local O = dflib.object
local C = dflib.class
local object_name = dflib.object_name
local null = cjson.null
local rf_string_check = utils.rf_string_check
local dal_get_specific_object_position = utils.dal_get_specific_object_position
local add_component_uid = utils.add_component_uid
local dal_match_product_id = utils.dal_match_product_id
local PRODUCT_ID_TIANCHI_SERVER = cfg.PRODUCT_ID_TIANCHI_SERVER
local CDTE = cfg.COMPONENT_DEVICE_TYPE_E
local get_chassis_disk_backplane = utils.get_chassis_disk_backplane
local get_serial_number = utils.get_serial_number
local get_part_number = utils.get_part_number
local dal_get_object_list_position =  utils.dal_get_object_list_position

local M = {}

-- 当前天池组件的ComponentType
local DEVICE_TYPE = {
    STORAGE_EXTENSION_UNIT = 5,
    IO_EXTENSION_UNIT = 15,
    COOLING_UNIT = 24,
    BASIC_COMPUTING_UNIT = 36,
    EXTENSION_UNIT = 93
}

local G_UNITS_PROPERTY = {
    {DEVICE_TYPE.STORAGE_EXTENSION_UNIT, 'StorageExtensionUnit', 'HddBackCardNum'}, -- 存储扩展组件
    {DEVICE_TYPE.IO_EXTENSION_UNIT, 'IOExtensionUnit', 'RiserCardNum'}, -- IO扩展组件
    {DEVICE_TYPE.COOLING_UNIT, 'CoolingUnit', null}, -- 散热组件，没有最大个数
    {DEVICE_TYPE.BASIC_COMPUTING_UNIT, 'BasicComputingUnit', 'CpuBoardNum'}, -- 基础计算组件
    {DEVICE_TYPE.EXTENSION_UNIT, 'ExtensionUnit', 'ExpBoardNum'} -- 系统扩展组件
}

local function get_uint16_hex(val)
    if val == 0 or val == 0xFFFF then
        return null
    end
    return string.format('0x%04x', val)
end

local function get_components_by_type(type)
    local obj_array = {}

    C.Component():fold(function(obj)
        if obj.DeviceType:fetch_or() == type and obj.Presence:fetch_or() == 1 then
            obj_array[#obj_array + 1] = obj
        end
    end)

    return obj_array
end

function get_units_atlas()
    local result = {}
    local curnumber = 0
    local maxnumber = nil

    maxnumber = O.ProductComponent['SoCBoardNum']:fetch_or(0)
    C.Component():fold(function(obj)
        obj.Presence:eq(1):next(function()
            if obj.DeviceType:fetch() == CDTE.COMPONENT_TYPE_SOC_BOARD then
                curnumber = curnumber + 1
            end
        end):fetch_or()
    end):fetch_or()

    if maxnumber and maxnumber ~= 0xff and maxnumber > 0 then
        result['BasicComputingUnit'] = {
            MaxNumber = maxnumber >= curnumber and maxnumber or curnumber,
            CurrentNumber = curnumber,
            DetailLink = '/UI/Rest/System/Units/BasicComputingUnit'
        }
    end
    return result
end

-- 天池组件资源入口
function M.GetUnits()
    if dal_match_product_id(cfg.PRODUCT_ID_ATLAS) == true then
        return get_units_atlas()
    end
    local result = {}
    return result
end

local function get_soc_board_extend_info(obj)
    local BIOS_VERSION_KEY = {
        'Bios1Version',
        'Bios2Version'
    }

    local response = {
        PartNumber = obj.PartNum:next(rf_string_check):fetch_or(null), -- 部件编码
        PCBVersion = obj.PcbVer:fetch_or(null), -- PCB版本
        CPLDVersion = null, -- CPLD版本
        HWSRVersion = null, -- 硬件自描述版本
        SerialNumber = obj.SerialNumber:next(rf_string_check):fetch_or(null), -- 序列号
        BoardId = obj.BoardId:next(get_uint16_hex):fetch_or(null) -- 单板Id
    }

    if obj.McuSupported:fetch() ~= 0 then
        response['McuVersion'] = obj.McuVersion:next(rf_string_check):fetch_or(null)
    end

    dal_get_object_list_position(obj, 'Firmware'):next(function(objList)
        for i = 1, #objList do
            response[BIOS_VERSION_KEY[objList[i].InstanceID:fetch()]] =
                objList[i].VersionString:next(rf_string_check):fetch_or(null)
        end
    end)
    return response
end

function get_bcu_atlas()
    local result = {}
    local component_array = get_components_by_type(CDTE.COMPONENT_TYPE_SOC_BOARD)
    for _, component_obj in ipairs(component_array) do
        local soc_board_obj = dal_get_specific_object_position(component_obj, 'SoCBoard'):fetch_or()
        result[#result + 1] = {
            Name = soc_board_obj.Name:fetch(), -- 名称
            Manufacturer = component_obj.Manufacturer:next(rf_string_check):fetch_or(null), -- 厂商
            Position = component_obj.Location:fetch_or(null), -- 位置
            Id = component_obj.DeviceNum:fetch(), -- 槽位
            Description = soc_board_obj.Description:fetch_or(null), -- 类型
            ComponentUniqueID = null, -- 组件ID
            ExtendInfo = get_soc_board_extend_info(soc_board_obj)
        }
    end
    return result
end

-- 获取基础计算组件信息
function M.GetBasicComputingUnit()
    if dal_match_product_id(cfg.PRODUCT_ID_ATLAS) == true then
        return get_bcu_atlas()
    end
    local result = {}
    return result
end

-- 获取存储扩展组件信息
function M.GetStorageExtensionUnit()
    local result = {}
    return result
end

-- 获取系统扩展组件信息
function M.GetExtensionUnit()
    local result = {}
    return result
end

-- 获取IO扩展组件信息
function M.GetIOExtensionUnit()
    local result = {}
    return result
end

-- 获取散热组件信息
function M.GetCoolingUnit()
    local result = {}
    return result
end

return M