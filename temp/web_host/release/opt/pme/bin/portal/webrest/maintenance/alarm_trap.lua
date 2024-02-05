local dflib = require "dflib"
local c = require "dflib.core"
local http = require "http"
local utils = require "utils"
local bit = require "bit"
local reply_ok = http.reply_ok
local reply_ok_encode_json = http.reply_ok_encode_json
local safe_call = http.safe_call
local reply_bad_request = http.reply_bad_request
local C = dflib.class
local O = dflib.object
local call_method = utils.call_method
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local guint32 = GVariant.new_uint32
local gstring = GVariant.new_string
local band = bit.band
local get_system_name = utils.get_system_name
local HasUserMgntPrivilege = utils.HasUserMgntPrivilege

local SNMP_PARAMETER_ERR = -5002
local SNMP_OUT_OF_RANGE_ERR = -5006
local SENSOR_MODULE_INVALID_IP_ADDR = -5011
local SENSOR_MODULE_INVALID_HOST_DOMAIN_NAME = -5012
local SNMP_COMMUNITY_VERSION3_ERR = 0x87
local SNMP_COMMUNITY_SPACE_FAIL = 0x86
local SNMP_COMMUNITY_LENGTH_FAIL = 0x85
local SNMP_COMMUNITY_COMPLEXITY_FAIL = 0x84
local snmpErrMap = {
    [SNMP_PARAMETER_ERR] = reply_bad_request("V3UserNameNotUsed"),
    [SNMP_COMMUNITY_VERSION3_ERR] = reply_bad_request("V3NotSetCommunityName"),
    [SNMP_COMMUNITY_SPACE_FAIL] = reply_bad_request("CommunityNameNotContainSpace"),
    [SNMP_COMMUNITY_LENGTH_FAIL] = reply_bad_request("InvalidCommunityNameLength"),
    [SNMP_COMMUNITY_COMPLEXITY_FAIL] = reply_bad_request("SimpleCommunityName"),
    [SNMP_OUT_OF_RANGE_ERR] = reply_bad_request("InvalidServerAddress"),
    [SENSOR_MODULE_INVALID_IP_ADDR] = reply_bad_request("InvalidServerAddress"),
    [SENSOR_MODULE_INVALID_HOST_DOMAIN_NAME] = reply_bad_request("InvalidServerAddress")
}

local TRAP_ID_BRDSN = 0
local TRAP_ID_ASSETTAG = 1
local TRAP_ID_HOSTNAME = 2
local RF_BRDSN_STR = "BoardSN"
local RF_PRODUCT_ASSET_TAG_STR = "ProductAssetTag"
local RF_HOST_NAME_STR = "HostName"
local server_identity_source_arr = {
    [TRAP_ID_BRDSN] = RF_BRDSN_STR,
    [TRAP_ID_ASSETTAG] = RF_PRODUCT_ASSET_TAG_STR,
    [TRAP_ID_HOSTNAME] = RF_HOST_NAME_STR
}
local NO_ACCESS = 15

local alarm_severities_arr = {None = 0x0, Normal = 0xf, Minor = 0xe, Major = 0xc, Critical = 0x8}
local function redfish_alarm_severities_change(data)
    if band(data, 0x01) == 1 then
        return "Normal"
    elseif band(data, 0x02) == 2 then
        return "Minor"
    elseif band(data, 0x04) == 4 then
        return "Major"
    elseif band(data, 0x08) == 8 then
        return "Critical"
    elseif data == 0 then
        return "None"
    end
end

local function str_to_val(t, s)
    for k, v in pairs(t) do
        if v == s then
            return k
        end
    end
    error(string.format("invalid value %s", s))
end

