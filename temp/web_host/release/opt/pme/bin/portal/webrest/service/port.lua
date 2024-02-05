local cfg = require "config"
local utils = require "utils"
local dflib = require "dflib"
local c = require "dflib.core"
local http = require "http"
local C = dflib.class
local O = dflib.object
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local guint16 = GVariant.new_uint16
local gint32 = GVariant.new_int32
local call_method = utils.call_method
local HasSecurityMgntPrivilege = utils.HasSecurityMgntPrivilege
local HasUserMgntPrivilege = utils.HasUserMgntPrivilege
local reply_internal_server_error = http.reply_internal_server_error
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_bad_request = http.reply_bad_request
local reply = http.reply
local OperateLog = utils.OperateLog
local get_system_name = utils.get_system_name
local IsMgmtBoard = utils.IsMgmtBoard

-- 端口服务模块错误码定义
local RF_ERROR = -1
local RFERR_INSUFFICIENT_PRIVILEGE = 403
local COMP_CODE_INVALID_CMD = 0xC1
local COMP_CODE_OUTOF_RANGE = 0xC9

local portErrMap = {
    [COMP_CODE_INVALID_CMD] = reply_bad_request("PropertyModificationNotSupported",
        "The property modification failed because it is not supported."),
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("PropertyModificationNeedPrivilege",
        "The property cannot be modified because of insufficient permission."),
    [COMP_CODE_OUTOF_RANGE] = reply_bad_request("PortIdModificationFailed",
        "The value for the property is not in the range of 1 to 65535 or already exists."),
    [RF_ERROR] = reply_bad_request("PortIdModificationFailed",
        "The value for the property is not in the range of 1 to 65535 or already exists.")
}

local stateErrMap = {
    [COMP_CODE_INVALID_CMD] = reply_bad_request("PropertyModificationNotSupported",
        "The property modification failed because it is not supported."),
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("PropertyModificationNeedPrivilege",
        "The property cannot be modified because of insufficient permission."),
    [COMP_CODE_OUTOF_RANGE] = reply_bad_request("PropertyValueNotInList",
        "The value for the property is not in the list of acceptable values."),
    [RF_ERROR] = reply_bad_request("PropertyValueNotInList",
        "The value for the property is not in the list of acceptable values.")
}

local M = {}
local SnmpSupported = C.PMEServiceConfig[0].SnmpdEnable:next(utils.ValueToBool):fetch_or(false)

local function GetHmmSshNatPortConfig()
    local enabled = C.PortConfig("ServiceID", cfg.SERVICE_CONFIGURATION_HMMSSHNAT).State:eq(1):next(
        function()
            return true
        end):fetch_or(false)
    local port = C.PortConfig("ServiceID", cfg.SERVICE_CONFIGURATION_HMMSSHNAT).Port:next(
        function(array)
            return array[1]
        end):fetch_or()
    if port == nil and not enabled then
        return nil
    end
    return {Enabled = enabled, Port = port}
end

local function GetPortState(objName, stateName)
    local system_name = get_system_name()
    if system_name == 'RMM' then
        return nil
    end
    return {
        Port = C[objName][0].Port:fetch_or(),
        Enabled = C[objName][0][stateName]:eq(1):next(function()
            return true
        end):fetch_or(false)
    }
end

