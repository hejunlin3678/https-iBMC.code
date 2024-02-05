local cjson = require 'cjson'
local dflib = require 'dflib'
local utils = require 'utils'
local c = require 'dflib.core'
local cfg = require 'config'
local http = require 'http'
local O = dflib.object
local C = dflib.class
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local null = cjson.null
local decode = cjson.decode
local ENABLE = cfg.ENABLE
local reply_ok = http.reply_ok
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_bad_request = http.reply_bad_request
local reply_not_found = http.reply_not_found
local reply_internal_server_error = http.reply_internal_server_error
local safe_call = http.safe_call
local ValueToBool = utils.ValueToBool
local ValueToBoolNoDefault = utils.ValueToBoolNoDefault
local call_method = utils.call_method
local rf_string_check = utils.rf_string_check
local dal_get_specific_object_position = utils.dal_get_specific_object_position
local dal_clear_string_blank = utils.dal_clear_string_blank
local tsort = table.sort
local G_OCP_LED_STATE = {[0x1] = 'Removable', [0x2] = 'Insertable', [0x0] = 'Inoperable', [0x3] = 'Unknown'}
local dal_get_specific_object_position = utils.dal_get_specific_object_position
local add_component_uid = utils.add_component_uid

local M = {}

-- PCIe资源ID归属定义
-- PCIe资源归属于其它PCIe设备时, 资源ID的基数, 实际的资源ID=基数 + PCIe设备FRU ID
local RESOURCE_ID_PCIE_DEVICE_BASE = 0x80
-- PCIe资源归属于其它PCIe设备时, 资源ID的最大值, 实际的资源ID=基数 + PCIe设备FRU ID
local RESOURCE_ID_PCIE_DEVICE_MAX = 0xC0

local RF_INFO_INVALID_DWORD = 0xFFFFFFFF
local INVALID_DOUBLE_VALUE = 0xffffffffffffffff

local NET_TYPE_E = {
    NET_TYPE_LOM = 1, -- 板载1网卡，对应eth0，部分网卡支持NCSI
    NET_TYPE_DEDICATED = 2, -- 专用网卡,对应eth2，不支持NCSI
    NET_TYPE_PCIE = 3, -- PCIE网卡,对应eth1，部分网卡支持NCSI
    NET_TYPE_AGGREGATION = 4, -- 聚合网卡，对应eth3，不支持NCSI
    NET_TYPE_LOM2 = 5, -- 板载2网卡,V5专用，对应eth1 ，部分网卡支持NCSI
    NET_TYPE_INNER_DEDICATED = 6, -- 非对外专用网卡,不支持NCSI
    NET_TYPE_MEZZ_CATED = 9, -- mezz卡 
    NET_TYPE_OCP_CARD = 10, -- OCPCard, 对应eth1，与PCIe的NCSI通过开关切换
    NET_TYPE_OCP2_CARD = 11,
    NET_TYPE_MAX = 12
}

function M.GetProcessor()
    local cpus = {}
    local npus = {}

    C.Cpu():fold(function(obj)
        local partNumber = nil
        if O.PMEServiceConfig.ARMEnable:fetch_or(0) ~= ENABLE then
            partNumber = obj.PartNum:next(rf_string_check):fetch_or(null)
        end
        if obj.Invisible:fetch_or(0) == 0 then
            cpus[#cpus + 1] = {
                State = obj.Presence:eq(1):fetch_or(nil) and 'Present' or 'Absent',
                DeviceLocator = obj.DeviceName:fetch_or(null),
                Manufacturer = obj.Manufacturer:next(rf_string_check):fetch_or(null),
                Model = utils.get_cpu_model(obj),
                Identification = obj.ProcessorID:fetch_or(null),
                FrequencyMHz = tonumber(string.match(obj.CurrentSpeed:fetch(), '%d+')),
                TotalCores = obj.CoreCount:neq(0):fetch_or(null),
                TotalThreads = obj.ThreadCount:neq(0):fetch_or(null),
                L1CacheKiB = obj.L1Cache:neq(0):fetch_or(null),
                L2CacheKiB = obj.L2Cache:neq(0):fetch_or(null),
                L3CacheKiB = obj.L3Cache:neq(0):fetch_or(null),
                EnabledSetting = obj.DisableCpuSw:eq(0):fetch_or(nil) and true or false,
                PartNumber = partNumber,
                SerialNumber = obj.SN:next(rf_string_check):fetch_or(null),
                OtherParameters = obj.MemoryTec:next(rf_string_check):fetch_or(null)
            }
        end
    end)
    if next(cpus) == nil then
        cpus = nil
    end

    C.NPU():fold(function(obj)
        local name = obj.Name:fetch_or(null)
        if name == '' then
            return
        end
        npus[#npus + 1] = {
            State = obj.Presence:eq(1):fetch_or(nil) and 'Present' or 'Absent',
            Name = obj.Name:fetch_or(null),
            Manufacturer = obj.Manufacturer:fetch_or(null),
            Model = obj.Version:fetch_or(null),
            PowerWatts = obj.Power:neq(0x8000):fetch_or(null),
            ComputeCapability = obj.ComputeCapability:fetch(),
            FirmwareVersion = obj.FirmwareVersion:fetch_or(null),
            SerialNumber = obj.SN:fetch_or(null)
        }
    end)
    if next(npus) == nil then
        npus = nil
    end

    return {CPU = cpus, NPU = npus}
end

