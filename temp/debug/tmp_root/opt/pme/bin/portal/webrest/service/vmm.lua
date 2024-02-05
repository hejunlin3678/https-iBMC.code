local utils = require "utils"
local dflib = require "dflib"
local c = require "dflib.core"
local cjson = require "cjson"
local http = require "http"
local C = dflib.class
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local gint32 = GVariant.new_int32
local call_method = utils.call_method
local HasSecurityMgntPrivilege = utils.HasSecurityMgntPrivilege
local HasVmmMgntPrivilege = utils.HasVmmMgntPrivilege
local get_security_enhanced_compatible_board_v1 = dflib.get_security_enhanced_compatible_board_v1
local reply_internal_server_error = http.reply_internal_server_error
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_ok = http.reply_ok
local reply_bad_request = http.reply_bad_request
local reply_not_found = http.reply_not_found

-- kvm模块错误码定义
local RF_ERROR = -1
local MANAGER_KVM_IS_IN_USE = 0xff
local RFERR_INSUFFICIENT_PRIVILEGE = 403
local COMP_CODE_INVALID_CMD = 0xC1
local COMP_CODE_OUTOF_RANGE = 0xC9

local portErrMap = {
    [COMP_CODE_INVALID_CMD] = reply_bad_request("PropertyModificationNotSupported",
        "The property %1 modification failed because it is not supported."),
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("PropertyModificationNeedPrivilege",
        "The property %1 cannot be modified because of insufficient permission."),
    [COMP_CODE_OUTOF_RANGE] = reply_bad_request("PortIdModificationFailed",
        "The value %1 for the property %2 is not in the range of 1 to 65535 or already exists."),
    [RF_ERROR] = reply_bad_request("PortIdModificationFailed",
        "The value %1 for the property %2 is not in the range of 1 to 65535 or already exists.")
}

local stateErrMap = {
    [COMP_CODE_INVALID_CMD] = reply_bad_request("PropertyModificationNotSupported",
        "The property %1 modification failed because it is not supported."),
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("PropertyModificationNeedPrivilege",
        "The property %1 cannot be modified because of insufficient permission."),
    [COMP_CODE_OUTOF_RANGE] = reply_bad_request("PropertyValueNotInList",
        "The value %1 for the property %2 is not in the list of acceptable values."),
    [RF_ERROR] = reply_bad_request("PropertyValueNotInList",
        "The value %1 for the property %2 is not in the list of acceptable values.")
}

local errMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("PropertyModificationNeedPrivilege",
        "The property %1 cannot be modified because of insufficient permission."),
    [MANAGER_KVM_IS_IN_USE] = reply_bad_request("KvmInUse",
        "Failed to set the KVM encryption status because the KVM is in use.")
}

local M = {}

function M.GetVMM()
    local KVMEnable = C.PMEServiceConfig[0].KVMEnable:next(utils.ValueToBool):fetch_or(false)
    if not KVMEnable then
        return reply_not_found()
    end
    local VMM = C.ProxyVmm[0]
    return {
        Port = VMM.Port:fetch_or(),
        Enabled = VMM.State:eq(1):next(function()
            return true
        end):fetch_or(false),
        EncryptionEnabled = VMM.EncryptState:eq(1):next(function()
            return true
        end):fetch_or(false),
        EncryptionConfigurable = get_security_enhanced_compatible_board_v1() == 1,
        MaximumNumberOfSessions = 1,
        NumberOfActiveSessions = C.Vmm[0].ConnectNum:fetch_or()
    }
end

function M.SetVMM(user, data)
    -- VMM
    local KVMEnable = C.PMEServiceConfig[0].KVMEnable:next(utils.ValueToBool):fetch_or(false)
    if not KVMEnable then
        return reply_bad_request("PropertyModificationNotSupported")
    end
    local ret = C.ProxyVmm[0]:next(function(obj)
        if data.Port then
            if not HasSecurityMgntPrivilege(user) then
                return reply_bad_request("PropertyModificationNeedPrivilege", {"Port"})
            end
            local ok, err = call_method(user, obj, "SetPort", portErrMap, gint32(data.Port))
            if not ok then
                return err
            end
        end
        if data.Enabled == true then
            if not HasSecurityMgntPrivilege(user) then
                return reply_bad_request("PropertyModificationNeedPrivilege", {"Enabled"})
            end
            local ok, err = call_method(user, obj, "SetState", stateErrMap, gbyte(1))
            if not ok then
                return err
            end
        elseif data.Enabled == false then
            if not HasSecurityMgntPrivilege(user) then
                return reply_bad_request("PropertyModificationNeedPrivilege", {"Enabled"})
            end
            local ok, err = call_method(user, obj, "SetState", stateErrMap, gbyte(0))
            if not ok then
                return err
            end
        end

        if data.EncryptionEnabled ~= nil then 
            if get_security_enhanced_compatible_board_v1() ~= 1 then
                return reply_bad_request('PropertyModificationNotSupported')
            end   

            if data.EncryptionEnabled == false then
                if not HasVmmMgntPrivilege(user) then
                    return reply_bad_request("PropertyModificationNeedPrivilege", {"EncryptionEnabled"})
                end
                local ok, err = call_method(user, obj, "SetEncryptState", errMap, gint32(0))
                if not ok then
                    return err
                end
            elseif data.EncryptionEnabled == true then
                if not HasVmmMgntPrivilege(user) then
                    return reply_bad_request("PropertyModificationNeedPrivilege", {"EncryptionEnabled"})
                end
                local ok, err = call_method(user, obj, "SetEncryptState", errMap, gint32(1))
                if not ok then
                    return err
                end
            end
        end
    end):catch(function(err)
        return reply_internal_server_error("SetVMM EncryptionEnabled", err)
    end):fetch()

    if ret then
        ret:appendErrorData(M.GetVMM())
        return ret
    end

    return reply_ok_encode_json(M.GetVMM())
end

function M.VMMControl(user, data)
    local ret
    -- VMM
    ret = C.Vmm[0]:next(function(obj)
        if data.VmmControlType == "Disconnect" then
            call_method(user, obj, "DelectLink", nil, nil)
        end
    end):catch(function(err)
        return reply_internal_server_error("VMMControl ", err)
    end):fetch()

    if ret then
        return ret
    end

    return reply_ok()
end

return M
