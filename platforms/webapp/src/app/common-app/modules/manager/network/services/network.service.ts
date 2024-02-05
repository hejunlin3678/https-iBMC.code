import { Injectable } from '@angular/core';
import { HttpService, PRODUCT } from 'src/app/common-app/service';
import { TranslateService } from '@ngx-translate/core';
import { Observable } from 'rxjs/internal/Observable';
import { NetworkPort, NetworkLldp, NetworkProtocol, NetworkVlan, NetworkDns } from '../model';
import { CommonData } from 'src/app/common-app/models';
@Injectable({
    providedIn: 'root'
})
export class NetworkService {
    constructor(private http: HttpService, private translate: TranslateService) {}
    public i18n: any = {
        IBMC_IPV6_ADDRES_ERROR_1: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_1'),
        IBMC_IPV6_ADDRES_ERROR_2: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_2'),
        IBMC_IPV6_ADDRES_ERROR_3: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_3'),
        IBMC_IPV6_ADDRES_ERROR_4: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
        IBMC_IPV6_ADDRES_ERROR_5: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_5'),
        IBMC_IPV6_ADDRES_ERROR_6: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_6'),
        IBMC_IPV6_ADDRES_ERROR_7: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_7')
    };
    public isIRM210: boolean = false;

    // 保存网络配置
    public saveNetWork(params) {
        return this.http.patch('/UI/Rest/BMCSettings/NetworkSettings', params);
    }

    public networkList = () => {
        return new Observable((observer) => {
            this.http.get('/UI/Rest/BMCSettings/NetworkSettings').subscribe((response) => {
                const networkeData = response['body'];
                const networkPort = new NetworkPort();
                const networkLldp = new NetworkLldp();
                const networkProtocol = new NetworkProtocol();
                const networkVlan = new NetworkVlan();
                const networkDns = new NetworkDns();
                this.portDetails(networkPort, networkeData);
                this.networkProtocolDetails(networkProtocol, networkeData);
                this.networkDnsDetails(networkDns, networkeData);
                const lldpShowData = networkeData.LLDP;
                networkLldp.setLldpSupport(lldpShowData ? true : false);
                if (lldpShowData) {
                    this.lldpDataDetails(networkLldp, networkeData);
                }
                this.networkVlanDetails(networkVlan, networkeData);
                observer.next([networkPort, networkProtocol, networkLldp, networkDns, networkVlan]);
            });
        });
    }
    public networkProtocolDetails = (networkProtocol, networkeData) => {
        // 网络协议
        networkProtocol.setNetIPVersion(networkeData.NetworkProtocols.ProtocolMode);
        networkProtocol.setNetModelV4(networkeData.NetworkProtocols.IPv4Config.IPAddressMode);
        networkProtocol.setIpAddressV4(networkeData.NetworkProtocols.IPv4Config.IPAddress);
        networkProtocol.setMaskCodeV4(networkeData.NetworkProtocols.IPv4Config.SubnetMask);
        networkProtocol.setGatewayV4(networkeData.NetworkProtocols.IPv4Config.Gateway);
        networkProtocol.setAddMacV4(networkeData.NetworkProtocols.IPv4Config.PermanentMACAddress);
        // IPv6初始化
        const tmpArray = networkeData.NetworkProtocols.IPv6Config.IPv6Addresses;
        let ipAddressValue = '';
        let prefixLength = '';
        let localprefix = '';
        let localLink = '';
        let j = 2;
        const iPNameAddressValue = [];
        iPNameAddressValue.splice(0, iPNameAddressValue.length);
        for (const key of tmpArray) {
            if (key.IPAddressMode === 'Static' || key.IPAddressMode === 'DHCPv6') {
                ipAddressValue = key.IPAddress !== null ? key.IPAddress : '';
                prefixLength = key.PrefixLength !== null ? key.PrefixLength : '';
            } else if (key.IPAddressMode === 'LinkLocal') {
                localLink = key.IPAddress;
                localprefix = key.PrefixLength;
            } else if (key.IPAddressMode === 'SLAAC') {
                networkProtocol.setIpAddressShow(true);
                let iPAddressValueTmpArray = '';
                if (key.prefixLength !== '' && key.prefixLength !== null) {
                    iPAddressValueTmpArray = key.IPAddress + '/' + key.PrefixLength;
                } else {
                    iPAddressValueTmpArray = key.IPAddress;
                }
                iPNameAddressValue.push({
                    IP: this.translate.instant('IBMC_IP_ADDRESS01') + j,
                    IPAddressValue: iPAddressValueTmpArray
                });
                j++;
            }
        }
        // 链路本地地址
        let addressValueV6 = '';
        if (localprefix !== '' && localprefix !== null) {
            addressValueV6 = localLink + '/' + localprefix;
        } else {
            addressValueV6 = localLink;
        }
        networkProtocol.setNetModelV6(networkeData.NetworkProtocols.IPv6Config.IPv6Addresses[0].IPAddressMode);
        networkProtocol.setIpAddressV6(ipAddressValue);
        networkProtocol.setMaskCodeV6(prefixLength);
        networkProtocol.setGatewayV6(networkeData.NetworkProtocols.IPv6Config.Gateway);
        networkProtocol.setAddressValueV6(addressValueV6);
        networkProtocol.setIPAddresslabel(iPNameAddressValue);
    }

