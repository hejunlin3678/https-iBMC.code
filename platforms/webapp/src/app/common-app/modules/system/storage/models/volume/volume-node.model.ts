import { TiTreeNode } from '@cloud/tiny3/components/tree/TiTreeComponent';
import { Volume } from './volume.model';

export class VolumeNode implements TiTreeNode {

    id: string;
    label: string;
    url: string;
    children: TiTreeNode[];
    checked: boolean | string;
    active: boolean;
    labelId: string;
    readonly disabled: boolean = false;
    private raidUrl: string;
    private volume: Volume;
    private ssdCachecadeVolume: boolean;
    private ssdCachingEnabled: boolean;
    private capacityBytes: number;
    private raidIndex: number;
    private raidLevel: string;
    private raidType: string;
    private isSPType: boolean;
    private driverNode: string[] = [];
    readonly componentName: string = 'VolumeNode';
    readonly className: string = 'volumeClass';

    constructor(
        lable: string,
        url: string,
        raidUrl: string,
        id: string,
        raidIndex: number,
        raidLevel: string,
        labelId: string,
        raidType: string,
        isSPType: boolean,
        ssdCachecadeVolume: boolean,
        ssdCachingEnabled: boolean,
        capacityBytes: number
    ) {
        this.id = id;
        this.label =  lable;
        this.url = url;
        this.raidUrl = raidUrl;
        this.checked = false;
        this.raidIndex = raidIndex;
        this.raidLevel = raidLevel;
        this.labelId = labelId;
        this.raidType = raidType;
        this.isSPType = isSPType;
        this.ssdCachecadeVolume = ssdCachecadeVolume;
        this.ssdCachingEnabled = ssdCachingEnabled;
        this.capacityBytes = capacityBytes;
    }

    addChildren(children: TiTreeNode): void {
        if (!this.children) { this.children = []; }
        this.children.push(children);
    }

    set setChildren(children: TiTreeNode[]) {
        this.children = children;
    }

    addTreeNode(children: TiTreeNode[]) {
        if (!this.children) { this.children = []; }
        this.children = this.children.concat(children);
    }

    get getRaidIndex(): number {
      return this.raidIndex;
    }

    get getCapacityBytes(): number {
        return this.capacityBytes;
      }

    get getUrl(): string {
        return this.url;
    }

    get getRaidUrl(): string {
        return this.raidUrl;
    }

    get getRaidLevel() {
        return this.raidLevel;
    }

    get getDriverNode() {
        const driverNode = [];
        this.children[0]?.children?.forEach(item => {
            driverNode.push(item.label);
        });
        this.driverNode = [...new Set(driverNode)];
        return this.driverNode;
    }

    get getRaidType() {
        return this.raidType;
    }

    get getSPType(): boolean {
        return this.isSPType;
    }

    set setVolume(volume: Volume) {
        this.volume = volume;
    }

    get getVolume(): Volume {
        return this.volume;
    }
    get getSSDCachecadeVolume(): boolean {
        return this.ssdCachecadeVolume;
    }

    get getSSDCachingEnabled(): boolean {
        return this.ssdCachingEnabled;
    }
}
