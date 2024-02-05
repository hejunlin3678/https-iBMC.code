local dflib = require "dflib"
local defs = require 'define'
local c = require "dflib.core"
local http = require "http"
local tools = require 'tools'
local utils = require "utils"
local download = require "download"
local logging = require "logging"
local bit = require "bit"
local cfg = require 'config'
local reply_ok = http.reply_ok
local reply_ok_encode_json = http.reply_ok_encode_json
local safe_call = http.safe_call
local reply_bad_request = http.reply_bad_request
local C = dflib.class
local O = dflib.object
local rf_string_check = utils.rf_string_check
local call_method = utils.call_method
local dal_set_file_owner = utils.dal_set_file_owner
local dal_set_file_mode = utils.dal_set_file_mode
local get_datetime = c.get_datetime
local get_timestamp_offset = c.get_timestamp_offset
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local guint16 = GVariant.new_uint16
local gint32 = GVariant.new_int32
local guint32 = GVariant.new_uint32
local gstring = GVariant.new_string
local band = bit.band
local format = string.format

local RF_ERROR = -1
local VOS_ERR_NOTSUPPORT = -8

local LOGENTRY_STATUS_INFORMATIONAL_CODE = 0
local LOGENTRY_STATUS_MINOR_CODE = 1
local LOGENTRY_STATUS_MAJOR_CODE = 2
local LOGENTRY_STATUS_CRITICAL_CODE = 3
local INFORMATIONAL_STRING = "Informational"
local MINOR_STRING = "Minor"
local MAJOR_STRING = "Major"
local CRITICAL_STRING = "Critical"
local levelsMap = {
    [INFORMATIONAL_STRING] = LOGENTRY_STATUS_INFORMATIONAL_CODE,
    [MINOR_STRING] = LOGENTRY_STATUS_MINOR_CODE,
    [MAJOR_STRING] = LOGENTRY_STATUS_MAJOR_CODE,
    [CRITICAL_STRING] = LOGENTRY_STATUS_CRITICAL_CODE
}
local function get_event_level(level)
    if level == LOGENTRY_STATUS_INFORMATIONAL_CODE then
        return INFORMATIONAL_STRING
    elseif level == LOGENTRY_STATUS_MINOR_CODE then
        return MINOR_STRING
    elseif level == LOGENTRY_STATUS_MAJOR_CODE then
        return MAJOR_STRING
    elseif level == LOGENTRY_STATUS_CRITICAL_CODE then
        return CRITICAL_STRING
    end
end

local SEL_TAR_PATH = "/tmp/sel.tar"
local ParamValueErr = "ActionParameterValueFormatError"

local EVENT_ERR_SEL_LIMIT_INVALID = -4001
local EVENT_ERR_SEL_SEARCH_STR_INVALID = -4003
local EVENT_ERR_SEL_LEVEL_INVALID = -4004
local EVENT_ERR_SEL_ENTITY_INVALID = -4005
local EVENT_ERR_SEL_BEGIN_TIME_INVALID = -4006
local EVENT_ERR_SEL_END_TIME_INVALID = -4007
local EVENT_ERR_SEL_OFFSET_INVALID = -4008
local EVENT_ERR_SEL_NO_SENSOR_EVENT = 202
local selLogErrMap = {
    [EVENT_ERR_SEL_OFFSET_INVALID] = reply_bad_request(ParamValueErr, "Skip"),
    [EVENT_ERR_SEL_LIMIT_INVALID] = reply_bad_request(ParamValueErr, "Top"),
    [EVENT_ERR_SEL_LEVEL_INVALID] = reply_bad_request(ParamValueErr, "Level"),
    [EVENT_ERR_SEL_ENTITY_INVALID] = reply_bad_request(ParamValueErr, "Subject"),
    [EVENT_ERR_SEL_BEGIN_TIME_INVALID] = reply_bad_request(ParamValueErr, "BeginTime"),
    [EVENT_ERR_SEL_END_TIME_INVALID] = reply_bad_request(ParamValueErr, "EndTime"),
    [EVENT_ERR_SEL_SEARCH_STR_INVALID] = reply_bad_request(ParamValueErr, "SearchString"),
    [EVENT_ERR_SEL_NO_SENSOR_EVENT] = reply_ok()
}

