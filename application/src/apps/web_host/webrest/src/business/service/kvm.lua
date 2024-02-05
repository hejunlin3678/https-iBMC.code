local utils = require 'utils'
local dflib = require 'dflib'
local c = require 'dflib.core'
local cjson = require 'cjson'
local http = require 'http'
local C = dflib.class
local O = dflib.object
local null = cjson.null
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local gint32 = GVariant.new_int32
local gvstring = GVariant.new_vstring
local call_method = utils.call_method
local HasSecurityMgntPrivilege = utils.HasSecurityMgntPrivilege
local HasKvmMgntPrivilege = utils.HasKvmMgntPrivilege
local OperateLog = utils.OperateLog
local get_compatibility_enabled = dflib.get_compatibility_enabled
local reply_internal_server_error = http.reply_internal_server_error
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_bad_request = http.reply_bad_request
local safe_call = http.safe_call
local reply_ok = http.reply_ok
local reply_not_found = http.reply_not_found

-- kvm模块错误码定义
local RF_ERROR = -1
local MANAGER_KVM_IS_IN_USE = 0xff
local RFERR_INSUFFICIENT_PRIVILEGE = 403
local COMP_CODE_INVALID_CMD = 0xC1
local COMP_CODE_OUTOF_RANGE = 0xC9

local AUTO_LOCK_KEY_ARRAY_LENGTH = 4

local key_table = {
    'Meta', 'Backspace', 'Tab', 'Enter', 'Clear', '', 'Shift', 'Control', 'Alt', 'AltGraph', 'Pause', 'CapsLock',
    'Escape', 'Space', 'PageUp', 'PageDown', 'End', 'Home', 'ArrowLeft', 'ArrowUp', 'ArrowRight', 'ArrowDown', ',',
    '<', '-', '_', '.', '>', '/', '?', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '`', '~', ';', ':', '\'',
    '"', '=', '+', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
    't', 'u', 'v', 'w', 'x', 'y', 'z', '0(Numpad)', 'Insert(Numpad)', '1', 'End(Numpad)', '2(Numpad)',
    'ArrowDown(Numpad)', '3(Numpad)', 'PageDown(Numpad)', '4(Numpad)', 'ArrowLeft(Numpad)', '5(Numpad)',
    'Clear(Numpad)', '6(Numpad)', 'ArrowRight(Numpad)', '7(Numpad)', 'Home(Numpad)', '8(Numpad)',
    'ArrowUp(Numpad)', '9(Numpad)', 'PageUp(Numpad)', '*', '+', '-', 'Delete', '/', 'Delete', 'NumLock',
    'ScrollLock', 'F1', 'F2', 'F3', 'F4', 'F5', 'F6', 'F7', 'F8', 'F9', 'F10', 'F11', 'F12', '', 'Insert', '[',
    '{', '\\', '|', ']', ']', 'ContextMenu', 'IntlRo', 'Hiragana', '', '', '', 'IntlBackslash'
}

local portErrMap = {
    [COMP_CODE_INVALID_CMD] = reply_bad_request('PropertyModificationNotSupported',
        'The property %1 modification failed because it is not supported.'),
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request('PropertyModificationNeedPrivilege',
        'The property %1 cannot be modified because of insufficient permission.'),
    [COMP_CODE_OUTOF_RANGE] = reply_bad_request('PortIdModificationFailed',
        'The value %1 for the property %2 is not in the range of 1 to 65535 or already exists.'),
    [RF_ERROR] = reply_bad_request('PortIdModificationFailed',
        'The value %1 for the property %2 is not in the range of 1 to 65535 or already exists.')
}

