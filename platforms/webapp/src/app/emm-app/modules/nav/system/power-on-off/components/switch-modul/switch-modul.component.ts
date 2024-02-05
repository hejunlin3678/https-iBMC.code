import { Component, OnInit, Input } from '@angular/core';
import { TiTableColumns, TiTableRowData, TiTableSrcData, TiMessageService } from '@cloud/tiny3';
import { TipParams, DataDetail, PowerOnOffModel, SOLTTYPE, ACTIONTYPE } from '../../models';
import { PowerOnOffService } from '../../services/power-on-off.service';
import { TranslateService } from '@ngx-translate/core';
import { Observable } from 'rxjs/internal/Observable';

@Component({
    selector: 'app-switch-modul',
    templateUrl: './switch-modul.component.html',
    styleUrls: ['./switch-modul.component.scss']
})
export class SwitchModulComponent implements OnInit {
    @Input() modulSrcData: TiTableSrcData;
    @Input() btnControl: boolean;
    constructor(
        private tiMessage: TiMessageService,
        private service: PowerOnOffService,
        private translate: TranslateService,
    ) { }
    public models = new PowerOnOffModel();
    public modulDisplayed: TiTableRowData[] = [];
    public modulCheckedList: DataDetail[] = [];
    public modulColumns: TiTableColumns[];
    public btnList: any = [];
    public action: string = 'GracefulShutdown';
    public resetTips: string = '';

    ngOnInit(): void {
        this.btnList = this.models.btnList;
        this.modulColumns = this.models.modulColumns;
    }

    /**
     * 为了更好的渲染性能，建议在ngFor数据时总是trackBy数据条目中的某个唯一属性值 index
     * @param index 索引
     * @param item 当前元素，Swi可能有多个平面，列表存在Id重复
     */
    public trackByFn(index: number, item: any): number {
        return index;
    }

    /**
     * 处理单板执行不同按钮操作
     * @param action 操作类型
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
        const checkLength = this.modulCheckedList.length;
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
            content: this.translate.instant('POWERONOFF_SWI_CHECK'),
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
                        this.handleSwiReset();
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
     * 查询单板是否可修改状态，否则给与提示设置失败
     */
    public handleSwiReset() {
        this.service.notAllowTips(this.getOperateArr().notAllow);
        const operateArr = this.getOperateArr().allow;
        this.getFailTips(operateArr).subscribe((res) => {
            if (res.length > 0) {
                this.service.errorTips('error', res, 'POWERONOFF_SERUP_FAILED');
            }
        });
        this.modulCheckedList = operateArr;
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
     * 返回不能修改状态的单板soltId，可修改的请求接口
     * @param item 当前单板
     */
    public request(item): Promise<string> {
        const params = {
            FruControlType: this.action,
            FruID: item.panel
        };
        return new Promise((resolve, reject) => {
            this.service.resetSwiStatus(item.Id, params).subscribe((res) => {
                this.findSwiState(item);
            }, (err) => {
                resolve(item.slotId);
            });
        });
    }

    /**
     * 筛选出可操作的单板列表,不可操作列表
     */
    public getOperateArr(): { allow: DataDetail[], notAllow: DataDetail[] } {
        const tempArr = [];
        this.modulCheckedList.forEach(item => {
            if (item.panel === 0 ||
                (item.HotswapState === 1 && (this.action === ACTIONTYPE.FORCERESTART || this.action === ACTIONTYPE.GRACEFULSHUTDOWN)) ||
                (item.HotswapState === 0 && this.action !== ACTIONTYPE.ON)
            ) {
                tempArr.push(item);
            }
        });
        const operateArr = [...this.modulCheckedList].filter(item => [...tempArr].every(item1 => item1 !== item));
        return { allow: operateArr, notAllow: tempArr };
    }

    /**
     * 单板状态修改接口请求，根据返回状态进一步处理
     * @param swiObj 选中的单板参数
     */
    public findSwiState(swiObj: any) {
        this.service.getSystemsPower(swiObj.Id).subscribe((res) => {
            const oem = this.service.getOemData(res.body)?.PresentFrus;
            if (!oem) { return; }
            const oemSwi = oem.filter((item) => {
                return item.FruType !== 'OS';
            });
            if (oemSwi.length > 0) {
                const swiState = oem[oem.length - 1].PowerState;
                this.modulSrcData.data.forEach((item, index) => {
                    if (item.Id === swiObj.Id) {
                        item.HotswapState = SOLTTYPE[swiState];
                        switch (swiState) {
                            case 'On':
                                this.changeSwiState(swiObj.Id, item.powerFlat, index);
                                break;
                            case 'Off':
                                this.changeSwiState(swiObj.Id, item.powerFlat, index);
                                if (this.action === ACTIONTYPE.FORCERESTART) {
                                    this.findSwiState(swiObj);
                                }
                                break;
                            case 'PoweringOn':
                            case 'PoweringOff':
                                this.findSwiState(swiObj);
                                break;
                            default:
                        }
                    }
                });
            }
        });
    }

    /**
     * 修改单板状态,提示修改结果
     * @param id 单板Id
     * @param powerFlat 单板平面
     * @param index 当前操作单板索引
     */
    public changeSwiState(id: string, powerFlat: string, index: number) {
        const currentSwi = this.modulSrcData.data[index];
        if (this.action === ACTIONTYPE.GRACEFULSHUTDOWN) {
            this.resetTips += id + ' ' + powerFlat + this.translate.instant('POWERONOFF_COMMA');
            currentSwi.HotswapState = 1;
            currentSwi.status = 'POWERONOFF_BEEN_OFF';
            currentSwi.image = 'assets/emm-assets/image/hardware/powerOff.png';
        } else {
            this.resetTips += id + ' ' + powerFlat + this.translate.instant('POWERONOFF_COMMA');
            currentSwi.HotswapState = 4;
            currentSwi.status = 'POWERONOFF_BEEN_ON';
            currentSwi.image = 'assets/emm-assets/image/hardware/powerUp.png';
        }
        if (this.action === ACTIONTYPE.GRACEFULSHUTDOWN) {
            this.service.errorTips('success', this.resetTips, 'POWERONOFF_OFF_SUCCESSFUL');
        }
        if (this.action === ACTIONTYPE.ON) {
            this.service.errorTips('success', this.resetTips, 'POWERONOFF_ON_SUCCESSFUL');
        }
        if (this.action === ACTIONTYPE.FORCERESTART) {
            this.service.errorTips('success', this.resetTips, 'COMMON_SUCCESS');
        }
    }
}
