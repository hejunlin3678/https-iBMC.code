import { NetIPv46 } from '../net/net-ipv46';

/**
 * 网络属性
 */
export class NetworkAttr {
    public name: [string, string];
    public firmwarePackageVersion: [string, string];
    public driverName: [string, string];
    public driverVersion: [string, string];
    public speedMbps: [string, string];
    public autoNeg: [string, string];
    public fullDuplex: [string, string];
    public bDF: [string, string];
    public mACAddress: [string, string];
    public permanentMACAddress: [string, string];
    public linkSpeedGbps?: [string, string];
    public wWPN?: [string, string];
    public wWNN?: [string, string];
    public workMode?: [string, string];
    public peerDeviceCredit?: [string, string];
    public localDeviceCredit?: [string, string];
    public tXRateGbps?: [string, string];
    public rXRateGbps?: [string, string];
    public rateNegotiationStage?: [string, string];
    public portStatus?: [string, string];
    public workingRate?: [string, string];
    public vlan?: [string, string];
    public netIPv46: NetIPv46;
    constructor(
        name: string,
        firmwarePackageVersion: string,
        driverName: string,
        driverVersion: string,
        speedMbps: string,
        autoNeg: string,
        fullDuplex: string,
        bDF: string,
        mACAddress: string,
        permanentMACAddress: string,
        netIPv46: NetIPv46,
        linkSpeedGbps?: string,
        wWPN?: string,
        wWNN?: string,
        workMode?: string,
        peerDeviceCredit?: string,
        localDeviceCredit?: string,
        tXRateGbps?: string,
        rXRateGbps?: string,
        rateNegotiationStage?: string,
        portStatus?: string,
        workingRate?: string,
        vlan?: string
    ) {
        this.name = ['IBMC_NETWORK_PORT_NAME', name];
        this.firmwarePackageVersion = ['COMMON_FIXED_VERSION', firmwarePackageVersion];
        this.driverName = ['NET_DRIVER_NAME', driverName];
        this.driverVersion = ['NET_DRIVER_VERSION', driverVersion];
        this.speedMbps = ['NET_SPEED_RATE', speedMbps];
        this.autoNeg = ['NET_AUTOMATIC_NEGOTIATION', autoNeg];
        this.fullDuplex = ['NET_DUPLEX_FLAG_FROM_MCTP', fullDuplex];
        this.bDF = ['BUS_INFO_BDF', bDF];
        this.mACAddress = ['NET_MAC_ADDRESS', mACAddress];
        this.permanentMACAddress = ['OTHER_PERMANENT_MACADDR', permanentMACAddress];
        this.vlan = ['NET_VLANS', vlan];

        this.linkSpeedGbps = ['NET_SPEED_RATE', linkSpeedGbps];
        this.wWPN = ['WWPN', wWPN];
        this.wWNN = ['WWNN', wWNN];
        this.workMode = ['COMMON_WORKING_MODE', workMode];
        this.peerDeviceCredit = ['NET_RIGHT_CERITICAL', peerDeviceCredit];
        this.localDeviceCredit = ['NET_LOCAL_CERITICAL', localDeviceCredit];
        this.tXRateGbps = ['NET_SEND_RATE', tXRateGbps];
        this.rXRateGbps = ['NET_RECIVE_RATE', rXRateGbps];
        this.rateNegotiationStage = ['NET_RATE_LEVEL', rateNegotiationStage];
        this.portStatus = ['PORT_STATUS', portStatus];
        this.workingRate = ['NET_WORK_RATE', workingRate];
        this.netIPv46 = netIPv46;
    }
}

