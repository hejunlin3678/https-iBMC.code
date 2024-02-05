local dflib = require 'dflib'
local c = require 'dflib.core'
local defs = require 'define'
local utils = require 'utils'
local cfg = require 'config'
local http = require 'http'
local cjson = require 'cjson'
local download = require 'download'
local bit = require 'bit'
local logging = require 'logging'
local band = bit.band
local rshift = bit.rshift
local bnot = bit.bnot
local reply_ok = http.reply_ok
local reply_bad_request = http.reply_bad_request
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_internal_server_error = http.reply_internal_server_error
local safe_call = http.safe_call
local GVariant = c.GVariant
local get_datetime = c.get_datetime
local C = dflib.class
local O = dflib.object
local class_name = dflib.class_name
local null = cjson.null
local call_method = utils.call_method
local call_remote_method2_async = utils.call_remote_method2_async
local OnOffToStr = utils.OnOffToStr
local OnOffStrToValue = utils.OnOffStrToValue
local dal_rf_get_board_type = utils.dal_rf_get_board_type
local read_history = utils.read_history
local GetChassisId = utils.GetChassisId
local GetChassisObj = utils.GetChassisObj
local GetChassisComponentInfoById = utils.GetChassisComponentInfoById
local gbyte = GVariant.new_byte
local guint16 = GVariant.new_uint16
local guint32 = GVariant.new_uint32
local gdouble = GVariant.new_double
local gstring = GVariant.new_string
local BOARD_MM = cfg.BOARD_MM
local BOARD_RM = cfg.BOARD_RM
local BOARD_BLADE = cfg.BOARD_BLADE
local BOARD_RACK = cfg.BOARD_RACK
local BOARD_SWITCH = cfg.BOARD_SWITCH
local get_system_name = utils.get_system_name
local MGMT_SOFTWARE_TYPE_EM = cfg.MGMT_SOFTWARE_TYPE_EM
local IsPangeaPacificSeries = utils.IsPangeaPacificSeries

local PROTERY_PS_INPUTMODE_VALUE_DC = 0
local PROTERY_PS_INPUTMODE_VALUE_AC = 1
local PROTERY_PS_INPUTMODE_VALUE_AC_OR_DC = 2
local SMM_PMBUS_PROTOCOL_V5 = 7

local RF_POWER_CONTROL_PSU_LOAD_BALANCE = 0
local RF_POWER_CONTROL_PSU_ACTIVE_STANDBY = 1

local KEEP_PWR_STATUS_AFTER_LEAKAGE = 0
local POWER_OFF_AFTER_LEAKAGE = 1

local RF_ERROR = -1
local PRODUCT_VERSION_V5 = 5
local RFERR_INSUFFICIENT_PRIVILEGE = 403
local COMP_CODE_STATUS_INVALID = 0xD5
local PME_SERVICE_SUPPORT = 1
local SECURITY_ENHANCE_STATUS = 1
-- 电源模块错误码定义
local ERR_ACTIVE_PS_ONT_PST = -8000 -- 至少有一个主用电源不在位
local ERR_NO_STANDBY_PS_PST = -8001 -- 没有一个备用电源在位
local ERR_ACTIVE_PS_ONT_HLS = -8002 -- 主用电源不健康
local ERR_CURRENT_POWER_HIGN = -8003 -- 当前功率超过所有主用电源额定功率值的75%
local ERR_ACTIVE_PS_NUM_NE = -8004 -- 主用电源的数量不够
local ERR_PS_NOT_SUPPORT = -8005 -- 电源不支持主备供电
local ERR_ACTIVE_STANDBY_MODE_UNSUPPORT = -8006 -- 要配置的主备供电模式不支持
local ERR_NAR_VOLT_ERROR = -8008 -- N+R电源电压不满足
local ERR_NAR_PS_NOT_SUPPORT = -8011 -- 电源不支持
local ERR_CMD_INVALID = 0xC2  -- arm带BBU环境不支持强制重启
local POWER_ON_FAILED_BY_BIOS_UP = 0x1D5  --  BIOS正在升级
local POWER_ON_FAILED_BY_VRD_UP = 0x3D5  --  VRD正在升级
local powerErrMap = {
    [ERR_ACTIVE_PS_ONT_PST] = reply_bad_request('ActivePsNotPosition'),
    [ERR_NO_STANDBY_PS_PST] = reply_bad_request('NoStandbyPsPosition'),
    [ERR_ACTIVE_PS_ONT_HLS] = reply_bad_request('ActivePsNotHealth'),
    [ERR_CURRENT_POWER_HIGN] = reply_bad_request('CurrentPowerHigh'),
    [ERR_ACTIVE_PS_NUM_NE] = reply_bad_request('ActivePsNotEnough'),
    [ERR_PS_NOT_SUPPORT] = reply_bad_request('PsNotSupport'),
    [ERR_ACTIVE_STANDBY_MODE_UNSUPPORT] = reply_bad_request('ActiveStandbyModeUnsupport'),
    [ERR_NAR_VOLT_ERROR] = reply_bad_request('VoltageOfRedundancyPsNotMatch'),
    [ERR_NAR_PS_NOT_SUPPORT] = reply_bad_request('PsNotSupportNAR'),
    [ERR_CMD_INVALID] = reply_bad_request('ActionFailedByBbuExist'),
    [POWER_ON_FAILED_BY_BIOS_UP] = reply_bad_request('ResetOperationFailed'),
    [POWER_ON_FAILED_BY_VRD_UP] = reply_bad_request('ResetOperationFailedVRD')
}

