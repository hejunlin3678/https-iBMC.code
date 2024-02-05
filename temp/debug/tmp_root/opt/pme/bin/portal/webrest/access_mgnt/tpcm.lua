local dflib = require 'dflib'
local C = dflib.class
local O = dflib.object
local bit = require 'bit'
local utils = require 'utils'
local http = require "http"
local c = require "dflib.core"
local cfg = require "config"
local call_method = utils.call_method
local reply_ok = http.reply_ok
local reply_bad_request = http.reply_bad_request
local reply_not_found = http.reply_not_found
local GVariant = c.GVariant
local gstring = GVariant.new_string
local gbyte = GVariant.new_byte
local BoolToValue = utils.BoolToValue
local guint32 = GVariant.new_uint32

local M = {}
local REAUTH_ERROR_INFO = "%s, because the current user password is incorrect or the account is locked"

-- from https://pcisig.com/membership/member-companies?combine=
local TCM_ALL_MEMBER_MAP = {
    [0x1b4e] = "国民技术",
    [0x0] = "未知厂商"
}
local TSB_DISABLE = 0x10;
local ADDR_INVALID = 0x11;
local TSB_ERROR = 0x12;
local errMap = {
    [TSB_DISABLE] = reply_bad_request("TsbServerDisable"),
    [ADDR_INVALID] = reply_bad_request("Addrinvalid"),
    [TSB_ERROR] = reply_bad_request("TsbError")
}

local function GetVersionStr(version)
    local t = {}
    for i = 0, 3 do
        t[i + 1] = bit.band(bit.rshift(version, (i * 8)), 0xFF)
    end
    return string.format("%d.%d.%d.%d", t[1], t[2], t[3], t[4])
end

function M.GetTPCMBaseInfo(user)
    -- tpcm_log_version default: 1.0.0.1
    local tpcmLogVersion = O.TPCMBaseInfo.TPCMLogVersion:fetch_or(0x01000001)
    local TCMManufacturerId = O.TPCMBaseInfo.TCMManufacturer:fetch()
    local TCMManufacturer = "未知厂商"
    if TCM_ALL_MEMBER_MAP[TCMManufacturerId] ~= nil then
        TCMManufacturer = "国民技术"
    end
    local tcmVersion = O.TPCMBaseInfo.TCMVersion:fetch()
    return {
        ProductName = "华为BMC TPCM",
        ProductModel = "Hi1711",
        ProductVersion = GetVersionStr(tpcmLogVersion),
        ProductManufacturer = "Huawei",
        TCMManufacturer = TCMManufacturer,
        TCMVersion = GetVersionStr(tcmVersion)
    }
end

function M.GetTPCMDetailedInfo(user)
    local res = C.TPCMMeasured():fold(function(obj, acc)
        local ActualDigest = obj.ActualDigest:fetch()
        local StandardDigest = obj.StandardDigest:fetch()
        acc[#acc + 1] = {
            MeasuredObject = obj.MeasuredObject:fetch(),
            PCR = obj.PCRName:fetch(),
            ActualDigest = ActualDigest,
            StandardDigest = StandardDigest,
            IsMatch = obj.CompResult:fetch() == 0
        }
        return acc
    end, {}):fetch()
    local BiosVersion = O.Bios:next(function(oo)
        local version = oo.Version:fetch()
        local unitNum = oo.UnitNum:fetch()
        return string.format("%s(U%d)", version, unitNum)
    end):fetch_or("")
    res[1]["Version"] = O.BMC.L1FWVersion:fetch_or("")
    res[2]["Version"] = O.BMC.UbootVersion:fetch_or("")
    res[3]["Version"] = string.format("%s(U%d)", O.BMC.PMEVer:fetch(), O.BMC.FlashUnitNum:fetch())
    res[4]["Version"] = BiosVersion
    res[5]["Version"] = O.TPCMGRUB.ObjectVersion:fetch_or("")
    res[6]["Version"] = O.TPCMOSCfg.ObjectVersion:fetch_or("")
    res[7]["Version"] = O.TPCMOSKernel.ObjectVersion:fetch_or("")
    res[8]["Version"] = O.TPCMOSInitrd.ObjectVersion:fetch_or("")
    return res
end

function M.GetTPCMTsbInfo(user)
    -- 判断机型
    if O.BMC.ChipName:fetch_or() ~= 'Hi1711' then
        return reply_not_found('ResourceMissingAtURI')
    end
    local ok, ret = call_method(user, O.TPCMBaseInfo, "GetTpcmSupportState", nil, nil)
    if not ok or ret[1] ~= 1 then
        return reply_not_found('ResourceMissingAtURI')
    end
    local ok, TsbEnableStatus = pcall(function()
        return O.TPCMBaseInfo.TsbEnableStatus:fetch()
    end)
    if not ok then
        TsbEnableStatus = 0
    end
    return {
        TrustedCenterServerAddr = O.TPCMConfig.TrustedCenterServerAddr:fetch(),
        MeasureEnable = O.TPCMConfig.MeasureEnable:fetch(),
        ControlAction = O.TPCMConfig.ControlAction:fetch(),
        TsbEnableStatus = TsbEnableStatus
    }
end

