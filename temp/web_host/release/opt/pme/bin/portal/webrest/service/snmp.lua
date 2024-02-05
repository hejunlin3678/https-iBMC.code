local cfg = require "config"
local utils = require "utils"
local dflib = require "dflib"
local c = require "dflib.core"
local http = require "http"
local bit = require "bit"
local logging = require "logging"
local cjson = require 'cjson'
local C = dflib.class
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local gint32 = GVariant.new_int32
local gstring = GVariant.new_string
local call_method = utils.call_method
local reply_internal_server_error = http.reply_internal_server_error
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_bad_request = http.reply_bad_request
local null = cjson.null

-- kvm模块错误码定义
local RF_ERROR = -1
local ERR_EXCEED_MAX_LEN = -2
local RFERR_WRONG_PARAM = 400
local SNMP_COMMUNITY_FIELD = 0xFF
local SNMP_COMMUNITY_SPACE_FAIL = 0x86
local SNMP_COMMUNITY_LENGTH_FAIL = 0x85
local SNMP_COMMUNITY_COMPLEXITY_FAIL = 0x84
local SNMP_COMMUNITY_TOO_WEAK = 0x88
local SNMP_COMMUNITY_PREVIOUS = 0x89
local RFERR_INSUFFICIENT_PRIVILEGE = 403
local COMP_CODE_INVALID_CMD = 0xC1
local COMP_CODE_OUTOF_RANGE = 0xC9

local errMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("PropertyModificationNeedPrivilege",
        "The property cannot be modified because of insufficient permission."),
    [SNMP_COMMUNITY_TOO_WEAK] = reply_bad_request("CommunityNameInWeakPWDDict",
        "The community name cannot be included in the weak password dictionary."),
    [SNMP_COMMUNITY_SPACE_FAIL] = reply_bad_request("CommunityNameNotContainSpace",
        "The property cannot contain spaces."),
    [SNMP_COMMUNITY_LENGTH_FAIL] = reply_bad_request("InvalidCommunityNameLength",
        "Invalid length of the property %1."),
    [SNMP_COMMUNITY_COMPLEXITY_FAIL] = reply_bad_request("SimpleCommunityName",
        "The property is too simple or the same as the one set previously."),
    [SNMP_COMMUNITY_FIELD] = reply_bad_request("SameCommunityName",
        "The property is the same as another community name."),
    [RFERR_WRONG_PARAM] = reply_bad_request("PropertyValueFormatError",
        "The value for the property is of a different format than the property can accept."),
    [ERR_EXCEED_MAX_LEN] = reply_bad_request("PropertyValueExceedsMaxLength",
        "The value for the property cannot exceed characters."),
    [RF_ERROR] = reply_bad_request("PortIdModificationFailed",
        "The value for the property is not in the list of acceptable values.")
}

local errROMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("PropertyModificationNeedPrivilege",
        "The property cannot be modified because of insufficient permission."),
    [SNMP_COMMUNITY_TOO_WEAK] = reply_bad_request("CommunityNameInWeakPWDDict",
        "The community name cannot be included in the weak password dictionary."),
    [SNMP_COMMUNITY_SPACE_FAIL] = reply_bad_request("CommunityNameNotContainSpace",
        "The property cannot contain spaces."),
    [SNMP_COMMUNITY_LENGTH_FAIL] = reply_bad_request("InvalidCommunityNameLength",
        "Invalid length of the property %1."),
    [SNMP_COMMUNITY_COMPLEXITY_FAIL] = reply_bad_request("SimpleCommunityName",
        "The property is too simple or the same as the one set previously."),
    [SNMP_COMMUNITY_FIELD] = reply_bad_request("SameROCommunityName",
        "The read-only community name cannot be the same as the read/write community name."),
    [SNMP_COMMUNITY_PREVIOUS] = reply_bad_request("ROCommunitySimilarWithHistory",
        "The new and old passwords of the read-only community name must have at least two different characters."),
    [RFERR_WRONG_PARAM] = reply_bad_request("PropertyValueFormatError",
        "The value for the property is of a different format than the property can accept."),
    [ERR_EXCEED_MAX_LEN] = reply_bad_request("PropertyValueExceedsMaxLength",
        "The value for the property cannot exceed characters.")
}

local errRWMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("PropertyModificationNeedPrivilege",
        "The property cannot be modified because of insufficient permission."),
    [SNMP_COMMUNITY_TOO_WEAK] = reply_bad_request("CommunityNameInWeakPWDDict",
        "The community name cannot be included in the weak password dictionary."),
    [SNMP_COMMUNITY_SPACE_FAIL] = reply_bad_request("CommunityNameNotContainSpace",
        "The property cannot contain spaces."),
    [SNMP_COMMUNITY_LENGTH_FAIL] = reply_bad_request("InvalidCommunityNameLength",
        "Invalid length of the property %1."),
    [SNMP_COMMUNITY_COMPLEXITY_FAIL] = reply_bad_request("SimpleCommunityName",
        "The property is too simple or the same as the one set previously."),
    [SNMP_COMMUNITY_FIELD] = reply_bad_request("SameRWCommunityName",
        "The read/write community name cannot be the same as the read-only community name."),
    [SNMP_COMMUNITY_PREVIOUS] = reply_bad_request("RWCommunitySimilarWithHistory",
        "The new and old passwords of the read/write community name must have at least two different characters."),
    [RFERR_WRONG_PARAM] = reply_bad_request("PropertyValueFormatError",
        "The value for the property is of a different format than the property can accept."),
    [ERR_EXCEED_MAX_LEN] = reply_bad_request("PropertyValueExceedsMaxLength",
        "The value for the property cannot exceed characters.")
}

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

