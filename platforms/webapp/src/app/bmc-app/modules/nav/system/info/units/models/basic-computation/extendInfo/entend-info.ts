import { ExtendTitle } from '../../extend-title';

export class ExtendInfo extends ExtendTitle {
    // 序列号
    private serialNumber: [string, string];
    // PCB版本
    private pCBVersion: [string, string];
    // CPLD版本
    private cPLDVersion: [string, string];
    // 硬件自描述
    private selfDescripVersion: [string, string];
    // BIOS1版本
    private bios1Version: [string, string];
    // 部件编码
    private partNumber: [string, string];
    // BIOS2版本
    private bios2Version: [string, string];
    // 单板ID
    private boardId: [string, string];
    // MCU版本
    private mcuVersion: [string, string];

    constructor(
        serialNumber: string,
        pCBVersion: string,
        cPLDVersion: string,
        selfDescripVersion: string,
        bios1Version: string,
        partNumber: string,
        bios2Version: string,
        boardId: string,
        mcuVersion: string
    ) {
        super();
        if (serialNumber !== '--') {
            this.serialNumber = ['COMMON_SERIALNUMBER', serialNumber];
        }
        if (pCBVersion !== '--') {
            this.pCBVersion = ['COMMON_ALARM_PCB_VERSION', pCBVersion];
        }
        if (cPLDVersion !== '--') {
            this.cPLDVersion = ['COMMIN_CPLD_VERSION', cPLDVersion];
        }
        if (selfDescripVersion !== '--') {
            this.selfDescripVersion = ['OTHER_HAESWARD_DESCRIPTION_VERSION', selfDescripVersion];
        }
        if (bios1Version !== '--') {
            this.bios1Version = ['OTHER_BIOS1_VERSION', bios1Version];
            // 如果没有bios2版本，bios1版本标签替换成“BIOS版本”显示
            if (bios2Version === '--') {
                this.bios1Version[0] = 'INFO_MAIN_BOARD_BIOS_VERSION';
            }
        }
        if (partNumber !== '--') {
            this.partNumber = ['OTHER_PART_NUMBER', partNumber];
        }
        if (bios2Version !== '--') {
            this.bios2Version = ['OTHER_BIOS2_VERSION', bios2Version];
        }
        if (boardId !== '--') {
            this.boardId = ['OTHER_BOARD_ID', boardId];
        }
        if (mcuVersion !== '--') {
            this.mcuVersion = ['OTHER_MCU_VERSION', mcuVersion];
        }
    }
}
