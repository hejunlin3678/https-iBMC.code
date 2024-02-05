local utils = require "utils"
local dflib = require "dflib"
local c = require "dflib.core"
local http = require "http"
local logging = require "logging"
local cfg = require 'config'
local C = dflib.class
local O = dflib.object
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local gstring = GVariant.new_string
local guint32 = GVariant.new_uint32
local call_method = utils.call_method
local reply_internal_server_error = http.reply_internal_server_error
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_bad_request = http.reply_bad_request
local reply_forbidden = http.reply_forbidden
local IsMultiplaneSupport = utils.IsMultiplaneSupport
local FN_TYPE_OUTTER_OM = cfg.FN_TYPE_OUTTER_OM

local KEY_FILE_TYPE_NOT_SUPPORTED = -3 -- 不支持该加密算法类型的密钥文件导入。
local RFERR_WRONG_PARAM = 400
local REDFISH_VOS_ERROR = -1
local NTP_PROPERTY_FORMAT_ERROR = 4

local MANAGER_NTP_E = {MANAGER_NTP_MANUAL = 1, MANAGER_NTP_DHCPV4 = 2, MANAGER_NTP_DHCPV6 = 3}
local IP_VERSION_E = {ENABLE_IPV4_ONLY = 0, ENABLE_IPV6_ONLY = 1, ENABLE_IPV4_AND_IPV6 = 2}
local errMap = {
    [KEY_FILE_TYPE_NOT_SUPPORTED] = reply_bad_request('KeyFileTypeNotSupported'),
    [NTP_PROPERTY_FORMAT_ERROR] = reply_bad_request('PropertyValueFormatError')
}

local timeZoneErrMap = {
    [KEY_FILE_TYPE_NOT_SUPPORTED] = reply_bad_request('KeyFileTypeNotSupported'),
    [NTP_PROPERTY_FORMAT_ERROR] = reply_bad_request('PropertyValueFormatError'),
    [RFERR_WRONG_PARAM] = reply_bad_request('PropertyValueNotInList')
}

local NTPModeErrMap = {
    [REDFISH_VOS_ERROR] = reply_bad_request('PropertyModificationNotSupported')
}

local M = {}

local function GetTimeZone()
    local timeZoneStr
    local now = os.time()
    local date = os.date("*t", now)
    -- 支持夏令时时区显示
    local timeZone = os.difftime(now + (date.isdst and 3600 or 0), os.time(os.date("!*t", now))) / 60
    if timeZone < 0 then
        timeZoneStr = string.format("-%.2d:%.2d", -timeZone / 60, -timeZone % 60)
    else
        timeZoneStr = string.format("+%.2d:%.2d", timeZone / 60, timeZone % 60)
    end

    return timeZoneStr
end

local function GetDHCPv4Enable()
    local EthGroup = O.EthGroup0
    if IsMultiplaneSupport() then
        local outGroupId = C.EthFunction('Functiontype', FN_TYPE_OUTTER_OM).Plane1GroupId:fetch_or()
        if outGroupId == nil then
          return nil
        end
        EthGroup =  C.EthGroup('GroupID', outGroupId)
    end
    if EthGroup.IpVersion:fetch() ~= 1 and EthGroup.IpMode:fetch() == 2 then
        return true
    else
        return false
    end
end

local function GetDHCPv6Enable()
    local EthGroup = O.EthGroup0
    if IsMultiplaneSupport() then
        local outGroupId = C.EthFunction('Functiontype', FN_TYPE_OUTTER_OM).Plane1GroupId:fetch_or()
        if outGroupId == nil then
          return nil
        end
        EthGroup =  C.EthGroup('GroupID', outGroupId)
    end
    if EthGroup.IpVersion:fetch() ~= 0 and EthGroup.Ipv6Mode:fetch() == 2 then
        return true
    else
        return false
    end
end

function M.GetNTP(user)
    local keyStatus
    local outputlist = C.NTP[0]:next(function(obj)
        local ok, ret = call_method(user, obj, "GetNTPGroupKeyStatus", errMap)
        if ok then
            return ret
        end
    end):fetch_or()
    if outputlist ~= nil then
        keyStatus = outputlist[1]
    end
    return {
        TimeZoneConfigurable = C.PRODUCT[0].TimeSrc:next(function(flag)
            if flag == 2 then
                return false
            else
                return true
            end
        end):fetch(),
        DateTimeLocalOffset = C.BMC[0].TimeZoneStr:fetch(),
        TimeZone = GetTimeZone(),
        Enabled = C.NTP[0].EnableStatus:next(function(status)
            if status == 1 then
                return true
            else
                return false
            end
        end):fetch(),
        DHCPv4Enable = GetDHCPv4Enable(),
        DHCPv6Enable = GetDHCPv6Enable(),
        AddressMode = C.NTP[0].Mode:next(function(mode)
            if mode == MANAGER_NTP_E.MANAGER_NTP_MANUAL then
                return "Static"
            elseif mode == MANAGER_NTP_E.MANAGER_NTP_DHCPV4 then
                return "IPv4"
            elseif mode == MANAGER_NTP_E.MANAGER_NTP_DHCPV6 then
                return "IPv6"
            end
        end):fetch(),
        ServerCount = C.NTP[0].NtpServerCount:fetch(),
        PreferredServer = C.NTP[0].PreferredServer:fetch(),
        AlternateServer = C.NTP[0].AlternativeServer:fetch(),
        ExtraServer = C.NTP[0].AlternativeServerArray:fetch(),
        MinPollingInterval = C.NTP[0].MinPollInterval:fetch(),
        MaxPollingInterval = C.NTP[0].MaxPollInterval:fetch(),
        AuthEnabled = C.NTP[0].AuthEnableStatus:next(function(status)
            if status == 0 then
                return false
            else
                return true
            end
        end):fetch(),
        DstEnabled = C.BMC[0].DSTEnable:next(function(status)
            if status == 1 then
                return true
            else
                return false
            end
        end):fetch(),
        KeyStatus = keyStatus
    }
