import { pluck } from 'rxjs/internal/operators/pluck';
import { map } from 'rxjs/internal/operators/map';
import { Injectable } from '@angular/core';
import { GlobalDataService, HttpService } from 'src/app/common-app/service';
import { TranslateService } from '@ngx-translate/core';
import { Observable } from 'rxjs/internal/Observable';
import { NetworkProtocol, NetworkDns, NetworkLldp } from '../model';
import { UserInfoService } from 'src/app/common-app/service';
import { forkJoin } from 'rxjs';
@Injectable({
    providedIn: 'root'
})
export class NetworkService {

    constructor(
        private userInfo: UserInfoService,
        private http: HttpService,
        private translate: TranslateService,
        private global: GlobalDataService
    ) { }
    public i18n: any = {
        IBMC_IPV6_ADDRES_ERROR_1: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_1'),
        IBMC_IPV6_ADDRES_ERROR_2: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_2'),
        IBMC_IPV6_ADDRES_ERROR_3: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_3'),
        IBMC_IPV6_ADDRES_ERROR_4: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4'),
        IBMC_IPV6_ADDRES_ERROR_5: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_5'),
        IBMC_IPV6_ADDRES_ERROR_6: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_6'),
        IBMC_IPV6_ADDRES_ERROR_7: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_7'),
        IBMC_IPV6_ADDRES_ERROR_8: this.translate.instant('IBMC_IP_CONFLICT'),
        IBMC_IPV6_ADDRES_ERROR_9: this.translate.instant('IBMC_IPV6_ADDRES_ERROR_4')
    };
    public header = {
        headers: {
            token: this.userInfo.token
        }
    };
    public LABEL_ID = 2;
    public getHMM() {
        return this.http.get('/redfish/v1/Managers/HMM1');
    }
    public getHostName(HMMID) {
        return this.http.get(`/redfish/v1/Managers/${HMMID}/EthernetInterfaces/StaticIp`);
    }

    public saveHost(HMMID, params) {
        return this.http.patch(`/redfish/v1/Managers/${HMMID}/EthernetInterfaces/StaticIp`, params, this.header);
    }

    // 保存网络配置
    public saveNetWork(params) {
        return this.http.patch('/redfish/v1/chassis/ipconfig', params, this.header);
    }

    public getLldp(HMMID) {
        return this.http.get(`/redfish/v1/Managers/${HMMID}/LldpService`).pipe(
            map((res) => {
                const data = res.body;
                const networkLldp = new NetworkLldp();
                const lldpShowData = data;
                networkLldp.setLldpSupport(data ? true : false);
                if (lldpShowData) {
                    this.lldpDataDetails(networkLldp, data);
                }
                return networkLldp;
            })
        );
    }
    public saveLldp(HMMID, params) {
        return this.http.patch(`/redfish/v1/Managers/${HMMID}/LldpService`, params, this.header);
    }

