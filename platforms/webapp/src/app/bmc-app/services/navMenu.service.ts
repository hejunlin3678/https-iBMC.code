import { Injectable } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { UserInfoService } from '../../common-app/service/user-info.service';
import { GlobalDataService } from '../../common-app/service/global-data.service';
import { BrowserType } from '../../common-app/utils/browser';
import { ISecondMenuItem, IMainMenuItem } from 'src/app/common-app/models';
import { SysId, MainId, UserId, ServiceId, ManageId } from '../models/common.datatype';
import { hideFansAndPower } from 'src/app/common-app/utils/fan.util';

@Injectable({
    providedIn: 'root'
})
export class NavMenuService {
    private isIE: boolean = new BrowserType().getBrowserType().browser === 'IE';
    constructor(
        private translate: TranslateService,
        private userInfo: UserInfoService,
        private globalData: GlobalDataService
    ) {
        this.refreshMenu();
    }

    // 系统管理二级菜单和左侧菜单
    private systemSecondMenu: ISecondMenuItem[] = [
        {
            id: SysId.Info,
            navId: 'secSysInfo',
            hide: false,
            lable: 'COMMON_SYSTEM_INFO',
            router: ['/navigate/system/info'],
            iconClass: 'icon-info',
            isIE: this.isIE
        },
        {
            id: SysId.Monitor,
            navId: 'secSysMonitor',
            hide: false,
            lable: 'COMMON_SYSTEM_PERFORM',
            router: ['/navigate/system/monitor'],
            iconClass: 'icon-monitor',
            isIE: this.isIE
        },
        {
            id: SysId.Storage,
            navId: 'secSysStorage',
            hide: false,
            lable: 'COMMON_SYSTEM_STORAGE',
            router: ['/navigate/system/storage'],
            iconClass: 'icon-store',
            isIE: this.isIE
        },
        {
            id: SysId.Power,
            navId: 'secSysPower',
            hide: false,
            lable: 'COMMON_SYSTEM_POWER',
            router: ['/navigate/system/power'],
            iconClass: 'icon-power',
            isIE: this.isIE
        },
        {
            id: SysId.Fans,
            navId: 'secSysFans',
            hide: true,
            lable: 'COMMON_SYSTEM_FANS',
            router: ['/navigate/system/fans'],
            iconClass: 'icon-fans',
            isIE: this.isIE
        },
        {
            id: SysId.Bios,
            navId: 'secSysBios',
            hide: false,
            lable: 'COMMON_SYSTEM_BIOS',
            router: ['/navigate/system/bios'],
            iconClass: 'icon-bios',
            isIE: this.isIE
        },
        {
            id: SysId.Partity,
            navId: 'secSysPartity',
            hide: true,
            lable: 'COMMON_SYSTEM_PARTITY',
            router: ['/navigate/system/partity'],
            iconClass: 'icon-partity',
            isIE: this.isIE
        }
    ];

    // 维护诊断二级菜单和左侧菜单
    private maintanceSecondMenu: ISecondMenuItem[] = [
        {
            lable: this.translate.instant('ALARM_EVENT'),
            router: ['/navigate/maintance/alarm'],
            iconClass: 'icon-alarm',
            id: MainId.Event,
            navId: 'secMainEvent',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('ALARM_REPORT'),
            router: ['/navigate/maintance/report/'],
            iconClass: 'icon-report',
            id: MainId.Report,
            navId: 'secMainReport',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('COMMON_MAINTANCE_FDM'),
            router: ['/navigate/maintance/fdm'],
            iconClass: 'icon-fdm',
            id: MainId.Fdm,
            navId: 'secMainFdm',
            hide: true,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('VIDEO_SCREEN'),
            router: ['/navigate/maintance/video'],
            iconClass: 'icon-video',
            id: MainId.Video,
            navId: 'secMainVideo',
            hide: true,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('SYSTEM_LOG'),
            router: ['/navigate/maintance/systemlog'],
            iconClass: 'icon-systemLog',
            id: MainId.SysLog,
            navId: 'secMainSysLog',
            hide: true,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('COMMON_MAINTANCE_IBMC_LOG'),
            router: ['/navigate/maintance/ibmclog'],
            iconClass: 'icon-ibmcLog',
            id: MainId.IbmcLog,
            navId: 'secMainIbmcLog',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('WORK_NOTE'),
            router: ['/navigate/maintance/worknote'],
            iconClass: 'icon-work',
            id: MainId.Note,
            navId: 'secMainNote',
            hide: false,
            isIE: this.isIE
        }
    ];

