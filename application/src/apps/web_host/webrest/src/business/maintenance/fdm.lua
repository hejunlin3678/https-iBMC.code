local dflib = require "dflib"
local c = require "dflib.core"
local tasks = require "tasks"
local utils = require "utils"
local http = require "http"
local cjson = require "cjson"
local logging = require "logging"
local cfg = require "config"
local bit = require "bit"
local json_decode = cjson.decode
local null = cjson.null
local file_exists = utils.file_exists
local call_method = utils.call_method
local IntToHexStr = utils.IntToHexStr
local rf_string_check = utils.rf_string_check
local redfish_get_board_class_name = utils.redfish_get_board_class_name
local dal_get_specific_object_position = utils.dal_get_specific_object_position
local flock = c.flock
local reply_internal_server_error = http.reply_internal_server_error
local reply_not_found = http.reply_not_found
local C = dflib.class
local O = dflib.object
local class_name = dflib.class_name
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local CDTE = cfg.COMPONENT_DEVICE_TYPE_E
local COMPONENT_TYPE_HDD_BACKPLANE = CDTE.COMPONENT_TYPE_HDD_BACKPLANE
local reply_bad_request = http.reply_bad_request

local MAX_RECURSION_DEPTH = 200
local HEALTH_REPORT_JSON_PATH = "/opt/pme/pram/diagnose_fdm_health_report.json"

local REAR_BACKPLANE_CONNECT = "RearHddBackConnect"
local INNER_BACKPLANE_CONNECT = "InnerHddBackConnect"
local FRONT_BACKPLANE_CONNECT = "FrontHddBackConnect"

local CLASS_RAIDCARD_NAME = "Raid"
local CLASS_PCIECARD_NAME = "PcieCard" -- PCIE标卡类
local CLASS_MEZZ = "MezzCard"
local CLASS_NETCARD_NAME = "NetCard"

-- 获取硬盘背板的后缀字符
local function get_chassis_disk_backplane(comObj)
    comObj.DeviceType:eq(COMPONENT_TYPE_HDD_BACKPLANE):fetch()
    local pAnchorObj = dal_get_specific_object_position(comObj, "Anchor")
    local type = pAnchorObj:next(function(anchorObj)
        return anchorObj.Position
    end):catch(0):next(function(position)
        return C.Connector("Position", position)
    end):next(function(connectorObj)
        return connectorObj.Type
    end):fetch_or()
    if type == REAR_BACKPLANE_CONNECT then
        return "Rear"
    elseif type == INNER_BACKPLANE_CONNECT then
        return "Inner"
    elseif type == FRONT_BACKPLANE_CONNECT then
        return "Front"
    end
    return ""
end

local speedgbs_map = {
    [1] = 1.5,
    [2] = 3,
    [3] = 6,
    [4] = 12,
    [5] = 2.5,
    [6] = 5,
    [7] = 8,
    [8] = 10,
    [9] = 16,
    [10] = 20,
    [11] = 30,
    [12] = 32,
    [13] = 40,
    [14] = 64,
    [15] = 80,
    [16] = 96,
    [17] = 128,
    [18] = 160,
    [19] = 256,
    [20] = 22.5
}
local function redfish_system_storage_drive_speedgbs_result(speedgbs)
    return speedgbs_map[speedgbs];
end

local RFPROP_DRIVE_POWER_STATE_V_SPUNUP_NUM = 0
local RFPROP_DRIVE_POWER_STATE_V_SPUNDOWN_NUM = 1
local RFPROP_DRIVE_POWER_STATE_V_TRANSITION_NUM = 2
local RFPROP_DRIVE_POWER_STATE_V_SPUNUP = "Spun Up"
local RFPROP_DRIVE_POWER_STATE_V_SPUNDOWN = "Spun Down"
local RFPROP_DRIVE_POWER_STATE_V_TRANSITION = "Transition"
local function get_drive_oem_property_powerstate(obj)
    local v = obj.PowerState:fetch()
    if v == RFPROP_DRIVE_POWER_STATE_V_SPUNUP_NUM then
        return RFPROP_DRIVE_POWER_STATE_V_SPUNUP
    elseif v == RFPROP_DRIVE_POWER_STATE_V_SPUNDOWN_NUM then
        return RFPROP_DRIVE_POWER_STATE_V_SPUNDOWN
    elseif v == RFPROP_DRIVE_POWER_STATE_V_TRANSITION_NUM then
        return RFPROP_DRIVE_POWER_STATE_V_TRANSITION
    end
end

