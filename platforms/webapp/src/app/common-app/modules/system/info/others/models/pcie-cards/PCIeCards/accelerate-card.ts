import { ExtendAttr } from '../../extend-attribute';
import { isOtherEmptyProperty } from 'src/app/common-app/utils';

export class AccelerateCard extends ExtendAttr {
    // 名称
    private productName: string;
    // 单板ID
    private boardId: number;
    // PCB版本
    private pcbVersion: string;
    // 序列号
    private serialNumber: string;
    // 固件版本
    private firmwareVersion: string;
    // BOM ID
    private bomId: number;

    constructor(productName: string, boardId: number, pcbVersion: string, serialNumber: string, firmwareVersion: string, bomId: number) {
        super();
        if (productName !== undefined) {
            this.productName = isOtherEmptyProperty(productName);
        }
        if (boardId !== undefined) {
            this.boardId = isOtherEmptyProperty(boardId);
        }
        if (pcbVersion !== undefined) {
            this.pcbVersion = isOtherEmptyProperty(pcbVersion);
        }
        if (serialNumber !== undefined) {
            this.serialNumber = isOtherEmptyProperty(serialNumber);
        }
        if (firmwareVersion !== undefined) {
            this.firmwareVersion = isOtherEmptyProperty(firmwareVersion);
        }
        if (bomId !== undefined) {
            this.bomId = isOtherEmptyProperty(bomId);
        }
    }

    set setProductName(productName: string) {
        if (productName !== undefined) {
            this.productName = isOtherEmptyProperty(productName);
        }
    }

    set setBoardId(boardId: number) {
        if (boardId !== undefined) {
            this.boardId = isOtherEmptyProperty(boardId);
        }
    }

    set setPcbVersion(pcbVersion: string) {
        if (pcbVersion !== undefined) {
            this.pcbVersion = isOtherEmptyProperty(pcbVersion);
        }
    }

    set setSerialNumber(serialNumber: string) {
        if (serialNumber !== undefined) {
            this.serialNumber = isOtherEmptyProperty(serialNumber);
        }
    }

    set setFirmwareVersion(firmwareVersion: string) {
        if (firmwareVersion !== undefined) {
            this.firmwareVersion = isOtherEmptyProperty(firmwareVersion);
        }
    }

    set setBomId(bomId: number) {
        if (bomId !== undefined) {
            this.bomId = isOtherEmptyProperty(bomId);
        }
    }
}