function M.GetPortConfig(user)
    local RmcpConfig = C.RmcpConfig[0]
    local result = {
        SSH = {    
            Port = C.PortConfig("ServiceID", cfg.SERVICE_CONFIGURATION_SSH).Port:next(
                function(array)
                    return array[1]
                end):fetch_or(),
            Enabled = C.PortConfig("ServiceID", cfg.SERVICE_CONFIGURATION_SSH).State:eq(1):next(
                function()
                    return true
                end):fetch_or(false)
        },
        HTTP = {
            Port = C.PortConfig("ServiceID", cfg.SERVICE_CONFIGURATION_WEB_HTTP).Port:next(
                function(array)
                    return array[1]
                end):fetch_or(),
            Enabled = C.PortConfig("ServiceID", cfg.SERVICE_CONFIGURATION_WEB_HTTP).State:eq(1):next(
                function()
                    return true
                end):fetch_or(false)
        },
        HTTPS = {
            Port = C.PortConfig("ServiceID", cfg.SERVICE_CONFIGURATION_WEB_HTTPS).Port:next(
                function(array)
                    return array[1]
                end):fetch_or(),
            Enabled = C.PortConfig("ServiceID", cfg.SERVICE_CONFIGURATION_WEB_HTTPS).State:eq(1):next(
                function()
                    return true
                end):fetch_or(false)
        },
        IPMILANRMCP = {
            Port = RmcpConfig.Port1:fetch_or(),
            SparePort = RmcpConfig.Port2:fetch_or(),
            Enabled = RmcpConfig.LanState:eq(1):next(function()
                return true
            end):fetch_or(false)
        },
        IPMILANRMCPPlus = {
            Enabled = RmcpConfig.LanPlusState:eq(1):next(function()
                return true
            end):fetch_or(false)
        },
        NAT = GetHmmSshNatPortConfig()
    }
    if SnmpSupported == true then
        result.SNMPAgent = {
            Port = C.Snmp[0].PortID:fetch_or(),
            Enabled = C.Snmp[0].State:eq(1):next(function()
                return true
            end):fetch_or(false)
        }
    end
    if IsMgmtBoard() == false then
        result.KVM = C.PMEServiceConfig[0].KVMEnable:neq(0):next(function()
            return GetPortState('ProxyKvm', 'State')
        end):fetch_or(nil)
        result.VMM = C.PMEServiceConfig[0].KVMEnable:neq(0):next(function()
            return GetPortState('ProxyVmm', 'State')
        end):fetch_or(nil)
        result.Video = C.PMEServiceConfig[0].JavaVideoEnable:neq(0):next(function()
            return GetPortState('Video', 'State')
        end):fetch_or(nil)
        result.VNC = C.PMEServiceConfig[0].VNCEnable:neq(0):next(function()
            return GetPortState('VNC', 'EnableState')
        end):fetch_or(nil)
    end
	return result
end

local function SetPortConfigUint16Port(ctx, portObj, port)
    return http.safe_call(function ()
        local ok, ret = call_method(ctx, portObj, 'SetPort', portErrMap, guint16(port))
        return ok and http.reply_ok() or ret
    end)
end

local function SetPortConfigInt32Port(ctx, portObj, port)
    return http.safe_call(function ()
        local ok, ret = call_method(ctx, portObj, 'SetPort', portErrMap, gint32(port))
        return ok and http.reply_ok() or ret
    end)
end

local function SetPortConfigEnabled(ctx, portObj, enabled)
    return http.safe_call(function ()
        local ok, ret = call_method(ctx, portObj, 'SetState', stateErrMap, gbyte(enabled == true and 1 or 0))
        return ok and http.reply_ok() or ret
    end)
end

local function SetSnmpPortConfigPort(ctx, portObj, port)
    return http.safe_call(function ()
        local ok, ret = call_method(ctx, portObj, 'SetSnmpPortID', portErrMap, gint32(port))
        return ok and http.reply_ok() or ret
    end)
end

local function SetSnmpPortConfigEnabled(ctx, portObj, enabled)
    return http.safe_call(function ()
        local ok, ret = call_method(ctx, portObj, 'SetSnmpState', stateErrMap, gbyte(enabled == true and 1 or 0))
        return ok and http.reply_ok() or ret
    end)
end

local function SetIpmiPortConfigPort(ctx, portObj, ports)
    return http.safe_call(function ()
        local ok, ret = call_method(ctx, portObj, 'SetRmcpPort', portErrMap, ports)
        return ok and http.reply_ok() or ret
    end)
end

local function SetIpmiEnabled(ctx, portObj, inputList)
    return http.safe_call(function ()
        local ok, ret = call_method(ctx, portObj, 'SetIPMILanProtocolState', stateErrMap, inputList)
        return ok and http.reply_ok() or ret
    end)
end

