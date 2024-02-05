import { Component, OnInit } from '@angular/core';
import { TiTableSrcData, TiTableRowData } from '@cloud/tiny3';
import { BridgeCard } from '../../classes/bridge-card/bridge-card';
import { NetService } from '../../net.service';
import { BridgeService } from './bridge.service';
import { LoadingService } from 'src/app/common-app/service';

@Component({
    selector: 'app-bridge',
    templateUrl: './bridge.component.html',
    styleUrls: ['./bridge.component.scss']
})
export class BridgeComponent implements OnInit {

    public activeNum: number;
    public activeIndex: number;
    public bridgeCard: BridgeCard;
    public srcData: TiTableSrcData;
    public columns;
    public data: TiTableRowData[] = [];
    constructor(
        private netService: NetService,
        private service: BridgeService,
        private loading: LoadingService
    ) { }

    ngOnInit() {
        this.srcData = {
            data: this.data,
            state: {
                searched: false,
                sorted: false,
                paginated: false
            }
        };
        this.activeNum = this.netService.activeNum;
        this.activeIndex = this.netService.activeIndex;

        this.bridgeCard = this.netService.netTree.getCardArray[this.activeIndex].cards[this.activeNum] as BridgeCard;
        if (!this.bridgeCard.getInit) {
            this.loading.state.next(true);
            this.service.factory(this.bridgeCard.getMoreInfo, this.bridgeCard.getName).subscribe(
                (card) => {
                    this.loading.state.next(false);
                    this.bridgeCard = card;
                    this.columns = this.bridgeCard.ports ? this.bridgeCard.ports.titles : [];
                    this.srcData.data = this.bridgeCard.ports ? this.bridgeCard.ports.netPorts : [];
                    this.netService.netTree.getCardArray[this.activeIndex].cards[this.activeNum] = card;
                }, () => {
                    this.loading.state.next(false);
                }
            );
        } else {
            this.columns = this.bridgeCard.ports ? this.bridgeCard.ports.titles : [];
            this.srcData.data = this.bridgeCard.ports ? this.bridgeCard.ports.netPorts : [];
        }
    }

}