    public getDns(HMMID) {
        return this.http.get(`/redfish/v1/Managers/${HMMID}/EthernetInterfaces/StaticIp`).pipe(
            map((res) => {
                const data = res?.body;
                const networkDns = new NetworkDns();
                this.networkDnsDetails(networkDns, data);
                return networkDns;
            })
        );
    }
    public networkList = () => {
        return new Observable((observer) => {
            this.http.get('/redfish/v1/chassis/ipconfig').subscribe((response) => {
                const networkeData = response.body.IPinfo;
                const networkProtocol = new NetworkProtocol();
                this.networkProtocolDetails(networkProtocol, networkeData);
                let subBoardId = 'HMM2';
                if (this.global.managerId !== 'HMM1') {
                    subBoardId = 'HMM1';
                }
                forkJoin([
                    this.getHostName(this.global.managerId),
                    this.getHostName(subBoardId)
                ]).subscribe({
                    next: ([res, subRes]) => {
                        this.setIpAdress(res, networkProtocol);
                        let subLinkLocal: string;
                        for (const item of subRes?.body?.IPv6Addresses) {
                            // 获取备板链路本地地址
                            if (item.AddressOrigin === 'LinkLocal') {
                                subLinkLocal = `${item?.Address}/${item?.PrefixLength}`;
                            }
                        }
                        networkProtocol.setAddressValueV6Standby(subLinkLocal);
                        observer.next([networkProtocol]);
                        },
                    error: (error) => {
                        this.getHostName(this.global.managerId).subscribe(res => {
                            this.setIpAdress(res, networkProtocol);
                            observer.next([networkProtocol]);
                        });
                    }
                });
            });
        });
    }
    // 设置主板ip相关数据
    public setIpAdress(res, networkProtocol) {
        const iPv6Addresses = res?.body?.IPv6Addresses;
        let linkLocal: string;
        let j = this.LABEL_ID;
        const iPNameAddressValue = [];
        for (const item of iPv6Addresses) {
            // 链路本地地址
            if (item.AddressOrigin === 'LinkLocal') {
                linkLocal = `${item?.Address}/${item?.PrefixLength}`;
            }
            // IP地址段
            if (item.AddressOrigin === 'SLAAC') {
                // 是否显示
                networkProtocol.setIpAddressShow(true);
                let iPAddressValueTmpArray = '';
                if (item?.PrefixLength) {
                    iPAddressValueTmpArray = item.Address + '/' + item.PrefixLength;
                } else {
                    iPAddressValueTmpArray = item.Address;
                }
                // 多个ip地址段添加编号j
                iPNameAddressValue.push({
                    IP: this.translate.instant('IBMC_IP_ADDRESS01') + j,
                    IPAddressValue: iPAddressValueTmpArray
                });
                j++;
            }
        }
        networkProtocol.setAddressValueV6(linkLocal);
        networkProtocol.setIPAddresslabel(iPNameAddressValue);
    }

