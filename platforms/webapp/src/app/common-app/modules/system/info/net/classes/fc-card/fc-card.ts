import { NIC } from '../nic';
import { PortArr } from '../ports/port-attr';

export class FcCard extends NIC {
    public cardName: [string, string];
    public model: [string, string];
    public chipModel: [string, string];
    public manufacturer: [string, string];
    public chipManufacturer: [string, string];
    public boardId: [string, string];
    public healthStatus: [string, string];
    public partNumber: [string, string];
    // PCB版本
    public pCBVersion: [string, string];
    // 资源归属
    public associatedResource: [string, string];
    public serialNumber: [string, string];
    private pcieSlotId: [string, string];
    public ports: PortArr;
    constructor(
        name: string,
        id: string,
        moreInfo: object,
        init: boolean,
        cardName: string,
        model: string,
        chipModel: string,
        manufacturer: string,
        chipManufacturer: string,
        boardId,
        partNumber: string,
        pCBVersion: string,
        associatedResource: string,
        serialNumber: string,
        pcieSlotId: string
    ) {
        super(name, id, moreInfo, init);
        this.cardName = ['NET_NAME', cardName];
        this.model = ['NET_TYPE', model];
        this.chipModel = ['NET_CHIP_MODEL', chipModel];
        this.manufacturer = ['COMMON_MANUFACTURER', manufacturer];
        this.chipManufacturer = ['NET_CHIP_VENDOR', chipManufacturer];
        this.boardId = ['OTHER_BOARD_ID', boardId];
        this.partNumber = ['OTHER_PART_NUMBER', partNumber];
        this.pCBVersion = ['COMMON_ALARM_PCB_VERSION', pCBVersion];
        this.associatedResource = ['COMMON_RESOURCE_OWNER', associatedResource];
        this.serialNumber = ['COMMON_SERIALNUMBER', serialNumber];
        this.pcieSlotId = ['PCIE_SLOTID', pcieSlotId];
    }

    set setPort(ports: PortArr) {
        this.ports = ports;
    }

    get getPort(): PortArr {
        return this.ports;
    }
}
