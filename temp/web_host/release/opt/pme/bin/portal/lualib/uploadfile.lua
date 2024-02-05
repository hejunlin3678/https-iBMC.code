local dflib = require "dflib"
local http = require "http"
local c = require "dflib.core"
local formdata = require "formdata"
local utils = require 'utils'
local logging = require "logging"
local cfg = require 'config'
local portal = require "portal"
local C = dflib.class
local O = dflib.object
local realpath = c.realpath
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local gstring = GVariant.new_string
local gvstring = GVariant.new_vstring
local guint32 = GVariant.new_uint32
local new_byte_string = GVariant.new_byte_string
local reply_ok = http.reply_ok
local reply_internal_server_error = http.reply_internal_server_error
local reply_bad_request = http.reply_bad_request
local reply_forbidden = http.reply_forbidden
local RedfishOperateLog = utils.RedfishOperateLog
local redfish_call_method = utils.redfish_call_method
local call_method = utils.call_method
local dal_set_file_owner = utils.dal_set_file_owner
local dal_set_file_mode = utils.dal_set_file_mode
local OperateLog = utils.OperateLog
local get_file_length = utils.file_length
local file_exists = utils.file_exists
local modify_file_mode = utils.modify_file_mode
local check_file_exists = utils.check_file_exists
local real_file_path = utils.real_file_path
local is_tmp_web_path = utils.is_tmp_web_path
local lower = string.lower
local format = string.format
local DFL_OK = dflib.DFL_OK

local MSG_INVALID_FILE = "An error occurred during the firmware upload process. Details: invalid upload file."
local MSG_FILE_EXCEED = "An error occurred during the firmware upload process. Details: File size is out of range."
local MSG_CAPACITY_EXCEED =
    "An error occurred during the firmware upload process. Details: insufficient memory capacity for the upload."

local encryptFileDataErrMap = {
    [-1] = reply_bad_request('FirmwareUploadError', MSG_INVALID_FILE)
}

local function is_web_uploadfile(uri)
    return uri:lower():match("^/ui/rest") ~= nil
end

local function is_web_upload_spfw_file(uri)
    return uri:lower():match("^/ui/rest/bmcsettings/updateservice/spfwupdate") ~= nil
end

local MAX_FILEPATH_LENGTH = 256
local MAX_FILE_SIZE_1M = (1024 * 1024)
local MAX_FILE_SIZE_2M = (2 * 1024 * 1024)
local MAX_FILE_SIZE_90M = (90 * 1024 * 1024)
local MAX_FILE_SIZE_2K = (2 * 1024)
local MAX_FILE_SIZE_500M = (500 * 1024 * 1024)

local file_type_len_array = {
    cer = MAX_FILE_SIZE_1M,
    pem = MAX_FILE_SIZE_1M,
    cert = MAX_FILE_SIZE_1M,
    crt = MAX_FILE_SIZE_1M,
    xml = MAX_FILE_SIZE_1M,
    p12 = MAX_FILE_SIZE_1M,
    keytab = MAX_FILE_SIZE_1M,
    crl = MAX_FILE_SIZE_1M,
    pfx = MAX_FILE_SIZE_2M,
    keys = MAX_FILE_SIZE_2M,
    pub = MAX_FILE_SIZE_2K,
    hpm = MAX_FILE_SIZE_90M,
    zip = MAX_FILE_SIZE_90M,
    asc = MAX_FILE_SIZE_90M
}

local spfw_file_type_len_array = {
    zip = MAX_FILE_SIZE_500M,
    asc = MAX_FILE_SIZE_90M,
    p7s = MAX_FILE_SIZE_90M
}

local function check_allow_file_types(allowFileTypes, name_suffix)
    if not allowFileTypes then
        return true
    end

    for _, allowType in ipairs(allowFileTypes) do
        if lower(allowType) == name_suffix then
            return true
        end
    end

    return false
end