local LEAK_POLICY_ERR_MAP = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request('PropertyModificationNeedPrivilege', {'LeakStrategy'}),
    [COMP_CODE_STATUS_INVALID] = reply_bad_request('PropertyModificationNotSupported', {'LeakStrategy'})
}

local BOARD_NOT_SUPPORT_DEEP_SLEEP = 1
local PS_NOT_SUPPORT_DEEP_SLEEP = 2
local replyPMNotSupported = reply_bad_request('PropertyModificationNotSupported')
local deepSleepErrMap = {
    [BOARD_NOT_SUPPORT_DEEP_SLEEP] = replyPMNotSupported,
    [PS_NOT_SUPPORT_DEEP_SLEEP] = replyPMNotSupported
}

local LEAK_POLICY_MAP = {
    [KEEP_PWR_STATUS_AFTER_LEAKAGE] = 'ManualPowerOff',
    [POWER_OFF_AFTER_LEAKAGE] = 'AutoPowerOff'
}

local MAX_POWER_DELAY_COUNT = 1200 -- 最大上电延时时间2分钟，100ms/count
local MAX_POWER_DELAY_SECONDS = 120.0

local function GetClassAndMethod()
    local data = {}
    -- pangea Atlantic_smm
    if O.PRODUCT.MgmtSoftWareType:fetch_or(null) == 0x53 then
        data.class = C.ShelfPowerCapping()
        data.powerEnableMethod = "SetShelfPowerCappingEnable"
        data.powerLimitMethod = "SetShelfPowerCappingValue"
        -- pangea大西洋直通板涉及主从同步问题，所以必须使用remote_method
        data.callMethodName = call_remote_method2_async
    -- pangea Pacific
    elseif O.PRODUCT.MgmtSoftWareType:fetch_or(null) == 0x23 then
        data.class = C.PowerCapping()
        data.powerEnableMethod = "SetEnable"
        data.powerLimitMethod = "SetLimit"
        data.callMethodName = call_remote_method2_async
    -- other boards
    else
        data.class = C.PowerCapping()
        data.powerEnableMethod = "SetEnable"
        data.powerLimitMethod = "SetLimit"
        data.callMethodName = call_method
    end
    data.powerWorkModeMethod = "SetPlatformWorkMode"
    data.inputParam = {}
    return data
end

local function set_powercontrol_powerlimit_enable(user, obj, param)
    local val = OnOffStrToValue(param.inputParam.LimitState)
    if obj.Enable:fetch() ~= val then
        local ok, ret = param.callMethodName(user, obj, param.powerEnableMethod,
            powerErrMap, gbyte(val))
        if not ok then
            return ret
        end
    end
end

local function set_powercontrol_property_limit(user, obj, param)
    local ok, ret = param.callMethodName(user, obj, param.powerLimitMethod,
        powerErrMap, guint16(param.inputParam.LimitInWatts))
    if not ok then
        return ret
    end
end

local function AcDcToStr(v)
    if v == PROTERY_PS_INPUTMODE_VALUE_DC then
        return 'DC'
    elseif v == PROTERY_PS_INPUTMODE_VALUE_AC then
        return 'AC'
    elseif v == PROTERY_PS_INPUTMODE_VALUE_AC_OR_DC then
        return 'ACorDC'
    end
end

local function ActualActiveToStr(v)
    if v == 0 then
        return 'Active'
    elseif v == 1 then
        return 'Standby'
    end
end

-- workMode 为 0 表示负载均衡, 为 1 表示主备，为 2 表示单电源
local POWERMODE_SHARING = 'Sharing'
local POWERMODE_ACTIVE_STANDBY = 'Active/Standby'
local POWERMODE_SINGLE = 'Single'
local function WorkModelToStr(workMode)
    if workMode == 0 then
        return POWERMODE_SHARING
    elseif workMode == 1 then
        return POWERMODE_ACTIVE_STANDBY
    elseif workMode == 2 then
        return POWERMODE_SINGLE
    end
end

local function GetExpectedMode(obj, i)
    return obj.ExpectedActive:next(function(v)
        return (band(rshift(bnot(v), i - 1), 0x01) ~= 0) and 'Active' or 'Standby'
    end):fetch_or(null)
end

local function GetPowerSupplyMode(obj, work_mode)
    if work_mode == null then
        return null
    end
    if work_mode == POWERMODE_SHARING then
        return 'Shared'
    end
    if work_mode == POWERMODE_SINGLE then
        return 'Single'
    end
    return obj.ActualActive:next(ActualActiveToStr):fetch_or(null)
end

local function NormalAndRedundancyToStr(v)
    if v == 0 then
        return 'Disabled'
    else
        return C.SysPower[0].NormalAndRedundancyEnable:fetch_or(0) == 1 and 'Enabled' or 'Disabled'
    end
