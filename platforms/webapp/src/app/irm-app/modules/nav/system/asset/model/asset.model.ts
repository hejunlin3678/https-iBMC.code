export class AssetModel {

    constructor(param: {
        state,
        uNum,
        tagUid,
        shelfTime,
        pattern,
        deviceType,
        serialNumber,
        partNumber,
        uHeight,
        factory,
        lifeCircle,
        startTime,
        weight,
        ratedPower,
        owner,
        extendField,
        rWCapability,
        availableRackSpaceU
    }) {
        this.state = param.state;
        this.uNum = param.uNum;
        this.tagUid = param.tagUid;
        this.shelfTime = param.shelfTime;
        this.pattern = param.pattern;
        this.deviceType = param.deviceType;
        this.serialNumber = param.serialNumber;
        this.partNumber = param.partNumber;
        this.uHeight = param.uHeight;
        this.factory = param.factory;
        this.lifeCircle = param.lifeCircle;
        this.startTime = param.startTime;
        this.weight = param.weight;
        this.ratedPower = param.ratedPower;
        this.owner = param.owner;
        this.extendField = param.extendField;
        this.rWCapability = param.rWCapability;
        this.availableRackSpaceU = param.availableRackSpaceU;
    }

    public state: string;
    public uNum: string;
    public tagUid: string;
    public shelfTime: string;
    public pattern: string;
    public deviceType: string;
    public serialNumber: string;
    public partNumber: string;
    public uHeight: number;
    public factory: string;
    public lifeCircle: number;
    public startTime: string;
    public weight: number;
    public ratedPower: number;
    public owner: string;
    public extendField: string[];
    public rWCapability: string;
    public availableRackSpaceU: number;
}
