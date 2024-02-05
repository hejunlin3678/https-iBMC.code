local utils = require 'utils'
local defs = require 'define'
local dflib = require 'dflib'
local c = require 'dflib.core'
local cjson = require 'cjson'
local tools = require 'tools'
local http = require 'http'
local logging = require 'logging'
local tasks = require 'tasks'
local cfg = require 'config'
local download = require 'download'
local webtask = require 'webrest.task'
local C = dflib.class
local O = dflib.object
local null = cjson.null
local GVariant = c.GVariant
local get_file_accessible = c.get_file_accessible
local gbyte = GVariant.new_byte
local gint32 = GVariant.new_int32
local gstring = GVariant.new_string
local call_method = utils.call_method
local call_method_async = utils.call_method_async
local safe_call = http.safe_call
local reply_internal_server_error = http.reply_internal_server_error
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_ok = http.reply_ok
local reply_bad_request = http.reply_bad_request
local BoolToValue = utils.BoolToValue
local file_exists = utils.file_exists
local proxy_copy_file = utils.proxy_copy_file
local dal_set_file_owner = utils.dal_set_file_owner
local dal_set_file_mode = utils.dal_set_file_mode
local OperateLog = utils.OperateLog
local format = string.format
local DFL_OK = dflib.DFL_OK

local BLACKBOX_DATA_SIZE = 4 * 1024 * 1024
local SERIAL_DATA_SIZE_1710 = 2 * 1024 * 1024
local SOL_SYSTEMCOM = 1

local CAPTURE_SCREENSHOT = 'CaptureScreenshot'
local STATE_NOT_COMPLETED = 'Not Completed'
local STATE_COMPLETED = 'Completed'
local PROPERTY_POWEROFF = 'PoweroffVideo'
local PROPERTY_OSRESET = 'OsresetVideo'
local PROPERTY_CATERROR = 'CaterrorVideo'
local PROPERTY_LOCAL = 'LocalVideo'
local VIDEO_NAME_POWEROFF = 'video_poweroff.rep'
local VIDEO_NAME_OSRESET = 'video_osreset.rep'
local VIDEO_NAME_CATERROR = 'video_caterror.rep'
local VIDEO_PATH_POWEROFF = '/tmp/poweroff/video_poweroff.rep'
local VIDEO_PATH_OSRESET = '/tmp/osreset/video_osreset.rep'
local VIDEO_PATH_CATERROR = '/data/share/video/video_caterror.rep'
local VIDEO_CATERROR = 'caterror video'
local VIDEO_POWEROFF = 'poweroff video'
local VIDEO_OSRESET = 'osreset video'
local SCREEN_FILE_WEB_PATH = '/tmp/web/'
local IMG_PATH = '/data/share/img/manualscreen.jpeg'
local IMG_NAME = 'manualscreen.jpeg'
local IMAGE_DIR = '/data/share/img/'
local IMAGE_PATH = {
    '/data/share/img/img1.jpeg', '/data/share/img/img2.jpeg', '/data/share/img/img3.jpeg',
    '/data/share/img/manualscreen.jpeg'
}
local VIDEO_FILES = {VIDEO_PATH_CATERROR, VIDEO_PATH_POWEROFF, VIDEO_PATH_OSRESET}
local VIDEO_PATH_AND_NAME_MAP = {
    [PROPERTY_POWEROFF] = {
        VideoPath = VIDEO_PATH_POWEROFF,
        VideoName = VIDEO_NAME_POWEROFF,
        Category = VIDEO_POWEROFF
    },
    [PROPERTY_OSRESET] = {VideoPath = VIDEO_PATH_OSRESET, VideoName = VIDEO_NAME_OSRESET, Category = VIDEO_OSRESET},
    [PROPERTY_CATERROR] = {
        VideoPath = VIDEO_PATH_CATERROR,
        VideoName = VIDEO_NAME_CATERROR,
        Category = VIDEO_CATERROR
    }
}

local SolDdrSize = {1, 2, 4, 8, 16, 32, 64, 128}

