local dflib = require "dflib"
local c = require "dflib.core"
local utils = require "utils"
local cjson = require "cjson"
local thermal = require "thermal"
local power = require "power"
local C = dflib.class
local null = cjson.null
local call_method = utils.call_method
local GVariant = c.GVariant
local gint32 = GVariant.new_int32
local SmartModeToStr = thermal.SmartModeToStr
local set_smart_cooling_mode = thermal.set_smart_cooling_mode
local filter_string = power.filter_string

local M = {}

local function get_fru_info(id, type)
    return C.Fru('Position', id + type):next(function(obj)
            local elabel_obj = obj:ref_obj('Elabel'):fetch()
            return {
                Board = {
                    FruFileId = elabel_obj.BoardFileId:next(filter_string):fetch_or(null),
                    Manufacturer = elabel_obj.BoardManufacturer:next(filter_string):fetch_or(null),
                    ManufacturingDate = elabel_obj.MfgDateTime:next(filter_string):fetch_or(null),
                    PartNumber = elabel_obj.BoardPartNumber:next(filter_string):fetch_or(null),
                    ProductName = elabel_obj.BoardProductName:next(filter_string):fetch_or(null),
                    SerialNumber = elabel_obj.BoardSerialNumber:next(filter_string):fetch_or(null),
                },
                Product = {
                    Manufacturer = elabel_obj.ProductManufacturer:next(filter_string):fetch_or(null),
                    PartNumber = elabel_obj.ProductPartNumber:next(filter_string):fetch_or(null),
                    ProductName = elabel_obj.ProductName:next(filter_string):fetch_or(null),
                    SerialNumber = elabel_obj.ProductSerialNumber:next(filter_string):fetch_or(null),
                    Version = elabel_obj.ProductVersion:next(filter_string):fetch_or(null),
                },
            }
    end):fetch()
end

local FAN_FRU_POSITION_BASE = 2
function M.GetChassisThermal()
    local fan = C.SMMFANClass():fold(function(obj, fanList)
        local fanId = obj.FANSlot:fetch()
        local fanName=string.format('Fan%d', fanId)
        fanList[#fanList + 1] = obj.Presence:eq(1):next(function()
            return {
                Name = fanName,
                Health = utils.get_property_and_severity(obj.FanHealth:fetch_or(null)),
                Speed = C.Cooling():next(function(cooling_obj)
                        local ok, ret = call_method(nil, cooling_obj, 'GetFanState', nil, {gint32(fanId)})
                        if not ok then
                            return ret
                        end
                        return ret[2]
                end):fetch(),
                SoftwareVersion = obj.SoftVersion:fetch_or(null),
                PcbVersion = string.char(obj.PCBVersion:fetch_or(0) + string.byte('A')),
                Fru = get_fru_info(fanId - 1, FAN_FRU_POSITION_BASE),
                State = 'Enabled'
            }
        end):catch(function()
            return {
                Name = fanName,
                Health = null,
                Speed = null,
                SoftwareVersion = null,
                PcbVersion = null,
                State = 'Absent',
                Fru = null
            }
        end):fetch()
        return fanList
    end, {}):fetch()
    return {
        FanSmartCoolingMode = C.Cooling[0]:next(function(obj)
                if obj.Mode:fetch() == 0 then
                    return SmartModeToStr(obj.DefThermalLevel:fetch())
                else
                    return null
                end
            end):fetch_or(null),
        Fan = fan,
    }
end

function M.UpdateChassisThermal(data, user)
    return set_smart_cooling_mode(user, data.FanSmartCoolingMode)
end

M.get_fru_info = get_fru_info
return M
