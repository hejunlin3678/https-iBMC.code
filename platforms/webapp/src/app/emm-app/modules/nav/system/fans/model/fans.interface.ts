
export class Fans {
    private name: string;
    private speed: string;
    private softwareVersion: string;
    private pcbVersion: string;
    private health: string;
    private status: number;
    private detailInfo: object;
    private detailInfoProduct: object;

    constructor(
        name: string,
        speed: string,
        softwareVersion: string,
        pcbVersion: string,
        health: string,
        status: number,
        detailInfo: object,
        detailInfoProduct: object,
    ) {
        this.name = name;
        this.speed = speed;
        this.softwareVersion = softwareVersion;
        this.pcbVersion = pcbVersion;
        this.health = health;
        this.status = status;
        this.detailInfo = detailInfo;
        this.detailInfoProduct = detailInfoProduct;
    }
}
export interface ITitle {
    title: string;
    width: string;
    show: boolean;
}
export interface ISpeedAdjustmentMode {
    id: string;
    label: string;
    content: string;
    isSelect: boolean;
    isShow: boolean;
    tip?: string;
}