    // 用户与安全二级菜单和左侧菜单
    private userSafeSecondMenu: ISecondMenuItem[] = [
        {
            lable: this.translate.instant('COMMON_USER_LOCALUSERS'),
            router: ['/navigate/user/localuser'],
            iconClass: 'icon-local',
            id: UserId.LocalUsers,
            navId: 'secUserLocalUsers',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('COMMON_USER_LDAP'),
            router: ['/navigate/user/ldap'],
            iconClass: 'icon-ldap',
            id: UserId.Ldap,
            navId: 'secUserLDAP',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('COMMON_USER_KERBEROS'),
            router: ['/navigate/user/kerberos'],
            iconClass: 'icon-kerberos',
            id: UserId.Kerberos,
            navId: 'secUserKerberos',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('COMMON_USER_TWO_FACTOR'),
            router: ['/navigate/user/two-factor'],
            iconClass: 'icon-twoFac',
            id: UserId.TwoFactor,
            navId: 'secUserTwoFactor',
            hide: true,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('COMMON_USER_ONLINE'),
            router: ['/navigate/user/online-user'],
            iconClass: 'icon-online',
            id: UserId.Online,
            navId: 'secUserOnline',
            hide: true,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('COMMON_USER_SECURITY'),
            router: ['/navigate/user/security'],
            iconClass: 'icon-security',
            id: UserId.Security,
            navId: 'secUserSecurity',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('TPCM_TITLE'),
            router: ['/navigate/user/tpcm'],
            iconClass: 'icon-tpcm',
            id: UserId.TrustedCompute,
            navId: 'tpcmPage',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('IBMC_CERTIFICATE_UPDATE'),
            router: ['/navigate/user/cert-update'],
            iconClass: 'icon-certUpdate',
            id: UserId.CertUpdate,
            navId: 'secCertUpdate',
            hide: false,
            isIE: this.isIE
        }
    ];

    // 服务管理二级菜单和左侧菜单
    private serviceManageSecondMenu: ISecondMenuItem[] = [
        {
            lable: this.translate.instant('SERVICE_PORT_SERVICE'),
            router: ['/navigate/service/port'],
            iconClass: 'icon-port',
            id: ServiceId.Port,
            navId: 'secPortService',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('SERVICE_WEB_SERVICE'),
            router: ['/navigate/service/web'],
            iconClass: 'icon-web',
            id: ServiceId.Web,
            navId: 'secWebService',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('HOME_VIRTUALCONSOLE_TITLE'),
            router: ['/navigate/service/kvm'],
            iconClass: 'icon-kvm',
            id: ServiceId.Kvm,
            navId: 'secKvmService',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('SERVICE_VMM_ENABLE_LABEL'),
            router: ['/navigate/service/vmm'],
            iconClass: 'icon-vmm',
            id: ServiceId.Vmm,
            navId: 'secVmmService',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('SERVICE_VNC'),
            router: ['/navigate/service/vnc'],
            iconClass: 'icon-vnc',
            id: ServiceId.Vnc,
            navId: 'secVncService',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('SERVICE_SNMP'),
            router: ['/navigate/service/snmp'],
            iconClass: 'icon-snmp',
            id: ServiceId.Snmp,
            navId: 'secSnmpService',
            hide: true,
            isIE: this.isIE
        }
    ];