local trap_version_str = {[0] = "SNMPv1", [1] = "SNMPv2c", [3] = "SNMPv3"}
local trap_mode_str = {[0] = "EventCode", [1] = "OID", [2] = "PreciseAlarm"}
local PROPERTY_TRAP_ITEM_TIME = "Time" -- TRAP-ITEM配置项报文是否带时间信息
local PROPERTY_TRAP_ITEM_SEVERITY = "Severity" -- TRAP-ITEM配置项报文是否带告警等级信息
local PROPERTY_TRAP_ITEM_SENSOR_NAME = "SensorName" -- TRAP-ITEM配置项报文是否带传感器信息
local PROPERTY_TRAP_ITEM_EVENT_CODE = "EventCode" -- TRAP-ITEM配置项报文是否带事件码信息
local PROPERTY_TRAP_ITEM_EVENT_DESC = "EventDesc" -- TRAP-ITEM配置项报文是否带事件描述信息
local PROPERTY_TRAP_ITEM_EVENT_DESCRIPTION = "EventDescription" -- TRAP-ITEM获取报文是否带事件描述信息
local METHOD_TRAP_ITEM_SET_TIME_SEND = "SetTimeSend" -- TRAP-ITEM报文是否带时间信息设置方法
local METHOD_TRAP_ITEM_SET_SEVERITY_SEND = "SetSeveritySend" -- TRAP-ITEM报文是否带告警等级信息设置方法
-- TRAP-ITEM报文是否带传感器信息设置方法
local METHOD_TRAP_ITEM_SET_SENSOR_NAME_SEND = "SetSensorNameSend"
local METHOD_TRAP_ITEM_SET_EVENT_CODE_SEND = "SetEventCodeSend" -- TRAP-ITEM报文是否带事件码信息设置方法
-- TRAP-ITEM报文是否带事件描述信息设置方法
local METHOD_TRAP_ITEM_SET_EVENT_DESC_SEND = "SetEventDescSend"
local function get_item_message_content(obj)
    local content_props = {
        PROPERTY_TRAP_ITEM_TIME, PROPERTY_TRAP_ITEM_SENSOR_NAME, PROPERTY_TRAP_ITEM_SEVERITY,
        PROPERTY_TRAP_ITEM_EVENT_CODE, PROPERTY_TRAP_ITEM_EVENT_DESC
    }
    local ret = {}
    for _, v in ipairs(content_props) do
        obj[v]:eq(1):next(function()
            ret[#ret + 1] = v
        end):catch()
    end
    return ret
end
local message_content_method = {
    [PROPERTY_TRAP_ITEM_TIME] = METHOD_TRAP_ITEM_SET_TIME_SEND,
    [PROPERTY_TRAP_ITEM_SEVERITY] = METHOD_TRAP_ITEM_SET_SEVERITY_SEND,
    [PROPERTY_TRAP_ITEM_SENSOR_NAME] = METHOD_TRAP_ITEM_SET_SENSOR_NAME_SEND,
    [PROPERTY_TRAP_ITEM_EVENT_CODE] = METHOD_TRAP_ITEM_SET_EVENT_CODE_SEND,
    [PROPERTY_TRAP_ITEM_EVENT_DESCRIPTION] = METHOD_TRAP_ITEM_SET_EVENT_DESC_SEND
}

local function GetTrapServerList()
    return C.TrapItemCfg():fold(function(obj, acc)
        local ID = obj.IndexNum:fetch()
        local BobEnabled = nil
        local Enabled = obj.ItemEnable:fetch() == 1
        local IPAddress = obj.DestIpAddr:fetch()
        local Port = obj.DestIpPort:fetch()
        local MessageDelimiter = string.format("%c", obj.Separator:fetch())
        local MessageContent = get_item_message_content(obj)
        local DisplayKeywords = obj.ShowKeyWord:fetch() == 1
        local sys_name = get_system_name()

        if sys_name ~= 'RMM' then
            BobEnabled = obj.BobEnable:fetch() == 1
        end
        acc[#acc + 1] = {
            ID = ID,
            BobEnabled = BobEnabled,
            Enabled = Enabled,
            IPAddress = IPAddress,
            Port = Port,
            MessageDelimiter = MessageDelimiter,
            MessageContent = MessageContent,
            DisplayKeywords = DisplayKeywords
        }
        return acc
    end, {}):fetch()
end

local function GetAvailableUsers(user)
    return C.User():fold(function(obj, acc)
        if obj.PwdValidDays:fetch_or() == 0 then
            return acc
        end
        local userName = obj.UserName:fetch()
        if not HasUserMgntPrivilege(user) and (user.AuthType ~= 0 or user.UserName ~= userName) then
            return acc
        end
        if userName and #userName > 0 and obj.Privilege:fetch() ~= NO_ACCESS and obj.IsUserEnable:fetch_or() == 1 then
            acc[#acc + 1] = userName
        end
        return acc
    end, {}):fetch()
end

local function SetEnabled(user, obj, Enabled)
    if Enabled == nil then
        return
    end

    return safe_call(function()
        local v = Enabled and 1 or 0
        local ok, ret = call_method(user, obj, "SetTrapEnable", nil, gbyte(v))
        if not ok then
            return ret
        end
    end)
end

local function SetVersion(user, obj, Version)
    if Version == nil then
        return
    end

    return safe_call(function()
        local v = str_to_val(trap_version_str, Version)
        local ok, ret = call_method(user, obj, "SetVersion", nil, gbyte(v))
        if not ok then
            return ret
        end
    end)
end

local function SetSNMPv3User(user, obj, SNMPv3User)
    if SNMPv3User == nil then
        return
    end

    local userId = C.User("UserName", SNMPv3User):next(function(userNameObj)
        return userNameObj.UserId
    end):fetch_or(nil)
    if not userId then
        return reply_bad_request("UserNameNotExist")
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetTrapv3Userid", snmpErrMap, gbyte(userId))
        if not ok then
            return ret
        end
    end)
end

local function SetMode(user, obj, Mode)
    if Mode == nil then
        return
    end

    return safe_call(function()
        local v = str_to_val(trap_mode_str, Mode)
        local ok, ret = call_method(user, obj, "SetTrapMode", nil, gbyte(v))
        if not ok then
            return ret
        end
    end)
end

local function SetServerIdentity(user, obj, ServerIdentity)
    if ServerIdentity == nil then
        return
    end

    return safe_call(function()
        local v = str_to_val(server_identity_source_arr, ServerIdentity)
        local ok, ret = call_method(user, obj, "SetTrapIdentity", nil, gbyte(v))
        if not ok then
            return ret
        end
    end)
end

local function SetCommunityName(user, obj, CommunityName)
    if CommunityName == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetCommunityName", snmpErrMap, gstring(CommunityName))
        if not ok then
            return ret
        end
    end)