function M.GetMemory()
    local result = {}
    local maxnumber = O.ProductComponent.MemoryNum:fetch_or(null)

    C.Memory():fold(function(obj)
        obj.Presence:eq(1):next(function()
            local bomNumber = nil
            if O.PMEServiceConfig.ARMEnable:fetch_or(0) ~= ENABLE then
                bomNumber = obj.BomNum:next(rf_string_check):fetch_or(null)
            end

            local OperatingSpeed
            if O.RedfishEventService.EventDetailCustomizedID:fetch_or(null) == ENABLE then
                OperatingSpeed = tonumber(string.match(obj.ClockSpeed:fetch(), '%d+'))
            else
                OperatingSpeed = obj.ConfigMemClkSpeed:eq('Unknown'):fetch_or(nil) and null or
                    tonumber(string.match(obj.ConfigMemClkSpeed:fetch(), '%d+'))
            end
			
            result[#result + 1] = {
                General = {
                    DeviceLocator = obj.DeviceName:fetch_or(null),
                    Manufacturer = obj.Manufacturer:fetch_or(null),
                    CapacityMiB = tonumber(string.match(obj.Capacity:fetch(), '%d+')),
                    OperatingSpeedMHz = OperatingSpeed,
                    AllowedSpeedsMHz = tonumber(string.match(obj.ClockSpeed:fetch(), '%d+')),
                    DeviceType = obj.Type:fetch_or(null),
                    Position = obj.Location:fetch_or(null),
                    MinVoltageMillivolt = obj.MinimumVoltage:neq(0):fetch_or(null),
                    PartNumber = obj.PartNum:fetch_or(null),
                    SerialNumber = obj.SN:fetch_or(null),
                    ManufacturerID = obj.ManufacturerID:fetch_or(null),
                    ManufacturingLocation = obj.ManufacturingLocation:fetch_or(null),
                    ManufacturingDate = obj.ManufacturingDate:fetch_or(null),
                    DataWidthBits = obj.BitWidth:neq(0):fetch_or(null),
                    RankCount = obj.Rank:neq(0):fetch_or(null),
                    TypeDetail = obj.Technology:fetch_or(null),
                    DimmTemp = obj.DimmTemp:neq(0x4000):fetch_or(null),
                    BomNumber = bomNumber
                },
                DCPMM = obj.Technology2:eq('Intel persistent memory'):next(
                    function()
                        return {
                            RemainingServiceLifePercent = obj.RemainLife:neq(0xFF):fetch_or(null),
                            MediumTemperatureCelsius = obj.MediaTemp:neq(0x4000):fetch_or(null),
                            ControllerTemperatureCelsius = obj.ControllerTemp:neq(0x4000):fetch_or(null),
                            FirmwareRevision = obj.FWVersion:next(rf_string_check):fetch_or(null),
                            VolatileRegionSizeLimitMiB = obj.VolatileCapacity:neq(0):fetch_or(null),
                            PersistentRegionSizeLimitMiB = obj.PersistentCapacity:neq(0):fetch_or(null)
                        }
                    end):fetch_or()
            }
        end)
    end)

    return {MaxNumber = maxnumber, List = result}
end

local function get_reading_and_status(obj)
    local status = null
    local reading = null
    local reading_status = c.EVENT_WORK_STATE_S(obj.ReadingStatus:fetch())
    if reading_status.disable_scanning ~= 0 and reading_status.disable_scanning_local ~= 0 and
        reading_status.disable_all ~= 0 and reading_status.initial_update_progress ~= 0 and
        reading_status.disable_access_error ~= 0 then
        reading = tonumber(string.format('%.3f', obj.ReaddingConvert:fetch()))
        local assert_status = c.SENSOR_STATE_BIT_T(obj.AssertStatus:fetch())
        if assert_status.assert_nonrecoverable_lgl ~= 0 or assert_status.assert_nonrecoverable_ugh ~= 0 then
            status = 'nr'
        elseif assert_status.assert_critical_lgl ~= 0 or assert_status.assert_critical_ugh ~= 0 then
            status = 'cr'
        elseif assert_status.assert_noncritical_lgl ~= 0 or assert_status.assert_noncritical_ugh ~= 0 then
            status = 'nc'
        else
            status = 'ok'
        end
    end
    return reading, status
end

local SENSOR_CAP_READABLE = 1
local SENSOR_CAP_READABLE_SETTABLE = 2

local function get_sensor_threshold(obj)
    local lower_fatal = null
    local lower_critical = null
    local lower_noncritical = null
    local upper_noncritical = null
    local upper_critical = null
    local upper_fatal = null

    local sensor_cap = c.SENSOR_CAPABILITY_S(obj.Capabilities:fetch())
    if sensor_cap.threshold_access_support == SENSOR_CAP_READABLE_SETTABLE or sensor_cap.threshold_access_support ==
        SENSOR_CAP_READABLE then
        local reading_mask = c.SENSOR_READINGMASK_BIT(obj.DiscreteReadingMask:fetch())
        lower_fatal = (reading_mask.readable_lower_nonrecoverable ~= 0) and
                          tonumber(string.format('%.3f', obj.LowerNonrecoverableConvert:fetch())) or null
        lower_critical = (reading_mask.readable_lower_critical ~= 0) and
                             tonumber(string.format('%.3f', obj.LowerCriticalConvert:fetch())) or null
        lower_noncritical = (reading_mask.readable_lower_noncritical ~= 0) and
                                tonumber(string.format('%.3f', obj.LowerNoncriticalConvert:fetch())) or null
        upper_noncritical = (reading_mask.readable_upper_noncritical ~= 0) and
                                tonumber(string.format('%.3f', obj.UpperNoncriticalConvert:fetch())) or null
        upper_critical = (reading_mask.readable_upper_critical ~= 0) and
                             tonumber(string.format('%.3f', obj.UpperCriticalConvert:fetch())) or null
        upper_fatal = (reading_mask.readable_upper_nonrecoverable ~= 0) and
                          tonumber(string.format('%.3f', obj.UpperNonrecoverableConvert:fetch())) or null
    end
    return lower_fatal, lower_critical, lower_noncritical, upper_noncritical, upper_critical, upper_fatal
