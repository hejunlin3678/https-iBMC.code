import { isOtherEmptyProperty } from 'src/app/common-app/utils';
import { ExtendAttr } from '../../extend-attribute';

export class StorageCard extends ExtendAttr {
    // 名称
    private productName: [string, string];
    // 单板ID
    private boardId: [string, number];
    // PCB版本
    private pcbVersion: [string, string];
    // PCB版本
    private pcbVersionuctName: [string, string];
    // SSD1
    private ssd1Presence: [string, string];
    // SSD2
    private ssd2Presence: [string, string];
    // 序列号
    private serialNumber: [string, string];
    // 型号
    private model: [string, string];
    // 固件版本
    private firmwareVersion: [string, string];
    // 剩余磨损率
    private residualWearRate: [string, string];

    constructor(productName: string, boardId: number, pcbVersion: string, serialNumber: string) {
        super();
        if (productName !== undefined) {
            this.productName = ['COMMON_NAME', isOtherEmptyProperty(productName)];
        }
        if (boardId !== undefined) {
            this.boardId = ['OTHER_BOARD_ID', isOtherEmptyProperty(boardId)];
        }
        if (pcbVersion !== undefined) {
            this.pcbVersion = ['COMMON_ALARM_PCB_VERSION', isOtherEmptyProperty(pcbVersion)];
        }
        if (serialNumber !== undefined) {
            this.serialNumber = ['COMMON_SERIALNUMBER', isOtherEmptyProperty(serialNumber)];
        }
    }

    set setPcbVersionuctName(pcbVersionuctName: string) {
        if (pcbVersionuctName !== undefined) {
            this.pcbVersionuctName = ['COMMON_ALARM_PCB_VERSION', isOtherEmptyProperty(pcbVersionuctName)];
        }
    }
    set setSsd1Presence(ssd1Presence: string) {
        if (ssd1Presence !== undefined) {
            this.ssd1Presence = ['SSD1', isOtherEmptyProperty(ssd1Presence)];
        }
    }
    set setSsd2Presence(ssd2Presence: string) {
        if (ssd2Presence !== undefined) {
            this.ssd2Presence = ['SSD2', isOtherEmptyProperty(ssd2Presence)];
        }
    }

    set setModel(model: string) {
        if (model !== undefined) {
            this.model = ['OTHER_MODEL', isOtherEmptyProperty(model)];
        }
    }
    set setFirmwareVersion(firmwareVersion: string) {
        if (firmwareVersion !== undefined) {
            this.firmwareVersion = ['COMMON_FIXED_VERSION', isOtherEmptyProperty(firmwareVersion)];
        }
    }
    set setResidualWearRate(residualWearRate: string) {
        if (residualWearRate !== undefined && isOtherEmptyProperty(residualWearRate) !== '--') {
            residualWearRate = residualWearRate + '%';
            this.residualWearRate = ['WEAR_RATE', residualWearRate];
        }
    }
}