function M.GetSNMP()
    local SNMP = C.Snmp[0]
    return {
        Port = SNMP.PortID:fetch(),
        Enabled = SNMP.State:eq(1):next(function()
            return true
        end):fetch_or(false),

        SystemContact = SNMP.sysContact:fetch(),
        SystemLocation = SNMP.sysLocation:next(function(location)
            if location ~= nil and location ~= "N/A" and location ~= "null" and location ~= "UnKnown" then
                return location
            else
                logging:error("Invalid string. SystemLocation: %s", location)
                return nil
            end
        end):fetch(),
        SNMPv1v2 = {
            SNMPv1Enabled = SNMP.V1State:next(function(state)
                if state == 0 then
                    return false
                elseif state == 1 then
                    return true
                else
                    logging:error("state is out of range. state: %d", state)
                    return nil
                end
            end):fetch(),
            SNMPv2Enabled = SNMP.V2CState:next(function(state)
                if state == 0 then
                    return false
                elseif state == 1 then
                    return true
                else
                    logging:error("state is out of range. state: %d", state)
                    return nil
                end
            end):fetch(),
            LongPasswordEnabled = SNMP.LongPasswordEnable:next(
                function(state)
                    if state == 0 then
                        return false
                    elseif state == 1 then
                        return true
                    else
                        logging:error("state is out of range. state: %d", state)
                        return nil
                    end
                end):fetch(),
            ROCommunityName = null,
            RWCommunityName = null,
            PasswordComplexityCheckEnabled = C.PasswdSetting[0].EnableStrongPassword:next(
                function(value)
                    if value == 0 then
                        return false
                    elseif value == 1 then
                        return true
                    else
                        logging:error("EnableStrongPassword is out of range. EnableStrongPassword: %d", value)
                    end
                end):fetch(),
            LoginRule = SNMP.SNMPV1V2CPermitRuleIds:next(function(type)
                local loginRuleArray = {}
                local loginRuleBitArr = {1, 2, 4}
                local loginRuleStrArr = {"Rule1", "Rule2", "Rule3"}
                for key, value in ipairs(loginRuleBitArr) do
                    if bit.band(type, value) == value then
                        loginRuleArray[#loginRuleArray + 1] = loginRuleStrArr[key]
                    end
                end
                return loginRuleArray
            end):fetch(),
            LoginRuleInfo = C.PermitRule():fold(function(obj, acc)
                local enableValue = C.SecurityEnhance[0].PermitRuleIds:fetch()
                local TimeRuleInfo = obj.TimeRuleInfo:fetch()
                local StartTime, EndTime = TimeRuleInfo:match("([^/]+)/(.+)")
                local loginRuleBitArr = {1, 2, 4}
                local loginRuleStrArr = {"Rule1", "Rule2", "Rule3"}
                local loginRuleEnable
                acc[#acc + 1] = {
                    ID = obj.RuleId:next(function(id)
                        if bit.band(loginRuleBitArr[id + 1], enableValue) == loginRuleBitArr[id + 1] then
                            loginRuleEnable = true
                        else
                            loginRuleEnable = false
                        end

                        if 0 <= id and id < 3 then
                            return loginRuleStrArr[id + 1]
                        else
                            logging:error("LoginRuleId is out of range. Id: %d", id)
                        end
                    end):fetch(),
                    Enabled = loginRuleEnable,
                    StartTime = StartTime,
                    EndTime = EndTime,
                    IP = obj.IpRuleInfo:fetch(),
                    Mac = obj.MacRuleInfo:fetch()
                }
                return acc
            end, {}):fetch()
        },
        SNMPv3 = {
            Enabled = SNMP.V3Status:next(function(state)
                if state == 0 then
                    return false
                elseif state == 1 then
                    return true
                else
                    logging:error("state is out of range. state: %d", state)
                    return nil
                end
            end):fetch(),
            EngineID = SNMP.OldEngineID:fetch()
        }
    }
end

function M.SetSNMP(data, user)
    -- SNMPAgent
    local ret = C.Snmp[0]:next(function(obj)
        if data.Port then
            local ok, err = call_method(user, obj, "SetSnmpPortID", portErrMap, gint32(data.Port))
            if not ok then
                return err
            end
        end
        if data.Enabled == true then
            local ok, err = call_method(user, obj, "SetSnmpState", stateErrMap, gbyte(1))
            if not ok then
                return err
            end
        elseif data.Enabled == false then
            local ok, err = call_method(user, obj, "SetSnmpState", stateErrMap, gbyte(0))
            if not ok then
                return err
            end
        end
        if data.SystemContact then
            local ok, err = call_method(user, obj, "SetsysContact", errMap, gstring(data.SystemContact))
            if not ok then
                return err
            end
        end
        if data.SystemLocation then
            local ok, err = call_method(user, obj, "SetsysLocation", errMap, gstring(data.SystemLocation))
            if not ok then
                return err
            end
        end

        if data.SNMPv1v2 then
            local version = 0
            if data.SNMPv1v2.SNMPv1Enabled == true then
                local ok, err = call_method(user, obj, "SetSnmpVersionState", errMap, gbyte(version), gbyte(1))
                if not ok then
                    return err
                end
            elseif data.SNMPv1v2.SNMPv1Enabled == false then
                local ok, err = call_method(user, obj, "SetSnmpVersionState", errMap, gbyte(version), gbyte(0))
                if not ok then
                    return err
                end
            end
            version = 1
            if data.SNMPv1v2.SNMPv2Enabled == true then
                local ok, err = call_method(user, obj, "SetSnmpVersionState", errMap, gbyte(version), gbyte(1))
                if not ok then
                    return err
                end
            elseif data.SNMPv1v2.SNMPv2Enabled == false then
                local ok, err = call_method(user, obj, "SetSnmpVersionState", errMap, gbyte(version), gbyte(0))
                if not ok then
                    return err
                end
            end

            if data.SNMPv1v2.LongPasswordEnabled == true then
                local ok, err = call_method(user, obj, "SetLongPasswordEnable", errMap, gbyte(1))
                if not ok then
                    return err
                end
            elseif data.SNMPv1v2.LongPasswordEnabled == false then
                local ok, err = call_method(user, obj, "SetLongPasswordEnable", errMap, gbyte(0))
                if not ok then
                    return err
                end
            end

            if data.SNMPv1v2.ROCommunityName then
                local ok, err = call_method(user, obj, "SetSnmpROCommunity", errROMap,
                    gstring(data.SNMPv1v2.ROCommunityName))
                if not ok then
                    return err
                end
            end
            if data.SNMPv1v2.RWCommunityName then
                local ok, err = call_method(user, obj, "SetSnmpRWCommunity", errRWMap,
                    gstring(data.SNMPv1v2.RWCommunityName))
                if not ok then
                    return err
                end
            end
            if data.SNMPv1v2.LoginRule then
                local value = 0
                for _, rule in pairs(data.SNMPv1v2.LoginRule) do
                    if rule == "Rule1" then
                        value = value + 1
                    elseif rule == "Rule2" then
                        value = value + 2
                    elseif rule == "Rule3" then
                        value = value + 4
                    end
                end
                local ok, err = call_method(user, obj, "SetSNMPV1V2CPermitRuleIds", errMap, gbyte(value))
                if not ok then
                    return err
                end
            end
        end

        if data.SNMPv3 then
            if data.SNMPv3.Enabled ~= nil then
                local ok, err = call_method(user, obj, "SetSnmpVersionState", errMap, gbyte(2), gbyte(data.SNMPv3.Enabled and 1 or 0))
                if not ok then
                    return err
                end
            end
        end
    end):catch(function(err)
        return reply_internal_server_error("SetSNMP ", err)
    end):fetch()

    if ret then
        ret:appendErrorData(M.GetSNMP())
        return ret
    end

    return reply_ok_encode_json(M.GetSNMP())
end

return M
