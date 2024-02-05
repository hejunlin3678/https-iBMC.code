import { ExtendTitle } from '../extend-title';

export class BasicComputation {
    // 名称
    private boardName: string;
    // 位置
    private position: string;
    // 厂商
    private manufacturer: string;
    // 槽位
    private id: number;
    // 类型
    private deviceType: string;
    // 组件UID
    private componentUniqueID: string;
    // 扩展信息
    private extendInfo: ExtendTitle;

    constructor(
        boardName: string,
        position: string,
        manufacturer: string,
        id: number,
        deviceType: string,
        componentUniqueID: string,
    ) {
        this.boardName = boardName;
        this.position = position;
        this.manufacturer = manufacturer;
        this.id = id;
        this.deviceType = deviceType;
        this.componentUniqueID = componentUniqueID;
    }

    set setExtendInfo(extendInfo: ExtendTitle) {
        this.extendInfo = extendInfo;
    }

}