local PD_STATE_UNCONFIGURED_GOOD = 0 -- Unconfigured good drive
local PD_STATE_UNCONFIGURED_BAD = 1 -- Unconfigured bad drive
local PD_STATE_HOT_SPARE = 2 -- Hot spare drive
local PD_STATE_OFFLINE = 3 -- Configured - good drive (data invalid)
local PD_STATE_FAILED = 4 -- Configured - bad drive (data invalid)
local PD_STATE_REBUILD = 5 -- Configured - drive is rebuilding
local PD_STATE_ONLINE = 6 -- Configured - drive is online
local PD_STATE_COPYBACK = 7 -- drive is getting copied
local PD_STATE_SYSTEM = 8 -- drive is exposed and controlled by host
local PD_STATE_SHIELD_UNCONFIGURED = 9 -- UnConfigured - shielded
local PD_STATE_SHIELD_HOT_SPARE = 10 -- Hot Spare - shielded
local PD_STATE_SHIELD_CONFIGURED = 11 -- Configured - shielded
local PD_STATE_FOREIGN = 12 -- With foreign configuration
local PD_STATE_ACTIVE = 13
local PD_STATE_STANDBY = 14
local PD_STATE_SLEEP = 15
local PD_STATE_DST = 16
local PD_STATE_SMART = 17
local PD_STATE_SCT = 18
local PD_STATE_RAW = 20
local PD_STATE_READY = 21
local PD_STATE_NOT_SUPPORTED = 22
local PD_STATE_PREDICTIVE_FAILURE = 23
local PD_STATE_DIAGNOSING = 24
local PD_STATE_INCOMPATIBLE = 25
local PD_STATE_ERASING = 26
local RF_PHYSICALDISKSTATE_UNCONFIGUREDGOOD = "UnconfiguredGood"
local RF_PHYSICALDISKSTATE_UNCONFIGUREDBAD = "UnconfiguredBad"
local RF_PHYSICALDISKSTATE_HOTSPARE = "HotSpareDrive"
local RF_PHYSICALDISKSTATE_CONFIGUREDGOOD = "Offline"
local RF_PHYSICALDISKSTATE_CONFIGUREDBAD = "Failed"
local RF_PHYSICALDISKSTATE_ACTIVE = "Active"
local RF_PHYSICALDISKSTATE_STANDBY = "Standby"
local RF_PHYSICALDISKSTATE_SLEEP = "Sleep"
local RF_PHYSICALDISKSTATE_DST = "DSTInProgress"
local RF_PHYSICALDISKSTATE_SMART = "SMARTOfflineDataCollection"
local RF_PHYSICALDISKSTATE_SCT = "SCTCommand"
local RF_PHYSICALDISKSTATE_REBUILDING = "Rebuilding"
local RF_PHYSICALDISKSTATE_ONLINE = "Online"
local RF_PHYSICALDISKSTATE_GETTINGCOPIED = "GettingCopied"
local RF_PHYSICALDISKSTATE_CONTROLLEDBYHOST = "JBOD"
local RF_PHYSICALDISKSTATE_UNCONFIGUREDSHIELDED = "UnconfiguredShielded"
local RF_PHYSICALDISKSTATE_HOTSPARESHIELDED = "HotSpareShielded"
local RF_PHYSICALDISKSTATE_CONFIGUREDSHIELDED = "ConfiguredShielded"
local RF_PHYSICALDISKSTATE_FOREIGN = "Foreign"
local RF_PHYSICALDISKSTATE_RAW = "Raw"
local RF_PHYSICALDISKSTATE_READY = "Ready"
local RF_PHYSICALDISKSTATE_NOTSUPPORTED = "NotSupported"
local RF_PHYSICALDISKSTATE_PREDICTIVE_FAILURE = "PredictiveFailure"
local RF_PHYSICALDISKSTATE_DIAGNOSING = "Diagnosing"
local RF_PHYSICALDISKSTATE_INCOMPATIBLE = "Incompatible"
local RF_PHYSICALDISKSTATE_ERASING = "EraseInProgress"
local state_map = {
    [PD_STATE_UNCONFIGURED_GOOD] = RF_PHYSICALDISKSTATE_UNCONFIGUREDGOOD,
    [PD_STATE_UNCONFIGURED_BAD] = RF_PHYSICALDISKSTATE_UNCONFIGUREDBAD,
    [PD_STATE_HOT_SPARE] = RF_PHYSICALDISKSTATE_HOTSPARE,
    [PD_STATE_OFFLINE] = RF_PHYSICALDISKSTATE_CONFIGUREDGOOD,
    [PD_STATE_FAILED] = RF_PHYSICALDISKSTATE_CONFIGUREDBAD,
    [PD_STATE_REBUILD] = RF_PHYSICALDISKSTATE_REBUILDING,
    [PD_STATE_ONLINE] = RF_PHYSICALDISKSTATE_ONLINE,
    [PD_STATE_COPYBACK] = RF_PHYSICALDISKSTATE_GETTINGCOPIED,
    [PD_STATE_SYSTEM] = RF_PHYSICALDISKSTATE_CONTROLLEDBYHOST,
    [PD_STATE_SHIELD_UNCONFIGURED] = RF_PHYSICALDISKSTATE_UNCONFIGUREDSHIELDED,
    [PD_STATE_SHIELD_HOT_SPARE] = RF_PHYSICALDISKSTATE_HOTSPARESHIELDED,
    [PD_STATE_SHIELD_CONFIGURED] = RF_PHYSICALDISKSTATE_CONFIGUREDSHIELDED,
    [PD_STATE_FOREIGN] = RF_PHYSICALDISKSTATE_FOREIGN,
    [PD_STATE_ACTIVE] = RF_PHYSICALDISKSTATE_ACTIVE,
    [PD_STATE_STANDBY] = RF_PHYSICALDISKSTATE_STANDBY,
    [PD_STATE_SLEEP] = RF_PHYSICALDISKSTATE_SLEEP,
    [PD_STATE_DST] = RF_PHYSICALDISKSTATE_DST,
    [PD_STATE_SMART] = RF_PHYSICALDISKSTATE_SMART,
    [PD_STATE_SCT] = RF_PHYSICALDISKSTATE_SCT,
    [PD_STATE_RAW] = RF_PHYSICALDISKSTATE_RAW,
    [PD_STATE_READY] = RF_PHYSICALDISKSTATE_READY,
    [PD_STATE_NOT_SUPPORTED] = RF_PHYSICALDISKSTATE_NOTSUPPORTED,
    [PD_STATE_PREDICTIVE_FAILURE] = RF_PHYSICALDISKSTATE_PREDICTIVE_FAILURE,
    [PD_STATE_DIAGNOSING] = RF_PHYSICALDISKSTATE_DIAGNOSING,
    [PD_STATE_INCOMPATIBLE] = RF_PHYSICALDISKSTATE_INCOMPATIBLE,
    [PD_STATE_ERASING] = RF_PHYSICALDISKSTATE_ERASING,
}
local function redfish_drive_physicaldiskstate_result(s)
    return state_map[s]