local TIMEOUT_SEC = 10
local VIDEO_JNLP_FORMAT = '<?xml version="1.0" encoding="UTF-8"?>\
	<jnlp spec="1.0+" codebase="https://%s">\
		<information>\
			<title>Video Player   IP : %s   SN : %s</title>\
			<vendor>iBMC</vendor>\
		</information>\
		<resources>\
			<j2se version="1.7+" />\
			<jar href="/bmc/pages/jar/%s?authParam=%s" main="true"/>\
		</resources>\
		<applet-desc name="Video Player   IP : %s   SN : %s" main-class="com.huawei.kinescope.ui.KinAppletPlay" width="950" height="700" >\
			<param name="local" value="%s"/>\
			<param name="videoType" value="%d"/>\
			<param name="press" value="%d"/>\
			<param name="salt" value="%s"/>\
			<param name="videokey" value="%s"/>\
			<param name="port" value="%d"/>\
			<param name="bladesize" value="1"/>\
			<param name="ip" value="%s"/>\
		</applet-desc>\
		<security>\
			<all-permissions/>\
		</security>\
    </jnlp>'

local VIDEO_UNCONNECTED = -1001
local STOP_VIDEO_ERR_MAP = {
    [VIDEO_UNCONNECTED] = reply_bad_request('VideoLinkNotExist', 'The recording link does not exist')
}

local ERR_SCREENSHOT_STATE = 0xFF
local CAPTURE_SCREEN_ERR_MAP = {
    [ERR_SCREENSHOT_STATE] = reply_bad_request('DisableScreenShotInCurrentState',
        'The current status does not support manual screenshots')
}

local function GetFilePath(videoType)
    if videoType == PROPERTY_POWEROFF then
        return 1, 1, VIDEO_PATH_POWEROFF
    elseif videoType == PROPERTY_OSRESET then
        return 3, 1, VIDEO_PATH_OSRESET;
    elseif videoType == PROPERTY_CATERROR then
        return 2, 1, VIDEO_PATH_CATERROR
    elseif videoType == PROPERTY_LOCAL then
        return 0, 0
    else
        error('Video Type Unknown')
    end
end

local function get_file_create_time(file)
    local long_time = c.file_stat(file).st_ctime
    return os.date('%Y-%m-%d %H:%M:%S', long_time)
end

local function CaptureScreen(user, saveScreen)
    local info = {State = STATE_NOT_COMPLETED, ScreenshotCreateTime = nil}
    local error = nil
    -- URL of the registration progress query interface
    local url = webtask.addTaskInfo(CAPTURE_SCREENSHOT, defs.DOWNLOAD_PICTURE, function(_)
        -- Check error information.
        if error then
            if http.is_reply(error) then
                return error
            end
            return reply_internal_server_error(nil, error)
        end
        if file_exists(IMG_PATH) then
            info.ScreenshotCreateTime = get_file_create_time(IMG_PATH)
            info.State = STATE_COMPLETED
        end
        return reply_ok_encode_json(info)
    end)

    -- Task repetition check. Only one screenshot task is allowed at a time.
    if url == nil then
        return reply_bad_request('DuplicateExportingErr')
    end

    local timeout = TIMEOUT_SEC
    local watchLoop = function()
        -- 1、start screenshot
        local ok, ret = pcall(saveScreen)
        if not ok or not http.is_reply(ret) then
            local errMsg = type(ret) == 'string' and ret or cjson.encode(ret)
            logging:error('prepare capture screen shot failed: %s', errMsg)
            error = reply_internal_server_error('CaptureScreenshotFailed', ret)
            return
        elseif not ret:isOk() then
            logging:error('prepare capture screen shot failed: %s', cjson.encode(ret:body()))
            error = reply_internal_server_error('CaptureScreenshotFailed', ret)
            return
        end
        -- 2、cyclic check whether the screenshot is successful.
        while true do
            if file_exists(IMG_PATH) then
                break
            end
            tasks.sleep(1000)
            timeout = timeout - 1
            if timeout <= 0 then
                logging:error('capture screen shot timeout (%d s)', TIMEOUT_SEC)
                error = reply_bad_request('CaptureScreenshotFailed', 'capture screen shot timeout')
                return
            end
        end
        local screen_path = SCREEN_FILE_WEB_PATH .. IMG_NAME
        if not proxy_copy_file(user, IMG_PATH, screen_path, cfg.REDFISH_USER_UID, cfg.APPS_USER_GID, 0) then
            error('copy screenshot failed')
        end
    end
    -- create a task to control the screenshot logic
    tasks.start(function()
        -- Name a task. Only one task can be created for a file at a time.
        tasks.register(CAPTURE_SCREENSHOT)
        -- enter the monitoring cycle
        local ok, err = pcall(watchLoop)
        webtask.finishTaskInfo(url)
        if not ok then
            logging:error(err)
        end
        tasks.start(function()
            tasks.sleep(1000 * 60)
            webtask.delTaskInfo(url)
            logging:info('remove monitor progress url %s', url)
            tasks.exit()
        end)
        tasks.exit()
    end)
    return reply_ok_encode_json({url = '/UI/Rest/task/' .. url})
