import { ExtendAttr } from '../../extend-attribute';

export class FPGACard extends ExtendAttr {
    // 名称
    private productName: [string, string];
    // 单板ID
    private boardId: [string, string];
    // PCB版本
    private pcbVersion: [string, string];
    // MCU固件版本
    private mcuFirmwareVersion: [string, string];
    // 平台ID
    private platformId: [string, string];
    // 功率(W)
    private power: [string, string];
    // 固件版本

    constructor(
        productName: string,
        boardId: string,
        pcbVersion: string,
        mcuFirmwareVersion: string,
        platformId: string,
        power: string,
    ) {
        super();
        if (productName !== '--') {
            this.productName = ['COMMON_NAME', productName];
        }
        if (boardId !== '--') {
            this.boardId = ['OTHER_BOARD_ID', boardId];
        }
        if (pcbVersion !== '--') {
            this.pcbVersion = ['COMMON_ALARM_PCB_VERSION', pcbVersion];
        }
        if (mcuFirmwareVersion !== '--') {
            this.mcuFirmwareVersion = ['OTHER_MCU_FIXED_VERSION', mcuFirmwareVersion];
        }
        if (platformId !== '--') {
            this.platformId = ['OTHER_PLATFORM_ID', platformId];
        }
        this.power = ['OTHER_POWER', power];
    }

}
