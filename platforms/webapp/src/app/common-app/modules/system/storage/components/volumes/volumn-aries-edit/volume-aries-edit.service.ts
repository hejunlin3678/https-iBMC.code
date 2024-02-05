import { Injectable } from '@angular/core';
import { IVolumeAriesEditData, StorageTree } from '../../../models';
import { deleteEmptyProperty } from '../../../utils';
import { HttpService } from 'src/app/common-app/service';
import { VolumeAriesEditModel } from '../../../models/volume/volume-aries-edit.model';
import { interNameArr } from 'src/app/common-app/models';

@Injectable({
    providedIn: 'root'
})
export class VolumeAriesEditService {

    constructor(
        private http: HttpService
    ) { }

    private save(dataCopy: any, data: IVolumeAriesEditData) {
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
        let name = null;
        if (obj.hasOwnProperty('Name')) {
            name = obj['Name'];
        }
        const param = deleteEmptyProperty(obj);
        // 名称为空时增加参数
         if (name === '') {
            param['Name'] = name;
         }
        return (param && JSON.stringify(param) !== '{}') ? param : null;
    }
    public updateEditData() {
        const node = StorageTree.getInstance().getCheckedNode;
        const param = this.save(VolumeAriesEditModel.getInstance().getOriginData, VolumeAriesEditModel.getInstance().getSelectData);
        return this.http.patch(node.url, param);
    }
}