end

function M.GetThresholdSensor()
    local list = {}
    local total_count = 0

    C.ThresholdSensor():fold(function(obj)
        local reading, status = get_reading_and_status(obj)
        local lower_fatal, lower_critical, lower_noncritical, upper_noncritical, upper_critical, upper_fatal =
            get_sensor_threshold(obj)
        list[#list + 1] = {
            Name = obj.SensorName:fetch(),
            Unit = obj.SensorUnitStr:neq('unspecified'):fetch_or(null),
            Status = status,
            ReadingValue = reading,
            LowerThresholdFatal = lower_fatal,
            LowerThresholdCritical = lower_critical,
            LowerThresholdNonCritical = lower_noncritical,
            UpperThresholdNonCritical = upper_noncritical,
            UpperThresholdCritical = upper_critical,
            UpperThresholdFatal = upper_fatal
        }
        total_count = total_count + 1
    end)

    return {TotalCount = total_count, List = list}
end

local function get_ipv4(port)
    local ipv4s = {}
    local tmparr = port.IPv4Info:fetch()

    for _, v in pairs(tmparr) do
        local arr = utils.split(v, ';')
        if arr and #arr == 4 then
            ipv4s[#ipv4s + 1] = {
                Address = arr[4] == 'N/A' and null or arr[4],
                Mask = arr[1] == 'N/A' and null or arr[1],
                Gateway = arr[3] == 'N/A' and null or arr[3]
            }
        end
    end
    if next(ipv4s) == nil then
        ipv4s = nil
    end
    return ipv4s
end

local function get_ipv6(port)
    local ipv6s = {}
    local tmparr = port.IPv6Info:fetch()

    for _, v in pairs(tmparr) do
        local arr = utils.split(v, ';')
        if arr and #arr == 5 then
            ipv6s[#ipv6s + 1] = {
                Address = arr[4] == 'N/A' and null or arr[4],
                Prefix = arr[2] == 'N/A' and null or arr[2],
                Gateway = arr[5] == 'N/A' and null or arr[5]
            }
        end
    end
    if next(ipv6s) == nil then
        ipv6s = nil
    end
    return ipv6s
end

local function get_vlan(port)
    local vlans = {}
    local bdf = port.BDF:fetch()
    C.BusinessPortVLAN():fold(function(vlan)
        vlan.EthBDF:eq(bdf):next(function()
            vlans[#vlans + 1] = {
                ID = vlan.Id:fetch(),
                Enable = vlan.VLANEnable:fetch() == 1 and true or false,
                Priority = vlan.VLANPriority:fetch()
            }
        end)
    end)
    if next(vlans) == nil then
        vlans = nil
    end
    return vlans
end

local PCIEFUNCTION_VALUE = {
    PCIESWITCH = {0xFF, 'PCIeSwitch'},
    PCH = {0xFE, 'PCH'},
    CPU1_AND_CPU2 = {0xFD, 'CPU1,CPU2'},
    ALL_CPU = {0xFC, 'CPU1,CPU2,CPU3,CPU4'}
}

local function get_pcbversion_and_associatedresource(netcard)
    local resid = netcard.ResId:fetch()
    local associated_resource = nil
    local pciecard_handle
    local pcb_version = C.Fru('FruId', netcard:ref_obj('RefComponent'):fetch_or().FruId:fetch_or()).PcbVersion:next(rf_string_check):fetch_or(null)
    if pcb_version == null then
        pciecard_handle = dal_get_specific_object_position(netcard, 'PcieCard'):fetch_or()
        if pciecard_handle then
            pcb_version = pciecard_handle.PcbVer:next(rf_string_check):fetch_or(null)
        end
    end

    if resid == 0 then
        return nil, nil
    end

    if resid == PCIEFUNCTION_VALUE.PCIESWITCH[1] then
        associated_resource = PCIEFUNCTION_VALUE.PCIESWITCH[2]
    elseif resid == PCIEFUNCTION_VALUE.PCH[1] then
        associated_resource = PCIEFUNCTION_VALUE.PCH[2]
    elseif resid == PCIEFUNCTION_VALUE.CPU1_AND_CPU2[1] then
        associated_resource = PCIEFUNCTION_VALUE.CPU1_AND_CPU2[2]
    elseif resid == PCIEFUNCTION_VALUE.ALL_CPU[1] then
        associated_resource = PCIEFUNCTION_VALUE.ALL_CPU[2]
    elseif resid > RESOURCE_ID_PCIE_DEVICE_BASE and resid < RESOURCE_ID_PCIE_DEVICE_MAX then
        local fruId = resid - RESOURCE_ID_PCIE_DEVICE_BASE
        associated_resource = C.Component('FruId', fruId).DeviceName:fetch_or(null)
    else
        associated_resource = string.format('CPU%d', resid)
    end

    return pcb_version, associated_resource
end

local function get_network_attribute(netcard, port)
    local ipv4 = nil
    local ipv6 = nil
    local vlan = nil

    O.sms0.Status:neq('N/A'):next(function()
        ipv4 = get_ipv4(port)
        ipv6 = get_ipv6(port)
        vlan = get_vlan(port)
    end)

    local network_attribute = {
        Name = port.OSEthName:neq('N/A'):fetch_or(nil),
        SpeedMbps = port.SpeedFromMctp:neq(0xffffffff):neq(-1):fetch_or(null),
        AutoNeg = port.NegotiateFlagFromMctp:next(ValueToBoolNoDefault):fetch_or(null),
        FullDuplex = port.DuplexFlagFromMctp:next(ValueToBoolNoDefault):fetch_or(null),
        BusInfo = port.BDF:neq('N/A'):fetch_or(nil),
        FirmwareVersion = port.FirmwareVersion:neq('N/A'):fetch_or(nil),
        DriverName = port.DriverName:neq('N/A'):fetch_or(nil),
        DriverVersion = port.DriverVersion:neq('N/A'):fetch_or(nil),
        MAC = port.ActualMac:neq('00:00:00:00:00:00'):neq('N/A'):fetch_or(nil) or
            port.MacAddr:neq('00:00:00:00:00:00'):neq('N/A'):fetch_or(nil),
        PermanentPhysicalAddress = port.MacAddr:neq('00:00:00:00:00:00'):neq('N/A'):fetch_or(nil),
        IPv4Addresses = ipv4,
        IPv6Addresses = ipv6,
        VLAN = vlan
    }

    return network_attribute
