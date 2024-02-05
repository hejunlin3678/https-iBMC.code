local cjson = require 'cjson'
local dflib = require 'dflib'
local utils = require 'utils'
local http = require 'http'
local c = require 'dflib.core'
local null = cjson.null
local C = dflib.class
local O = dflib.object
local gstring = c.GVariant.new_string
local gbyte = c.GVariant.new_byte
local guint32 = c.GVariant.new_uint32
local rf_string_check = utils.rf_string_check
local get_system_name = utils.get_system_name
local reply_bad_request = http.reply_bad_request

local COMPONENT_TYPE_MAINBOARD = 16
local RFERR_INSUFFICIENT_PRIVILEGE = 403
local RFERR_NO_RESOURCE = 404

local LoadCapacityErrMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request('InsufficientPrivilege')
}

local RackErrMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request('InsufficientPrivilege'),
    [RFERR_NO_RESOURCE] = reply_bad_request('ResourceMissingAtURI')
}

local M = {}

local function getGuid()
    return O.BMC.DeviceGuid:neq('N/A'):next(function(guid)
        local tmp = {}
        for i = 13, 16, 1 do
            table.insert(tmp, guid[i])
        end
        table.insert(tmp, guid[11])
        table.insert(tmp, guid[12])
        table.insert(tmp, guid[9])
        table.insert(tmp, guid[10])
        for i = 8, 1, -1 do
            table.insert(tmp, guid[i])
        end
        return string.format('%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X', unpack(tmp))
    end):fetch_or(null)
end

local function getManufactureDate()
    return O.Fru0Elabel.MfgDateTime:neq('N/A'):next(function(str)
        -- 制造时间中的/转换为-
        return string.gsub(str, '/', '-')
    end):fetch_or(null)
end

local function CheckRackStringvalid(str_value, num_value)
    if num_value < 10 then
        if string.len(str_value) > 1 then              -- 数值在0至9之间的整数对应的字符串长度为1
            return -1
        end
    elseif 10 <= num_value and num_value < 100 then    -- 数值在10至100之间的整数对应字符串长度为2，包含"10"
        if string.len(str_value) > 2 then
            return -1
        end
    elseif 100 <= num_value and num_value <= 500 then  -- 数值在100至500之间的整数字符串长度为3，包含"100"及"500"
        if string.len(str_value) > 3 then
            return -1
        end
    end
    return nil
end

local function get_unit_number(unit_num)
    if unit_num and unit_num ~= 0 then
        return '(U' .. unit_num .. ')'
    else
        return ''
    end
end

