import { Injectable } from '@angular/core';
import { interNameArr } from 'src/app/common-app/models';
import { HttpService } from 'src/app/common-app/service';
import { IRaidEditData, RaidEditModel, StorageTree } from '../../../models';
import { deleteEmptyProperty } from '../../../utils';

@Injectable({
    providedIn: 'root'
})
export class RaidEditService {

    constructor(
        private http: HttpService
    ) { }

    private save(dataCopy: any, data: IRaidEditData): any {
        const obj = {};
        for (const attr in data) {
            if (JSON.stringify(data[attr]) !== JSON.stringify(dataCopy[attr])) {
                const key: string = attr.substring(0, 1).toUpperCase() + attr.substring(1);
                let value: any = null;
                if (data[attr] && typeof data[attr] === 'object' && interNameArr.indexOf(data[attr].name) > -1) {
                    value = data[attr].id;
                } else {
                    value = data[attr];
                }
                obj[key] = value;
            }
        }
        deleteEmptyProperty(obj);
        return (obj && JSON.stringify(obj) !== '{}') ? obj : null;
    }
    public updateEditData() {
        const node = StorageTree.getInstance().getCheckedNode;
        const param = this.save(RaidEditModel.getInstance().getOriginData, RaidEditModel.getInstance().getSelectData);
        // 启动盘配置
        if (param && (param.BootDevice1 || param.BootDevice2)) {
            const device1 = RaidEditModel.getInstance().getSelectData.bootDevice1;
            const device2 = RaidEditModel.getInstance().getSelectData.bootDevice2;
            param.BootDevices = [device1.label, device2?.label || 'None'];
            delete param.BootDevice1;
            delete param.BootDevice2;
        }
        // 硬盘写缓存策略配置
        if (param.WriteCacheType || param.WriteCachePolicy) {
            const type = RaidEditModel.getInstance().getSelectData.writeCacheType;
            const policy = RaidEditModel.getInstance().getSelectData.writeCachePolicy;
            param.WriteCachePolicy = {};
            param.WriteCachePolicy[type.id as string] = policy.id;
            delete param.WriteCacheType;
        }

        if (RaidEditModel.getInstance().getSelectData.enabled) {
            param.ConsisCheckInfo = {};
            if (param.Enabled !== undefined) {
                param.ConsisCheckInfo.Enabled = param.Enabled;
                delete param.Enabled;
            }

            if (param.AutoRepairEnabled !== undefined) {
                param.ConsisCheckInfo.AutoRepairEnabled = param.AutoRepairEnabled;
                delete param.AutoRepairEnabled;
            }

            if (param.Rate) {
                param.ConsisCheckInfo.Rate = param.Rate;
                delete param.Rate;
            }

            if (param.Period) {
                param.ConsisCheckInfo.Period = param.Period;
                delete param.Period;
            }

            if (param.DelayForStart || param.DelayForStart === 0) {
                param.ConsisCheckInfo.DelayForStart = Number(param.DelayForStart);
                delete param.DelayForStart;
            }
            if (JSON.stringify(param.ConsisCheckInfo) === '{}') {
                delete param.ConsisCheckInfo;
            }
        } else if (RaidEditModel.getInstance().getSelectData.enabled !== RaidEditModel.getInstance().getOriginData.enabled) {
            param.ConsisCheckInfo = {
                Enabled: RaidEditModel.getInstance().getSelectData.enabled
            };
            delete param.Enabled;
        }
        return this.http.patch(node.url, param);
    }

    public reset() {
        const node = StorageTree.getInstance().getCheckedNode;
        const url = `/UI/Rest/System/Storage/${node.id}/RestoreDefaultSettings`;
        return this.http.post(url, {});
    }
    // 	导入Foreign配置
    public foreignConfig() {
        const node = StorageTree.getInstance().getCheckedNode;
        const url = `/UI/Rest/System/Storage/${node.id}/ImportForeignConfig`;
        return this.http.post(url, {});
    }
    // 	清除Foreign配置
    public clearForeignConfig() {
        const node = StorageTree.getInstance().getCheckedNode;
        const url = `/UI/Rest/System/Storage/${node.id}/ClearForeignConfig`;
        return this.http.post(url, {});
    }
}
