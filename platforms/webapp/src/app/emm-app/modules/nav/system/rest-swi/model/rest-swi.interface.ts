
export class Fans {
    private name: string;
    private speed: string;
    private softwareVersion: string;
    private pcbVersion: string;
    private health: string;
    private statusHealth: string;
    private detailInfo: object;
    private detailInfoProduct: object;

    constructor(
        name: string,
        speed: string,
        softwareVersion: string,
        pcbVersion: string,
        health: string,
        statusHealth: string,
        detailInfo: object,
        detailInfoProduct: object,
    ) {
        this.name = name;
        this.speed = speed;
        this.softwareVersion = softwareVersion;
        this.pcbVersion = pcbVersion;
        this.health = health;
        this.statusHealth = statusHealth;
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
