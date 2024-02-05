local utils = require 'utils'
local dflib = require 'dflib'
local c = require 'dflib.core'
local cjson = require 'cjson'
local http = require 'http'
local bit = require 'bit'
local logging = require 'logging'
local cfg = require 'config'
local C = dflib.class
local O = dflib.object
local null = cjson.null
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local guint16 = GVariant.new_uint16
local guint32 = GVariant.new_uint32
local gstring = GVariant.new_string
local call_method = utils.call_method
local reply_ok = http.reply_ok
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_bad_request = http.reply_bad_request
local get_system_name = utils.get_system_name
local IsMultiplaneSupport = utils.IsMultiplaneSupport
local FN_TYPE_OUTTER_OM = cfg.FN_TYPE_OUTTER_OM
local IsPangeaPacificSeries = utils.IsPangeaPacificSeries

-- 网口类型
local ETHERNET_TYPE_E = {
    INNER_ETHERNET = 1, -- 内部网口
    OUT_ETHERNET = 2, -- 外部网口
    VETH_ETHERNET = 3 -- VETH
}

--  IP模式
local IP_MODE_E = {
    IP_UNSPECIFIC_MODE = 0,
    IP_STATIC_MODE = 1,
    IP_DHCP_MODE = 2,
    IP_LOAD_BY_SMS_MODE = 3,
    IP_OTHER_PROTOCOL_MODE = 4,
    IP_MODE_NUM = 5
}

local BUSY_PORT_AVAILABLEFLAG_E = {
    AVAILABLE_FLAG_DISABLED = 0,
    AVAILABLE_FLAG_ENABLED = 1,
    AVAILABLE_FLAG_VIRTUAL = 3
}

local TYPE_MASK_E = {CHASSIS_TYPE_MASK = 0xf0, BMC_TYPE_MASK = 0x0f}

local CHASSIS_TYPE_E = {
    CHASSIS_TYPE_RACK = 0x00,
    CHASSIS_TYPE_X_SERIAL = 0x10,
    CHASSIS_TYPE_BLADE = 0x20,
    CHASSIS_TYPE_SWITCH = 0x30,
    CHASSIS_TYPE_MM = 0x40,
    CHASSIS_TYPE_CMC = 0x50
}

local BOARD_TYPE_E = {
    BOARD_SWITCH = 1,
    BOARD_BLADE = 2,
    BOARD_OTHER = 3,
    BOARD_MM = 4,
    BOARD_RACK = 5,
    BOARD_HMM = 6,
    BOARD_RM = 7
}

local NET_TYPE_E = {
    NET_TYPE_LOM = 1, -- 板载1网卡，对应eth0，部分网卡支持NCSI
    NET_TYPE_DEDICATED = 2, -- 专用网卡,对应eth2，不支持NCSI
    NET_TYPE_PCIE = 3, -- PCIE网卡,对应eth1，部分网卡支持NCSI
    NET_TYPE_AGGREGATION = 4, -- 聚合网卡，对应eth3，不支持NCSI
    NET_TYPE_LOM2 = 5, -- 板载2网卡,V5专用，对应eth1 ，部分网卡支持NCSI
    NET_TYPE_INNER_DEDICATED = 6, -- 非对外专用网卡,不支持NCSI
    NET_TYPE_CABINET_VLAN = 7,   -- 机柜Vlan网口，不支持NCSI
    NET_TYPE_MEZZ_CATED = 9, -- mezz卡
    NET_TYPE_OCP_CARD = 10, -- OCPCard, 对应eth1，与PCIe的NCSI通过开关切换
    NET_TYPE_OCP2_CARD = 11,
    NET_TYPE_MAX = 12
}

local NET_TYPE_STRING_E = {
    NET_TYPE_LOM_STR = 'LOM', -- 板载1网卡，对应eth0，部分网卡支持NCSI
    NET_TYPE_DEDICATED_STR = 'Dedicated', -- 专用网卡,对应eth2，不支持NCSI
    NET_TYPE_PCIE_STR = 'ExternalPCIe', -- PCIE网卡,对应eth1，部分网卡支持NCSI
    NET_TYPE_AGGREGATION_STR = 'Aggregation', -- 聚合网卡，对应eth3，不支持NCSI
    NET_TYPE_LOM2_STR = 'FlexIO', -- 板载2网卡,V5专用，对应eth1 ，部分网卡支持NCSI
    NET_TYPE_OCP_STR = 'OCP', -- OCPCard, 对应eth1，与PCIe的NCSI通过开关切换
    NET_TYPE_OCP2_STR = 'OCP2' -- OCP2Card, 对应eth1，与PCIe的NCSI通过开关切换
}

local LLDP_WORKMODE_E = {
    RFVALUE_LLDP_WORKMODE_TX_CODE = 1,
    RFVALUE_LLDP_WORKMODE_RX_CODE = 2,
    RFVALUE_LLDP_WORKMODE_TX_RX_CODE = 3,
    RFVALUE_LLDP_WORKMODE_DISABLED_CODE = 0
}

local NET_MODE_E = {NET_MODE_MANUAL = 1, NET_MODE_AUTO = 2}

local NET_MODE_STRING_E = {NET_MODE_STRING_FIXED = 'Fixed', NET_MODE_STRING_AUTO = 'Automatic'}

local NCSI_MODE_E = {NCSI_MODE_MANUAL = 0, NCSI_MODE_AUTO = 1}

local NCSI_MODE_STRING_E = {NCSI_MODE_STRING_MANUAL = 'Manual Switch Mode', NCSI_MODE_STRING_AUTO = 'Auto Failover Mode'}

-- 导入失败类型
local REDFISH_ETH_ERR = -1
local REDFISH_VOS_ERROR = -1
local REDFISH_ETH_PARAM_NOT_VALIID = -2
local REDFISH_ETH_UNSUPPORT = -3
local LINK_DISCONNECTED_ERROR = -5
local REDFISH_ETH_AVAFLAG_DISABLED = -8
local COMP_CODE_INVALID_CMD = 0xC1
local ERROR_PARAMETER_INVALID = 0xf009 -- 参数错误码

local errMap = {
    [REDFISH_VOS_ERROR] = reply_bad_request('InvalidHostName'),
    [REDFISH_ETH_PARAM_NOT_VALIID] = reply_bad_request('InvalidIPv6Address'),
    [REDFISH_ETH_UNSUPPORT] =  reply_bad_request('NetPortNoLink'),
    [LINK_DISCONNECTED_ERROR] =  reply_bad_request('NetModeNotSupportActivePort'),
    [REDFISH_ETH_AVAFLAG_DISABLED] =  reply_bad_request('NetPortDisabled')
}

local DNSDomainErrMap = {
    [REDFISH_VOS_ERROR] = reply_bad_request('InvalidNameServer'),
    [COMP_CODE_INVALID_CMD] = reply_bad_request('PropertyModificationNotSupported')
}

local ipv6MaskGatewayErrMap = {
    [REDFISH_ETH_ERR] = reply_bad_request("InvalidIPv6Gateway"),
    [REDFISH_ETH_PARAM_NOT_VALIID] = reply_bad_request("InvalidIPv6Gateway"),
    [COMP_CODE_INVALID_CMD] = reply_bad_request('PropertyModificationNotSupported')
}

local ipMaskGatewayErrMap = {
    [COMP_CODE_INVALID_CMD] = reply_bad_request('PropertyModificationNotSupported'),
    [ERROR_PARAMETER_INVALID] = reply_bad_request('InvalidIPv4Address')
}

local M = {}

