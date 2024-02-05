import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { NetworkService } from './services';
import { LoadingService, UserInfoService } from 'src/app/common-app/service';
import { NetworkPort, NetworkLldp, NetworkDns, NetworkVlan, NetworkProtocol } from './model';

@Component({
    selector: 'app-network',
    templateUrl: './network.component.html',
    styleUrls: ['../manager.component.scss', './network.component.scss']
})
export class NetworkComponent implements OnInit {

    constructor(
        private translate: TranslateService,
        private user: UserInfoService,
        private networkService: NetworkService,
        private loadingService: LoadingService,
    ) {}

    private i18n: any = {};
    public networkeData: any = {};
    public networkeDetail: any = {};
    public lldpResData: any = {};
    public lldpShowData: any = {};
    public lldpSupport: boolean = false;
    public tempUrl = '';
    public productType: boolean = true;
    public isSystemLock = this.user.systemLockDownEnabled;

    public networkPort = new NetworkPort();
    public networkProtocol = new NetworkProtocol();
    public networkLldp = new NetworkLldp();
    public networkDns = new NetworkDns();
    public networkVlan = new NetworkVlan();
    public networkGetInit = () => {
        this.networkService.networkList().subscribe((res) => {
            if (res) {
                this.networkPort = res[0];
                this.networkProtocol = res[1];
                this.networkLldp = res[2];
                this.networkDns = res[3];
                this.networkVlan = res[4];
            }
            this.loadingService.state.next(false);
        }, () => {
            this.loadingService.state.next(false);
        });
    }
    // 系统锁定状态变化
    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }
    ngOnInit(): void {
        this.loadingService.state.next(true);
        // 监控语言切换变化
        this.translate.onLangChange.subscribe((res) => {
			for (const key in this.i18n) {
				if (Object.prototype.hasOwnProperty.call(this.i18n, key)) {
					this.i18n[key] = res.translations[key];
				}
			}
        });
        this.networkGetInit();
    }

}
