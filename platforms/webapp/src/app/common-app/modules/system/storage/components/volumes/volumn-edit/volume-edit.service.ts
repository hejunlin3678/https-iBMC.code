import { Injectable } from '@angular/core';
import { IVolumeEditData, StorageTree, VolumeEditModel } from '../../../models';
import { deleteEmptyProperty } from '../../../utils';
import { HttpService } from 'src/app/common-app/service';
import { interNameArr } from 'src/app/common-app/models';

@Injectable({
    providedIn: 'root'
})
export class VolumeEditService {

    constructor(
        private http: HttpService
    ) { }

    private save(dataCopy: any, data: IVolumeEditData) {
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
}
