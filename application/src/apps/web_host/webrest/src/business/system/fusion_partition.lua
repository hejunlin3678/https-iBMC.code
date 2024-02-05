local utils = require 'utils'
local dflib = require 'dflib'
local c = require 'dflib.core'
local cjson = require 'cjson'
local http = require 'http'
local logging = require 'logging'
local O = dflib.object
local null = cjson.null
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local gstring = GVariant.new_string
local call_method = utils.call_method
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_bad_request = http.reply_bad_request
local reply_ok = http.reply_ok

local LOG_TYPE_LOCAL = 0
local PARTITION_4P = 0
local PARTITION_8P = 1
-- 节点模式
local NODE_MODE_E = {NODE_SLAVE = 0, NODE_MASTER = 1, NODE_MAX = 2}

-- 硬分区配置情况
local PARTITION_E = {
    PARTITION_4P = 0,
    PARTITION_8P = 1,
    PARTITION_CENTRALIZE = 2, -- 16P, 32P情况
    PARTITION_MAX = 3
};

local RFPROP_VAL_FP_SINGLE_SYSTEM = 'SingleSystem'
local RFPROP_VAL_FP_DUAL_SYSTEM = 'DualSystem'

local RFERR_INSUFFICIENT_PRIVILEGE = 403
local PARTITON_CODE_SLAVEBMC_NOT_PRESENT = 3001 -- 从BMC不在位
local PARTITON_CODE_BMC_NOT_POWEROFF = 3002 -- 主、从BMC的系统不是下电的
local PARTITON_CODE_PARTITION_CONF_SAME = 3003 -- 要配置分区的和当前的配置相同
local PARTITON_CODE_BMC_VERSION_NOT_SAME = 3004 -- 主、从BMC的版本不相同
local PARTITON_CODE_CPLD_VERSION_NOT_SAME = 3005 -- 主、从BMC的CPLD版本不相同
local PARTITON_CODE_SLAVE_BMC_AUTH_FAIL = 3006 -- 从BMC认证失败
local PARTITON_CODE_SLAVE_BMC_USER_NOT_PRI = 3007 -- 从BMC的权限需要管理员或者操作员

local errMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request('PropertyModificationNeedPrivilege'),
    [PARTITON_CODE_SLAVEBMC_NOT_PRESENT] = reply_bad_request('SetFusionPartitionFailed',
        {'the system B is not present'}),
    [PARTITON_CODE_BMC_NOT_POWEROFF] = reply_bad_request('SetFusionPartitionFailed',
        {'the OS of system A or B is powered on'}),
    [PARTITON_CODE_PARTITION_CONF_SAME] = reply_bad_request('SetFusionPartitionFailed', {
        'the FusionPartition\'s value entered is the same as the current value'
    }),
    [PARTITON_CODE_BMC_VERSION_NOT_SAME] = reply_bad_request('SetFusionPartitionFailed',
        {'the system A and B versions are different'}),
    [PARTITON_CODE_CPLD_VERSION_NOT_SAME] = reply_bad_request('SetFusionPartitionFailed', {
        'the CPLD versions of the system A and B are different'
    }),
    [PARTITON_CODE_SLAVE_BMC_AUTH_FAIL] = reply_bad_request('SetFusionPartitionFailed',
        {'the authentication of the system B failed'}),
    [PARTITON_CODE_SLAVE_BMC_USER_NOT_PRI] = reply_bad_request('SetFusionPartitionFailed', {
        'the user of the system B has insufficient permission'
    })
}

local function int_to_bool(value)
    if value == 0 then
        return false
    elseif value == 1 then
        return true
    else
        return null
    end
end

local function act_manager_set_fusion_partition(user, FusionPartition, RemoteNodeUserName, RemoteNodePassword)
    if FusionPartition == nil then
        return
    end
    if O.xmlPartition.NodeMode:fetch() ~= NODE_MODE_E.NODE_MASTER then
        return reply_bad_request('ActionNotSupported')
    end
    local fp_mode_byte = (FusionPartition == 'DualSystem') and PARTITION_4P or PARTITION_8P
    local inputlist = {
        gbyte(fp_mode_byte), gstring(RemoteNodeUserName), gstring(RemoteNodePassword), gbyte(LOG_TYPE_LOCAL),
        gstring(user.ClientIp)
    }

    local ok, ret = call_method(user, O.Partition, 'SwitchPartition', errMap, inputlist)
    if not ok then
        return ret
    end
end

local function set_manager_vga_enabled(user, VGAUSBDVDEnabled)
    if VGAUSBDVDEnabled == nil then
        return
    end
    if O.xmlPartition.NodeMode:fetch() ~= NODE_MODE_E.NODE_MASTER then
        return reply_bad_request('ActionNotSupported')
    end

    local vgaState
    if VGAUSBDVDEnabled then
        vgaState = 1
    else
        vgaState = 0
    end

    local ok, ret = call_method(user, O.Partition, 'SwitchVGA', errMap, gbyte(vgaState))
    if not ok then
        return ret
    end
end

local function fusion_partition_to_str(xml_partition)
    if xml_partition == PARTITION_E.PARTITION_4P then
        return RFPROP_VAL_FP_DUAL_SYSTEM
    elseif xml_partition == PARTITION_E.PARTITION_8P then
        return RFPROP_VAL_FP_SINGLE_SYSTEM
    else
        return null
    end
end

local M = {}

function M.GetFusionPartition()
    local result = {}
    if O.xmlPartition.NodeMode:fetch() ~= NODE_MODE_E.NODE_MASTER then
        result['FusionPartition'] = null
        result['VGAUSBDVDEnabled'] = null
    end
    -- get_fusion_partition
    result['FusionPartition'] = O.xmlPartition.XmlPartition:next(fusion_partition_to_str):fetch()
    result['VGAUSBDVDEnabled'] = O.Partition.CurrentVGASwitch:next(int_to_bool):fetch() -- get_vga_enabled

    return result
end

function M.SetFusionPartition(user, data)
    local result = reply_ok()
    result:join(act_manager_set_fusion_partition(user, data.FusionPartition, data.RemoteNodeUserName,
        data.RemoteNodePassword))
    result:join(set_manager_vga_enabled(user, data.VGAUSBDVDEnabled))
    if result:isOk() then
        return reply_ok_encode_json(M.GetFusionPartition())
    end
    result:appendErrorData(M.GetFusionPartition())
    return result
end

return M
