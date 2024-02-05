import { Component, OnInit, Input } from '@angular/core';
import { TiTableColumns, TiTableRowData, TiTableSrcData, TiMessageService } from '@cloud/tiny3';
import { TipParams, DataDetail, PowerOnOffModel, ACTIONTYPE, SOLTTYPE } from '../../models';
import { PowerOnOffService } from '../../services/power-on-off.service';
import { TranslateService } from '@ngx-translate/core';
import { Observable } from 'rxjs/internal/Observable';

@Component({
    selector: 'app-calculate-node',
    templateUrl: './calculate-node.component.html',
    styleUrls: ['./calculate-node.component.scss']
})
export class CalculateNodeComponent implements OnInit {
    @Input() nodeSrcData: TiTableSrcData;
    @Input() btnControl: boolean;
    constructor(
        private tiMessage: TiMessageService,
        private service: PowerOnOffService,
        private translate: TranslateService,
    ) { }
    public models = new PowerOnOffModel();
    public nodeDisplayed: TiTableRowData[] = [];
    public nodeCheckedList: DataDetail[] = [];
    public nodeColumns: TiTableColumns[];
    public btnList: any = [];
    public action: any = 'GracefulShutdown';
    public resetTips: string = '';
    public operateCount: number = 0;
    public operateSuccessCount: number = 0;

    ngOnInit(): void {
        this.btnList = this.models.btnList;
        this.nodeColumns = this.models.nodeColumns;
    }

    /**
     * 为了更好的渲染性能，建议在ngFor数据时总是trackBy数据条目中的某个唯一属性值 Id
     * @param index 索引
     * @param item 当前元素
     */
    public trackByFn(index: number, item: any): number {
        return item.Id;
    }

    /**
     * 处理单板执行不同按钮操作
     * @param string
     */
    public changeStatus(action: string) {
        this.action = action;
        this.BladeResetTips();
    }

    /**
     * 至少选择一个单板
     * @param action 执行操作类型
     * @param checkList 选中的单板列表
     */
    public BladeResetTips() {
        const checkLength = this.nodeCheckedList.length;
        if (checkLength < 1) {
            const params: TipParams = {
                id: 'chooseLeastOne',
                type: 'prompt',
                title: this.translate.instant('COMMON_INFORMATION'),
                content: this.translate.instant('POWERONOFF_CHOOSE_LEAST'),
                okShow: true,
                cancalShow: false
            };
            this.showTips(params);
            return false;
        }

        const instances: TipParams = {
            id: 'checkTips',
            type: 'prompt',
            title: this.translate.instant('ALARM_OK'),
            content: this.translate.instant('POWERONOFF_GO_CHECK'),
            okShow: true,
            cancalShow: true
        };
        this.showTips(instances, true);
    }

    /**
     * 点击操作按钮，根据参数显示提示弹框
     * @param params 弹框相关参数配置
     * @param operate 执行数据处理方法
     */
    public showTips(params: any, operate?: boolean) {
        const tipModel = this.tiMessage.open({
            id: params.id,
            closeIcon: false,
            type: params.type,
            okButton: {
                show: params.okShow,
                text: this.translate.instant('COMMON_OK'),
                autofocus: false,
                click: () => {
                    tipModel.close();
                    if (operate) {
                        this.handleBoardReset();
                    }
                }
            },
            cancelButton: {
                show: params.cancalShow,
                text: this.translate.instant('ALARM_CANCEL'),
            },
            title: params.title,
            content: params.content,
        });
    }

    /**
     * 将选中单板列表根据执行操作进行分类，可执行操作单板请求接口，不可执行操作单板进行error提示
     */
    public handleBoardReset() {
        this.service.notAllowTips(this.getOperateArr().notAllow);
        const operateArr = this.getOperateArr().allow;
        this.operateCount = operateArr.length;
        this.getFailTips(operateArr).subscribe((res) => {
            if (res.length > 0) {
                this.service.errorTips('error', res, 'POWERONOFF_SERUP_FAILED');
            }
        });
        this.nodeCheckedList = operateArr;
    }

    /**
     * 获取所有操作失败的单板，返回字符串列表
     * @param operateArr 可操作单板列表
     */
    public getFailTips(operateArr): Observable<string> {
        let failedTips = '';
        const promiseArr = [];
        operateArr.forEach((item) => {
            const promise = this.request(item);
            promiseArr.push(promise);
        });
        return new Observable(observer => {
            Promise.all(promiseArr).then(res => {
                res.forEach(item => {
                    failedTips += item + this.translate.instant('POWERONOFF_COMMA');
                });
                observer.next(failedTips);
            }).catch((error) => {
                observer.next(error);
            });
        });
    }