function M.GetProductInfo()
    local rack = {}
    local basicinfo = {}
    local os = {}
    local digitalwarranty = {}
    local mainboard = {}
    local product = {}
    local maxNodeIndex = 0
    local nodeIndex = 0
    local pcbVersion = null
    local boardID = null
    local sys_name = get_system_name()

    C.Component():fold(function(obj)
        if obj.DeviceType:eq(COMPONENT_TYPE_MAINBOARD):fetch_or() and obj.Presence:neq(0):fetch_or() and
            obj.FruId:neq(0xff):fetch_or() then
            pcbVersion = C.Fru('FruId', obj.FruId:fetch()).PcbVersion:next(rf_string_check):fetch_or(null)
            boardID = string.format('0x%04x', obj.BoardId:fetch_or())
            return 0, false
        end
    end):fetch_or()

    if sys_name ~= 'RMM' then
        product = {
            ProductName = O.BMC.SystemName:fetch_or(null),
            ProductAlias = O.PRODUCT.ProductAlias:fetch_or(null),
            ProductManufacturer = O.Fru0Elabel.ProductManufacturer:fetch_or(null),
            AssetTag = O.Fru0Elabel.ProductAssetTag:fetch_or(null),
            ProductSN = O.Fru0Elabel.ProductSerialNumber:fetch_or(null),
            SystemSN = O.Fru0Elabel.SysSerialNum:fetch_or(null),
            DeviceLocation = O.BMC.LocationInfo:fetch_or(null),
            PartNumber = O.Fru0Elabel.ProductPartNumber:fetch_or(null)
        }

        mainboard = {
            BMCVersion = O.BMC.PMEVer:fetch_or(null) .. get_unit_number(O.BMC.FlashUnitNum:fetch_or(null)),
            BIOSVersion = O.Bios.Version:neq('000'):next(function(Version)
                return Version .. get_unit_number(O.Bios.UnitNum:fetch_or(null))
            end):fetch_or(null),
            CPLDVersion = O.BMC.CpldVersion:fetch_or(null) .. get_unit_number(O.BMC.CpldUnitNum:fetch_or(null)),
            ActiveUbootVersion = O.BMC.UbootVersion:fetch_or(null),
            BackupUbootVersion = O.BMC.UbootBackupVer:fetch_or(null),
            PCHModel = O.MainBoard.PCHModel:neq(''):fetch_or(nil),
            PCBVersion = pcbVersion,
            BoardID = boardID,
            BoardSN = O.Fru0Elabel.BoardSerialNumber:fetch_or(null),
            BoardModel = O.Fru0Elabel.BoardProductName:fetch_or(null),
            BoardManufacturer = O.Fru0Elabel.BoardManufacturer:fetch_or(null),
            PartNumber = O.Fru0Elabel.BoardPartNumber:fetch_or(null),
            ChassisID = O.PRODUCT.ServerType:eq(1):next(function()
                return O.PRODUCT.ChassisNum:fetch_or(null)
            end):fetch_or(nil),
            DeviceSlotID = C.Chassis[0]:next(function(obj)
                maxNodeIndex = obj.MaxNodeIndex:fetch_or(0)
                nodeIndex = obj.NodeIndex:fetch_or(0)
                local productId = O.PRODUCT.ProductId:fetch_or(null)
                if nodeIndex > maxNodeIndex then
                    return
                end
                if nodeIndex == 0 and productId ~= 0x0d then
                    return
                end
                return nodeIndex
            end):fetch_or(nil),
            ProductId = O.PRODUCT.ProductId:fetch_or(null)
        }

        os = {
            Domain = O.ComputerSystem0.Domain:neq('N/A'):fetch_or(nil),
            HostName = O.ComputerSystem0.HostName:neq('N/A'):fetch_or(nil),
            HostDescription = O.ComputerSystem0.Description:neq('N/A'):fetch_or(nil),
            OSVersion = O.ComputerSystem0.OSVersion:neq('N/A'):fetch_or(nil),
            KernalVersion = O.ComputerSystem0.KernelVersion:neq('N/A'):fetch_or(nil),
            iBMAVersion = O.sms0.FirmwareVersion:neq('N/A'):fetch_or(nil),
            iBMARunningStatus = O.sms0.Status:neq('N/A'):fetch_or(nil),
            iBMADriver = O.sms0.VNICDriveVersion:neq('N/A'):fetch_or(nil)
        }

        digitalwarranty = {
            ProductName = O.BMC.SystemName:fetch_or(null),
            SerialNumber = O.Fru0Elabel.SysSerialNum:fetch_or(null),
            ManufactureDate = getManufactureDate(),
            UUID = getGuid(),
            UnitType = 'Device',
            StartPoint = O.PRODUCT.StartPoint:neq('N/A'):fetch_or(null),
            Lifespan = O.PRODUCT.Lifespan:neq(0):fetch_or(null)
        }
    end

    if sys_name == 'RMM' then
        rack = {
            Name = O.RackAssetMgmt.Name:neq('N/A'):fetch_or(null),
            Row = O.RackAssetMgmt.Row:neq('N/A'):fetch_or(null),
            Rack = O.RackAssetMgmt.Rack:neq('N/A'):fetch_or(null),
            RatedPowerWatts = O.RackAssetMgmt.RatedPowerWatts:neq(0):fetch_or(null),
            LoadCapacityKg = O.RackAssetMgmt.LoadCapacityKg:neq(0):fetch_or(null),
            TotalUCount = O.RackAssetMgmt.UCount:fetch_or(null),
            UcountUsed = O.RackAssetMgmt.UCountUsed:fetch_or(null)
        }

        basicinfo = {
            Model = O.Fru0Elabel.ProductName:neq(''):fetch_or(null),
            FirmwareVersion = O.BMC.PMEVer:neq(''):fetch_or(null),
            SerialNumber = O.Fru0Elabel.ProductSerialNumber:neq(''):fetch_or(null),
            Manufacturer = O.Fru0Elabel.ProductManufacturer:neq(''):fetch_or(null),
            AssetTag = O.Fru0Elabel.ProductAssetTag:neq(''):fetch_or(null),
            PartNumber = O.Fru0Elabel.ProductPartNumber:neq(''):fetch_or(null),
            BoardID = boardID
        }
    end

    if next(product) == nil then
        product = nil
    end
    if next(os) == nil then
        os = nil
    end
    if next(digitalwarranty) == nil then
        digitalwarranty = nil
    end
    if next(mainboard) == nil then
        mainboard = nil
    end
    if next(rack) == nil then
        rack = nil
    end
    if next(basicinfo) == nil then
        basicinfo = nil
    end

    return {
        Product = product,
        MainBoard = mainboard,
        OS = os,
        DigitalWarranty = digitalwarranty,
        RackInfo = rack,
        BasicInfo = basicinfo
    }
