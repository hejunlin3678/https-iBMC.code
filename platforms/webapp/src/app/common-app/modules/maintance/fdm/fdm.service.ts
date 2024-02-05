import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { map } from 'rxjs/internal/operators/map';
import { FDMNode } from './classes/fdm-node';
import { displayDateTime } from './utils/fdm.utils';
import { BehaviorSubject } from 'rxjs/internal/BehaviorSubject';

@Injectable({
    providedIn: 'root'
})
export class FdmService {

    public activeNode$: BehaviorSubject<FDMNode>;
    public treeNode: FDMNode[];

    constructor(
        private http: HttpService,
    ) { }

    public factory() {
        const url = '/UI/Rest/Maintenance/FDM';
        return this.getData(url).pipe(
            map(([treeData]) => {
                let treeNode: FDMNode[] = [];
                treeNode = this.getTreeNode(treeData);
                return treeNode;
            })
        );
    }

    private getTreeNode(arr: any): FDMNode[] {
        const treeNode: FDMNode[] = [];
        arr.forEach((item: any) => {
            const label = item.SilkName;
            const componentID = Number(item.ComponentID);
            const eventReports = item.EventReports;
            const deviceInfo = item.DeviceInfo;
            const onlineTime = displayDateTime(item.OnlineTimeStamp, item.OnlineTimeZone);
            const node = new FDMNode(
                label,
                componentID,
                eventReports,
                deviceInfo,
                onlineTime
            );
            if (item.AssociateDevice && item.AssociateDevice.length > 0) {
                node.setChildren = this.getTreeNode(item.AssociateDevice);
            }
            treeNode.push(node);
        });
        if (treeNode[0]) {
            treeNode[0].checked = true;
        }
        return treeNode;
    }

    /**
     * 发送请求
     * @param url 请求链接
     */
    private getData(url: string) {
        return this.http.get(url).pipe(
            map(
                (data: any) => {
                    if (data.body && data.body.DeviceTree) {
                        return [data.body.DeviceTree];
                    }
                }
            )
        );
    }

}