local function SetSshPortConfig(ctx, portConfig, result)
    C.PortConfig("ServiceID", cfg.SERVICE_CONFIGURATION_SSH):next(
        function(portObj)
            if portConfig.Port then
                result:join(SetPortConfigUint16Port(ctx, portObj, portConfig.Port))
            end
            if type(portConfig.Enabled) == "boolean" then
                result:join(SetPortConfigEnabled(ctx, portObj, portConfig.Enabled))
            end
        end):fetch_or()
end

local function SetHttpPortConfig(ctx, portConfig, result)
    C.PortConfig("ServiceID", cfg.SERVICE_CONFIGURATION_WEB_HTTP):next(
        function(portObj)
            if portConfig.Port then
                result:join(SetPortConfigUint16Port(ctx, portObj, portConfig.Port))
            end
            if type(portConfig.Enabled) == "boolean" then
                result:join(SetPortConfigEnabled(ctx, portObj, portConfig.Enabled))
            end
        end):fetch_or()
end

local function SetHttpsPortConfig(ctx, portConfig, result)
    C.PortConfig("ServiceID", cfg.SERVICE_CONFIGURATION_WEB_HTTPS):next(
        function(portObj)
            if portConfig.Port then
                result:join(SetPortConfigUint16Port(ctx, portObj, portConfig.Port))
            end
            if type(portConfig.Enabled) == "boolean" then
                result:join(SetPortConfigEnabled(ctx, portObj, portConfig.Enabled))
            end
        end):fetch_or()
end

local function SetNatPortConfig(ctx, portConfig, result)
    C.PortConfig("ServiceID", cfg.SERVICE_CONFIGURATION_HMMSSHNAT):next(
        function(portObj)
            if portConfig.Port then
                result:join(SetPortConfigUint16Port(ctx, portObj, portConfig.Port))
            end
            if type(portConfig.Enabled) == "boolean" then
                result:join(SetPortConfigEnabled(ctx, portObj, portConfig.Enabled))
            end
        end):fetch_or()
end

local function SetVmmPortConfig(ctx, portConfig, result)
    C.ProxyVmm[0]:next(
        function(portObj)
            if portConfig.Port then
                result:join(SetPortConfigInt32Port(ctx, portObj, portConfig.Port))
            end
            if type(portConfig.Enabled) == "boolean" then
                result:join(SetPortConfigEnabled(ctx, portObj, portConfig.Enabled))
            end
        end):fetch_or()
end

local function SetKvmPortConfig(ctx, portConfig, result)
    C.ProxyKvm[0]:next(
        function(portObj)
            if portConfig.Port then
                result:join(SetPortConfigInt32Port(ctx, portObj, portConfig.Port))
            end
            if type(portConfig.Enabled) == "boolean" then
                result:join(SetPortConfigEnabled(ctx, portObj, portConfig.Enabled))
            end
        end):fetch_or()
end

local function SetVideoPortConfig(ctx, portConfig, result)
    C.ProxyVideo[0]:next(
        function(portObj)
            if portConfig.Port then
                result:join(SetPortConfigInt32Port(ctx, portObj, portConfig.Port))
            end
            if type(portConfig.Enabled) == "boolean" then
                result:join(SetPortConfigEnabled(ctx, portObj, portConfig.Enabled))
            end
        end):fetch_or()
end

local function SetVncPortConfig(ctx, portConfig, result)
    C.VNC[0]:next(
        function(portObj)
            if portConfig.Port then
                result:join(SetPortConfigInt32Port(ctx, portObj, portConfig.Port))
            end
            if type(portConfig.Enabled) == "boolean" then
                result:join(SetPortConfigEnabled(ctx, portObj, portConfig.Enabled))
            end
        end):fetch_or()
end

local function SetSnmpPortConfig(ctx, portConfig, result)
    C.Snmp[0]:next(
        function (portObj)
            if portConfig.Port then
                result:join(SetSnmpPortConfigPort(ctx, portObj, portConfig.Port))
            end
            if type(portConfig.Enabled) == "boolean" then
                result:join(SetSnmpPortConfigEnabled(ctx, portObj, portConfig.Enabled))
            end
        end):fetch()
end

