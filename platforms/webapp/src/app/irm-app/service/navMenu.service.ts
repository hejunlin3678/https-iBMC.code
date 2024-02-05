import { Injectable } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { UserInfoService } from 'src/app/common-app/service/user-info.service';
import { GlobalDataService } from 'src/app/common-app/service';
import { BrowserType } from '../../common-app/utils/browser';

interface IMainMenuItem {
    id: string;
    hide: boolean;
    lable: string;
    router: string[];
    children: ISecondMenuItem[];
}

interface ISecondMenuItem {
    id: string;
    navId: string;
    hide: boolean;
    lable: string;
    router: string[];
    iconClass: string;
    isIE: boolean;
}

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
            id: 'leftSysInfo',
            navId: 'secSysInfo',
            hide: false,
            lable: 'COMMON_SYSTEM_INFO',
            router: ['/navigate/system/info'],
            iconClass: 'icon-info',
            isIE: this.isIE
        },
        {
            id: 'leftSysPower',
            navId: 'secSysPower',
            hide: false,
            lable: 'COMMON_SYSTEM_POWER_MANAGEMENT',
            router: ['/navigate/system/power'],
            iconClass: 'icon-power',
            isIE: this.isIE
        },
        {
            id: 'leftSysBattery',
            navId: 'secSysBattery',
            hide: false,
            lable: 'COMMON_SYSTEM_BATTERY',
            router: ['/navigate/system/battery'],
            iconClass: 'icon-battery',
            isIE: this.isIE
        },
        {
            id: 'leftSysFans',
            navId: 'secSysFans',
            hide: false,
            lable: 'COMMON_SYSTEM_FUN_MANAGEMENT',
            router: ['/navigate/system/fans'],
            iconClass: 'icon-fans',
            isIE: this.isIE
        },
        {
            id: 'leftSysAsset',
            navId: 'secSysAsset',
            hide: false,
            lable: 'COMMON_SYSTEM_MANAGEMENT',
            router: ['/navigate/system/asset'],
            iconClass: 'icon-asset',
            isIE: this.isIE
        }
    ];

    // 维护诊断二级菜单和左侧菜单
    private maintanceSecondMenu: ISecondMenuItem[] = [
        {
            lable: this.translate.instant('ALARM_EVENT'),
            router: ['/navigate/maintance/alarm'],
            iconClass: 'icon-alarm',
            id: 'leftMainEvent',
            navId: 'secMainEvent',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('ALARM_REPORT'),
            router: ['/navigate/maintance/report/'],
            iconClass: 'icon-report',
            id: 'leftMainReport',
            navId: 'secMainReport',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('COMMON_MAINTANCE_IBMC_LOG'),
            router: ['/navigate/maintance/ibmclog'],
            iconClass: 'icon-ibmcLog',
            id: 'leftMainIbmcLog',
            navId: 'secMainIbmcLog',
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
            id: 'leftUserLocalUsers',
            navId: 'secUserLocalUsers',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('COMMON_USER_LDAP'),
            router: ['/navigate/user/ldap'],
            iconClass: 'icon-ldap',
            id: 'leftUserLdap',
            navId: 'secUserLDAP',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('COMMON_USER_KERBEROS'),
            router: ['/navigate/user/kerberos'],
            iconClass: 'icon-kerberos',
            id: 'leftUserKerberos',
            navId: 'secUserKerberos',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('COMMON_USER_ONLINE'),
            router: ['/navigate/user/online-user'],
            iconClass: 'icon-online',
            id: 'leftUserOnline',
            navId: 'secUserOnline',
            hide: true,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('COMMON_USER_SECURITY'),
            router: ['/navigate/user/security'],
            iconClass: 'icon-security',
            id: 'leftUserSecurity',
            navId: 'secUserSecurity',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('IBMC_CERTIFICATE_UPDATE'),
            router: ['/navigate/user/cert-update'],
            iconClass: 'icon-certUpdate',
            id: 'leftCertUpdate',
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
            id: 'leftPortService',
            navId: 'secPortService',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('SERVICE_WEB_SERVICE'),
            router: ['/navigate/service/web'],
            iconClass: 'icon-web',
            id: 'leftWebService',
            navId: 'secWebService',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('SERVICE_SNMP'),
            router: ['/navigate/service/snmp'],
            iconClass: 'icon-snmp',
            id: 'leftSnmpService',
            navId: 'secSnmpService',
            hide: false,
            isIE: this.isIE
        }
    ];

    // IBMC管理二级菜单和左侧菜单
    private ibmcManageMentSecondMenu: ISecondMenuItem[] = [
        {
            lable: this.translate.instant('IBMC_NETWORK'),
            router: ['/navigate/manager/network'],
            iconClass: 'icon-network',
            id: 'leftNetworkConfig',
            navId: 'secNetworkConfig',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('IBMC_LEFTMENU_NTP_TIME'),
            router: ['/navigate/manager/ntp'],
            iconClass: 'icon-ntp',
            id: 'leftNtpConfig',
            navId: 'secTimezoneConfig',
            hide: false,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('IBMC_FIMWARE_UPDATE'),
            router: ['/navigate/manager/upgrade'],
            iconClass: 'icon-upgrade',
            id: 'leftUpgradeConfig',
            navId: 'secUpgradeConfig',
            hide: true,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('COMMON_CONFIG_UPDATE'),
            router: ['/navigate/manager/configupgrade'],
            iconClass: 'icon-config',
            id: 'leftUpdateConfig',
            navId: 'secUpdateConfig',
            hide: true,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('COMMON_CONFIG_LANGUAGE'),
            router: ['/navigate/manager/language'],
            iconClass: 'icon-language',
            id: 'leftLanguageConfig',
            navId: 'secLanguageConfig',
            hide: true,
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
        this.userSafeSecondMenu[3].hide = !hasConfigureUsers;
        this.ibmcManageMentSecondMenu[3].hide = !hasConfigureUsers;

        // 根据是否具有常规配置权限来修改相关菜单的显示或隐藏，涉及菜单 固件升级，语言管理
        const hasConfigureComponents = this.userInfo.hasPrivileges(['ConfigureComponents']);
        this.ibmcManageMentSecondMenu[2].hide = !hasConfigureComponents;
        this.ibmcManageMentSecondMenu[4].hide = !hasConfigureComponents;
    }
}