end

local function LimitExceptionToStr(v)
    if v == 0 then
        return 'NoAction'
    elseif v == 1 then
        return 'HardPowerOff'
    elseif v == 2 then
        return 'Reset'
    end
end

local function LimitExceptionStrToValue(v)
    if v == 'NoAction' then
        return 0
    elseif v == 'HardPowerOff' then
        return 1
    elseif v == 'Reset' then
        return 2
    end
end

local function filter_string(val)
    if #val > 0 and val ~= 'N/A' and val ~= 'UnKnown' then
        -- 去掉字符串前后的空格
        local v = val:match '^%s*(.-)%s*$'
        if #v > 0 then
            return v
        end
    end
    return null
end

local POWER_RESTORE_POLICY_ALWAYS_OFF = 0
local POWER_RESTORE_POLICY_PREVIOUS = 1
local POWER_RESTORE_POLICY_ALWAYS_ON = 2
local POWER_RESTORE_POLICY_ALWAYS_ON_RO = 0xF2
local function PowerRestorePolicyToStr(v)
    if v == POWER_RESTORE_POLICY_ALWAYS_OFF then
        return 'StayOff'
    elseif v == POWER_RESTORE_POLICY_PREVIOUS then
        return 'RestorePreviousState'
    elseif v == POWER_RESTORE_POLICY_ALWAYS_ON then
        return 'TurnOn'
    elseif v == POWER_RESTORE_POLICY_ALWAYS_ON_RO then
        return 'TurnOnRO'
    else
        error('invalid power restore policy')
    end
end

local function PowerRestorePolicyStrValue(v)
    if v == 'StayOff' then
        return POWER_RESTORE_POLICY_ALWAYS_OFF
    elseif v == 'RestorePreviousState' then
        return POWER_RESTORE_POLICY_PREVIOUS
    elseif v == 'TurnOn' then
        return POWER_RESTORE_POLICY_ALWAYS_ON
    elseif v == 'TurnOnRO' then
        return POWER_RESTORE_POLICY_ALWAYS_ON_RO
    end
end

local DELAY_MODE_SHORT = 0 -- 短延时，2s内随机延时，或按槽位延时
local DELAY_MODE_HALF = 1 -- 长延时，随即选取一半的服务器进行延时，延时时间环境变量中读取
local DELAY_MODE_ALL = 2 -- 长延时，所有服务器都进行延时
-- 随机延时模式，即该服务器可指定xx秒(1~xx秒内随机分配一个延时时间进行延时)
local DELAY_MODE_RANDOM = 3
local function DelayModeToStr(delayModel)
    if delayModel == DELAY_MODE_SHORT then
        return 'DefaultDelay'
    elseif delayModel == DELAY_MODE_HALF then
        return 'HalfDelay'
    elseif delayModel == DELAY_MODE_ALL then
        return 'FixedDelay'
    elseif delayModel == DELAY_MODE_RANDOM then
        return 'RandomDelay'
    else
        error('invalid delay mode')
    end
end

local function DelayModeStrToValue(delayModel)
    if delayModel == 'DefaultDelay' then
        return DELAY_MODE_SHORT
    elseif delayModel == 'HalfDelay' then
        return DELAY_MODE_HALF
    elseif delayModel == 'FixedDelay' then
        return DELAY_MODE_ALL
    elseif delayModel == 'RandomDelay' then
        return DELAY_MODE_RANDOM
    end
end

local function DeepSleepModeEnabled()
    return O.PMEServiceConfig.PowerDeepSleepEnable:fetch_or(0) ~= 0
end

local function PowerManagementSupported()
    return O.ProductComponent.PsNum:fetch_or(0) ~= 0
end

local function GetPowerControlDeepSleep()
    if O.PRODUCT.ProductVersionNum:fetch_or(0) < PRODUCT_VERSION_V5 then
        return nil
    end
    if not PowerManagementSupported() then
        return nil
    end
    if not DeepSleepModeEnabled() then
        return nil
    end
    return O.syspower.DeepSleepModeEnable:next(OnOffToStr):fetch_or(null)
end

local function PowerActiveStandbyEnabled()
    return O.PMEServiceConfig.PowerActiveStandbyEnable:fetch_or(0) ~= 0
end

local function GetPresense(obj)
    return obj.Presence:fetch_or(0)
end

local function GetState(obj)
    local presense = GetPresense(obj)
    if presense == 0 then
        return 'Absent'
    end
    local state = obj.ActualActive:fetch_or(0)
    return state == 0 and 'Enabled' or 'StandbySpare'
end

local function GetLeakPolicy(policy)
    return LEAK_POLICY_MAP[policy] or null
end

local function SetLeakStrategy(user, data)
    return safe_call(function()
        if C.PRODUCT[0].LeakDetectSupport:fetch_or() ~= 1 then
            return reply_bad_request('PropertyModificationNotSupported', {'LeakStrategy'})
        end
        return O.Cooling:next(function(cool_obj)
            local input_args = {gbyte((data.LeakStrategy == 'ManualPowerOff') and 0 or 1)}
            local ok, ret =
                call_method(user, cool_obj, 'SetLeakageAction', LEAK_POLICY_ERR_MAP, input_args)
            if not ok then
                return ret
            end
        end):fetch_or(reply_internal_server_error(nil, 'can not set LeakStrategy'))
    end)
