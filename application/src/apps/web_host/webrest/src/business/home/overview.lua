local dflib = require 'dflib'
local defs = require 'define'
local cjson = require 'cjson'
local cfg = require 'config'
local tools = require 'tools'
local utils = require 'utils'
local c = require 'dflib.core'
local http = require 'http'
local download = require 'download'
local logging = require 'logging'
local bit = require 'bit'
local null = cjson.null
local O = dflib.object
local C = dflib.class
local json_decode = cjson.decode
local call_method = utils.call_method
local dal_trim_string = utils.dal_trim_string
local gbyte = c.GVariant.new_byte
local reply_ok = http.reply_ok
local dal_rf_get_board_type = utils.dal_rf_get_board_type
local dal_set_file_owner = utils.dal_set_file_owner
local dal_set_file_mode = utils.dal_set_file_mode
local rf_string_check = utils.rf_string_check
local get_system_name = utils.get_system_name
local IsMgmtBoard = utils.IsMgmtBoard
local GVariant=c.GVariant
local guint16 = GVariant.new_uint16
local gint32 = GVariant.new_int32
local gstring = GVariant.new_string
local guint32 = GVariant.new_uint32
local reply_bad_request = http.reply_bad_request
local IsPangeaPacificSeries = utils.IsPangeaPacificSeries
local INVALID_DATA_DWORD = cfg.INVALID_DATA_DWORD
local DISABLE = cfg.DISABLE
local FRU_DEFS = cfg.FRU_DEFS
local BOARD_SWITCH = cfg.BOARD_SWITCH
local OUTTER_GROUP_TYPE = cfg.OUTTER_GROUP_TYPE
local COMPONENT_TYPE_FAN = cfg.COMPONENT_DEVICE_TYPE_E.COMPONENT_TYPE_FAN
local SENSOR_CAP_READABLE_SETTABLE = cfg.SENSOR_CAP_READABLE_SETTABLE
local SENSOR_CAP_READABLE = cfg.SENSOR_CAP_READABLE
local safe_call = http.safe_call
local reply_ok_encode_json = http.reply_ok_encode_json
local dal_check_string_is_ascii = utils.dal_check_string_is_ascii
local FN_TYPE_OUTTER_OM = cfg.FN_TYPE_OUTTER_OM

local EVENT_WORK_STATE_S = c.EVENT_WORK_STATE_S
local SENSOR_READINGMASK_BIT = c.SENSOR_READINGMASK_BIT
local SENSOR_CAPABILITY_S = c.SENSOR_CAPABILITY_S

local MD_EBUSY = -7999993
local MD_EONUPGRADE = -7999000
local MD_ENOSPACE = -7998999
local FM_ERROR_NO_RECOVER_POINT = -3999977
local BLADE_TYPE_SWI = 0xC0
local SENSOR_TYPE_FAN = 0x04
-- 节点模式
local NODE_MODE_E = {NODE_SLAVE = 0, NODE_MASTER = 1, NODE_MAX = 2}
-- 硬分区配置情况
local PARTITION_E = {
    PARTITION_4P = 0,
    PARTITION_8P = 1,
    PARTITION_CENTRALIZE = 2, -- 16P, 32P情况
    PARTITION_MAX = 3
};

local PEM_EVENT_CODE = 0x03000000
local FANTRAY_EVENT_CODE = 0x04000000

local SMM_ACTIVE_DUMP_FAILED = -3
local SMM_STDBY_DUMP_FAILED = -2

local factoryErrMap = {
    [FM_ERROR_NO_RECOVER_POINT] = http.reply_bad_request('NoRestorePoint')
}

local dumpErrMap = {
    [MD_EBUSY] = http.reply_bad_request('DumpExportingErr'),
    [MD_EONUPGRADE] = http.reply_bad_request('UpgradingErr'),
    [MD_ENOSPACE] = http.reply_bad_request('InsufficientMemoryErr'),
}

local function get_GUID()
    return O.BMC.DeviceGuid:next(function(guid)
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
    end)
end

-- 获取交换平面电源状态
local function get_switch_plane_powerstatus()
    for i = FRU_DEFS.BASE_FRU_ID, FRU_DEFS.FC_FRU_ID + 1 do
        if C.FruPayload('FruID', i):next(function(obj)
            return obj.PowerState:eq(1)
        end):fetch_or(false) then
            return true
        end
    end
end

-- 获取系统上电状态
local function get_system_powerstate()
    local state = 'Off'
    dal_rf_get_board_type(O.PRODUCT):next(function(board_type)
        if board_type == BOARD_SWITCH then
            if get_switch_plane_powerstatus() then
                state = 'On'
            else
                state = 'Off'
            end
        else
            if C.Payload[0].ChassisPowerState:fetch() == 1 then
                state = 'On'
            else
                state = 'Off'
            end
        end
    end)
    return state
end

local function get_processor_summary()
    return O.PMEServiceConfig.X86Enable:neq(DISABLE):next(
        function()
            local count = 0
            local max_count = 0
            local cpu_health_code = 0
            C.Cpu():fold(function(obj)
                obj.Id:neq(0):next(function()
                    return obj.Presence:neq(0)
                end):next(function()
                    if obj.Invisible:fetch_or(0) == 0 then
                        count = count + 1
                    end
                end):catch()
                if obj.Invisible:fetch_or(0) == 0 then
                    max_count = max_count + 1
                end
            end):catch()

            C.Component():fold(function(obj)
                obj.DeviceType:eq(0):next(function()
                    obj.Health:next(function(Health)
                        if Health > cpu_health_code then
                            cpu_health_code = Health
                        end
                    end)
                end)
            end):fetch_or()

            return {
                MaxNum = max_count,
                CurrentNum = count,
                UseagePercent = C.MeInfo[0].CpuUtilise:neq(0xffff):fetch_or(null),
                ThresholdPercent = C.MeInfo[0].CpuUtiliseThre:fetch_or(null),
                CpuHealthCode = cpu_health_code
            }
        end):fetch_or(null)