end

local function get_link_view(netcard, port)
    local link_view = nil

    netcard.LLDPEnableFlag:eq(1):next(function()
        link_view = {
            SwitchName = port.PortConnecSwitchName:neq('N/A'):fetch_or(nil),
            SwitchConnectionID = port.ChassisIDSubDesp:neq('N/A'):fetch_or(nil),
            SwitchPortID = port.PortIDSubDesp:neq('N/A'):fetch_or(nil),
            SwitchVLANID = port.PortOSVlanID:neq(0xFFFF):fetch_or(nil)
        }
    end)

    if link_view == nil or next(link_view) == nil then
        return nil
    else
        return link_view
    end
end

local function get_dcb(netcard, port)
    local dcb = nil

    netcard.MctpSupport:eq(1):next(function()
        dcb = {
            Pfcup = port.Pfcmap:neq(0xffff):fetch_or(nil),
            Up2cos = port.Cos2up:fetch_or(null), ---g_variant_get_fixed_array
            Pgid = port.Uppgid:fetch_or(null), ---g_variant_get_fixed_array
            PGPCT = port.Pgpct:fetch_or(null), ---g_variant_get_fixed_array
            PgStrict = port.Strict:fetch_or(null) ---g_variant_get_fixed_array
        }
    end)

    if dcb == nil or next(dcb) == nil then
        return nil
    else
        return dcb
    end
end

local function get_statistics(netcard, port)
    local statistics = nil
    local received = nil
    local transmitted = nil

    netcard.MctpSupport:eq(1):next(function()
        received = {
            TotalBytesReceived = port.TotalBytesReceived:neq('N/A'):fetch_or(nil),
            UnicastPacketsReceived = port.UnicastPacketsReceived:neq('N/A'):fetch_or(nil),
            MulticastPacketsReceived = port.MulticastPacketsReceived:neq('N/A'):fetch_or(nil),
            BroadcastPacketsReceived = port.BroadcastPacketsReceived:neq('N/A'):fetch_or(nil),
            FcsReceiveErrors = port.FcsReceiveErrors:neq('N/A'):fetch_or(nil),
            AlignmentErrors = port.AlignmentErrors:neq('N/A'):fetch_or(nil),
            RuntPacketsReceived = port.RuntPacketsReceived:neq('N/A'):fetch_or(nil),
            JabberPacketsReceived = port.JabberPacketsReceived:neq('N/A'):fetch_or(nil)
        }
        if received == nil or next(received) == nil then
            received = nil
        end

        transmitted = {
            TotalBytesTransmitted = port.TotalBytesTransmitted:neq('N/A'):fetch_or(nil),
            UnicastPacketsTransmitted = port.UnicastPacketsTransmitted:neq('N/A'):fetch_or(nil),
            MulticastPacketsTransmitted = port.MulticastPacketsTransmitted:neq('N/A'):fetch_or(nil),
            BroadcastPacketsTransmitted = port.BroadcastPacketsTransmitted:neq('N/A'):fetch_or(nil),
            SingleCollisionTransmitFrames = port.SingleCollisionTransmitFrames:neq('N/A'):fetch_or(nil),
            MultipleCollisionTransmitFrames = port.MultipleCollisionTransmitFrames:neq('N/A'):fetch_or(nil),
            LateCollisionFrames = port.LateCollisionFrames:neq('N/A'):fetch_or(nil),
            ExcessiveCollisionFrames = port.ExcessiveCollisionFrames:neq('N/A'):fetch_or(nil)
        }
        if transmitted == nil or next(transmitted) == nil then
            transmitted = nil
        end

        statistics = {Received = received, Transmitted = transmitted}
    end)

    if statistics == nil or next(statistics) == nil then
        return nil
    else
        return statistics
    end
end