function M.SetTPCMTsbInfo(user, data)
    -- 判断机型
    if O.BMC.ChipName:fetch_or() ~= 'Hi1711' then
        return reply_not_found('ResourceMissingAtURI')
    end
    local ok, ret = call_method(user, O.TPCMBaseInfo, "GetTpcmSupportState", nil, nil)
    if not ok or ret[1] ~= 1 then
        return reply_not_found('ResourceMissingAtURI')
    end
    -- 双因素、ldap、krb和sso登录无需二次认证
    if O.MutualAuthentication.MutualAuthenticationState:fetch_or(0) ~= 1 and user and user.AuthType ~= cfg.LOG_TYPE_LDAP_KRB_SSO then
        if not data or not data.ReauthKey or #data.ReauthKey > 128 or not data.ReauthKey:match('^[a-zA-Z0-9+/=]+$') then
            utils.OperateLog(user, REAUTH_ERROR_INFO, "Set tpcm config failed")
            return http.reply_error(http.status.HTTP_UNAUTHORIZED, 'ReauthFailed', REAUTH_FAIL_REQ_INFO)
        end

        local reauth = utils.ReAuthUser(user, data.ReauthKey)
        if reauth then
            utils.OperateLog(user, REAUTH_ERROR_INFO, "Set tpcm config failed")
            return reauth
        end
    end
     -- 判断tsb使能状态
    local ok, TsbEnableStatus = pcall(function()
        return O.TPCMBaseInfo.TsbEnableStatus:fetch()
    end)
    if ok and TsbEnableStatus == 1 then
        if data.MeasureEnable ~= nil then
            local MeasureEnable = gbyte(BoolToValue(data.MeasureEnable))
            local ok, ret = call_method(user, O.TPCMConfig, "SetTPCMTsbEnable", errMap, MeasureEnable)
            if not ok then
                utils.OperateLog(user, "Set tpcm measure enable to (%s) failed", data.MeasureEnable and "open" or "close")
                return reply_bad_request("SetTPCMTsbMeasureEnableFaild")
            end
            utils.OperateLog(user, "Set tpcm measure enable to (%s) successfully", data.MeasureEnable and "open" or "close")
        end
        if data.ControlAction ~= nil then
            local ControlAction = guint32(BoolToValue(data.ControlAction))
            local ok, ret = call_method(user, O.TPCMConfig, "SetTPCMTsbAction", errMap, ControlAction)
            if not ok then
                utils.OperateLog(user, "Set tpcm control action to (%s) failed", data.ControlAction and "open" or "close")
                return reply_bad_request("SetTPCMTsbControlActionFaild")
            end
            utils.OperateLog(user, "Set tpcm control action to (%s) successfully", data.ControlAction and "open" or "close")
        end
        if data.TrustedCenterServerAddr ~= nil then
            local ok, ret = call_method(user, O.TPCMConfig, "SetTPCMTsbIp", errMap, gstring(data.TrustedCenterServerAddr))
            if not ok then
                return ret
            end
        end
        return reply_ok()
    end
    return reply_bad_request("TsbServerDisable")
end

function M.SetStandardDigest(user, data)
    -- 判断机型
    if O.BMC.ChipName:fetch_or() ~= 'Hi1711' then
        return reply_not_found('ResourceMissingAtURI')
    end
    -- 判断tcm在位状态
    local ok, ret = call_method(user, O.TPCMBaseInfo, "GetTpcmSupportState", nil, nil)
    if not ok or ret[1] ~= 1 then
        return reply_not_found('ResourceMissingAtURI')
    end

    -- 判断tsb使能状态
    local ok, TsbEnableStatus = pcall(function()
        return O.TPCMBaseInfo.TsbEnableStatus:fetch()
    end)
    if not ok or TsbEnableStatus ~= 1 then
        return reply_bad_request("TsbServerDisable")
    end

    -- 双因素、ldap、krb和sso登录无需二次认证
    if O.MutualAuthentication.MutualAuthenticationState:fetch_or(0) ~= 1 and user and user.AuthType ~= cfg.LOG_TYPE_LDAP_KRB_SSO then
        if not data or not data.ReauthKey or #data.ReauthKey > 128 or not data.ReauthKey:match('^[a-zA-Z0-9+/=]+$') then
            utils.OperateLog(user, REAUTH_ERROR_INFO, "Set tpcm Digest failed")
            return http.reply_error(http.status.HTTP_UNAUTHORIZED, 'ReauthFailed', REAUTH_FAIL_REQ_INFO)
        end

        local reauth = utils.ReAuthUser(user, data.ReauthKey)
        if reauth then
            utils.OperateLog(user, REAUTH_ERROR_INFO, "Set tpcm Digest failed")
            return reauth
        end
    end

    object = {"Bios", "Shim", "Grub", "Grub.Cfg", "Kernel", "Initrd"}
    digest = {data.BiosStandardDigest, data.ShimStandardDigest, data.GrubStandardDigest, data.GrubCfgStandardDigest,
        data.KernelStandardDigest, data.InitrdStandardDigest}
    for i=1, #object do
        if digest[i] then
            local inputList = {gbyte(i), gstring(digest[i])}
            local ok, ret = call_method(user, C.TPCMMeasured[0]:next():fetch(), "SetStandardDigest", errMap, inputList)
            if not ok then
                utils.OperateLog(user, "Set tpcm %s Standard Digest failed", object[i])
                return ret
            end
            utils.OperateLog(user, "Set tpcm %s Standard Digest successfully", object[i])
        end
    end

    return reply_ok()
end
return M