function GetEthGroup()
    local EthGroup = O.EthGroup0
    if IsMultiplaneSupport() then
        local outGroupId = C.EthFunction('Functiontype', FN_TYPE_OUTTER_OM).Plane1GroupId:fetch_or()
        if outGroupId == nil then
            return nil
        end
        EthGroup = C.EthGroup('GroupID', outGroupId)
    end
    return EthGroup
end

local function GetBoardType()
    local type = C.PRODUCT[0].MgmtSoftWareType:fetch()
    local chassis_type = bit.band(type, TYPE_MASK_E.CHASSIS_TYPE_MASK)

    if chassis_type == CHASSIS_TYPE_E.CHASSIS_TYPE_SWITCH then
        return BOARD_TYPE_E.BOARD_SWITCH
    elseif chassis_type == CHASSIS_TYPE_E.CHASSIS_TYPE_BLADE or chassis_type ==
        CHASSIS_TYPE_E.CHASSIS_TYPE_X_SERIAL then
        return BOARD_TYPE_E.BOARD_BLADE
    elseif chassis_type == CHASSIS_TYPE_E.CHASSIS_TYPE_MM then
        return BOARD_TYPE_E.BOARD_MM
    elseif chassis_type == CHASSIS_TYPE_E.CHASSIS_TYPE_RACK then
        return BOARD_TYPE_E.BOARD_RACK
    else
        return BOARD_TYPE_E.BOARD_OTHER
    end
end

local function GetNetworkMode()
    local EthGroup = GetEthGroup()
    if GetBoardType() == BOARD_TYPE_E.BOARD_SWITCH or EthGroup.NetModeFlag:fetch() == 0 then
        return nil
    end
    local type = EthGroup.NetMode:fetch()
    if type == NET_MODE_E.NET_MODE_MANUAL then
        return NET_MODE_STRING_E.NET_MODE_STRING_FIXED
    elseif type == NET_MODE_E.NET_MODE_AUTO then
        return NET_MODE_STRING_E.NET_MODE_STRING_AUTO
    end
end

local function GetNcsiMode()
    local EthGroup = GetEthGroup()
    if GetBoardType() == BOARD_TYPE_E.BOARD_SWITCH or EthGroup.NetModeFlag:fetch() == 0 or EthGroup.NetMode:fetch() == 1 then
        return nil
    end
    local type = EthGroup.NcsiMode:fetch()
    if type == NCSI_MODE_E.NCSI_MODE_MANUAL then
        return NCSI_MODE_STRING_E.NCSI_MODE_STRING_MANUAL
    elseif type == NCSI_MODE_E.NCSI_MODE_AUTO then
        return NCSI_MODE_STRING_E.NCSI_MODE_STRING_AUTO
    end
end

local function IPVersion2Str(ipVer)
    if ipVer == 0 then
        return 'IPv4'
    elseif ipVer == 1 then
        return 'IPv6'
    elseif ipVer == 2 then
        return 'IPv4AndIPv6'
    else
        logging:error('invalid ip version value is %d', ipVer)
        return nil
    end
end

local function IPVersion2Int(ipVer)
    if ipVer == 'IPv4' then
        return 0
    elseif ipVer == 'IPv6' then
        return 1
    elseif ipVer == 'IPv4AndIPv6' then
        return 2
    else
        logging:error('invalid ip version str is %s', ipVer)
        return nil
    end
end

local function Int2Boolen(flag)
    if flag == 1 then
        return true
    else
        return false
    end
end

local function LinkStatus2Str(status)
    if status == 0 then
        return 'Disconnected'
    elseif status == 1 then
        return 'Connected'
    else
        logging:error('invalid link status is %d', status)
        return nil
    end
end

local function NetTypeInt2Str(type)
    if type == NET_TYPE_E.NET_TYPE_LOM then
        return NET_TYPE_STRING_E.NET_TYPE_LOM_STR
    elseif type == NET_TYPE_E.NET_TYPE_DEDICATED or type == NET_TYPE_E.NET_TYPE_CABINET_VLAN then
        return NET_TYPE_STRING_E.NET_TYPE_DEDICATED_STR
    elseif type == NET_TYPE_E.NET_TYPE_PCIE then
        return NET_TYPE_STRING_E.NET_TYPE_PCIE_STR
    elseif type == NET_TYPE_E.NET_TYPE_AGGREGATION then
        return NET_TYPE_STRING_E.NET_TYPE_AGGREGATION_STR
    elseif type == NET_TYPE_E.NET_TYPE_LOM2 then
        return NET_TYPE_STRING_E.NET_TYPE_LOM2_STR
    elseif type == NET_TYPE_E.NET_TYPE_OCP_CARD then
        return NET_TYPE_STRING_E.NET_TYPE_OCP_STR
    elseif type == NET_TYPE_E.NET_TYPE_OCP2_CARD then
        return NET_TYPE_STRING_E.NET_TYPE_OCP2_STR
    else
        logging:error('invalid type is %d', type)
        return nil
    end
end

local function NetTypeStr2Int(str)
    if str == NET_TYPE_STRING_E.NET_TYPE_LOM_STR then
        return NET_TYPE_E.NET_TYPE_LOM
    elseif str == NET_TYPE_STRING_E.NET_TYPE_DEDICATED_STR then
        return NET_TYPE_E.NET_TYPE_DEDICATED
    elseif str == NET_TYPE_STRING_E.NET_TYPE_PCIE_STR then
        return NET_TYPE_E.NET_TYPE_PCIE
    elseif str == NET_TYPE_STRING_E.NET_TYPE_AGGREGATION_STR then
        return NET_TYPE_E.NET_TYPE_AGGREGATION
    elseif str == NET_TYPE_STRING_E.NET_TYPE_LOM2_STR then
        return NET_TYPE_E.NET_TYPE_LOM2
    elseif str == NET_TYPE_STRING_E.NET_TYPE_OCP_STR then
        return NET_TYPE_E.NET_TYPE_OCP_CARD
    elseif str == NET_TYPE_STRING_E.NET_TYPE_OCP2_STR then
        return NET_TYPE_E.NET_TYPE_OCP2_CARD
    else
        logging:error('invalid str is %s', str)
        return nil
    end
end

local function GetPortNumber(handle, type)
    local activePort = handle.ActivePort:fetch_or()
    local ncsiPort = handle.NCSIPort:fetch_or()
    local groupid = handle.GroupID:fetch_or()
    local sys_name = get_system_name()
    local portNumber = nil
    if type ~= NET_TYPE_E.NET_TYPE_LOM and type ~= NET_TYPE_E.NET_TYPE_PCIE and type ~= NET_TYPE_E.NET_TYPE_LOM2 and
        type ~= NET_TYPE_E.NET_TYPE_OCP_CARD and type ~= NET_TYPE_E.NET_TYPE_OCP2_CARD then
            if sys_name ~= 'RMM' and sys_name ~= 'DA122C' and IsMultiplaneSupport() == false and not IsPangeaPacificSeries() then
                return 1
            else
                portNumber = C.Eth():fold(function(obj)
                    if obj.Num:fetch_or() == activePort and obj.GroupID:fetch_or() == groupid then
                        return obj.id:fetch(), false
                    end
                end):fetch_or() 
            end
    else
        portNumber = C.BusinessPort():fold(function(obj)
            if obj.EthNum:fetch() == activePort and obj.PortNum:fetch() == ncsiPort then
                return obj.SilkNum:fetch(), false
            end
        end):fetch_or()
    end       
    return portNumber
end