end

local PD_HOT_SPARE_NONE = 0
local PD_HOT_SPARE_GLOBAL = 1
local PD_HOT_SPARE_DEDICATED = 2
local PD_HOT_SPARE_AUTO_REPLACE = 3
local RF_HOTSPARETYPE_NONE = "None"
local RF_HOTSPARETYPE_GLOBAL = "Global"
local RF_HOTSPARETYPE_DEDICATED = "Dedicated"
local RF_HOTSPARETYPE_AUTO_REPLACE = "Auto Replace"
local function get_system_storage_drive_hotsparetype(obj)
    local v = obj.HotSpare:fetch()
    if v == PD_HOT_SPARE_NONE then
        return RF_HOTSPARETYPE_NONE
    elseif v == PD_HOT_SPARE_GLOBAL then
        return RF_HOTSPARETYPE_GLOBAL
    elseif v == PD_HOT_SPARE_DEDICATED then
        return RF_HOTSPARETYPE_DEDICATED
    elseif v == PD_HOT_SPARE_AUTO_REPLACE then
        return RF_HOTSPARETYPE_AUTO_REPLACE;
    else
        logging:error("hotsparetype value is error")
        return null
    end
end

local DRIVE_INDICATORLED_OFF = 0
local DRIVE_INDICATORLED_BLINKING = 1
local DRIVE_FAULT_OK = 0
local DRIVE_FAULT_YES = 1
local RF_INDICATORLED_OFF = "Off"
local RF_INDICATORLED_BLINKING = "Blinking"
local function get_system_storage_drive_indicatorled(obj)
    -- 定位灯状态需要两个值判断，LOCATION为0，定位关闭，
    --    location 为1时    fault 为1, 盘正在重构  定位关闭。
    --                      fault 为0，开启定位
    local indicatorled = obj.Location:fetch()
    local fault = obj.Fault:fetch()
    if ((indicatorled == DRIVE_INDICATORLED_OFF) or
        ((indicatorled == DRIVE_INDICATORLED_BLINKING) and (fault == DRIVE_FAULT_YES))) then
        return RF_INDICATORLED_OFF
    elseif ((indicatorled == DRIVE_INDICATORLED_BLINKING) and (fault == DRIVE_FAULT_OK)) then
        return RF_INDICATORLED_BLINKING
    else
        logging:error("indicatorled value error")
        return null
    end
end

local RF_PATROLSTATE_PATROLEDORNOPATROLED = "DoneOrNotPatrolled"
local RF_PATROLSTATE_PATROLLING = "Patrolling"
local function get_drive_oem_patrolstate(obj)
    local v = obj.PatrolState:fetch()
    if v == 0 then
        return RF_PATROLSTATE_PATROLEDORNOPATROLED
    elseif v == 1 then
        return RF_PATROLSTATE_PATROLLING
    end
    logging:error("The value of hdd patrolstate")
    return null
end

local SDCARD_SYNC_DONE = 0
local SDCARD_WAITTING_SYNC = 1
local SDCARD_SYNC_IN_PROGRESS = 2
local RF_REBUILDSTATE_REBUILDING = "Rebuilding"
local RF_REBUILDSTATE_REBUILDORNOREBUILD = "DoneOrNotRebuilt"
local function get_drive_oem_rebuidstate(obj)
    -- 0 重构结束或等待重构。2 正在重构
    local v = obj.RebuildState:fetch()
    if v == SDCARD_SYNC_DONE or v == SDCARD_WAITTING_SYNC then
        -- SD 卡重构完成或等待重构
        return RF_REBUILDSTATE_REBUILDORNOREBUILD
    elseif v == SDCARD_SYNC_IN_PROGRESS then
        -- SD 卡正在重构
        return RF_REBUILDSTATE_REBUILDING
    end
    logging:error("the value of rebuildstate is error")
    return null