end

local function get_memory_summary()
    return O.PMEServiceConfig.X86Enable:neq(DISABLE):next(
        function()
            local count = 0
            local max_count = 0
            local capacity = 0
            local mem_health_code = 0
            C.Memory():fold(function(obj)
                obj.Presence:neq(0):next(function()
                    count = count + 1
                    local tmp_cap = obj.Capacity:fetch()
                    if string.match(tmp_cap, 'MB') or string.match(tmp_cap, 'mb') then
                        capacity = capacity + (string.match(tmp_cap, '%d+') / 1024)
                    elseif string.match(tmp_cap, 'GB') or string.match(tmp_cap, 'gb') then
                        capacity = capacity + string.match(tmp_cap, '%d+')
                    end
                end):catch()
                max_count = max_count + 1
            end):catch()

            C.Component():fold(function(obj)
                obj.DeviceType:eq(1):next(function()
                    obj.Health:next(function(Health)
                        if Health > mem_health_code then
                            mem_health_code = Health
                        end
                    end)
                end)
            end):fetch_or()

            return {
                MaxNum = max_count,
                CurrentNum = count,
                UseagePercent = C.MeInfo[0].MemUtilise:neq(0xffff):fetch_or(null),
                ThresholdPercent = C.MeInfo[0].MemUtiliseThre:fetch_or(null),
                CapacityGiB = capacity,
                MemHealthCode = mem_health_code
            }
        end):fetch_or(null)
end

local function GetSystemBatterySummary()        
    local system_name = get_system_name()
    if system_name ~= 'RMM' then
        return nil
    end
    return {
        PresentBatteryCount = O.sysbattery.BatteryPresentCounts:fetch_or(null),
        TotalBatteryCount = O.ProductComponent.BBUModuleNum:fetch_or(null)
    }
end

local function get_power_summary()
    local PowerConsumedWatts = C.SysPower():fold(function(obj, acc)
        return acc + obj.Power:fetch_or(0)
    end, 0):catch()

    local UpperPowerThresholdWatts = C.SysPower[0].HighPowerThresholdWatts:neq(INVALID_DATA_DWORD):catch()

    -- 获取电源相关属性
    local PowerCapacityWatts = 0
    local PresentPSUNum = 0
    local PSU_health_code = 0
    O.ProductComponent.PsNum:next(function(psu_num)
        return psu_num == 0 and c.get_object_count('OnePower') or psu_num
    end):next(function(psu_num)
        for i = 1, psu_num do
            if IsPangeaPacificSeries() then
                i = i - 1
            end
            local power_name = string.format('PSU%d', i)
            C.OnePower('DeviceName', power_name):next(function(obj)
                PowerCapacityWatts = PowerCapacityWatts + obj.PsRate:fetch_or(0)
                if obj.Presence:eq(1):fetch_or(nil) then
                    PresentPSUNum = PresentPSUNum + 1
                end
            end)
        end
    end):catch()

    local max_count = 0
    C.ProductComponent[0].PsNum:next(function(power_num)
        max_count = power_num
    end):catch()

    C.MsmNode[0]:next(function(obj)
        PowerCapacityWatts = C.SysPower[0].HighPowerThresholdWatts:neq(INVALID_DATA_DWORD):fetch()
    end):fetch_or(0)
    
    C.Component():fold(function(obj)
        obj.DeviceType:eq(3):next(function()
            obj.Health:next(function(Health)
                if Health > PSU_health_code then
                    PSU_health_code = Health
                end
            end)
        end)
    end):fetch_or()

    return {
        MaxNum = max_count,
        CurrentNum = PresentPSUNum,
        PowerConsumedWatts = PowerConsumedWatts:fetch(),
        UpperPowerThresholdWatts = UpperPowerThresholdWatts:fetch(),
        PowerCapacityWatts = PowerCapacityWatts,
        PSUHealthCode = PSU_health_code
    }
end

local function get_fan_summary()
    local count = 0
    local max_count = 0
    local fan_health_code = 0
    O.ProductComponent.FanNum:neq(0):next(function(fan_num)
        max_count = fan_num
        C.Component():fold(function(obj)
            obj.DeviceType:eq(COMPONENT_TYPE_FAN):next(function(Health)
                obj.Presence:eq(1):next(function()
                    count = count + 1
                end)
                obj.Health:next(function(Health)
                    if Health > fan_health_code then
                        fan_health_code = Health
                    end
                end)
            end)
        end):fetch_or()
    end)

    if count > 0 then
        count = count - 1
    end

    return {MaxNum = max_count, 
            CurrentNum = count,
            FanHealthCode = fan_health_code
}
end

local function get_sensor_threshold(obj, property_name, readable, sensor_cap)
    if not (sensor_cap.threshold_access_support == SENSOR_CAP_READABLE_SETTABLE or
        sensor_cap.threshold_access_support == SENSOR_CAP_READABLE) or readable == 0 then
        return null
    end
    return obj[property_name]:fetch_or(nil)
end

