import { Injectable } from '@angular/core';
import { IVolumeSPEditData, StorageTree } from '../../../models';
import { deleteEmptyProperty } from '../../../utils';
import { HttpService } from 'src/app/common-app/service';
import { VolumeSPEditModel } from '../../../models/volume/volume-sp-edit.model';
import { interNameArr } from 'src/app/common-app/models';
import { Observable } from 'rxjs';

@Injectable({
  providedIn: 'root',
})
export class VolumeSPEditService {
  constructor(private http: HttpService) {}

  private save(dataCopy: any, data: IVolumeSPEditData): {} {
    const obj = {};
    for (const attr in data) {
      if (JSON.stringify(data[attr]) !== JSON.stringify(dataCopy[attr])) {
        const key: string =
          attr.substring(0, 1).toUpperCase() + attr.substring(1);
        let value: any = null;
        if (attr === 'name' && data[attr]) {
          data[attr] = data[attr].trim();
        }
        if (
          data[attr] &&
          data[attr].name &&
          interNameArr.indexOf(data[attr].name) > -1
        ) {
          value = data[attr].id;
        } else {
          value = data[attr];
        }
        obj[key] = value;
      }
    }
    let name = null;
    if (Object.prototype.hasOwnProperty.call(obj, 'Name')) {
      name = (obj as any)?.Name;
    }
    const param: any = deleteEmptyProperty(obj);
    // 名称为空时增加参数
    if (name === '') {
      param.Name = name;
    }
    return param && JSON.stringify(param) !== '{}' ? param : null;
  }
  public updateEditData(): Observable<any> {
    const node = StorageTree.getInstance().getCheckedNode;
    const param = this.save(
      VolumeSPEditModel.getInstance().getOriginData,
      VolumeSPEditModel.getInstance().getSelectData
    );
    return this.http.patch(node.url, param);
  }
}