local SSL_UNKNOWN_ERROR = 0x00020000 -- 未知错误
local SSL_INVALID_PARAMETER = 0x00020001 -- 无效参数
local SSL_SENSOR_NOT_EXIST = 0x00020003 -- 此传感器不存在
local SSL_INVALID_DATA = 0x00020004 -- 无效数据
local SSL_SCANNING_DISABLE = 0x00020005 -- 禁止扫描
local SSL_SDR_INITIAL_FAILED = 0x00020006 -- SDR初始化失败
local SSL_INITIAL_FAILED = 0x00020007 -- 初始化失败
local SSL_IPMI_MESSAGE_SEND_FAILED = 0x00020008 -- 消息请求发送失败
local SSL_INVALID_USERNAME = 0x00020009 -- 无效的用户名
local SSL_USER_NOT_EXIST = 0x0002000A -- 该用户不存在
local SSL_FUNCTION_NOT_SUPPORT = 0x0002000B -- 功能不支持
local SSL_DEVICE_NOT_PRESENT = 0x0002000C -- 设备不在位
local SSL_INTERNAL_ERROR = 0x0002000D -- 内部错误
local SSL_CMD_RESPONSE_TIMEOUT = 0x0002000E -- 命令响应超时
local SSL_NO_DATA = 0x0002000F -- 无数据,数据为空
local SSL_WEAK_CA_MD = 0x00020010 -- 加密方式太弱
local SSL_CACERT_OVERDUED = 8 -- 证书过期
local selLogSSLErrMap = {
    [SSL_UNKNOWN_ERROR] = reply_bad_request("SSL_UNKNOWN_ERROR"),
    [SSL_INVALID_PARAMETER] = reply_bad_request("SSL_INVALID_PARAMETER"),
    [SSL_SENSOR_NOT_EXIST] = reply_bad_request("SSL_SENSOR_NOT_EXIST"),
    [SSL_INVALID_DATA] = reply_bad_request("SSL_INVALID_DATA"),
    [SSL_SCANNING_DISABLE] = reply_bad_request("SSL_SCANNING_DISABLE"),
    [SSL_SDR_INITIAL_FAILED] = reply_bad_request("SSL_SDR_INITIAL_FAILED"),
    [SSL_INITIAL_FAILED] = reply_bad_request("SSL_INITIAL_FAILED"),
    [SSL_IPMI_MESSAGE_SEND_FAILED] = reply_bad_request("SSL_IPMI_MESSAGE_SEND_FAILED"),
    [SSL_INVALID_USERNAME] = reply_bad_request("SSL_INVALID_USERNAME"),
    [SSL_USER_NOT_EXIST] = reply_bad_request("SSL_USER_NOT_EXIST"),
    [SSL_FUNCTION_NOT_SUPPORT] = reply_bad_request("SSL_FUNCTION_NOT_SUPPORT"),
    [SSL_DEVICE_NOT_PRESENT] = reply_bad_request("SSL_DEVICE_NOT_PRESENT"),
    [SSL_INTERNAL_ERROR] = reply_bad_request("SSL_INTERNAL_ERROR"),
    [SSL_CMD_RESPONSE_TIMEOUT] = reply_bad_request("SSL_CMD_RESPONSE_TIMEOUT"),
    [SSL_NO_DATA] = reply_bad_request("SSL_NO_DATA"),
    [SSL_WEAK_CA_MD] = reply_bad_request("EncryptionAlgorithmIsWeak"),
    [RF_ERROR] = reply_bad_request("METHOD_FAILED")
}

local SYSLOG_CERT_TYPE_HOST = 1
local SYSLOG_CERT_TYPE_CLIENT = 2
local function selLogSSLErrMapFun(type)
    local errCode = type == SYSLOG_CERT_TYPE_HOST and "CertImportFailed" or "EncryptedCertImportFailed"
    return function(errId)
        if errId == RF_ERROR or errId == SSL_INVALID_DATA or errId == SSL_NO_DATA or errId == SSL_INTERNAL_ERROR or
            errId == SSL_INVALID_PARAMETER then
            return reply_bad_request(errCode)
        elseif errId == SSL_WEAK_CA_MD then
            return reply_bad_request("EncryptionAlgorithmIsWeak")
        elseif errId == SSL_CACERT_OVERDUED then
            return reply_bad_request("CertificateExpired")
        end
    end
end