end

local function CopyScreenshotImgToTmpPath(user)
    if not file_exists(SCREEN_FILE_WEB_PATH) then
        assert(c.system_s('/bin/mkdir', '-p', SCREEN_FILE_WEB_PATH) == DFL_OK, 'create screenshot directory failed')
        assert(c.system_s('/bin/chmod', '777', SCREEN_FILE_WEB_PATH) == DFL_OK, 'chmod screenshot directory failed')
    end
    for _, value in pairs(IMAGE_PATH) do
        if file_exists(value) then
            local screen_path = SCREEN_FILE_WEB_PATH .. value:sub(#IMAGE_DIR + 1)
            if not proxy_copy_file(user, value, screen_path, cfg.REDFISH_USER_UID, cfg.APPS_USER_GID, 0) then
                error('copy screenshot failed')
            end
        end
    end
end

local function get_screenshot_create_time()
    local result = {}
    for i = 1, #IMAGE_PATH do
        if not get_file_accessible(IMAGE_PATH[i]) then
            result['Time' .. (i - 1)] = ''
        else
            result['Time' .. (i - 1)] = get_file_create_time(IMAGE_PATH[i])
        end
    end
    return result
end

local function get_file_lenth(file)
    local file_len = c.get_file_length(file)
    logging:info('file_len:%u', file_len)
    return file_len
end

local function get_video_info()
    local result = {}
    for _, file in ipairs(VIDEO_FILES) do
        if not get_file_accessible(file) then
            result[#result + 1] = null
        else
            local ctime = get_file_create_time(file)
            local file_len = get_file_lenth(file)
            result[#result + 1] = {VideoSizeByte = file_len, CreateTime = ctime}
        end
    end
    return result
end

local M = {}

local function GetSolDataDirection(comObj)
    return comObj.SolDataDirection:fetch_or()
end

local function GetSolTxSize(comObj)
    return comObj.TxSize:fetch_or()
end

local function GetSolRxSize(comObj)
    return comObj.RxSize:fetch_or()
end

local function GetChipName()
    return O.BMC.ChipName:fetch_or()
end

local function GetSerialPortDataSize()
    if GetChipName() == 'Hi1710' then
        logging:info('bmc chip name is %s', 'Hi1710')
        return SERIAL_DATA_SIZE_1710
    end
    local dataSize = 0
    local refObjNames = O.Diagnose.SolUartObj:fetch()
    for _, refObjName in ipairs(refObjNames) do
        local comObj = O[refObjName]
        local solEnable = comObj.SolEnable:fetch_or()
        if solEnable == nil then
            logging:error('get sol enable failed')
            return nil
        end
        if solEnable == 0 then
            goto continue
        end
        local solDataDirection = GetSolDataDirection(comObj)
        if solDataDirection == nil then
            logging:error('get sol data direction failed')
            return nil
        end
        local configSizeIndex = solDataDirection == 0 and GetSolTxSize(comObj) or GetSolRxSize(comObj)
        if configSizeIndex == nil or configSizeIndex >= #SolDdrSize then
            logging:error('configSizeIndex is invalid')
            return nil
        end
        dataSize = dataSize + SolDdrSize[configSizeIndex + 1] * 1024 * 256
        ::continue::
    end

    if dataSize == 0 then
        logging:error('dataSize is invalid')
        return nil
    end
    return dataSize
end

function M.GetSystemDiagnostic(user)
    CopyScreenshotImgToTmpPath(user)
    local diagnose = C.Diagnose[0]
    local video = C.Video[0]
    local kvm = C.Kvm[0]
    return {
        IsSupportBlackBoxOfSdi = C.PCIeSDICard[0].McuSupported:eq(1):next(
            function()
                return true
            end):fetch_or(false),
        PCIeInterfaceEnabled = diagnose.PCIeInterfaceSwitch:eq(1):next(
            function()
                return true
            end):fetch_or(false),
        BlackBoxEnabled = diagnose.BlackBoxState:eq(1):next(
            function()
                return true
            end):fetch_or(false),
        BlackBoxSize = BLACKBOX_DATA_SIZE,
        SerialPortDataSize = GetSerialPortDataSize(),
        SerialPortDataEnabled = diagnose.SolDataState:eq(1):next(
            function()
                return true
            end):fetch_or(false),
        VideoRecordingEnabled = video.VideoSwitch:eq(1):next(
            function()
                return true
            end):fetch_or(false),
        ScreenshotEnabled = kvm.ScreenSwitch:eq(1):next(function()
            return true
        end):fetch_or(false),
        ScreenshotCreateTime = get_screenshot_create_time(),
        VideoRecordInfo = get_video_info(),
        VideoPlaybackConnNum = video.ConnectNum:fetch_or(null)
    }
end

function M.UpdateSystemDiagnostic(user, data)
    local result = reply_ok()
    if data.PCIeInterfaceEnabled ~= nil then
        result:join(safe_call(function()
            return C.Diagnose[0]:next(function(obj)
                local ok, err = call_method(user, obj, 'SetPCIeInterfaceSwitch', nil,
                    gbyte(BoolToValue(data.PCIeInterfaceEnabled)))
                if not ok then
                    return err
                end
            end):fetch()
        end))
    end
    if data.BlackBoxEnabled ~= nil then
        result:join(safe_call(function()
            return C.Diagnose[0]:next(function(obj)
                local ok, err = call_method(user, obj, 'SetBlackBoxDumpState', nil,
                    gbyte(BoolToValue(data.BlackBoxEnabled)))
                if not ok then
                    return err
                end
            end):fetch()
        end))
    end
    if data.SerialPortDataEnabled ~= nil then
        result:join(safe_call(function()
            return C.Diagnose[0]:next(function(obj)
                local ok, err = call_method(user, obj, 'SetSolDataState', nil,
                    gint32(BoolToValue(data.SerialPortDataEnabled)))
                if not ok then
                    return err
                end
            end):fetch()
        end))
    end
    if data.VideoRecordingEnabled ~= nil then
        result:join(safe_call(function()
            return C.Video[0]:next(function(obj)
                local ok, err = call_method(user, obj, 'SetRecordVideoSwitch', nil,
                    gbyte(BoolToValue(data.VideoRecordingEnabled)))
                if not ok then
                    return err
                end
            end):fetch()
        end))
    end
    if data.ScreenshotEnabled ~= nil then
        result:join(safe_call(function()
            return C.ProxyKvm[0]:next(function(obj)
                local ok, err = call_method(user, obj, 'SetScreenSwitch', nil,
                    gbyte(BoolToValue(data.ScreenshotEnabled)))
                if not ok then
                    return err
                end
            end):fetch()
        end))
    end
    if result:isOk() then
        return reply_ok_encode_json(M.GetSystemDiagnostic())
    end
    result:appendErrorData(M.GetSystemDiagnostic())
    return result
end

function M.DownloadBlackBox(user)
    if C.Diagnose[0].PCIeInterfaceSwitch:fetch() == 0 or C.Diagnose[0].BlackBoxState:fetch() == 0 then
        return reply_bad_request('FeatureDisabledAndNotSupportOperation', 'The black box function is disabled')
    end
    return C.Diagnose[0]:next(function(obj)
        local collect_cb = function()
            local ok, ret = utils.call_method_async(user, obj, 'GetBlackBox')
            if not ok then
                return ret
            end
            return reply_ok()
        end
        local progress_cb = function(file_path)
            return C.Diagnose[0]:next(function(diagnose)
                if diagnose.BlackBoxProcessbar:fetch() == 100 then
                    if not dal_set_file_owner('/tmp/blackbox.tar', cfg.REDFISH_USER_UID, cfg.APPS_USER_GID) then
                        error('chown blackbox failed')
                    end
                    if not dal_set_file_mode('/tmp/blackbox.tar', cfg.FILE_MODE_600) then
                        error('chmod blackbox failed')
                    end
                    tools.movefile('/tmp/blackbox.tar', file_path)
                end
                return diagnose.BlackBoxProcessbar
            end):fetch()
        end
        return download(user, 'blackbox.tar', 'blackbox data', defs.DOWNLOAD_BLACK_BOX, collect_cb, progress_cb)
    end):catch(function(err)
        return reply_bad_request('ActionNotSupported', err)
    end):fetch()
end

function M.DownloadBlackBoxOfSdi(user)
    if C.Diagnose[0].PCIeInterfaceSwitch:fetch() == 0 then
        return reply_bad_request('FeatureDisabledAndNotSupportOperation')
    end
    if C.Payload[0].PowerGd:fetch() == 0 then -- 下电状态,不允许下载
        OperateLog(user, 'Downloading sdi blackbox not allowed in power-off state')
        return reply_bad_request('ActionFailedByPowerOff')
    end
    return C.MctpDevOperate[0]:next(function(obj)
        if obj == nil then
            return reply_bad_request('FeatureDisabledAndNotSupportOperation')
        end
        logging:debug("DownloadBlackBoxOfSdi: Got an object[%s]", dflib.object_name(obj))
        local collect_cb = function()
            local ok, ret = utils.call_method_async(user, obj, 'GetBlackBox')
            if not ok then
                return ret
            end
            return reply_ok()
        end
        local progress_cb = function(file_path)
            return C.MctpDevOperate[0]:next(function(mctpoperate)
                if mctpoperate == nil then
                    return reply_bad_request('FeatureDisabledAndNotSupportOperation')
                end
                logging:debug("DownloadBlackBoxOfSdi.progress_cb: Got an object[%s]", dflib.object_name(mctpoperate))
                if mctpoperate.BlackBoxProcessbar:fetch() == 100 then
                    if not dal_set_file_owner('/tmp/sdi_blackbox.tar', cfg.REDFISH_USER_UID, cfg.APPS_USER_GID) then
                        error('chown sdi_blackbox failed')
                    end
                    if not dal_set_file_mode('/tmp/sdi_blackbox.tar', cfg.FILE_MODE_600) then
                        error('chmod sdi_blackbox failed')
                    end
                    tools.movefile('/tmp/sdi_blackbox.tar', file_path)
                elseif mctpoperate.BlackBoxProcessbar:fetch() > 100 then
                    return 0 - mctpoperate.BlackBoxProcessbar:fetch() -- 大于100的进度,是失败了
                end
                return mctpoperate.BlackBoxProcessbar
            end):fetch()
        end
        return download(user, 'sdi_blackbox.tar', 'sdi_blackbox data', defs.DOWNLOAD_SDI_BLACK_BOX, collect_cb, progress_cb)
    end):catch(function(err)
        return reply_bad_request('ActionNotSupported', err)
    end):fetch()
end

function M.DownloadSerialPort(user)
    if C.Diagnose[0].SolDataState:fetch() == 0 then
        return reply_bad_request('FeatureDisabledAndNotSupportOperation',
            'The serial port data recording function is disabled')
    end
    return C.Diagnose[0]:next(function(obj)
        local collect_cb = function()
            local ok, ret = utils.call_method_async(user, obj, 'GetSystemCom')
            if not ok then
                return ret
            end
            return reply_ok()
        end
        local progress_cb = function(file_path)
            return C.Diagnose[0]:next(function(diagnose)
                if diagnose.SystemComProcessbar:fetch() == 100 then
                    if not dal_set_file_owner('/tmp/systemcom.tar', cfg.REDFISH_USER_UID, cfg.APPS_USER_GID) then
                        error('chown systemcom failed')
                    end
                    if not dal_set_file_mode('/tmp/systemcom.tar', cfg.FILE_MODE_600) then
                        error('chmod systemcom failed')
                    end
                    tools.movefile('/tmp/systemcom.tar', file_path)
                end
                return diagnose.SystemComProcessbar
            end):fetch()
        end
        return download(user, 'systemcom.tar', 'serial port data', defs.DOWNLOAD_SYSTEM_COM, collect_cb,
            progress_cb)
    end):catch(function(err)
        return reply_bad_request('ActionNotSupported', err)
    end):fetch()
end

function M.DownloadNPULog(user, data)
    local npu_id = data.NpuId
    local down_name = {
        "npu1_log.tar.gz", "npu2_log.tar.gz", "npu3_log.tar.gz", "npu4_log.tar.gz",
        "npu5_log.tar.gz", "npu6_log.tar.gz", "npu7_log.tar.gz", "npu8_log.tar.gz"
    }
    local ok, handle = c.get_object_handle_nth("NPU", npu_id - 1)
    if ok ~= 0 then
        return reply_bad_request('ActionNotSupported', 'NPU log collection is not supported')
    end

    return C.NPU[npu_id - 1]:next(function(obj)
        local collect_cb = function()
            local ok, ret = utils.call_method_async(user, obj, 'CollectNpuLog')
            if not ok then
                return ret
            end
            return reply_ok()
        end
        local progress_cb = function(file_path)
            return C.NPU[0]:next(function(npu)
                if npu.NpuLogProcessBar:fetch() == 100 then
                    if not dal_set_file_owner('/tmp/npu_log.tar.gz', cfg.REDFISH_USER_UID, cfg.APPS_USER_GID) then
                        error('chown npu_log failed')
                    end
                    if not dal_set_file_mode('/tmp/npu_log.tar.gz', cfg.FILE_MODE_600) then
                        error('chmod npu_log failed')
                    end
                    tools.movefile('/tmp/npu_log.tar.gz', file_path)
                end
                return npu.NpuLogProcessBar
            end):fetch()
        end
        return download(user, down_name[npu_id], 'NPU log', defs.DOWNLOAD_NPU_LOG, collect_cb,
            progress_cb)
    end):catch(function(err)
        return reply_bad_request('ActionNotSupported', err)
    end):fetch()
end

function M.DownloadVideo(user, data)
    local path = VIDEO_PATH_AND_NAME_MAP[data.VideoType]['VideoPath']
    local name = VIDEO_PATH_AND_NAME_MAP[data.VideoType]['VideoName']
    local category = VIDEO_PATH_AND_NAME_MAP[data.VideoType]['Category']
    if not file_exists(path) then
        return reply_bad_request('FileNotExist', 'The video does not exist.')
    end
    local collect_cb = function(filePath)
        if not proxy_copy_file(user, path, filePath, cfg.REDFISH_USER_UID, cfg.APPS_USER_GID, 0) then
            error('copy video failed')
        end
        return reply_ok()
    end

    local progress_cb = function(filePath)
        if file_exists(filePath) then
            return 100
        end
        return 0
    end
    return download(user, name, category, defs.DOWNLOAD_VIDEO, collect_cb, progress_cb)
end

local function is_language_valid(language)
    local language_set = utils.split(utils.GetLanguageSet(true), ',')
    for _, v in ipairs(language_set) do
        if v == language then
            return true
        end
    end
    return false
end

function M.ExportVideoStartupFile(user, hostname, data)
    local playType = data.PlayType
    local videoType = data.VideoType
    local hostName = data.HostName
    local serverName, _ = hostname:match("^(.*):([0-9]+)$")
    if not serverName then
        serverName = hostname
    end
    local language = data.Language
    if not is_language_valid(language) then
        OperateLog(user, 'Export video jnlp file failed')
        return reply_bad_request('InvalidFiled', 'Ensure the language is valid')
    end
    local mold, compress, filePath = GetFilePath(videoType)
    -- Check whether the file exists
    if filePath and not file_exists(filePath) then
        OperateLog(user, 'Export video jnlp file failed')
        return reply_bad_request('FileNotExist', 'The recording file does not exist')
    end
    -- Obtain the serial number
    local sn = C.BMC[0].DeviceSerialNumber:fetch_or()
    if not sn then
        OperateLog(user, 'Export video jnlp file failed')
        return reply_internal_server_error()
    end
    -- Obtain the port number
    local port = C.Video[0].Port:fetch_or()
    if not port then
        OperateLog(user, 'Export video jnlp file failed')
        return reply_internal_server_error()
    end
    local result = reply_ok()
    local key, salt
    -- Obtain Key and Salt
    C.Video[0]:next(function(obj)
        result:join(safe_call(function()
            local input_list = {gstring(user.UserName), gstring(user.ClientIp)}
            local ok, ret = call_method(user, obj, 'SetVideoName', nil, input_list)
            if not ok then
                return ret
            end
        end))
        result:join(safe_call(function()
            local ok, ret = call_method(user, obj, 'GetVideoKey')
            if not ok then
                return ret
            end
            key = ret[1]
            salt = ret[2]
        end))
    end):fetch()
    if not result:isOk() then
        OperateLog(user, 'Export video jnlp file failed')
        return result
    end

    local ok, jarName = pcall(function()
        for _, f in ipairs(c.file_list('/opt/pme/web/htdocs/bmc/pages/jar/', true)) do
            if f:match('videoplayer[^/]*$') then
                return f
            end
        end
    end)
    if not ok or not jarName then
        OperateLog(user, 'Export video jnlp file failed')
        return reply_internal_server_error()
    end

    if playType == 'HTML5' then
        OperateLog(user, 'Export video jnlp file to local successfully')
        return reply_ok_encode_json({
            ['bladesize'] = '1',
            ['ip'] = serverName,
            ['local'] = language,
            ['port'] = port,
            ['press'] = compress,
            ['salt'] = salt,
            ['title'] = format('Video Player   IP : %s   SN : %s', serverName, sn),
            ['videoType'] = mold,
            ['videoKey'] = key
        })
    elseif playType == 'Java' then
        local buf = format(VIDEO_JNLP_FORMAT, hostName, serverName, sn, jarName, key, serverName, sn, language, mold,
            compress, salt, key, port, serverName)
        OperateLog(user, 'Export video jnlp file to local successfully')
        return reply_ok(buf, {['content-type'] = 'application/octet-stream'})
    end
    OperateLog(user, 'Export video jnlp file failed')
    return reply_internal_server_error()
end

function M.StopVideo(user)
    return C.Video[0]:next(function(obj)
        local ok, ret = call_method(user, obj, 'DelectLink', STOP_VIDEO_ERR_MAP)
        if not ok then
            return ret
        end
        return reply_ok()
    end):fetch()
end

function M.CaptureScreenshot(user)
    return C.Kvm[0]:next(function(obj)
        local saveScreen = function()
            local ok, ret = call_method_async(user, obj, 'SaveScreen', CAPTURE_SCREEN_ERR_MAP, gbyte(0))
            if not ok then
                return ret
            end
            return reply_ok()
        end
        return CaptureScreen(user, saveScreen)
    end):catch(function(err)
        return reply_bad_request('ActionNotSupported', err)
    end):fetch()
end

function M.DeleteScreenshot(user)
    return C.Kvm[0]:next(function(obj)
        local ok, ret = call_method(user, obj, 'RmScreen', nil, gbyte(1))
        if not ok then
            return ret
        end
        return reply_ok()
    end):fetch()
end

return M