    // IBMC管理二级菜单和左侧菜单
    private ibmcManageMentSecondMenu: ISecondMenuItem[] = [
        {
            lable: this.translate.instant('IBMC_NETWORK'),
            router: ['/navigate/manager/network'],
            iconClass: 'icon-network',
            id: ManageId.Network,
            navId: 'secNetworkConfig',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('IBMC_LEFTMENU_NTP_TIME'),
            router: ['/navigate/manager/ntp'],
            iconClass: 'icon-ntp',
            id: ManageId.Ntp,
            navId: 'secTimezoneConfig',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('IBMC_FIMWARE_UPDATE'),
            router: ['/navigate/manager/upgrade'],
            iconClass: 'icon-upgrade',
            id: ManageId.Upgrade,
            navId: 'secUpgradeConfig',
            hide: true,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('COMMON_CONFIG_UPDATE'),
            router: ['/navigate/manager/configupgrade'],
            iconClass: 'icon-config',
            id: ManageId.Update,
            navId: 'secUpdateConfig',
            hide: true,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('COMMON_CONFIG_LANGUAGE'),
            router: ['/navigate/manager/language'],
            iconClass: 'icon-language',
            id: ManageId.Language,
            navId: 'secLanguageConfig',
            hide: true,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('IBMC_LICENSE_MANAGE'),
            router: ['/navigate/manager/license'],
            iconClass: 'icon-license',
            id: ManageId.License,
            navId: 'secLicenseConfig',
            hide: true,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('IBMA_TITLE'),
            router: ['/navigate/manager/ibma'],
            iconClass: 'icon-ibma',
            id: ManageId.Ibma,
            navId: 'secIbmaConfig',
            hide: true,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('IBMC_SP_TITLE'),
            router: ['/navigate/manager/sp'],
            iconClass: 'icon-sp',
            id: ManageId.Sp,
            navId: 'secSpConfig',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('IBMC_USB_TITLE'),
            router: ['/navigate/manager/usb'],
            iconClass: 'icon-usb',
            id: ManageId.USB,
            navId: 'secUSBConfig',
            hide: false,
            isIE: this.isIE
        }
    ];

    // 主菜单
    public mainMenu: IMainMenuItem[] = [
        {
            id: 'navHome',
            hide: false,
            lable: 'COMMON_HEADER_HOME',
            router: ['/navigate/home'],
            children: []
        },
        {
            id: 'navSystemManager',
            hide: false,
            lable: 'COMMON_HEADER_SYSTEM',
            router: ['/navigate/system'],
            children: this.systemSecondMenu
        },
        {
            id: 'navMaintance',
            hide: false,
            lable: 'COMMON_HEADER_MAINTANCE',
            router: ['/navigate/maintance'],
            children: this.maintanceSecondMenu
        },
        {
            id: 'navUsersManager',
            hide: false,
            lable: 'COMMON_HEADER_USERS',
            router: ['/navigate/user'],
            children: this.userSafeSecondMenu
        },
        {
            id: 'navServiceManager',
            hide: false,
            lable: 'COMMON_HEADER_SERVICE',
            router: ['/navigate/service'],
            children: this.serviceManageSecondMenu
        },
        {
            id: 'navIbmcManager',
            hide: false,
            lable: 'IBMC_HEAD_IBMC_MAGAGE',
            router: ['/navigate/manager'],
            children: this.ibmcManageMentSecondMenu
        }
    ];

