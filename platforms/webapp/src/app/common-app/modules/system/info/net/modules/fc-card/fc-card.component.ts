import { Component, OnInit } from '@angular/core';
import { FcCardService } from './fc-card.service';
import { NetService } from '../../net.service';
import { FcCard } from '../../classes/fc-card/fc-card';
import { TiTableSrcData, TiTableRowData } from '@cloud/tiny3';
import { LoadingService } from 'src/app/common-app/service';
import { NetCardService } from '../net-card/net-card.service';

@Component({
    selector: 'app-fc-card',
    templateUrl: './fc-card.component.html',
    styleUrls: ['./fc-card.component.scss']
})
export class FcCardComponent implements OnInit {
    public activeNum: number;
    public activeIndex: number;
    public fcCard: FcCard;
    public srcData: TiTableSrcData;
    public columns;
    public data: TiTableRowData[] = [];
    public type: string = 'fc';
    constructor(
        private netService: NetService,
        private fcCardService: FcCardService,
        private loading: LoadingService,
        private netCardService: NetCardService,
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

        this.fcCard = this.netService.netTree.getCardArray[this.activeIndex].cards[this.activeNum] as FcCard;

        if (!this.fcCard.getInit) {
            this.loading.state.next(true);
            this.fcCardService.factory(this.fcCard.getMoreInfo, this.fcCard.getName).subscribe(
                (card) => {
                    this.loading.state.next(false);
                    this.fcCard = card;
                    this.columns = this.fcCard.ports ? this.fcCard.ports.titles : [];
                    this.srcData.data = this.fcCard.ports ? this.fcCard.ports.netPorts : [];
                    this.netService.netTree.getCardArray[this.activeIndex].cards[this.activeNum] = card;
                }, () => {
                    this.loading.state.next(false);
                }
            );
        } else {
            this.columns = this.fcCard.ports ? this.fcCard.ports.titles : [];
            this.srcData.data = this.fcCard.ports ? this.fcCard.ports.netPorts : [];
        }
    }

    public sliderClick(i: number) {
        let currentData = null;
        currentData = this.fcCard.getPort.netPorts[i];
        if (!currentData.extendAttr.init) {
            this.netCardService.extendAttrFactory(currentData, 'Gbps').subscribe((value) => {
                currentData.extendAttr = value;
            });
        }
    }
}