local function CheckIpmiPort(ctx, portConfig)
    local inputList = {}
    if not portConfig.Port then
        inputList = {guint16(portConfig.SparePort), gbyte(10)}
    elseif not portConfig.SparePort then
        inputList = {guint16(portConfig.Port), gbyte(9)}
    else
        inputList = {guint16(portConfig.Port), gbyte(9), guint16(portConfig.SparePort), gbyte(10)}
    end

    return O.SSH:next(function(obj)
        local ok, _ = call_method(ctx, obj, 'PortCheck', nil, inputList)
        return ok
    end):fetch_or(false)
end

local function SetIpmiPortConfig(ctx, portConfig, result)
    C.RmcpConfig[0]:next(function(portObj)
        if portConfig.Port or portConfig.SparePort then
            if not CheckIpmiPort(ctx, portConfig) then
                OperateLog(ctx, "Check IPMI LAN service port failed")
                result:join(reply_bad_request("PortIdModificationFailed", {"IPMILANRMCP"}))
                return
            end

            local inputList
            if portConfig.Port == nil then
                inputList = {gbyte(2), guint16(portObj.Port1:fetch_or(623)), guint16(portConfig.SparePort)}
            elseif portConfig.SparePort == nil then
                inputList = {gbyte(2), guint16(portConfig.Port), guint16(portObj.Port2:fetch_or(624))}
            else
                inputList = {gbyte(2), guint16(portConfig.Port), guint16(portConfig.SparePort)}
            end
            result:join(SetIpmiPortConfigPort(ctx, portObj, inputList))
        end
        if type(portConfig.Enabled) == "boolean" then
            local inputList = {gbyte(1), gbyte(portConfig.Enabled == true and 1 or 0)}
            result:join(SetIpmiEnabled(ctx, portObj, inputList))
        end
    end):fetch_or()
end

local function SetServiceFlagConfig(ctx, result)
    C.RmcpConfig[0]:next(
        function (portObj)
            local inputList
            if C.RmcpConfig[0].LanState:fetch_or() == 1 and C.RmcpConfig[0].LanPlusState:fetch_or() == 0 then
                inputList = {gbyte(1), guint16(C.RmcpConfig[0].Port1:fetch_or()), guint16(C.RmcpConfig[0].Port2:fetch_or())}
            elseif C.RmcpConfig[0].LanState:fetch_or() == 1 and C.RmcpConfig[0].LanPlusState:fetch_or() == 1 then
                inputList = {gbyte(1), guint16(C.RmcpConfig[0].Port1:fetch_or()), guint16(C.RmcpConfig[0].Port2:fetch_or())}
            elseif C.RmcpConfig[0].LanState:fetch_or() == 0 and C.RmcpConfig[0].LanPlusState:fetch_or() == 1 then
                inputList = {gbyte(1), guint16(C.RmcpConfig[0].Port1:fetch_or()), guint16(C.RmcpConfig[0].Port2:fetch_or())}
            elseif C.RmcpConfig[0].LanState:fetch_or() == 0 and C.RmcpConfig[0].LanPlusState:fetch_or() == 0 then
                inputList = {gbyte(0), guint16(C.RmcpConfig[0].Port1:fetch_or()), guint16(C.RmcpConfig[0].Port2:fetch_or())}
            end
            SetIpmiPortConfigPort(ctx, portObj, inputList)
    end):fetch_or()
end

local function SetIpmiPlusPortConfig(ctx, portConfig, result)
    C.RmcpConfig[0]:next(
        function (portObj)
            if type(portConfig.Enabled) == "boolean" then
                local inputList = {gbyte(2), gbyte(portConfig.Enabled == true and 1 or 0)}
                result:join(SetIpmiEnabled(ctx, portObj, inputList))
            end
        end):fetch_or()
end