local stateErrMap = {
    [COMP_CODE_INVALID_CMD] = reply_bad_request('PropertyModificationNotSupported',
        'The property %1 modification failed because it is not supported.'),
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request('PropertyModificationNeedPrivilege',
        'The property %1 cannot be modified because of insufficient permission.'),
    [COMP_CODE_OUTOF_RANGE] = reply_bad_request('PropertyValueNotInList',
        'The value %1 for the property %2 is not in the list of acceptable values.'),
    [RF_ERROR] = reply_bad_request('PropertyValueNotInList',
        'The value %1 for the property %2 is not in the list of acceptable values.')
}

local errMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request('PropertyModificationNeedPrivilege',
        'The property %1 cannot be modified because of insufficient permission.'),
    [MANAGER_KVM_IS_IN_USE] = reply_bad_request('KvmInUse',
        'Failed to set the KVM encryption status because the KVM is in use.')
}

local function dal_check_valid_key(auto_Key)
    for _, v in pairs(key_table) do
        if string.lower(v) == string.lower(auto_Key) then
            return true
        end
    end

    return false
end

local M = {}

function M.GetKVM(user)
    local KVMEnable = C.PMEServiceConfig[0].KVMEnable:next(utils.ValueToBool):fetch_or(false)
    if not KVMEnable then
        return reply_not_found()
    end
    return {
        Enabled = C.ProxyKvm[0].State:eq(1):next(function()
            return true
        end):fetch_or(false),
        Port = C.ProxyKvm[0].Port:fetch_or(''),
        SessionTimeoutMinutes = C.Kvm[0].KvmTimeout:fetch_or(),
        EncryptionConfigurable = get_compatibility_enabled() == 1,
        VisibleJavaKvm = C.Kvm[0].VisibleJavaKvm:fetch_or(1) == 1,
        EncryptionEnabled = C.Kvm[0].EncryptState:eq(1):next(
            function()
                return true
            end):fetch_or(false),
        PersistentUSBConnectionEnabled = C.Kvm[0].KeyboardMode:eq(1):next(
            function()
                return true
            end):fetch_or(false),
        NumberOfActiveSessions = C.Kvm[0].ConnectNum:fetch_or(),
        ActivatedSessionsType = C.KvmLink():fold(function(obj)
            local propIp = obj.link_ip:fetch()
            if propIp == '0' then
                return null
            end
            local linkMode = obj.link_mode:fetch()
            if linkMode == 0 then
                return 'Shared', false
            elseif linkMode == 1 then
                return 'Private', false
            end
            return null
        end):fetch_or(null),
        LocalKvmEnabled = C.ProxyKvm[0].LocalKVMState:eq(1):next(
            function()
                return true
            end):fetch_or(false),
        AutoOSLockEnabled = C.Kvm[0].AutoOSLockState:eq(1):next(
            function()
                return true
            end):fetch_or(false),
        AutoOSLockType = C.Kvm[0].AutoOSLockType:next(function(type)
            if type == 1 then
                return 'Custom'
            else
                return 'Windows'
            end
        end):fetch(),
        AutoOSLockKey = C.Kvm[0].AutoOSLockKey:fetch_or(),
        MaximumNumberOfSessions = 2
    }
end

local function set_port(user, port)
    if not HasSecurityMgntPrivilege(user) then
        return reply_bad_request("PropertyModificationNeedPrivilege", {"Port"})
    end
    return safe_call(function()
        return C.ProxyKvm[0]:next(function(obj)
            local ok, err = call_method(user, obj, 'SetPort', portErrMap, gint32(port))
            if not ok then
                return err
            end
        end):catch(function(err)
            return reply_internal_server_error('SetKVM KVM', err)
        end):fetch()
    end)
end

local function set_enable_state(user, status)
    if not HasSecurityMgntPrivilege(user) then
        return reply_bad_request("PropertyModificationNeedPrivilege", {"Enabled"})
    end
    return safe_call(function()
        return C.ProxyKvm[0]:next(function(obj)
            local ok, err = call_method(user, obj, 'SetState', stateErrMap, status and gbyte(1) or gbyte(0))
            if not ok then
                return err
            end
        end):catch(function(err)
            return reply_internal_server_error('SetKVM KVM', err)
        end):fetch()
    end)
