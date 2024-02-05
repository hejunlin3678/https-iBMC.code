import { Injectable } from '@angular/core';
import { TiTreeUtil } from '@cloud/tiny3';
import { IOptions, interNameArr } from 'src/app/common-app/models';
import { RaidNode, DriverNode, IDriverEditData, StorageTree, DriverEditModel, VolumeNode } from '../../../models';
import { deleteEmptyProperty } from '../../../utils';
import { HttpService } from 'src/app/common-app/service';
import { TranslateService } from '@ngx-translate/core';

@Injectable({
    providedIn: 'root'
})
export class DriverEditService {

    constructor(
        private i18n: TranslateService,
        private http: HttpService
    ) { }

    public getLoginOptions() {
        const selectedNode = TiTreeUtil.getSelectedData(
            StorageTree.getInstance().getFoliages, false, true
        )[0] as DriverNode;
        const raidNode = selectedNode.parent[0] as RaidNode;
        const options: IOptions[] = [];
        const logicSelect: IOptions[] = [];
        raidNode.children.forEach((children) => {
            if (children.componentName && children.componentName === 'VolumeNode') {
                const volumeObj = children as VolumeNode;
                const hotSpareList = volumeObj.children?.filter(item => item.className === 'hotSpareClass') || [];
                if (volumeObj.getRaidLevel !== 'RAID0') {
                    const option = {
                        name: 'IOptions',
                        id: children.url,
                        label: children.label,
                        disabled: false
                    };
                    options.push(option);
                    if (volumeObj.getDriverNode.includes(selectedNode.label)) {
                        logicSelect.push(option);
                    } else if (hotSpareList.length > 0) {
                        hotSpareList[0].children.forEach(item => {
                            if (item?.label === selectedNode.label) {
                                logicSelect.push(option);
                            }
                        });
                    }
                }
            }
        });
        if (options.length === 0) {
            options.push({
                name: 'IOptions',
                id: `noData`,
                label: this.i18n.instant('COMMON_NO_DATA')
            });
            logicSelect.push({
                name: 'IOptions',
                id: `noData`,
                label: this.i18n.instant('COMMON_NO_DATA')
            });
        }
        return { options, logicSelect };
    }


    private save(dataCopy: any, data: IDriverEditData) {
        const obj = {};
        for (const attr in data) {
            if (JSON.stringify(data[attr]) !== JSON.stringify(dataCopy[attr])) {
                const key: string = attr.substring(0, 1).toUpperCase() + attr.substring(1);
                let value: any = null;
                if (data[attr] && data[attr].name && interNameArr.indexOf(data[attr].name) > -1) {
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
        const param = this.save(DriverEditModel.getInstance().getOriginData, DriverEditModel.getInstance().getSelectData);
        // 当设置热备，不能下发固件状态
        if (Object.prototype.hasOwnProperty.call(param, 'HotspareType') &&
            Object.prototype.hasOwnProperty.call(param, 'FirmwareStatus')) {
                delete param['FirmwareStatus'];
        }
        const spareforLogical = DriverEditModel.getInstance().getSelectData.spareforLogicalDrive;
        // 如果下发局部热备，一定要下发逻辑盘选项
        if (Object.prototype.hasOwnProperty.call(param, 'HotspareType') && param['HotspareType'] === 'Dedicated') {
            if (spareforLogical instanceof Array) {
                param['SpareforLogicalDrive'] = spareforLogical[0].id;
            } else {
                param['SpareforLogicalDrive'] = spareforLogical.id;
            }
        }
        // 如果下发自动替换，值需加空格
        if (Object.prototype.hasOwnProperty.call(param, 'HotspareType') && param['HotspareType'] === 'AutoReplace') {
            param['HotspareType'] = 'Auto Replace';
        }
        // 如果下发逻辑盘选项,需拼接多个选项的label
        if (Object.prototype.hasOwnProperty.call(param, 'SpareforLogicalDrive')) {
            const spareforLogicalDrive = [];
            if (spareforLogical instanceof Array) {
                spareforLogical.forEach(item => {
                    spareforLogicalDrive.push(item.label);
                });
                param['SpareforLogicalDrive'] = spareforLogicalDrive;
            } else {
                param['SpareforLogicalDrive'] = [spareforLogical.label];
            }
        }
        return this.http.patch(node.url, param);
    }

}