function M.SetPortConfig(data, user)
    local result = http.reply_ok()
    -- SSH
    if data.SSH then
        if not HasSecurityMgntPrivilege(user) then
            result:join(reply_bad_request("PropertyModificationNeedPrivilege", {"SSH"}))
        else
            SetSshPortConfig(user, data.SSH, result)
        end
    end
    -- HTTP
    if data.HTTP then
        if not HasSecurityMgntPrivilege(user) then
            result:join(reply_bad_request("PropertyModificationNeedPrivilege", {"HTTP"}))
        else
            SetHttpPortConfig(user, data.HTTP, result)
        end
    end
    -- HTTPS
    if data.HTTPS then
        if not HasSecurityMgntPrivilege(user) then
            result:join(reply_bad_request("PropertyModificationNeedPrivilege", {"HTTPS"}))
        else
            SetHttpsPortConfig(user, data.HTTPS, result)
        end
    end
    -- NAT
    if data.NAT then
        if not HasSecurityMgntPrivilege(user) then
            result:join(reply_bad_request("PropertyModificationNeedPrivilege", {"NAT"}))
        else
            SetNatPortConfig(user, data.NAT, result)
        end
    end
    if not IsMgmtBoard() then
        -- VMM
        if data.VMM then
            if not HasSecurityMgntPrivilege(user) then
                result:join(reply_bad_request("PropertyModificationNeedPrivilege", {"VMM"}))
            elseif O.PMEServiceConfig.KVMEnable:fetch_or(0) == 0 then
                result:join(reply_bad_request("PropertyModificationNotSupported", {"VMM"}))
            else
                SetVmmPortConfig(user, data.VMM, result)
            end
        end
        -- KVM
        if data.KVM then
            if not HasSecurityMgntPrivilege(user) then
                result:join(reply_bad_request("PropertyModificationNeedPrivilege", {"KVM"}))
            elseif O.PMEServiceConfig.KVMEnable:fetch_or(0) == 0 then
                result:join(reply_bad_request("PropertyModificationNotSupported", {"KVM"}))
            else
                SetKvmPortConfig(user, data.KVM, result)
            end
        end
        -- Video
        if data.Video then
            if not HasSecurityMgntPrivilege(user) then
                result:join(reply_bad_request("PropertyModificationNeedPrivilege", {"Video"}))
            elseif O.PMEServiceConfig.JavaVideoEnable:fetch_or(0) == 0 then
                result:join(reply_bad_request("PropertyModificationNotSupported", {"Video"}))
            else
                SetVideoPortConfig(user, data.Video, result)
            end
        end
        -- VNC
        if data.VNC then
            if not HasSecurityMgntPrivilege(user) then
                result:join(reply_bad_request("PropertyModificationNeedPrivilege", {"VNC"}))
            elseif O.PMEServiceConfig.VNCEnable:fetch_or(0) == 0 then
                result:join(reply_bad_request("PropertyModificationNotSupported", {"VNC"}))
            else
                SetVncPortConfig(user, data.VNC, result)
            end
        end
    end

    if SnmpSupported == true then
        -- SNMPAgent
        if data.SNMPAgent then
            if not HasUserMgntPrivilege(user) then
                result:join(reply_bad_request("PropertyModificationNeedPrivilege", {"SNMPAgent"}))
            else
                SetSnmpPortConfig(user, data.SNMPAgent, result)
            end
        end
    end

    -- IPMILANRMCP
    if data.IPMILANRMCP then
        if not HasSecurityMgntPrivilege(user) then
            result:join(reply_bad_request("PropertyModificationNeedPrivilege", {"IPMILANRMCP"}))
        else
            SetIpmiPortConfig(user, data.IPMILANRMCP, result)
        end
    end
    -- IPMILANRMCPPlus
    if data.IPMILANRMCPPlus then
        if not HasSecurityMgntPrivilege(user) then
            result:join(reply_bad_request("PropertyModificationNeedPrivilege", {"IPMILANRMCPPlus"}))
        else
            SetIpmiPlusPortConfig(user, data.IPMILANRMCPPlus, result)
        end
    end
    -- set ServiceFlag
    if data.IPMILANRMCP or data.IPMILANRMCPPlus then
        if not HasSecurityMgntPrivilege(user) then
            result:join(reply_bad_request("PropertyModificationNeedPrivilege", {"ServiceFlag"}))
        else
            SetServiceFlagConfig(user, result)
        end 
    end   
    if result:isOk() then
        return reply_ok_encode_json(M.GetPortConfig())
    end
    result:appendErrorData(M.GetPortConfig())
    return result
end

return M