local function GetManagementNetworkPort()
    if GetBoardType() == BOARD_TYPE_E.BOARD_SWITCH then
        return nil
    end

    local EthGroup = GetEthGroup()
    if EthGroup.NetModeFlag:fetch() == 0 then
        logging:error('netport not support adaptive')
        return nil
    end

    local type = EthGroup.Type:fetch()
    if O.PRODUCT.MgmtSoftWareType:fetch() == 0x24 then
        return {Type = NetTypeInt2Str(type), PortNumber = EthGroup.ActivePort:fetch()}
    end
    return {Type = NetTypeInt2Str(type), PortNumber = GetPortNumber(EthGroup, type)}
end

local function GetOcp2Card(obj, mgnportHandle, net_mode)
    local ocp2card = {}
    if mgnportHandle.OCP2NcsiEnable:fetch() == 0 then
        logging:error('ocp2 ncsi not support')
        return ocp2card
    end
    obj:ref_obj('RefNetCard'):next(function(refObj)
        if refObj.NcsiSupported:fetch() ~= 1 then
            return
        end
        local availableFlag = obj.AvailableFlag:fetch()
        if availableFlag == BUSY_PORT_AVAILABLEFLAG_E.AVAILABLE_FLAG_VIRTUAL then
            return
        end
        if availableFlag == BUSY_PORT_AVAILABLEFLAG_E.AVAILABLE_FLAG_DISABLED then
            ocp2card['LinkStatus'] = 'Disabled'
        else
            ocp2card['LinkStatus'] = LinkStatus2Str(obj.LinkStatus:fetch())
        end
        ocp2card['AdaptiveFlag'] = obj.AdaptiveFlag:next(Int2Boolen):fetch()
        ocp2card['PortNumber'] = obj.SilkNum:fetch()
    end):fetch()
    return ocp2card
end

local function GetDedicatedPort(sys_name)
    return C.Eth():fold(function(obj, acc)
        if obj.Type:fetch() == NET_TYPE_E.NET_TYPE_DEDICATED or obj.Type:fetch() == NET_TYPE_E.NET_TYPE_CABINET_VLAN then
            if obj.OutType:fetch() == ETHERNET_TYPE_E.OUT_ETHERNET then
                local linkstatus = LinkStatus2Str(obj.EthLinkStatus:fetch())
                local adaptiveflag = obj.AdaptiveFlag:next(Int2Boolen):fetch()
                local portnumber = obj.id:fetch()
                local ethnum = obj.Num:fetch()
                local ethtype = nil
                if ethnum == 0 and sys_name == 'RMM' then
                    ethtype = 'FE'
                elseif (ethnum == 2 or ethnum == 3) and sys_name == 'RMM' then
                    ethtype = 'GE'
                elseif ethnum == 2 and sys_name == 'DA122C' then
                    ethtype = 'FE'
                elseif ethnum == 3 and sys_name == 'DA122C' then
                    ethtype = 'GE'
                    linkstatus = 'Connected'
                else
                    ethtype = '10GE'
                end
                acc[#acc + 1] = {
                    LinkStatus = linkstatus,
                    AdaptiveFlag = adaptiveflag,
                    PortNumber = portnumber .. '(' .. ethtype .. ')'
                }
            end
        end
  
        return acc
    end, {}):fetch_or()
end

local function GetDedicatedMultiplanePort(obj)
    local Port = {}
    local outGroupId = C.EthFunction('Functiontype', FN_TYPE_OUTTER_OM).Plane1GroupId:fetch_or()
    if obj.Type:fetch() == NET_TYPE_E.NET_TYPE_DEDICATED and obj.GroupID:fetch() == outGroupId then
        if obj.OutType:fetch() == ETHERNET_TYPE_E.OUT_ETHERNET then
            Port['LinkStatus'] = LinkStatus2Str(obj.EthLinkStatus:fetch())
            Port['AdaptiveFlag'] = obj.AdaptiveFlag:next(Int2Boolen):fetch()
            Port['PortNumber'] = obj.id:fetch()
            Port['PortName'] = 'Mgmt'
            return Port
        end
    end
    return nil
end

local function GetPortNum(obj)
    if O.PRODUCT.ProductId:fetch() == 0x0d then
        return obj.id:fetch_or()
    else
        return 1
    end
end

