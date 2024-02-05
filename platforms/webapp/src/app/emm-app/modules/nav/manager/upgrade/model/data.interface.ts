import { UpgradeModel } from './upgrade-model';

export class IEmmInfoDetail {
    private slot: [string, string];
    private name: [string, string];
    private softwareVersion: [string, string];
    private cpldVersion: [string, string];
    private mainActiveUboot: [string, string];
    private mainBackupUboot: [string, string];
    private activeSoftware: [string, string];
    private backupSoftware: [string, string];
    private pcbVersion: [string, string];
    private bomVersion: [string, string];
    private activeBackupSoftware?: [string, string];

    constructor(
        slot: string,
        name: string,
        softwareVersion: string,
        cpldVersion: string,
        mainActiveUboot: string,
        mainBackupUboot: string,
        activeSoftware: string,
        backupSoftware: string,
        pcbVersion: string,
        bomVersion: string,
        activeBackupSoftware?: string
    ) {
        this.slot = ['INFO_MAIN_BOARD_SLOT', slot];
        this.name = ['OTHER_BOARDNAME', name];
        this.softwareVersion = ['EMM_UPGRADE_SOFTWARE_VERSION', softwareVersion];
        this.cpldVersion = ['COMMIN_CPLD_VERSION', cpldVersion];
        this.mainActiveUboot = ['EMM_UPGRADE_MAIN_BOOT', mainActiveUboot];
        this.mainBackupUboot = ['EMM_UPGRADE_STANDBY_BOOT', mainBackupUboot];
        this.activeSoftware = ['EMM_UPGRADE_MAIN_SOFTWARE', activeSoftware];
        this.backupSoftware = ['EMM_UPGRADE_STANDBY_SOFTWARE', backupSoftware];
        this.pcbVersion = ['EMM_UPGRADE_PCB', pcbVersion];
        this.bomVersion = ['EMM_UPGRADE_BOMT', bomVersion];
        this.activeBackupSoftware = ['EMM_UPGRADE_AVAILABLE', activeBackupSoftware];
    }
}
// EMM列表数据
export interface IEmmInfo {
    id: number;
    name: string;
    softwareVersion: string;
    spare: boolean;
    cpldVersion: string;
    infoDetail: object;
    upgrade?: UpgradeModel;
}

// 风扇信息
export interface IFansInfo {
    slot: string;
    boardType: string;
    HardwareVersion: string;
    softwareVersion: string;
}

// LCD信息
export interface ILcdInfo {
    name: string;
    version: string;
}

// 板名称
export enum EmmInfo {
    HMM1 = 'HMM1',
    HMM2 = 'HMM2',
    TCE8040 = 'TCE8040',
    TCE8080 = 'TCE8080',
    E9000 = 'E9000',
    MM920 = 'MM920'
}

