import { Injectable } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { BrowserType } from 'src/app/common-app/utils';
import { UserInfoService, GlobalDataService } from 'src/app/common-app/service';

import { ISecondMenuItem, IMainMenuItem } from 'src/app/common-app/models';
import { SysId, MainId, UserId, ServiceId, ManageId } from '../models/common.datatype';


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
            id: SysId.PowerInfo,
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
            hide: false,
            lable: 'FAN_NAME',
            router: ['/navigate/system/fans'],
            iconClass: 'icon-fans',
            isIE: this.isIE
        },
        {
            id: SysId.BmcUser,
            navId: 'secSysBmcUser',
            hide: false,
            lable: 'IBMC_USER_MANAGEMENT',
            router: ['/navigate/system/bmcUser'],
            iconClass: 'icon-local',
            isIE: this.isIE
        },
        {
            id: SysId.BmcNet,
            navId: 'secSysBmcNet',
            hide: false,
            lable: 'IBMC_NETWORK_MANAGEMENT',
            router: ['/navigate/system/bmcNetwork'],
            iconClass: 'icon-network',
            isIE: this.isIE
        },
        {
            id: SysId.PowerOnOff,
            navId: 'secSysPowerOnOff',
            hide: false,
            lable: 'POWER_ON_OFF',
            router: ['/navigate/system/powerOnOff'],
            iconClass: 'icon-powerOnOff',
            isIE: this.isIE
        },
        {
            id: SysId.RestSwi,
            navId: 'secSysRestSwi',
            hide: false,
            lable: 'EMM_SWAP_CONFIG_RECOVERY',
            router: ['/navigate/system/restSwi'],
            iconClass: 'icon-restSwi',
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
            lable: this.translate.instant('EMM_ALARM_CONFIG'),
            router: ['/navigate/maintance/alarm-config'],
            iconClass: 'icon-level',
            id: MainId.Config,
            navId: 'secMainConfig',
            hide: false,
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
            lable: this.translate.instant('COMMON_CONFIG_LANGUAGE'),
            router: ['/navigate/manager/language'],
            iconClass: 'icon-language',
            id: ManageId.Language,
            navId: 'secLanguageConfig',
            hide: true,
            isIE: this.isIE
        },
        {
            lable: this.translate.instant('COMMON_BOARD_SWITCH'),
            router: ['/navigate/manager/switch'],
            iconClass: 'icon-usb',
            id: ManageId.Switch,
            navId: 'secSwitchConfig',
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
     * 涉及普通用户的权限菜单主要是： 在线用户，固件升级，语言管理，配置更新
     * 涉及菜单支持的菜单主要包括：语言管理
     */
    public refreshMenu(): void {
        // 根据是否具有用户配置权限来修改相关菜单的显示或隐藏，包括菜单 在线用户，双因素认证，配置更新
        const hasConfigureUsers = this.userInfo.hasPrivileges(['ConfigureUsers']);
        this.hideMenu(this.userSafeSecondMenu, [UserId.Online, UserId.TwoFactor], !hasConfigureUsers);
        this.hideMenu(this.ibmcManageMentSecondMenu, ManageId.Upgrade, !hasConfigureUsers);

        // 根据是否具有常规配置权限来修改相关菜单的显示或隐藏，涉及菜单 固件升级，语言管理
        const hasConfigureComponents = this.userInfo.hasPrivileges(['ConfigureComponents']);
        this.hideMenu(this.ibmcManageMentSecondMenu, [ManageId.Upgrade, ManageId.Language], !hasConfigureComponents);

        // 根据“SwiSupported”判断是否显示，涉及交换配置恢复
        this.globalData.swiSupport.then(res => {
            this.hideMenu(this.systemSecondMenu, SysId.RestSwi, !res);
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