local function GetAllBMCNetport()
    if GetBoardType() == BOARD_TYPE_E.BOARD_SWITCH then
        return nil
    end

    local EthGroup = GetEthGroup()
    if EthGroup.NetModeFlag:fetch() == 0 then -- 不支持自适应，返回空数组
        logging:error('netport not support adaptive')
        return nil
    end

    local net_mode = EthGroup.NetMode:fetch()

    local dedicated = {}
    local aggregation = {}
    local members = {}
    local sys_name = get_system_name()
    C.Eth():fold(function(obj)
        -- Dedicated
        if sys_name == 'RMM' or sys_name == 'DA122C' then
            dedicated = GetDedicatedPort(sys_name)
        elseif IsMultiplaneSupport() then
            local PlanePort = GetDedicatedMultiplanePort(obj)
            table.insert(dedicated, PlanePort)
        elseif IsPangeaPacificSeries() then
            -- 存储管理口存在数组和管理板相关的端口
            local dedicated_temp = {}
            if obj.Type:fetch_or() == NET_TYPE_E.NET_TYPE_DEDICATED then
                if obj.OutType:fetch_or() == ETHERNET_TYPE_E.OUT_ETHERNET then
                    dedicated_temp['LinkStatus'] = LinkStatus2Str(obj.EthLinkStatus:fetch_or())
                    dedicated_temp['AdaptiveFlag'] = obj.AdaptiveFlag:next(Int2Boolen):fetch_or()
                    dedicated_temp['PortNumber'] = obj.id:fetch_or()
                    dedicated_temp['PortName'] = string.format("eth%d", obj.id:fetch_or())
                else
                    logging:error('is not out type eth')
                end
            end
            if next(dedicated_temp) ~= nil then
                dedicated[#dedicated + 1] = dedicated_temp
            end
        else
            if obj.Type:fetch() == NET_TYPE_E.NET_TYPE_DEDICATED then
                if obj.OutType:fetch() == ETHERNET_TYPE_E.OUT_ETHERNET then
                    dedicated['LinkStatus'] = LinkStatus2Str(obj.EthLinkStatus:fetch())
                    dedicated['AdaptiveFlag'] = obj.AdaptiveFlag:next(Int2Boolen):fetch()
                    dedicated['PortNumber'] = GetPortNum(obj)
                else
                    logging:error('is not out type eth')
                end
            end
        end
        -- Aggregation
        if obj.Type:fetch() == NET_TYPE_E.NET_TYPE_AGGREGATION then
            if obj.OutType:fetch() == ETHERNET_TYPE_E.OUT_ETHERNET then
                if C.MngtPortCap[0].AggregationEnable:fetch() ~= 0 then
                    table.insert(aggregation,
                        {LinkStatus = LinkStatus2Str(obj.EthLinkStatus:fetch()), PortNumber = 1})
                else
                    logging:error('aggregation port not enabled')
                end
            else
                logging:error('is not out type eth')
            end
        end
    end):fetch()
    members[NetTypeInt2Str(NET_TYPE_E.NET_TYPE_DEDICATED)] = dedicated
    members[NetTypeInt2Str(NET_TYPE_E.NET_TYPE_AGGREGATION)] = aggregation

    local lomArray = {}
    local pcieArray = {}
    local lom2Array = {}
    local ocpcardArray = {}
    local ocp2cardArray = {}
    local mgnportHandle = C.MngtPortCap[0]
    C.BusinessPort():fold(function(obj)
        local cartType = obj.CardType:fetch()
        local showFlag = obj.AlwaysAvailable:fetch()
        -- LOM
        if cartType == NET_TYPE_E.NET_TYPE_LOM then
            local lom = {}
            obj:ref_obj('RefNetCard'):next(function(refObj)
                if refObj.NcsiSupported:fetch() == 1 then
                    if obj.AvailableFlag:fetch() == BUSY_PORT_AVAILABLEFLAG_E.AVAILABLE_FLAG_DISABLED then
                        lom['LinkStatus'] = 'Disabled'
                    else
                        lom['LinkStatus'] = LinkStatus2Str(obj.LinkStatus:fetch())
                    end
                    lom['AdaptiveFlag'] = obj.AdaptiveFlag:next(Int2Boolen):fetch()
                    lom['PortNumber'] = obj.SilkNum:fetch()
                end
            end):fetch()
            if next(lom) ~= nil then
                lomArray[#lomArray + 1] = lom
            end
        end
        -- PCIe
        if cartType == NET_TYPE_E.NET_TYPE_PCIE and showFlag ~= 0xff then
            local pcie = {}
            if mgnportHandle.PcieNcsiEnable:fetch() ~= 0 then
                obj:ref_obj('RefNetCard'):next(function(refObj)
                    if refObj.NcsiSupported:fetch() == 1 then
                        local availableFlag = obj.AvailableFlag:fetch()
                        if availableFlag ~= BUSY_PORT_AVAILABLEFLAG_E.AVAILABLE_FLAG_VIRTUAL then
                            if availableFlag == BUSY_PORT_AVAILABLEFLAG_E.AVAILABLE_FLAG_DISABLED then
                                pcie['LinkStatus'] = 'Disabled'
                            else
                                pcie['LinkStatus'] = LinkStatus2Str(obj.LinkStatus:fetch())
                            end
                            pcie['AdaptiveFlag'] = obj.AdaptiveFlag:next(Int2Boolen):fetch()
                            pcie['PortNumber'] = obj.SilkNum:fetch()
                        end
                    end
                end):fetch()
            else
                logging:error('pcis ncsi Cable not present')
            end
            if next(pcie) ~= nil then
                pcieArray[#pcieArray + 1] = pcie
            end
        end
        -- LOM2
        if cartType == NET_TYPE_E.NET_TYPE_LOM2 then
            local lom2 = {}
            if mgnportHandle.Lom2NcsiEnable:fetch() ~= 0 then
                obj:ref_obj('RefNetCard'):next(function(refObj)
                    if refObj.NcsiSupported:fetch() == 1 then
                        local availableFlag = obj.AvailableFlag:fetch()
                        if availableFlag == BUSY_PORT_AVAILABLEFLAG_E.AVAILABLE_FLAG_DISABLED then
                            lom2['LinkStatus'] = 'Disabled'
                        else
                            lom2['LinkStatus'] = LinkStatus2Str(obj.LinkStatus:fetch())
                        end
                        lom2['AdaptiveFlag'] = obj.AdaptiveFlag:next(Int2Boolen):fetch()
                        lom2['PortNumber'] = obj.SilkNum:fetch()
                    end
                end):fetch()
            else
                logging:error('pcis flexIO Cable not present')
            end
            if next(lom2) ~= nil then
                lom2Array[#lom2Array + 1] = lom2
            end
        end
        -- OCP
        if cartType == NET_TYPE_E.NET_TYPE_OCP_CARD then
            local ocpcard = {}
            if mgnportHandle.OCPNcsiEnable:fetch() ~= 0 then
                obj:ref_obj('RefNetCard'):next(function(refObj)
                    if refObj.NcsiSupported:fetch() == 1 then
                        local availableFlag = obj.AvailableFlag:fetch()
                        if availableFlag ~= BUSY_PORT_AVAILABLEFLAG_E.AVAILABLE_FLAG_VIRTUAL then
                            if availableFlag == BUSY_PORT_AVAILABLEFLAG_E.AVAILABLE_FLAG_DISABLED then
                                ocpcard['LinkStatus'] = 'Disabled'
                            else
                                ocpcard['LinkStatus'] = LinkStatus2Str(obj.LinkStatus:fetch())
                            end
                            ocpcard['AdaptiveFlag'] = obj.AdaptiveFlag:next(Int2Boolen):fetch()
                            ocpcard['PortNumber'] = obj.SilkNum:fetch()
                        end
                    end
                end):fetch()
            else
                logging:error('ocp ncsi not support')
            end
            if next(ocpcard) ~= nil then
                ocpcardArray[#ocpcardArray + 1] = ocpcard
            end
        end
        -- OCP2
        if cartType == NET_TYPE_E.NET_TYPE_OCP2_CARD then
            local ocp2card = GetOcp2Card(obj, mgnportHandle, net_mode)
            if next(ocp2card) ~= nil then
                ocp2cardArray[#ocp2cardArray + 1] = ocp2card
            end
        end
    end):fetch_or()
    members[NetTypeInt2Str(NET_TYPE_E.NET_TYPE_LOM)] = lomArray
    members[NetTypeInt2Str(NET_TYPE_E.NET_TYPE_PCIE)] = pcieArray
    members[NetTypeInt2Str(NET_TYPE_E.NET_TYPE_LOM2)] = lom2Array
    members[NetTypeInt2Str(NET_TYPE_E.NET_TYPE_OCP_CARD)] = ocpcardArray
    members[NetTypeInt2Str(NET_TYPE_E.NET_TYPE_OCP2_CARD)] = ocp2cardArray
    return members
end

local function GetIPv4Config()
    local EthGroup = GetEthGroup()
    local gateway

    local address = EthGroup.IpAddr:fetch()
    local subnetmask = EthGroup.SubnetMask:fetch()
    local ipmode = EthGroup.IpMode:next(function(origin)
        if origin == 1 then
            return 'Static'
        elseif origin == 2 then
            return 'DHCP'
        else
            logging:error('invalid origin is %d', origin)
        end
    end):fetch()

    local softwareType = O.PRODUCT.MgmtSoftWareType:fetch()
    if softwareType == 0x50 then -- SMM集中管理BMC
        gateway =
            C.EthGroup('GroupID', C.EthFunction('Functiontype', 0).Plane1GroupId:fetch()).DefaultGateway:fetch()
        if address == '0.0.0.0' then
            address = nil
        end
        if subnetmask == '0.0.0.0' then
            subnetmask = nil
        end
    else
        gateway = EthGroup.DefaultGateway:fetch()
    end

    return {
        IPAddressMode = ipmode,
        IPAddress = address,
        SubnetMask = subnetmask,
        Gateway = gateway,
        PermanentMACAddress = EthGroup.Mac:fetch()
    }
end

local function GetIPv6Config()
    local gateway
    local addressArray = {}
    local EthGroup = GetEthGroup()
    local softwareType = O.PRODUCT.MgmtSoftWareType:fetch()
    if softwareType == 0x50 then -- SMM集中管理BMC
        gateway =
            C.EthGroup('GroupID', C.EthFunction('Functiontype', 0).Plane1GroupId:fetch()).Ipv6DefaultGateway:fetch()
    else
        gateway = EthGroup.Ipv6DefaultGateway:fetch()
    end

    -- Static/DHCPv6 address
    addressArray[#addressArray + 1] = {
        IPAddress = EthGroup.Ipv6Addr:next(function(ipAddr)
            if #ipAddr == 0 then
                return null
            else
                return ipAddr
            end
        end):fetch(),
        IPAddressMode = EthGroup.Ipv6Mode:next(function(origin)
            if origin == IP_MODE_E.IP_DHCP_MODE then
                return 'DHCPv6'
            else
                return 'Static'
            end
        end):fetch(),
        PrefixLength = EthGroup.Ipv6Prefix:next(function(prefix)
            if prefix == 0 then
                return null
            else
                return prefix
            end
        end):fetch()
    }
    -- LinkLocal address
    addressArray[#addressArray + 1] = {
        IPAddress = EthGroup.Ipv6LocalLink:fetch(),
        IPAddressMode = 'LinkLocal',
        PrefixLength = 64
    }

    -- SLAAC address
    local ret_info = {Gateway = gateway, IPv6Addresses = addressArray}
    local ip_addr = EthGroup.SLAACIpv6Addr:fetch()
    local prefix = EthGroup.SLAACIpv6Prefix:fetch()
    if #ip_addr ~= #prefix then
        return ret_info
    end

    for k, v in ipairs(ip_addr) do
        if #v ~= 0 then
            table.insert(addressArray, {IPAddress = v, IPAddressMode = 'SLAAC', PrefixLength = prefix[k]})
        end
    end
    return ret_info
end

local function GetVlan()
    local EthGroup = GetEthGroup()
    local softwareType = O.PRODUCT.MgmtSoftWareType:fetch_or()
    if softwareType == 0x50 or softwareType == 0x24 then
        return nil
    end

    local vlanStatus = C.EthFunction('Functiontype', 0).VlanStatus:fetch_or(0)
    if vlanStatus ~= 1 and EthGroup.NetModeFlag:fetch_or() == 0 then
        return nil
    end

    local enabled = EthGroup.VlanState:fetch_or()
    if enabled == 0 then
        return {Enabled = false, ID = 0}
    else
        return {Enabled = true, ID = EthGroup.VlanID:fetch_or()}
    end
end

local function GetLldpInfo()
    local sysem_name = get_system_name()
    if sysem_name == 'RMM' then
        return nil
    end
    local LLDPConfig = C.LLDPConfig[0]
    return O.PMEServiceConfig.LLDPSupport:eq(1):next(function()
        return {
            Enabled = LLDPConfig.LLDPEnable:next(Int2Boolen):fetch(),
            WorkMode = LLDPConfig.WorkMode:next(function(mode)
                if mode == LLDP_WORKMODE_E.RFVALUE_LLDP_WORKMODE_TX_CODE then
                    return 'Tx'
                elseif mode == LLDP_WORKMODE_E.RFVALUE_LLDP_WORKMODE_RX_CODE then
                    return 'Rx'
                elseif mode == LLDP_WORKMODE_E.RFVALUE_LLDP_WORKMODE_TX_RX_CODE then
                    return 'TxRx'
                elseif mode == LLDP_WORKMODE_E.RFVALUE_LLDP_WORKMODE_DISABLED_CODE then
                    return 'Disabled'
                else
                    logging:error('workmode code err %d', mode)
                    return null
                end
            end):fetch(),
            TxDelaySeconds = LLDPConfig.TxDelaySeconds:fetch(),
            TxIntervalSeconds = LLDPConfig.TxIntervalSeconds:fetch(),
            TxHold = LLDPConfig.TxHold:fetch()
        }
    end):fetch_or()
end

function M.GetNetworkSettings()
    local DNSSetting = C.DNSSetting[0]

    local EthGroup = GetEthGroup()
    return {
        HostName = C.BMC[0].HostName:fetch(),
        NetworkPort = {
            Mode = GetNetworkMode(),
            NcsiMode = GetNcsiMode(),
            ManagementNetworkPort = GetManagementNetworkPort(),
            Members = GetAllBMCNetport()
        },
        NetworkProtocols = {
            ProtocolMode = EthGroup.IpVersion:next(IPVersion2Str):fetch(),
            IPv4Config = GetIPv4Config(),
            IPv6Config = GetIPv6Config()
        },
        DNS = {
            AddressMode = DNSSetting.Mode:next(function(mode)
                if mode == 0 then
                    return 'Static'
                elseif mode == 1 then
                    local ipVersion = DNSSetting.IPVer:fetch()
                    if ipVersion == 0 then
                        return 'IPv4'
                    elseif ipVersion == 1 then
                        return 'IPv6'
                    else
                        logging:error('invalid dns ip version value is %d', ipVersion)
                        return null
                    end
                else
                    logging:error('invalid dns address origin value is %d', mode)
                    return null
                end
            end):fetch(),
            Domain = DNSSetting.DomainName:fetch(),
            PrimaryServer = DNSSetting.PrimaryDomain:fetch(),
            BackupServer = DNSSetting.BackupDomain:fetch(),
            TertiaryServer = DNSSetting.TertiaryDomain:fetch()
        },
        VLAN = GetVlan(),
        LLDP = GetLldpInfo()
    }
end

-- to do fetch
local function SetIPv4Config(data, user)
    local ret
    local EthGroup = O.EthGroup0
    if IsMultiplaneSupport() then
        local outGroupId = C.EthFunction('Functiontype', FN_TYPE_OUTTER_OM).Plane1GroupId:fetch_or()
        if outGroupId == nil then
            return nil
        end
        EthGroup = C.EthGroup('GroupID', outGroupId):next():fetch_or()
    end

    if data.IPAddressMode == 'DHCP' and (data.IPAddress or data.SubnetMask or data.Gateway) then
        ret = reply_bad_request('IPv4InfoConflictwithDHCP',
            'The request for IPv4Addresses modification failed because the value of IPv4Addresses/IPAddressMode is DHCP.')
        return ret
    end
    local ipMode = 0
    if data.IPAddressMode == 'DHCP' then
        ipMode = IP_MODE_E.IP_DHCP_MODE
    else
        ipMode = IP_MODE_E.IP_STATIC_MODE
    end

    if data.IPAddressMode then
        -- to do ret code
        local ok, err = call_method(user, EthGroup, 'SetIpMode', errMap, gbyte(ipMode))
        if not ok then
            return err
        end
    end

    if not data.IPAddress and not data.SubnetMask and not data.Gateway then
        return
    end

    -- to do single update
    local address
    local gateway
    local subnetmask

    if data.IPAddress then
        address = data.IPAddress
    else
        address = EthGroup.IpAddr:fetch_or('')
    end

    if data.SubnetMask then
        subnetmask = data.SubnetMask
    else
        subnetmask = EthGroup.SubnetMask:fetch_or('')
    end

    if data.Gateway then
        gateway = data.Gateway
    else
        local softwareType = O.PRODUCT.MgmtSoftWareType:fetch()
        if softwareType == 0x50 then -- SMM集中管理BMC
            gateway = C.EthGroup('GroupID',
                C.EthFunction('Functiontype', 0).Plane1GroupId:fetch()).DefaultGateway:fetch_or('')
        else
            gateway = EthGroup.DefaultGateway:fetch_or('')
        end
    end

    local ok, err = call_method(user, EthGroup, 'SetIpMaskGateway', function(errCode)
        if ipMaskGatewayErrMap[errCode] then
            return ipMaskGatewayErrMap[errCode]
        end
        return reply_bad_request("IPv4AddressConflict")
    end, gstring(address), gstring(subnetmask), gstring(gateway), gbyte(7))
    if not ok then
        return err
    end
end

-- to do 管理板 and fetch
local function SetIPv6Config(data, user)
    local ret
    local EthGroup = O.EthGroup0
    if IsMultiplaneSupport() then
        local outGroupId = C.EthFunction('Functiontype', FN_TYPE_OUTTER_OM).Plane1GroupId:fetch_or()
        if outGroupId == nil then
            return nil
        end
        EthGroup = C.EthGroup('GroupID', outGroupId):next():fetch_or()
    end

    if EthGroup.IpVersion:next(IPVersion2Str):fetch() == 'IPv4' then
        ret = reply_bad_request('ModifyFailedWithIPVersion',
            'Failed to modify IPv6Addresses information because IPv6 is disabled.')
        return ret
    end

    -- to do delete ip mask

    if data.IPAddressMode == 'DHCPv6' and (data.IPAddress or data.SubnetMask) then
        ret = reply_bad_request('IPv6InfoConflictwithDHCPv6',
            'The request for IPv6Addresses modification failed because the value of IPv6Addresses/IPAddressMode is DHCPv6.')
        return ret
    end

    local ipMode = 0
    if data.IPAddressMode == 'DHCPv6' then
        ipMode = IP_MODE_E.IP_DHCP_MODE
    else
        ipMode = IP_MODE_E.IP_STATIC_MODE
    end

    if data.IPAddressMode then
        -- to do ret code
        local ok, err = call_method(user, EthGroup, 'SetIpv6Mode', errMap, gbyte(ipMode))
        if not ok then
            return err
        end
    end

    if data.IPAddress then
        if EthGroup.Ipv6DefaultGateway:fetch() == data.IPAddress then
            ret = reply_bad_request('IPv6AddressConflictWithGateway',
                'The value for the property IPv6Address is the same as the property Gateway.')
            return ret
        end

        local ok, err = call_method(user, EthGroup, 'SetIpv6Addr', errMap, gstring(data.IPAddress))
        if not ok then
            return err
        end
    end

    if data.PrefixLength then
        if EthGroup.Ipv6Mode:fetch() == 2 then -- 获取模式为dhcp，不允许设置前缀长度
            ret = reply_bad_request('IPv6PrefixConflictWithDHCPv6',
                'The request for the property PrefixLength modification failed because the address is in DHCPv6 mode.')
            return ret
        end

        local ok, err = call_method(user, EthGroup, 'SetIpv6Prefix', errMap, gbyte(data.PrefixLength))
        if not ok then
            return err
        end
    end

    if data.Gateway then
        local objHandle
        local softwareType = O.PRODUCT.MgmtSoftWareType:fetch()
        if softwareType == 0x50 then -- SMM集中管理BMC
            objHandle = C.EthGroup('GroupID', C.EthFunction('Functiontype', 0).Plane1GroupId:fetch()):fetch()
        else
            objHandle = EthGroup
        end

        if objHandle.IpVersion == 0 then -- IPV4_ONLY
            ret = reply_bad_request('ModifyFailedWithIPVersion',
                'Failed to modify IPv6DefaultGateway information because IPv6 is disabled.')
            return ret
        end

        if softwareType == 0x50 and O.AsmObject.AsStatus:fetch() ~= 0 then
            ret = reply_bad_request('NotAllowedOnStandbyMM',
                'This is the standby management module. Please perform this operation on the active management module.')
            return ret
        end

        if softwareType ~= 0x50 and objHandle.Ipv6Mode:fetch() == IP_MODE_E.IP_DHCP_MODE then
            ret = reply_bad_request('IPv6GatewayConflictWithDHCPv6',
                'The request for the property Gateway modification failed because the address is in DHCPv6 mode.')
            return ret
        end

        if objHandle.Ipv6Addr:fetch() == data.Gateway then
            ret = reply_bad_request('IPv6AddressConflictWithGateway',
                'The value for the property Gateway is the same as the property IPv6Address.')
            return ret
        end

        if softwareType ~= 0x50 and objHandle.Ipv6Addr:fetch() == '' then
            ret = reply_bad_request('ModifyfailedWithEmptyAddress',
                'Failed to modify Gateway because the IPv6Address is empty.')
            return ret
        end

        local ok, err = call_method(user, EthGroup, 'SetIpv6DefaultGateway', ipv6MaskGatewayErrMap, gstring(data.Gateway))
        if not ok then
            return err
        end
    end
end

local function SetDNS(data, user)
    local ret = reply_ok()
    local EthGroup = GetEthGroup()
    ret = C.DNSSetting[0]:next(function(dnsHandle)
        -- AddressMode
        if data.AddressMode == 'Static' then
            local ok, err = call_method(user, dnsHandle, 'SetDdnsMode', errMap, gbyte(0)) -- DNS_ADD_ORIGIN_MANUAL
            if not ok then
                return err
            end
        elseif data.AddressMode == 'IPv4' or data.AddressMode == 'IPv6' then
            local dnsIpVer
            if data.AddressMode == 'IPv4' then
                dnsIpVer = 0
            else
                dnsIpVer = 1
            end

            if (data.AddressMode == 'IPv4' and EthGroup.IpVersion == 1) or
                (data.AddressMode == 'IPv6' and EthGroup.IpVersion == 0) then
                ret =
                    reply_bad_request('ModifyFailedWithIPVersion', 'Failed to modify DNSAddressMode information.')
                return ret
            end

            if (data.AddressMode == 'IPv4' and EthGroup.IpMode == 1) or
                (data.AddressMode == 'IPv6' and EthGroup.Ipv6Mode == 1) then
                ret = reply_bad_request('StaticIPMode', 'Failed to set the address origin')
                return ret
            end

            if dnsHandle.IPVer ~= dnsIpVer then
                local ok, err = call_method(user, dnsHandle, 'SetDdnsIPVer', errMap, gbyte(dnsIpVer)) -- DNS_ADD_ORIGIN_MANUAL
                if not ok then
                    return err
                end
            end

            local ok, err = call_method(user, dnsHandle, 'SetDdnsMode', errMap, gbyte(1)) -- DNS_ADD_ORIGIN_AUTO
            if not ok then
                return err
            end
        end

        -- Domain
        if data.Domain then
            local ok, err = call_method(user, dnsHandle, 'SetDomainName', errMap, gstring(data.Domain)) -- DNS_ADD_ORIGIN_AUTO
            if not ok then
                return err
            end
        end

        if dnsHandle.Mode == 1 and (data.PrimaryServer or data.BackupServer or data.TertiaryServer) then -- DNS_ADD_ORIGIN_AUTO
            ret = reply_bad_request('StaticIPMode', 'Failed to set the address origin')
            return ret
        end

        -- PreferredServer
        if data.PrimaryServer then
            local ok, err = call_method(user, dnsHandle, 'SetPrimaryDomain', DNSDomainErrMap, gstring(data.PrimaryServer)) -- DNS_ADD_ORIGIN_AUTO
            if not ok then
                return err
            end
        end
        -- BackupServer
        if data.BackupServer then
            local ok, err = call_method(user, dnsHandle, 'SetBackupDomain', DNSDomainErrMap, gstring(data.BackupServer)) -- DNS_ADD_ORIGIN_AUTO
            if not ok then
                return err
            end
        end
        -- TertiaryServer
        if data.TertiaryServer then
            local ok, err = call_method(user, dnsHandle, 'SetTertiaryDomain', DNSDomainErrMap, gstring(data.TertiaryServer)) -- DNS_ADD_ORIGIN_AUTO
            if not ok then
                return err
            end
        end
    end):fetch()
    if ret then
        return ret
    end
end

local function SetLLDP(data, user)
    local ret = C.LLDPConfig[0]:next(function(lldpHandle)
        if data.Enabled ~= nil then
            local flag
            if data.Enabled == true then
                flag = 1
            else
                flag = 0
            end
            local ok, err = call_method(user, lldpHandle, 'SetLLDPEnable', errMap, gbyte(flag)) -- DNS_ADD_ORIGIN_AUTO
            if not ok then
                return err
            end
        end

        if data.WorkMode then
            local mode
            if data.WorkMode == 'Tx' then
                mode = 1
            elseif data.WorkMode == 'Rx' then
                mode = 2
            elseif data.WorkMode == 'TxRx' then
                mode = 3
            elseif data.WorkMode == 'Disabled' then
                mode = 0
            end

            local ok, err = call_method(user, lldpHandle, 'SetWorkMode', errMap, gbyte(mode))
            if not ok then
                return err
            end
        end

        if data.TxDelaySeconds then
            local ok, err =
                call_method(user, lldpHandle, 'SetTxDelaySeconds', errMap, guint16(data.TxDelaySeconds))
            if not ok then
                return err
            end
        end

        if data.TxIntervalSeconds then
            local ok, err = call_method(user, lldpHandle, 'SetTxIntervalSeconds', errMap,
                guint16(data.TxIntervalSeconds))
            if not ok then
                return err
            end
        end

        if data.TxHold then
            local ok, err = call_method(user, lldpHandle, 'SetTxHold', errMap, guint16(data.TxHold))
            if not ok then
                return err
            end
        end
    end):fetch()
    if ret then
        return ret
    end
end

local function SetNetworkPortOCP2(user, EthGroup, ocp2)
    local adaptive
    for cnt = 1, #ocp2, 1 do
        if ocp2[cnt].AdaptiveFlag == nil then
            goto continue
        end
        adaptive = ocp2[cnt].AdaptiveFlag and 1 or 0
        local ok, err = call_method(user, EthGroup, 'ChooseAdaptiveEth', errMap,
            gbyte(NET_TYPE_E.NET_TYPE_OCP2_CARD), gbyte((ocp2[cnt].PortNumber)), gbyte(adaptive))
        if not ok then
            return false, err
        end
        ::continue::
    end
    return true
end

local function GetDedicatedPortNum()
    return C.Eth():fold(function(obj)
        if obj.Type:fetch() == NET_TYPE_E.NET_TYPE_DEDICATED and obj.OutType:fetch() ==
            ETHERNET_TYPE_E.OUT_ETHERNET then
            return obj.Num:fetch(), false
        end
    end):fetch_or(nil)
end

local function GetDedicatedPortNum_RM210(PCIeMgntEnable, OldPortNum)
    return C.Eth():fold(function(obj)
        if obj.OutType:fetch() == ETHERNET_TYPE_E.OUT_ETHERNET then
            if obj.Type:fetch() == NET_TYPE_E.NET_TYPE_DEDICATED and PCIeMgntEnable == 0 then
                return obj.Num:fetch(), false
            elseif obj.Type:fetch() == NET_TYPE_E.NET_TYPE_DEDICATED and PCIeMgntEnable == 1 and
                obj.id:fetch() == OldPortNum then
                return obj.Num:fetch(), false
            elseif obj.Type:fetch() == NET_TYPE_E.NET_TYPE_CABINET_VLAN and PCIeMgntEnable == 1 and
                obj.id:fetch() == OldPortNum then
                return obj.Num:fetch(), false
            end
        end
    end):fetch_or(nil)
end

local function GetMultiplaneDedicatedPortNum()
    local EthFunctionObj  = C.EthFunction('Functiontype', FN_TYPE_OUTTER_OM)
    local CurrentPlaneId = EthFunctionObj.CurrentPlaneId:fetch_or(0)
    if CurrentPlaneId == 1 then
        return EthFunctionObj.Plane1EthNum:fetch()
    elseif CurrentPlaneId == 2 then
        return EthFunctionObj.Plane2EthNum:fetch()
    else
        return 1
    end
end

local function SetDedicated(user, EthGroup, IsRM210, Dedicated)
    local PCIeMgntEnable = O.MngtPortCap.PCIeMgntEnable:fetch_or(0)
    for _, Item in ipairs(Dedicated) do
        if Item.AdaptiveFlag == nil then
            goto continue
        end
        local Adaptive = Item.AdaptiveFlag == true and 1 or 0
        local PortNumber
        if not IsRM210 then
            if IsMultiplaneSupport() then
                PortNumber = GetMultiplaneDedicatedPortNum()
            else
                PortNumber = GetDedicatedPortNum()
            end
        else
            PortNumber = GetDedicatedPortNum_RM210(PCIeMgntEnable, Item.PortNumber)
        end

        if not PortNumber then
            return http.reply_bad_request("InvalidPortNumber")
        end

        local ok, err = call_method(user, EthGroup, 'ChooseAdaptiveEth', errMap,
            gbyte(NET_TYPE_E.NET_TYPE_DEDICATED), gbyte(PortNumber), gbyte(Adaptive))
        if not ok then
            err:appendErrorData(M.GetNetworkSettings())
            return err
        end
        ::continue::
    end
end

function M.SetNetworkSettings(data, user)
    local ret
    local EthGroup = O.EthGroup0
    local sys_name = get_system_name()
    if IsMultiplaneSupport() then
        local outGroupId = C.EthFunction('Functiontype', FN_TYPE_OUTTER_OM).Plane1GroupId:fetch_or()
        if outGroupId == nil then
            return nil
        end
        EthGroup = C.EthGroup('GroupID', outGroupId):next(function(obj)
            return obj
        end):fetch()
    end

    if data.HostName then
        ret = C.BMC[0]:next(function(obj)
            local ok, err = call_method(user, obj, 'SetHostName', errMap, gstring(data.HostName))
            if not ok then
                return err
            end
        end):fetch()
        if ret then
            ret:appendErrorData(M.GetNetworkSettings())
            return ret
        end
    end

    if data.NetworkPort and data.NetworkPort.Mode then
        if GetBoardType() == BOARD_TYPE_E.BOARD_SWITCH or EthGroup.NetModeFlag:fetch() == 0 then
            ret = reply_bad_request('PropertyModificationNotSupported',
                'The property modification failed because it is not supported.')
            ret:appendErrorData(M.GetNetworkSettings())
            return ret
        end
        local netMode
        if data.NetworkPort.Mode == 'Fixed' then
            netMode = 1
        else
            netMode = 2
        end
        local ok, err = call_method(user, EthGroup, 'SetNetMode', errMap, gbyte(netMode))
        if not ok then
            err:appendErrorData(M.GetNetworkSettings())
            return err
        end
    end

    if data.NetworkPort and data.NetworkPort.NcsiMode then
        if GetBoardType() == BOARD_TYPE_E.BOARD_SWITCH or EthGroup.NetModeFlag:fetch() == 0 or EthGroup.NetMode:fetch() == 1 then
            ret = reply_bad_request('PropertyModificationNotSupported',
                'The property modification failed because it is not supported.')
            ret:appendErrorData(M.GetNetworkSettings())
            return ret
        end
        local ncsiMode
        if data.NetworkPort.NcsiMode == 'Manual Switch Mode' then
            ncsiMode = 0
        else
            ncsiMode = 1
        end
        local ok, err = call_method(user, EthGroup, 'SetNcsiMode', errMap, gbyte(ncsiMode))
        if not ok then
            err:appendErrorData(M.GetNetworkSettings())
            return err
        end
    end

    if data.NetworkPort and data.NetworkPort.ManagementNetworkPort then
        if NetTypeStr2Int(data.NetworkPort.ManagementNetworkPort.Type) ~= nil and
            data.NetworkPort.ManagementNetworkPort.PortNumber ~= nil then
            -- check type port exit and so on
            local ok, err = call_method(user, EthGroup, 'SetActivePort', errMap,
                gbyte(NetTypeStr2Int(data.NetworkPort.ManagementNetworkPort.Type)),
                gbyte(data.NetworkPort.ManagementNetworkPort.PortNumber))
            if not ok then
                err:appendErrorData(M.GetNetworkSettings())
                return err
            end
        end
    end

    if data.NetworkPort and data.NetworkPort.Members then
        if GetBoardType() == BOARD_TYPE_E.BOARD_SWITCH or EthGroup.NetModeFlag:fetch() == 0 then
            ret = reply_bad_request('PropertyModificationNotSupported',
                'The property AdaptivePort modification failed because it is not supported.')
            ret:appendErrorData(M.GetNetworkSettings())
            return ret
        end

        if EthGroup.NetMode:fetch() ~= NET_MODE_E.NET_MODE_AUTO then
            ret = reply_bad_request('NetmodeNotSupportAdaptive',
                'Modification failed because the network port mode is the fixed mode.')
            ret:appendErrorData(M.GetNetworkSettings())
            return ret
        end

        -- LOM
        if data.NetworkPort.Members.LOM then
            local adaptive
            for cnt = 1, #data.NetworkPort.Members.LOM, 1 do
                if data.NetworkPort.Members.LOM[cnt].PortNumber == nil then
                    ret = reply_bad_request('PropertyMissing',
                    'The property PortNumber is a required property and must be included in the request.')
                    ret:appendErrorData(M.GetNetworkSettings())
                    return ret
                end

                if data.NetworkPort.Members.LOM[cnt].AdaptiveFlag == nil then
                    goto continue
                end

                if data.NetworkPort.Members.LOM[cnt].AdaptiveFlag == true then
                    adaptive = 1
                else
                    adaptive = 0
                end

                local ok, err = call_method(user, EthGroup, 'ChooseAdaptiveEth', errMap,
                    gbyte(NET_TYPE_E.NET_TYPE_LOM), gbyte((data.NetworkPort.Members.LOM[cnt].PortNumber)),
                    gbyte(adaptive))
                if not ok then
                    err:appendErrorData(M.GetNetworkSettings())
                    return err
                end
                ::continue::
            end
        end
        -- Dedicated
        if data.NetworkPort.Members.Dedicated then
            local err = SetDedicated(user, EthGroup, sys_name == 'RMM' or sys_name == 'DA122C', data.NetworkPort.Members.Dedicated)
            if err then
                return err
            end
        end
        -- ExternalPCIe
        if data.NetworkPort.Members.ExternalPCIe then
            local adaptive
            for cnt = 1, #data.NetworkPort.Members.ExternalPCIe, 1 do
                if data.NetworkPort.Members.ExternalPCIe[cnt].AdaptiveFlag == nil then
                    goto continue
                end

                if data.NetworkPort.Members.ExternalPCIe[cnt].AdaptiveFlag == true then
                    adaptive = 1
                else
                    adaptive = 0
                end
                local ok, err = call_method(user, EthGroup, 'ChooseAdaptiveEth', errMap,
                    gbyte(NET_TYPE_E.NET_TYPE_PCIE),
                    gbyte((data.NetworkPort.Members.ExternalPCIe[cnt].PortNumber)), gbyte(adaptive))
                if not ok then
                    err:appendErrorData(M.GetNetworkSettings())
                    return err
                end
                ::continue::
            end
        end
        -- OCP
        if data.NetworkPort.Members.OCP then
            local adaptive
            for cnt = 1, #data.NetworkPort.Members.OCP, 1 do
                if data.NetworkPort.Members.OCP[cnt].AdaptiveFlag == nil then
                    goto continue
                end

                if data.NetworkPort.Members.OCP[cnt].AdaptiveFlag == true then
                    adaptive = 1
                else
                    adaptive = 0
                end
                local ok, err = call_method(user, EthGroup, 'ChooseAdaptiveEth', errMap,
                    gbyte(NET_TYPE_E.NET_TYPE_OCP_CARD), gbyte((data.NetworkPort.Members.OCP[cnt].PortNumber)),
                    gbyte(adaptive))
                if not ok then
                    err:appendErrorData(M.GetNetworkSettings())
                    return err
                end
                ::continue::
            end
        end
        -- OCP2
        if data.NetworkPort.Members.OCP2 then
            local ok, err = SetNetworkPortOCP2(user, EthGroup, data.NetworkPort.Members.OCP2)
            if not ok then
                err:appendErrorData(M.GetNetworkSettings())
                return err
            end
        end
        -- FlexIO
        if data.NetworkPort.Members.FlexIO then
            local adaptive
            for cnt = 1, #data.NetworkPort.Members.FlexIO, 1 do
                if data.NetworkPort.Members.FlexIO[cnt].AdaptiveFlag == nil then
                    goto continue
                end
                
                if data.NetworkPort.Members.FlexIO[cnt].AdaptiveFlag == true then
                    adaptive = 1
                else
                    adaptive = 0
                end
                local ok, err = call_method(user, EthGroup, 'ChooseAdaptiveEth', errMap,
                    gbyte(NET_TYPE_E.NET_TYPE_LOM2), gbyte((data.NetworkPort.Members.FlexIO[cnt].PortNumber)),
                    gbyte(adaptive))
                if not ok then
                    err:appendErrorData(M.GetNetworkSettings())
                    return err
                end
                ::continue::
            end
        end
    end

    -- ProtocolMode
    if data.NetworkProtocols and data.NetworkProtocols.ProtocolMode then
        local ok, err = call_method(user, EthGroup, 'SetIpVersion', errMap,
            gbyte(IPVersion2Int(data.NetworkProtocols.ProtocolMode)))
        if not ok then
            err:appendErrorData(M.GetNetworkSettings())
            return err
        end
    end

    --  IPv4Config // to do 管理板
    if data.NetworkProtocols and data.NetworkProtocols.IPv4Config then
        ret = SetIPv4Config(data.NetworkProtocols.IPv4Config, user)
        if ret and ret:isOk() == false then
            ret:appendErrorData(M.GetNetworkSettings())
            return ret
        end
    end

    --  IPv6Config
    if data.NetworkProtocols and data.NetworkProtocols.IPv6Config then
        ret = SetIPv6Config(data.NetworkProtocols.IPv6Config, user)
        if ret and ret:isOk() == false then
            ret:appendErrorData(M.GetNetworkSettings())
            return ret
        end
    end

    -- DNS
    if data.DNS then
        ret = SetDNS(data.DNS, user)
        if ret and ret:isOk() == false then
            ret:appendErrorData(M.GetNetworkSettings())
            return ret
        end
    end

    -- VLAN   // to check input
    if data.VLAN and (data.VLAN.ID ~= nil or data.VLAN.Enabled ~= nil) then
        local flag
        local id = data.VLAN.ID

        if EthGroup.NetModeFlag:fetch() == 0 then
            ret = reply_bad_request('PropertyModificationNotSupported',
                'The property VLAN modification failed because it is not supported.')
            ret:appendErrorData(M.GetNetworkSettings())
            return ret
        end

        if data.VLAN.Enabled == true and data.VLAN.ID == nil then
            ret = reply_bad_request('PropertyMissing',
                'The property ID is a required property and must be included in the request.')
            ret:appendErrorData(M.GetNetworkSettings())
            return ret
        end

        if data.VLAN.Enabled == false then
            id = 0
            flag = 0
        else
            flag = 1
        end

        local ok, err = call_method(user, EthGroup, 'SetVlanID', errMap, guint16(id), guint32(flag))
        if not ok then
            err:appendErrorData(M.GetNetworkSettings())
            return err
        end
    end

    -- LLDP
    if data.LLDP then
        ret = SetLLDP(data.LLDP, user)
        if ret and ret:isOk() == false then
            ret:appendErrorData(M.GetNetworkSettings())
            return ret
        end
    end

    return reply_ok_encode_json(M.GetNetworkSettings())
end

return M
