local dflib = require 'dflib'
local c = require 'dflib.core'
local http = require 'http'
local cjson = require 'cjson'
local utils = require 'utils'
local cfg = require 'config'
local bit = require "bit"
local logging = require "logging"
local reply_ok = http.reply_ok
local reply_bad_request = http.reply_bad_request
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_error = http.reply_error
local C = dflib.class
local O = dflib.object
local null = cjson.null
local safe_call = http.safe_call
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local gstring = GVariant.new_string
local call_method = utils.call_method
local HTTP_METHOD_NOT_IMPLEMENTED = http.status.HTTP_METHOD_NOT_IMPLEMENTED
local DISABLE = cfg.DISABLE
local ENABLE = cfg.ENABLE
local json_decode = cjson.decode
local json_encode = cjson.encode

local RF_ERROR = -1
local BIOS_ERROR_INVALID_INPUT_PARA = 0xe001
local notSupported = reply_error(HTTP_METHOD_NOT_IMPLEMENTED, 'PropertyModificationNotSupported')
local valueOutOfRange = reply_bad_request('ValueOutOfRange')
local errMap = {
    [RF_ERROR] = notSupported,
    [BIOS_ERROR_INVALID_INPUT_PARA] = valueOutOfRange
}

local CPU_PT_STATE_POWER_OFF = 0x03
local CPU_PT_STATE_OUTOF_RANGE = 0x02
local cpuErrMap = {
    [CPU_PT_STATE_POWER_OFF] = notSupported,
    [CPU_PT_STATE_OUTOF_RANGE] = reply_bad_request('PropertyValueOutOfRange')
}

local BIOS_BOOT_LEGACY = 0
local BIOS_BOOT_UEFI = 1
local function rfsystem_bootmode_to_string(boot_mode)
    if boot_mode == BIOS_BOOT_LEGACY then
        return 'Legacy'
    elseif boot_mode == BIOS_BOOT_UEFI then
        return 'UEFI'
    else
        return null
    end
end

local function rfsystem_bootmode_to_uchar(start_flag)
    if start_flag == 'Legacy' then
        return BIOS_BOOT_LEGACY
    elseif start_flag == 'UEFI' then
        return BIOS_BOOT_UEFI
    end
end

-- 启动设备的生效次数
local EFFECTIVE_NONE = 0 -- 不生效
local EFFECTIVE_ONCE = 1 -- 生效一次
local EFFECTIVE_FOREVER = 2 -- 永久生效
local function rfsystem_startenable_to_string(start_flag)
    if EFFECTIVE_ONCE == start_flag then
        return 'Once'
    elseif EFFECTIVE_NONE == start_flag then
        return 'Disabled'
    elseif EFFECTIVE_FOREVER == start_flag then
        return 'Continuous'
    else
        return null
    end
end

local function rfsystem_start_flag_to_uchar(start_flag)
    if 'Once' == start_flag then
        return EFFECTIVE_ONCE
    elseif 'Disabled' == start_flag then
        return EFFECTIVE_NONE
    elseif 'Continuous' == start_flag then
        return EFFECTIVE_FOREVER
    end
end
-- Force PXE
local FORCE_PEX = 0x01
-- Force boot from default Hard-drive
local FORCE_HARD_DRIVE = 0x02
-- Force boot from default CD/DVD
local FORCE_CD_DVD = 0x05
-- Force boot into BIOS Setup
local FORCE_BIOS_SETUP = 0x06
-- Force boot from Floppy/primary removable media
local FORCE_FLOPPY_REMOVABLE_MEDIA = 0x0F

local function rfsystem_startoption_to_string(start_option)
    if start_option == 0 then
        return 'None'
    elseif start_option == FORCE_PEX then
        return 'Pxe'
    elseif start_option == FORCE_HARD_DRIVE then
        return 'Hdd'
    elseif start_option == FORCE_CD_DVD then
        return 'Cd'
    elseif start_option == FORCE_BIOS_SETUP then
        return 'BiosSetup'
    elseif start_option == FORCE_FLOPPY_REMOVABLE_MEDIA then
        return 'Floppy'
    else
        return null
    end
end

local function rfsystem_startoption_to_uchar(start_option)
    if start_option == 'None' then
        return 0
    elseif start_option == 'Pxe' then
        return FORCE_PEX
    elseif start_option == 'Hdd' then
        return FORCE_HARD_DRIVE
    elseif start_option == 'Cd' then
        return FORCE_CD_DVD
    elseif start_option == 'BiosSetup' then
        return FORCE_BIOS_SETUP
    elseif start_option == 'Floppy' then
        return FORCE_FLOPPY_REMOVABLE_MEDIA
    end
