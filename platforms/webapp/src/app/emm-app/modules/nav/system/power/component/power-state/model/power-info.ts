import { BoardDomain } from './board-domain';
import { ProductDomain } from './product-domain';

export class PowerInfo {
    public slot: string;
    public powerType: string;
    public powerSoftHard: string;
    public currentPower: number;
    public ratedPower: number;
    public powerState: string;
    public powerStateImg: string;
    public boardInfo: BoardDomain;
    public productInfo: ProductDomain;

    constructor(param: PowerInfo) {
        const {
            slot,
            powerType,
            powerSoftHard,
            currentPower,
            ratedPower,
            powerState,
            powerStateImg,
            boardInfo,
            productInfo
        } = param;
        this.slot = slot;
        this.powerType = powerType;
        this.powerSoftHard = powerSoftHard;
        this.currentPower = currentPower;
        this.ratedPower = ratedPower;
        this.powerState = powerState;
        this.powerStateImg = powerStateImg;
        this.boardInfo = boardInfo;
        this.productInfo = productInfo;
    }
}
