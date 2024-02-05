local registry = require 'redfish.registry'
local create_message = registry.create_message

local M = {}

function M.create_message(msg_id, msg_args, related_props)
    return create_message({code = msg_id, message = msg_args, props = related_props})
end

function M.replace_manufacture_for_uri(uri, custom_manufacture)
    local result

    if custom_manufacture == "Huawei" then
        return uri
    end

    local pattern1_src = "Oem/Huawei/"
    local pattern1_dst = "Oem/" .. custom_manufacture .. "Err/"
    result = string.gsub(uri, pattern1_src, pattern1_dst)

    local pattern2_src = "Oem/" .. custom_manufacture .. "/"
    local pattern2_dst = "Oem/Huawei/"
    result = string.gsub(result, pattern2_src, pattern2_dst)

    return result
end

function M.replace_manufacture_for_req(request_str, custom_manufacture)
    local result

    if custom_manufacture == "Huawei" then
        return request_str
    end

    local pattern1_src = "\"Oem\"[%s\b\f]*:[%s\b\f]*{[%s\b\f]*\"Huawei\"[%s\b\f]*:"
    local pattern1_dst = "\"Oem\":{\"" .. custom_manufacture .. "Err\":"
    result = string.gsub(request_str, pattern1_src, pattern1_dst)

    local pattern2_src = "'Oem'[%s\b\f]*:[%s\b\f]*{[%s\b\f]*'Huawei'[%s\b\f]*:"
    local pattern2_dst = "'Oem':{'" .. custom_manufacture .. "Err':"
    result = string.gsub(result, pattern2_src, pattern2_dst)

    local pattern3_src = "Oem/Huawei/"
    local pattern3_dst = "Oem/" .. custom_manufacture .. "Err/"
    result = string.gsub(result, pattern3_src, pattern3_dst)

    local pattern4_src = "\"Oem\"[%s\b\f]*:[%s\b\f]*{[%s\b\f]*\"" .. custom_manufacture .. "\"[%s\b\f]*:"
    local pattern4_dst = "\"Oem\":{\"Huawei\":"
    result = string.gsub(result, pattern4_src, pattern4_dst)

    local pattern5_src = "'Oem'[%s\b\f]*:[%s\b\f]*{[%s\b\f]*'" .. custom_manufacture .. "'[%s\b\f]*:"
    local pattern5_dst = "'Oem':{'Huawei':"
    result = string.gsub(result, pattern5_src, pattern5_dst)

    local pattern6_src = "Oem/" .. custom_manufacture .. "Err/"
    local pattern6_dst = "Oem/Huawei/"
    result = string.gsub(result, pattern6_src, pattern6_dst)

    return result
end

function M.replace_manufacture_for_rsp(response_str, custom_manufacture)
    local result

    if custom_manufacture == "Huawei" then
        return response_str
    end

    local pattern1_src = "'Oem'[%s\b\f]*:[%s\b\f]*{[%s\b\f]*'Huawei'[%s\b\f]*:"
    local pattern1_dst = "'Oem':{'" .. custom_manufacture .. "':"
    result = string.gsub(response_str, pattern1_src, pattern1_dst)

    local pattern2_src = "\"Oem\"[%s\b\f]*:[%s\b\f]*{[%s\b\f]*\"Huawei\"[%s\b\f]*:"
    local pattern2_dst = "\"Oem\":{\"" .. custom_manufacture .. "\":"
    result = string.gsub(result, pattern2_src, pattern2_dst)

    local pattern3_src = "Oem/Huawei/"
    local pattern3_dst = "Oem/" .. custom_manufacture .. "/"
    result = string.gsub(result, pattern3_src, pattern3_dst)

    local pattern4_src = "\"Oem\"[%s\b\f]*:[%s\b\f]*{[%s\b\f]*\"" .. custom_manufacture .. "Err\"[%s\b\f]*:"
    local pattern4_dst = "\"Oem\":{\"Huawei\":"
    result = string.gsub(result, pattern4_src, pattern4_dst)

    local pattern5_src = "'Oem'[%s\b\f]*:[%s\b\f]*{[%s\b\f]*'" .. custom_manufacture .. "Err'[%s\b\f]*:"
    local pattern5_dst = "'Oem':{'Huawei':"
    result = string.gsub(result, pattern5_src, pattern5_dst)

    local pattern6_src = "Oem/" .. custom_manufacture .. "Err"
    local pattern6_dst = "Oem/Huawei"
    result = string.gsub(result, pattern6_src, pattern6_dst)

    return result
end

return M
