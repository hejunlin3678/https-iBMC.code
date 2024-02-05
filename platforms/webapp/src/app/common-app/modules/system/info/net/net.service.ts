import { Injectable } from '@angular/core';
import { HttpService } from 'src/app/common-app/service';
import { map } from 'rxjs/internal/operators/map';
import { NetTree } from './classes/net-tree';
import { NIC } from './classes/nic';
import { NetCardArr } from './classes/net-card/net-card-arr';
import { FcCardArr } from './classes/fc-card/fc-card-arr';
import { TeamArr } from './classes/team-card/team-card-arr';
import { BridgeArr } from './classes/bridge-card/bridge-card-arr';

@Injectable({
    providedIn: 'root'
})
export class NetService {

    public netTree: NetTree;
    public activeNum: number;
    public activeIndex: number;

    constructor(
        private http: HttpService,
    ) { }

    public factory() {
        const url = '/UI/Rest/System/NetworkAdapter';
        return this.getData(url).pipe(
            map(([body]) => {
                const tree = new NetTree();
                const cards = body;

                for (const key in cards) {
                    if (cards.hasOwnProperty(key) && cards[key].length > 0) {
                        const cardArr: NIC[] = [];
                        for (const [idx, item] of cards[key].entries()) {
                            cardArr.push(new NIC(item.CardName || item.Name, key + '_' +  idx, item));
                        }
                        switch (key) {
                            case 'NetCard':
                                tree.addAbstractNIC(new NetCardArr(cardArr));
                                break;
                            case 'FCCard':
                                tree.addAbstractNIC(new FcCardArr(cardArr));
                                break;
                            case 'Team':
                                tree.addAbstractNIC(new TeamArr(cardArr));
                                break;
                            case 'Bridge':
                                tree.addAbstractNIC(new BridgeArr(cardArr));
                                break;
                            default:
                                break;
                        }
                    }
                }

                return tree;
            })
        );
    }

    /**
     * 发送请求
     * @param url 请求链接
     */
    private getData(url: string) {
        return this.http.get(url).pipe(
            map(
                (data: any) => {
                    if (!(data.body)) {
                        data.body = {};
                    }
                    return [data.body];
                }
            )
        );
    }
}