-- 定义导入Cert证书或CRL通用错误码
local IMPORT_CERT_URI_DISMATCH_ERR = 10 -- URI不满足正则匹配
local IMPORT_CERT_INVALID_FILEPATH_ERR = 11 -- 本地导入文件路径无效
local IMPORT_CERT_FILE_LEN_EXCEED_ERR = 12 -- 文件超长
local IMPORT_CRL_ERR_FORMAT_ERR = 0x100
local IMPORT_CRL_ERR_ISSUER_VERIFY_FAIL = 0x101
local IMPORT_CRL_ERR_ROOT_CERT_NOT_IMPORT = 0x102
local IMPORT_CRL_ERR_ISSUING_DATE_INVALID = 0x103
local IMPORT_CRL_ERR_NOT_SUPPORT_CRL_SIGN = 0x104

local importCrlErrMap = {
    [IMPORT_CERT_URI_DISMATCH_ERR] = reply_bad_request("PropertyValueFormatError"),
    [IMPORT_CERT_INVALID_FILEPATH_ERR] = reply_bad_request("CrlImportFailed"),
    [IMPORT_CERT_FILE_LEN_EXCEED_ERR] = reply_bad_request("CrlImportFailed"),
    [IMPORT_CRL_ERR_FORMAT_ERR] = reply_bad_request("CrlFileFormatError"),
    [IMPORT_CRL_ERR_ISSUER_VERIFY_FAIL] = reply_bad_request("RootCertificateMismatch"),
    [IMPORT_CRL_ERR_ROOT_CERT_NOT_IMPORT] = reply_bad_request("RootCertificateNotImported"),
    [IMPORT_CRL_ERR_ISSUING_DATE_INVALID] = reply_bad_request("CrlIssuingDateInvalid"),
    [IMPORT_CRL_ERR_NOT_SUPPORT_CRL_SIGN] = reply_bad_request("CANotSupportCrlSignature")
}

local DELETE_CRL_NOT_EXISTED_ERR = 0x110 -- CRL文件不存在
local deleteCrlErrMap = {
    [DELETE_CRL_NOT_EXISTED_ERR] = reply_bad_request("CrlNotExisted"),
}

