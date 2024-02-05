import { NIC } from '../nic';
import { PortArr } from '../ports/port-attr';

export class NetCard extends NIC {

    private cardName: [string, string];
    private cardModel: [string, string];
    private manufacturer: [string, string];
    private chipManufacturer: [string, string];
    private boardId: [string, string];
    private componentUniqueID: [string, string];
    private firmwareVersion: [string, string];
    private cardManufacturer: [string, string];
    private module: [string, string];
    private pcbVersion: [string, string];
    private associatedResource: [string, string];
    private rootBDF: [string, string];
    private pcieSlotId: [string, string];
    private ports: PortArr;

    constructor(
        name: string,
        id: string,
        moreInfo: object,
        init: boolean,
        cardName: string,
        cardModel: string,
        manufacturer: string,
        chipManufacturer: string,
        boardId: string,
        componentUniqueID: string,
        firmwareVersion: string,
        cardManufacturer: string,
        module: string,
        pcbVersion: string,
        associatedResource: string,
        rootBDF: string,
        pcieSlotId: string,
        hotPlugSupported: boolean,
        hotPlugCtrlStatus: string,
        adapterId: string,
        hotPlugAttention: boolean,
    ) {
        super(name, id, moreInfo, init, hotPlugSupported, hotPlugCtrlStatus, adapterId, hotPlugAttention);
        this.cardName = ['NET_NAME', cardName];
        this.cardModel = ['NET_TYPE', cardModel];
        this.manufacturer = ['NET_CHIP_VENDOR', manufacturer];
        this.boardId = ['OTHER_BOARD_ID', boardId];
        this.componentUniqueID = ['OTHER_COMPONENT_UNIQUE_ID', componentUniqueID];
        this.firmwareVersion = ['COMMON_FIXED_VERSION', firmwareVersion];
        this.cardManufacturer = ['COMMON_MANUFACTURER', cardManufacturer];
        this.chipManufacturer = ['NET_CHIP_VENDOR', chipManufacturer];
        this.module = ['NET_CHIP_MODEL', module];
        this.pcbVersion = ['COMMON_ALARM_PCB_VERSION', pcbVersion];
        this.associatedResource = ['COMMON_RESOURCE_OWNER', associatedResource];
        this.rootBDF = ['BUS_INFO_BDF', rootBDF];
        this.pcieSlotId = ['PCIE_SLOTID', pcieSlotId];
    }

    set setPort(ports: PortArr) {
        this.ports = ports;
    }

    get getPort(): PortArr {
        return this.ports;
    }
}