end

local function set_kvm_timeout(user, period)
    if not HasKvmMgntPrivilege(user) then
        return reply_bad_request("PropertyModificationNeedPrivilege", {"SessionTimeoutMinutes"})
    end
    return safe_call(function()
        return C.Kvm[0]:next(function(obj)
            if period then
                local ok, err = call_method(user, obj, 'SetKvmTimeout', errMap, gint32(period))
                if not ok then
                    return err
                end
            end
        end):catch(function(err)
            return reply_internal_server_error('SetKVM TimeoutMinutes', err)
        end):fetch()
    end)
end

local function set_encrypt_state(user, status)

    if get_compatibility_enabled() ~= 1 then
        return reply_bad_request('PropertyModificationNotSupported')
    end   

    if not HasKvmMgntPrivilege(user) then
        return reply_bad_request("PropertyModificationNeedPrivilege", {"EncryptionEnabled"})
    end
    return safe_call(function()
        return C.ProxyKvm[0]:next(function(obj)
            local ok, err = call_method(user, obj, 'SetEncryptState', errMap, status and gint32(1) or gint32(0))
            if not ok then
                return err
            end
        end):catch(function(err)
            return reply_internal_server_error('SetKVM EncryptionEnabled', err)
        end):fetch()
    end)
end

local function set_local_kvm_state(user, status)
    if not HasKvmMgntPrivilege(user) then
        return reply_bad_request("PropertyModificationNeedPrivilege", {"LocalKvmEnabled"})
    end
    return safe_call(function()
        return C.ProxyKvm[0]:next(function(obj)
            local ok, err = call_method(user, obj, 'SetLocalKVMState', errMap, status and gbyte(1) or gbyte(0))
            if not ok then
                return err
            end
        end):catch(function(err)
            return reply_internal_server_error('SetKVM LocalKvmEnabled', err)
        end):fetch()
    end)
end

local function set_key_board_mode(user, status)
    if not HasKvmMgntPrivilege(user) then
        return reply_bad_request("PropertyModificationNeedPrivilege", {"PersistentUSBConnectionEnabled"})
    end
    return safe_call(function()
        return C.Kvm[0]:next(function(obj)
            local ok, err = call_method(user, obj, 'SetKeyboardMode', errMap, status and gint32(1) or gint32(0))
            if not ok then
                return err
            end
        end):catch(function(err)
            return reply_internal_server_error('SetKVM PersistentUSBConnectionEnabled', err)
        end):fetch()
    end)
end

local function set_auto_os_lock_state(user, status)
    if not HasKvmMgntPrivilege(user) then
        return reply_bad_request("PropertyModificationNeedPrivilege", {"AutoOSLockEnabled"})
    end
    return safe_call(function()
        return C.Kvm[0]:next(function(obj)
            local ok, err = call_method(user, obj, 'SetAutoOSLockState', errMap, status and gbyte(1) or gbyte(0))
            if not ok then
                return err
            end
        end):catch(function(err)
            return reply_internal_server_error('SetKVM AutoOSLockEnabled', err)
        end):fetch()
    end)
end

local function set_auto_os_lock_type(user, lock_type)
    if not HasKvmMgntPrivilege(user) then
        return reply_bad_request("PropertyModificationNeedPrivilege", {"AutoOSLockType"})
    end
    return safe_call(function()
        return C.Kvm[0]:next(function(obj)
            if lock_type == 'Custom' then
                local ok, err = call_method(user, obj, 'SetAutoOSLockType', errMap, gbyte(1))
                if not ok then
                    return err
                end
            elseif lock_type == 'Windows' then
                local ok, err = call_method(user, obj, 'SetAutoOSLockType', errMap, gbyte(0))
                if not ok then
                    return err
                end
            end
        end):catch(function(err)
            return reply_internal_server_error('SetKVM AutoOSLockType', err)
        end):fetch()
    end)
