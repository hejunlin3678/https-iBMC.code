import { TiTreeNode } from '@cloud/tiny3/components/tree/TiTreeComponent';

export class HotSpareNode implements TiTreeNode {

    id: string;
    label: string;
    children: TiTreeNode[];
    active: boolean;
    checked: boolean | string;
    labelId: string;
    readonly disabled: boolean = false;
    readonly className: string = 'hotSpareClass';

    constructor(lable: string, id: string, labelId: string) {
        this.id = id;
        this.label =  lable;
        this.checked = false;
        this.labelId = labelId;
    }

    addChildren(children: TiTreeNode): void {
        if (!this.children) { this.children = []; }
        this.children.push(children);
    }

    set setChildren(children: TiTreeNode[]) {
        this.children = children;
    }

}