end

local RF_DEVIES_MEDIATYPE_HDD_VALUE = 0
local RF_DEVIES_MEDIATYPE_SSD_VALUE = 1
local RF_DEVIES_MEDIATYPE_SSM_VALUE = 2
local RF_DEVIES_MEDIATYPE_HDD_NAME = "HDD"
local RF_DEVIES_MEDIATYPE_SSD_NAME = "SSD"
local RF_DEVIES_MEDIATYPE_SSM_NAME = "SSM"
local function get_drive_mediatype(obj)
    local v = obj.MediaType:fetch()
    if v == RF_DEVIES_MEDIATYPE_HDD_VALUE then
        return RF_DEVIES_MEDIATYPE_HDD_NAME
    elseif v == RF_DEVIES_MEDIATYPE_SSD_VALUE then
        return RF_DEVIES_MEDIATYPE_SSD_NAME
    elseif v == RF_DEVIES_MEDIATYPE_SSM_VALUE then
        return RF_DEVIES_MEDIATYPE_SSM_NAME
    end
end

local RFPROP_PCIEFUNCTION_PCIESWITCH_VALUE = 0xFF
local RFPROP_PCIEFUNCTION_PCIESWITCH = "PCIeSwitch"
local RFPROP_PCIEFUNCTION_PCH = "PCH"
local RFPROP_PCIEFUNCTION_PCH_VALUE = 0xFE
local RESOURCE_ID_PCIE_CARD_BELONG_TO_CPU1_AND_CPU2 = 0xFD
local RESOURCE_ID_PCIE_CARD_BELONG_TO_CPU1_AND_CPU2_STR = "CPU1,CPU2"
local function get_chassis_pciefunction_oem_associatedresource(obj)
    local resourceNum = obj.ResId:neq(0):fetch()
    if resourceNum == RFPROP_PCIEFUNCTION_PCIESWITCH_VALUE then
        return RFPROP_PCIEFUNCTION_PCIESWITCH
    elseif resourceNum == RFPROP_PCIEFUNCTION_PCH_VALUE then
        return RFPROP_PCIEFUNCTION_PCH
    elseif resourceNum == RESOURCE_ID_PCIE_CARD_BELONG_TO_CPU1_AND_CPU2 then
        return RESOURCE_ID_PCIE_CARD_BELONG_TO_CPU1_AND_CPU2_STR
    else
        return string.format("CPU%d", resourceNum)
    end
end

local function get_chassis_board_associatedrsc(obj)
    if obj == nil then
        return
    end
    local cname = redfish_get_board_class_name(obj)
    local cardObj = dal_get_specific_object_position(obj, cname):fetch()
    if cname == CLASS_PCIECARD_NAME or cname == CLASS_NETCARD_NAME or cname == CLASS_MEZZ or cname ==
        CLASS_RAIDCARD_NAME then
        return get_chassis_pciefunction_oem_associatedresource(cardObj)
    end

    -- 其他卡不支持资源归属
    logging:error("get_chassis_board_associatedrsc: invalid resource %s", cname)
    error("invalid resource")
end

local RFPROP_TYPE_MAINBOARD = "MainBoard"
local function get_mainboard_info(obj)
    local ret = {}
    ret.BoardId = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null)
    ret.Manufacturer = obj.Manufacturer:fetch()
    O.BMC:next(function(oo)
        local version = oo.CpldVersion:fetch()
        local unitNum = oo.CpldUnitNum:fetch()
        ret.CPLDVersion = string.format("%s(U%d)", version, unitNum)

        local pmeVersion = oo.PMEVer:fetch()
        local flashUnitNum = oo.FlashUnitNum:fetch()
        ret.PMEVersion = string.format("%s(U%d)", pmeVersion, flashUnitNum)

        ret.UbootVersion = oo.UbootVersion:fetch_or()
        ret.UbootBackupVer = oo.UbootBackupVer:fetch_or()
    end):fetch_or()
    ret.BiosVersion = O.Bios:next(function(oo)
        local version = oo.Version:fetch()
        local unitNum = oo.UnitNum:fetch()
        return string.format("%s(U%d)", version, unitNum)
    end):fetch_or()
    O.MainBoard:next(function(oo)
        ret.PchModel = oo.PCHModel:fetch()
    end):fetch_or()
    ret.PCBVersion = C.Fru('FruId', obj.FruId:fetch()).PcbVersion:next(rf_string_check):fetch_or(null)
    obj:ref("FruId"):next(function(fru_info)
        return O[fru_info.obj_name]
    end):next(function(oo)
        return O[oo.Elabel:fetch()]
    end):next(function(oo)
        ret.PartNumber = oo.BoardPartNumber:fetch()
        ret.SerialNumber = oo.BoardSerialNumber:fetch()
    end):fetch_or()
    ret.DeviceType = RFPROP_TYPE_MAINBOARD
    ret.BoardModel = O.Fru0Elabel.BoardProductName:fetch_or(null)
    return ret