end

local function set_auto_os_lock_key(user, lock_keys)
    if not HasKvmMgntPrivilege(user) then
        return reply_bad_request("PropertyModificationNeedPrivilege", {"AutoOSLockKey"})
    end
    return safe_call(function()
        return C.Kvm[0]:next(function(obj)
            local ok, err = call_method(user, obj, 'SetAutoOSLockKey', errMap, gvstring(lock_keys))
            if not ok then
                return err
            end
        end):catch(function(err)
            local len = #lock_keys
            if len > AUTO_LOCK_KEY_ARRAY_LENGTH then
                return reply_bad_request('PropertyMemberQtyExceedLimit', {'AutoOSLockKey'}, {'AutoOSLockKey'})
            end
            for _, v in ipairs(lock_keys) do
                if #v == 0 then
                    len = len - 1
                else
                    if not dal_check_valid_key(v) then
                        return reply_bad_request('PropertyValueNotInList', {v, 'AutoOSLockKey'}, {'AutoOSLockKey'})
                    end
                end
            end

            if len == 0 then
                return reply_bad_request('PropertyMemberQtyExceedLimit', {'AutoOSLockKey'}, {'AutoOSLockKey'})
            end

            return reply_internal_server_error('SetKVM SetAutoOSLockKey', err)
        end):fetch()
    end)
end

function M.SetKVM(user, data)
    local KVMEnable = C.PMEServiceConfig[0].KVMEnable:next(utils.ValueToBool):fetch_or(false)
    if not KVMEnable then
        return reply_bad_request("PropertyModificationNotSupported")
    end
    local result = reply_ok()
    -- KVM
    if data.Port ~= nil then
        result:join(set_port(user, data.Port))
    end

    if data.Enabled ~= nil then
        result:join(set_enable_state(user, data.Enabled))
    end

    -- SessionTimeoutMinutes
    if data.SessionTimeoutMinutes then
        result:join(set_kvm_timeout(user, data.SessionTimeoutMinutes))
    end

    -- EncryptionEnabled
    if data.EncryptionEnabled ~= nil then
        result:join(set_encrypt_state(user, data.EncryptionEnabled))
    end

    -- LocalKvmEnabled
    if data.LocalKvmEnabled ~= nil then
        result:join(set_local_kvm_state(user, data.LocalKvmEnabled))
    end

    -- PersistentUSBConnectionEnabled
    if data.PersistentUSBConnectionEnabled ~= nil then
        result:join(set_key_board_mode(user, data.PersistentUSBConnectionEnabled))
    end

    -- AutoOSLockEnabled
    if data.AutoOSLockEnabled ~= nil then
        result:join(set_auto_os_lock_state(user, data.AutoOSLockEnabled))
    end

    -- AutoOSLockType
    if data.AutoOSLockType then
        result:join(set_auto_os_lock_type(user, data.AutoOSLockType))
    end

    -- AutoOSLockKey
    if data.AutoOSLockKey then
        result:join(set_auto_os_lock_key(user, data.AutoOSLockKey))
    end

    if not result:isOk() then
        result:appendErrorData(M.GetKVM())
        return result
    end

    return reply_ok_encode_json(M.GetKVM())
end

local function get_kvm_file_name()
    local ok, files = pcall(c.file_list, '/opt/pme/web/htdocs/bmc/pages/jar/', true)
    if not ok then
        return ''
    end

    for _, line in ipairs(files) do
        if line:match('^vconsole') then
            return line
        end
    end
    return ''
end

