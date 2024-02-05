local function decode_nginx_file(fields, types)
    for key, val in pairs(fields) do
        if types[key] == 'FILE' then
            local file_name_key = key .. '.name'
            local real_file_name = fields[file_name_key]

            fields[file_name_key] = nil
            types[key] = nil
            types[key] = {type = "FILE", filename = real_file_name}
        end
    end
end

local M = {}

function M.decode(fd)
    decode_nginx_file(fd.fields, fd.types)
    return {user = fd.user, fields = fd.fields, headers = fd.types}
end

return M
