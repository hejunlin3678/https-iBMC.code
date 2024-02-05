import { IAccess, IAccessInfo, SystemModel } from '../home.datatype';

export class Accesses {
    private accesses: IAccess[];

    constructor(
        accessInfo: IAccessInfo,
        hasLoginRight: boolean,
        hasConfigureComponentsRight: boolean,
        hasOemSecurityMgmtRight: boolean,
        hasConfigureUserRight: boolean,
        hardShow: boolean
    ) {
        this.accesses = [
            // 本地用户
            {
                show: hasLoginRight,
                title: 'HOME_LOCALUSER',
                id: 'LocalUser',
                icon: 'dash-quick-access-user-icon',
                imgUrl: `assets/pangea-assets/image/home/default/dash-quick-access-user-icon.png`,
                state: '/navigate/user/localuser',
                disable: !hasLoginRight,
                lock: false
            },
            // 网络配置
            {
                show: hasLoginRight,
                title: 'HOME_NETWORK_CONFIGURATION',
                id: 'Network',
                icon: 'dash-quick-access-netw-icon',
                imgUrl: `assets/pangea-assets/image/home/default/dash-quick-access-netw-icon.png`,
                state: '/navigate/manager/network',
                lock: false
            },
            // 电源控制
            {
                show: hasLoginRight,
                title: 'HOME_POWER_CONTROL',
                id: 'PowerControl',
                icon: 'dash-quick-access-powe-icon',
                imgUrl: `assets/pangea-assets/image/home/default/dash-quick-access-powe-icon.png`,
                state: '/navigate/system/power',
                lock: false
            },
            // 固件升级
            {
                show: hasConfigureComponentsRight,
                title: 'HOME_FIRMWARE_UPGRADES',
                id: 'Upgrade',
                icon: 'dash-quick-access-hotw-icon',
                imgUrl: `assets/pangea-assets/image/home/default/dash-quick-access-hotw-icon.png`,
                state: '/navigate/manager/upgrade',
                lock: false
            },
            // 一键收集
            {
                show: hasOemSecurityMgmtRight,
                title: 'HOME_ONE_CLICK_COLLECTION',
                id: 'Collection',
                icon: 'dash-quick-access-conl-icon',
                imgUrl: `assets/pangea-assets/image/home/default/dash-quick-access-conl-icon.png`,
                onKeySelect: true,
                lock: false
            },
            // 恢复配置
            {
                show: hasConfigureUserRight,
                title: 'HOME_RESTORE_CONFIGURATION',
                id: 'ConfigureUser',
                icon: 'dash-quick-access-rest-icon',
                imgUrl: `assets/pangea-assets/image/home/default/dash-quick-access-rest-icon.png`,
                restoreSet: true,
                lock: true
            },
            {
                show: hardShow,
                title: 'HOME_HARD_PARTITION_SETTINGS',
                id: 'HardPart',
                icon: 'dash-quick-access-hardpart-icon',
                imgUrl: `assets/pangea-assets/image/home/default/dash-quick-access-hardpart-icon.png`,
                state: '/navigate/system/partity',
                disable: !hasLoginRight,
                lock: false
            },
            {
                show: accessInfo.FusionPartition === SystemModel.DualSystem,
                title: 'HOME_NODE_JUMP',
                id: 'NodeJump',
                icon: 'dash-quick-access-nodejump-icon',
                imgUrl: `assets/pangea-assets/image/home/default/dash-quick-access-nodejump-icon.png`,
                disable: !hasLoginRight,
                nodeChange: true,
                nodeIp: accessInfo.nodeIp,
                lock: false
            }
        ];

        this.accesses = this.accesses.filter((access: IAccess) => access.show);
    }

    get getAccesses(): IAccess[] {
        return this.accesses;
    }
}