local KVM_JNLP_FILE_FORMAT = '<?xml version="1.0" encoding="UTF-8"?>\n\
         <jnlp spec="1.0+" codebase="https://%s">\n\
		 <information>\n\
		 <title>Remote Virtual Console   IP :%s   SN: %s</title>\n\
		      <vendor>BMC</vendor>\n\
	  </information>\n\
		      <resources>\n\
			      <j2se version="1.7+" />\n\
			      <jar href="/bmc/pages/jar/%s?authParam=%d" main="true"/>\n\
		      </resources>\n\
		      <applet-desc name="Remote Virtual Console   IP :%s   SN: %s" main-class="com.kvm.KVMApplet" width="950" height="700" >\n\
			      <param name="verifyValue" value="%d"/>\n\
			      <param name="mmVerifyValue" value="%d"/>\n\
			      <param name="decrykey" value="%s"/>\n\
			      <param name="local" value="%s"/>\n\
			      <param name="compress" value="%d"/>\n\
			      <param name="vmm_compress" value="%d"/>\n\
			      <param name="port" value="%d"/>\n\
			      <param name="vmmPort" value="%d"/>\n\
			      <param name="privilege" value="%d"/>\n\
			      <param name="bladesize" value="1"/>\n\
			      <param name="IPA" value="%s"/>\n\
			      <param name="IPB" value="%s"/>\n\
			      <param name="verifyValueExt" value="%s"/>\n\
			      <param name="title" value=" IP :%s   SN :%s"/>\n\
			      <param name="capsSync" value="%d"/>\n\
		     </applet-desc>\n\
		     <security>\n\
			      <all-permissions/>\n\
		     </security>\n\
	  </jnlp>'

function M.GenerateStartupFile(user, hostname, data)
    local KVMEnable = C.PMEServiceConfig[0].KVMEnable:next(utils.ValueToBool):fetch_or(false)
    if not KVMEnable then
        return http.reply_method_not_implemented()
    end

    local server_name, port = hostname:match("^(.*):([0-9]+)$")
    if not server_name then
        server_name = hostname
    end

    local vmm_obj = C.Vmm[0]
    local vmm_compress = vmm_obj.EncryptState:fetch()
    local vmm_port = vmm_obj.Port:fetch()

    local kvm_obj = C.Kvm[0]
    local jar_name = get_kvm_file_name()
    local device_sn = O.BMC.DeviceSerialNumber:fetch()
    local kvm_compress = kvm_obj.EncryptState:fetch()
    local kvm_port = kvm_obj.Port:fetch()

    local caps_enable = C.PRODUCT[0].KVMCapsSyncEnable:fetch()

    local verify_id
    local secret_key
    local verify_id_ext
    local ok, resp = utils.call_method(user, O.Kvm0, 'GetLinkKey', nil, nil)
    if ok then
        verify_id = resp[1]
        secret_key = resp[2]
        verify_id_ext = resp[3]
        local input_list = {
            GVariant.new_int32(verify_id), GVariant.new_int32(data.Mode == 'Shared' and 0 or 1),
            GVariant.new_string(user.UserName), GVariant.new_string(secret_key),
            GVariant.new_int32(user.Privilege), GVariant.new_string(verify_id_ext)
        }
        utils.call_method(user, O.Kvm0, 'SetCodeKey', nil, input_list)
    end

    local user_privilege = 0
    for v in user.RoleId:gmatch('(%d+)') do
        user_privilege = (user_privilege > tonumber(v)) and user_privilege or tonumber(v)
    end

    resp = string.format(KVM_JNLP_FILE_FORMAT, hostname, server_name, device_sn, jar_name, verify_id, server_name, device_sn,
        verify_id, verify_id, secret_key, data.Language, kvm_compress, vmm_compress, kvm_port, vmm_port,
        user_privilege, server_name, server_name, verify_id_ext, server_name, device_sn, caps_enable) -- 序列号格式化
    OperateLog(user, 'Export kvm startup file to local successfully')

    local header = {
        ['content-type'] = "application/x-java-jnlp-file",
        ['Content-Disposition'] = "filename = kvm.jnlp"
    }

    return http.reply_ok(resp, header)
end

return M
