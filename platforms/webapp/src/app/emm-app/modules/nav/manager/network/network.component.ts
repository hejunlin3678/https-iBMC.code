import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { NetworkService } from './services';
import { GlobalDataService, LoadingService, UserInfoService } from 'src/app/common-app/service';
import { NetworkDns, NetworkProtocol, NetworkLldp } from './model';

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
        private global: GlobalDataService
    ) {}

    private i18n: any = {};
    public networkeData: any = {};
    public networkeDetail: any = {};
    public tempUrl = '';
    public productType: boolean = true;
    public isSystemLock = this.user.systemLockDownEnabled;
    // ip地址是否在编辑状态
    public protocolBeingEdited: boolean = false;
    public protocolEditDisabled: boolean = false;
    // dns是否在编辑状态
    public dnsBeingEdited: boolean = false;
    public dnsEditDisabled: boolean = false;
    // lldp是否在编辑状态
    public lldpBeingEdited: boolean = false;
    public lldpEditDisabled: boolean = false;
    public HMMID: string;
    public statusSymbol = {
        networkProtocol: Symbol(),
        networkDns: Symbol(),
        networkLldp: Symbol()
    };


    public networkProtocol = new NetworkProtocol();
    public networkDns = new NetworkDns();
    public networkLldp = new NetworkLldp();
    public networkGetInit = () => {
        this.networkService.networkList().subscribe((res) => {
            if (res) {
                this.networkProtocol = res[0];
            }
            this.loadingService.state.next(false);
        }, () => {
            this.loadingService.state.next(false);
        });
    }
    public getHostName() {
        // 获取hostname
        this.networkService.getHMM().subscribe((response) => {
            if (response?.body?.Status?.State === 'Enabled') {
                this.HMMID = 'HMM1';
            } else {
                this.HMMID = 'HMM2';
            }
            this.global.managerId = this.HMMID;
            this.getLldp();
            this.getDns();
            this.networkGetInit();
        }, (error) => {
            if (error.status === 403 || error.status === 404) {
                this.HMMID = 'HMM2';
            }
            this.global.managerId = this.HMMID;
            this.getLldp();
            this.getDns();
            this.networkGetInit();
        });
    }
    public getLldp() {
        this.networkService.getLldp(this.HMMID).subscribe((res) => {
            this.networkLldp = res;
        });
    }
    public getDns() {
        this.networkService.getDns(this.HMMID).subscribe((res) => {
            this.networkDns = res;
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
        this.getHostName();
    }
    public statusChange(status: boolean, type: symbol) {
        switch (type) {
            case this.statusSymbol.networkProtocol:
                this.protocolBeingEdited = status;
                break;
            case this.statusSymbol.networkDns:
                this.dnsBeingEdited = status;
                break;
            case this.statusSymbol.networkLldp:
                this.lldpBeingEdited = status;
                break;
        }
        this.protocolEditDisabled = status;
        this.dnsEditDisabled = status;
        this.lldpEditDisabled = status;
    }
}