local SYSLOG_MGNT_NET_PTL_UDP = 0x01
local SYSLOG_MGNT_NET_PTL_TCP = 0x02
local SYSLOG_MGNT_NET_PTL_TLS = 0x03
local RF_SYSLOG_PROTOCOL_TLS = "TLS"
local RF_SYSLOG_PROTOCOL_TCP = "TCP"
local RF_SYSLOG_PROTOCOL_UDP = "UDP"
local transmission_protocol_arr = {
    [SYSLOG_MGNT_NET_PTL_UDP] = RF_SYSLOG_PROTOCOL_UDP,
    [SYSLOG_MGNT_NET_PTL_TCP] = RF_SYSLOG_PROTOCOL_TCP,
    [SYSLOG_MGNT_NET_PTL_TLS] = RF_SYSLOG_PROTOCOL_TLS
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

local SYSLOG_MGNT_AUTH_TYPE_ONE = 0x01
local SYSLOG_MGNT_AUTH_TYPE_TWO = 0x02
local RF_AUTH_MODE_ONE_WAY = "OneWay"
local RF_AUTH_MODE_TWO_WAY = "TwoWay"
local authenticate_mode_arr = {
    [SYSLOG_MGNT_AUTH_TYPE_ONE] = RF_AUTH_MODE_ONE_WAY,
    [SYSLOG_MGNT_AUTH_TYPE_TWO] = RF_AUTH_MODE_TWO_WAY
}

local SYSLOG_MGNT_MSG_FORMAT_CUSTOM = 0
local SYSLOG_MGNT_MSG_FORMAT_RFC3164 = 1
local RF_SYSLOG_MSG_FORMAT_CUSTOM = "Custom"
local RF_SYSLOG_MSG_FORMAT_RFC3164 = "RFC3164"
local syslog_mgnt_msg_format_arr = {
    [SYSLOG_MGNT_MSG_FORMAT_CUSTOM] = RF_SYSLOG_MSG_FORMAT_CUSTOM,
    [SYSLOG_MGNT_MSG_FORMAT_RFC3164] = RF_SYSLOG_MSG_FORMAT_RFC3164
}

local event_subject_type = {
    'CPU',
    'Memory',
    'Disk',
    'PSU',
    'Fan',
    'Disk Backplane',
    'RAID Card',
    SUBJECT_TYPE_UNKNOWN,
    'PCIe Card',
    'AMC',
    'HBA',
    'Mezz Card',
    SUBJECT_TYPE_UNKNOWN,
    'NIC',
    'Memory Board',
    'PCIe Riser',
    'Mainboard',
    'LCD',
    'Chassis',
    'NCM',
    'Switch Module',
    'Storage Board',
    'Chassis Backplane',
    'HMM/CMC',
    'Fan Backplane',
    'PSU Backplane',
    'BMC',
    'MM/MMC',
    'Twin Node Backplane',
    'Base Plane',
    'Fabric Plane',
    'Switch Mezz',
    'LED',
    'SD Card',
    'Security Module',
    'I/O Board',
    'CPU Board',
    'RMC',
    'PCIe Adapter',
    'PCH',
    'Cable',
    'Port',
    'LSW',
    'PHY',
    'System',
    'M.2 Transfer Card',
    'LED Board',
    'LPM',
    'PIC Card',
    'Button',
    'Expander',
    'CPI',
    'ACM',
    'CIM',
    'PFM',
    'KPAR',
    'JC',
    'SCM',
    'Minisas HD channel',
    'SATA DOM channel',
    'GE channel',
    'XGE channel',
    'PCIe Switch',
    'Interface Device',
    'xPU Carrier Board',
    'Disk BaseBoard',
    'VGA Interface Card',
    'Pass-Through Card',
    'Logical Driver',
    'PCIe Retimer',
    'PCIe Repeater',
    'SAS',
    'Memory Channel',
    'BMA',
    'LOM',
    'Signal Adapter Board',
    'Horizontal Connection Board',
    'Node',
    'Asset Locate Board',
    'Unit',
    'RMM',
    'Rack',
    'BBU',
    'OCP Card',
    'Leakage Detection Card',
    'MESH Card',
    'NPU',
    'CIC Card',
    "Expansion Module",
    "Fan Module",
    "AR Card",
    SUBJECT_TYPE_UNKNOWN,
    'SoC Board',
    "Expand Board"
}

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

-- 事件源 bit0:操作日志 bit1:安全日志 bit2:系统事件
local SYSLOG_MGNT_SRC_OP = 0x01
local SYSLOG_MGNT_SRC_SEC = 0x02
local SYSLOG_MGNT_SRC_SEL = 0x04
local RF_SYSLOG_ITEM_LOG_OP = "OperationLog"
local RF_SYSLOG_ITEM_LOG_SEC = "SecurityLog"
local RF_SYSLOG_ITEM_LOG_SEL = "EventLog"
local function syslog_item_get_logtype(log_type)
    local ret = {}
    if bit.band(log_type, SYSLOG_MGNT_SRC_OP) ~= 0 then
        ret[#ret + 1] = RF_SYSLOG_ITEM_LOG_OP
    end
    if bit.band(log_type, SYSLOG_MGNT_SRC_SEC) ~= 0 then
        ret[#ret + 1] = RF_SYSLOG_ITEM_LOG_SEC
    end
    if bit.band(log_type, SYSLOG_MGNT_SRC_SEL) ~= 0 then
        ret[#ret + 1] = RF_SYSLOG_ITEM_LOG_SEL
    end
    return ret
end

local function get_logype_by_array(logtypes)
    local ret = 0
    for _, s in ipairs(logtypes) do
        if s == RF_SYSLOG_ITEM_LOG_OP then
            ret = bit.bor(ret, SYSLOG_MGNT_SRC_OP)
        elseif s == RF_SYSLOG_ITEM_LOG_SEC then
            ret = bit.bor(ret, SYSLOG_MGNT_SRC_SEC)
        elseif s == RF_SYSLOG_ITEM_LOG_SEL then
            ret = bit.bor(ret, SYSLOG_MGNT_SRC_SEL)
        end
    end
    return ret
end

local function parseDateTime(str)
    local y, m, d, h, M, s = str:match("^(%d+)-(%d+)-(%d+) (%d+):(%d+):(%d+)$")
    if y then
        return os.time {year = y, month = m, day = d, hour = h, min = M, sec = s}
    end
end

local function __get_cert_info_from_output(output_list)
    return {
        Issuer = output_list[1],
        Subject = output_list[2],
        ValidNotBefore = output_list[3],
        ValidNotAfter = output_list[4],
        SerialNumber = output_list[5]
    }
end

local function GetRootCertificate(user, obj)
    local ok, ret = call_method(user, obj, "GetCertInfo", selLogSSLErrMap, gbyte(SYSLOG_CERT_TYPE_HOST))
    if not ok then
        logging:error("get RootCertificate failed: %s", ret)
        return
    end

    local RootCertificate = __get_cert_info_from_output(ret)
    RootCertificate.CrlVerificationEnabled = obj.CRLVerificationEnabled:fetch() == 1
    RootCertificate.CrlValidFrom = obj.CRLStartTime:fetch()
    RootCertificate.CrlValidTo = obj.CRLExpireTime:fetch()
    return RootCertificate
end

local function GetClientCertificate(user, obj)
    local ok, ret = call_method(user, obj, "GetCertInfo", selLogSSLErrMap, gbyte(SYSLOG_CERT_TYPE_CLIENT))
    if not ok then
        logging:error("get ClientCertificate failed: %s", ret)
        return
    end
    return __get_cert_info_from_output(ret)
end

local function GetSyslogServerList()
    return C.SyslogItemCfg():fold(function(obj, acc)
        local ID = obj.Index:fetch_or()
        local IP = obj.DestAddr:fetch_or()
        local Port = obj.DestPort:fetch_or()
        local LogType = syslog_item_get_logtype(obj.LogSrcMask:fetch())
        local Enabled = obj.EnableState:fetch() == 1 and true or false
        acc[#acc + 1] = {ID = ID, IP = IP, Port = Port, LogType = LogType, Enabled = Enabled}
        return acc
    end, {}):fetch()
end

local function SetEnabled(user, obj, EnableState)
    if EnableState == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetEnableState", nil, gbyte(EnableState and 1 or 0))
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
        local v = str_to_val(server_identity_source_arr, ServerIdentity) + 1
        local ok, ret = call_method(user, obj, "SetMsgIdentity", function(errId)
            if errId == VOS_ERR_NOTSUPPORT then
                return reply_bad_request("NotSupport")
            end
        end, guint32(v))
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
        local ok, ret = call_method(user, obj, "SetMsgSeverity", nil, guint32(v))
        if not ok then
            return ret
        end
    end)
end

local function SetTransmissionProtocol(user, obj, TransmissionProtocol)
    if TransmissionProtocol == nil then
        return
    end

    return safe_call(function()
        local v = str_to_val(transmission_protocol_arr, TransmissionProtocol)
        local ok, ret = call_method(user, obj, "SetNetProtocol", nil, gbyte(v))
        if not ok then
            return ret
        end
    end)
end

local function SetAuthenticateMode(user, obj, AuthenticateMode)
    if AuthenticateMode == nil then
        return
    end

    return safe_call(function()
        local v = str_to_val(authenticate_mode_arr, AuthenticateMode)
        local ok, ret = call_method(user, obj, "SetAuthType", nil, gbyte(v))
        if not ok then
            return ret
        end
    end)
end

local function SetMessageFormat(user, obj, MessageFormat)
    if MessageFormat == nil then
        return
    end

    return safe_call(function()
        local v = str_to_val(syslog_mgnt_msg_format_arr, MessageFormat)
        local ok, ret = call_method(user, obj, "SetMsgFormat", nil, gbyte(v))
        if not ok then
            return ret
        end
    end)
end

local function SetEnableState(user, obj, Enabled)
    if Enabled == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetEnableState", nil, gbyte(Enabled and 1 or 0))
        if not ok then
            return ret
        end
    end)
end

local function SetDestAddr(user, obj, IP)
    if IP == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetDestAddr", nil, gstring(IP))
        if not ok then
            return ret
        end
    end)
end

local function SetDestPort(user, obj, Port)
    if Port == nil then
        return
    end

    return safe_call(function()
        local ok, ret = call_method(user, obj, "SetDestPort", nil, guint32(Port))
        if not ok then
            return ret
        end
    end)
end

local function SetLogSrcMask(user, obj, LogType)
    if LogType == nil then
        return
    end

    return safe_call(function()
        local v = get_logype_by_array(LogType)
        local ok, ret = call_method(user, obj, "SetLogSrcMask", nil, guint32(v))
        if not ok then
            return ret
        end
    end)
end

local function SetSyslogServerList(user, result, SyslogServerList)
    if SyslogServerList == nil then
        return
    end

    local syslog_item_count = dflib.object_count("SyslogItemCfg")
    if #SyslogServerList > syslog_item_count then
        result:join(reply_bad_request("PropertyMemberQtyExceedLimit"))
        return
    end

    for _, server in ipairs(SyslogServerList) do
        C.SyslogItemCfg[server.ID]:next(function(obj)
            result:join(SetEnableState(user, obj, server.Enabled))
            result:join(SetDestAddr(user, obj, server.IP))
            result:join(SetDestPort(user, obj, server.Port))
            result:join(SetLogSrcMask(user, obj, server.LogType))
        end):catch()
    end
end

local function ProcessSelLog(ret)
    local TotalCount = 0
    if #ret > 0 then
        TotalCount = ret[1]
    end

    local res = {}
    local index = 2
    while index <= #ret do
        local row = {}
        local column = ret[index]
        index = index + 1
        for _ = 1, column do
            local key = ret[index]
            local value = ret[index + 1]
            row[key] = value
            index = index + 2
        end
        res[#res + 1] = {
            ID = tonumber(row.eventid),
            Level = get_event_level(tonumber(row.level)),
            Subject = tonumber(row.subjecttype),
            SubjectName = event_subject_type[tonumber(row.subjecttype) + 1],
            Content = row.eventdesc,
            CreatedTime = row.alerttime,
            EventCode = row.eventcode,
            HandlingSuggestion = row.eventsugg,
            Status = row.status
        }
    end
    return {List = res, TotalCount = TotalCount}
end

local function check_and_transforms_level(rawLevel)
    local retLevel = 0
    for k, v in pairs(rawLevel) do
        -- 检查入参是否在范围内
        if not levelsMap[v] then
            logging:error("Invalid alarm level exists")
            return nil
        end
        -- 实现一个int32数据包含多个告警等级：bit0为1表示正常，bit1为1表示轻微，bit2为1表示严重，bit3为1表示紧急
        retLevel = retLevel + 2 ^ levelsMap[v]
        -- 检查入参是否重复：如果重复，returnLevel右移levelsMap[v]位后bit0为0.Lua无位运算且5.3及以上版本才有整除
        if ((retLevel - (retLevel % (2 ^ levelsMap[v]))) / (2 ^ levelsMap[v])) % 2 == 0 then
            logging:error("Duplicate alarm level exist.")
            return nil
        end
    end
    -- 优化全选为全部，能执行到此处时，rawLevel中最多有4个元素
    if #rawLevel == 4 then
        retLevel = 255
    end

    return retLevel
end

local function check_and_transforms_subject(rawSubject)
    local retSubject = ""

    local selSubjectCheck = {}
    local types = {}
    local count = 0
    for k, v in pairs(utils.get_event_subtype()) do
        types[v] = k -- 取出当前环境所有主体类型及对应的ID
        selSubjectCheck[v] = 1 -- 主体类型存在标记为1
        count = count + 1 -- 计数
    end

    for k, v in pairs(rawSubject) do
        -- 检查入参是否在范围内
        if not types[v] then
            logging:error("Invalid subject type exists")
            return nil
        end
        -- 检查入参是否重复，检查位不为1即重复
        if selSubjectCheck[v] ~= 1 then
            logging:error("Duplicate subject type exist.")
            return nil
        end
        selSubjectCheck[v] = 0 -- 检测到一个符合的主体类型，计数减1，检查位置0
        count = count - 1
        if k == 1 then
            retSubject = retSubject .. tostring(types[v])
        else
            retSubject = retSubject .. "," .. tostring(types[v])
        end
    end
    -- 优化全选到全部
    if count == 0 then
        retSubject = ""
    end

    return retSubject
end

local M = {}

function M.GetAlarm()
    return O.Warning:next(function(obj)
        local results = {}
        local record_id = 0

        -- record_id 值不为0xffff时循环获取SEL
        while record_id ~= 0xFFFF do
            local ok, ret = call_method(nil, obj, "GetEventItem", selLogErrMap, {guint16(record_id)})
            if not ok and ret:isOk() then
                return reply_ok_encode_json({TotalCount = 0, List = {}})
            end
            results[#results + 1] = {
                ID = record_id,
                Level = get_event_level(ret[5]),
                Subject = ret[2],
                Content = ret[7],
                CreatedTime = get_datetime(ret[4]),
                EventCode = format("0x%08X", ret[6]),
                HandlingSuggestion = rf_string_check(ret[8])
            }
            record_id = ret[1]
        end
        return {TotalCount = #results, List = results}
    end):fetch()
end

function M.GetEventObjectType()
    local types = {}
    for k, v in pairs(utils.get_event_subtype()) do
        types[#types + 1] = {ID = k, Label = v}
    end
    return {List = types, TotalCount = #types}
end

function M.GetEvent(data, user)
    local selLevel = nil
    if data.Level then
        selLevel = check_and_transforms_level(data.Level)
        if not selLevel then
            return reply_bad_request(nil, "invalid Level")
        end
    end
    
    local selSubject = nil
    if data.Subject then
        selSubject = check_and_transforms_subject(data.Subject)
        if not selSubject then
            return reply_bad_request(nil, "invalid Subject")
        end
    end

    local beginTime = -1
    local endTime = -1
    local timeZone = get_timestamp_offset()
    if data.BeginTime then
        beginTime = parseDateTime(data.BeginTime)
        if not beginTime then
            return reply_bad_request(nil, "invalid BeginTime")
        end
        beginTime = beginTime + timeZone * 60
    end

    if data.EndTime then
        endTime = parseDateTime(data.EndTime)
        if not endTime then
            return reply_bad_request(nil, "invalid EndTime")
        end
        endTime = endTime + timeZone * 60
    end

    return C.Sel[0]:next(function(obj)
        local ok, ret = call_method(user, obj, "QuerySelLog", selLogErrMap, {
            gint32(data.Top), gint32(data.Skip), gstring("en"), gstring(data.SearchString or ""),
            gint32(selLevel or 255), gstring(selSubject or ""), gint32(beginTime), gint32(endTime)
        })
        if not ok then
            return ret
        end
        return ProcessSelLog(ret)
    end):fetch()
end

function M.DownloadEvent(user)
    return C.Sel[0]:next(function(obj)
        local collect_cb = function()
            local ok, ret = utils.call_method_async(user, obj, "CollectSel", nil, gstring(SEL_TAR_PATH))
            if not ok then
                return ret
            end
            return reply_ok()
        end

        local progress_cb = function(filePath)
            if not dal_set_file_owner("/tmp/sel.tar", cfg.REDFISH_USER_UID, cfg.APPS_USER_GID) then
                error('chown sel.tar failed')
            end
            if not dal_set_file_mode("/tmp/sel.tar", cfg.FILE_MODE_600) then
                error('chmod sel.tar failed')
            end
            -- 文件能以可写模式打开就认为文件准备好了
            local file, _ = io.open("/tmp/sel.tar", "a+")
            if file then
                file:close()
                tools.movefile("/tmp/sel.tar", filePath)
                return 100
            end
            return 0
        end

        return download(user, 'sel.tar', 'system event log', defs.DOWNLOAD_SEL, collect_cb, progress_cb)
    end):catch(function(err)
        return reply_bad_request("ActionNotSupported", err)
    end):fetch()
end

function M.ClearEvent(user)
    return C.Sel[0]:next(function(obj)
        local ok, ret = utils.call_method_async(user, obj, "ClearSel", nil, nil)
        if not ok then
            return ret
        end
        return reply_ok()
    end):fetch()
end

function M.GetSyslogNotification(user)
    return C.SyslogConfig[0]:next(function(obj)
        local TransmissionProtocol = transmission_protocol_arr[obj.NetProtocol:fetch()]
        local AuthenticateMode = authenticate_mode_arr[obj.AuthType:fetch()]
        local RootCertificate = nil
        local ClientCertificate = nil
        if TransmissionProtocol == "TLS" then
            RootCertificate = GetRootCertificate(user, obj)
            if AuthenticateMode == "TwoWay" then
                ClientCertificate = GetClientCertificate(user, obj)
            end
        end
        local SyslogServerList = GetSyslogServerList()
        return {
            TransmissionProtocol = TransmissionProtocol,
            EnableState = obj.EnableState:fetch() ~= 0 and true or false,
            AlarmSeverity = redfish_alarm_severities_change(obj.MsgSeverity:fetch()),
            MessageFormat = syslog_mgnt_msg_format_arr[obj.MsgFormat:fetch()],
            ServerIdentity = server_identity_source_arr[obj.MsgIdentity:fetch() - 1],
            AuthenticateMode = AuthenticateMode,
            SyslogServerList = SyslogServerList,
            RootCertificate = RootCertificate,
            ClientCertificate = ClientCertificate
        }
    end):fetch()
end

function M.UpdateSyslogNotification(data, user)
    return C.SyslogConfig[0]:next(function(obj)
        local result = reply_ok()
        result:join(SetEnabled(user, obj, data.EnableState))
        result:join(SetMessageFormat(user, obj, data.MessageFormat))
        result:join(SetServerIdentity(user, obj, data.ServerIdentity))
        result:join(SetAlarmSeverity(user, obj, data.AlarmSeverity))
        result:join(SetTransmissionProtocol(user, obj, data.TransmissionProtocol))
        result:join(SetAuthenticateMode(user, obj, data.AuthenticateMode))
        SetSyslogServerList(user, result, data.SyslogServerList)

        if result:isOk() then
            return reply_ok_encode_json(M.GetSyslogNotification(user))
        end
        result:appendErrorData(M.GetSyslogNotification(user))
        return result
    end):fetch()
end

function M.TestSyslogNotification(data, user)
    local syslog_item_count = dflib.object_count("SyslogItemCfg")
    if data.ID >= syslog_item_count or data.ID < 0 then
        return reply_bad_request(nil, "invalid member id")
    end

    return C.SyslogItemCfg[data.ID]:next(function(obj)
        local ok, ret = call_method(user, obj, "SetTestTrigger", function(errId)
            if errId == RF_ERROR then
                return reply_bad_request("SyslogTestFailed")
            end
        end, gbyte(data.ID))
        if not ok then
            return ret
        end
        return reply_ok()
    end):fetch()
end

function M.ImportSyslogRootCertificate(data)
    local user = data.user
    if utils.CheckFormData(data) == false then
        logging:error('Failed to import syslog root certificate')
        return reply_bad_request('InvalidParam')
    end
    local filePath = utils.GetFormDataFilePath(data)
    if not filePath then
        return reply_bad_request("InvalidFile")
    end

    return C.SyslogConfig[0]:next(function(obj)
        local input = {gbyte(SYSLOG_CERT_TYPE_HOST), gstring(filePath), gstring("NULL")}
        local errMap = selLogSSLErrMapFun(SYSLOG_CERT_TYPE_HOST)
        local ok, ret = call_method(user, obj, "ImportCert", errMap, input)
        if not ok then
            return ret
        end
        return reply_ok()
    end):fetch()
end

function M.ImportSyslogClientCertificate(data)
    local user = data.user
    if utils.CheckFormData(data) == false then
        logging:error('Failed to import syslog client certificate')
        return reply_bad_request('InvalidParam')
    end
    local filePath = utils.GetFormDataFilePath(data)
    if not filePath then
        return reply_bad_request("InvalidFile")
    end

    return C.SyslogConfig[0]:next(function(obj)
        local pwd = data.fields.Password or ''
        local input = {gbyte(SYSLOG_CERT_TYPE_CLIENT), gstring(filePath), gstring(pwd)}
        local errMap = selLogSSLErrMapFun(SYSLOG_CERT_TYPE_CLIENT)
        local ok, ret = call_method(user, obj, "ImportCert", errMap, input)
        if not ok then
            return ret
        end
        return reply_ok()
    end):fetch()
end

function M.ImportSyslogCrl(data)
    local user = data.user
    if utils.CheckFormData(data) == false then
        logging:error('Failed to import syslog crl')
        return reply_bad_request('InvalidParam')
    end
    local filePath = utils.GetFormDataFilePath(data)
    if not filePath then
        return reply_bad_request("InvalidFile")
    end

    return C.SyslogConfig[0]:next(function(obj)
        local ok, ret = call_method(user, obj, "ImportCRL", importCrlErrMap, gstring(filePath))
        if not ok then
            return ret
        end
        return reply_ok()
    end):fetch()
end

function M.DeleteSyslogCrl(user)
    return C.SyslogConfig[0]:next(function(obj)
        local ok, ret = call_method(user, obj, "DeleteCRL", deleteCrlErrMap)
        if not ok then
            return ret
        end
        return reply_ok()
    end):fetch()
end

return M
