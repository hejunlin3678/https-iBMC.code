local http = require 'http'
local bit = require 'bit'
local c = require 'dflib.core'
local reply_ok = http.reply_ok
local reply_forbidden = http.reply_forbidden
local get_file_accessible = c.get_file_accessible
local band = bit.band
local format = string.format

local USERROLE_DIAGNOSEMGNT = 0x02
local USERROLE_KVMMGNT = 0x80

local PIC_CFG_INFO_MAP = {
    ['img1.jpeg'] = USERROLE_DIAGNOSEMGNT,
    ['img2.jpeg'] = USERROLE_DIAGNOSEMGNT,
    ['img3.jpeg'] = USERROLE_DIAGNOSEMGNT,
    ['manualscreen.jpeg'] = USERROLE_DIAGNOSEMGNT,
    ['realtimedisplay.jpeg'] = USERROLE_KVMMGNT,
    ['realtimedisplay.bmp'] = USERROLE_KVMMGNT
}

local M = {}

function M.DownloadStaticFile(ctx)
    local file_name = ctx.uri:match('^/bmc/tmpshare/tmp/web/(.+)$')
    local expected_priv = PIC_CFG_INFO_MAP[file_name]
    if not ctx.Privilege or not expected_priv then
        return reply_forbidden('InsufficientPrivilege')
    end

    if band(ctx.Privilege, expected_priv) == 0 then
        return reply_forbidden('InsufficientPrivilege')
    end

    if not get_file_accessible(format('/tmp/web/%s', file_name)) then
        return reply_forbidden('InsufficientPrivilege')
    end

    return reply_ok()
end

return M
