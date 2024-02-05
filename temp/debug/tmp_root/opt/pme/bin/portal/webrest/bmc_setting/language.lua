local utils = require "utils"
local dflib = require "dflib"
local c = require "dflib.core"
local http = require "http"
local logging = require "logging"
local C = dflib.class
local GVariant = c.GVariant
local gstring = GVariant.new_string
local call_method = utils.call_method
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_bad_request = http.reply_bad_request
local reply_internal_server_error = http.reply_internal_server_error
local OperateLog = utils.OperateLog
local split = utils.split

local RFERR_INSUFFICIENT_PRIVILEGE = 403
local RFERR_NO_RESOURCE = 404

local LANGUAGE_NULL_OR_EMPTY = -2001
local LANGUAGE_ONLY_ONE_LANGUAGE = -2002
local LANGUAGE_NO_UNINSTALLED_ZH_OR_EN = -2003
local LANGUAGE_NOT_INSTALLED = -2004

local LANGUAGE_NOT_SUPPORT = -2005
local LANGUAGE_ONLY_ONE_ENABLED = -2006
local LANGUAGE_NO_DISABLED_ZH_OR_EN = -2007
local LANGUAGE_NOT_ENABLED = -2008
local LANGUAGE_NOT_DISABLED = -2009

local errMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request("InsufficientPrivilege",
        "There are insufficient privileges for the account or credentials associated with the current session to perform the requested operation."),
    [RFERR_NO_RESOURCE] = reply_bad_request("ResourceMissingAtURI",
        "The resource at the URI /UI/Rest/UpdateService/SwitchImage was not found."),
    [LANGUAGE_NULL_OR_EMPTY] = reply_bad_request("ActionParameterValueFormatError",
        "The value for the parameter in the action is of a different format than the parameter can accept."),
    [LANGUAGE_ONLY_ONE_LANGUAGE] = reply_bad_request("OnlyOneLanguageInstalled",
        "Uninstall the language failed because there is only one language installed."),
    [LANGUAGE_NO_UNINSTALLED_ZH_OR_EN] = reply_bad_request("NotSupportZhAndEnUninstalled",
        "Uninstall the language failed because Chinese and English cannot be uninstalled."),
    [LANGUAGE_NOT_INSTALLED] = reply_bad_request("LanguageNotInstalled",
        "Uninstall the language failed because this language has not been installed."),
    [LANGUAGE_NOT_SUPPORT] = reply_bad_request("LanguageNotSupport",
        "This language does not support this operation."),
    [LANGUAGE_ONLY_ONE_ENABLED] = reply_bad_request("OnlyOneLanguageEnabled",
        "This language cannot be disabled, because it is the only enabled language."),
    [LANGUAGE_NO_DISABLED_ZH_OR_EN] = reply_bad_request("NotSupportZhAndEnDisabled",
        "The Chinese and English languages cannot be disabled."),
    [LANGUAGE_NOT_ENABLED] = reply_bad_request("LanguageNotEnabled",
        "The language is already disabled."),
    [LANGUAGE_NOT_DISABLED] = reply_bad_request("LanguageNotDisabled",
        "The language is already enabled.")
}

local M = {}

function M.GetLanguage()
    logging:debug(" GetLanguage...")
    return {LanguageSet = utils.GetLanguageSet(true)}
end

-- 要求language_req必须都要在language_system中
local function _FindLanguage(language_system, language_req)
    local language_req_table = split(language_req, ',')
    local language_system_table = split(language_system, ',')
    local is_valid_item

    for _, req in ipairs(language_req_table) do
        is_valid_item = false
        for _, sys in ipairs(language_system_table) do
            if req == sys then
                is_valid_item = true
                break
            end
        end
        if is_valid_item == false then
            return false
        end
    end
    return true
end

-- 存在language_str中一种语言在language_set中，就返回true
local function _FindOneLanguage(language_str, language_set)
    local language_set_arr = split(language_set, ',')

    for _, language in ipairs(language_set_arr) do
        if string.find(language_str, language) then
            return true
        end
    end
    return false
end

function M.OpenLanguage(data, user)
    logging:debug(" OpenLanguage...")
    -- 入参合法性校验，返回不支持的语言
    if not _FindLanguage(C.BMC[0].LanguageSetSupport:fetch_or(), data.LanguageSetStr) then
        return errMap[LANGUAGE_NOT_SUPPORT]
    end

    -- 入参合法性校验，返回当前语言已开启
    if _FindLanguage(utils.GetLanguageSet(true), data.LanguageSetStr) then
        return errMap[LANGUAGE_NOT_DISABLED]
    end

    local all_language_set = utils.GetLanguageSet(true)
    local language_set_arr = split(data.LanguageSetStr, ',')

    for _, language in ipairs(language_set_arr) do
        if not string.find(all_language_set, language, 1, true) then
            all_language_set = (#all_language_set == 0) and language or (all_language_set .. ',' .. language)
        end
    end

    local ok, ret = pcall(function()
        local ok, err = call_method(nil, C.BMC[0]:next():fetch(), "SetLanguageSet", nil, gstring(all_language_set))
        if not ok then
            return err
        end
    end)

    if not ok or ret then
        OperateLog(user, "Failed to enable the language.")
        return reply_internal_server_error('InternalError')
    end
    if #language_set_arr > 1 then
        OperateLog(user, "The languages (%s) are enabled", data.LanguageSetStr)
    else
        OperateLog(user, "The language (%s) is enabled", data.LanguageSetStr)
    end
    return reply_ok_encode_json(M.GetLanguage(user))
end

function M.CloseLanguage(data, user)
    logging:debug(" CloseLanguage...")
    -- 入参合法性校验，返回不支持的语言
    if not _FindLanguage(C.BMC[0].LanguageSetSupport:fetch_or(), data.LanguageSetStr) then
        return errMap[LANGUAGE_NOT_SUPPORT]
    end

    -- 入参合法性校验，当前只有一个语言打开
    local language_set_arr = split(utils.GetLanguageSet(true), ',')
    if #language_set_arr == 1 then
        return errMap[LANGUAGE_ONLY_ONE_ENABLED]
    end

    -- 入参合法性校验，返回不支持中英文语言的关闭
    if string.find(data.LanguageSetStr, "en", 1, true) or string.find(data.LanguageSetStr, "zh", 1, true) then
        return errMap[LANGUAGE_NO_DISABLED_ZH_OR_EN]
    end

    -- 入参合法性校验，返回当前语言未打开
    if not _FindOneLanguage(utils.GetLanguageSet(true), data.LanguageSetStr) then
        return errMap[LANGUAGE_NOT_ENABLED]
    end

    language_set_arr = split(utils.GetLanguageSet(true), ',')
    local language_set = ""

    for _, language in ipairs(language_set_arr) do
        if not string.find(data.LanguageSetStr, language, 1, true) then
            language_set = (#language_set == 0) and language or (language_set .. ',' .. language)
        end
    end

    local ok, ret = pcall(function()
        local ok, err = call_method(nil, C.BMC[0]:next():fetch(), "SetLanguageSet", nil, gstring(language_set))
        if not ok then
            return err
        end
    end)

    if not ok or ret then
        OperateLog(user, "Failed to disable the language.", data.LanguageSetStr)
        return reply_internal_server_error('InternalError')
    end

    language_set_arr = split(data.LanguageSetStr, ',')
    if #language_set_arr > 1 then
        OperateLog(user, "The languages (%s) are disabled", data.LanguageSetStr)
    else
        OperateLog(user, "The language (%s) is disabled", data.LanguageSetStr)
    end
    return reply_ok_encode_json(M.GetLanguage(user))
end
return M