end

local function SetAlarmSeverity(user, obj, AlarmSeverity)
    if AlarmSeverity == nil then
        return
    end

    return safe_call(function()
        local v = alarm_severities_arr[AlarmSeverity]
        local ok, ret = call_method(user, obj, "SetSendSeverity", nil, gbyte(v))
        if not ok then
            return ret
        end
    end)
end

local function SetItemEnable(user, obj, Enabled)
    if Enabled == nil then
        return
    end

    return safe_call(function()
        local v = gbyte(Enabled and 1 or 0)
        local ok, ret = call_method(user, obj, "SetItemEnable", nil, v)
        if not ok then
            return ret
        end
    end)
end

local function SetBobEnabled(user, obj, Enabled)
    if Enabled == nil then
        return
    end

    return safe_call(function()
        local v = gbyte(Enabled and 1 or 0)
        local ok, ret = call_method(user, obj, "SetBobEnable", nil, v)
        if not ok then
            return ret
        end
    end)
end

local function SetDestIpAddr(user, obj, IPAddress)
    if IPAddress == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetDestIpAddr", snmpErrMap, gstring(IPAddress))
        if not ok then
            return ret
        end
    end)
end

local function SetDestIpPort(user, obj, Port)
    if Port == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetDestIpPort", function(errId)
            if errId == SNMP_OUT_OF_RANGE_ERR then
                return reply_bad_request("PortIdModificationFailed")
            end
        end, guint32(Port))
        if not ok then
            return ret
        end
    end)
end