-- 获取进风口温度及传感器门限信息
local function get_thermal_summary()
    return C.ThresholdSensor('InnerName', 'Inlet_TEMP'):next(
        function(obj)
            local InletTemperature = obj.ReadingStatus:next(
                function(reading_status)
                    return EVENT_WORK_STATE_S(reading_status)
                end):next(function(sensor_reading_status)
                if sensor_reading_status.disable_scanning == 1 and sensor_reading_status.disable_scanning_local == 1 and
                    sensor_reading_status.disable_all == 1 and sensor_reading_status.initial_update_progress == 1 and
                    sensor_reading_status.disable_access_error == 1 then
                    return string.format('%.2f', obj.ReaddingConvert:fetch_or(0))
                else
                    return null
                end
            end)
            local mask = obj.DiscreteReadingMask:fetch_or(0)
            local reading_mask = SENSOR_READINGMASK_BIT(mask)
            local capabilities = obj.Capabilities:fetch_or(0)
            local sensor_cap = SENSOR_CAPABILITY_S(capabilities)
            return {
                InletTemperature = InletTemperature:fetch_or(nil),
                InletTempMinorAlarmThreshold = get_sensor_threshold(obj, 'UpperNoncriticalConvert',
                    reading_mask.readable_upper_noncritical, sensor_cap),
                InletTempMajorAlarmThreshold = get_sensor_threshold(obj, 'UpperCriticalConvert',
                    reading_mask.readable_upper_critical, sensor_cap)
            }
        end):fetch_or(null)
end

local function get_netadapter_summary()
    local count = 0
    local netadapter_health_code = 0
    local NIC_health_code = 0
    C.NetCard():fold(function(obj)
        obj.VirtualNetCardFlag:neq(1):next(function()
            count = count + 1
        end)
        obj.Health:next(function(Health)
            if Health > netadapter_health_code then
                netadapter_health_code = Health
            end
        end)
    end):fetch_or()

    C.Component():fold(function(obj)
        obj.DeviceType:eq(13):next(function()
            obj.Health:next(function(Health)
                if Health > NIC_health_code then
                    NIC_health_code = Health
                end
            end)
        end)
    end)

    if NIC_health_code > netadapter_health_code then
        netadapter_health_code = NIC_health_code
    end

    return {MaxNum = count, 
            CurrentNum = count,
            NetadapterHealthCode = netadapter_health_code
}
end

local COMPONENT_TYPE_SD_CARD = 33
local function get_storage_summary()
    local raid_count = 0
    local logical_count = 0
    local disk_count = 0
    local used_capacity = 0
    local total_capacity = 0
    local useage = null
    local hdd_health_code = 0
    local pciessd_health_code = 0
    C.Hdd():fold(function(obj)
        obj.Presence:eq(1):next(function()
            disk_count = disk_count + 1
        end)
    end)

    C.PCIeDisk():fold(function(obj)
        obj.Presence:eq(1):next(function()
            disk_count = disk_count + 1
        end)
        obj.HddHealth:next(function(HddHealth)
            if HddHealth > pciessd_health_code then
                pciessd_health_code = HddHealth
            end
        end)
    end)

    C.Component():fold(function(obj)
        obj.DeviceType:eq(COMPONENT_TYPE_SD_CARD):next(function()
            obj.Presence:eq(1):next(function()
                disk_count = disk_count + 1
            end)
        end)
    end)

    C.Component():fold(function(obj)
        obj.DeviceType:eq(2):next(function()
            obj.Health:next(function(Health)
                if Health > hdd_health_code then
                    hdd_health_code = Health
                end
            end)
        end)
    end):fetch_or()

    C.LogicalDrive():fold(function(_)
        logical_count = logical_count + 1
    end)

    C.RaidController():fold(function(obj)
        obj.Id:neq(0xFF):next(function()
            raid_count = raid_count + 1
        end)
    end)

    C.OSDrivePartition():fold(function(obj)
        obj.UsedCapacityGiB:neq(0xFFFF):next(function(used)
            used_capacity = used_capacity + used
        end)
        obj.TotalCapacityGiB:neq(0xFFFF):next(function(total)
            total_capacity = total_capacity + total
        end)
    end)

    if total_capacity ~= 0 then
        useage = math.ceil(100 * used_capacity / total_capacity)
    end

    if pciessd_health_code > hdd_health_code then
        hdd_health_code = pciessd_health_code
    end

    return {
        RaidControllerNum = raid_count,
        LogicalDriveNum = logical_count,
        PhysicalDriveNum = disk_count,
        DiskUsagePercent = useage,
        DiskThresholdPercent = C.MeInfo[0].DiskPartitionUsageThre:fetch_or(null),
        DiskHealthCode = hdd_health_code
    }
end

local function GetChassisDeviceTypeSummary()
    local system_name = get_system_name()
    if system_name ~= 'RMM' then
        return nil
    end

    local server_count = 0
    local storage_count = 0
    local network_count = 0 
    local other_count = 0

    C.UnitInfo():fold(function(obj)
        obj.Detected:neq(0):next(function()
            obj.DeviceType:next(function(devicetype)
                if devicetype == 'Server' then
                    server_count = server_count + 1
                elseif devicetype == 'Storage' then 
                    storage_count = storage_count + 1
                elseif devicetype == 'Network' then 
                    network_count = network_count + 1
                else
                    other_count = other_count + 1
                end      
            end)
        end)
    end)

    return {
        ServerCount = server_count,
        StorageCount = storage_count,
        NetworkCount = network_count,
        OtherCount = other_count
    }
end

local function dal_get_byte_array_with_separator(obj, propertyName, separator)
    if not obj then
        return
    end
    return obj[propertyName]:next(function(propByteArray)
        -- 当只有一个元素且为0时，说明为框架初默认始化的值，不做转换输出
        if #propByteArray == 1 and propByteArray[1] == 0 then
            return
        end
        local byteArrayPart = ''
        for i = 1, #propByteArray do
            if i ~= 1 then
                byteArrayPart = byteArrayPart .. format('%s%.2X', separator, propByteArray[i])
            else
                byteArrayPart = byteArrayPart .. format('%.2X', propByteArray[i])
            end
        end
        return byteArrayPart
    end):fetch()
