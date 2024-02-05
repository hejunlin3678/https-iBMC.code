import { FatherObj } from './fatherObj';

export class Mainboard extends FatherObj {
    private liveTime: [string, string];
    private boardId: [string, string];
    private ibmcVersion: [string, string];
    private version: [string, string];
    private biosVersion: [string, string];
    private pcbVersion: [string, string];
    private cpldVersion: [string, string];
    private pchVersion: [string, string];
    private mainBoot: [string, string];
    private deviceType: [string, string];
    private backupBoot: [string, string];
    private serialNumber: [string, string];
    private partNumber: [string, string];

    constructor(
        liveTime: string,
        boardId: string,
        ibmcVersion: string,
        version: string,
        biosVersion: string,
        pcbVersion: string,
        cpldVersion: string,
        pchVersion: string,
        mainBoot: string,
        deviceType: string,
        backupBoot: string,
        serialNumber: string,
        partNumber: string,
    ) {
        super();
        this.liveTime = ['FDM_ALARM_LIVE_TIME', liveTime];
        this.boardId = ['OTHER_BOARD_ID', boardId];
        this.ibmcVersion = ['INFO_MAIN_BOARD_IBMC_VERSION', ibmcVersion];
        this.version = ['FDM_ALARM_MAINBORD_VENDOR', version];
        this.biosVersion = ['IBMC_FDM_ALARM_BIOS_VERSION', biosVersion];
        this.pcbVersion = ['COMMON_ALARM_PCB_VERSION', pcbVersion];
        this.cpldVersion = ['COMMIN_CPLD_VERSION', cpldVersion];
        this.pchVersion = ['FDM_PCH_VERSION', pchVersion];
        this.mainBoot = ['FDM_UBOOT_VERSION', mainBoot];
        this.deviceType = ['FDM_MAINBOARD_MODEL', deviceType];
        this.backupBoot = ['FDM_BY_UBOOT_VERSION', backupBoot];
        this.serialNumber = ['FDM_MAINBOARD_SN', serialNumber];
        this.partNumber = ['OTHER_PART_NUMBER', partNumber];
    }
}