local function SetSeparator(user, obj, MessageDelimiter)
    if MessageDelimiter == nil or #MessageDelimiter == 0 then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetSeparator", nil, gbyte(MessageDelimiter:byte()))
        if not ok then
            return ret
        end
    end)
end

local function SetMessageContent(user, obj, funcName, val)
    return safe_call(function()
        local ok, ret = call_method(user, obj, funcName, nil, gbyte(val and 1 or 0))
        if not ok then
            return ret
        end
    end)
end

local function SetDisplayKeywords(user, obj, DisplayKeywords)
    if DisplayKeywords == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetShowKeyWord", nil, gbyte(DisplayKeywords and 1 or 0))
        if not ok then
            return ret
        end
    end)
end

local function SetTrapServerList(user, result, TrapServerList)
    if TrapServerList == nil then
        return
    end

    local tmp = {}
    for _, v in ipairs(TrapServerList) do
        tmp[v.ID] = v
    end

    C.TrapItemCfg():fold(function(obj)
        local id = obj.IndexNum:fetch()
        local v = tmp[id]
        if v then
            result:join(SetItemEnable(user, obj, v.Enabled))
            result:join(SetBobEnabled(user, obj, v.BobEnabled))
            result:join(SetDestIpAddr(user, obj, v.IPAddress))
            result:join(SetDestIpPort(user, obj, v.Port))
            result:join(SetSeparator(user, obj, v.MessageDelimiter))
            if v.MessageContent then
                local tmp1 = {}
                for _, i in ipairs(v.MessageContent) do
                    tmp1[i] = true
                end
                for k, f in pairs(message_content_method) do
                    result:join(SetMessageContent(user, obj, f, tmp1[k]))
                end
            end
            result:join(SetDisplayKeywords(user, obj, v.DisplayKeywords))
        end
    end):catch()
end

local M = {}

function M.GetSNMPv3User(obj)
    return obj.Trapv3Userid:next(function(val)
        return C.User("UserId", val):next(function(userObj)
            return userObj.UserName
        end)
    end):fetch()
end

function M.GetTrapNotification(user)
    return C.TrapConfig[0]:next(function(obj)
        return {
            Enabled = obj.TrapEnable:fetch() == 1,
            Version = trap_version_str[obj.TrapVersion:fetch()],
            Mode = trap_mode_str[obj.TrapMode:fetch()],
            ServerIdentity = server_identity_source_arr[obj.TrapIdentity:fetch()],
            SNMPv3User = M.GetSNMPv3User(obj),
            AvailableUsers = GetAvailableUsers(user),
            AlarmSeverity = redfish_alarm_severities_change(obj.SendSeverity:fetch()),
            TrapServerList = GetTrapServerList()
        }
    end):fetch()
end

function M.UpdateTrapNotification(data, user)
    return C.TrapConfig[0]:next(function(obj)
        local result = reply_ok()

        result:join(SetEnabled(user, obj, data.Enabled))
        result:join(SetVersion(user, obj, data.Version))
        result:join(SetSNMPv3User(user, obj, data.SNMPv3User))
        result:join(SetMode(user, obj, data.Mode))
        result:join(SetServerIdentity(user, obj, data.ServerIdentity))
        result:join(SetCommunityName(user, obj, data.CommunityName))
        result:join(SetAlarmSeverity(user, obj, data.AlarmSeverity))
        SetTrapServerList(user, result, data.TrapServerList)

        if result:isOk() then
            return reply_ok_encode_json(M.GetTrapNotification(user))
        end
        result:appendErrorData(M.GetTrapNotification(user))
        return result
    end):fetch()
end

function M.TestTrapNotification(data, user)
    local count = dflib.object_count("SmtpItemCfg")
    if data.ID >= count or data.ID < 0 then
        return reply_bad_request("InvalidParam")
    end

    return C.TrapConfig[0]:next(function(obj)
        local ok, ret = call_method(user, obj, "SendTest", function()
            return reply_bad_request("TrapSendTestErr")
        end, gbyte(data.ID))
        if not ok then
            return ret
        end
        return reply_ok()
    end):fetch()
end

return M