end

function M.UpdateProductInfo(data, user)
    local result = http.reply_ok()

    if data.AssetTag ~= nil then
        result:join(http.safe_call(function()
            local ok, ret = utils.call_method(user, O.Fru0Elabel, 'SetProductAssetTag', nil,
                gstring(data.AssetTag), gbyte(0))
            if not ok then
                return ret
            end
        end))
    end

    if data.DeviceLocation ~= nil then
        result:join(http.safe_call(function()
            local ok, ret = utils.call_method(user, O.BMC, 'SetLocationInfo', nil, gstring(data.DeviceLocation))
            if not ok then
                return ret
            end
        end))
    end

    -- 设置 电子保单的服务起始时间
    if data.DigitalWarranty and data.DigitalWarranty.StartPoint then
        result:join(http.safe_call(function()
            local ok, ret = utils.call_method(user, O.PRODUCT, 'SetStartPoint', nil,
                gstring(data.DigitalWarranty.StartPoint))
            if not ok then
                return ret
            end
        end))
    end

    -- 设置 电子保单的服务年限
    if data.DigitalWarranty and data.DigitalWarranty.Lifespan then
        result:join(http.safe_call(function()
            local ok, ret = utils.call_method(user, O.PRODUCT, 'SetLifespan', nil,
                gbyte(data.DigitalWarranty.Lifespan))
            if not ok then
                return ret
            end
        end))
    end

    -- 设置机柜承重信息
    if data.RackInfo and data.RackInfo.LoadCapacityKg then
        if data.RackInfo.LoadCapacityKg == null then
            data.RackInfo.LoadCapacityKg = 0
        end
        result:join(http.safe_call(function()
            local ok, ret = utils.call_method(user, O.RackAssetMgmt, 'SetLoadCapacity', LoadCapacityErrMap,
                guint32(data.RackInfo.LoadCapacityKg))
            if not ok then
                return ret
            end
        end))
    end

    -- 设置机柜额定功率
    if data.RackInfo and data.RackInfo.RatedPowerWatts then
        if data.RackInfo.RatedPowerWatts == null then
            data.RackInfo.RatedPowerWatts = 0
        end
        result:join(http.safe_call(function()
            local ok, ret = utils.call_method(user, O.RackAssetMgmt, 'SetRatedPower', LoadCapacityErrMap,
                guint32(data.RackInfo.RatedPowerWatts))
            if not ok then
                return ret
            end
        end))
    end

    -- 设置机柜信息
    if data.RackInfo and data.RackInfo.Rack then
        result:join(http.safe_call(function()
            local ok, ret = utils.call_method(user, O.RackAssetMgmt, 'SetRack', RackErrMap, gstring(data.RackInfo.Rack))
            if not ok then
                return ret
            end
        end))
    end

    if data.RackInfo and data.RackInfo.Row then
        result:join(http.safe_call(function()
            local ok, ret = utils.call_method(user, O.RackAssetMgmt, 'SetRow', RackErrMap, gstring(data.RackInfo.Row))
            if not ok then
                return ret
            end
        end))
    end

    if data.RackInfo and data.RackInfo.Name then
        result:join(http.safe_call(function()
            local ok, ret = utils.call_method(user, O.RackAssetMgmt, 'SetName', RackErrMap, gstring(data.RackInfo.Name))
            if not ok then
                return ret
            end
        end))
    end

    if result:isOk() then
        return http.reply_ok_encode_json(M.GetProductInfo())
    end
    return result
end

return M
