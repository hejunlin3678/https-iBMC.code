import { Injectable } from '@angular/core';
import { IVolumeEditData, IVolumePMCEditData, StorageTree, VolumeEditModel } from '../../../models';
import { deleteEmptyProperty } from '../../../utils';
import { HttpService } from 'src/app/common-app/service';
import { VolumePMCEditModel } from '../../../models/volume/volume-pmc-edit.model';
import { interNameArr } from 'src/app/common-app/models';

@Injectable({
    providedIn: 'root'
})
export class VolumeEditService {

    constructor(
        private http: HttpService
    ) { }

    private save(dataCopy: any, data: IVolumeEditData | IVolumePMCEditData): any {
        const obj = {};
        for (const attr in data) {
            if (JSON.stringify(data[attr]) !== JSON.stringify(dataCopy[attr])) {
                const key: string = attr.substring(0, 1).toUpperCase() + attr.substring(1);
                let value: any = null;
                if (attr === 'name' && data[attr]) {
                    data[attr] = data[attr].trim();
                }
                if (data[attr] && data[attr].name && interNameArr.indexOf(data[attr].name) > -1) {
                    value = data[attr].id;
                } else {
                    value = data[attr];
                }
                obj[key] = value;
            }
        }
        const param = deleteEmptyProperty(obj);
        return (param && JSON.stringify(param) !== '{}') ? param : null;
    }
    public updateEditData() {
        const node = StorageTree.getInstance().getCheckedNode;
        const param = this.save(VolumeEditModel.getInstance().getOriginData, VolumeEditModel.getInstance().getSelectData);
        return this.http.patch(node.url, param);
    }

    public updatePMCEditData() {
        const node = StorageTree.getInstance().getCheckedNode;
        const param = this.save(VolumePMCEditModel.getInstance().getOriginData, VolumePMCEditModel.getInstance().getSelectData);
        // 若有容量单位单位变化或有容量变化，则将容量数据也带到发送参数中
        if (param.CapacityUnit || param.CapacityBytes) {
            const unitType = {'MB': 1, 'GB': 1024, 'TB': 1024 * 1024};
            let capacityBytes = param.CapacityBytes || VolumePMCEditModel.getInstance().getSelectData.capacityBytes;
            const unit = param.CapacityUnit || VolumePMCEditModel.getInstance().getSelectData.capacityUnit;
            capacityBytes = Math.floor(parseFloat(capacityBytes) * unitType[unit.id || unit]);
            delete param.CapacityBytes;
            delete param.CapacityUnit;
            param.CapacityMBytes = capacityBytes;
        }
        return this.http.patch(node.url, param);
    }
}
