import { TiTreeNode } from '@cloud/tiny3/components/tree/TiTreeComponent';
import { Driver } from './driver.model';

export class DriverNode implements TiTreeNode {

    id: string;
    label: string;
    url: string;
    active: boolean;
    parent: TiTreeNode[];
    disabled: boolean;
    labelId: string;
    readonly componentName: string = 'DriverNode';
    private driver: Driver;
    private cryptoEraseSupported: boolean;
    private raidIndex: number;
    readonly className: string = 'driverClass';
    private raidType: string;

    // 参与逻辑处理数据
    private isRaid: boolean;

    constructor(
        lable: string,
        url: string,
        isRaid: boolean,
        cryptoEraseSupported: boolean,
        id: string,
        raidIndex: number,
        labelId: string,
        raidType: string
    ) {
        this.id = id;
        this.label = lable;
        this.url = url;
        this.isRaid = isRaid;
        this.cryptoEraseSupported = cryptoEraseSupported;
        this.disabled = false;
        this.raidIndex = raidIndex;
        this.labelId = labelId;
        this.raidType = raidType;
    }

    get getRaidIndex(): number {
      return this.raidIndex;
    }

    get getIsRaid(): boolean {
        return this.isRaid;
    }


    set setDriver(driver: Driver) {
        this.driver = driver;
    }

    get getDriver(): Driver {
        return this.driver;
    }

    get getCryptoErase() {
        return this.cryptoEraseSupported;
    }

    get getRaidType(): string {
        return this.raidType;
    }

}