local function check_file_info(allowFileTypes, filename, file_length, rest_type)
    local name_suffix = string.match(filename, "%.([^.]+)$")
    if not name_suffix then
        logging:error("upload invalid file name %s", filename)
        return false, reply_bad_request("FirmwareUploadError", MSG_INVALID_FILE)
    end

    name_suffix = lower(name_suffix)
    local file_type_len
    if rest_type == "spfw" then
        file_type_len = spfw_file_type_len_array[name_suffix]
    else
        file_type_len = file_type_len_array[name_suffix]
    end

    if not file_type_len or not check_allow_file_types(allowFileTypes, name_suffix) then
        logging:error("upload invalid file type %s", filename)
        return false, reply_bad_request("FirmwareUploadError", MSG_INVALID_FILE)
    end

    if file_length > file_type_len then
        logging:error("upload file too lagger: name = %s, len = %d, max_len = %d", filename, file_length,
            file_length)
        return false, reply_bad_request("FirmwareUploadError", MSG_FILE_EXCEED)
    end

    return true
end

local function get_available_space(dir)
    local ok, info = pcall(c.statvfs, dir)
    if not ok then
        return 200 * 1024 * 1024
    end
    return info.avail_size
end

local function process_file(data, filename, tmp_file_path, operate_log_func, rest_type)
    if not filename then
        logging:error("get upload file name failed: %s", filename)
        return false, reply_internal_server_error("InternalError")
    end

    local user = data.user
    local len
    if rest_type == "spfw" then
        local ret = C.USBMassStorage[0]:next(function(obj)
            local ok, outputlist = call_method(user, obj, "GetFileLength", nil, gstring(tmp_file_path))
            if not ok then    
                logging:error("get sp firmware length failed")
                return reply_internal_server_error("InternalError")
            end

            len = outputlist[1]
        end):fetch()

        if ret then
            return ret
        end
    else
        len = get_file_length(tmp_file_path)
    end

    if not len then
        operate_log_func(user, 'Upload file(%s) failed', filename)
        logging:error("get upload file length failed: [%s]", filename)
        return false, reply_internal_server_error("InternalError")
    end

    local ok_file_info, res = check_file_info(data.allowFileTypes, filename, len, rest_type)
    if not ok_file_info then
        operate_log_func(user, 'Upload file(%s) failed', filename)
        return false, res
    end

    local target_file_path
    if rest_type == "spfw" then
        target_file_path = format("/data/sp/spforbmc/operate/spfwupdate/%s", filename)
    else
        target_file_path = format("/tmp/web/%s", filename)
    end

    if c.is_symbol_link(tmp_file_path) then
        operate_log_func(user, 'Upload file(%s) failed', filename)
        logging:error("check upload file symbol link failed: [%s]", filename)
        return false, reply_internal_server_error("InternalError")
    end

    if check_file_exists(user, target_file_path) then
        operate_log_func(user, 'Upload file(%s) failed', filename)
        return false, reply_bad_request("SpecifiedFileExist")
    end

    if #target_file_path > MAX_FILEPATH_LENGTH then
        operate_log_func(user, 'Upload file(%s) failed', filename)
        return false, reply_bad_request('FirmwareUploadError', MSG_INVALID_FILE)
    end

    O.PrivilegeAgent:call('DeleteFile', nil, {gstring(target_file_path)})
    if rest_type == "spfw" then
        local ret = C.USBMassStorage[0]:next(function(obj)
            local ok, err = call_method(user, obj, "RenameLocalFwSP", nil, gstring(tmp_file_path), gstring(target_file_path))
            if not ok then
                    return err
            end
        end):fetch()
    else
        local result = c.system_s("/bin/mv", tmp_file_path, target_file_path)
        if result ~= DFL_OK then
            logging:error("move file failed, %s", result)
            operate_log_func(user, 'Upload file failed')
            return false, reply_internal_server_error("InternalError")
        end
    end
    logging:info("upload move file success")
    return true, target_file_path
end

local function safe_process_file(data, filename, tmp_file_path, operate_log_func, rest_type)
    dal_set_file_owner(tmp_file_path, cfg.REDFISH_USER_UID, cfg.APPS_USER_GID)
    dal_set_file_mode(tmp_file_path, cfg.FILE_MODE_600)
    local ok, ret, target_file_path = pcall(process_file, data, filename, tmp_file_path, operate_log_func, rest_type)
    if not ok then
        logging:error("process upload file failed, %s", ret)
        operate_log_func(data.user, 'Upload file failed')
        return false, reply_internal_server_error("InternalError")
    end

    return ret, target_file_path
end

local M = {}

