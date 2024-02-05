local tasks = require "tasks"
local webtask = require "webrest.task"
local http = require "http"
local c = require "dflib.core"
local logging = require "logging"
local cjson = require "cjson"
local utils = require "utils"
local reply_bad_request = http.reply_bad_request
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_not_found = http.reply_not_found
local get_datetime = c.get_datetime
local OperateLog = utils.OperateLog
local OperateLogService = utils.OperateLog

local STATE_PREPARE = "PREPARE"
local STATE_WAIT_TRANSFER = "WAIT_TRANSFER"
local STATE_START_TRANSFER = "START_TRANSFER"
local STATE_END_TRANSFER = "END_TRANSFER"

local path = "/tmp/"
local TIMEOUT_SEC = 100

-- operate log stake, do nothing
local function OperateLogStake()
end

return function(user, fileName, file_category, download_type, collect_cb, progress_cb, filePath)
    local info = {state = STATE_PREPARE, prepare_progress = 0, start_time = get_datetime(), timeout = nil}
    local error = nil
    local fileSize = 0
    local rangeStart = 0
    local rangeEnd = 0
    local deletable = not filePath

    if file_category == 'config file' or file_category == 'license' or file_category == 'dump file' then
        OperateLogService = OperateLogStake
    else
        OperateLogService = utils.OperateLog
    end

    logging:info("start prepare download file %s", fileName)

    -- 注册进度查询接口 URL
    local url = webtask.addTaskInfo(fileName, download_type, function(param)
        -- 检查错误信息
        if error then
            if http.is_reply(error) then
                return error
            end
            return reply_bad_request(nil, error)
        end

        if param.range and #param.range == 2 then
            if param.range[2] == 0 then
                return reply_ok_encode_json(info)
            else
                rangeStart = math.min(param.range[1], fileSize)
                rangeEnd = math.min(param.range[2], fileSize)

                if rangeEnd >= fileSize then
                    info.state = STATE_END_TRANSFER
                else
                    -- 这里应该统计每次的 rangeStart 和 rangeEnd 来判断是否下载完成
                    -- 暂时不统计也没关系,用最后一次传输的时间计算超时来删除下载任务
                    -- 对于超大文件,前端应该使用 http 分段下载协议,保证每个分段不超时
                    info.timeout = TIMEOUT_SEC
                    info.state = STATE_START_TRANSFER
                end
                logging:info("transfer file %s range [%d - %d / %d]", fileName, rangeStart, rangeEnd, fileSize)
                return reply_ok_encode_json(info)
            end
        end
        return reply_not_found()
    end)

    if url == nil then
        OperateLog(user, "Export %s failed", file_category)
        return reply_bad_request("DuplicateExportingErr")
    end

    filePath = filePath or (path .. url)

    -- 监控循环,用来监控下载进度
    local watchLoop = function()
        -- 1、开始收集数据
        local ok, ret = pcall(collect_cb, filePath)
        if not ok or not http.is_reply(ret) then
            local errMsg = type(ret) == "string" and ret or cjson.encode(ret)
            OperateLogService(user, "Export %s failed", file_category)
            logging:error("prepare download file %s failed: %s", fileName, errMsg)
            error = reply_bad_request(nil, ret)
            return
        elseif not ret:isOk() then
            error = ret
            OperateLogService(user, "Export %s failed", file_category)
            logging:error("prepare download file %s failed: %s", fileName, cjson.encode(ret:body()))
            return
        end

        -- 2、循环检查文件生成进度
        while true do
            local ok, ret = pcall(progress_cb, filePath)
            if not ok or ret < 0 then
                if ret <= -0x80 then
                    OperateLog(user, "Download %s failed", file_category)
                    local msg = string.format("Download file %s failed: %s.", fileName, ret)
                    logging:error(msg)
                    -- TransferError=0xFF Duplication=0xFE  PackFailure=0xFD  NoResponse=0xFC
                    local array =
                        {'FileTransferErrorDesc', 'FileDuplicationDownLoad', 'DataFilePackError', 'DeviceNoResponse'}
                    error = reply_bad_request(array[0xFF + ret + 1], msg)
                    return
                end
                error = ret
                OperateLogService(user, "Export %s failed", file_category)
                logging:error("prepare download file %s failed: %s", fileName, ret)
                return
            end

            info.prepare_progress = ret
            logging:info("prepare download file %s progress %d%%", fileName, ret)

            if ret >= 100 then
                -- 生成文件成功,延时 0.5 后秒检查文件是否存在以及获取文件大小
                tasks.sleep(500)
                local file = io.open(filePath)
                if not file then
                    local msg = string.format("prepare download file %s failed: file not exist", fileName)
                    error = reply_bad_request(nil, msg)
                    OperateLog(user, "Export %s failed", file_category)
                    logging:error(msg)
                    return
                end

                local size, err = file:seek("end")
                if not size then
                    local msg = string.format("prepare download file %s failed: get file size error %s", fileName,
                        err)
                    error = reply_bad_request(nil, msg)
                    OperateLog(user, "Export %s failed", file_category)
                    logging:error(msg)
                    file:close()
                    return
                end
                fileSize = size
                file:close()

                logging:info("prepare download file %s [size = %d] success, %s", fileName, fileSize, url)
                break
            end

            tasks.sleep(1000)
        end

        -- 文件生成成功,开始等待传输
        info.state = STATE_WAIT_TRANSFER
        info.downloadurl = string.format("/UI/Rest/download/%s", url)
        webtask.finishTaskInfo(url)

        -- 超过 TIMEOUT_SEC 秒还未开始传输就退出
        info.timeout = TIMEOUT_SEC
        while info.state == STATE_WAIT_TRANSFER do
            tasks.sleep(1000)
            info.timeout = info.timeout - 1
            if info.timeout <= 0 then
                OperateLog(user, "Export %s failed", file_category)
                logging:error("wait transfer file %s timeout (%d s)", fileName, TIMEOUT_SEC)
                error = reply_bad_request(nil, "wait transfer timeout")
                return
            end
        end

        -- 检查最后一次传输超时
        while info.state == STATE_START_TRANSFER do
            tasks.sleep(1000)
            info.timeout = info.timeout - 1
            if info.timeout <= 0 then
                OperateLog(user, "Export %s failed", file_category)
                logging:error("transfer file %s timeout (%d s)", fileName, TIMEOUT_SEC)
                error = reply_bad_request(nil, "transfer timeout")
                return
            end
        end

        OperateLogService(user, "Export %s to local successfully", file_category)
    end

    -- 新建任务来控制文件下载逻辑
    tasks.start(function()
        -- 用 fileName 给任务命名,同一时间同一文件只允许一个下载任务
        tasks.register(fileName)

        -- 进入监控循环
        local ok, err = pcall(watchLoop)
        if not ok then
            logging:error(err)
        end
        webtask.finishTaskInfo(url)

        -- 删除文件
        if deletable then
            c.rm_filepath(filePath)
        end

        -- 延时 1 分钟后销毁 URL
        tasks.start(function()
            tasks.sleep(1000 * 60)
            webtask.delTaskInfo(url)
            logging:info("remove download file url %s/%s", url, fileName)
            tasks.exit()
        end)

        tasks.exit()
    end)

    return reply_ok_encode_json({url = "/UI/Rest/task/" .. url})
end
