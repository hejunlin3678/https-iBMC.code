import { Injectable } from '@angular/core';
import { DataDetail, SOLTTYPE, DETAILTYPE } from '../models/powerOnOff';
import { Observable } from 'rxjs/internal/Observable';
import { TranslateService } from '@ngx-translate/core';
import { HttpService, AlertMessageService } from 'src/app/common-app/service';

@Injectable({
    providedIn: 'root'
})
export class PowerOnOffService {
    constructor(
        private http: HttpService,
        private alertMessage: AlertMessageService,
        private translate: TranslateService,
    ) { }

    /**
     * 接口获取计算节点数据
     */
    public getNodeData(): Observable<any> {
        return this.http.get('/redfish/v1/chassisoverview', {type: 'redfish'});
    }

    /**
     * 获取交换模块单板数据
     * @param id 单板Id
     */
    public getModulData(id: string): Observable<any> {
        const url = `/redfish/v1/Systems/${id}`;
        return this.http.get(url, {type: 'redfish'});
    }

    /**
     * 修改单板状态请求
     * @param board 单板Id
     * @param action 执行动作
     */
    public resetStatus(board: string, action: string): Observable<any> {
        const url = `/redfish/v1/Systems/${board}/Actions/ComputerSystem.Reset`;
        const params = {
            ResetType: action
        };
        return this.http.post(url, params, {type: 'redfish'});
    }

    /**
     * 获取单板的相关信息
     * @param id 单板Id
     */
    public getSystemsPower(id: string): Observable<any> {
        const url = `/redfish/v1/Systems/${id}`;
        return this.http.get(url, {type: 'redfish'});
    }

    /**
     * 修改交换模块状态请求
     * @param board 单板Id
     * @param action 执行动作
     */
    public resetSwiStatus(swi: string, params: any): Observable<any> {
        const url = `/redfish/v1/Systems/${swi}/Actions/Oem/Huawei/ComputerSystem.FruControl`;
        return this.http.post(url, params, {type: 'redfish'});
    }


    /**
     * 初始化表格数据，分类
     * @param data 接口返回数据
     */
    public dataInit(data: any[]): { nodeData: DataDetail[], modulData: DataDetail[] } {
        const initNodeData = [];
        const initModulData = [];
        const modulData = [];
        data.forEach((item: any) => {
            const soltData: DataDetail = {};
            // 过滤计算节点数据
            if (item.Id.includes('Blade')) {
                soltData.Id = item.Id;
                soltData.slotId = item.Id.replace('Blade', 'Slot');
                soltData.HotswapState = this.getBoardType(item.HotswapState, item.ManagedByMM).HotswapState;
                soltData.status = this.getBoardType(item.HotswapState, item.ManagedByMM).status;
                soltData.image = this.getBoardType(item.HotswapState, item.ManagedByMM).image;
                initNodeData.push(soltData);
                // 过滤交换模块数据
            } else if (item.Id.includes('Swi')) {
                modulData.push(item);
            }
        });
        modulData.forEach((item: any) => {
            const swiData: DataDetail = {};
            // 因为通信丢失和不支持EMM管理的交换板没有平面 单独处理
            swiData.Id = item.Id;
            swiData.slotId = swiData.Id;
            swiData.HotswapState = item.HotswapState;
            if (item.HotswapState === 7 || item.ManagedByMM === false) {
                swiData.panel = 0;
                swiData.status = this.getBoardType(item.HotswapState, item.ManagedByMM).status;
                swiData.image = this.getBoardType(item.HotswapState, item.ManagedByMM).image;
                initModulData.push(swiData);
            } else {
                this.getModulData(item.Id).subscribe((res) => {
                    const oem = this.getOemData(res?.body)?.PresentFrus;
                    if (oem !== undefined || oem !== null) {
                        const presentFrus = oem.filter((el) => {
                            return el.FruType !== 'OS';
                        });
                        const frusLength = presentFrus.length;
                        if (frusLength > 0) {
                            presentFrus.forEach((element) => {
                                if (element.FruType === 'Base') {
                                    swiData.panel = 1;
                                    swiData.powerFlat = 'Base';
                                } else if (element.FruType === 'Fabric') {
                                    swiData.panel = 2;
                                    swiData.powerFlat = 'Fabric';
                                    swiData.slotId = frusLength >= 2 ? swiData.Id + ' Fabric' : swiData.Id;
                                } else {
                                    swiData.panel = 3;
                                    swiData.powerFlat = 'FC';
                                    swiData.slotId = frusLength >= 2 ? swiData.Id + ' FC' : swiData.Id;
                                }
                                swiData.status = this.getBoardType(element.PowerState).status;
                                swiData.image = this.getBoardType(element.PowerState).image;
                                initModulData.push(JSON.parse(JSON.stringify(swiData)));
                            });
                        }
                    }
                });
            }
        });
        return { nodeData: initNodeData, modulData: initModulData };
    }

    /**
     * 获取当前单板的具体图标、状态
     * @param hotswapState 单板状态
     * @param managedByMM 是否支持管理
     */
    public getBoardType(hotswapState: any, managedByMM?: boolean): DataDetail {
        const bladeDataSucc: DataDetail = {};
        if (managedByMM === false || managedByMM === null) {
            bladeDataSucc.HotswapState = 8;
            bladeDataSucc.status = 'POWERONOFF_NOT_MANAGED';
            bladeDataSucc.image = 'assets/emm-assets/image/hardware/verlow2.png';
        } else {
            const isSwi = typeof (hotswapState) === 'string';
            bladeDataSucc.HotswapState = isSwi ? SOLTTYPE[hotswapState] : hotswapState;
            const typeList = isSwi ? DETAILTYPE[hotswapState].split(' ') : DETAILTYPE[SOLTTYPE[hotswapState]].split(' ');
            bladeDataSucc.status = typeList[0];
            bladeDataSucc.image = `assets/emm-assets/image/hardware/${typeList[1]}.png`;
        }
        return bladeDataSucc;
    }

    /**
     * 根据不可操作单板列表做出命令不能执行提示
     * @param tempArr 不可操作单板列表
     */
    public notAllowTips(tempArr: DataDetail[]) {
        if (tempArr.length > 0) {
            let errotSlot = '';
            tempArr.forEach(item => {
                errotSlot += item.slotId + this.translate.instant('POWERONOFF_COMMA');
            });
            if (tempArr.length > 0) {
                this.errorTips('error', errotSlot, 'POWERONOFF_COMMAND');
            }
        }
    }

    /**
     * 根据操作异常的单板做提示
     * @param type 提示类型
     * @param msg 所需提示单板名称
     * @param errTip 提示内容
     */
    public errorTips(type: string, msg: string, errTip: string) {
        this.alertMessage.eventEmit.emit({
            type,
            label: msg.substr(0, msg.length - 1) + this.translate.instant(errTip)
        });
    }

    // 获取Oem下数据
    public getOemData(object) {
        let data = null;
        if (object && object.Oem) {
            data = Object.keys(object.Oem)[0];
            return object.Oem[data];
        }
        return null;
    }
}
