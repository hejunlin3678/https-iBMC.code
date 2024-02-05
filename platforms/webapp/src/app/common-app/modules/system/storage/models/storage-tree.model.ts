import { TiTreeNode } from '@cloud/tiny3/components/tree/TiTreeComponent';
import { TiTreeUtil } from '@cloud/tiny3';
import { RaidNode } from './raid';

export class StorageTree {

    private constructor() {
        this.foliages = [];
    }


    private static instance: StorageTree;

    private foliages: TiTreeNode[];

    public static getInstance(): StorageTree {
        if (!StorageTree.instance) {
            StorageTree.instance = new StorageTree();
        }
        return StorageTree.instance;
    }

    public static destroy(): void {
        if (StorageTree.instance) {
            StorageTree.instance = null;
        }
    }

    public addFoliage(foliage: TiTreeNode): void {
        this.foliages.push(foliage);
    }

    get getFoliages(): TiTreeNode[] {
        return this.foliages;
    }

    public addTreeNode(children: TiTreeNode[]) {
        if (children) {
            this.foliages = this.foliages.concat(children);
        }
    }

    public checkedNode(nodeId: string, raidId: number) {
        // 设置节点选中状态
        this._checkedNode(this.foliages, nodeId);
    }

    private _checkedNode(treeNode: TiTreeNode[], id: string) {
        if (treeNode && treeNode.length > 0) {
            treeNode.forEach(
                (node: TiTreeNode) => {
                    // id某些特殊场景下存在重复，labelId前端生成具备唯一性
                    if (node.labelId === id) {
                        node.active = true;
                        // checked 存在indeterminate状态，不能更改
                        node.checked = node.checked === false ? true : node.checked;
                    } else {
                        node.active = false;
                        node.checked = node.checked === true ? false : node.checked;
                    }
                    this._checkedNode(node.children || [], id);
                }
            );
        }
    }

    get getCheckedNode(): TiTreeNode {
        const checkNode: TiTreeNode = null;
        return this._getCheckedNode(this.foliages, checkNode);
    }

    private _getCheckedNode(nodeArr: TiTreeNode[], checkNode: TiTreeNode): TiTreeNode {
        if (!checkNode && nodeArr && nodeArr.length > 0) {
            nodeArr.forEach(
                (node: TiTreeNode) => {
                    if (node.active) {
                        checkNode = node;
                        return checkNode;
                    } else {
                        checkNode = this._getCheckedNode(node.children || [], checkNode);
                    }
                }
            );
        }
        return checkNode;
    }

    public multipleChange(treeNode: TiTreeNode) {
        if (this.foliages && this.foliages.length > 0) {
            this.foliages.forEach(
                (node) => {
                    if (node.id === treeNode.id) {
                        node.setMultiple = true;
                    } else if (node.getMultiple) {
                        node.setMultiple = false;
                        node.resetBtn();
                    }
                }
            );
        }
        this._changeDisable(this.foliages, true);
    }

    public changeDisable() {
        this._changeDisable(this.foliages, false);
    }

    private _changeDisable(nodeArr: TiTreeNode[], state: boolean) {
        if (nodeArr && nodeArr.length > 0) {
            nodeArr.forEach(
                (node: TiTreeNode) => {
                    if (node.componentName !== 'VolumeNode') {
                        node.disabled = state;
                    } else {
                        // checked 存在indeterminate状态，不能更改
                        node.checked = node.checked === true ? false : node.checked;
                    }
                    this._changeDisable(node.children, state);
                }
            );
        }
    }

    public getParentRaid(index: number): RaidNode {
        const raidNode: RaidNode = this.foliages[index] as RaidNode;
        return raidNode;
    }

    public getParentRaidById(id: string): RaidNode {
        const raidNode: RaidNode = this.foliages.find(item => item.id === id) as RaidNode;
        return raidNode;
    }

    public getCheckedParent(node: TiTreeNode): TiTreeNode {
        return TiTreeUtil.getParentNode(this.foliages, node);
    }

    public searchNodeById(id: string): TiTreeNode {
        let resultNode: TiTreeNode = null;
        TiTreeUtil.traverse(this.foliages, (node: TiTreeNode): void => {
            if (node.labelId === id) { resultNode = node; }
        });
        return resultNode;
    }

    public getSelectedData(): TiTreeNode[] {
        const resultNode: TiTreeNode[] = [];
        TiTreeUtil.traverse(this.foliages, (node: TiTreeNode): void => {
            if (node.checked === true) { resultNode.push(node); }
        });
        return resultNode;
    }

    public getSelectVolume(): TiTreeNode[] {
        const resultNode: TiTreeNode[] = [];
        TiTreeUtil.traverse(this.foliages, (node: TiTreeNode): void => {
            if (node.checked === true && node.componentName === 'VolumeNode') { resultNode.push(node); }
        });
        return resultNode;
    }
}