end

local function GetRackPowerconsumedwatts()
    local PowerConsumedWatts = C.SysPower():fold(function(obj, acc)
        return acc + obj.Power:fetch_or(0)
    end, 0):fetch_or()
    return PowerConsumedWatts
end

local function GetPsSupplyChannel(obj)
    local powersupplychannel = obj.PowerSupplyChannel:fetch()
    if powersupplychannel == 0 then
        return 'MainCircuit'
    else
        return 'BackupCircuit'
    end
end

local M = {}

function M.GetPowerSupply()
    return O.ProductComponent.PsNum:next(function(psNum)
        local supplyList = {}
        local expectedSupplyList = {}
        local sysPower = O.syspower
        local powerconsumedwatts = null
        local expectedWorkMode = sysPower.ExpectedMode:next(WorkModelToStr):fetch_or(null)
        local powerActiveStandbyEnabled = PowerActiveStandbyEnabled()
        local workMode = sysPower.ActualMode:next(WorkModelToStr):fetch_or(null)
        local deepSleep = GetPowerControlDeepSleep()
        local nAndR = C.PMEServiceConfig[0].NormalAndRedundancySupport:next(NormalAndRedundancyToStr):fetch_or(null)
        local sys_name = get_system_name()
        if sys_name == 'RMM' then
            powerconsumedwatts = GetRackPowerconsumedwatts()
        end

        for i = 1, psNum do
            local ps_i = i
            if IsPangeaPacificSeries() then
                ps_i = i - 1
            end
            local powerName = string.format('PSU%d', ps_i)
            if powerActiveStandbyEnabled and expectedWorkMode == POWERMODE_ACTIVE_STANDBY then
                expectedSupplyList[#expectedSupplyList + 1] =
                    {Name = powerName, Mode = GetExpectedMode(sysPower, i)}
            else
                expectedSupplyList[#expectedSupplyList + 1] = {Name = powerName, Mode = null}
            end
            C.OnePower('DeviceName', powerName):next(function(obj)
                return {
                    PowerSupplyChannel = GetPsSupplyChannel(obj),
                    State = GetState(obj),
                    PowerSupplyType = obj.AcDc:next(AcDcToStr):fetch_or(null),
                    Name = powerName,
                    LineInputVoltage = obj.Vin:fetch_or(null),
                    PowerCapacityWatts = obj.PsRate:fetch_or(null),
                    Model = obj.PsType:next(filter_string):fetch_or(null),
                    Mode = GetPowerSupplyMode(obj, workMode),
                    FirmwareVersion = obj.PsVer:next(filter_string):neq(null):catch(
                        function()
                            return obj.PsFwVer:next(filter_string)
                        end):fetch_or(null),
                    SerialNumber = obj.SN:next(filter_string):fetch_or(null),
                    Manufacturer = obj.Manufacture:next(filter_string):fetch_or(null),
                    PartNumber = obj.PartNum:next(filter_string):fetch_or(null),
                    OutputVoltage = obj.Vout:fetch_or(null),
                    InputWatts = obj.Power:next(function(v)
                        return math.floor(v + 0.5)
                    end):fetch_or(null)
                }
            end):catch(function()
                return {
                    PowerSupplyChannel = null,
                    PowerSupplyType = null,
                    Name = powerName,
                    LineInputVoltage = null,
                    PowerCapacityWatts = null,
                    Model = null,
                    Mode = null,
                    FirmwareVersion = null,
                    SerialNumber = null,
                    Manufacturer = null,
                    PartNumber = null,
                    OutputVoltage = null,
                    InputWatts = null
                }
            end):next(function(v)
                supplyList[#supplyList + 1] = v
            end)
        end
        return {
            ExpectedSupplyList = expectedSupplyList,
            SupplyList = supplyList,
            PowerMode = powerActiveStandbyEnabled and expectedWorkMode or nil,
            NormalAndRedundancy = nAndR,
            DeepSleep = deepSleep,
            PowerConsumedWatts = powerconsumedwatts
        }
    end):fetch()
end

function M.UpdatePowerSupply(data, user)
    -- 不输入PowerMode，只输入SupplyList当做主备切换
    if not data.PowerMode and data.SupplyList then
        data.PowerMode = POWERMODE_ACTIVE_STANDBY
    end
    local powerClass = GetClassAndMethod()

    local result = reply_ok()
    if data.PowerMode then
        result:join(safe_call(function()
            -- 判断是否支持电源管理
            if O.ProductComponent.PsNum:fetch_or(0) == 0 then
                return reply_bad_request('PropertyModificationNotSupported', 'Property Modification Not Supported')
            end

            -- 判断当前系统主备设置功能是否disabled
            local enabled = O.PMEServiceConfig.PowerActiveStandbyEnable:neq(0):fetch_or()
            if enabled == nil then
                return reply_bad_request('ActiveStandbyDisabled', 'ActiveStandby Disabled')
            end

            if data.PowerMode == POWERMODE_ACTIVE_STANDBY then
                local supplyList = data.SupplyList

                -- 主备模式，至少需要指定一个主用电源
                if not supplyList or #supplyList == 0 then
                    return reply_bad_request('ActivePsNecessary', 'Active Ps Necessary')
                end

                local nar = C.SysPower[0].NormalAndRedundancyEnable:fetch_or(0)
                local input_list = {gbyte(RF_POWER_CONTROL_PSU_ACTIVE_STANDBY), gbyte(nar)}
                for _, supply in ipairs(supplyList) do
                    local id = supply.Name:match('^PSU(%d+)$')
                    if not id then
                        return reply_bad_request('InvalidPSUId', 'Invalid PSU Id')
                    end
                    if supply.Mode == 'Active' then
                        if IsPangeaPacificSeries() then
                            input_list[#input_list + 1] = gbyte(tonumber(id))
                        else
                            input_list[#input_list + 1] = gbyte(tonumber(id) - 1)
                        end
                    end
                end

                local ok, ret = powerClass.callMethodName(user, O.syspower, powerClass.powerWorkModeMethod, powerErrMap, input_list)
                if not ok then
                    return ret
                end
            elseif data.PowerMode == POWERMODE_SHARING then
                local nar = C.SysPower[0].NormalAndRedundancyEnable:fetch_or(0)
                local input_list = {gbyte(RF_POWER_CONTROL_PSU_LOAD_BALANCE), gbyte(nar)}
                local ok, ret = powerClass.callMethodName(user, O.syspower, powerClass.powerWorkModeMethod, powerErrMap, input_list)
                if not ok then
                    return ret
                end
            end
        end))
    end

    if data.DeepSleep then
        result:join(safe_call(function()
            local v = gbyte(OnOffStrToValue(data.DeepSleep))
            local ok, ret = call_method(user, O.syspower, 'SetDeepSleepMode', deepSleepErrMap, v)
            if not ok then
                return ret
            end
        end))
    end

    if result:isOk() then
        return reply_ok_encode_json(M.GetPowerSupply())
    end
    result:appendErrorData(M.GetPowerSupply())
    return result
end

local function GetPowerLimitExceptionSupported(obj)
    local supportd = obj.FailActionSupport:fetch_or(nil)
    if supportd == nil then
        return null
    end
    return supportd ~= 0
end

local function GetLimitState()
    if O.PRODUCT.MgmtSoftWareType:fetch_or(null) == 0x53 then
        return C.ShelfPowerCapping[0].Enable:fetch_or(0)
    end
    return C.PowerCapping[0].Enable:fetch_or(0)
end

local function GetLimitInWatts()
    if O.PRODUCT.MgmtSoftWareType:fetch_or(null) == 0x53 then
        return (GetLimitState() == 1) and C.ShelfPowerCapping[0].Value:fetch() or null
    end
    return (GetLimitState() == 1) and C.PowerCapping[0].LimitValue:fetch() or null
end

local function isSupportLimitStatus()
    local versionNum = C.PRODUCT[0].ProductVersionNum:fetch_or(nil)
    local serverType = C.PRODUCT[0].MgmtSoftWareType:fetch_or(nil)
    if versionNum > 5 and serverType == 16 then
        return true
    end
    return false
end
local function GetPowerPowerLimitStatus()
    local supportLimitStatus = isSupportLimitStatus();
    if supportLimitStatus == false then
        return null    
    end
    local limitStatus = { "NodePowerLimit",
                          "MSPP",
                          "ChassisPowerLimitAcitve",
                          "ChassisPowerLimitInactive"
    } 
    local shelfEnable = C.ShelfPowerCapping[0].Enable:fetch_or(nil)
    local shelfActiveState = C.ShelfPowerCapping[0].ActiveState:fetch_or(nil)
    local MSPPEnable = C.MSPP[0].Enable:fetch_or(nil)

    if shelfEnable == 1 and shelfActiveState == 0 then
        return limitStatus[4]
    end
    if shelfEnable == 1 and shelfActiveState == 1 then
        return limitStatus[3]
    end  
    if MSPPEnable == 1 then
        return limitStatus[2]
    end
    return limitStatus[1]
end

local function GetLimitState()
    if O.PRODUCT.MgmtSoftWareType:fetch_or(null) == 0x53 then
        return C.ShelfPowerCapping[0].Enable:fetch_or(0)
    end
    return C.PowerCapping[0].Enable:fetch_or(0)
end

local function GetLimitInWatts()
    if O.PRODUCT.MgmtSoftWareType:fetch_or(null) == 0x53 then
        return (GetLimitState() == 1) and C.ShelfPowerCapping[0].Value:fetch() or null
    end
    return (GetLimitState() == 1) and C.PowerCapping[0].LimitValue:fetch() or null
end

local function GetPowerLimit()
    return C.PowerCapping[0]:next(function(obj)
        local LimitInWatts
        local LimitException
        local enabled = GetLimitState()
        if obj.ManualSetEnable:fetch_or(0) == 1 then
            LimitInWatts = GetLimitInWatts()
            LimitException = LimitExceptionToStr(obj.FailAction:fetch())
        end
        local MSPPEnabled = C.MSPP[0].Enable:fetch_or()
        return {
            LimitState = OnOffToStr(enabled),
            LimitInWatts = LimitInWatts,
            PowerLimitExceptionSupported = GetPowerLimitExceptionSupported(obj),
            LimitException = LimitException,
            MinLimitInWatts = obj.BaseValue:fetch_or(),
            MaxLimitInWatts = obj.TopValue:fetch_or(),
            MSPPEnabled = not MSPPEnabled and null or MSPPEnabled == 1,
            PowerLimitStatus = GetPowerPowerLimitStatus()
        }
    end):fetch_or(null)
end

local function GetHighPowerThresholdWatts()
    return O.syspower.HighPowerThresholdWatts:next(math.ceil):fetch_or(null)
end

local function GetHistory()
    return read_history('ps_web_view.dat', function(fields)
        return {
            Time = fields[1],
            PowerWatts = fields[2],
            PowerAverageWatts = fields[3],
            PowerPeakWatts = fields[4]
        }
    end)
end

function M.GetPower()
    return C.SysPower():fold(function(sysPowerObj, acc)
        if not sysPowerObj then
            return acc, false
        end

        -- 判断是否泰山系列则不支持CPU内存功耗查询
        local CurrentCPUPowerWatts, CurrentMemoryPowerWatts
        C.PMEServiceConfig[0].ARMEnable:eq(0):next(function()
            CurrentCPUPowerWatts = C.MeInfo[0].CpuCurPower:fetch_or()
            CurrentMemoryPowerWatts = C.MeInfo[0].MemCurPower:fetch_or()
        end):catch(function()
            CurrentCPUPowerWatts = nil
            CurrentMemoryPowerWatts = nil
        end)

        acc.Metrics[#acc.Metrics + 1] = {
            StatisticsCollected = sysPowerObj.RecordBeginTime:next(get_datetime):fetch_or(null),
            PowerConsumedWatts = sysPowerObj.Power:fetch(),
            CurrentCPUPowerWatts = CurrentCPUPowerWatts,
            CurrentMemoryPowerWatts = CurrentMemoryPowerWatts,
            MaxConsumedWatts = sysPowerObj.PeakValue:fetch(),
            MaxConsumedOccurred = sysPowerObj.PeakTime:next(get_datetime):fetch_or(null),
            AverageConsumedWatts = sysPowerObj.AveragePower:fetch(),
            TotalConsumedPowerkWh = sysPowerObj.PowerConsumption:fetch()
        }

        return acc
    end, {Metrics = {}}):next(function(ret)
        -- 支持功率封顶功能时才展示以下内容
        ret.PowerLimit = GetPowerLimit()
        ret.HighPowerThresholdWatts = GetHighPowerThresholdWatts()
        ret.History = GetHistory()
        return ret
    end):fetch_or({
        PowerLimit = GetPowerLimit(),
        HighPowerThresholdWatts = GetHighPowerThresholdWatts(),
        History = GetHistory()
    })
end

local MINIMUM_ALLOWABLE_POWER = 1
function M.UpdatePower(user, data)
    local result = reply_ok()
    local powerClass = GetClassAndMethod()
    if data.PowerLimit then
        powerClass.inputParam = data.PowerLimit
        result:join(safe_call(function()
            return powerClass.class:fold(function(obj)
                -- 设置 powerlimit disable
                if powerClass.inputParam.LimitState then
                    local reply = set_powercontrol_powerlimit_enable(user, obj, powerClass)
                    if reply then
                        return reply, false
                    end
                end

                if powerClass.inputParam.LimitInWatts then 
                    if OnOffToStr(obj.Enable:fetch_or(0)) == 'Off' then
                        return reply_bad_request(nil, 'invalid LimitInWatts value'), false
                    end

                    local reply = set_powercontrol_property_limit(user, obj, powerClass)
                    if reply then
                        return reply, false
                    end
                end

                if powerClass.inputParam.LimitException then
                    local v = LimitExceptionStrToValue(powerClass.inputParam.LimitException)
                    local ok, ret = call_method(user, obj, 'SetFailAction', powerErrMap, gbyte(v))
                    if not ok then
                        return ret, false
                    end
                end
            end):next(function(err)
                return err or reply_ok()
            end):fetch()
        end))
    end

    if data.HighPowerThresholdWatts then
        local threshold = data.HighPowerThresholdWatts
        result:join(safe_call(function()
            return C.PowerCapping[0]:next(function(obj)
                local topVal = obj.TopValue:fetch()
                if threshold > topVal or threshold < MINIMUM_ALLOWABLE_POWER then
                    return reply_bad_request('PropertyValueOutOfRange', {threshold, topVal})
                end
                return C.SysPower[0]:next(function(sysPowerObj)
                    local ok, ret = call_method(user, sysPowerObj, 'SetHighPowerThreshold', powerErrMap,
                        gdouble(threshold))
                    if not ok then
                        return ret
                    end
                end)
            end):fetch()
        end))
    end

    if result:isOk() then
        return reply_ok_encode_json(M.GetPower())
    end
    result:appendErrorData(M.GetPower())
    return result
end

function M.ResetPowerStatistics(user)
    return C.SysPower[0]:next(function(obj)
        local ok, ret = call_method(user, obj, 'Reset', powerErrMap)
        if not ok then
            return ret
        end
        return reply_ok()
    end):fetch()
end

function M.DeletePowerHistory(user)
    return C.SysPower[0]:next(function(obj)
        local ok, ret = call_method(user, obj, 'ClearRecord', powerErrMap)
        if not ok then
            return ret
        end
        return reply_ok()
    end):fetch()
end

function M.GetPowerHistory(user)
    local path
    return C.SysPower[0]:next(function(obj)
        local collect_cb = function(filePath)
            path = filePath
            local ok, ret = utils.call_method_async(user, obj, 'CollectHistoryData', powerErrMap,
                {gstring(filePath)})
            if not ok then
                return ret
            end
            return reply_ok()
        end

        local progress_cb = function()
            local progress = C.SysPower[0].CollectHistoryDataStatus:fetch()
            if progress == 100 then
                if not utils.dal_set_file_owner(path, cfg.REDFISH_USER_UID, cfg.APPS_USER_GID) then
                    error('chown history.csv failed')
                end
                if not utils.dal_set_file_mode(path, cfg.FILE_MODE_600) then
                    error('chmod history.csv failed')
                end
            end
            return progress
        end

        return download(user, 'history.csv', 'power history data', defs.DOWNLOAD_POWER_HISTORY, collect_cb,
            progress_cb)
    end):catch(function(err)
        return reply_bad_request('ActionNotSupported', err)
    end):fetch()
end

local board_type_to_chassis_type = {
    [BOARD_RACK] = 'Rack',
    [BOARD_BLADE] = 'Blade',
    [BOARD_MM] = 'Module'
}

local function GetMainboardChassisType()
    return dal_rf_get_board_type():next(function(boardType)
        return board_type_to_chassis_type[boardType] or 'Other'
    end):fetch()
end

local function GetChassisType()
    local chassis_id = GetChassisId()
    if not chassis_id then
        return null
    end
    local board_type = dal_rf_get_board_type():fetch()
    local chassis_obj = GetChassisObj(chassis_id)
    if not chassis_obj or (board_type == BOARD_RM and class_name(chassis_obj) == 'UnitInfo')  then
        return null
    end
    local chassis_component_info = GetChassisComponentInfoById(chassis_id)
    return chassis_component_info and chassis_component_info[4] or GetMainboardChassisType()
end

local BASE_FRU_ID = cfg.FRU_DEFS.BASE_FRU_ID
local FC_FRU_ID = cfg.FRU_DEFS.FC_FRU_ID
function M.GetPowerControl()
    return C.Payload[0]:next(function(obj)
        local PwrButtonLock = obj.PwrButtonLock:fetch()
        local delayCount = obj.PowerDelayCount:fetch()
        local delaySeconds = 0
        if delayCount > MAX_POWER_DELAY_COUNT then
            delaySeconds = MAX_POWER_DELAY_SECONDS
        else
            delaySeconds = delayCount / 10
        end

        local powerState
        if dal_rf_get_board_type():fetch() == BOARD_SWITCH then
            for i = BASE_FRU_ID, FC_FRU_ID do
                powerState = C:FruPayload('FruID', i):next(
                    function(fruPayloadObj)
                        return fruPayloadObj.PowerState
                    end):fetch_or()
                if powerState then
                    break
                end
            end
        else
            powerState = obj.ChassisPowerState:fetch()
        end
        local leakdet_support = C.PRODUCT[0].LeakDetectSupport:fetch_or() == 1
        return {
            PowerState = OnOffToStr(powerState),
            PanelPowerButtonEnabled = OnOffToStr(PwrButtonLock),
            PowerOnStrategy = PowerRestorePolicyToStr(obj.PowerRestorePolicy:fetch()),
            PowerRestoreDelay = {Mode = DelayModeToStr(obj.PowerDelayMode:fetch()), Seconds = delaySeconds},
            PowerOffAfterTimeout = {
                Enabled = OnOffToStr(obj.PowerOffTimeoutEN:fetch()),
                TimeoutSeconds = obj.PowerOffTimeout:fetch(),
                DefaultSeconds = obj.PowerOffTmDefault:fetch(),
                MinSeconds = obj.PowerOffTmMin:fetch_or(0),
                MaxSeconds = obj.PowerOffTmMax:fetch_or(0)
            },
            LeakDetectionSupport = leakdet_support,
            LeakStrategy = leakdet_support and C.Cooling[0].Policy:next(GetLeakPolicy):fetch_or(null) or nil,
            ChassisType = GetChassisType()
        }
    end):fetch()
end

local function SystemLockDownSupport()
    return C.PMEServiceConfig[0]:next(function(obj)
        return obj.SystemLockDownSupport
    end):eq(PME_SERVICE_SUPPORT):fetch_or(false)
end
local function GetSystemLockDownStatus()
    return C.SecurityEnhance[0]:next(function(obj)
        return obj.SystemLockDownStatus
    end):eq(SECURITY_ENHANCE_STATUS):fetch_or(false)
end
local function SystemLockdownCheck()
    if SystemLockDownSupport() and GetSystemLockDownStatus() then
        return true 
    end
    return false
end

function M.UpdatePowerControl(user, data)
    -- 系统锁定检查
    if SystemLockdownCheck() then
        return reply_bad_request('SystemLockdownForbid')
    end
    return C.Payload[0]:next(function(obj)
        local result = reply_ok()

        -- 设置通电开机策略
        if data.PowerOnStrategy then
            result:join(safe_call(function()
                local powerOnPolicy = PowerRestorePolicyStrValue(data.PowerOnStrategy)
                local ok, ret = call_method(user, obj, 'SetPowerRestorePolicy', powerErrMap, gbyte(powerOnPolicy))
                if not ok then
                    return ret
                end
            end))
        end

        local PowerRestoreDelay = data.PowerRestoreDelay
        if PowerRestoreDelay then
            -- 设置上电延迟模式
            if PowerRestoreDelay.Mode then
                result:join(safe_call(function()
                    local delayMode = DelayModeStrToValue(PowerRestoreDelay.Mode)
                    local ok, ret = call_method(user, obj, 'SetPowerDelayMode', powerErrMap, gbyte(delayMode))
                    if not ok then
                        return ret
                    end
                end))
            end

            local seconds = PowerRestoreDelay.Seconds
            -- 设置上电延迟时间
            if seconds then
                result:join(safe_call(function()
                    if type(seconds) ~= 'number' or tostring(seconds):match('%.%d%d+') then
                        local prop = 'Oem/Huawei/PowerRestoreDelaySeconds'
                        return reply_bad_request('PropertyValueFormatError')
                    end
                    if seconds < 0 or seconds > 120 then 
                        return reply_bad_request('PropertyValueFormatError')
                    end
                    local delaySeconds = seconds * 10
                    local ok, ret =
                        call_method(user, obj, 'SetPowerDelayCount', powerErrMap, guint32(delaySeconds))
                    if not ok then
                        return ret
                    end
                end))
            end
        end

        -- 设置面板电源按钮使能
        if data.PanelPowerButtonEnabled then
            result:join(safe_call(function()
                local powerButton = OnOffStrToValue(data.PanelPowerButtonEnabled)
                local ok, ret = call_method(user, obj, 'SetPowerButtonLock', powerErrMap, gbyte(powerButton))
                if not ok then
                    return ret
                end
            end))
        end

        local PowerOffAfterTimeout = data.PowerOffAfterTimeout
        if PowerOffAfterTimeout then
            -- 设置下电时限开关状态
            if PowerOffAfterTimeout.Enabled then
                result:join(safe_call(function()
                    local enabled = OnOffStrToValue(PowerOffAfterTimeout.Enabled)
                    local ok, ret = call_method(user, obj, 'SetPowerOffTimeoutEN', powerErrMap, guint32(enabled))
                    if not ok then
                        return ret
                    end
                end))
            end

            -- 设置安全下电超时时间
            if PowerOffAfterTimeout.TimeoutSeconds then
                result:join(safe_call(function()
                    local timeoutSeconds = PowerOffAfterTimeout.TimeoutSeconds
                    local ok, ret = call_method(user, obj, 'SetPowerOffTimeout', function(errId)
                        if errId == RF_ERROR then
                            local maxTime = obj.PowerOffTmMax:fetch()
                            local minTime = obj.PowerOffTmMin:fetch()
                            return reply_bad_request('PropertyValueOutOfRange',
                                string.format('invalid param %d %d', maxTime, minTime))
                        end
                        return powerErrMap[errId]
                    end, guint32(timeoutSeconds))
                    if not ok then
                        return ret
                    end
                end))
            end
        end

        -- 设置漏液检测板下电策略
        if data.LeakStrategy then
            result:join(SetLeakStrategy(user, data))
        end

        if result:isOk() then
            return reply_ok_encode_json(M.GetPowerControl())
        end
        result:appendErrorData(M.GetPowerControl())
        return result
    end):fetch()
end

local powerMethodMap = {
    On = {'Payload', 'PowerOn'},
    ForceOff = {'Payload', 'ForcePowerOff'},
    GracefulShutdown = {'Payload', 'PowerOff'},
    ForceRestart = {'FruPayload', 'ColdReset'},
    ForcePowerCycle = {'FruPayload', 'GracefulReboot'},
    Nmi = {'Payload', 'DiagInterrupt'}
}
function M.SetPowerOperateType(user, data)
    local info = powerMethodMap[data.OperateType]
    if not info then
        return reply_internal_server_error(nil, 'invalid power operator type')
    end
    local className = info[1]
    local methodName = info[2]
    return C[className][0]:next(function(obj)
        local ok, ret = call_method(user, obj, methodName, powerErrMap)
        if not ok then
            return ret
        end
        return reply_ok()
    end):fetch_or(reply_bad_request("ResetOperationNotAllowed"))
end

M.filter_string = filter_string

M.AcDcToStr = AcDcToStr

M.GetState = GetState

return M
