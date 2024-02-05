import { Component, OnInit, ViewChild } from '@angular/core';
import { IPTypes } from './model';
import { Ipv4NetComponent } from './components/ipv4-net/ipv4-net.component';
import { Ipv6NetComponent } from './components/ipv6-net/ipv6-net.component';
import { LoadingService } from 'src/app/common-app/service/loading.service';
import { NetworkService } from './network.service';
import { GlobalDataService } from 'src/app/common-app/service';
import { forkJoin } from 'rxjs';

@Component({
    selector: 'app-network',
    templateUrl: './network.component.html',
    styleUrls: ['./network.component.scss']
})
export class NetworkComponent implements OnInit {
    @ViewChild(Ipv4NetComponent) ipv4Net: Ipv4NetComponent;
    @ViewChild(Ipv6NetComponent) ipv6Net: Ipv6NetComponent;

    public isSystemLock: boolean = false;
    public slotList;
    public bmcNetTabs: any = {
        IPv4: {
            title: IPTypes.IPV4,
            id: IPTypes.IPV4
        },
        IPv6: {
            title: IPTypes.IPV6,
            id: IPTypes.IPV6
        }
    };
    public managerId = '';
    constructor(
        private loading: LoadingService,
        private bmcNetService: NetworkService,
        private global: GlobalDataService
    ) { }

    ngOnInit(): void {
        this.loading.state.next(false);
        forkJoin([
            this.bmcNetService.getOverViewInfo(),
            this.bmcNetService.getHMM1MainBoard()
        ]).subscribe({
            next: (resArr) => {
                this.slotList = resArr[0];
                if (resArr[1].Status.State === 'Enabled') {
                    this.managerId = 'HMM1';
                } else {
                    this.managerId = 'HMM2';
                }
                this.global.managerId = this.managerId;
            },
            error: () => {

            }
        });
    }
}