end

-- Force protocol and device
local FORCE_PEX_TFTP = 0x01
local FORCE_PEX_HTTPS = 0x02
local FORCE_HARD_DRIVE_NVMEM = 0x01
local FORCE_HARD_DRIVE_EMMC = 0x02
local FORCE_HARD_DRIVE_SATA = 0x03
local FORCE_FLOPPY_REMOVABLE_MEDIA_USB = 0x01
local function rfsystem_startoptionext_to_string(start_option_ext)
    local start_option = C.Bios[0].StartOption:fetch_or(null)
    local start_option_map = {
        [FORCE_PEX] = {
            [FORCE_PEX_TFTP] = 'Tftp',
            [FORCE_PEX_HTTPS] = 'Https'
        },
        [FORCE_HARD_DRIVE] = {
            [FORCE_HARD_DRIVE_NVMEM] = 'NvmeM2',
            [FORCE_HARD_DRIVE_EMMC] = 'Emmc',
            [FORCE_HARD_DRIVE_SATA] = 'Sata'
        },
        [FORCE_FLOPPY_REMOVABLE_MEDIA] = {
            [FORCE_FLOPPY_REMOVABLE_MEDIA_USB] = 'Usb'
        }
    }
    if start_option_map[start_option] and start_option_map[start_option][start_option_ext] then
        return start_option_map[start_option][start_option_ext]
    else
        return 'None'
    end
end

-- Supported protocol and device
local SUPPORTED_PROTOCOL_TFTP = 0x00
local SUPPORTED_PROTOCOL_HTTPS = 0x01
local SUPPORTED_OPTION_NVMEM = 0x04
local SUPPORTED_OPTION_EMMC = 0x05
local SUPPORTED_OPTION_SATA = 0x06
local SUPPORTED_OPTION_USB = 0x08
local SUPPORTED_OPTION_NONE = 0xFF
local function rfsystem_startoptionext_to_uchar(start_option_ext)
    if start_option_ext == 'Tftp' then
        return SUPPORTED_PROTOCOL_TFTP
    elseif start_option_ext == 'Https' then
        return SUPPORTED_PROTOCOL_HTTPS
    elseif start_option_ext == 'NvmeM2' then
        return SUPPORTED_OPTION_NVMEM
    elseif start_option_ext == 'Emmc' then
        return SUPPORTED_OPTION_EMMC
    elseif start_option_ext == 'Sata' then
        return SUPPORTED_OPTION_SATA
    elseif start_option_ext == 'Usb' then
        return SUPPORTED_OPTION_USB
    elseif start_option_ext == 'None' then
        return SUPPORTED_OPTION_NONE
    end
end

local OnOffToStr = utils.OnOffToStr
local OnOffStrToValue = utils.OnOffStrToValue

local M = {}

local function get_supported_startoption_list(start_option)
    local start_option_list = {
        [0] = 'None', [FORCE_PEX] = 'Pxe', [FORCE_HARD_DRIVE] = 'Hdd',
        [FORCE_CD_DVD] = 'Cd', [FORCE_BIOS_SETUP] = 'BiosSetup', 
        [FORCE_FLOPPY_REMOVABLE_MEDIA] = 'Floppy'
    }
    local i = 1
    local supported_start_options = {}
    for k, v in pairs(start_option_list) do
        if bit.band(bit.lshift(1, k), start_option) ~= 0 then
            supported_start_options[i] = v
            i = i + 1
        end
    end
    return supported_start_options
end

