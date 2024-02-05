import { TiTreeNode } from '@cloud/tiny3/components/tree/TiTreeComponent';
import { IRaidButton } from '../storage-interface';
import { Raid } from './raid.model';

export class RaidNode implements TiTreeNode {

    id: string;
    label: string;
    url: string;
    children: TiTreeNode[];
    checked: boolean | string;
    active: boolean;
    expanded: boolean;
    disabled: boolean;
    labelId: string;
    epdSupported: boolean;
    jbodStateSupported: boolean;
    readonly componentName: string = 'RaidNode';
    private multiple: boolean;

    private raidButton: IRaidButton;
    private raid: Raid;
    private raidType: string;
    private specials8iBoard: boolean;
    private supportCacheCade: boolean;

    constructor(
        lable: string,
        url: string,
        id: string,
        volumeSupported: boolean,
        labelId: string,
        epdSupported: boolean,
        jbodStateSupported: boolean,
        raidType: string,
        specials8iBoard: boolean,
        supportCacheCade: boolean
    ) {
        this.id = id;
        this.label =  lable;
        this.url = url;
        this.expanded = false;
        this.checked = false;
        this.multiple = false;
        this.raidButton = {
            isShow: volumeSupported,
            button: [
                {
                    id: 'add',
                    label: 'STORE_ADD',
                    isShow: true,
                    disable: true
                },
                {
                    id: 'edit',
                    label: 'COMMON_EDIT',
                    isShow: true,
                    disable: true
                },
                {
                    id: 'delete',
                    label: 'COMMON_DELETE',
                    isShow: false,
                    disable: false
                },
                {
                    id: 'cancel',
                    label: 'COMMON_CANCEL',
                    isShow: false,
                    disable: false
                },
            ]
        };
        this.disabled = false;
        this.labelId = labelId;
        this.epdSupported = epdSupported;
        this.jbodStateSupported = jbodStateSupported;
        this.raidType = raidType;
        this.specials8iBoard = specials8iBoard;
        this.supportCacheCade = supportCacheCade;
    }

    get getMultiple(): boolean {
        return this.multiple;
    }

    set setMultiple(multiple: boolean) {
        this.multiple = multiple;
    }

    addChildren(children: TiTreeNode): void {
        if (!this.children) { this.children = []; }
        this.children.push(children);
    }

    addTreeNode(children: TiTreeNode[]) {
        if (!this.children) { this.children = []; }
        this.children = this.children.concat(children);
    }

    set setRaid(raid: Raid) {
        this.raid = raid;
    }

    get getRaid(): Raid {
        return this.raid;
    }

    get getUrl(): string {
        return this.url;
    }

    get getRaidButton(): IRaidButton {
        return this.raidButton;
    }

    get getRaidType(): string {
        return this.raidType;
    }

    get getSpecials8iBoard(): boolean {
        return this.specials8iBoard;
    }

    get getSupportCacheCade(): boolean {
        return this.supportCacheCade;
    }

    set setRaidButton(raidButton: IRaidButton) {
        this.raidButton = raidButton;
    }

    set setRaidButShow(isShow: boolean) {
        this.raidButton.isShow = isShow;
    }

    set setAddButShow(isShow: boolean) {
        const raidButton = Object.assign({}, this.raidButton);
        raidButton.button[0].isShow = isShow;
        this.raidButton = raidButton;
    }

    set setAddButDisable(disable: boolean) {
        this.raidButton.button[0].disable = disable;
    }

    set setEditButShow(isShow: boolean) {
        const raidButton = Object.assign({}, this.raidButton);
        raidButton.button[1].isShow = isShow;
        this.raidButton = raidButton;
    }

    set setEditButDisable(disable: boolean) {
        this.raidButton.button[1].disable = disable;
    }

    set setDelButShow(isShow: boolean) {
        const raidButton = Object.assign({}, this.raidButton);
        raidButton.button[2].isShow = isShow;
        this.raidButton = raidButton;
    }

    set setDelButDisable(disable: boolean) {
        this.raidButton.button[2].disable = disable;
    }

    set setCancleButShow(isShow: boolean) {
        const raidButton = Object.assign({}, this.raidButton);
        raidButton.button[3].isShow = isShow;
        this.raidButton = raidButton;
    }

    set setCancleButDisable(disable: boolean) {
        this.raidButton.button[3].disable = disable;
    }

    set setEpdSupported(epdSupported: boolean) {
        this.epdSupported = epdSupported;
    }

    get getEpdSupported(): boolean {
        return this.epdSupported;
    }

    set setJbodStateSupported(jbodStateSupported: boolean) {
        this.jbodStateSupported = jbodStateSupported;
    }

    get getJbodStateSupported(): boolean {
        return this.jbodStateSupported;
    }

    public resetBtn() {
        this.setAddButShow = true;
        this.setEditButShow = true;
        this.setCancleButShow = false;
        this.setDelButShow = false;
    }

    /**
     * raid按钮操作状态判定方法
     * 存在逻辑盘才能操作添加、编辑按钮; 只存在物理盘可以操作添加按钮
     * @param driveNum 物理盘数量
     * @param volumeNum 逻辑盘数量
     */
    public checkBtnState(driveNum: number, volumeNum: number, volumeSupport: boolean): void {
        if (volumeNum > 0) {
            this.setEditButDisable = false;
            this.setAddButDisable = false;
        } else if (driveNum > 0 && volumeSupport) {
            this.setAddButDisable = false;
        }
    }

}