local function get_threshold_critical_format(reading_value_name, reading_value_array, lower_threshold_critical, upper_threshold_critical)
    local cur_value = null
    local lower_threshold = null
    local upper_threshold = null
    local result = {}
    for k, v in pairs(reading_value_array) do
        if v and v ~= INVALID_DOUBLE_VALUE then
            cur_value = tonumber(string.format('%.2f', v))
        end
        if lower_threshold_critical and lower_threshold_critical ~= INVALID_DOUBLE_VALUE then
            lower_threshold = tonumber(string.format('%.2f', lower_threshold_critical))
        end
        if upper_threshold_critical and upper_threshold_critical ~= INVALID_DOUBLE_VALUE then
            upper_threshold = tonumber(string.format('%.2f', upper_threshold_critical))
        end
        local array = {}
        array[reading_value_name] = cur_value
        array["LowerThresholdCritical"] = lower_threshold
        array["UpperThresholdCritical"] = upper_threshold
        result[#result + 1] = array
    end
    return result
end

local function get_transceiver_type(optical)
    local transceiver_type = optical.TransceiverType:next(rf_string_check):fetch_or(nil)
    if transceiver_type ~= nil and transceiver_type ~= null then
        return utils.split(transceiver_type, ';')
    end
    return transceiver_type
end

local function get_optical_module_info(optical)
    local production_date = nil
    optical.ManufactureDate:neq('N/A'):next(function()
        production_date = '20' .. string.sub(optical.ManufactureDate:fetch(), 1, 2) .. '-' ..
                              string.sub(optical.ManufactureDate:fetch(), 3, 4) .. '-' ..
                              string.sub(optical.ManufactureDate:fetch(), 5, 6)
    end)

    local medium_mode = nil
    optical.MediumMode:neq('N/A'):next(function()
        if 'SM' == optical.MediumMode:fetch() then
            medium_mode = 'Monomode'
        elseif 'MM' == optical.MediumMode:fetch() then
            medium_mode = 'Multimode'
        end
    end)

    local speedArray = {}
    optical.Speed:next(function(speed)
        if speed ~= nil and #speed ~= 0 and (#speed ~= 1 or speed[1] ~= '') then
            for k, v in pairs(speed) do
                local value = v:gsub("[^%d]", "")
                if  value ~= nil then
                    speedArray[#speedArray + 1] = tonumber(value)
                end
            end
        end
    end):fetch_or()

    local optical_module = {
        Manufacturer = optical.VendorName:neq('N/A'):fetch_or(nil),
        PartNumber = optical.PartNumber:neq('N/A'):fetch_or(nil),
        SerialNumber = optical.SerialNumber:neq('N/A'):fetch_or(nil),
        ProductionDate = production_date,
        TransceiverType = get_transceiver_type(optical),
        PackagingType = optical.Type:neq('N/A'):fetch_or(nil),
        MediumMode = medium_mode,
        WaveLengthNanometer = optical.Wavelength:neq('N/A'):fetch_or(nil),
        SupportedSpeedsMbps = speedArray,
        Indentifier = optical.Indentifier:neq('N/A'):fetch_or(nil),
        DeviceType = optical.DeviceType:neq('N/A'):fetch_or(nil),
        ConnectorType = optical.ConnectorType:neq('N/A'):fetch_or(nil),
        TransferDistance = optical.TransferDistance:neq(0xffff):fetch_or(nil),
        RxLosState = optical.RxLosState:neq(0xff):fetch_or(nil),
        TxFaultState = optical.TxFultState:neq(0xff):fetch_or(nil),
        Temperature = {
            ReadingCelsius = optical.TempCurrent:neq(INVALID_DOUBLE_VALUE):next(function(value)
                return tonumber(string.format('%.2f', value))
            end):fetch_or(null),
            LowerThresholdCritical = optical.TempLowAlarm:neq(INVALID_DOUBLE_VALUE):next(function(value)
                return tonumber(string.format('%.2f', value))
            end):fetch_or(null),
            UpperThresholdCritical = optical.TempHighAlarm:neq(INVALID_DOUBLE_VALUE):next(function(value)
                return tonumber(string.format('%.2f', value))
            end):fetch_or(null)
        },
        Voltage = {
            ReadingVolts = optical.VccCurrent:neq(INVALID_DOUBLE_VALUE):next(function(value)
                return tonumber(string.format('%.2f', value))
            end):fetch_or(null),
            LowerThresholdCritical = optical.VccLowAlarm:neq(INVALID_DOUBLE_VALUE):next(function(value)
                return tonumber(string.format('%.2f', value))
            end):fetch_or(null),
            UpperThresholdCritical = optical.VccHighAlarm:neq(INVALID_DOUBLE_VALUE):next(function(value)
                return tonumber(string.format('%.2f', value))
            end):fetch_or(null)
        },
        TXBiasCurrent = get_threshold_critical_format("ReadingMilliAmperes", optical.TxBiasCurrent:fetch_or(), 
            optical.TxBiasLowAlarm:fetch_or(), optical.TxBiasHighAlarm:fetch_or()),
        RXPower = get_threshold_critical_format("ReadingMilliWatts", optical.RxPowerCurrent:fetch_or(), 
            optical.RxPowerLowAlarm:fetch_or(), optical.RxPowerHighAlarm:fetch_or()),
        TXPower = get_threshold_critical_format("ReadingMilliWatts", optical.TxPowerCurrent:fetch_or(), 
            optical.TxPowerLowAlarm:fetch_or(), optical.TxPowerHighAlarm:fetch_or())
    }
    return optical_module
end

local function get_optical_module(_, port)
    local optical_module = nil
    C.OpticalModule():fold(function(optical)
        optical.Present:eq(1):next(function()
            if dflib.object_name(optical:ref_obj('RefPortObj'):fetch()) == dflib.object_name(port) then
                optical_module = get_optical_module_info(optical)
            end
        end)
        if optical_module then
            return _, false
        end
    end)
    if not optical_module then
        C.VirOpticalModule():fold(function(optical)
            optical.Present:eq(1):next(function()
                if dflib.object_name(optical:ref_obj('RefPortObj'):fetch()) == dflib.object_name(port) then
                    optical_module = get_optical_module_info(optical)
                end
            end)
            if optical_module then
                return _, false
            end
        end)
    end

    if optical_module == nil or next(optical_module) == nil then
        return nil
    else
        return optical_module
    end
end

local function get_chassis_networkport_linkstatus(obj)
    return obj.LinkStatus:next(function(status)
        if status == 0 then
            return 'Down'
        elseif status == 1 then
            return 'Up'
        end
        return null
    end):fetch_or(null)
end

local function get_system_ethernetinterface_link_status(obj)
    return obj.LinkStatusStr:neq('N/A'):fetch_or(obj.LinkStatus:fetch() == 0 and 'NoLink' or null)
end

local function GetLinkStatus(obj)
    if obj.AvailableFlag:fetch() == 0 then
        return 'Disabled'
    end
    if obj.OSEthName:fetch() == 'N/A' then
        return get_chassis_networkport_linkstatus(obj)
    end
    return get_system_ethernetinterface_link_status(obj)
end

local function GetIFType(obj)
    if obj.IFType:fetch() == 0 then
        return 'Electrical'
    elseif obj.IFType:fetch() == 1 then
        return 'Optical'
    else
        return nil
    end
end

local function GetSilkNum(netcard, obj)
    if O.PRODUCT.ProductId:fetch_or(null) == 0x0d then 
        if (O.BOARD.BoardId:fetch_or(null) == 0x1b and netcard.CardType:fetch_or(null) == 3) --盘古大西洋PCIE卡从0开始
            or (netcard.CardType:fetch_or(null) == 1 and O.BOARD.BoardId:fetch_or(null) ~= 159) then --盘古板载网卡丝印从0开始
            return obj.SilkNum:fetch() - 1
        end
    end
    return obj.SilkNum:fetch()
end

local function get_netcard_port_properties(netcard)
    local port_properties = {}
    C.BusinessPort():fold(function(port)
        -- CardType为6:虚拟网口，在此需要过滤，web不显示
        if dflib.object_name(port:ref_obj('RefNetCard'):fetch()) == dflib.object_name(netcard) and port.CardType:fetch_or() ~= 6 and port.SilkNum:fetch() ~= 255 then
            local basic_attribute = {
                PhysicalPortNumber = GetSilkNum(netcard, port),
                LinkStatus = GetLinkStatus(port),
                InterfaceType = port.PortType:eq(0):fetch_or(nil) and 'Virtual' or 'Physical',
                MediaType = GetIFType(port)
            }
            port_properties[#port_properties + 1] = {
                BasicAttribute = basic_attribute,
                NetworkAttribute = get_network_attribute(netcard, port),
                LinkView = get_link_view(netcard, port),
                DCB = get_dcb(netcard, port),
                Statistics = get_statistics(netcard, port),
                OpticalModule = get_optical_module(netcard, port)
            }
        end
    end)

    if port_properties == nil or next(port_properties) == nil then
        return nil
    else
        return port_properties
    end
end

local function get_fccard_port_properties(netcard)
    local port_properties = {}
    C.BusinessPort():fold(function(port)
        if dflib.object_name(port:ref_obj('RefNetCard'):fetch_or()) == dflib.object_name(netcard) and
            port:ref_obj('RefSlavePort'):neq(null) then
            local slave_port = port:ref_obj('RefSlavePort'):fetch_or()
            local work_mode = nil
            local nego_stage = nil
            local port_status = nil

            if slave_port.WorkTopo:fetch_or() == 1 then
                work_mode = 'Loop'
            elseif slave_port.WorkTopo:fetch_or() == 2 then
                work_mode = 'NonLoop'
            end

            if slave_port.SnStage:fetch_or() == 0 or slave_port.SnStage:fetch_or() == 1 then
                nego_stage = 'WaitSignal'
            elseif slave_port.SnStage:fetch_or() == 2 or slave_port.SnStage:fetch_or() == 3 or slave_port.SnStage:fetch_or() ==
                4 then
                nego_stage = 'Negotiating'
            elseif slave_port.SnStage:fetch_or() == 5 then
                nego_stage = 'NegotiationCompleted'
            end

            if slave_port.PortStatus:fetch_or() == 0 then
                port_status = 'Disabled'
            elseif slave_port.PortStatus:fetch_or() == 1 then
                port_status = 'LinkUp'
            elseif slave_port.PortStatus:fetch_or() == 4 then
                port_status = 'WaitSignal'
            end

            port_properties[#port_properties + 1] = {
                BasicAttribute = {
                    Name = slave_port.PortName:neq('N/A'):fetch_or(null),
                    FC_ID = slave_port.FCId:neq('N/A'):fetch_or(null),
                    PhysicalPortNumber = GetSilkNum(netcard, port),
                    LinkStatus = slave_port.LinkStatus:neq('N/A'):fetch_or(null),
                    InterfaceType = slave_port.PortType:neq('N/A'):fetch_or(null),
                    MediaType = GetIFType(port)
                },
                NetworkAttribute = {
                    LinkSpeedGbps = slave_port.LinkSpeed:neq(-1):neq(RF_INFO_INVALID_DWORD):fetch_or(null),
                    WWPN = slave_port.WWPN:neq('N/A'):neq('00:00:00:00:00:00'):fetch_or(null),
                    WWNN = slave_port.WWNN:neq('N/A'):neq('00:00:00:00:00:00'):fetch_or(null),
                    FirmwareVersion = port.FirmwareVersion:neq('N/A'):fetch_or(null),
                    DriverName = port.DriverName:neq('N/A'):fetch_or(null),
                    DriverVersion = port.DriverVersion:neq('N/A'):fetch_or(null),
                    WorkMode = work_mode,
                    TxBbCredit = slave_port.TxBbCredit:neq(0xffff):fetch_or(nil),
                    RxBbCredit = slave_port.RxBbCredit:neq(0xffff):fetch_or(nil),
                    TXRateGbps = slave_port.TxSnSpeed:neq(0xff):fetch_or(nil),
                    RXRateGbps = slave_port.RxSnSpeed:neq(0xff):fetch_or(nil),
                    RateNegotiationStage = nego_stage,
                    PortStatus = port_status,
                    WorkingRate = slave_port.WorkSpeed:neq(0xff):fetch_or(nil)
                },
                OpticalModule = get_optical_module(netcard, port)
            }
        end
    end)

    if port_properties == nil or next(port_properties) == nil then
        return nil
    else
        return port_properties
    end
end

local function get_fccards(netcard)
    local pcie_slot_id = nil
    netcard.CardType:eq(NET_TYPE_E.NET_TYPE_PCIE):next(function()
        pcie_slot_id = netcard.SlotId:fetch()
    end)
    local board_id = nil
    netcard:ref_obj('RefComponent'):fetch().BoardId:neq(0xffff):neq(0):next(
        function(obj)
            board_id = string.format('0x%04x', netcard:ref_obj('RefComponent'):fetch().BoardId:fetch())
        end)
    local pcb_version, associated_resource = get_pcbversion_and_associatedresource(netcard)
    local fccard = {
        DisplayName = netcard:ref_obj('RefComponent'):fetch().DeviceName:fetch(),
        CardName = netcard.ProductName:fetch(),
        CardModel = netcard.CardDesc:fetch(),
        CardManufacturer = netcard.Manufacture:fetch(),
        ChipModel = netcard.Model:fetch(),
        SerialNumber = netcard.SerialNumber:fetch_or(),
        PartNumber = dal_get_specific_object_position(netcard, "PcieCard"):fetch().PartNum:fetch_or(),
        ChipManufacturer = netcard.ChipManufacturer:fetch(),
        PortProperties = get_fccard_port_properties(netcard),
        BoardID = board_id,
        PcieSlotId = pcie_slot_id,
        PCBVersion = pcb_version,
        AssociatedResource = associated_resource,
        AdapterId = (netcard:ref_obj('RefComponent'):fetch().Location:fetch_or('') .. netcard:ref_obj('RefComponent'):fetch().DeviceName:fetch('')):gsub(' ', ''),
    }

    return fccard
end

local function GetLedState(v)
    return G_OCP_LED_STATE[v] or null
end

local function get_location_devicename(obj)
    return obj.Location:next(dal_clear_string_blank):fetch_or('') ..
               obj.DeviceName:next(dal_clear_string_blank):fetch_or('')
end

local function get_netcards(netcard)
    local pcb_version, associated_resource = get_pcbversion_and_associatedresource(netcard)
    local board_id = nil
    netcard:ref_obj('RefComponent'):fetch().BoardId:neq(0xffff):neq(0):next(
        function(obj)
            board_id = string.format('0x%04x', netcard:ref_obj('RefComponent'):fetch().BoardId:fetch())
        end)
    local pcie_slot_id = nil
    local pciecard_handle
    netcard.CardType:eq(3):next(function()
        pciecard_handle = dal_get_specific_object_position(netcard, 'PcieCard'):fetch_or()
        if pciecard_handle.LogicSlot:fetch_or() ~= nil and pciecard_handle.LogicSlot:fetch_or() ~= null and pciecard_handle.LogicSlot:fetch_or() ~=0xff then
            pcie_slot_id = pciecard_handle.LogicSlot:fetch_or()
        else
            pcie_slot_id = netcard.SlotId:fetch()
        end
    end)
    local result = {
        DisplayName = netcard:ref_obj('RefComponent'):fetch().DeviceName:fetch(),
        CardName = netcard.ProductName:fetch(),
        CardModel = netcard.CardDesc:fetch(),
        CardManufacturer = netcard.Manufacture:fetch(),
        ChipModel = netcard.Model:fetch(),
        ChipManufacturer = netcard.ChipManufacturer:fetch(),
        BoardID = board_id,
        PCBVersion = pcb_version,
        AssociatedResource = associated_resource,
        BusInfo = netcard.RootBDF:fetch(),
        PcieSlotId = pcie_slot_id,
        PortProperties = get_netcard_port_properties(netcard),
        HotPlugSupported = netcard.SupportHotPlug:neq(0xff):next(ValueToBool):fetch_or(null),
        HotPlugAttention = netcard.PrePlug:neq(0xff):next(ValueToBool):fetch_or(null),
        OrderlyHotplugCtrlStatus = netcard.HotplugCtrlStatus:neq(0xff):next(GetLedState):fetch_or(null),
        AdapterId = netcard:ref_obj('RefComponent'):next(get_location_devicename):fetch_or(null)
    }

    local component_unique_id = nil                                                            
    netcard:ref_obj('RefComponent'):fetch().ComponentUniqueID:next(                           
        function(obj)                                                                         
            component_unique_id = netcard:ref_obj('RefComponent'):fetch_or(null).ComponentUniqueID:fetch_or(null)
        end) 
    add_component_uid(result, component_unique_id)
    return result
end

local function get_teambridge(port)
    local ports = {}
    local contains = nil
    port.ContainedPorts:neq('N/A'):next(function()
        contains = utils.split(port.ContainedPorts:fetch(), ';')
    end)

    C.BusinessPort():fold(function(node)
        for _, v in ipairs(contains) do
            if v == node.OSEthName:fetch() then
                ports[#ports + 1] = {
                    PhysicalPortNumber = node.SilkNum:fetch(),
                    Name = node.OSEthName:neq('N/A'):fetch_or(nil),
                    NetCardName = node.DisplayName:neq('N/A'):fetch_or(nil),
                    MAC = node.ActualMac:neq('00:00:00:00:00:00'):neq('N/A'):fetch_or(nil) or
                        port.MacAddr:neq('00:00:00:00:00:00'):neq('N/A'):fetch_or(nil),
                    LinkStatus = node.LinkStatusStr:neq('N/A'):fetch_or(
                        port.LinkStatus:fetch() == 0 and 'NoLink' or null)
                }
                break
            end
        end
    end)

    if next(ports) == nil then
        ports = nil
    end

    local detail = {
        Name = port.OSEthName:neq('N/A'):fetch_or(nil),
        LinkStatus = port.LinkStatusStr:neq('N/A'):fetch_or(port.LinkStatus:fetch() == 0 and 'NoLink' or null),
        WorkMode = port.WorkMode:neq('N/A'):fetch_or(nil),
        IPv4Addresses = get_ipv4(port),
        IPv6Addresses = get_ipv6(port),
        VLAN = get_vlan(port),
        MAC = port.ActualMac:neq('00:00:00:00:00:00'):neq('N/A'):fetch_or(nil) or
            port.MacAddr:neq('00:00:00:00:00:00'):neq('N/A'):fetch_or(nil),
        PortProperties = ports
    }

    return detail
end

local function is_fc_card(netcard)
    local flag = false
    local card_name = dflib.object_name(netcard)
    C.BusinessPort():fold(function(port)
        if port.RefNetCard:fetch() == card_name and port.SupportedFuncType:fetch() == 2 then
            flag = true
        end
    end)
    return flag
end

local function get_net_card_info(netCardHandle)
    return {
        netCardHandle,
        netCardHandle.SlotId:fetch_or(0),
        netCardHandle.CardType:fetch_or(0)
    }
end

local function net_card_slot_compared(netcard_1, netcard_2)
    if netcard_1[2] > netcard_2[2] then
        return false -- 交换
    end

    if netcard_1[2] == netcard_2[2] and 
       netcard_1[3] >= netcard_2[3] then
        return false -- 交换
    end

    return true  -- 不交换
end

local function net_card_compared(netcard_1, netcard_2)
    if netcard_1[3] == NET_TYPE_E.NET_TYPE_LOM then
        if netcard_2[3] == NET_TYPE_E.NET_TYPE_LOM then
            return net_card_slot_compared(netcard_1, netcard_2);
        end

        return true
    end

    if netcard_1[3] == NET_TYPE_E.NET_TYPE_LOM2 then
        if netcard_2[3] == NET_TYPE_E.NET_TYPE_LOM then
            return false
        elseif netcard_2[3] == NET_TYPE_E.NET_TYPE_LOM2 then
            return net_card_slot_compared(netcard_1, netcard_2);
        end

        return true
    end

    if netcard_2[3] == NET_TYPE_E.NET_TYPE_LOM or netcard_2[3] == NET_TYPE_E.NET_TYPE_LOM2 then
        return false
    end

    return net_card_slot_compared(netcard_1, netcard_2);
end

function M.GetNetworkAdapter()
    local fc_cards = {}
    local net_cards = {}
    local net_cards_res = {}
    local team = {}
    local bridge = {}
    C.NetCard():fold(function(netcard)
        netcard.VirtualNetCardFlag:neq(1):next(function()
            if is_fc_card(netcard) then
                fc_cards[#fc_cards + 1] = get_fccards(netcard)
            else
                net_cards[#net_cards + 1] = get_net_card_info(netcard) 
            end
        end)
    end)

    tsort(net_cards, net_card_compared)

    for i=1, #net_cards, 1 do
        net_cards_res[#net_cards_res + 1] = get_netcards(net_cards[i][1])
    end

    C.BusinessPort():fold(function(port)
        if port.CardType:fetch() == 7 then
            team[#team + 1] = get_teambridge(port)
        elseif port.CardType:fetch() == 8 then
            bridge[#bridge + 1] = get_teambridge(port)
        end
    end)
    return {NetCard = net_cards_res, FCCard = fc_cards, Team = team, Bridge = bridge}
end

local function create_error_message(errorData)
    local body_ok, errorJson = pcall(decode, errorData)
    if not body_ok then
        return reply_bad_request()
    end
    local messageId = errorJson.MessageId
    if not messageId then
        return reply_bad_request()
    end
    local prop, val, relProp, relVal = errorJson.Property, errorJson.Value, errorJson.RelatedProperty,
        errorJson.RelatedValue
    if messageId == 'PropertyMissing' or messageId == 'ExcessiveQuantity' or messageId == 'InsufficientQuantity' or
        messageId == 'PropertyUnknown' or messageId == 'ChipRuleCheckFail' or messageId == 'PropertyValueError' or
        messageId == 'BootProtocolNotFoundInPFs' then
        if prop then
            return reply_bad_request(messageId, {prop}, {prop})
        end
    elseif messageId == 'PropertyModificationNotSupported' then
        return reply_bad_request(messageId, {'Configuration'}, {'Configuration'})
    elseif messageId == 'PropertyValueNotInList' or messageId == 'PropertyValueTypeError' then
        if prop and val then
            return reply_bad_request(messageId, {val, prop}, {prop})
        end
    elseif messageId == 'CurrentModeNotSupport' then
        if prop and relProp and relVal then
            return reply_bad_request(messageId, {relProp, relVal, prop}, {prop})
        end
    elseif messageId == 'SettingBandWidthFailed' then
        if prop and relProp then
            return reply_bad_request(messageId, {prop, relProp}, {prop})
        end
    end
    return reply_bad_request(messageId)
end

function M.SetNetworkAdapter(adapterId, user, data)
    if not adapterId or #adapterId > 128 or #adapterId == 0 then
        return reply_bad_request("Invalid param")
    end
    local netcard = C.NetCard():fold(function(obj)
        local isMatch = obj:ref_obj('RefComponent'):next(function(oo)
            return get_location_devicename(oo) == adapterId
        end):fetch_or(false)
        if isMatch then
            return obj, false
        end
    end):fetch_or()

    if not netcard then
        return reply_not_found('ResourceMissingAtURI')
    end

    local result = reply_ok()
    result:join(safe_call(function()
        local ok, ret = call_method(user, netcard, 'SetPrePlug', nil, {gbyte(data.HotPlugAttention and 0 or 1)})
        if not ok then
            return reply_internal_server_error('InternalError')
        elseif ret and ret[1] then
            return create_error_message(ret[1])
        end
    end))
    if result:isOk() then
        return reply_ok_encode_json(M.GetNetworkAdapter())
    end
    result:appendErrorData(M.GetNetworkAdapter())
    return result
end

return M