    // lldp
    public lldpDataDetails = (networkLldp, networkeData) => {
        networkLldp.setLldpEnabled(networkeData?.LLDP?.Enabled);
        networkLldp.setLldpWorkMode(networkeData?.LLDP?.WorkMode);
        networkLldp.setTxDelaySeconds(networkeData?.LLDP?.TxDelaySeconds);
        networkLldp.setTxIntervalSeconds(networkeData?.LLDP?.TxIntervalSeconds);
        networkLldp.setTxHold(networkeData?.LLDP?.TxHold);
    }

    // port
    public portDetails = (networkPort, networkeData) => {
        // 主机名
        networkPort.setHostName(networkeData.HostName);
        // 网口模式是否显示
        if (networkeData.NetworkPort && networkeData.NetworkPort.Members) {
            networkPort.setProductType(true);
        } else {
            networkPort.setProductType(false);
            return;
        }

        const managerPort = networkeData.NetworkPort.Members;

        const tempDedicated = [];
        const tempAggregation = [];
        const tempExtend = [];
        const tempExtend2 = [];
        const tempPcie = [];
        const tempOcp = [];
        const tempOcp2 = [];

        const tempCheckDedicated = [];
        const tempCheckAggregation = [];
        const tempCheckExtend = [];
        const tempCheckExtend2 = [];
        const tempCheckPcie = [];
        const tempCheckOcp = [];
        const tempCheckOcp2 = [];

        let countDisabledDedicated = 0;
        let countDisabledAggregation = 0;
        let countDisabledExtend = 0;
        let countDisabledExtend2 = 0;
        let countDisabledPcie = 0;
        let countDisabledOcp = 0;
        let countDisabledOcp2 = 0;

        this.isIRM210 = CommonData.productType === PRODUCT.IRM;
        // 生成管理网口的数据和自适应网口数据，注意，后面还需要根据 Oem.Huawei.AdaptivePort 数组内的内容对checkbox数组做处理，确定是否选中状态
        // 专用网口
        // 如果有PortName属性,选项的label属性就需要用PortName进行展示
        if (managerPort.Dedicated[0] && managerPort.Dedicated[0].PortName) {
            const protDedicated = managerPort.Dedicated;
            const status = protDedicated.LinkStatus == null ? 'Disconnected' : protDedicated.LinkStatus;
            protDedicated?.forEach((item) => {
                const oRadio = {
                    key: item?.PortName ? `${item.PortName}` : `eth${item.PortNumber}`,
                    Type: 'Dedicated',
                    id: item.PortNumber,
                    status: item.LinkStatus == null ? 'Disconnected' : item.LinkStatus,
                    disable: status === 'Disabled'
                };
                const oCheckbox = {
                    title: item?.PortName ? `${item.PortName}` : `eth${item.PortNumber}`,
                    checked: item.AdaptiveFlag,
                    Type: 'Dedicated',
                    id: item.PortNumber,
                    status: item.LinkStatus == null ? 'Disconnected' : item.LinkStatus,
                    disable: status === 'Disabled'
                };
                tempDedicated.push(oRadio);
                tempCheckDedicated.push(oCheckbox);
                if (status === 'Disabled') {
                    countDisabledDedicated++;
                }
            });
        } else if (managerPort.Dedicated.PortName) {
            const protDedicated = managerPort.Dedicated;
            const status = protDedicated.LinkStatus == null ? 'Disconnected' : protDedicated.LinkStatus;
            const oRadio = {
                key: protDedicated.PortName,
                Type: 'Dedicated',
                id: protDedicated.PortNumber,
                status: protDedicated.LinkStatus == null ? 'Disconnected' : protDedicated.LinkStatus,
                disable: status === 'Disabled'
            };
            const oCheckbox = {
                title: protDedicated.PortName,
                checked: protDedicated.AdaptiveFlag,
                Type: 'Dedicated',
                id: protDedicated.PortNumber,
                status: protDedicated.LinkStatus == null ? 'Disconnected' : protDedicated.LinkStatus,
                disable: status === 'Disabled'
            };
            tempDedicated.push(oRadio);
            tempCheckDedicated.push(oCheckbox);
            if (status === 'Disabled') {
                countDisabledDedicated++;
            }
        } else if (this.isIRM210 && managerPort.Dedicated.length > 0) {
            managerPort.Dedicated.forEach((item) => {
                const status = item.LinkStatus === null ? 'Disconnected' : item.LinkStatus;
                const oRadio = {
                    key: 'Port' + item.PortNumber,
                    Type: 'Dedicated',
                    id: parseInt(item.PortNumber, 10),
                    status: item.LinkStatus === null ? 'Disconnected' : item.LinkStatus,
                    disable: status === 'Disabled'
                };
                const oCheckbox = {
                    title: 'Port' + item.PortNumber,
                    checked: item.AdaptiveFlag,
                    Type: 'Dedicated',
                    id: parseInt(item.PortNumber, 10),
                    status: item.LinkStatus === null ? 'Disconnected' : item.LinkStatus,
                    disable: status === 'Disabled'
                };
                tempDedicated.push(oRadio);
                tempCheckDedicated.push(oCheckbox);
                if (status === 'Disabled') {
                    countDisabledDedicated++;
                }
            });
        } else if (
            managerPort.Dedicated &&
            JSON.stringify(managerPort.Dedicated) !== '{}' &&
            !(managerPort.Dedicated instanceof Array)
        ) {
            const item = managerPort.Dedicated[0] ? managerPort.Dedicated[0] : managerPort.Dedicated;
            const status = item.LinkStatus == null ? 'Disconnected' : item.LinkStatus;
            const oRadio = {
                key: 'Mgmt',
                Type: 'Dedicated',
                id: item.PortNumber,
                status: item.LinkStatus == null ? 'Disconnected' : item.LinkStatus,
                disable: status === 'Disabled'
            };
            const oCheckbox = {
                title: 'Mgmt',
                checked: item.AdaptiveFlag === true ? true : false,
                Type: 'Dedicated',
                id: item.PortNumber,
                status: item.LinkStatus == null ? 'Disconnected' : item.LinkStatus,
                disable: status === 'Disabled'
            };
            tempDedicated.push(oRadio);
            tempCheckDedicated.push(oCheckbox);
            if (status === 'Disabled') {
                countDisabledDedicated++;
            }
        } else if (managerPort.Dedicated instanceof Array) {
            const itemList = managerPort.Dedicated;
            itemList.forEach(item => {
                const status = item.LinkStatus == null ? 'Disconnected' : item.LinkStatus;
                const oRadio = {
                    key: 'Port' + item.PortNumber,
                    Type: 'Dedicated',
                    id: parseInt(item.PortNumber, 10),
                    status: item.LinkStatus == null ? 'Disconnected' : item.LinkStatus,
                    disable: status === 'Disabled'
                };
                const oCheckbox = {
                    title: 'Port' + item.PortNumber,
                    checked: item.AdaptiveFlag === true ? true : false,
                    Type: 'Dedicated',
                    id: parseInt(item.PortNumber, 10),
                    status: item.LinkStatus == null ? 'Disconnected' : item.LinkStatus,
                    disable: status === 'Disabled'
                };
                tempDedicated.push(oRadio);
                tempCheckDedicated.push(oCheckbox);
                if (status === 'Disabled') {
                    countDisabledDedicated++;
                }
            });
        }
        // 汇聚网口
        if (JSON.stringify(managerPort.Aggregation) !== '{}') {
            for (const keyPort of managerPort.Aggregation) {
                const item = keyPort;
                const status = item.LinkStatus == null ? 'Disconnected' : item.LinkStatus;
                const oRadio = {
                    key: 'Port' + item.PortNumber,
                    Type: 'Aggregation',
                    id: item.PortNumber,
                    status: item.LinkStatus == null ? 'Disconnected' : item.LinkStatus,
                    disable: status === 'Disabled'
                };
                const oCheckbox = {
                    title: 'Port' + item.PortNumber,
                    checked: item.AdaptiveFlag,
                    Type: 'Aggregation',
                    id: item.PortNumber,
                    status: item.LinkStatus == null ? 'Disconnected' : item.LinkStatus,
                    disable: status === 'Disabled'
                };
                if (this.isIRM210) {
                    oRadio.id = parseInt(oRadio.id, 10);
                    oCheckbox.id = parseInt(oCheckbox.id, 10);
                }
                tempAggregation.push(oRadio);
                tempCheckAggregation.push(oCheckbox);
                if (status === 'Disabled') {
                    countDisabledAggregation++;
                }
            }
        }
        // 板载网口
        if (JSON.stringify(managerPort.LOM) !== '{}') {
            for (const keyPort of managerPort.LOM) {
                const item = keyPort;
                const status = item.LinkStatus == null ? 'Disconnected' : item.LinkStatus;
                const oRadio = {
                    key: 'Port' + item.PortNumber,
                    Type: 'LOM',
                    id: item.PortNumber,
                    status: item.LinkStatus == null ? 'Disconnected' : item.LinkStatus,
                    disable: status === 'Disabled'
                };
                const oCheckbox = {
                    title: 'Port' + item.PortNumber,
                    checked: item.AdaptiveFlag,
                    Type: 'LOM',
                    id: item.PortNumber,
                    status: item.LinkStatus == null ? 'Disconnected' : item.LinkStatus,
                    disable: status === 'Disabled'
                };
                if (this.isIRM210) {
                    oRadio.id = parseInt(oRadio.id, 10);
                    oCheckbox.id = parseInt(oCheckbox.id, 10);
                }

                tempExtend.push(oRadio);
                tempCheckExtend.push(oCheckbox);
                if (status === 'Disabled') {
                    countDisabledExtend++;
                }
            }
        }
        // 板载网口2
        if (JSON.stringify(managerPort.FlexIO) !== '{}') {
            for (const keyPort of managerPort.FlexIO) {
                const item = keyPort;
                const status = item.LinkStatus == null ? 'Disconnected' : item.LinkStatus;
                const oRadio = {
                    key: 'Port' + item.PortNumber,
                    Type: 'FlexIO',
                    id: item.PortNumber,
                    status: item.LinkStatus == null ? 'Disconnected' : item.LinkStatus,
                    disable: status === 'Disabled'
                };
                const oCheckbox = {
                    title: 'Port' + item.PortNumber,
                    checked: item.AdaptiveFlag,
                    Type: 'FlexIO',
                    id: item.PortNumber,
                    status: item.LinkStatus == null ? 'Disconnected' : item.LinkStatus,
                    disable: status === 'Disabled'
                };
                if (this.isIRM210) {
                    oRadio.id = parseInt(oRadio.id, 10);
                    oCheckbox.id = parseInt(oCheckbox.id, 10);
                }
                tempExtend2.push(oRadio);
                tempCheckExtend2.push(oCheckbox);
                if (status === 'Disabled') {
                    countDisabledExtend2++;
                }
            }
        }
        // PCIE扩展网口
        if (JSON.stringify(managerPort.ExternalPCIe) !== '{}') {
            for (const keyPort of managerPort.ExternalPCIe) {
                const item = keyPort;
                const status = item.LinkStatus == null ? 'Disconnected' : item.LinkStatus;
                const oRadio = {
                    key: 'Port' + item.PortNumber,
                    Type: 'ExternalPCIe',
                    id: item.PortNumber,
                    status: item.LinkStatus == null ? 'Disconnected' : item.LinkStatus,
                    disable: status === 'Disabled'
                };
                const oCheckbox = {
                    title: 'Port' + item.PortNumber,
                    checked: item.AdaptiveFlag,
                    Type: 'ExternalPCIe',
                    id: item.PortNumber,
                    status: item.LinkStatus == null ? 'Disconnected' : item.LinkStatus,
                    disable: status === 'Disabled'
                };
                if (this.isIRM210) {
                    oRadio.id = parseInt(oRadio.id, 10);
                    oCheckbox.id = parseInt(oCheckbox.id, 10);
                }
                tempPcie.push(oRadio);
                tempCheckPcie.push(oCheckbox);
                if (status === 'Disabled') {
                    countDisabledPcie++;
                }
            }
        }
        // OCP扩展网口
        if (JSON.stringify(managerPort.OCP) !== '{}') {
            for (const keyPort of managerPort.OCP) {
                const item = keyPort;
                const status = item.LinkStatus == null ? 'Disconnected' : item.LinkStatus;
                const oRadio = {
                    key: 'Port' + item.PortNumber,
                    Type: 'OCP',
                    id: item.PortNumber,
                    status: item.LinkStatus == null ? 'Disconnected' : item.LinkStatus,
                    disable: status === 'Disabled'
                };
                const oCheckbox = {
                    title: 'Port' + item.PortNumber,
                    checked: item.AdaptiveFlag,
                    Type: 'OCP',
                    id: item.PortNumber,
                    status: item.LinkStatus == null ? 'Disconnected' : item.LinkStatus,
                    disable: status === 'Disabled'
                };
                tempOcp.push(oRadio);
                tempCheckOcp.push(oCheckbox);
                if (status === 'Disabled') {
                    countDisabledOcp++;
                }
            }
        }
        // OCP2扩展网口
        if (managerPort.OCP2 && JSON.stringify(managerPort.OCP2) !== '{}') {
            for (const keyPort of managerPort.OCP2) {
                const item = keyPort;
                const status = item.LinkStatus === null ? 'Disconnected' : item.LinkStatus;
                const oRadio = {
                    key: 'Port' + item.PortNumber,
                    Type: 'OCP2',
                    id: item.PortNumber,
                    status: item.LinkStatus === null ? 'Disconnected' : item.LinkStatus,
                    disable: status === 'Disabled'
                };
                const oCheckbox = {
                    title: 'Port' + item.PortNumber,
                    checked: item.AdaptiveFlag,
                    Type: 'OCP2',
                    id: item.PortNumber,
                    status: item.LinkStatus == null ? 'Disconnected' : item.LinkStatus,
                    disable: status === 'Disabled'
                };
                tempOcp2.push(oRadio);
                tempCheckOcp2.push(oCheckbox);
                if (status === 'Disabled') {
                    countDisabledOcp2++;
                }
            }
        }

        networkPort.setSchemaRadio(networkeData.NetworkPort.Mode);
        networkPort.setNcsiRadio(networkeData.NetworkPort.NcsiMode);
        networkPort.setNetworkPortType(networkeData.NetworkPort.ManagementNetworkPort);

        networkPort.setTempDedicated(tempDedicated);
        networkPort.setCountDedicated(countDisabledDedicated);
        networkPort.setTempCheckDedicated(tempCheckDedicated);

        networkPort.setTempAggregation(tempAggregation);
        networkPort.setCountAggregation(countDisabledAggregation);
        networkPort.setTempCheckAggregation(tempCheckAggregation);

        networkPort.setTempExtend(tempExtend);
        networkPort.setCountExtend(countDisabledExtend);
        networkPort.setTempCheckExtend(tempCheckExtend);

        networkPort.setTempExtend2(tempExtend2);
        networkPort.setCountExtend2(countDisabledExtend2);
        networkPort.setTempCheckExtend2(tempCheckExtend2);

        networkPort.setTempPcie(tempPcie);
        networkPort.setCountPcie(countDisabledPcie);
        networkPort.setTempCheckPcie(tempCheckPcie);

        networkPort.setTempOcp(tempOcp);
        networkPort.setCountOcp(countDisabledOcp);
        networkPort.setTempCheckOcp(tempCheckOcp);

        networkPort.setTempOcp2(tempOcp2);
        networkPort.setCountOcp2(countDisabledOcp2);
        networkPort.setTempCheckOcp2(tempCheckOcp2);
    }

