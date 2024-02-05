import { Injectable, EventEmitter } from '@angular/core';
import { Subject } from 'rxjs';

@Injectable({
    providedIn: 'root'
})

export class GlobalDataService {
    constructor() {
        this.ibmaSupport = new Promise((resolve, reject) => {
            this.ibmaResolve = resolve;
        });
        this.ibmaRunning = new Promise((resolve, reject) => {
            this.ibmaRunningResolve = resolve;
        });
        this.partitySupport = new Promise((resolve, reject) => {
            this.partityResolve = resolve;
        });
        this.licenseSupport = new Promise((resolve, reject) => {
            this.licenseResolve = resolve;
        });
        this.fdmSupport = new Promise((resolve, reject) => {
            this.fdmResolve = resolve;
        });
        this.fanSupport = new Promise((resolve, reject) => {
            this.fanResolve = resolve;
        });
        this.snmpSupport = new Promise((resolve, reject) => {
            this.snmpResolve = resolve;
        });
        this.spSupport = new Promise((resolve, reject) => {
            this.spResolve = resolve;
        });
        this.tpcmSupport = new Promise((resolve, reject) => {
            this.tpcmResolve = resolve;
        });
        this.usbSupport = new Promise((resolve, reject) => {
            this.usbResolve = resolve;
        });
        this.kvmSupport = new Promise((resolve, reject) => {
            this.kvmResolve = resolve;
        });
        this.vncSupport = new Promise((resolve, reject) => {
            this.vncResolve = resolve;
        });
        this.vmmSupport = new Promise((resolve, reject) => {
            this.vmmResolve = resolve;
        });
        this.perfSupSupport = new Promise((resolve, reject) => {
            this.perfSupResolve = resolve;
        });
        this.videoScreenshotSupport = new Promise((resolve, reject) => {
            this.videoScreenshotResolve = resolve;
        });
        this.productType = null;
	this.powerCappingSupport = new Promise((resolve, reject) => {
            this.powerCappingResolve = resolve;
        });
        this.powerSleepSupport = new Promise((resolve, reject) => {
            this.powerSleepResolve = resolve;
        });
        this.swiSupport = new Promise((resolve, reject) => {
            this.swiSupportResolve = resolve;
        });
    }

    /**
     * webType 1:ibmc ,2:emm , 3:irm
     * 这里做成Promise对象主要是由于Angular内没有同步阻塞整个浏览器进程的方法，导致代码流程不能完全阻止
     * 后续的v1请求返回数据的时间差可能大于或小于下一个使用 webType的组件初始化时间。
     */
    public webType: Promise<number>;
    public ibmaSupport: Promise<boolean>;
    public ibmaResolve: (resolve) => void;
    public ibmaRunning: Promise<boolean>;
    public ibmaRunningResolve: (resolve) => void;
    public partitySupport: Promise<boolean>;
    public partityResolve: (resolve) => void;
    public licenseSupport: Promise<boolean>;
    public licenseResolve: (resolve) => void;
    public fdmSupport: Promise<boolean>;
    public fdmResolve: (resolve) => void;
    public fanSupport: Promise<boolean>;
    public fanResolve: (resolve) => void;
    public tceResolve: (resolve) => void;
    public snmpSupport: Promise<boolean>;
    public snmpResolve: (resolve) => void;
    public spSupport: Promise<boolean>;
    public spResolve: (resolve) => void;
    public tpcmSupport: Promise<boolean>;
    public tpcmResolve: (resolve) => void;
    public usbSupport: Promise<boolean>;
    public usbResolve: (resolve) => void;
    public powerCappingSupport: Promise<boolean>;
    public powerCappingResolve: (resolve) => void;
    public powerSleepSupport: Promise<boolean>;
    public powerSleepResolve: (resolve) => void;
    public swiSupport: Promise<boolean>;
    public swiSupportResolve: (resolve) => void;
    public kvmSupport: Promise<boolean>;
    public kvmResolve: (resolve) => void;
    public vncSupport: Promise<boolean>;
    public vncResolve: (resolve) => void;
    public vmmSupport: Promise<boolean>;
    public vmmResolve: (resolve) => void;
    public perfSupSupport: Promise<boolean>;
    public perfSupResolve: (resolve) => void;
    public videoScreenshotSupport: Promise<boolean>;
    public videoScreenshotResolve: (resolve) => void;
    public productType: string = '';
    public productName: string = '';
    public fqdn: string = '';
    public langSet: string = '';
    public copyright: string = '';
    public pwdCheckedState: false;
    public downloadKVMLink: string = '';
    public softwareName: string = '';

    // ibmc时间和头部导航的告警信息
    public ibmcTime: Subject<any> = new Subject();
    public resendOverview: Subject<any> = new Subject();
    public keepAliveInterval: number = 0;
    public failedCount: number = 0;
    public sessionTimeOut: boolean = false;

    public routeChagneError: Subject<any> = new Subject();
    public systemLockState: Subject<boolean> = new Subject();
    public appDetectChagne: Subject<any> = new Subject();
    public closePresetCert: Subject<any> = new Subject();
    public managerId: string;

    public reset() {
        this.failedCount = 0;
    }
}