end

local function get_cpu_state(obj)
    if obj.Presence:fetch() ~= 1 then
        return "Absent"
    end

    local cpuState = obj.FailIsolate:fetch_or(0xFF)
    local enablementStatus = obj.DisableCpuHw:catch(function()
        cpuState = 0xFF
        return 0
    end):fetch()
    cpuState = bit.bor(cpuState, enablementStatus)
    if cpuState == 0 then
        return "Enabled"
    elseif cpuState == 1 then
        return "Disabled"
    end
    return ""
end

local PROTERY_PS_INPUTMODE_VALUE_DC = 0
local PROTERY_PS_INPUTMODE_VALUE_AC = 1
local PROTERY_PS_INPUTMODE_VALUE_AC_OR_DC = 2
local function AcDcToStr(v)
    if v == PROTERY_PS_INPUTMODE_VALUE_DC then
        return "DC"
    elseif v == PROTERY_PS_INPUTMODE_VALUE_AC then
        return "AC"
    elseif v == PROTERY_PS_INPUTMODE_VALUE_AC_OR_DC then
        return "ACorDC"
    end
end

local function check_fdm_enabled()
    return C.FdmConfig[0].FdmEnable:fetch_or(false) == 1
end

local function check_fdm_pfae_report_display()
    return C.FdmWebConfig[0].FdmPfaeEnable:neq(0):next(function()
        return C.FdmWebConfig[0].FdmPfaeResultDisplay:neq(0)
    end):fetch_or(false)
end

local function get_fdm_report_json(user)
    if not file_exists(HEALTH_REPORT_JSON_PATH) then
        local regernate = 1
        local obj = C.FdmWebConfig[0]
        local ok, ret = call_method(user, obj, "FDMFruTree", gbyte(regernate))
        if not ok then
            return false, ret
        end

        -- 从下发任务到生成文件，每隔1秒检查一次，最长10秒的等待时间
        for _ = 1, 10 do
            tasks.sleep(1000)
            if file_exists(HEALTH_REPORT_JSON_PATH) then
                break
            end
        end
    end

    -- 为了安全再多等 1 秒吧
    tasks.sleep(1000)

    local f, _ = io.open(HEALTH_REPORT_JSON_PATH, "r")
    if not f then
        logging:error("open diagnose fdm health report file failed")
        return false, reply_internal_server_error(nil, "get diagnose fdm health report failed")
    end

    local ok, errInfo = flock(f, "exclusive")
    if not ok then
        f:close()
        logging:error("lock diagnose fdm health report file failed: %s", errInfo)
        return false, reply_internal_server_error(nil, "get diagnose fdm health report failed")
    end

    local data = f:read("*a")

    flock(f, "unlock")
    f:close()

    local ok, ret = pcall(json_decode, data)
    if not ok then
        logging:error("decode diagnose fdm health report file failed: %s", ret)
        return false, reply_internal_server_error()
    end

    return true, ret
end

local function get_online_timestamp(v)
    -- 老web也是读取MaintenceHistroy的第一个对象中的ReportTimeStamp作为上线时间的
    if not v.MaintenceHistroy then
        return false, "MaintenceHistroy not found."
    end

    if #v.MaintenceHistroy == 0 then
        return false, "MaintenceHistroy is empty."
    end

    local t = v.MaintenceHistroy[1]
    if not t.ReportTimeStamp then
        return false, "ReportTimeStamp not found."
    end

    if not t.ReportTimeZone then
        return false, "ReportTimeZone not found."
    end

    return true, t.ReportTimeStamp, t.ReportTimeZone
end

local GetDevTree