    /**
     * 涉及普通用户的权限菜单主要是： 在线用户，双因素认证，固件升级，FDM,录像截屏，语言管理，配置更新
     * 涉及菜单支持的菜单主要包括：系统监控，风扇，硬分区，FDM，VNC,SNMP，IBMA管理，许可证，语言管理
     */
    public refreshMenu(): void {
        // 根据是否具有用户配置权限来修改相关菜单的显示或隐藏，包括菜单 在线用户，双因素认证，配置更新
        const hasConfigureUsers = this.userInfo.hasPrivileges(['ConfigureUsers']);
        this.hideMenu(this.userSafeSecondMenu, [UserId.Online, UserId.TwoFactor], !hasConfigureUsers);
        this.hideMenu(this.ibmcManageMentSecondMenu, ManageId.Update, !hasConfigureUsers);

        // 根据是否具有调试诊断权限来修改相关菜单的显示或隐藏，涉及菜单 FDM, 录像截屏，系统日志
        const hasOemDiagnosis = this.userInfo.hasPrivileges(['OemDiagnosis']);
        this.hideMenu(this.maintanceSecondMenu, [MainId.Video, MainId.SysLog], !hasOemDiagnosis);

        // 根据是否具有常规配置权限来修改相关菜单的显示或隐藏，涉及菜单 固件升级，语言管理
        const hasConfigureComponents = this.userInfo.hasPrivileges(['ConfigureComponents']);
        this.hideMenu(this.ibmcManageMentSecondMenu, [ManageId.Upgrade, ManageId.Language], !hasConfigureComponents);

        // 是否显示IBMA,根据接口字段来实现
        this.globalData.ibmaSupport.then((res: boolean) => {
            this.hideMenu(this.ibmcManageMentSecondMenu, ManageId.Ibma, !res);
        });

        // 是否显示硬分区,根据接口字段来实现
        this.globalData.partitySupport.then((res: boolean) => {
            this.hideMenu(this.systemSecondMenu, SysId.Partity, !res);
        });

        // 是否显示许可证,根据接口字段来实现
        this.globalData.licenseSupport.then((res: boolean) => {
            this.hideMenu(this.ibmcManageMentSecondMenu, ManageId.License, !res);
        });

        // 是否显示FDM,根据接口字段来实现
        this.globalData.fdmSupport.then((res: boolean) => {
            this.hideMenu(this.maintanceSecondMenu, MainId.Fdm, !res);
        });

        // 是否显示风扇
        this.globalData.fanSupport.then((res: boolean) => {
            this.hideMenu(this.systemSecondMenu, SysId.Fans, !res || hideFansAndPower());
        });

        // 是否显示SNMP,根据接口字段来实现
        this.globalData.snmpSupport.then((res: boolean) => {
            this.hideMenu(this.serviceManageSecondMenu, [ServiceId.Snmp], !res);
        });

        // 是否显示SP管理,根据接口字段来实现
        this.globalData.spSupport.then(res => {
            if (!this.ibmcManageMentSecondMenu[7].hide) {
                this.hideMenu(this.ibmcManageMentSecondMenu, ManageId.Sp, !res);
            }
        });

        // 是否显示SP管理,根据接口字段来实现
        this.globalData.usbSupport.then(res => {
            if (!this.ibmcManageMentSecondMenu[8].hide) {
                this.hideMenu(this.ibmcManageMentSecondMenu, ManageId.USB, !res);
            }
        });

        // 是否显示虚拟控制台,根据接口字段来实现
        this.globalData.kvmSupport.then(res => {
            this.hideMenu(this.serviceManageSecondMenu, ServiceId.Kvm, !res);
        });

        // 是否显示vnc,根据接口字段来实现
        this.globalData.vncSupport.then(res => {
            this.hideMenu(this.serviceManageSecondMenu, ServiceId.Vnc, !res);
        });

        // 是否显示虚拟媒体,根据接口字段来实现
        this.globalData.vmmSupport.then(res => {
            this.hideMenu(this.serviceManageSecondMenu, ServiceId.Vmm, !res);
        });

        // 是否显示性能监控,根据接口字段来实现
        this.globalData.perfSupSupport.then(res => {
            this.hideMenu(this.systemSecondMenu, SysId.Monitor, !res);
        });

        // 是否显示tpcm
        this.globalData.tpcmSupport.then((res: boolean) => {
            this.hideMenu(this.userSafeSecondMenu, UserId.TrustedCompute, !res);
        });

        // 是否显示录像截屏,根据接口字段来实现，但判断优先级在用户权限之下
        this.globalData.videoScreenshotSupport.then(res => {
            if (!res) {
                this.hideMenu(this.maintanceSecondMenu, MainId.Video, !res);
            }
        });
    }

    private hideMenu(menuArr: ISecondMenuItem[], menuId: string | string[], isHide: boolean): void {
        menuArr.map((item) => {
            if (item.id === menuId || menuId?.indexOf(item.id) > -1) {
                item.hide = isHide;
            }
        });
    }
}