    // DNS
    public networkDnsDetails = (networkDns, networkeData) => {
        networkDns.setIpVersion(networkeData.NetworkProtocols.ProtocolMode);
        networkDns.setDnsAddress(networkeData.DNS.AddressMode);
        networkDns.setDnsDomain(networkeData.DNS.Domain);
        networkDns.setDnsServer(networkeData.DNS.PrimaryServer);
        networkDns.setDnsServer1(networkeData.DNS.BackupServer);
        networkDns.setDnsServer2(networkeData.DNS.TertiaryServer);
        networkDns.setNetModelV4(networkeData.NetworkProtocols.IPv4Config.IPAddressMode);
        networkDns.setNetModelV6(networkeData.NetworkProtocols.IPv6Config.IPv6Addresses[0].IPAddressMode);
    }

    // vlan
    public networkVlanDetails = (networkVlan, networkeData) => {
        networkVlan.setVlanEnable(networkeData.VLAN ? networkeData.VLAN.Enabled : '');
        networkVlan.setVlanId(networkeData.VLAN ? networkeData.VLAN.ID : '');
        networkVlan.setProductTypeVlan(true);
        if (networkeData.VLAN === null || networkeData.VLAN === undefined) {
            networkVlan.setProductTypeVlan(false);
        }
    }
}