local function GetDevChildren(chs, depth)
    local ret = {}
    if chs and depth > 0 then
        for _, v in ipairs(chs) do
            ret[#ret + 1] = GetDevTree(v, depth)
        end
    end
    return ret
end

GetDevTree = function(v, depth)
    local alias = v.Alias or 0
    local EventReports
    if alias ~= 0 then
        EventReports = string.format("/UI/Rest/Maintenance/FDM/%d/EventReports", alias)
    end

    local DeviceInfo
    if v.RefFru and v.RefFru.RefFruObj then
        DeviceInfo = string.format("/UI/Rest/Maintenance/FDM/Device/%s", v.RefFru.RefFruObj)
    end

    local ok, TimeStamp, TimeZone = get_online_timestamp(v)
    if not ok then
        logging:error("%s : %s", v.SilkName, TimeStamp)
        TimeStamp = nil
        TimeZone = nil
    end

    return {
        SilkName = v.SilkName,
        ComponentType = v.ComponentType,
        ComponentID = v.ComponentID,
        HealthStatus = v.HealthStatus,
        Alias = alias,
        OnlineTimeStamp = TimeStamp,
        OnlineTimeZone = TimeZone,
        EventReports = EventReports,
        DeviceInfo = DeviceInfo,
        AssociateDevice = GetDevChildren(v.Children, depth - 1)
    }
end

local function get_maintence_reports_by_alias(v, alias, depth)
    if depth == 0 then
        return false
    end

    if v.Alias == alias then
        return true, v.MaintenceHistroy
    end

    if v.Children then
        for _, ch in ipairs(v.Children) do
            local ok, ret = get_maintence_reports_by_alias(ch, alias, depth - 1)
            if ok then
                return ok, ret
            end
        end
    end

    return false
end

local M = {}

function M.GetFDM(user)
    if not check_fdm_enabled() then
        return reply_not_found()
    end

    if not check_fdm_pfae_report_display() then
        return reply_not_found()
    end

    local ok, fdm = get_fdm_report_json(user)
    if not ok then
        return fdm
    end

    local DevTree = fdm.DevTree
    if DevTree and DevTree.Children then
        return {DeviceTree = GetDevChildren(DevTree.Children, MAX_RECURSION_DEPTH)}
    end
    return {}
end

function M.GetFDMEventReports(alias, user)
    if not check_fdm_enabled() then
        return reply_not_found()
    end

    if not check_fdm_pfae_report_display() then
        return reply_not_found()
    end

    local ok, fdm = get_fdm_report_json(user)
    if not ok then
        return fdm
    end

    local EventRecords = {}
    if fdm.HealthReport then
        for _, h in ipairs(fdm.HealthReport) do
            if h.Alias == alias and h.History then
                for _, v in ipairs(h.History) do
                    EventRecords[#EventRecords + 1] = {
                        ReportTimeStamp = v.ReportTimeStamp,
                        ReportTimeZone = v.ReportTimeZone,
                        SilkName = v.SilkName,
                        Level = v.Level,
                        Information = v.Information,
                        Type = v.Type,
                        EventCodeStr = v.EventCodeStr
                    }
                end
                break
            end
        end
    end

    local status, ret = get_maintence_reports_by_alias(fdm.DevTree or {}, alias, MAX_RECURSION_DEPTH)
    if status then
        for _, v in ipairs(ret) do
            EventRecords[#EventRecords + 1] = {
                ReportTimeStamp = v.ReportTimeStamp,
                ReportTimeZone = v.ReportTimeZone,
                SilkName = v.SilkName,
                ComponentType = v.ComponentType,
                ComponentID = v.ComponentID,
                SN = v.PPIN,
                Level = v.Level,
                EventCodeStr = v.EventCodeStr,
                Type = v.Type,
                Information = v.Information
            }
        end
    end

    return {EventRecords = EventRecords}
end

local function get_memory_info(obj)
    return {
        DeviceName = obj.DeviceName:fetch(),
        SerialNumber = obj.SN:fetch(),
        MemoryDeviceType = obj.Type:fetch(),
        Manufacturer = obj.Manufacturer:fetch(),
        MinVoltageMillivolt = obj.MinimumVoltage:fetch(),
        Position = obj.Location:fetch(),
        PartNumber = obj.PartNum:fetch(),
        CapacityMiB = tonumber(string.match(obj.Capacity:fetch(), "%d+")),
        DataWidthBits = obj.BitWidth:fetch(),
        OperatingSpeedMhz = tonumber(string.match(obj.ClockSpeed:fetch(), "%d+")),
        Technology = obj.Technology2:fetch(),
        RankCount = obj.Rank:neq(0):fetch_or(null)
    }
end

local function get_netcard_info(obj)
    return {
        Name = obj.ProductName:fetch(),
        Model = obj.Model:fetch(),
        Manufacturer = obj.ChipManufacturer:fetch(),
        CardManufacturer = obj.Manufacture:fetch(),
        BoardId = obj.RefComponent:next(function(RefComponent)
            return O[RefComponent].BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null)
        end):fetch_or(null),
        CardModel = obj.CardDesc:fetch(),
        PCBVersion = C.Fru('FruId', obj:ref_obj('RefComponent'):fetch_or().FruId:fetch_or()).PcbVersion:next(rf_string_check):fetch_or(null)
    }
end

local function get_riserpciecard_info(obj)
    local slotId = obj.Slot:fetch()
    local deviceName = C.Component():fold(function(comRiserCard)
        if comRiserCard.DeviceNum:eq(slotId):fetch_or() and 
            comRiserCard.BoardId:eq(obj.BoardId:fetch()):fetch_or() then
            return comRiserCard.DeviceName:fetch_or(null), false
        end
    end):fetch_or()
    return {
        Name = deviceName,
        Manufacturer = obj.Manufacturer:fetch(),
        Slot = slotId,
        PCBVersion = obj.PcbVer:next(rf_string_check):fetch_or(null),
        BoardId = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null),
        Description = obj.Type:fetch()
    }
end