end
function SetTime(DateTime, user)
    local TimeStr = {}
    local Time = {}
    
    if C.NTP[0].EnableStatus:fetch_or(0) == 1 then
        return reply_forbidden('ModifyFailedWithNtpMode')
    end

    for T in string.gmatch(DateTime, "%d+") do
        TimeStr[#TimeStr + 1] = T
    end
    
    if #TimeStr ~= 6 then
        return reply_bad_request('PropertyValueFormatError', nil, {'DateTime', DateTime})
    end

    Time.year = tonumber(TimeStr[1])
    Time.month = tonumber(TimeStr[2])
    Time.day = tonumber(TimeStr[3])
    Time.hour = tonumber(TimeStr[4])
    Time.min = tonumber(TimeStr[5])
    Time.sec = tonumber(TimeStr[6])

    local TimeStamp = os.time(Time)

    local ret = C.BoardRTC[0]:next(function(obj)
        local ok, err = call_method(user, obj, "SetRtcTime", nil, guint32(TimeStamp))
        if not ok then
            return err
        end
    end):catch(function(err)
        return reply_internal_server_error("SetRtcTime", err)
    end):fetch()
    return ret
end

function SetMinMaxPollingInterval(user, obj, MinPollingInterval, MaxPollingInterval)
    -- 如果同时设置最小轮询间隔和最大轮询间隔
    if MinPollingInterval and MaxPollingInterval then 
        if MinPollingInterval > MaxPollingInterval then -- 最大轮询间隔必须要大于最小轮询间隔
            return false, reply_bad_request('SettingPollingIntervalFailed', "Failed to set property MinPollingInterval, because the value of property MaxPollingInterval must be greater than or equal to that of property MinPollingInterval")
        end

        -- 设置最小轮询时间
        local min_ok, min_err = call_method(user, obj, "SetNtpMinPollingInterval", errMap, gbyte(MinPollingInterval))
        if not min_ok then
            return false, min_err
        end

        -- 设置最大轮询时间
        local max_ok, max_err = call_method(user, obj, "SetNtpMaxPollingInterval", errMap, gbyte(MaxPollingInterval))
        if not max_ok then
            return false, max_err
        end

        return true
    end   

    -- 如果只设置最小轮询时间
    if MinPollingInterval then 
        if MinPollingInterval > obj.MaxPollInterval:fetch_or() then
            return false, reply_bad_request('SettingPollingIntervalFailed', "Failed to set property MinPollingInterval, because the value of property MaxPollingInterval must be greater than or equal to that of property MinPollingInterval")
        end
        local ok, err = call_method(user, obj, "SetNtpMinPollingInterval", errMap,gbyte(MinPollingInterval))
        if not ok then
            return false, err
        end
    end

    -- 如果同时设置最小轮询间隔和最大轮询间隔
    if MaxPollingInterval then
        if MaxPollingInterval < obj.MinPollInterval:fetch_or() then
            return false, reply_bad_request('SettingPollingIntervalFailed', "Failed to set property MaxPollingInterval, because the value of property MaxPollingInterval must be greater than or equal to that of property MinPollingInterval")
        end
        local ok, err = call_method(user, obj, "SetNtpMaxPollingInterval", errMap,gbyte(MaxPollingInterval))
        if not ok then
            return false, err
        end
    end

    return true
end

function M.SetNTP(data, user)
    local ret
    ret = C.BMC[0]:next(function(obj)
        if data.DateTimeLocalOffset then
            local timeZoneStr = data.DateTimeLocalOffset
            local timeZone
            if timeZoneStr:sub(1, 1) == "-" or timeZoneStr:sub(1, 1) == "+" then
                timeZone = "UTC"..timeZoneStr
            elseif tonumber(timeZoneStr:sub(1, 1)) ~= nil and tonumber(timeZoneStr:sub(1, 1)) >= 0 and
                tonumber(timeZoneStr:sub(1, 1)) <= 9 then
                timeZone = "UTC+"..timeZoneStr
            else
                timeZone = timeZoneStr
            end

            local ok, err = call_method(user, obj, "SetTimeZoneStr", timeZoneErrMap, gstring(timeZone))
            if not ok then
                return err
            end
        end
        -- DstEnabled
        if data.DstEnabled ~= nil then
            local flag
            if data.DstEnabled == true then
                flag = 1
            else
                flag = 0
            end
            
            local ok, err = call_method(user, obj, "SetDSTEnable", errMap, gbyte(flag))
            if not ok then
                return err
            end
        end
    end):catch(function(err)
        return reply_internal_server_error("InternalError", err)
    end):fetch()
    if ret then
        ret:appendErrorData(M.GetNTP())
        return ret
    end

    ret = C.NTP[0]:next(function(obj)
        -- Enabled
        if data.Enabled ~= nil then
            local flag
            if data.Enabled == true then
                flag = 1
            else
                flag = 0
            end
            local ok, err = call_method(user, obj, "SetEnableStatus", errMap, gbyte(flag))
            if not ok then
                return err
            end
        end
        -- DateTime
        if data.DateTime then
            ret = SetTime(data.DateTime, user)
            if ret then
                ret:appendErrorData(M.GetNTP())
                return ret
            end
        end
        -- AddressMode
        if data.AddressMode then
            local flag
            local EthGroup = O.EthGroup0
            if IsMultiplaneSupport() then
                local outGroupId = C.EthFunction('Functiontype', FN_TYPE_OUTTER_OM).Plane1GroupId:fetch_or()
                if outGroupId == nil then
                    return nil
                end
                EthGroup = C.EthGroup('GroupID', outGroupId)
            end
            if data.AddressMode == "IPv4" or data.AddressMode == "IPv6" then
                local ipVerson = EthGroup.IpVersion:fetch_or()
                if (data.AddressMode == "IPv4" and ipVerson == IP_VERSION_E.ENABLE_IPV6_ONLY) or
                    (data.AddressMode == "IPv6" and ipVerson == IP_VERSION_E.ENABLE_IPV4_ONLY) then
                    ret = reply_bad_request("ModifyFailedWithIPVersion",
                        "Failed to modify ntp AddressMode information.")
                    return ret
                end

                if (data.AddressMode == "IPv4" and EthGroup.IpMode:fetch_or() == 1) or
                    (data.AddressMode == "IPv6" and EthGroup.Ipv6Mode:fetch_or() == 1) then
                    ret = reply_bad_request("StaticIPMode", "Failed to set the address origin")
                    return ret
                end
            end

            if data.AddressMode == "Static" then
                flag = MANAGER_NTP_E.MANAGER_NTP_MANUAL
            elseif data.AddressMode == "IPv4" then
                flag = MANAGER_NTP_E.MANAGER_NTP_DHCPV4
            elseif data.AddressMode == "IPv6" then
                flag = MANAGER_NTP_E.MANAGER_NTP_DHCPV6
            end

            local ok, err = call_method(user, obj, "SetNTPMode", NTPModeErrMap, gbyte(flag))
            if not ok then
                return err
            end
        end
        -- PreferredServer
        if data.PreferredServer then
            local ok, err = call_method(user, obj, "SetPreferredServer", errMap, gstring(data.PreferredServer))
            if not ok then
                return err
            end
        end
        -- AlternateServer
        if data.AlternateServer then
            local ok, err = call_method(user, obj, "SetAlternativeServer", errMap, gstring(data.AlternateServer))
            if not ok then
                return err
            end
        end
        -- ExtraServer
        if data.ExtraServer then
            local ok, err = call_method(user, obj, "SetAlternativeServerArray", errMap, gstring(data.ExtraServer))
            if not ok then
                return err
            end
        end

        local poll_interval_ok, res = SetMinMaxPollingInterval(user, obj, data.MinPollingInterval, data.MaxPollingInterval)
        if not poll_interval_ok then
            return res
        end

        -- AuthEnabled
        if data.AuthEnabled ~= nil then
            local flag
            if data.AuthEnabled == true then
                flag = 1
            else
                flag = 0
            end
            local ok, err = call_method(user, obj, "SetNTPAuthEnableStatus", errMap, gbyte(flag))
            if not ok then
                return err
            end
        end
    end):catch(function(err)
        return reply_internal_server_error("InternalError", err)
    end):fetch()
    if ret then
        ret:appendErrorData(M.GetNTP())
        return ret
    end
    return reply_ok_encode_json(M.GetNTP(user))
end

function M.ImportKey(formdata)
    local user = formdata.user
    if utils.CheckFormData(formdata) == false then
        logging:error('Failed to import key')
        return reply_bad_request('InvalidParam')
    end
    local filePath = utils.GetFormDataFilePath(formdata)
    if not filePath then
        return reply_bad_request("InvalidFile")
    end

    local ret = C.NTP[0]:next(function(obj)
        local ok, result = call_method(user, obj, "ImportNTPGroupKey", errMap, gstring(filePath))
        if not ok then
            logging:error(" ImportKey error ")
            return result
        end
    end):fetch()
    if ret then
        return ret
    end
    return reply_ok_encode_json()
end

return M
