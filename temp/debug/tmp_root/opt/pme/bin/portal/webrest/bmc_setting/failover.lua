local utils = require "utils"
local dflib = require "dflib"
local cjson = require 'cjson'
local c = require "dflib.core"
local http = require "http"
local logging = require "logging"
local C = dflib.class
local O = dflib.object
local null = cjson.null
local GVariant = c.GVariant
local guint32 = GVariant.new_uint32
local call_method = utils.call_method
local reply_bad_request = http.reply_bad_request
local reply_internal_server_error = http.reply_internal_server_error
local reply_forbidden = http.reply_forbidden
local reply_ok = http.reply_ok

local SET_FAILOVER_OK = 0
local ASM_FAILOVER_OCCURRED_BY_USER_COMMAND = 1

local HMM_INDEX_E = {
    HMM1 = 1,
    HMM2 = 2
}

local ASM_STATUS_E = {
    ASM_ACTIVE = 0,
    ASM_STANDBY = 1
}

local DATA_SYNC_STATUS_E = {
    SYNCING = 0,
    COMPLETE = 1
}

local M = {}

function M.GetFailover()
    local standbyindex = O.SMM.Index:next(function(index) 
        if index == HMM_INDEX_E.HMM1 then
            return HMM_INDEX_E.HMM2
        else
            return HMM_INDEX_E.HMM1
        end
    end):fetch()
    local syncstatus = O.AsmObject.AsDataSyncStatus:next(function(status)
        if status == DATA_SYNC_STATUS_E.SYNCING then
            return 'Syncing'
        elseif status == DATA_SYNC_STATUS_E.COMPLETE then
            return 'Complete'
        else
            return null
        end
    end):fetch()
    
    return {
        SyncStatus = syncstatus,
        ActiveIndex = O.SMM.Index:fetch(),
        StandbyIndex = standbyindex,
        StandbyPresent = O.AsmObject.AsCpldRemotePresentStatus:fetch()
    }
end

function M.SetFailover(user)
    local AsmObject = O.AsmObject

    if O.AsmObject.AsStatus:fetch_or(1) ~= ASM_STATUS_E.ASM_ACTIVE then
        return reply_bad_request("CurrentMMNotActive")
    end

    local ok, outputlist = call_method(user, AsmObject, 'AsmFailover', function(errcode)
        return reply_internal_server_error("InternalError")
    end, guint32(ASM_FAILOVER_OCCURRED_BY_USER_COMMAND))
    if not ok then
        return outputlist
    end

    if outputlist[1] == SET_FAILOVER_OK then
        return reply_ok()
    end
    return reply_forbidden("ForceFailoverError", outputlist[2])
end

return M