    /**
     * 返回不能修改状态的单板soltId
     * @param item 当前单板
     */
    public request(item): Promise<string> {
        return new Promise((resolve, reject) => {
            this.service.resetStatus(item.Id, this.action).subscribe((res) => {
                this.findBladeState(item.Id, item.slotId);
            }, (err) => {
                this.operateCount++;
                resolve(item.slotId);
            });
        });
    }

    /**
     * 获取可执行操作单板列表，不可操作列表
     */
    public getOperateArr(): { allow: DataDetail[], notAllow: DataDetail[] } {
        // 过滤出 下电状态做非上电操作 或者通讯异常 以及不被管理的单板
        const tempArr = this.nodeCheckedList.filter((item: any) => {
            return (item.HotswapState === 1 && (this.action !== ACTIONTYPE.ON)) || item.HotswapState === 7 || item.HotswapState === 8;
        });
        const operateArr = [...this.nodeCheckedList].filter(item => [...tempArr].every(item1 => item1 !== item));
        return { allow: operateArr, notAllow: tempArr };
    }

    /**
     * 可执行操作单板请求接口，重置状态时获取当前单板的状态信息
     * @param id 单板Id
     */
    public findBladeState(id: string, slotId: string) {
        this.service.getSystemsPower(id).subscribe((res) => {
            const oemblade = this.service.getOemData(res.body)?.PresentFrus;
            if (!oemblade) { return; }
            if (oemblade.length > 0) {
                const bladeState = oemblade[oemblade.length - 1].PowerState;
                const boardIndex = this.nodeSrcData.data.findIndex((item) => {
                    return item.Id === id;
                });
                const currentBoard = this.nodeSrcData.data[boardIndex];
                const soltState = res.body.PowerState;
                currentBoard.HotswapState = SOLTTYPE[bladeState];
                switch (bladeState) {
                    case 'On':
                        this.changeBladeState(slotId, soltState, boardIndex);
                        break;
                    case 'Off':
                        if (this.action === ACTIONTYPE.FORCERESTART || this.action === ACTIONTYPE.FORCEPOWERCYCLE) {
                            // 当做强制下电再上电操作时，需要展示中间的图片
                            if (this.action === ACTIONTYPE.FORCEPOWERCYCLE && currentBoard.Id === id) {
                                currentBoard.image = 'assets/emm-assets/image/hardware/powerOff.png';
                            }
                            this.findBladeState(id, slotId);
                        } else {
                            this.changeBladeState(slotId, soltState, boardIndex);
                        }
                        break;
                    case 'PoweringOn':
                    case 'PoweringOff':
                        this.findBladeState(id, slotId);
                        break;
                    default:
                }
            }
        });
    }

    /**
     * 根据单板信息改变单板状态，并提示结果
     * @param id 单板Id
     * @param slotId 单板槽位号
     * @param soltState 操作之前的状态
     * @param index 修改单板的索引
     */
    public changeBladeState(slotId: string, soltState: string, index: number) {
        const currentBoard = this.nodeSrcData.data[index];
        if (this.action === ACTIONTYPE.FORCEOFF || this.action === ACTIONTYPE.GRACEFULSHUTDOWN) {
            this.resetTips += slotId + this.translate.instant('POWERONOFF_COMMA');
            currentBoard.HotswapState = 1;
            currentBoard.status = 'POWERONOFF_BEEN_OFF';
            currentBoard.image = 'assets/emm-assets/image/hardware/powerOff.png';
        } else {
            this.resetTips += slotId + this.translate.instant('POWERONOFF_COMMA');
            currentBoard.HotswapState = 4;
            currentBoard.status = 'POWERONOFF_BEEN_ON';
            currentBoard.image = 'assets/emm-assets/image/hardware/powerUp.png';
        }
        this.operateSuccessCount++;
        // 所有请求都完成后统一做提示
        if (this.operateSuccessCount === this.operateCount) {
            if (this.action === ACTIONTYPE.FORCEOFF || this.action === ACTIONTYPE.GRACEFULSHUTDOWN) {
                this.service.errorTips('success', this.resetTips, 'POWERONOFF_OFF_SUCCESSFUL');
            }
            if (this.action === ACTIONTYPE.ON || (this.action === ACTIONTYPE.FORCEPOWERCYCLE && soltState === 'On')) {
                this.service.errorTips('success', this.resetTips, 'POWERONOFF_ON_SUCCESSFUL');
            }
            if ((this.action === ACTIONTYPE.FORCERESTART && soltState === 'On' || this.action === ACTIONTYPE.NMI)) {
                this.service.errorTips('success', this.resetTips, 'COMMON_SUCCESS');
            }
            // 提示完成后清空提示
            this.operateSuccessCount = 0;
            this.resetTips = '';
        }
        // 修改成功后清空选中项
        this.nodeCheckedList = [];
    }
}