function M.UploadFile(data)
    local operate_log_func = is_web_uploadfile(data.uri) and OperateLog or RedfishOperateLog
    if data.error then
        operate_log_func(data.user, 'Upload file failed')
        return reply_bad_request('FirmwareUploadError', MSG_INVALID_FILE)
    end

    local rest_type
    if is_web_upload_spfw_file(data.uri) then
        rest_type = "spfw"
    else
        rest_type = "file"
    end

    local fd = formdata.decode(data)
    local upload_file_list = {}
    for name, header in pairs(fd.headers) do
        if header.type == "FILE" then
            local tmp_file_path = fd.fields[name]
            local filename = string.match(header.filename, "([^\\/]+)$")
            local ok, target_file_path = safe_process_file(data, filename, tmp_file_path, operate_log_func, rest_type)
            if not ok then
                if rest_type == "spfw" and #upload_file_list ~= 0 then
                    C.USBMassStorage[0]:next(function(obj)
                        ok, _ = call_method(fd.user, obj, "ClearFirmware", nil, gvstring(upload_file_list))
                        if not ok then
                            logging:error("clean firmware failed")
                        end
                    end):fetch()
                else
                    for _, value in ipairs(upload_file_list) do
                        local real_path = realpath(value)
                        if real_path and file_exists(real_path) then
                            c.rm_filepath(real_path)
                        end
                    end
                end
                return target_file_path
            end
            upload_file_list[#upload_file_list + 1] = target_file_path
            fd.fields[name] = target_file_path
            operate_log_func(fd.user, 'Upload file(%s) successfully', filename)
        end

        if header == "FILE_CONTENT" then
            fd.fields[name].data = portal.base64_encode(fd.fields[name].data)
            return reply_ok(fd)
        end
    end
    return reply_ok(fd)
end

function M.UploadHpmFile(data)
    local operate_log_func = is_web_uploadfile(data.uri) and OperateLog or RedfishOperateLog
    if data.error then
        operate_log_func(data.user, 'Upload file failed')
        return reply_bad_request('FirmwareUploadError', MSG_INVALID_FILE)
    end

    local upload_file_list = {}
    local fd = formdata.decode(data)
    for name, header in pairs(fd.headers) do
        if header.type == "FILE" then
            local tmp_file_path = fd.fields[name]
            local filename = string.match(header.filename, "([^\\/]+)$")
            local filename_prefix = string.match(filename, "(.*).[hH][pP][mM]$")
            -- 升级文件只允许包含数字、字母、小括号、点、下划线、空格和横杠。正则表达式检查是否包含除a-zA-Z0-9._()-之外的字符。
            if (not filename_prefix or string.match(filename_prefix, [=[[^a-zA-Z0-9._()-[%s]]]=]) ~= nil) then
                logging:error("File name is invalid.")
                for _, value in ipairs(upload_file_list) do
                    local real_path = realpath(value)
                    if real_path and file_exists(real_path) then
                        c.rm_filepath(real_path)
                    end
                end
                return reply_bad_request("FileNameError")
            end
            local ok, target_file_path = safe_process_file(data, filename, tmp_file_path, operate_log_func)
            if not ok then
                for _, value in ipairs(upload_file_list) do
                    local real_path = realpath(value)
                    if real_path and file_exists(real_path) then
                        c.rm_filepath(real_path)
                    end
                end
                return target_file_path
            end
            upload_file_list[#upload_file_list + 1] = target_file_path
            fd.fields[name] = target_file_path
            operate_log_func(fd.user, 'Upload file(%s) successfully', filename)
        end
    end
    return reply_ok(fd)
end

function M.UploadFirmwareInventory(data)
    local operate_log_func = is_web_uploadfile(data.uri) and OperateLog or RedfishOperateLog
    if data.error then
        operate_log_func(data.user, 'Upload file failed')
        return reply_bad_request('FirmwareUploadError', MSG_INVALID_FILE)
    end

    local file_flag = false
    local fd = formdata.decode(data)
    for name, header in pairs(fd.headers) do
        if header.type == "FILE" then
            local tmp_file_path = fd.fields[name]
            local filename = string.match(header.filename, "([^\\/]+)$")
            local ok, target_file_path = safe_process_file(data, filename, tmp_file_path, operate_log_func)
            if not ok then
                return target_file_path
            end
            fd.fields[name] = target_file_path
            modify_file_mode(fd.user, target_file_path)
            operate_log_func(fd.user, 'Upload file(%s) successfully', filename)
            file_flag = true
        end
    end

    if not file_flag then
        logging:error("upload no file")
        return reply_bad_request("FirmwareUploadError", MSG_INVALID_FILE)
    end

    return reply_ok()
end

-- data:user
--      url
--      types:imgfile-FILE_CONTENT
--      fields:imgfile:file_name-文件名称
--                     data-文件内容
function M.UploadFirmwareInventoryMemory(data)
    local operate_log_func = is_web_uploadfile(data.uri) and OperateLog or RedfishOperateLog
    if data.error then
        operate_log_func(data.user, 'Upload file failed')
        return reply_bad_request("FirmwareUploadError", MSG_INVALID_FILE)
    end

    local content = data.fields.imgfile.data
    local filename = data.fields.imgfile.file_name
    local filePath = "/tmp/web/" .. filename
    local realPath = real_file_path(filePath)
    if not realPath or not is_tmp_web_path(realPath) then
        operate_log_func(data.user, 'Upload file(%s) failed', filename)
        return reply_bad_request("FirmwareUploadError", MSG_INVALID_FILE)
    end

    if check_file_exists(data.user, filePath) then
        operate_log_func(data.user, 'Upload file(%s) failed', filename)
        return reply_bad_request("SpecifiedFileExist")
    end
    O.PrivilegeAgent:call('DeleteFile', nil, {gstring(filePath)})

    local ok, result = call_method(data.user, O.UploadSensitiveFile, "EncryptFileData", encryptFileDataErrMap, new_byte_string(content), gstring(filePath))
    if not ok then
        operate_log_func(data.user, 'Upload file(%s) failed', filename)
        return result
    end

    modify_file_mode(data.user, filePath)
    operate_log_func(data.user, 'Upload file(%s) successfully', filename)
    return reply_ok()
end

local function get_content_length(data)
    local headers = data.headers
    if not headers then
        return
    end

    local len = headers["content-length"]
    if not len then
        return
    end

    return tonumber(len)
end

function M.UploadFileCheck(data, ctx)
    local operate_log_func = is_web_uploadfile(data.original_uri) and OperateLog or RedfishOperateLog

    local rest_type = "file"
    if is_web_upload_spfw_file(data.original_uri) then
        rest_type = "spfw"
	end

    if rest_type == "file" then
        -- 检查上传目录 /tmp/web 是否存在，没有就创建。
        -- 如果使用的是 nginx 文件上传插件，在 nginx 启动时会自动创建 /tmp/web 目录，到了这里
        -- 目录一定存在，如果运行中删除该目录 nginx 不会路由到这里而是直接返回 503 Service Temporarily Unavaliable。
        if not file_exists("/tmp/web") then
            if c.system_s('/bin/mkdir', '-p', "/tmp/web") ~= DFL_OK or
                c.system_s('/bin/chmod', '1777', "/tmp/web") ~= DFL_OK then
                logging:error("create upload directory failed")
                return reply_internal_server_error("InternalError")
            end
        end
    end

    local content_length = get_content_length(data)
    if not content_length or content_length == 0 then
        logging:error("get upload content-length failed")
        return reply_bad_request("UnrecognizedRequestBody")
    end

    -- 硬盘可用字节数
    if rest_type == "spfw" then
        local ret = C.USBMassStorage[0]:next(function(obj)
            local ok, err = call_method(ctx, obj, "CheckFirmwarePath", nil, guint32(content_length))
            if not ok then
                logging:error("check firmware path failed")
                return err
            end
        end):fetch()

        if ret then
            return ret
        end
    else
        local available_space = get_available_space("/tmp/web")
        if not available_space then
            logging:error("get upload dir available space failed")
            return reply_internal_server_error("InternalError")
        end

        -- 用 content-length 当做文件长度不精确，实际文件会比 content-length 小，这对判断
        -- 硬盘空间是否足够影响不大。
        if content_length > available_space then
            logging:error(
                "upload file failed: insufficient memory capacity for the upload, available = %d, content_length =  %d",
                available_space, content_length)
            operate_log_func(ctx, 'Upload file failed')
            return reply_forbidden("FirmwareUploadError", MSG_CAPACITY_EXCEED)
        end
    end

    return reply_ok()
end

return M