end

local function get_chassis_oem_rfid_uid(obj)
    local strRfidUid = dal_get_byte_array_with_separator(obj, 'RFIDTagUID', ':')
    return (strRfidUid and #strRfidUid ~= 0 and strRfidUid ~= 'N/A') and strRfidUid or null
end

local function get_time(time)
    local now = os.time()
    local date = os.date("*t", now)
    local time_value = 0
    -- 支持夏令时时区显示
    local time_zone = os.difftime(now + (date.isdst and 3600 or 0), os.time(os.date('!*t', now))) / 60

    if time == 0 or time == null then
        time_value = os.time()
    else
        time_value = time
    end
    if time_zone < 0 then
        return os.date('%Y-%m-%d %X (UTC', time_value) ..
                   string.format('-%.2d:%.2d', -time_zone / 60, -time_zone % 60) .. ')'
    else
        return os.date('%Y-%m-%d %X (UTC', time_value) ..
                   string.format('+%.2d:%.2d', time_zone / 60, time_zone % 60) .. ')'
    end
end

local function GetDiscoveredTime(obj)
    local discoveredtime = obj.DiscoveredTime:fetch_or(null)
    if discoveredtime == 0 or discoveredtime == null then
        return null
    else
        return get_time(discoveredtime)
    end 
end

local function GetExtendField(obj)
    local str = obj.ExtendField:next(rf_string_check):fetch_or(null)
    if str == null then
        return str
    else
        return json_decode(str)
    end
    
end   

local function GetComuniteSummary()
    local system_name = get_system_name()
    if system_name ~= 'RMM' then
        return nil
    end
    return C.UnitInfo():fold(function(obj, acc)
        local topuslot = obj.UNum:fetch_or(null)
        local id = 'U' .. topuslot
        acc[#acc + 1] = {
            RefComponent = obj.RefComponent:fetch_or(null),
            name = 'Chassis Asset Info',
            ChassisType = null,
            WeightKg = obj.WeightKg:neq(0):fetch_or(null),
            Manufacturer = obj.Manufacturer:neq(''):fetch_or(null),
            SerialNumber = obj.SerialNumber:neq(''):fetch_or(null),
            PartNumber = obj.PartNumber:neq(''):fetch_or(null),
            Model = obj.Model:neq(''):fetch_or(null),
            IndicatorLED = obj.IndicatorLED:neq(''):fetch_or(null),
            state = obj.Detected:next(function(UnitState)
                if UnitState == 0 then
                    return 'Absent'
                else
                    return 'Enabled'
                end
            end):fetch_or(),
            health = obj.WarnStatus:next(function(UnitWarnStatus)
                if UnitWarnStatus == 0 then
                    return 'OK'
                else
                    return 'Warning'
                end
            end):fetch_or(),
            TopUSlot = topuslot,
            IndicatorColor = obj.IndicatorColor:neq(''):fetch_or(null),
            UHeight = obj.UHeight:neq(0):fetch_or(null),
            AvailableRackSpaceU = obj.AvailableRackSpaceU:neq(0):fetch_or(null),
            DeviceType = obj.DeviceType:neq(''):fetch_or(null),
            RatedPowerWatts = obj.RatedPowerWatts:neq(0):fetch_or(null),
            CheckInTime = obj.CheckInTime:neq(''):fetch_or(null),
            AssetOwner = obj.AssetOwner:neq(''):fetch_or(null),
            LifeCycleYear = obj.LifeCycleYear:neq(0):fetch_or(null),
            RFIDTagUID = get_chassis_oem_rfid_uid(obj),
            RWCapability = obj.RWCapability:next(function(rwcapability)
                if rwcapability == 0 then
                    return 'ReadWrite'
                else
                    return 'ReadOnly'
                end
            end):fetch_or(),
            ExtendField = GetExtendField(obj),
            Id = id,
            DiscoveredTime = GetDiscoveredTime(obj)
        }
        return acc
    end, {}):fetch_or()
end

local function GetRackSummary()
    local RackDirection = null
    local RackDirectionType = null
    local RackUnitOccupyDirection = null
    local RackAssetTag = null
    local system_name = get_system_name()
    if system_name ~= 'RMM' then
        return nil
    end
    O.RackAssetMgmt.Direction:next(function(direction)
        if direction == 0 then
            RackDirection = 'FromBottomUp'
        elseif direction == 1 then
            RackDirection = 'FromTopDown'
        end
    end)

    O.RackAssetMgmt.DirectionType:next(function(directiontype)
        if directiontype == 0 then
            RackDirectionType = 'Changeable'
        elseif directiontype == 1 then
            RackDirectionType = 'Fixed'
        end
    end)

    O.RackAssetMgmt.UnitOccupyDirection:next(function(unitoccupydirection)
        if unitoccupydirection == 0 then
            RackUnitOccupyDirection = 'Downward'
        elseif unitoccupydirection == 1 then
            RackUnitOccupyDirection = 'Upward'
        end
    end)
    
    C.Elabel():fold(function(obj)
        obj.SysName:eq('RM210'):next(function()
            RackAssetTag = obj.ProductAssetTag:fetch_or(null)
        end)
    end)

    return {
        Direction = RackDirection,
        DirectionType = RackDirectionType,
        UnitOccupyDirection = RackUnitOccupyDirection,
        AssetTag = RackAssetTag
    }
end

local function get_led_state()
    local state_str = null
    O.UIDLed.State:next(function(state)
        if state == 0 then
            state_str = 'Off'
        elseif state == 255 then
            state_str = 'Lit'
        else
            state_str = 'Blinking'
        end
    end)
    return state_str
end
-----------------------------------------------------------
-- export
-----------------------------------------------------------

local function GetFanSupported()
    local fansFlag = false
    local inletTemperatureFlag = false
    local fanCount = 0

    local fanObjs = C.FANClass():fold(function(obj, acc)
        acc[obj.FANID:fetch()] = obj
        fanCount = fanCount + 1
        return acc
    end, {}):fetch_or()

    local fanSensors = C.ThresholdSensor():fold(function(obj, acc)
        if obj.SensorType:fetch() == SENSOR_TYPE_FAN then
            local InnerName = obj.InnerName:fetch()
            local fanId, FOrR = InnerName:match("^FAN(%d)_([^_]+)")
            if fanId then
                acc[#acc + 1] = {obj = obj, FOrR = FOrR, FanId = tonumber(fanId)}
            end
        end
        return acc
    end, {}):fetch_or()

    for _, fanSensor in ipairs(fanSensors) do
        local fanObj = fanObjs[fanSensor.FanId]
        if fanObj then
            fansFlag = true
            break
        end
    end

    C.ThresholdSensor("InnerName", "Inlet_TEMP"):next(function(obj)
        return obj.ReadingStatus:next(function(reading_status)
            return EVENT_WORK_STATE_S(reading_status)
        end):next(function(sensor_reading_status)
            if sensor_reading_status.disable_scanning == 1 and sensor_reading_status.disable_scanning_local == 1 and
                sensor_reading_status.disable_all == 1 and sensor_reading_status.initial_update_progress == 1 and
                sensor_reading_status.disable_access_error == 1 then
                inletTemperatureFlag = true
            end
        end)
    end):fetch_or(null)

    local customConfigFlag = O.Cooling.SmartCoolingEnable:fetch_or(0) == 1
    return fansFlag or inletTemperatureFlag or customConfigFlag
end

local function get_smm_health_level()
    local code_mask = 0xFF000000
    local max_level = 0
    O.Warning:next(function(obj)
        local record_id = 0
        while record_id ~= 0xFFFF do
            local ok, ret = call_method(nil, obj, 'GetEventItem', nil, {guint16(record_id)})
            if not ok then
                return ret
            end
            record_id = ret[1]
            local event_code = ret[6]
            if bit.band(event_code, code_mask) == PEM_EVENT_CODE or bit.band(event_code, code_mask) ==
                FANTRAY_EVENT_CODE then
                goto continue
            end
            local event_level = ret[5]
            if event_level > 0 and event_level <= 3 and event_level > max_level then
                max_level = event_level
            end
            ::continue::
        end
    end)
    return utils.get_property_and_severity(max_level)
end

local function get_blade_health_level(ipmb_addr)
    local max_level = 0
    O.ShelfManage:next(function(obj)
        local ok, ret = call_method(nil, obj, 'GetBladeAlarmEventMsgList', nil, {gbyte(ipmb_addr)})
        if not ok then
            return ret
        end
        for i = 1, #ret, 2 do
            if ret[i][18] > 0 and ret[i][18] <= 3 and ret[i][18] > max_level then
                max_level = ret[i][18]
            end
        end
    end)
    return utils.get_property_and_severity(max_level)
end

local function convert_width_height(size)
    local width, height
    local board_size = {
        [1] = function()
            width = 1
            height = 1
        end,
        [2] = function()
            width = 1
            height = 2
        end,
        [4] = function()
            width = 2
            height = 2
        end,
    }
    if board_size[size] then
        board_size[size]()
    end
    return width, height
end

local COMPONENT_TYPE_BACKPLANE_BOARD = 22
local SMM_BACK_PLANE_POSITION = 1

local function get_chassis_summary()
    local elabel_obj = C.Fru('Position', SMM_BACK_PLANE_POSITION):next(function(obj)
        return obj:ref_obj('Elabel'):fetch()
    end):fetch()
    local chass_obj = O.Chassis
    local chassis = {
        ChassisName = chass_obj.ChassisName:fetch_or(null),
        ChassisLocation = chass_obj.Location:fetch_or(null),
        ChassisID = chass_obj.ChassisNum:fetch_or(null),
        Model = chass_obj.CustomData1:fetch_or(null),
        ProductName = elabel_obj.BoardProductName:fetch_or(null),
        BackPlanePcbVersion = C.Fru('Type', COMPONENT_TYPE_BACKPLANE_BOARD).PcbVersion:fetch_or(null),
        Type = elabel_obj.ChassisType:fetch_or(null),
        SerialNumber = elabel_obj.ChassisSerialNumber:fetch_or(null),
        PartNumber = elabel_obj.ChassisPartNumber:fetch_or(null),
    }
    return chassis
end

local function get_chassis_component_summary()
    local component = C.IpmbEthBlade():fold(function(obj, blade)
        if obj.Presence:fetch() ~= 1 then
            return blade
        end
        local blade_type = obj.BladeType:fetch()
        local slave_addr = obj.SlaveAddr:fetch()
        local health_level
        local id = dflib.object_name(obj)
        if blade_type == 0x03 then
            if slave_addr == O.SMM.IpmbAddr:fetch() then
                health_level = get_smm_health_level()
            else
                health_level = get_blade_health_level(slave_addr)
            end
            id = 'H'..string.sub(id, 2)
        else
            health_level = get_blade_health_level(slave_addr)
            id = string.upper(string.sub(id, 1, 1))..string.lower(string.sub(id, 2))
        end
        local width, height = convert_width_height(obj.BoardWidthType:fetch())
        blade[#blade + 1] = {
            Id = id,
            HotswapState = obj.HotSwap:fetch(),
            Health = health_level,
            ManagedByMM = obj.ManagedByMM:next(function(state)
                if state == 0 then
                    return nil
                end
                return utils.ValueToBool(state - 1)
            end):fetch(),
            Width = width,
            Height = height,
        }
        return blade
    end, {}):fetch()
    return component
end

local function get_chassis_power_summary()
    local power_supply = C.PowerMgnt():fold(function(power_obj, psu_list)
        if power_obj.Presence:fetch() == 0 then
            return psu_list
        end
        psu_list[#psu_list + 1] = {
            Name = 'PSU' .. (power_obj.Slot:fetch() + 1),
            Health = C.OnePower('PsId', power_obj.Slot:fetch()):next(function(obj)
                return utils.get_property_and_severity(obj.Health:fetch())
            end):fetch(),
        }
        return psu_list
    end, {}):fetch()

    local power = {
        PowerConsumedWatts = O.shelfPowerCapping.CurrentPower:fetch_or(null),
        PowerCapacityWatts = C.SysPower().MaxCapValue:fetch_or(null),
        SupplyList = power_supply,
    }
    return power
end

local function get_chassis_fan_summary()
    local fan = C.SMMFANClass():fold(function(obj, fan_info)
        if obj.Presence:fetch() ~= 1 then
            return fan_info
        end
        local fan_slot_id = obj.FANSlot:fetch()
        fan_info[#fan_info + 1] = {
            Name = 'Fan' .. fan_slot_id,
            Health = utils.get_property_and_severity(obj.FanHealth:fetch()),
            Speed = C.Cooling():next(function(cooling_obj)
                local ok, ret = call_method(nil, cooling_obj, 'GetFanState', nil, {gint32(fan_slot_id)})
                if not ok then
                    return ret
                end
                if ret[2] == 0xff then
                    return null
                end
                return ret[2]
            end):fetch(),
        }
        return fan_info
    end, {}):fetch()
    return fan
end


local M = {}

function M.GetGenericInfo(user)
    local customed_cert_flag = false
    local ok, ret = call_method(user, C.SSL[0]:next():fetch(), 'GetCustomedCertFlag', function(errId)
        if errId == -1 then
            return http.reply_bad_request()
        end
    end)
    if ok then
        customed_cert_flag = ret[1]
    end
    local tpcm_supported_state = false
    local ok, ret = call_method(user, O.TPCMBaseInfo, "GetTpcmSupportState", nil, nil)
    if ok and ret[1] == 1 then
        tpcm_supported_state = true
    end
    return {
        HealthSummary = {
            NumberOfCriticalAlarm = O.Warning.CriticalAlarmNum:fetch(),
            NumberOfMajorAlarm = O.Warning.MajorAlarmNum:fetch(),
            NumberOfMinorAlarm = O.Warning.MinorAlarmNum:fetch()
        },
        PowerState = get_system_powerstate(),
        IndicatorLEDState = get_led_state(),
        Copyright = O.Contact.Copyright:fetch(),
        CurrentTime = get_time(0),
        LanguageSet = utils.split(utils.GetLanguageSet(true), ','),
        SystemLockDownEnabled = O.PMEServiceConfig.SystemLockDownSupport:eq(1):next(
            function()
                if O.SecurityEnhance.SystemLockDownStatus:fetch_or(0) == 1 then
                    return true
                else
                    return false
                end
            end):fetch_or(nil),
        -- FDMSupported 控制web维护诊断中的FDM PFAE页签是否显示
        FDMSupported = C.FdmConfig[0].FdmEnable:neq(0):next(
            function()
                return C.FdmWebConfig[0].FdmPfaeEnable:fetch_or(0) == 1 and
                    C.FdmWebConfig[0].FdmPfaeResultDisplay:fetch_or(0) == 1
            end):fetch_or(false),
        LicSerSupported = C.LicenseManage[0]:next(function()
            return true
        end):fetch_or(false),
        SmsSupported = C.SMS[0].SmsEnabled:next(utils.ValueToBool):fetch_or(false),
        SnmpSupported = C.PMEServiceConfig[0].SnmpdEnable:next(utils.ValueToBool):fetch_or(false),
        FusionPartSupported = O.xmlPartition.NodeMode:next(
            function(mode)
                local fusionPartition = O.xmlPartition.XmlPartition:fetch_or()
                if mode == NODE_MODE_E.NODE_MASTER and
                    (fusionPartition == PARTITION_E.PARTITION_4P or fusionPartition == PARTITION_E.PARTITION_8P) then
                    return true
                else
                    return false
                end
            end):fetch_or(false),
        FanSupported = GetFanSupported(),
        ArmSupported = C.PMEServiceConfig[0].ARMEnable:next(utils.ValueToBool):fetch_or(false),
        StorageConfigReady = C.PRODUCT[0].StorageConfigReady:fetch(),
        SPSupported = C.PMEServiceConfig[0].VirtualUmsEnable:next(utils.ValueToBool):fetch_or(false),
        USBSupported = C.USBMgmt[0]:next(function(obj)
            if obj.Support:fetch() ~= 1 or obj.Presence:fetch() ~= 1 then
                return false
            end
            return true
        end):fetch_or(false),
        iBMARunningStatus = O.sms0.Status:neq("N/A"):fetch_or(),
        CustomizedId = O.RedfishEventService.EventDetailCustomizedID:fetch_or(),
        CustomeCertFlag = customed_cert_flag,
        KVMSupported = C.PMEServiceConfig[0].KVMEnable:next(utils.ValueToBool):fetch_or(false),
        VMMSupported = C.PMEServiceConfig[0].KVMEnable:next(utils.ValueToBool):fetch_or(false),
        VNCSupported = C.PMEServiceConfig[0].VNCEnable:next(utils.ValueToBool):fetch_or(false),
        PerfSupported = C.PMEServiceConfig[0].PerfMonEnable:next(utils.ValueToBool):fetch_or(false),
        PowerDeepSleepSupported = C.PMEServiceConfig[0].PowerDeepSleepEnable:next(utils.ValueToBool):fetch_or(false),
        PowerCappingSupport = C.PMEServiceConfig[0].PowerCappingSupport:next(utils.ValueToBool):fetch_or(false),
        SwiSupported = C.IpmbEthBlade("BladeType", BLADE_TYPE_SWI):next(function()
            return true
        end):fetch_or(false),
        TPCMSupported = tpcm_supported_state,
        VideoScreenshotSupported = C.PMEServiceConfig[0].JavaVideoEnable:next(utils.ValueToBool):fetch_or(false)
    }
end

local LedStateMap = {Off = {0, 0}, Lit = {0xFF, 0}, Blinking = {0xFF, 0xFF}}

function M.UpdateGenericInfo(data, user)
    local info = LedStateMap[data.IndicatorLEDState]
    if info == nil then
        return http.reply_bad_request('ActionParameterMissing')
    end

    local ok, ret = call_method(user, O.UIDLed, 'SetUIDLedIdentify', nil, gbyte(info[1]), gbyte(info[2]))
    if not ok then
        return ret
    end
    return reply_ok()
end

function M.GetOverview()
    local rackinfo = {}
    local system_name = get_system_name()
    local EthGroup = O.EthGroup0
    local OutGroupId = C.EthFunction('Functiontype', FN_TYPE_OUTTER_OM).Plane1GroupId:fetch_or(nil)
    if OutGroupId ~= nil then
        EthGroup =  C.EthGroup('GroupID', OutGroupId)
    end

    local deviceinfo = {
        ProductName = O.BMC.SystemName:fetch_or(null),
        ProductAlias = O.PRODUCT.ProductAlias:fetch_or(null),
        TEEOSVersion = O.Bios.TeeosVersion:fetch_or(null),
        ProductSN = O.Fru0Elabel.ProductSerialNumber:fetch_or(null),
        SystemSN = O.BMC.DeviceSerialNumber:fetch_or(null),
        HostName = O.BMC.HostName:next(dal_trim_string):fetch_or(null),
        DomainName = O.DNSSetting.DomainName:next(dal_trim_string):fetch_or(null),
        BMCVersion = O.BMC.PMEVer:fetch_or(null),
        BIOSVersion = O.ProductComponent:next(function(obj)
            if obj.HostNum:fetch_or(0) <= 1 then
                return O.Bios.Version:next(rf_string_check):neq('000'):fetch_or(null)
            end
            return nil
        end):fetch_or(nil),
        GUID = get_GUID():fetch_or(null),
        MAC = C.EthGroup('OutType', OUTTER_GROUP_TYPE).Mac:fetch_or(null),
            DeviceIPv4 = EthGroup.IpAddr:fetch_or(null),
            DeviceIPv6 = EthGroup.Ip6Addr:fetch_or(null),
            DefaultIPAddress = EthGroup:next(function(obj)
            if obj.FactoryDefaultIpMode:fetch_or() == 1 and obj.FactoryDefaultIpAddr:fetch_or() then
                return {Mode = 'Static', Address = obj.FactoryDefaultIpAddr:fetch()}
            elseif obj.FactoryDefaultIpMode:fetch_or() == 2 then
                return {Mode = 'DHCP', Address = null}
            end
            if obj['d:IpMode']:fetch() == 0 or obj['d:IpMode']:fetch() == 1 then
                return {Mode = 'Static', Address = obj['d:IpAddr']:fetch(null)}
            else
                return {Mode = 'DHCP', Address = null}
            end
        end):fetch_or(null)
    }

    local summary = {
        Processor = get_processor_summary(),
        Memory = get_memory_summary(),
        Storage = get_storage_summary(),
        Power = get_power_summary(),
        Fan = get_fan_summary(),
        Thermal = get_thermal_summary(),
        Netadapter = get_netadapter_summary(),
        SysBattery = GetSystemBatterySummary(),
        DeviceStatistics = GetChassisDeviceTypeSummary(),
        Comunite = GetComuniteSummary(),
        RackInformation = GetRackSummary()
    }

    local vagusbdvdenabled = O.Partition.CurrentVGASwitch:next(utils.ValueToBool):fetch_or()   
    local fusionpartition = O.xmlPartition.XmlPartition:next(
        function(partition)
            if partition == 0 then
                return 'DualSystem'
            else
                return 'SingleSystem'
            end
        end):fetch_or(nil)
    
    local remoteipv4address = O.Partition.RemoteExternIp:fetch_or(nil)
    local kvmclenturl = O.Contact.DownloadKVMLink:fetch_or(null)
    local productimage = O.PRODUCT.ProductPic:fetch_or('')
    if system_name == 'RMM' then
        rackinfo = {
            LoadCapacityKg = O.RackAssetMgmt.LoadCapacityKg:neq(0):fetch_or(null),
            Country = O.RackAssetMgmt.Country:next(rf_string_check):fetch_or(null),
            Territory = O.RackAssetMgmt.Territory:next(rf_string_check):fetch_or(null),
            City = O.RackAssetMgmt.City:next(rf_string_check):fetch_or(null),
            Street = O.RackAssetMgmt.Street:next(rf_string_check):fetch_or(null),
            HouseNumber = O.RackAssetMgmt.HouseNumber:next(rf_string_check):fetch_or(null),
            Name = O.RackAssetMgmt.Name:next(rf_string_check):fetch_or(null),
            PostCode = O.RackAssetMgmt.PostCode:next(rf_string_check):fetch_or(null),
            Building = O.RackAssetMgmt.Building:next(rf_string_check):fetch_or(null),
            Floor = O.RackAssetMgmt.Floor:next(rf_string_check):fetch_or(null),
            Room = O.RackAssetMgmt.Room:next(rf_string_check):fetch_or(null),
            LocationInfo = O.RackAssetMgmt.LocationInfo:next(rf_string_check):fetch_or(null),
            Row = O.RackAssetMgmt.Row:next(rf_string_check):fetch_or(null),
            Rack = O.RackAssetMgmt.Rack:next(rf_string_check):fetch_or(null),
            TotalUCount = O.RackAssetMgmt.UCount:fetch_or(null),
            UcountUsed = O.RackAssetMgmt.UCountUsed:fetch_or(null)
        }
    end

    if next(rackinfo) == nil then
        rackinfo = nil
    end

    return {
        DeviceInfo = deviceinfo,
        Summary = summary,
        VGAUSBDVDEnabled = vagusbdvdenabled,
        FusionPartition = fusionpartition,
        RemoteBMCIPv4Address = remoteipv4address,
        KVMClientUrl = kvmclenturl,
        ProductImage = productimage,
        RackInfo = rackinfo
    }
end

function M.GetChassisOverview()
    return {
        Chasis = get_chassis_summary(),
        Component = get_chassis_component_summary(),
        Power = get_chassis_power_summary(),
        Fan = get_chassis_fan_summary()
    }
end
 
function M.UpdateChassisOverview(data, user)
    local result = reply_ok()
    local obj = O.Chassis
    if data.ChassisID then
        result:join(safe_call(function()
            local ok, ret = call_method(user, obj, 'SetChassisNumber', nil, {guint32(data.ChassisID)})
            if not ok then
                return ret
            end
        end))
    end

    if data.ChassisName then
        result:join(safe_call(function()
            local ok, ret = call_method(user, obj, 'SetChassisName', nil, {gstring(data.ChassisName)})
            if not ok then
                return ret
            end
        end))
    end

    if data.ChassisLocation then
        result:join(safe_call(function()
            if not dal_check_string_is_ascii(data.ChassisLocation) then
                return reply_bad_request('PropertyValueFormatError',
                    'The Location is of a different format than the property can accept.')
            end
            local ok, ret = call_method(user, obj, 'SetChassisLocation', nil, {gstring(data.ChassisLocation)})
            if not ok then
                return ret
            end
        end))
    end
    local chassis = {
        ChassisName = O.Chassis.ChassisName:fetch_or(null),
        ChassisLocation = O.Chassis.Location:fetch_or(null),
        ChassisID = O.Chassis.ChassisNum:fetch_or(null)
    }
    if result:isOk() then
        return reply_ok_encode_json(chassis)
    end
    result:appendErrorData(chassis)
    return result
end

function M.RestoreFactory(data, user)
    local reauth = utils.ReAuthUser(user, data.ReauthKey)
    if reauth then
        utils.OperateLog(user,
            'Failed to recover manufacturer default configuration, because the current user password is incorrect or the account is locked')
        return reauth
    end
    local ok, ret = call_method(user, O.log_0, 'AdvanceLoadBak', factoryErrMap)
    if not ok then
        return ret
    end
    return reply_ok()
end

function M.Dump(user)
    return C.Dump[0]:next(function(obj)
    local member_id = 'hmm'
    local system_name = get_system_name()
        local collect_cb = function()
            local ok, ret
            if IsMgmtBoard() and system_name ~= 'RMM' then
                ok, ret = utils.call_method_async(user, obj, 'DumpInfoBothSmmAsync', nil, gbyte(0), gstring(member_id))
            else
                ok, ret = utils.call_method_async(user, obj, 'DumpInfoAsync', nil, gbyte(0))
            end
            if not ok then
                return ret
            end
            return reply_ok()
        end

        local progress_cb = function(file_path)
            local ok, ret
            if IsMgmtBoard() and system_name ~= 'RMM' then    
                ok, ret = call_method(user, obj, 'GetBothSmmDumpPhase', nil, nil)
            else
                ok, ret = call_method(user, obj, 'GetDumpPhase', nil, nil)
            end
            if not ok then
                logging:error('progress_cb GetDumpPhase failed')
                return 0
            end
            if IsMgmtBoard() and system_name ~= 'RMM' then
                if ret[1] == SMM_ACTIVE_DUMP_FAILED or ret[1] == SMM_STDBY_DUMP_FAILED then
                    ret[1] = 100
                end
            end 
            if ret[1] == 100 then
                if not dal_set_file_owner("/tmp/dump_info.tar.gz", cfg.REDFISH_USER_UID, cfg.APPS_USER_GID) then
                    error('chown dump_info.tar.gz failed')
                end
                if not dal_set_file_mode("/tmp/dump_info.tar.gz", cfg.FILE_MODE_600) then
                    error('chmod dump_info.tar.gz failed')
                end
                tools.movefile('/tmp/dump_info.tar.gz', file_path)
            end
            return ret[1]
        end

        return download(user, 'dump.tar.gz', 'dump file', defs.DOWNLOAD_DUMP, collect_cb, progress_cb)
    end):catch(function(err)
        return http.reply_bad_request('ActionNotSupported', err)
    end):fetch()
end

return M
