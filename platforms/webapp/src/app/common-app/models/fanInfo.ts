export class FanInfo {
    public id: string;
    public fanName: string;
    public fanModel: string;
    public fanSpeed: string;
    public fanRate: string;
    public fanPartNumber: string;
    // 状态是否为“未检测到”
    public presence: string;

    constructor(id: string, fanName: string, fanModel: string, fanSpeed: string, fanRate: string, fanPartNumber: string, presence: string) {
        this.id = id;
        this.fanName = fanName === null ? '--' : fanName;
        this.fanModel = fanModel === null ? '--' : fanModel;
        this.fanSpeed = fanSpeed === null ? '--' : fanSpeed;
        this.fanRate = fanRate === null ? '--' : fanRate;
        this.fanPartNumber = fanPartNumber === null ? '--' : fanPartNumber;
        this.presence = presence === null ? '' : presence;
    }
}