local function get_pciecard_info(obj)
    local slaveCard
    local pcieCardSlot = obj.PcieCardSlot:fetch_or(0)
    local cname = obj.SlaveCard:fetch_or('')
    if cname == '' then
        slaveCard = obj
    else
        slaveCard = C(cname):fold(function(slaveCardObj)
            if slaveCardObj.Slot:fetch_or(0) == pcieCardSlot then
                return slaveCardObj, false
            end
        end):fetch_or()
    end
    local comObj = O[obj.RefComponent:fetch()]
    return {
        ProductName = slaveCard.Name:fetch_or(null),
        Manufacturer = obj.Manufacturer:neq("N/A"):fetch_or(null),
        VendorId = obj.VenderId:neq(0xffff):next(IntToHexStr):fetch_or(null),
        SubsystemVendorId = obj.SubVenderId:neq(0xffff):next(IntToHexStr):fetch_or(null),
        SubsystemId = obj.SubDeviceId:neq(0xffff):next(IntToHexStr):fetch_or(null),
        Description = obj.CardDesc:fetch(),
        PcieCardSlot = pcieCardSlot,
        DeviceId = obj.DeviceId:neq(0xffff):next(IntToHexStr):fetch_or(null),
        AssociatedResource = get_chassis_board_associatedrsc(comObj)
    }
end

local function get_hddbackplan_info(obj)
    local comObj = O[obj.RefComponent:fetch()]
    return {
        Description = obj.Type:fetch_or(),
        Manufacturer = obj.Manufacturer:fetch_or(),
        PCBVersion = obj.PcbVer:next(rf_string_check):fetch_or(null),
        CPLDVersion = obj.LogicVer:fetch_or(),
        BoardId = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null),
        Location = comObj.Location:fetch_or("") .. get_chassis_disk_backplane(comObj),
        CardNo = comObj.DeviceNum:fetch_or()
    }
end

local function TrimStr(str)
    local strBuf = str:match '^%s*(.-)%s*$'
    return strBuf
end

local function CheckStrEffective(str)
    if str == 'N/A' or str == 'NA' or str == 'UnKnown' or str == 'Undefined' or str == '' or str == nil then
        logging:error('check str effective fail.')
        return null
    else
        return str
    end
end

local function get_hdd_info(obj)
    return {
        Model = obj.ModelNumber:next(CheckStrEffective):fetch(),
        Protocol = obj.InterfaceTypeString:next(CheckStrEffective):fetch(),
        Revision = obj.FirmwareVersion:next(CheckStrEffective):fetch(),
        Manufacturer = obj.Manufacturer:next(CheckStrEffective):fetch(),
        SerialNumber = obj.SerialNumber:next(TrimStr):next(CheckStrEffective):fetch(),
        SASAddress = {obj.SASAddress1:next(CheckStrEffective):fetch(), obj.SASAddress2:next(CheckStrEffective):fetch()},
        MediaType = get_drive_mediatype(obj),
        CapacityBytes = obj.CapacityMB:next(function(capacity)
            return capacity >= 0 and capacity * 1024 * 1024 or null
        end):fetch_or(null),
        CapableSpeedGbs = redfish_system_storage_drive_speedgbs_result(obj.InterfaceSpeed:fetch()),
        NegotiatedSpeedGbs = redfish_system_storage_drive_speedgbs_result(obj.LinkSpeed:fetch()),
        TemperatureCelsius = obj.Temperature:neq(0xFF):fetch_or(null),
        PowerState = get_drive_oem_property_powerstate(obj),
        FirmwareStatus = redfish_drive_physicaldiskstate_result(obj.FirmwareStatus:fetch()),
        HotspareType = get_system_storage_drive_hotsparetype(obj),
        HoursOfPoweredUp = obj.HoursOfPoweredUp:next(function(time)
            return (time < 0xFFFF) and time or null
        end):fetch_or(null),
        IndicatorLED = get_system_storage_drive_indicatorled(obj),
        PatrolState = get_drive_oem_patrolstate(obj), -- 巡检状态
        RebuildState = get_drive_oem_rebuidstate(obj), -- 重构状态
        PredictedMediaLifeLeftPercent = obj.RemnantMediaWearout:neq(255):fetch_or(null),
        PartNum = obj.PartNum:next(CheckStrEffective):fetch(),
        AssociatedResource = null -- 参考 get_drive_oem_associated_resource
    }
end

local function get_cpu_info(obj)
    return {
        FrequencyMHz = tonumber(string.match(obj.CurrentSpeed:fetch(), "%d+")),
        Manufacturer = obj.Manufacturer:fetch(),
        TotalCores = obj.CoreCount:fetch(),
        TotalThreads = obj.ThreadCount:fetch(),
        L1CacheKiB = obj.L1Cache:fetch(),
        L2CacheKiB = obj.L2Cache:fetch(),
        L3CacheKiB = obj.L3Cache:fetch(),
        PartNumber = obj.PartNum:next(rf_string_check):fetch_or(null),
        IdentificationRegisters = obj.ProcessorID:fetch(),
        Model = utils.get_cpu_model(obj),
        SerialNumber = obj.SN:fetch(),
        OtherParameters = obj.MemoryTec:fetch(),
        State = get_cpu_state(obj),
        DeviceName = obj.DeviceName:fetch()
    }
end

