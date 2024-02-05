local logging = require 'logging'

local M = {}
local MAX_DEPTH = 5
local KEY_TABLE = {'id', 'user', 'ip', 'mod_time', 'detail_info'}

local VALUE_KEY_TABLE = {id = 1, user = 2, ip = 3, mod_time = 4, detail_info = 5}

local function parse_str2xml(str)
    local tmp_str
    tmp_str = string.gsub(str, '&', '&amp;')
    tmp_str = string.gsub(tmp_str, '<', '&lt;')
    tmp_str = string.gsub(tmp_str, '>', '&gt;')
    return tmp_str
end

local function parse_xml2str(str)
    local tmp_str
    tmp_str = string.gsub(str, '&lt;', '<')
    tmp_str = string.gsub(tmp_str, '&gt;', '>')
    tmp_str = string.gsub(tmp_str, '&amp;', '&')
    return tmp_str
end

local function xml2table_rec(xml, depth)
    local root = {}
    local key_start = 1
    local key_end = 1
    local c, key, empty
    local is_find = false

    while depth < MAX_DEPTH do
        local key_num = 0
        key_start, key_end, c, key, empty = string.find(xml, '<(%/?)(.-)(%/?)>', key_start)
        if not key_start then
            return is_find and root or parse_xml2str(xml)
        end
        is_find = true

        if empty == '/' or c == '/' then
            root[key] = {}
        elseif c == '' then
            -- 获取有多少对key，找到结尾
            local tmp_start = key_start
            local tmp_end
            while tmp_start do
                tmp_start, tmp_end = string.find(xml, '<' .. key .. '>.-</' .. key .. '>', tmp_start)
                key_num = key_num + 1
                if not tmp_end then
                    break
                end
                tmp_start = tmp_end + 1
                key_end = tmp_end
                if VALUE_KEY_TABLE[key] then
                    break
                end
            end
            -- 如果有多对，说明是个数组
            if key_num > 2 then
                root[key] = {}
            end
            logging:debug('key = %s, num = %d', key, key_num)
            local xml_sub = string.sub(xml, key_start, key_end)
            for v in string.gmatch(xml_sub, '%s-%<' .. key .. '%>(.-)%<%/' .. key .. '%>%s-') do
                if key_num > 2 then
                    table.insert(root[key], xml2table_rec(v, depth + 1))
                else
                    root[key] = xml2table_rec(v, depth + 1)
                end
            end
        end

        key_start = key_end + 1
    end
    return root
end

local function get_child_key(val)
    if type(val) ~= 'table' then
        return val
    end

    return next(val)
end

local function table2xml_rec(root, key_name, depth)
    local xml = {}
    if depth > MAX_DEPTH then
        return xml
    end

    local tabs = string.rep('  ', depth)
    local is_record = root.id
    if is_record then
        local new_root = {}
        for k, v in pairs(root) do
            new_root[VALUE_KEY_TABLE[k]] = v
        end
        root = new_root
    end
    for key, val in pairs(root) do
        if key ~= 'recordId' and key ~= 'recordCount' then
            if type(val) ~= 'table' then
                if is_record then
                    key = KEY_TABLE[key]
                end
                table.insert(xml, tabs .. '<' .. key .. '>' .. parse_str2xml(val) .. '</' .. key .. '>')
            else
                if type(key) == 'number' and not is_record then
                    table.insert(xml, tabs .. '<' .. key_name .. '>\n' .. table2xml_rec(val, nil, depth + 1) ..
                        '\n' .. tabs .. '</' .. key_name .. '>')
                else
                    local child_key = get_child_key(val)
                    if child_key == nil then
                        table.insert(xml, tabs .. '<' .. key .. '/>')
                    elseif type(child_key) == 'number' and not is_record then
                        table.insert(xml, table2xml_rec(val, key, depth))
                    else
                        table.insert(xml,
                            tabs .. '<' .. key .. '>\n' .. table2xml_rec(val, key, depth + 1) .. '\n' .. tabs ..
                                '</' .. key .. '>')
                    end
                end
            end
        end
    end

    return table.concat(xml, '\n')
end

function M.xml2table(xml)
    local root = {}
    local _, _, content = string.find(xml, '<work_record>(.-)</work_record>', 1)
    root.work_record = xml2table_rec(content, 1)
    return root
end

function M.table2xml(root, table_name)
    local xml = {}

    table.insert(xml, [[<?xml version="1.0" encoding="UTF-8"?>]])
    table.insert(xml, '<' .. table_name .. '>')
    table.insert(xml, '  <recordId>' .. root.recordId .. '</recordId>')
    table.insert(xml, '  <recordCount>' .. root.recordCount .. '</recordCount>')
    table.insert(xml, table2xml_rec(root, nil, 1))
    table.insert(xml, '</' .. table_name .. '>\n')

    return table.concat(xml, '\n')
end

return M