    public networkProtocolDetails = (networkProtocol, networkeData) => {
        // 备板为null时表示备板不在位，页面隐藏备用ip
        if (networkeData?.IPv6?.Standby === null || networkeData?.IPv4?.Standby === null) {
            networkProtocol.setStandbyStatus(false);
        } else {
            networkProtocol.setStandbyStatus(true);
        }
        if (!Object.prototype.hasOwnProperty.call(networkeData, 'IPv6')) {
            networkeData['IPv6'] = {
                'Active': [
                    {
                        'Address': '',
                        'PrefixLength': '',
                        'AddressOrigin': 'Static'
                    }
                ],
                'Float': [
                    {
                        'Address': '',
                        'PrefixLength': '',
                        'AddressOrigin': ''
                    }
                ],
                'DefaultGateway': '',
                'Standby': [
                    {
                        'Address': '',
                        'PrefixLength': '',
                        'AddressOrigin': ''
                    }
                ]
            };
        }
        if (!Object.prototype.hasOwnProperty.call(networkeData, 'IPv4')) {
            networkeData['IPv4'] = {
                'Active': [
                    {
                        'Address': '',
                        'PrefixLength': '',
                        'AddressOrigin': 'Static'
                    }
                ],
                'Float': [
                    {
                        'Address': '',
                        'PrefixLength': '',
                        'AddressOrigin': ''
                    }
                ],
                'DefaultGateway': '',
                'Standby': [
                    {
                        'Address': '',
                        'PrefixLength': '',
                        'AddressOrigin': ''
                    }
                ]
            };
        }
        // 网络协议
        networkProtocol.setNetIPVersion(networkeData?.IPVersion);
        networkProtocol.setNetModelV4(networkeData?.IPv4?.Active[0]?.AddressOrigin);
        networkProtocol.setIpAddressV4(networkeData?.IPv4?.Active[0]?.Address);
        networkProtocol.setMaskCodeV4(networkeData?.IPv4?.Active[0]?.SubnetMask);
        networkProtocol.setGatewayV4(networkeData?.IPv4?.DefaultGateway);
        networkProtocol.setAddMacV4(networkeData?.IPv4?.Active[0]?.PermanentMACAddress);
        const localprefix = networkeData?.IPv6?.Active[0]?.PrefixLength;
        const localLink = networkeData?.IPv6?.Active[0]?.Address;
        // 链路本地地址
        let addressValueV6 = '';
        if (localprefix !== '' && localprefix !== null) {
            addressValueV6 = localLink + '/' + localprefix;
        } else {
            addressValueV6 = localLink;
        }
        networkProtocol.setNetModelV6(networkeData?.IPv6?.Active[0]?.AddressOrigin);
        networkProtocol.setIpAddressV6(networkeData?.IPv6?.Active[0]?.Address);
        networkProtocol.setMaskCodeV6(networkeData?.IPv6?.Active[0]?.PrefixLength);
        networkProtocol.setGatewayV6(networkeData?.IPv6?.DefaultGateway);
        if (networkeData?.IPv4?.Standby) {
            networkProtocol.setIpAddressV4Standby(networkeData?.IPv4?.Standby[0]?.Address);
            networkProtocol.setMaskCodeV4Standby(networkeData?.IPv4?.Standby[0]?.SubnetMask);
        } else {
            networkProtocol.setIpAddressV4Standby('');
            networkProtocol.setMaskCodeV4Standby('');
        }
        if (networkeData?.IPv6?.Standby) {
            networkProtocol.setIpAddressV6Standby(networkeData?.IPv6?.Standby[0]?.Address);
            networkProtocol.setPrefixV6Standby(networkeData?.IPv6?.Standby[0]?.PrefixLength);
        } else {
            networkProtocol.setIpAddressV6Standby('');
            networkProtocol.setPrefixV6Standby('');
        }
        if (networkeData?.IPv4?.Float) {
            networkProtocol.setIpAddressV4Float(networkeData?.IPv4?.Float[0]?.Address);
            networkProtocol.setMaskCodeV4Float(networkeData?.IPv4?.Float[0]?.SubnetMask);
        } else {
            networkProtocol.setIpAddressV4Float('');
            networkProtocol.setMaskCodeV4Float('');
        }
        if (networkeData?.IPv6?.Float) {
            networkProtocol.setIpAddressV6Float(networkeData?.IPv6?.Float[0]?.Address);
            networkProtocol.setPrefixV6Float(networkeData?.IPv6?.Float[0]?.PrefixLength);
        } else {
            networkProtocol.setIpAddressV6Float('');
            networkProtocol.setPrefixV6Float('');
        }
    }
    // DNS
    public networkDnsDetails = (networkDns, networkeData) => {
        networkDns.setIpVersion(networkeData?.Oem?.Huawei?.DNSAddressOrigin);
        networkDns.setDnsAddress(networkeData?.Oem?.Huawei?.DNSAddressOrigin);
        networkDns.setDnsDomain(networkeData?.FQDN);
        networkDns.setDnsServer(networkeData?.NameServers[0]);
        networkDns.setDnsServer1(networkeData?.NameServers[1]);
        networkDns.setDnsServer2(networkeData?.NameServers[2]);
        networkDns.setNetModelV4(networkeData?.NetworkProtocols?.IPv4Config?.IPAddressMode);
        networkDns.setNetModelV6(networkeData?.NetworkProtocols?.IPv6Config?.IPv6Addresses[0]?.IPAddressMode);
    }

    // lldp
    public lldpDataDetails = (networkLldp, networkeData) => {
        networkLldp.setLldpEnabled(networkeData.LldpEnabled);
        networkLldp.setLldpWorkMode(networkeData.WorkMode);
        networkLldp.setTxDelaySeconds(networkeData.TxDelaySeconds);
        networkLldp.setTxIntervalSeconds(networkeData.TxIntervalSeconds);
        networkLldp.setTxHold(networkeData.TxHold);
    }

}
