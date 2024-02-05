import {
    Component,
    OnInit,
    ViewContainerRef,
    ViewChild,
    ComponentFactoryResolver,
    OnDestroy,
    ElementRef,
    ComponentRef
} from '@angular/core';
import { StorageService } from './storage.service';
import { TiTreeNode, TiMessageService, TiTreeComponent } from '@cloud/tiny3';
import { dynamicComponent } from './components/dynamicComponent';
import { StorageTree, RaidNode, VolumeNode, DriverNode, VolumeCreateModel, VolumeStaticModel } from './models';
import { TranslateService } from '@ngx-translate/core';
import { TpHalfmodalComponent } from '@cloud/tinyplus3';
import { VolumeCreateComponent } from './components/volume-create/volume-create.component';
import { LoadingService, UserInfoService, AlertMessageService, SystemLockService } from 'src/app/common-app/service';
import { StorageCollectLogService } from 'src/app/common-app/service/storage-collectlog.service';
import { getMessageId, errorTip } from 'src/app/common-app/utils';


@Component({
    selector: 'app-storage',
    templateUrl: './storage.component.html',
    styleUrls: ['./storage.component.scss']
})
export class StorageComponent implements OnInit, OnDestroy {

    @ViewChild('storageContainer', { static: false, read: ViewContainerRef }) componentContainer: ViewContainerRef;
    @ViewChild('createVolume', { static: false, read: ViewContainerRef }) createVolume: ViewContainerRef;
    @ViewChild('target', { static: true }) target: ElementRef;
    @ViewChild('modal', { static: true }) halfmodal: TpHalfmodalComponent;


    public cmpRef: any;
    public storageTree: StorageTree;
    public showData: TiTreeNode[];
    public systemLocked: boolean = true;
    public storeTip: string = this.i18n.instant('STORE_TIP');

    constructor(
        private storageService: StorageService,
        private componentFactoryResolver: ComponentFactoryResolver,
        private loading: LoadingService,
        private i18n: TranslateService,
        public user: UserInfoService,
        private alert: AlertMessageService,
        private tiMessage: TiMessageService,
        private lockService: SystemLockService,
        private storageCollectLogService: StorageCollectLogService,
    ) {
        this.showData = [];
    }

    displayComponent(node: TiTreeNode) {
        const raidNode = (node.getRaidIndex || node.getRaidIndex === 0) ?
            this.storageTree.getParentRaid(node.getRaidIndex) : null;
        if (raidNode && raidNode.getMultiple) {
            return null;
        }
        // 选中节点
        this.storageTree.checkedNode(node.labelId, node.getRaidIndex);

        // 清空容器
        if (this.componentContainer) {
            this.componentContainer.clear();
        }
        // 加载组件
        const name = node.componentName;
        const component = this.componentFactoryResolver.resolveComponentFactory(
            dynamicComponent[name]
        );
        this.cmpRef = this.componentContainer.createComponent(component);

        // 如果加载的是控制器组件，则每次切换时都改变 PMC 卡的关联逻辑盘列表数据
        if (VolumeStaticModel.logicDriverList[node.label]) {
            VolumeStaticModel.logicDriverList.maxCacheCount = VolumeStaticModel.logicDriverList[node.label].maxCacheCount;
        }
    }

    public raidButtonClick(id: string, halfmodal: TpHalfmodalComponent, node: RaidNode, event?): void {
        event.stopPropagation();
        switch (id) {
            case 'add':
                this.addVolume(halfmodal, node);
                break;
            case 'edit':
                this.editRaid(node);
                break;
            case 'delete':
                this.deleteBatchVolumn();
                break;
            case 'cancel':
                this.cancelEdit(node);
                break;
            default:
                break;
        }
    }

    private addVolume(halfmodal: TpHalfmodalComponent, node: RaidNode) {
        // 选中节点
        this.displayComponent(node);
        // 通过组件打开半屏弹窗
        halfmodal.show(this.target.nativeElement);

        if (this.createVolume) {
            this.createVolume.clear();
        }
        const component = this.componentFactoryResolver.resolveComponentFactory(VolumeCreateComponent);
        const addVolumeCpy: ComponentRef<any> = this.createVolume.createComponent(component);
        addVolumeCpy.instance.selectNode = node;
        addVolumeCpy.instance.close.subscribe((value) => {
            VolumeCreateModel.destroy();
            this.halfmodal.hide();
            addVolumeCpy.destroy();
            if (value) {
                // 创建逻辑盘后重新刷新StorageTree
                StorageTree.destroy();
                this.ngOnInit();
            }
        });
    }

    private editRaid(node: RaidNode) {
        this.storageTree.multipleChange(node);
        node.expanded = true;
        node.setAddButShow = false;
        node.setEditButShow = false;
        node.setCancleButShow = true;
        node.setDelButShow = true;
    }