local function get_supported_startoptionext_list(start_option_ext)
    local start_option_ext_list = {
        [SUPPORTED_PROTOCOL_TFTP] = 'Tftp', [SUPPORTED_PROTOCOL_HTTPS] = 'Https', [SUPPORTED_OPTION_NVMEM] = 'NvmeM2', 
        [SUPPORTED_OPTION_EMMC] = 'Emmc', [SUPPORTED_OPTION_SATA] = 'Sata', [SUPPORTED_OPTION_USB] = 'Usb'
    }
    local i = 1
    local supported_start_options_ext = {}
    for k, v in pairs(start_option_ext_list) do
        if bit.band(bit.lshift(1, k), start_option_ext) ~= 0 then
            supported_start_options_ext[i] = v
            i = i + 1
        end
    end

    local none_config = {}
    local pxe_protocol = {}
    local hdd_device = {}
    local cd_dvd = {}
    local floppy_device = {}
    for k, ext in pairs(supported_start_options_ext) do
        if ext == 'Tftp' or ext == 'Https' then
            pxe_protocol[#pxe_protocol + 1] = ext
        elseif ext == 'NvmeM2' or ext == 'Emmc' or ext == 'Sata' then
            hdd_device[#hdd_device + 1] = ext
        elseif ext == 'Usb' then
            floppy_device[#floppy_device + 1] = ext
        end
    end

    local result = {}
    result['None'] = none_config
    result['Pxe'] = pxe_protocol
    result['Hdd'] = hdd_device
    result['Cd'] = cd_dvd
    result['Floppy'] = floppy_device
    return result
end

function M.GetBootOptions(user)
    return C.Bios[0]:next(function(obj)
        local boot_mode_sw_display_enabled = obj.BiosBootModeSwEnable:next(ValueToBool):fetch()
        local boot_mode_swi = obj.BiosBootModeSw:next(OnOffToStr):fetch()
        local start_option = obj.StartOption:next(rfsystem_startoption_to_string):fetch()
        local start_flag = obj.StartOptionFlag:next(rfsystem_startenable_to_string):fetch()
        local boot_mode = obj.BiosBootMode:next(rfsystem_bootmode_to_string):fetch()
        local supported_start_options = obj.SupportedStartOption:next(get_supported_startoption_list):fetch()
        local start_option_ext = obj.StartOptionExt:next(rfsystem_startoptionext_to_string):fetch()
        local supported_start_options_ext = obj.SupportedStartOptionExt:next(get_supported_startoptionext_list):fetch()
        local boot_order = null
        local effective_status, order
        local ok, ret = pcall(function()
            local ok, ret = call_method(user, obj, 'GetBiosSettingEffectiveStatus', nil, {})
            return ok and ret or nil
        end)
        if ok and ret and ret[1] then
            effective_status = (ret[1] == 0) and 'Ineffective' or 'Effective'
        end
        ok, ret = pcall(function()
            return obj:call('GetBIOSJsonFile', nil, {gstring('setting.json')})
        end)
        if ok and ret and ret[1] then
            order = json_decode(ret[1])
        end
        if ok and effective_status == 'Ineffective' and order and order.BootTypeOrder0 and order.BootTypeOrder1 and
            order.BootTypeOrder2 and order.BootTypeOrder3 then
            boot_order = {order.BootTypeOrder0, order.BootTypeOrder1, order.BootTypeOrder2, order.BootTypeOrder3}
        else
            ok, ret = pcall(function()
                local status, result = call_method(user, obj, 'GetBIOSJsonFile', nil,
                    {gstring('currentvalue.json')})
                return status and result or nil
            end)
            if ok and ret and ret[1] then
                order = json_decode(ret[1])
                boot_order = {
                    order.BootTypeOrder0, order.BootTypeOrder1, order.BootTypeOrder2, order.BootTypeOrder3
                }
            end
        end
        return {
            BootModeConfigOverIpmiEnabled = boot_mode_swi,
            BootSourceOverrideMode = boot_mode,
            PreferredBootMedium = obj.SupportedStartOptionExt:next(function(support_optionext)
                -- 当SupportedStartOptionExt为0，表示不支持扩展启动设备或启动协议
                if support_optionext == 0 then
                    return {
                        AllowableValues = supported_start_options,
                        Target = start_option,
                        EffectivePeriod = start_flag
                    }
                else
                    return {
                        AllowableValuesExt = supported_start_options_ext,
                        Target = start_option,
                        TargetExt = start_option_ext,
                        EffectivePeriod = start_flag
                    }
                end
            end):fetch_or(),
            BootOrder = boot_order,
            BootModeIpmiSWDisplayEnabled = boot_mode_sw_display_enabled
        }
    end):fetch()
end

local PRODUCT_VERSION_V5 = 5
function M.UpdateBootOptions(user, data)
    return C.Bios[0]:next(function(obj)
        local result = reply_ok()
        if data.BootModeConfigOverIpmiEnabled then
            result:join(safe_call(function()
                if not obj.BiosBootModeSupportFlag:fetch_or() then
                    return reply_bad_request('PropertyNotWritable')
                end

                local v = gbyte(OnOffStrToValue(data.BootModeConfigOverIpmiEnabled))
                local ok, ret = call_method(user, obj, 'SetBiosBootModeSw', errMap, v)
                if not ok then
                    return ret
                end
            end))
        end

        if data.PreferredBootMedium then
            -- 启动有效次数设置
            if data.PreferredBootMedium.EffectivePeriod then
                result:join(safe_call(function()
                    if O.PMEServiceConfig.X86Enable:fetch() == DISABLE then
                        return notSupported
                    end

                    local v = gbyte(rfsystem_start_flag_to_uchar(data.PreferredBootMedium.EffectivePeriod))
                    local ok, ret = call_method(user, obj, 'SetStartOptionFlag', errMap, v)
                    if not ok then
                        return ret
                    end
                end))
            end

            -- 启动项设置
            if data.PreferredBootMedium.Target then
                result:join(safe_call(function()
                    if O.PMEServiceConfig.X86Enable:fetch() == DISABLE then
                        return notSupported
                    end
                    local v = {}
                    if data.PreferredBootMedium.TargetExt then
                        v = {gbyte(rfsystem_startoption_to_uchar(data.PreferredBootMedium.Target)), 
                                        gbyte(rfsystem_startoptionext_to_uchar(data.PreferredBootMedium.TargetExt))}
                        local ok, ret = call_method(user, obj, 'SetStartOptionExt', errMap, v)
                        if not ok then
                            return ret
                        end
                    else
                        v = {gbyte(rfsystem_startoption_to_uchar(data.PreferredBootMedium.Target))}
                        local ok, ret = call_method(user, obj, 'SetStartOption', errMap, v)
                        if not ok then
                            return ret
                        end
                    end
                end))
            end
        end

        -- boot 启动模式处理设置
        if data.BootSourceOverrideMode then
            result:join(safe_call(function()
                local objPMEServiceConfig = O.PMEServiceConfig
                if objPMEServiceConfig.X86Enable:fetch() == DISABLE then
                    return notSupported
                end

                if obj.BiosBootModeSupportFlag:fetch() ~= ENABLE then
                    return notSupported
                end

                local v = gbyte(rfsystem_bootmode_to_uchar(data.BootSourceOverrideMode))
                local ok, ret = call_method(user, obj, 'SetBiosBootMode', errMap, v)
                if not ok then
                    return ret
                end
            end))
        end

        if data.BootOrder then
            local order = data.BootOrder
            local bootOrder = json_encode({
                BootTypeOrder0 = order[1],
                BootTypeOrder1 = order[2],
                BootTypeOrder2 = order[3],
                BootTypeOrder3 = order[4]
            })
            local input_list = {gstring('setting.json'), gstring(bootOrder)}
            result:join(safe_call(function()
                local ok, ret = call_method(user, obj, 'SetBIOSJsonFile', errMap, input_list)
                if not ok then
                    return ret
                end
            end))
        end
        if result:isOk() then
            return reply_ok_encode_json(M.GetBootOptions())
        end
        result:appendErrorData(M.GetBootOptions())
        return result
    end):fetch()
end

function M.GetCPUAdjust()
    return C.PowerCapping[0]:next(function(obj)
        local ret = {Pstate = null, Tstate = null, PStateLimit = null, TStatelimit = null}
        pcall(function()
            local PTState = obj:call('GetPT')
            ret.Pstate = PTState[1]
            ret.Tstate = PTState[2]
        end)
        pcall(function()
            local PTStateLimit = obj:call('GetPTLimit')
            if PTStateLimit[1] ~= 0 then
                ret.PStateLimit = PTStateLimit[1] - 1
            end
            if PTStateLimit[2] ~= 0 then
                ret.TStatelimit = PTStateLimit[2] - 1
            end
        end)
        return ret
    end):fetch()
end

function M.UpdateCPUAdjust(user, data)
    local objPMEServiceConfig = O.PMEServiceConfig
    if objPMEServiceConfig.ARMEnable:fetch() == ENABLE then
        return notSupported
    end
    return C.PowerCapping[0]:next(function(obj)
        local result = reply_ok()
        result:join(safe_call(function()
            local PTStateLimit = obj:call('GetPTLimit')
            local p_limit = PTStateLimit[1]
            local t_limit = PTStateLimit[2]
            local current_pt_state = obj:call('GetPT')
            local p_state = current_pt_state[1] or 0
            local t_state = current_pt_state[2] or 0

            -- 设置CPU的工作频率
            if data.Pstate then
                if p_limit == 0 then
                    return notSupported
                elseif data.Pstate > p_limit - 1 then
                    return valueOutOfRange
                else
                    p_state = data.Pstate
                end
            end

            -- 设置CPU的空闲时间
            if data.Tstate then
                if t_limit == 0 then
                    return notSupported
                elseif data.Tstate > t_limit - 1 then
                    return valueOutOfRange
                else
                    t_state = data.Tstate
                end
            end

            local ok, ret = call_method(user, obj, 'SetPT', cpuErrMap, {gbyte(p_state), gbyte(t_state)})
            if not ok then
                return ret
            end
        end))
        if result:isOk() then
            return reply_ok_encode_json(M.GetCPUAdjust())
        end
        result:appendErrorData(M.GetCPUAdjust())
        return result
    end):fetch()
end

return M
