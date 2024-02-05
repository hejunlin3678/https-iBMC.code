import { UntypedFormGroup } from '@angular/forms';

export interface IIpv4Group {
    ipAddress: string;
    gateWay: string;
    subnetMask: string;
}

export interface IIPV4Table {
    slotNum: string;
    linkUrl: string;
    url: string;
    ipControlName: string;
    maskControlName: string;
    gateControlName: string;
    dhcpControlName: string;
    formGroup: UntypedFormGroup;
}

export interface IIPV6Table {
    slotNum: string;
    linkUrl: string;
    slaacQuantity: SLACCIPv6Addresses[];
    url: string;
    ipControlName: string;
    gateControlName: string;
    prefixControlName: string;
    dhcpControlName: string;
    formGroup: UntypedFormGroup;
}

// IPV4批量设置传出数据类型
export interface IIPV4Batch {
    ipAddress: string;
    subnetMask: string;
    gateWay: string;
}

// SLACCIPv6Addresses数据类型
export interface SLACCIPv6Addresses {
    Id: number;
    Address: string;
    PrefixLength: number;
}
// IPV6批量设置传出数据类型
export interface IIPV6Batch {
    ipAddress: string;
    prefix: string;
    gateWay: string;
}
