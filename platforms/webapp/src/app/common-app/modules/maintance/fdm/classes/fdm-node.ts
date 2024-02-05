import { TiTreeNode } from '@cloud/tiny3/components/tree/TiTreeComponent';
import { FatherObj } from './fatherObj';

export class FDMNode implements TiTreeNode {

    label: string;
    expanded: boolean;
    collapseIcon: string;
    children: TiTreeNode[];
    checked: boolean | string;

    private url: string;
    private componentID: number;
    private eventReports: string;
    private deviceInfo: string;
    private onlineTime: string;
    private fatherObj: FatherObj;

    constructor(
        label: string,
        componentID: number,
        eventReports: string,
        deviceInfo: string,
        onlineTime: string,
    ) {
        this.label = label;
        this.componentID = componentID;
        this.eventReports = eventReports;
        this.deviceInfo = deviceInfo;
        this.onlineTime = onlineTime;
        this. checked = false;
    }

    set setChildren(nodeArr: FDMNode[]) {
        this.children = nodeArr;
    }

    get getLabel(): string {
        return this.label;
    }

    get getChildren(): TiTreeNode[] {
        return this.children;
    }

    addTreeNode(children: TiTreeNode[]) {
        this.children = this.children.concat(children);
    }

    get getComponentID(): number {
        return this.componentID;
    }

    get getEventReports(): string {
        return this.eventReports;
    }

    get getDeviceInfo(): string {
        return this.deviceInfo;
    }

    get getOnlineTime(): string {
        return this.onlineTime;
    }

    get getFatherObj(): FatherObj {
        return this.fatherObj || null;
    }

    set setFatherObj(obj: FatherObj) {
        this.fatherObj = obj;
    }

}
