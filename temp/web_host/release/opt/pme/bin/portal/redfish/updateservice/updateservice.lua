local http = require "http"
local cjson = require "cjson"
local redfish_utils = require 'redfish_utils'
local reply_accepted = http.reply_accepted
local create_message = redfish_utils.create_message

local M = {}

function M.post_update_firmware_inventory()
    local GeneralError = create_message('GeneralError')
    local SuccessMsg = create_message('Success')
    SuccessMsg.MessageId = "iBMC.0.1.0.Success"
    if SuccessMsg.Resolution == "None" then
        SuccessMsg.Resolution = ""
    end
    if SuccessMsg.Message == "Successfully Completed Request" then
        SuccessMsg.Message = "Successfully Completed Request."
    end
    local text = cjson.encode({
        error = {
            code = GeneralError.MessageId,
            message = GeneralError.Message,
            ['@Message.ExtendedInfo'] = {SuccessMsg}
        }
    })
    return reply_accepted(text)
end

return M