local function get_onepower_info(obj)
    return {
        FirmwareVersion = obj.PsFwVer:fetch(),
        PowerSupplyType = obj.AcDc:next(AcDcToStr):fetch(),
        Manufacturer = obj.Manufacture:fetch(),
        PowerCapacityWatts = obj.PsRate:fetch(),
        Model = obj.PsType:fetch(),
        PartNumber = obj.PartNum:fetch(),
        SerialNumber = obj.SN:fetch(),
        MemberId = obj.PsId:fetch()
    }
end

local SENSOR_TYPE_FAN = 4
local function get_fan_sensor(fanId)
    return C.ThresholdSensor():fold(function(obj, acc)
        if obj.SensorType:fetch() == SENSOR_TYPE_FAN then
            local InnerName = obj.InnerName:fetch()
            local fid, FOrR = InnerName:match("^FAN(%d)_([^_]+)")
            if tonumber(fid) == fanId then
                acc[#acc + 1] = {obj = obj, FOrR = FOrR}
            end
        end
        return acc
    end, {}):fetch()
end
local FANTYPE_F_SPEED = "Fspeed"
local FANTYPE_R_SPEED = "Rspeed"
local FAN_PROPERTY_FRPM = "FRPM"
local FAN_PROPERTY_RRPM = "RRPM"
local function get_fan_speed_percent(obj, FOrR)
    local maxSpeedProp, currentSpeedProp
    if FOrR == "F" then
        maxSpeedProp = FANTYPE_F_SPEED
        currentSpeedProp = FAN_PROPERTY_FRPM
    elseif FOrR == "R" then
        maxSpeedProp = FANTYPE_R_SPEED
        currentSpeedProp = FAN_PROPERTY_RRPM
    else
        maxSpeedProp = FANTYPE_F_SPEED
        currentSpeedProp = FAN_PROPERTY_FRPM
    end
    local Model = obj.Model:fetch()
    return C.CLASSFANTYPE("Name", Model):next(function(fanTypeObj)
        local currentFanSpeed = obj[currentSpeedProp]:fetch()
        local maxFanSpeed = fanTypeObj[maxSpeedProp]:fetch()
        assert(maxFanSpeed > 0)
        return math.floor(currentFanSpeed * 100 / maxFanSpeed)
    end):fetch_or()
end

local function get_fan_info(obj)
    local speeds = {}
    local speedRatio = {}
    local sensors = get_fan_sensor(obj.FANID:fetch())
    for _, v in ipairs(sensors) do
        speeds[#speeds + 1] = tostring(v.obj.ReaddingConvert:fetch())
        speedRatio[#speedRatio + 1] = tostring(get_fan_speed_percent(obj, v.FOrR))
    end
    return {
        PartNumber = obj.PartNum:fetch(),
        Speed = table.concat(speeds, "/"),
        SpeedRatio = table.concat(speedRatio, "/"),
        Model = obj.Model:fetch(),
        Name = obj.DeviceName:fetch()
    }
end

local function get_raid_info(obj)
    local comObj = O[obj.RefComponent:fetch_or()]
    local firmwareObj = O[obj.Firmware:fetch_or()]
    return {
        ProductName = obj.ProductName:fetch_or(),
        SupportedRAIDLevels = obj.Model:fetch_or(),
        PCBVersion = obj.PcbVer:next(rf_string_check):fetch_or(),
        Location = comObj.Location:fetch_or(),
        CPLDVersion = firmwareObj.VersionString:fetch_or(),
        Manufacturer = obj.Manufacturer:fetch_or(),
        BoardId = obj.BoardId:neq(0):neq(0xffff):next(IntToHexStr):fetch_or(null),
        AssociatedResource = get_chassis_board_associatedrsc(comObj),
        CardNo = comObj.DeviceNum:fetch_or(),
        Description = obj.Type:fetch_or()
    }
end

local DEVICEINFO_CMD = {
    ["Memory"] = get_memory_info,
    ["NetCard"] = get_netcard_info,
    ["RiserPcieCard"] = get_riserpciecard_info,
    ["PcieCard"] = get_pciecard_info,
    ["HDDBackplane"] = get_hddbackplan_info,
    ["Hdd"] = get_hdd_info,
    ["Cpu"] = get_cpu_info,
    ["OnePower"] = get_onepower_info,
    ["FANClass"] = get_fan_info,
    ["Raid"] = get_raid_info
}

function M.GetFDMDeviceInfo(refObjName, _)
    if not refObjName or #refObjName > 64 then
        return reply_bad_request("Invalid param")
    end

    if not refObjName:match('^[a-zA-Z0-9_%-]+$') then
        return reply_bad_request("Invalid param")
    end

    return O[refObjName]:next(function(obj)
        local cname = class_name(obj)
        local cmd = DEVICEINFO_CMD[cname]
        if cmd then
            return cmd(obj)
        elseif obj.DeviceType:fetch_or(0) == CDTE.COMPONENT_TYPE_MAINBOARD then
            return get_mainboard_info(obj)
        end
        return {}
    end):fetch()
end

return M
