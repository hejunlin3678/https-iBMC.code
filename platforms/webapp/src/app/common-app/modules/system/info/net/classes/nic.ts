export class NIC {

    private name: string;
    private isInit: boolean;
    private id: string;
    public moreInfo: object;
    public hotPlugSupported: boolean;
    public hotPlugAttention: boolean;
    public hotPlugCtrlStatus: string;
    public adapterId: string;

    constructor(name: string, id: string, moreInfo: object, init?: boolean,
        hotPlugSupported?: boolean, hotPlugCtrlStatus?: string, adapterId?: string, hotPlugAttention?: boolean) {
        this.name = name;
        this.id = id;
        this.moreInfo = moreInfo;
        this.hotPlugSupported = hotPlugSupported;
        this.hotPlugAttention = hotPlugAttention;
        this.hotPlugCtrlStatus = hotPlugCtrlStatus;
        this.adapterId = adapterId;
        this.isInit = init ? init : false;
    }

    get getInit(): boolean {
        return this.isInit;
    }

    get getName(): string {
        return this.name;
    }

    get getMoreInfo(): object {
        return this.moreInfo;
    }
}