    private cancelEdit(node: RaidNode) {
        node.setMultiple = false;
        node.setAddButShow = true;
        node.setEditButShow = true;
        node.setCancleButShow = false;
        node.setDelButShow = false;
        this.storageTree.changeDisable();
    }

    public deleteVolume(selectedData: TiTreeNode[]) {
        this.loading.state.next(true);
        this.storageService.deleteBatchVolumn(selectedData).subscribe(
            () => {
                this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            },
            (error) => {
                const msgId = error.error ? getMessageId(error.error)[0].errorId : 'COMMON_FAILED';
                this.alert.eventEmit.emit(errorTip(msgId, this.i18n));
            },
            () => {
                this.loading.state.next(false);
                StorageTree.destroy();
                this.ngOnInit();
            }
        );
    }

    public deleteBatchVolumn() {
        // 获取当前选中项，参数：树节点数据，选中项是否只包含叶子节点，是否多选
        const selectedData: TiTreeNode[] = this.storageTree.getSelectVolume();
        if (!selectedData || selectedData.length < 1) {
            return null;
        }
        const deleteInstance = this.tiMessage.open({
            id: 'deleteBatchVolumeMessage',
            type: 'prompt',
            title: this.i18n.instant('COMMON_CONFIRM'),
            content: this.i18n.instant('STORE_DELETE_CONTINUE'),
            okButton: {
                text: this.i18n.instant('COMMON_OK'),
                autofocus: false,
                click: () => {
                    this.deleteVolume(selectedData);
                    deleteInstance.close();
                }
            },
            cancelButton: {
                text: this.i18n.instant('COMMON_CANCEL')
            }
        });
    }

    public deleteOneVolumn(node: VolumeNode, event?) {
        event.stopPropagation();
        const msgInstance = this.tiMessage.open({
            id: 'deleteOneVolumeMessage',
            type: 'prompt',
            title: this.i18n.instant('COMMON_CONFIRM'),
            content: this.i18n.instant('STORE_CONFIRM_CONTINUE'),
            okButton: {
                text: this.i18n.instant('COMMON_OK'),
                autofocus: false,
                click: () => {
                    const selectedData: TiTreeNode[] = [node];
                    this.deleteVolume(selectedData);
                    msgInstance.close();
                }
            },
            cancelButton: {
                text: this.i18n.instant('COMMON_CANCEL')
            }
        });
    }

    public cancelCreate() {
        this.halfmodal.hide();
    }

    public entryDrive(node: DriverNode, event?) {
        event.stopPropagation();
        const entryDriveInstance = this.tiMessage.open({
            id: 'entryDriveMessage',
            type: 'prompt',
            title: this.i18n.instant('COMMON_CONFIRM'),
            content: this.i18n.instant('STORE_CONFIRM_CONTINUE'),
            okButton: {
                text: this.i18n.instant('COMMON_OK'),
                autofocus: false,
                click: () => {
                    this.loading.state.next(true);
                    this.storageService.entryDrive(node.id).subscribe(
                        () => {
                            this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                        },
                        (error) => {
                            const msgId = error?.error ? getMessageId(error.error)[0]?.errorId : 'COMMON_FAILED';
                            this.alert.eventEmit.emit(errorTip(msgId, this.i18n));
                            this.loading.state.next(false);
                        },
                        () => {
                            this.loading.state.next(false);
                        }
                    );
                    entryDriveInstance.close();
                }
            },
            cancelButton: {
                text: this.i18n.instant('COMMON_CANCEL')
            }
        });

    }

    public beforeExpand(treeCom: TiTreeComponent): void {
        // 获取当前点击节点
        const item: TiTreeNode = treeCom.getBeforeExpandNode();
        if (item.componentName === 'VolumeNode') {
            this.displayComponent(item);
        }
        // 将该节点展开
        item.expanded = true;
    }

    ngOnInit() {
        this.loading.state.next(true);
        this.storageService.factory().subscribe(
            ({stroageTreeFoliages, bmaEnabled}) => {
                if (stroageTreeFoliages) {
                    this.storageTree = stroageTreeFoliages;
                    this.showData = this.storageTree.getFoliages;
                    this.storageService.storageTree = this.storageTree;
                    this.displayComponent(this.storageTree.getFoliages[0]);
                }
                this.storeTip = bmaEnabled === 1 ? this.i18n.instant('STORE_TIP') : this.i18n.instant('STORE_TIP_SMSENABLE');
            },
            () => { },
            () => {
                this.loading.state.next(false);
            }
        );
        this.systemLocked = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.systemLocked = res;
        });
        this.storageCollectLogService.changeState.subscribe((res: any) => {
            const list = this.storageTree.getFoliages;
            list.forEach((item: any) => {
                if (item.id === res.key) {
                    item.raidButton.button.forEach((ele: any) => {
                        ele.disable =  res.value;
                    });
                }
            });
        });
    }

    ngOnDestroy(): void {
        if (this.cmpRef) {
            this.cmpRef.destroy();
        }
        StorageTree.destroy();
    }

}
