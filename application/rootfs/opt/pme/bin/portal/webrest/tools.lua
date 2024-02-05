local dflib = require 'dflib'
local utils = require 'utils'
local core = require 'dflib.core'
local O = dflib.object
local gstring = core.GVariant.new_string

local tools = {}

function tools.get_token_by_session(session)
    local user_info = {callerInfo = {gstring('Redfish-WebUI'), gstring(''), gstring('')}}
    local ok, resp = utils.call_method(user_info, O.Session, 'GetToken', nil, gstring(session))
    if ok then
        return resp[1]
    else
        return nil
    end
end

function tools.movefile(src_file, dst_file)
    local src_fd = io.open(src_file, 'r')
    local dst_fd = io.open(dst_file, 'w')
    dst_fd:write(src_fd:read('*all'))
    src_fd:close()
    dst_fd:close()
    core.rm_filepath(src_file)
end

return tools
