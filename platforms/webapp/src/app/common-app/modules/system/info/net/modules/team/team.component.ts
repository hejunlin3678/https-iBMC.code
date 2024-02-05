import { Component, OnInit } from '@angular/core';
import { TeamService } from './team.service';
import { NetService } from '../../net.service';
import { TiTableRowData, TiTableSrcData } from '@cloud/tiny3';
import { TeamCard } from '../../classes/team-card/team-card';
import { LoadingService } from 'src/app/common-app/service';

@Component({
  selector: 'app-team',
  templateUrl: './team.component.html',
  styleUrls: ['./team.component.scss']
})
export class TeamComponent implements OnInit {

    public activeNum: number;
    public activeIndex: number;
    public teamCard: TeamCard;
    public srcData: TiTableSrcData;
    public columns;
    public data: TiTableRowData[] = [];
    constructor(
        private netService: NetService,
        private fcCardService: TeamService,
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

        this.teamCard = this.netService.netTree.getCardArray[this.activeIndex].cards[this.activeNum] as TeamCard;
        if (!this.teamCard.getInit) {
            this.loading.state.next(true);
            this.fcCardService.factory(this.teamCard.getMoreInfo, this.teamCard.getName).subscribe(
                (card) => {
                    this.loading.state.next(false);
                    this.teamCard = card;
                    this.columns = this.teamCard.ports ? this.teamCard.ports.titles : [];
                    this.srcData.data = this.teamCard.ports ? this.teamCard.ports.netPorts :  [];
                    this.netService.netTree.getCardArray[this.activeIndex].cards[this.activeNum] = card;
                }, () => {
                    this.loading.state.next(false);
                }
            );
        } else {
            this.columns = this.teamCard.ports ? this.teamCard.ports.titles : [];
            this.srcData.data = this.teamCard.ports ? this.teamCard.ports.netPorts :  [];
        }
    }
}
