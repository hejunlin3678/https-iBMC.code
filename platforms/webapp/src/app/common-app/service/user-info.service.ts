import { Injectable } from '@angular/core';

@Injectable({
    providedIn: 'root'
})
export class UserInfoService {
    public userId: string = '';
    public userName: string = '';
    public userRole: string[] = [];
    public userIp: string;
    public token: string = '';
    public resourceId: string = '';
    public isLogin: boolean = false;
    public currentLoginIp: string = '';
    public lastLoginIp: string = '';
    public lastLoginTime: string = '';
    public loginTime: string;
    public managersId: string = '';
    public systemLockDownEnabled: boolean = false;
    public systemsId: string = '';
    public chassisId: string = '';
    public platform: string = '';
    // 是否开启了移动定制化开关
    public customizedId: number = 0;
    // 是否免密登录
    public loginWithoutPassword: boolean = false;
    // 是否是双因素登录
    public isTwoFacCertification: boolean = false;
    // 用户权限
    public privileges: string[] = [];
    // 标志是否使用退出按钮退出
    public isExited: boolean = false;

    public ibmaSupport: boolean | null = null;
    public licenseSupport: boolean | null = null;
    public partitySupport: boolean | null = null;
    public fdmSupport: boolean | null = null;
    public fanSupport: boolean | null = null;
    public tceSupport = null;
    public snmpSupport: boolean | null = null;
    public spSupport: boolean | null = null;
    public tpcmSupport: boolean | null = null;
    public usbSupport: boolean | null = null;
    public iBMARunning: boolean = false;
    public modifyVlanIdUnsupported: boolean | null = null;
    public kvmSupport: boolean | null = null;
    public vncSupport: boolean | null = null;
    public vmmSupport: boolean | null = null;
    public perfSupSupport: boolean | null = null;
    public videoScreenshotSupport: boolean | null = null;

    // 是否是自定义证书标志，默认值为true
    public customeCertFlag: boolean = true;

    public reset(): void {
        this.userId = '';
        this.userName = '';
        this.userRole = [];
        this.userIp = '';
        this.token = '';
        this.resourceId = '';
        this.isLogin = false;
        this.currentLoginIp = '';
        this.lastLoginIp = '';
        this.lastLoginTime = '';
        this.loginTime = '';
        this.managersId = '';
        this.systemLockDownEnabled = false;
        this.systemsId = '';
        this.chassisId = '';
        this.loginWithoutPassword = false;
        this.isTwoFacCertification = false;
        this.privileges = [];
        this.platform = '';
        this.ibmaSupport = null;
        this.licenseSupport = null;
        this.partitySupport = null;
        this.fdmSupport = null;
        this.fanSupport = null;
        this.snmpSupport = null;
        this.tceSupport = null;
        this.spSupport = null;
        this.tpcmSupport = null;
        this.usbSupport = null;
        this.iBMARunning = false;
        this.customizedId = 0;
        this.customeCertFlag = true;
        this.modifyVlanIdUnsupported = null;
        this.kvmSupport = null;
        this.vncSupport = null;
        this.vmmSupport = null;
        this.perfSupSupport = null;
        this.videoScreenshotSupport = null;
    }

    public get(attr) {
        if (attr) {
            return this[attr];
        }
    }

    public set(attr, value) {
        this[attr] = value;
    }

    // 查询是否具有指定的权限组合
    public hasPrivileges(roleArr: string[]) {
        if (this.privileges.length === 0) {
            return false;
        }

        const tempArr = this.privileges.filter((role) => {
            return roleArr.indexOf(role) > -1;
        });

        return tempArr.length === roleArr.length;
    }

    getToken() {
        return this.token;
    }
    getManagersId() {
        return this.managersId;
    }

    public updateStorageUserInfo() {

    }

    constructor() { }
}
